#include<GL/gl.h>
#include<malloc.h>
#include<string.h>
#include"grphcs.h"
#include"main.h"
#include"net.h"
#include"unit.h"
#include"win.h"
#include"menu.h"
uint8_t menu_scene=SCENE_MAINMEN;
static int8_t chsd();
static int8_t enterip();
static void deltxtbox(const txtbox_t*);
static int8_t seltxtbox(uint16_t);
static int8_t desltxtbox(uint16_t);
static void mkbutton(float,float,float,float,float,uint64_t,int8_t(*)(int64_t),int64_t);
static int8_t waiting(bool);
void menu_main(){
	termscene();
	mkbutton(0,.5,0,2,.5,TEX_HOST,(int8_t(*)(int64_t))chsd,0);
	mkbutton(0,-.5,0,2,.5,TEX_JOIN,(int8_t(*)(int64_t))enterip,0);
	glClearColor(0,0,0,1);
	menu_scene=SCENE_MAINMEN;
}
void menu_strtgm(){
	termscene();
	mkunit(-.5,.5,0);
	mkunit(.5,.5,0);
	mkunit(-.5,-.5,1);
	mkunit(.5,-.5,1);
	glClearColor(0,.5,0,1);
	menu_scene=SCENE_GAME;
}
static int8_t chsd(){
	termscene();
	const float sr=scrnwdth/scrnhght;
	mkbutton(-.5*sr,0,0,.95,.5,TEX_USFLAG,(int8_t(*)(int64_t))waiting,1);
	mkbutton(.5*sr,0,0,5.0/6,.5,TEX_GERMFLAG,(int8_t(*)(int64_t))waiting,0);
	glClearColor(0,0,0,1);
	menu_scene=SCENE_CHSD;
	return E_SUCC;
}
static int8_t enterip(){
	termscene();
	const pos_t pos={
		.eid=1,
		.z=1,
	};
	arrlst_add(&poses,&pos);
	const dim_t dim={
		.eid=1,
		.w=2,
		.h=.25,
	};
	arrlst_add(&dims,&dim);
	const col_t col={
		.eid=1,
		.r=1,
		.g=1,
		.b=1,
		.a=1,
	};
	arrlst_add(&cols,&col);
	const rect_t rect={
		.eid=1,
	};
	arrlst_add(&rects,&rect);
	const txtbox_t txtbox={
		.eid=1,
		.crsr=2,
		.free=deltxtbox,
		.str={
			.buf=malloc(16),
			.bs=16,
			.es=1
		},
		.chrs={
			.buf=malloc(32),
			.bs=32,
			.es=2
		},
		.acs=malloc(128),
		.func=net_join,
		.ml=15,
	};
	memset(txtbox.acs,0,128);
	const bool*const end=txtbox.acs+'9'+1;
	for(bool*i=txtbox.acs+'0';i<=end;i++){
		*i=1;
	}
	txtbox.acs['.']=1;
	arrlst_add(&txtboxes,&txtbox);
	const clkbl_t clkbl={
		.eid=1,
		.func=(int8_t(*)(int64_t))seltxtbox,
		.param=1,
	};
	arrlst_add(&clkbls,&clkbl);
	const relpos_t rp={
		.eid=2,
		.prnt=1,
		.x=-.9375,
		.z=-1,
	};
	arrlst_add(&relposes,&rp);
	const dim_t cd={
		.eid=2,
		.w=.03125,
		.h=.24,
	};
	arrlst_add(&dims,&cd);
	const col_t cc={
		.eid=2,
	};
	arrlst_add(&cols,&cc);
	const rect_t cr={
		.eid=2,
	};
	arrlst_add(&rects,&cr);
	menu_scene=SCENE_INPTIP;
	neweid=3;
	unit_pltrn=0;
	return E_SUCC;
}
static void deltxtbox(const txtbox_t*const tb){
	free(tb->str.buf);
	const uint16_t*const chrs=tb->chrs.buf;
	for(const uint16_t*i=chrs,*const end=i+tb->chrs.nels;i<end;i++){
		delent(*i);
	}
	free(chrs);
	free(tb->acs);
}
static int8_t seltxtbox(const uint16_t eid){
	win_seltxtbox=eid;
	win_clkoff=(int8_t(*)(int64_t,float,float))desltxtbox;
	win_clkoffparam=eid;
	const txtbox_t*const t=getent(&txtboxes,eid);
	if(t){
		col_t*const c=getent(&cols,t->crsr);
		if(c){
			c->a=1;
		}else{
			fprintf(stderr,"WARNING: entity %hu has no color\n",t->crsr);
		}
	}else{
		fprintf(stderr,"ERROR: entity %hu has no txtbox\n",eid);
	}
	return E_SUCC;
}
static int8_t desltxtbox(const uint16_t eid){
	win_seltxtbox=0;
	win_clkoff=NULL;
	const txtbox_t*const t=getent(&txtboxes,eid);
	if(!t){
		fprintf(stderr,"ERROR: entity %hu has no txtbox\n",eid);
		return E_NO_ENT;
	}
	col_t*const c=getent(&cols,t->crsr);
	if(c){
		c->a=0;
	}else{
		fprintf(stderr,"WARNING: entity %hu has no color\n",t->crsr);
	}
	return E_SUCC;
}
static void
mkbutton(const float x,const float y,const float z,const float w,const float h,const uint64_t t,int8_t(*const func)(int64_t),const int64_t param){
	const uint16_t eid=neweid;
	const pos_t pos={
		.eid=eid,
		.x=x,
		.y=y,
		.z=z,
	};
	arrlst_add(&poses,&pos);
	const dim_t dim={
		.eid=eid,
		.w=w,
		.h=h,
	};
	arrlst_add(&dims,&dim);
	const clkbl_t clkbl={
		.eid=eid,
		.func=func,
		.param=param,
	};
	arrlst_add(&clkbls,&clkbl);
	const tex_t tex={
		.eid=eid,
		.tex=t,
	};
	arrlst_add(&texes,&tex);
	neweid=eid+1;
}
static int8_t waiting(const bool allied){
	const int8_t err=net_strthst();
	if(err){
		fprintf(stderr,"ERROR: net_strthst failed with error code %hhu\n",err);
		menu_main();
		return err;
	}
	unit_allied=allied;
	termscene();
	const pos_t pos={
		.eid=1,
	};
	arrlst_add(&poses,&pos);
	const dim_t dim={
		.eid=1,
		.w=2,
		.h=.5,
	};
	arrlst_add(&dims,&dim);
	const tex_t tex={
		.eid=1,
		.tex=TEX_WTNG,
	};
	arrlst_add(&texes,&tex);
	menu_scene=SCENE_WTNG;
	unit_pltrn=1;
	return E_SUCC;
}
