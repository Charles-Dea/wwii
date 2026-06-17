#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"grphcs.h"
#include"main.h"
#include"net.h"
#include"win.h"
#include"unit.h"
#define INFMVSPD .21867
#define INFICNSZ .1
enum{
	UFLAGS_ALLIED=0b1,
	UFLAGS_SEL=0b10,
	UFLAGS_ACTED=0b100,
};
uint16_t unit_sel;
bool unit_allied;
bool unit_pltrn;
static void delunit(const udata_t*);
static int8_t tglsel(uint16_t);
static int8_t selct(uint16_t);
static uint16_t mkwpn(wpn_t*__restrict,uint16_t*__restrict,uint16_t,uint16_t,float);
void mkunit(const float x,const float y,const bool allied){
	const uint16_t eid=neweid;
	const pos_t pos={
		.eid=eid,
		.x=x,
		.y=y,
	};
	arrlst_add(&poses,&pos);
	const dim_t dim={
		.eid=eid,
		.w=INFICNSZ,
		.h=INFICNSZ,
	};
	arrlst_add(&dims,&dim);
	const tex_t tex={
		.eid=eid,
		.tex=allied?TEX_ALLIEDINF:TEX_AXISINF,
	};
	arrlst_add(&texes,&tex);
	const clkbl_t clkbl={
		.eid=eid,
		.func=(int8_t(*)(int64_t))tglsel,
		.param=eid,
	};
	arrlst_add(&clkbls,&clkbl);
	const rclkbl_t rclkbl={
		.eid=eid,
		.func=(int8_t(*)(int64_t))unit_fire,
		.param=eid,
	};
	arrlst_add(&rclkbls,&rclkbl);
	const uint16_t arrow=eid+1;
	const relpos_t arp={
		.eid=arrow,
		.prnt=eid,
		.z=.5,
	};
	arrlst_add(&relposes,&arp);
	const col_t acol={
		.eid=arrow,
	};
	arrlst_add(&cols,&acol);
	const arrow_t arr={
		.eid=arrow,
	};
	arrlst_add(&arrows,&arr);
	uint16_t*rngs;
	uint64_t nrngs;
	wpn_t*wpns;
	uint64_t nwpns;
	uint16_t neid;
	if(allied){
		nwpns=2;
		nrngs=nwpns+1;
		rngs=malloc(nrngs*2);
		const uint16_t r0=arrow+1;
		rngs[0]=r0;
		const relpos_t r0rp={
			.eid=r0,
			.prnt=eid,
			.z=1,
		};
		arrlst_add(&relposes,&r0rp);
		const col_t r0c={
			.eid=r0,
			.b=1,
		};
		arrlst_add(&cols,&r0c);
		const ring_t r0r={
			.eid=r0,
			.r=INFMVSPD,
		};
		arrlst_add(&rings,&r0r);
		wpns=malloc(nwpns*sizeof(wpn_t));
		wpns[0].acc=.0025;
		wpns[0].rng=3;
		wpns[0].rpm=150;
		wpns[0].num=1;
		const uint16_t w1=mkwpn(wpns,rngs+1,r0+1,eid,INFICNSZ/4);
		wpns[1].acc=.005;
		wpns[1].rng=1;
		wpns[1].rpm=30;
		wpns[1].num=11;
		neid=mkwpn(wpns+1,rngs+2,w1,eid,-INFICNSZ/4);
	}else{
		nwpns=3;
		nrngs=nwpns+1;
		rngs=malloc(nrngs*2);
		const uint16_t r0=arrow+1;
		rngs[0]=r0;
		const relpos_t r0rp={
			.eid=r0,
			.prnt=eid,
			.z=1,
		};
		arrlst_add(&relposes,&r0rp);
		const col_t r0c={
			.eid=r0,
			.b=1,
		};
		arrlst_add(&cols,&r0c);
		const ring_t r0r={
			.eid=r0,
			.r=INFMVSPD,
		};
		arrlst_add(&rings,&r0r);
		wpns=malloc(3*sizeof(wpn_t));
		wpns[0].acc=.0015;
		wpns[0].rng=4;
		wpns[0].rpm=225;
		wpns[0].num=1;
		const uint16_t w1=mkwpn(wpns,rngs+1,r0+1,eid,INFICNSZ/3);
		wpns[1].acc=.0001;
		wpns[1].rng=.4;
		wpns[1].rpm=100;
		wpns[1].num=2;
		const uint16_t w2=mkwpn(wpns+1,rngs+2,w1,eid,0);
		wpns[2].acc=.0075;
		wpns[2].rng=1;
		wpns[2].rpm=18;
		wpns[2].num=7;
		neid=mkwpn(wpns+2,rngs+3,w2,eid,-INFICNSZ/3);
	}
	const udata_t udata={
		.eid=eid,
		.arr=arrow,
		.flags=allied,
		.free=delunit,
		.wpns=wpns,
		.nwpns=nwpns,
		.rings=rngs,
		.nrings=nrngs,
	};
	arrlst_add(&udatas,&udata);
	neweid=neid;
}
int8_t unit_move(const uint16_t eid,const float x,const float y){
	pos_t*const p=getent(&poses,eid);
	if(!p){
		fprintf(stderr,"ERROR: entity %hu has no position\n",eid);
		return E_NO_ENT;
	}
	const float dx=x-p->x;
	const float dy=y-p->y;
	if(sqrtf(dx*dx+dy*dy)>INFMVSPD){
		return E_SUCC;
	}
	p->x=x;
	p->y=y;
	const int8_t err=unit_deselct(eid);
	if(err){
		fprintf(stderr,"WARNING: failed to deselect entity %hu with error code %hhu\n",eid,err);
	}
	udata_t*const u=getent(&udatas,eid);
	if(!u){
		fprintf(stderr,"ERROR: entity %hu has no udata\n",eid);
		return E_NO_ENT;
	}
	const int8_t flags=u->flags|UFLAGS_ACTED;
	u->flags=flags;
	tex_t*const t=getent(&texes,eid);
	if(t){
		t->tex=flags&UFLAGS_ALLIED?TEX_ALLIEDINFACTD:TEX_AXISINFACTD;
	}else{
		fprintf(stderr,"WARNING: entity %hu has no texture\n",eid);
	}
	net_move(eid,x,y);
	return E_SUCC;
}
int8_t unit_fire(const uint16_t trgt){
	const uint16_t shtr=unit_sel;
	if(!shtr){
		return E_SUCC;
	}
	udata_t*const su=getent(&udatas,shtr);
	if(!su){
		fprintf(stderr,"ERROR: entity %hu has no udata\n",shtr);
		return E_NO_ENT;
	}
	const udata_t*const tu=getent(&udatas,trgt);
	if(!tu){
		fprintf(stderr,"ERROR: entity %hu has no udata\n",trgt);
		return E_NO_ENT;
	}
	const int8_t sflags=su->flags|UFLAGS_ACTED;
	if((sflags&UFLAGS_ALLIED)==(tu->flags&UFLAGS_ALLIED)){
		return E_SUCC;
	}
	su->flags=sflags;
	int8_t err;
	const posn_t tp=getposn(trgt,&err);
	const posn_t sp=getposn(shtr,&err);
	const float dx=tp.x-sp.x;
	const float dy=tp.y-sp.y;
	const float d=sqrtf(dx*dx+dy*dy);
	const wpn_t*wi=su->wpns;
	const wpn_t*const __restrict wend=wi+su->nwpns;
	bool fired=0;
	uint8_t hits=0;
	for(;wi<wend;wi++){
		const uint8_t num=wi->num;
		if(d>wi->rng||!num){
			continue;
		}
		fired=1;
		const uint32_t acc=wi->acc*UINT32_MAX;
		for(uint16_t _=wi->rpm*num;_;_--){
			if((uint32_t)rand()<=acc){
				hits++;
			}
		}
	}
	if(!fired){
		return E_SUCC;
	}
	uint8_t sldrs=0;
	wpn_t*const tw=tu->wpns;
	wpn_t*twi=tw;
	const uint64_t tnwpns=tu->nwpns;
	const wpn_t*const __restrict twend=twi+tnwpns;
	for(;twi<twend;twi++){
		sldrs+=twi->num;
	}
	if(hits>=sldrs){
		delent(trgt);
		net_dstr(trgt);
	}else{
		uint16_t*const rings=tu->rings;
		uint64_t nrings=tu->nrings;
		uint64_t dstrd=0;
		for(uint8_t _=hits;_;_--){
			const double r=(double)((uint32_t)rand())/UINT32_MAX;
			double adjhc=0;
			for(uint64_t i=0;i<tnwpns;i++){
				twi=tw+i;
				uint8_t num=twi->num;
				adjhc+=(double)num/sldrs;
				if(r<adjhc){
					if(!(--num)){
						const uint64_t adji=i-dstrd++;
						uint16_t*const r=rings+adji+1;
						delent(*r);
						memmove(r,r+1,--nrings-adji-1);
					}
					twi->num=num;
					break;
				}
			}
			sldrs--;
		}
		uint8_t nums[tnwpns];
		uint8_t*ni=nums;
		for(twi=tw;twi<twend;twi++,ni++){
			const uint8_t num=twi->num;
			*ni=num;
			tex_t*const t0=getent(&texes,twi->sprt0);
			if(t0){
				t0->tex=num%10+TEX_0;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no texture\n",twi->sprt0);
			}
			const uint16_t sprt1=twi->sprt1;
			if(sprt1){
				tex_t*const t1=getent(&texes,sprt1);
				if(t1){
					t1->tex=num/10+TEX_0;
				}else{
					fprintf(stderr,"WARNING: entity %hu has no texture\n",sprt1);
				}
			}
		}
		net_dmg(trgt,nums,tnwpns);
	}
	err=unit_deselct(shtr);
	if(err){
		fprintf(stderr,"ERROR: failed to deselect entity %hu with error code %hhd\n",shtr,err);
		return err;
	}
	tex_t*const t=getent(&texes,shtr);
	if(t){
		t->tex=sflags&UFLAGS_ALLIED?TEX_ALLIEDINFACTD:TEX_AXISINFACTD;
	}else{
		fprintf(stderr,"WARNING: entity %hu has no texture\n",shtr);
	}
	return E_SUCC;
}
int8_t unit_deselct(const uint16_t eid){
	tex_t*const t=getent(&texes,eid);
	if(!t){
		fprintf(stderr,"ERROR: entity %hu has no texture\n",eid);
		return E_NO_ENT;
	}
	udata_t*const u=getent(&udatas,eid);
	if(!u){
		fprintf(stderr,"ERROR entity %hu has no udata\n",eid);
		return E_NO_ENT;
	}
	const int8_t flags=u->flags&~UFLAGS_SEL;
	t->tex=flags&UFLAGS_ALLIED?TEX_ALLIEDINF:TEX_AXISINF;
	u->flags=flags;
	const uint16_t*i=u->rings;
	const uint16_t*const __restrict end=i+u->nrings;
	for(;i<end;i++){
		col_t*const col=getent(&cols,*i);
		if(col){
			col->a=0;
		}else{
			fprintf(stderr,"WARNING: entity %hu has no color\n",*i);
		}
	}
	col_t*const arrcol=getent(&cols,u->arr);
	if(arrcol){
		arrcol->a=0;
	}else{
		fprintf(stderr,"WARNING: entity %hu has no color\n",u->arr);
	}
	win_clkoff=NULL;
	win_rclk=NULL;
	win_inmode=INMODE_NORM;
	unit_sel=0;
	return E_SUCC;
}
void unit_rstactd(){
	udata_t*i=udatas.buf;
	const udata_t*const __restrict end=i+udatas.nels;
	for(;i<end;i++){
		const int8_t flags=i->flags&~UFLAGS_ACTED;
		i->flags=flags;
		tex_t*const t=getent(&texes,i->eid);
		if(t){
			t->tex=flags&UFLAGS_ALLIED?TEX_ALLIEDINF:TEX_AXISINF;
		}else{
			fprintf(stderr,"WARNING: entity %hu has no texture\n",i->eid);
		}
	}
}
static void delunit(const udata_t*const u){
	delent(u->arr);
	const wpn_t*const wpns=u->wpns;
	for(const wpn_t*i=wpns,*const __restrict end=i+u->nwpns;i<end;i++){
		const uint16_t sprt1=i->sprt1;
		if(sprt1){
			delent(sprt1);
		}
		delent(i->sprt0);
	}
	free(u->wpns);
	const uint16_t*const rngs=u->rings;
	for(const uint16_t*i=rngs,*const end=i+u->nrings;i<end;i++){
		delent(*i);
	}
	free(rngs);
}
static int8_t tglsel(const uint16_t eid){
	const udata_t*const u=getent(&udatas,eid);
	if(!u){
		fprintf(stderr,"ERROR: entity %hu has no udata\n",eid);
		return E_NO_ENT;
	}
	if(u->flags&UFLAGS_SEL){
		return unit_deselct(eid);
	}
	return selct(eid);
}
static int8_t selct(const uint16_t eid){
	if(!unit_pltrn){
		return E_SUCC;
	}
	udata_t*const u=getent(&udatas,eid);
	if(!u){
		fprintf(stderr,"ERROR entity %hu has no udata\n",eid);
		return E_NO_ENT;
	}
	const int8_t flags=u->flags|UFLAGS_SEL;
	if(flags&UFLAGS_ACTED||(flags&UFLAGS_ALLIED)!=unit_allied){
		return E_SUCC;
	}
	tex_t*const t=getent(&texes,eid);
	if(t){
		t->tex=flags&UFLAGS_ALLIED?TEX_ALLIEDINFSEL:TEX_AXISINFSEL;
	}else{
		fprintf(stderr,"WARNING: entity %hu has no texture\n",eid);
	}
	u->flags=flags;
	const uint16_t*i=u->rings;
	const uint16_t*const __restrict end=i+u->nrings;
	for(;i<end;i++){
		col_t*const col=getent(&cols,*i);
		if(col){
			col->a=1;
		}else{
			fprintf(stderr,"WARNING: entity %hu has no color\n",*i);
		}
	}
	if(unit_sel&&unit_sel!=eid){
		unit_deselct(unit_sel);
	}
	win_clkoff=(int8_t(*)(int64_t,float,float))unit_deselct;
	win_rclk=(int8_t(*)(int64_t,float,float))unit_move;
	win_clkoffparam=eid;
	win_rclkparam=eid;
	unit_sel=eid;
	return E_SUCC;
}
static uint16_t mkwpn(wpn_t*const __restrict w,uint16_t*const __restrict ri,uint16_t eid,const uint16_t unit,const float y){
	const relpos_t rp={
		.eid=eid,
		.prnt=unit,
		.x=0,
		.y=0,
		.z=1,
	};
	arrlst_add(&relposes,&rp);
	const col_t col={
		.eid=eid,
		.r=1,
	};
	arrlst_add(&cols,&col);
	const ring_t ring={
		.eid=eid,
		.r=w->rng,
	};
	arrlst_add(&rings,&ring);
	*ri=eid++;
	const uint8_t num=w->num;
	if(num>=10){
		const relpos_t rp1={
			.eid=eid,
			.prnt=unit,
			.x=INFICNSZ/2+INFICNSZ/12,
			.y=y,
		};
		arrlst_add(&relposes,&rp1);
		const dim_t d1={
			.eid=eid,
			.w=INFICNSZ/6,
			.h=INFICNSZ/3,
		};
		arrlst_add(&dims,&d1);
		const tex_t t1={
			.eid=eid,
			.tex=TEX_0+num/10,
		};
		arrlst_add(&texes,&t1);
		w->sprt1=eid;
		const relpos_t rp0={
			.eid=++eid,
			.prnt=unit,
			.x=INFICNSZ/2+INFICNSZ/12+INFICNSZ/6,
			.y=y,
		};
		arrlst_add(&relposes,&rp0);
		const dim_t d0={
			.eid=eid,
			.w=INFICNSZ/6,
			.h=INFICNSZ/3,
		};
		arrlst_add(&dims,&d0);
		const tex_t t0={
			.eid=eid,
			.tex=TEX_0+num%10,
		};
		arrlst_add(&texes,&t0);
	}else{
		w->sprt1=0;
		const relpos_t rp={
			.eid=eid,
			.prnt=unit,
			.x=INFICNSZ/2+INFICNSZ/12,
			.y=y,
		};
		arrlst_add(&relposes,&rp);
		const dim_t d={
			.eid=eid,
			.w=INFICNSZ/6,
			.h=INFICNSZ/3,
		};
		arrlst_add(&dims,&d);
		const tex_t t={
			.eid=eid,
			.tex=TEX_0+num,
		};
		arrlst_add(&texes,&t);
	}
	w->sprt0=eid;
	return eid+1;
}
