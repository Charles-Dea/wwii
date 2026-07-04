#ifdef __linux__
#include<sys/time.h>
#include<unistd.h>
#else
#include<windows.h>
#endif
#include<malloc.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"flag.h"
#include"grphcs.h"
#include"menu.h"
#include"net.h"
#include"unit.h"
#include"win.h"
#define SBS 8
double scrnwdth,scrnhght;
uint16_t neweid=1;
bool running=1;
uint8_t reqcptr;
arrlst_t poses={
	.bs=SBS*sizeof(pos_t),
	.es=sizeof(pos_t),
};
arrlst_t dims={
	.bs=SBS*sizeof(dim_t),
	.es=sizeof(dim_t),
};
arrlst_t texes={
	.bs=SBS*sizeof(tex_t),
	.es=sizeof(tex_t),
};
arrlst_t clkbls={
	.bs=SBS*sizeof(clkbl_t),
	.es=sizeof(clkbl_t),
};
arrlst_t rclkbls={
	.bs=SBS*sizeof(rclkbl_t),
	.es=sizeof(rclkbl_t),
};
arrlst_t udatas={
	.bs=SBS*sizeof(udata_t),
	.es=sizeof(udata_t),
};
arrlst_t relposes={
	.bs=SBS*sizeof(relpos_t),
	.es=sizeof(relpos_t),
};
arrlst_t rings={
	.bs=SBS*sizeof(ring_t),
	.es=sizeof(ring_t),
};
arrlst_t cols={
	.bs=SBS*sizeof(col_t),
	.es=sizeof(col_t),
};
arrlst_t arrows={
	.bs=SBS*sizeof(arrow_t),
	.es=sizeof(arrow_t),
};
arrlst_t rects={
	.bs=SBS*sizeof(rect_t),
	.es=sizeof(rect_t),
};
arrlst_t txtboxes={
	.bs=SBS*sizeof(txtbox_t),
	.es=sizeof(txtbox_t),
};
arrlst_t flags={
	.bs=SBS*sizeof(flag_t),
	.es=sizeof(flag_t),
};
arrlst_t lines={
	.bs=SBS*sizeof(line_t),
	.es=sizeof(line_t),
};
arrlst_t hedges={
	.bs=SBS*sizeof(hedge_t),
	.es=sizeof(hedge_t),
};
arrlst_t scrnposes={
	.bs=SBS*sizeof(scrnpos_t),
	.es=sizeof(scrnpos_t),
};
static arrlst_t*const comps[]={
	&poses,
	&dims,
	&texes,
	&clkbls,
	&rclkbls,
	&udatas,
	&relposes,
	&rings,
	&cols,
	&arrows,
	&rects,
	&txtboxes,
	&flags,
	&lines,
	&hedges,
	&scrnposes,
};
static int64_t getutime(void);
#ifndef __linux__
static void usleep(int64_t);
#endif
int main(){
	srand(time(NULL));
	srand(rand());
	int8_t err;
	GLFWwindow*const __restrict win=win_open(&err);
	if(!win){
		return err;
	}
	grphcs_init();
	for(arrlst_t*const*i=comps,*const*const __restrict end=i+sizeof(comps)/8;i<end;i++){
		arrlst_t*const a=*i;
		a->buf=malloc(a->bs);
	}
	menu_main();
	net_init();
	unit_chklos();
	while(running&&!glfwWindowShouldClose(win)){
		const int64_t et=getutime()+100000/12;
		glfwPollEvents();
		net_lstn();
		grphcs_mvcam(win);
		if(menu_scene==SCENE_GAME){
			flag_update();
			if(flag_cptrd()>=reqcptr){
				const bool wnrald=unit_allied;
				menu_endscrn(wnrald);
				net_dscnct(wnrald);
			}
		}
		grphcs_draw(win);
		const int64_t st=et-getutime();
		if(st>0){
			usleep(st);
		}
	}
	termscene();
	grphcs_term();
	win_close(win);
}
posn_t getposn(const uint16_t eid,int8_t*const __restrict err){
	const pos_t*const pos=getent(&poses,eid);
	if(pos){
		const posn_t posn={
			.x=pos->x,
			.y=pos->y,
			.z=pos->z,
		};
		*err=E_SUCC;
		return posn;
	}
	const relpos_t*const relpos=getent(&relposes,eid);
	if(!relpos){
		*err=E_NO_ENT;
		posn_t posn={};
		return posn;
	}
	const posn_t prnt=getposn(relpos->prnt,err);
	if(*err){
		posn_t posn={};
		return posn;
	}
	const posn_t posn={
		.x=prnt.x+relpos->x,
		.y=prnt.y+relpos->y,
		.z=prnt.z+relpos->z,
	};
	return posn;
}
void termscene(){
	for(uint16_t i=neweid;i;i--){
		delent(i);
	}
	for(arrlst_t*const*i=comps,*const*const __restrict end=comps+sizeof(comps)/8;i<end;i++)
	{
		arrlst_t*const a=*i;
		free(a->buf);
		const uint64_t nbs=SBS*a->es;
		a->buf=malloc(nbs);
		a->bs=nbs;
		a->nels=0;
	}
	neweid=1;
	win_clkoff=NULL;
	win_rclk=NULL;
	win_seltxtbox=0;
	unit_sel=0;
	grphcs_camx=0;
	grphcs_camy=0;
	grphcs_zoom=1;
	menu_nxtrn=0;
}
void delent(const uint16_t eid){
	arrlst_t*const*i=comps;
	arrlst_t*const*const __restrict end=i+sizeof(comps)/8;
	for(;i<end;i++){
		arrlst_t*const a=*i;
		const uint64_t idx=getenti(a,eid);
		if(idx!=UINT64_MAX){
			const void*const comp=a->buf+idx*a->es;
			void(*const del)(const void*)=((void**)comp)[1];
			if(del){
				del(comp);
			}
			arrlst_del(a,idx);
		}
	}
}
void*getent(const arrlst_t*const arr,const uint16_t eid){
	const void*ents=arr->buf;
	const uint64_t es=arr->es;
	const uint64_t nels=arr->nels;
	for(int64_t low=0,high=nels-1;low<=high&&~low&0x8000000000000000&&high<nels;){
		const uint64_t mid=(low+high)/2;
		const uint16_t*const ent=ents+mid*es;
		const int16_t diff=*ent-eid;
		if(!diff){
			return ent;
		}
		if(diff&0x8000){
			low=mid+1;
		}else{
			high=mid-1;
		}
	}
	return NULL;
}
uint64_t getenti(const arrlst_t*const arr,const uint16_t eid){
	const void*ents=arr->buf;
	const uint64_t es=arr->es;
	const uint64_t nels=arr->nels;
	for(int64_t low=0,high=nels;low<=high&&~low&0x8000000000000000&&high<=nels;){
		const uint64_t mid=(low+high)/2;
		const uint16_t*const ent=ents+mid*es;
		const int16_t diff=*ent-eid;
		if(!diff){
			return mid;
		}
		if(diff&0x8000){
			low=mid+1;
		}else{
			high=mid-1;
		}
	}
	return UINT64_MAX;
}
#ifdef __linux__
static int64_t getutime(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec*1000000+tv.tv_usec;
}
#else
static int64_t getutime(){
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	return(((uint64_t)ft.dwHighDateTime<<32)|ft.dwLowDateTime)/10;
}
static void usleep(const int64_t u){
	HANDLE ht=CreateWaitableTimer(NULL,1,NULL);
	LARGE_INTEGER li;
	li.QuadPart=-(u*10);
	SetWaitableTimer(ht,&li,0,NULL,NULL,0);
	WaitForSingleObject(ht,INFINITE);
	CloseHandle(ht);
}
#endif
