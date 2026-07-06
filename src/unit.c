#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"grphcs.h"
#include"menu.h"
#include"net.h"
#include"win.h"
#include"unit.h"
#define INFMVSPD .6
#define INFICNSZ .1
uint16_t unit_sel;
bool unit_allied;
bool unit_pltrn;
static uint16_t mkwpn(wpn_t*__restrict,uint16_t*__restrict,uint16_t,uint16_t,float);
static int8_t tglsel(uint16_t);
static int8_t selct(uint16_t);
static int8_t setinvistex(const udata_t*,uint64_t);
static void
vsblto(posn_t,const udata_t*const*__restrict,uint64_t,const hshmp_t*__restrict);
static bool los(posn_t,posn_t,const hshmp_t*__restrict);
static void inhdgs(const hshmp_t*__restrict,posn_t);
static uint64_t ret(int64_t);
static void delunit(const udata_t*);
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
			.z=.9,
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
		wpns[0].acc=.05;
		wpns[0].rng=3;
		wpns[0].rpm=150;
		wpns[0].num=1;
		const uint16_t w1=mkwpn(wpns,rngs+1,r0+1,eid,INFICNSZ/4);
		wpns[1].acc=.1;
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
			.z=.9,
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
		wpns[0].acc=.03;
		wpns[0].rng=4;
		wpns[0].rpm=225;
		wpns[0].num=1;
		const uint16_t w1=mkwpn(wpns,rngs+1,r0+1,eid,INFICNSZ/3);
		wpns[1].acc=.002;
		wpns[1].rng=.4;
		wpns[1].rpm=100;
		wpns[1].num=2;
		const uint16_t w2=mkwpn(wpns+1,rngs+2,w1,eid,0);
		wpns[2].acc=.15;
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
		.morale=1,
	};
	arrlst_add(&udatas,&udata);
	neweid=neid;
}
void unit_chklos(){
	arrlst_t frndly={
		.buf=malloc(8*16),
		.bs=8*16,
		.es=8,
	};
	arrlst_t enmy={
		.buf=malloc(8*16),
		.bs=8*16,
		.es=8,
	};
	const bool ald=unit_allied;
	for(const udata_t*i=udatas.buf,*const __restrict end=i+udatas.nels;i<end;i++){
		arrlst_add((i->flags&UFLAGS_ALLIED)==ald?&frndly:&enmy,&i);
	}
	lnklst_t*map[16]={};
	const hshmp_t vsbl={
		.map=map,
		.nbkts=16,
		.hshfnc=ret,
	};
	for(
			const udata_t*const*i=frndly.buf,*const*const __restrict end=i+frndly.nels;
			i<end;
			i++
			){
		int8_t err;
		const posn_t posn=getposn((*i)->eid,&err);
		if(err){
			fprintf(stderr,"WARNING: entity %hu has no position\n",(*i)->eid);
			continue;
		}
		vsblto(posn,enmy.buf,enmy.nels,&vsbl);
	}
	const uint64_t utex=ald?TEX_AXISINF:TEX_ALLIEDINF;
	for(udata_t*const*i=enmy.buf,*const*const __restrict end=i+enmy.nels;i<end;i++){
		udata_t*const u=*i;
		const uint16_t eid=u->eid;
		if(hshmp_in(&vsbl,eid)){
			u->flags|=UFLAGS_VIS;
			tex_t*const t=getent(&texes,eid);
			if(t){
				t->tex=utex;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no texture\n",eid);
			}
			for(const wpn_t*i=u->wpns,*const __restrict end=i+u->nwpns;i<end;i++){
				const uint8_t num=i->num;
				const uint16_t sprt1=i->sprt1;
				if(sprt1){
					tex_t*const t1=getent(&texes,sprt1);
					if(t1){
						t1->tex=num/10+TEX_0;
					}else{
						fprintf(stderr,"WARNING: entity %hu has no texture\n",sprt1);
					}
				}
				tex_t*const t0=getent(&texes,i->sprt0);
				if(t0){
					t0->tex=num%10+TEX_0;
				}else{
					fprintf(stderr,"WARNING: entity %hu has no texture\n",i->sprt0);
				}
			}
		}else{
			u->flags&=~UFLAGS_VIS;
			tex_t*const t=getent(&texes,eid);
			if(t){
				t->tex=TEX_NULL;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no texture\n",eid);
			}
			for(const wpn_t*i=u->wpns,*const __restrict end=i+u->nwpns;i<end;i++){
				const uint16_t sprt1=i->sprt1;
				if(sprt1){
					tex_t*const t1=getent(&texes,sprt1);
					if(t1){
						t1->tex=TEX_NULL;
					}else{
						fprintf(stderr,"WARNING: entity %hu has no texture\n",sprt1);
					}
				}
				tex_t*const t0=getent(&texes,i->sprt0);
				if(t0){
					t0->tex=TEX_NULL;
				}else{
					fprintf(stderr,"WARNING: entity %hu has no texture\n",i->sprt0);
				}
			}
		}
	}
}
int8_t unit_move(const uint16_t eid,const float x,const float y){
	pos_t*const p=getent(&poses,eid);
	if(!p){
		fprintf(stderr,"ERROR: entity %hu has no position\n",eid);
		return E_NO_ENT;
	}
	udata_t*const u=getent(&udatas,eid);
	if(!u){
		fprintf(stderr,"ERROR: entity %hu has no udata\n",eid);
		return E_NO_ENT;
	}
	const float dx=x-p->x;
	const float dy=y-p->y;
	if(sqrtf(dx*dx+dy*dy)>INFMVSPD*u->morale){
		return E_SUCC;
	}
	p->x=x;
	p->y=y;
	const int8_t err=unit_deselct(eid);
	if(err){
		fprintf(
			stderr,
			"WARNING: failed to deselect entity %hu with error code %hhu\n",
			eid,
			err
		);
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
	unit_chklos();
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
	udata_t*const tu=getent(&udatas,trgt);
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
	lnklst_t*map[8]={};
	const hshmp_t hdgs={
		.map=map,
		.nbkts=8,
		.hshfnc=ret,
	};
	inhdgs(&hdgs,tp);
	const double cvr=hshmp_empty(&hdgs)?1:.3;
	const posn_t sp=getposn(shtr,&err);
	inhdgs(&hdgs,sp);
	if(!los(sp,tp,&hdgs)){
		return E_SUCC;
	}
	for(const lnklst_t*const*i=map,*const*const __restrict end=i+sizeof(map)/8;i<end;i++){
		delnklst(*i);
	}
	const float dx=tp.x-sp.x;
	const float dy=tp.y-sp.y;
	const float d=sqrtf(dx*dx+dy*dy);
	const wpn_t*wi=su->wpns;
	const wpn_t*const __restrict wend=wi+su->nwpns;
	const double smrl=su->morale;
	bool fired=0;
	uint8_t hits=0;
	double mrlchng=0;
	for(;wi<wend;wi++){
		const uint8_t num=wi->num;
		const float rng=wi->rng;
		if(d>wi->rng||!num){
			continue;
		}
		fired=1;
		const float dor=d/rng;
		const uint32_t acc=wi->acc*smrl*((double)1-dor*dor)*cvr*RAND_MAX;
		const uint16_t rpm=(double)wi->rpm*smrl;
		mrlchng-=rpm*.0005;
		for(uint16_t _=wi->rpm*num;_;_--){
			if(rand()<=acc){
				hits++;
				mrlchng-=.05;
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
			const uint32_t ri=rand();
			const double r=(double)ri/RAND_MAX;
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
		unit_chmrl(tu,mrlchng);
		net_dmg(trgt,nums,tnwpns,mrlchng);
	}
	err=unit_deselct(shtr);
	if(err){
		fprintf(
			stderr,
			"ERROR: failed to deselect entity %hu with error code %hhd\n",
			shtr,
			err
		);
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
int8_t unit_nxtrn(const bool ald){
	uint64_t t;
	if(unit_allied==ald){
		udata_t*i=udatas.buf;
		const udata_t*const __restrict end=i+udatas.nels;
		for(;i<end;i++){
			const int8_t flags=i->flags&~UFLAGS_ACTED;
			if((flags&UFLAGS_ALLIED)==ald){
				i->flags=flags;
				unit_chmrl(i,.1);
				unit_deselct(i->eid);
			}
		}
		net_endtrn();
		unit_pltrn=0;
		t=TEX_OPTRN;
	}else{
		unit_pltrn=1;
		t=TEX_NXTRN;
	}
	tex_t*const tex=getent(&texes,menu_nxtrn);
	if(tex){
		tex->tex=t;
	}else{
		fprintf(stderr,"WARNING: entity %hu has no texture\n",menu_nxtrn);
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
		const uint16_t ring=*i;
		if(!ring){
			continue;
		}
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
	setinvistex(u,flags&UFLAGS_ALLIED?TEX_AXISINF:TEX_ALLIEDINF);
	win_clkoff=NULL;
	win_rclk=NULL;
	win_inmode=INMODE_NORM;
	unit_sel=0;
	return E_SUCC;
}
void unit_chmrl(udata_t*const u,const double mrld){
	double mrl=u->morale+mrld;
	if(mrl<=0){
		mrl=0;
	}else if(mrl>=1){
		mrl=1;
	}
	u->morale=mrl;
	ring_t*const r=getent(&rings,u->rings[0]);
	if(r){
		r->r=INFMVSPD*mrl;
	}else{
		fprintf(stderr,"WARNING: entity %hu has no radius\n",u->rings[0]);
	}
}
static uint16_t mkwpn(
		wpn_t*const __restrict w,
		uint16_t*const __restrict ri,
		uint16_t eid,
		const uint16_t unit,
		const float y
		){
	const relpos_t rp={
		.eid=eid,
		.prnt=unit,
		.x=0,
		.y=0,
		.z=.9,
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
		const uint16_t ring=*i;
		if(ring){
			col_t*const col=getent(&cols,ring);
			if(col){
				col->a=1;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no color\n",ring);
			}
		}
	}
	if(unit_sel&&unit_sel!=eid){
		unit_deselct(unit_sel);
	}
	setinvistex(u,flags&UFLAGS_ALLIED?TEX_AXISINFACTD:TEX_ALLIEDINFACTD);
	win_clkoff=(int8_t(*)(int64_t,float,float))unit_deselct;
	win_rclk=(int8_t(*)(int64_t,float,float))unit_move;
	win_clkoffparam=eid;
	win_rclkparam=eid;
	unit_sel=eid;
	return E_SUCC;
}
static int8_t setinvistex(const udata_t*const u,const uint64_t t){
	int8_t err;
	const posn_t posn=getposn(u->eid,&err);
	if(err){
		fprintf(stderr,"ERROR: entity %hu has no position\n",u->eid);
		return err;
	}
	arrlst_t enmies={
		.buf=malloc(16*8),
		.bs=16*8,
		.es=8,
	};
	const bool ald=u->flags&UFLAGS_ALLIED;
	for(const udata_t*i=udatas.buf,*const __restrict end=i+udatas.nels;i<end;i++){
		if((i->flags&UFLAGS_ALLIED)!=ald){
			arrlst_add(&enmies,&i);
		}
	}
	lnklst_t*map[8]={};
	const hshmp_t vsbl={
		.map=map,
		.nbkts=8,
		.hshfnc=ret,
	};
	vsblto(posn,enmies.buf,enmies.nels,&vsbl);
	for(
			const udata_t*const*i=enmies.buf,*const*const __restrict end=i+enmies.nels;
			i<end;
			i++
			){
		const udata_t*const u=*i;
		const uint16_t eid=u->eid;
		if(u->flags&UFLAGS_VIS&&!hshmp_in(&vsbl,eid)){
			tex_t*const tex=getent(&texes,eid);
			if(tex){
				tex->tex=t;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no texture\n",eid);
			}
		}
	}
}
static void vsblto(
		const posn_t ep,
		const udata_t*const*const __restrict frndls,
		const uint64_t nf,
		const hshmp_t*const __restrict vsbl
		){
	lnklst_t*map[8]={};
	const hshmp_t hdgs={
		.map=map,
		.nbkts=8,
		.hshfnc=ret,
	};
	inhdgs(&hdgs,ep);
	for(const udata_t*const*i=frndls,*const*const __restrict end=i+nf;i<end;i++){
		const uint16_t eid=(*i)->eid;
		int8_t err;
		const posn_t fp=getposn(eid,&err);
		if(err){
			fprintf(stderr,"WARNING: entity %hu has no position\n",eid);
			continue;
		}
		const hshmp_t nh=cpyhshmp(&hdgs);
		inhdgs(&nh,fp);
		if(los(ep,fp,&nh)){
			hshmp_addifabs(vsbl,eid);
		}
		delhshmp(&nh);
	}
}
static bool los(const posn_t sp,const posn_t tp,const hshmp_t*const __restrict hdgs){
	if(sp.x==tp.x){
		float top,bot;
		if(sp.y>tp.y){
			top=sp.y;
			bot=tp.y;
		}else{
			top=tp.y;
			bot=sp.y;
		}
		for(const hedge_t*i=hedges.buf,*const __restrict end=i+hedges.nels;i<end;i++){
			const uint16_t h=i->eid;
			const line_t*const ln=getent(&lines,h);
			if(!ln){
				fprintf(stderr,"WANRING: entity %hu has no line\n",h);
				continue;
			}
			if(ln->x0==sp.x&&ln->x1==sp.x){
				continue;
			}
			const float y0=ln->y0;
			const float y1=ln->y1;
			if((y0<=top&&y0>=bot)||(y1<=top&&y1>=bot)){
				return 0;
			}
		}
	}else{
		const float stlm=(tp.y-sp.y)/(tp.x-sp.x);
		const float stlb=sp.y-stlm*sp.x;
		float stlft,stlrt;
		if(sp.x>tp.x){
			stlrt=sp.x;
			stlft=tp.x;
		}else{
			stlrt=tp.x;
			stlft=sp.x;
		}
		for(const hedge_t*i=hedges.buf,*const __restrict end=i+hedges.nels;i<end;i++){
			const uint16_t h=i->eid;
			if(hshmp_in(hdgs,h)){
				continue;
			}
			const line_t*const ln=getent(&lines,h);
			if(!ln){
				fprintf(stderr,"WARNING: entity %hu has no line\n",h);
				continue;
			}
			const float x0=ln->x0;
			const float y0=ln->y0;
			const float x1=ln->x1;
			const float hm=(ln->y1-y0)/(x1-x0);
			const float hb=y0-hm*x0;
			if(hm==stlm){
				if(hb==stlb&&x0>=stlft&&x0<=stlrt){
					return 0;
				}
			}else{
				const float ix=(stlb-hb)/(hm-stlm);
				float hl,hr;
				if(x0>x1){
					hr=x0;
					hl=x1;
				}else{
					hr=x1;
					hl=x0;
				}
				if(ix>=stlft&&ix<=stlrt&&ix>=hl&&ix<=hr){
					return 0;
				}
			}
		}
	}
	return 1;
}
static void inhdgs(const hshmp_t*const __restrict hdgs,const posn_t up){
	for(const hedge_t*i=hedges.buf,*const __restrict end=i+hedges.nels;i<end;i++){
		const uint16_t h=i->eid;
		const line_t*const l=getent(&lines,h);
		if(!l){
			fprintf(stderr,"WARNING: entity %hu has no line\n",i->eid);
			continue;
		}
		const float x1=l->x1;
		const float y1=l->y1;
		const float x=(x1+l->x0)/2;
		const float y=(y1+l->y0)/2;
		const float dx=x1-x;
		const float dy=y1-y;
		const float hl=sqrtf(dx*dx+dy*dy);
		const float hw=l->width/2;
		const float udx=up.x-x;
		const float udy=up.y-y;
		const float ud=sqrtf(udx*udx+udy*udy);
		const float a=atan2f(udy,udx)-atan2f(dy,dx);
		const float ux=cosf(a)*ud;
		const float uy=sinf(a)*ud;
		if(ux<=hl&&ux>=-hl&&uy<=hw&&uy>=-hw){
			hshmp_add(hdgs,h);
		}
	}
}
static void delunit(const udata_t*const u){
	delent(u->arr);
	for(const wpn_t*i=u->wpns,*const __restrict end=i+u->nwpns;i<end;i++){
		delent(i->sprt0);
		if(i->sprt1){
			delent(i->sprt1);
		}
	}
	free(u->wpns);
	for(const uint16_t*i=u->rings,*const __restrict end=i+u->nrings;i<end;i++){
		const uint16_t eid=*i;
		if(eid){
			delent(eid);
		}
	}
	free(u->rings);
	delent(u->arr);
}
static uint64_t ret(const int64_t x){
	return x;
}
