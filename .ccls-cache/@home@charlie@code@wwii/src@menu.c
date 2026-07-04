#ifndef __linux__
#include<windows.h>
#endif
#include<GL/gl.h>
#include<malloc.h>
#include<stdio.h>
#include<string.h>
#include"grphcs.h"
#include"flag.h"
#include"net.h"
#include"unit.h"
#include"win.h"
#include"menu.h"
uint8_t menu_scene=SCENE_MAINMEN;
uint16_t menu_nxtrn;
static int8_t chsd();
static int8_t enterip();
static void deltxtbox(const txtbox_t*);
static int8_t seltxtbox(uint16_t);
static int8_t desltxtbox(uint16_t);
static void mkbutton(float,float,float,float,float,uint64_t,int8_t(*)(int64_t),int64_t);
static int8_t waiting(bool);
static void mkhedge(float,float,float,float,float);
static void mksprt(float,float,float,float,uint64_t);
void menu_main(){
	termscene();
	mkbutton(0,.5,0,2,.5,TEX_HOST,(int8_t(*)(int64_t))chsd,0);
	mkbutton(0,-.5,0,2,.5,TEX_JOIN,(int8_t(*)(int64_t))enterip,0);
	glClearColor(0,0,0,1);
	menu_scene=SCENE_MAINMEN;
}
void menu_strtgm(){
	termscene();
	for(int8_t ald=0;ald<2;ald++){
		const float neg=(ald<<1)-1;
		for(float y=2.5;y<=3;y+=.5){
			const float ay=y*neg;
			for(float x=-1.75;x<=1.75;x+=.5){
				mkunit(x,ay,ald);
			}
		}
	}
	mkhedge(-2.3,-2,.5,-1.84,.05);
	mkhedge(.4,-1.75,3,-1.8,.06);
	mkhedge(-4,-4,-3,-1,.08);
	mkhedge(3.2,-1.74,4,-4,.04);
	mkhedge(-3,-3.5,3,-3.5,.05);
	mkhedge(-2.5,-1.5,2.4,-1.4,.07);
	mkhedge(-4.33,-1,-.32,-0.8,.054);
	mkhedge(0,-.6,5,-1.2,.065);
	mkhedge(-2,0,2,0,.05);
	mkhedge(-5,1,-3,.5,.05);
	mkhedge(-4,1.5,2,2,.09);
	mkhedge(-3,1,2,1,.05);
	mkhedge(0,.5,5,.5,.07);
	mkhedge(0,1.5,5,1.7,.065);
	mkflag(0,2.75,1.5,1);
	mkflag(0,-2.75,1.5,0);
	const uint16_t nxtrn=neweid;
	const scrnpos_t pos={
		.eid=nxtrn,
		.x=-.1,
		.y=.1,
		.xancr=XANCR_RGHT,
		.yancr=YANCR_BOT,
	};
	arrlst_add(&scrnposes,&pos);
	const dim_t dim={
		.eid=nxtrn,
		.w=.2,
		.h=.2,
	};
	arrlst_add(&dims,&dim);
	const tex_t tex={
		.eid=nxtrn,
		.tex=unit_pltrn?TEX_NXTRN:TEX_OPTRN,
	};
	arrlst_add(&texes,&tex);
	const clkbl_t clkbl={
		.eid=nxtrn,
		.func=(int8_t(*)(int64_t))unit_nxtrn,
		.param=unit_allied,
	};
	arrlst_add(&clkbls,&clkbl);
	neweid=nxtrn+1;
	menu_nxtrn=nxtrn;
	glClearColor(0,.5,0,1);
	reqcptr=2;
	menu_scene=SCENE_GAME;
	grphcs_camy=(double)((unit_allied<<1)-1)*2.5;
	unit_chklos();
}
void menu_endscrn(const bool ald){
	termscene();
	uint64_t adjtex,ftex;
	float fw;
	if(ald){
		adjtex=TEX_AMRCN;
		ftex=TEX_USFLAG;
		fw=2.0/3*1.9;
	}else{
		adjtex=TEX_GRMN;
		ftex=TEX_GERMFLAG;
		fw=2.0/3*5/3;
	}
	mksprt(0,2.0/3,2.0/3*4,2.0/3,adjtex);
	mksprt(0,0,fw,2.0/3,ftex);
	mksprt(0,-2.0/3,2.0/3*4,2.0/3,TEX_VCTRY);
	glClearColor(0,0,0,1);
	menu_scene=SCENE_ENDSCRN;
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
static void mkbutton(
		const float x,
		const float y,
		const float z,
		const float w,
		const float h,
		const uint64_t t,
		int8_t(*const func)(int64_t),
		const int64_t param
		){
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
	reqcptr=allied?1:2;
	termscene();
	mksprt(0,0,2,.5,TEX_WTNG);
	/*const pos_t pos={
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
	arrlst_add(&texes,&tex);*/
	menu_scene=SCENE_WTNG;
	unit_pltrn=1;
	return E_SUCC;
}
static void
mkhedge(const float x0,const float y0,const float x1,const float y1,const float w){
	const uint16_t eid=neweid;
	const hedge_t hedge={
		.eid=eid,
	};
	arrlst_add(&hedges,&hedge);
	const line_t line={
		.eid=eid,
		.x0=x0,
		.y0=y0,
		.z0=1,
		.x1=x1,
		.y1=y1,
		.z1=1,
		.width=w,
	};
	arrlst_add(&lines,&line);
	const col_t col={
		.eid=eid,
		.g=.3,
		.a=1,
	};
	arrlst_add(&cols,&col);
	neweid=eid+1;
}
static void
mksprt(const float x,const float y,const float w,const float h,const uint64_t t){
	const uint16_t eid=neweid;
	const pos_t pos={
		.eid=eid,
		.x=x,
		.y=y,
	};
	arrlst_add(&poses,&pos);
	const dim_t dim={
		.eid=eid,
		.w=w,
		.h=h,
	};
	arrlst_add(&dims,&dim);
	const tex_t tex={
		.eid=eid,
		.tex=t,
	};
	arrlst_add(&texes,&tex);
	neweid=eid+1;
}
