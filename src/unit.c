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
#define PNLCHRHGHT .05
uint16_t unit_sel;
bool unit_allied;
bool unit_pltrn;
uint16_t unit_panel;
arrlst_t unit_chrs={
	.es=2,
};
const wpn_t unit_wpns[]={
	{
		.name="M1 Garand",
		.acc=.025,
		.rng=1.5,
		.rpm=30,
		.type=WT_RFL,
	},
	{
		.name="M1918A2 Browning Automatic Rifle",
		.acc=.0125,
		.rng=3,
		.rpm=150,
		.type=WT_MG,
	},
	{
		.name="Karabiner 98 kurz",
		.acc=.0375,
		.rng=1.5,
		.rpm=18,
		.type=WT_RFL,
	},
	{
		.name="Maschinengewehr 42",
		.acc=.0075,
		.rng=4,
		.rpm=225,
		.type=WT_MG,
	},
	{
		.name="Maschinenpistole 40",
		.acc=.005,
		.rng=.8,
		.rpm=100,
		.type=WT_SMG,
	},
	{
		.name="P38",
		.acc=.0375,
		.rng=.15,
		.rpm=50,
		.type=WT_PSTL,
	},
};
static const char*const rnames[]={
	"Squad Leader",
	"Lieutenant",
	"Machine Gunner",
	"Assistant Machine Gunner",
	"Rifleman",
};
static int8_t tglsel(uint16_t);
static int8_t selct(uint16_t);
static float rndrstr(const char*,float,float);
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
	const uint16_t mvr=arrow+1;
	const relpos_t mr={
		.eid=mvr,
		.prnt=eid,
		.z=.9,
	};
	arrlst_add(&relposes,&mr);
	const ring_t ring={
		.eid=mvr,
		.r=INFMVSPD,
	};
	arrlst_add(&rings,&ring);
	const col_t mc={
		.eid=mvr,
		.b=.75,
	};
	arrlst_add(&cols,&mc);
	const uint16_t sprt1=mvr+1;
	const relpos_t pos1={
		.eid=sprt1,
		.prnt=eid,
		.x=3*INFICNSZ/4,
	};
	arrlst_add(&relposes,&pos1);
	const dim_t dim1={
		.eid=sprt1,
		.w=INFICNSZ/2,
		.h=INFICNSZ,
	};
	arrlst_add(&dims,&dim1);
	const tex_t tex1={
		.eid=sprt1,
		.tex=TEX_1,
	};
	arrlst_add(&texes,&tex1);
	const uint16_t sprt0=sprt1+1;
	const relpos_t pos0={
		.eid=sprt0,
		.prnt=sprt1,
		.x=INFICNSZ/2,
	};
	arrlst_add(&relposes,&pos0);
	const dim_t dim0={
		.eid=sprt0,
		.w=INFICNSZ/2,
		.h=INFICNSZ,
	};
	arrlst_add(&dims,&dim0);
	sldr_t*sldrs;
	uint64_t nsldrs;
	if(allied){
		sldrs=malloc(12*sizeof(sldr_t));
		sldrs->role=SR_SQUADLEAD;
		uint64_t*const slw=malloc(8);
		*slw=WPN_M1;
		sldrs->wpns=slw;
		sldrs->nwpns=1;
		sldr_t*const ltnt=sldrs+1;
		ltnt->role=SR_LIEUTENANT;
		uint64_t*const lw=malloc(8);
		*lw=WPN_M1;
		ltnt->wpns=lw;
		ltnt->nwpns=1;
		sldr_t*const mg=ltnt+1;
		mg->role=SR_MGNR;
		uint64_t*const bar=malloc(8);
		*bar=WPN_BAR;
		mg->wpns=bar;
		mg->nwpns=1;
		sldr_t*const astmg=mg+1;
		astmg->role=SR_ASTMGNR;
		uint64_t*const amgw=malloc(8);
		*amgw=WPN_M1;
		astmg->wpns=amgw;
		astmg->nwpns=1;
		for(sldr_t*i=astmg+1,*const __restrict end=sldrs+12;i<end;i++){
			i->role=SR_RFLMAN;
			uint64_t*const wpn=malloc(8);
			*wpn=WPN_M1;
			i->wpns=wpn;
			i->nwpns=1;
		}
		nsldrs=12;
	}else{
		sldrs=malloc(10*sizeof(sldr_t));
		sldrs->role=SR_SQUADLEAD;
		uint64_t*const slw=malloc(8);
		*slw=WPN_MP40;
		sldrs->wpns=slw;
		sldrs->nwpns=1;
		sldr_t*const ltnt=sldrs+1;
		ltnt->role=SR_LIEUTENANT;
		uint64_t*const lw=malloc(8);
		*lw=WPN_MP40;
		ltnt->wpns=lw;
		ltnt->nwpns=1;
		sldr_t*const mg=ltnt+1;
		mg->role=SR_MGNR;
		uint64_t*const mgws=malloc(16);
		*mgws=WPN_MG42;
		mgws[1]=WPN_P38;
		mg->wpns=mgws;
		mg->nwpns=2;
		for(sldr_t*i=mg+1,*const __restrict end=sldrs+10;i<end;i++){
			i->role=SR_RFLMAN;
			uint64_t*const rfl=malloc(8);
			*rfl=WPN_KAR98;
			i->wpns=rfl;
			i->nwpns=1;
		}
		nsldrs=10;
	}
	const tex_t tex0={
		.eid=sprt0,
		.tex=nsldrs%10+TEX_0,
	};
	arrlst_add(&texes,&tex0);
	const udata_t udata={
		.eid=eid,
		.arr=arrow,
		.sprt1=sprt1,
		.sprt0=sprt0,
		.free=delunit,
		.sldrs={
			.buf=sldrs,
			.nels=nsldrs,
			.bs=nsldrs*8,
			.es=sizeof(sldr_t),
		},
		.morale=1,
		.mvr=mvr,
		.flags=allied,
	};
	arrlst_add(&udatas,&udata);
	neweid=sprt0+1;
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
			const uint64_t nsldrs=u->sldrs.nels;
			tex_t*const t1=getent(&texes,u->sprt1);
			if(t1){
				t1->tex=nsldrs/10+TEX_0;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no texture\n",u->sprt1);
			}
			tex_t*const t0=getent(&texes,u->sprt0);
			if(t0){
				t0->tex=nsldrs%10+TEX_0;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no texture\n",u->sprt0);
			}
		}else{
			u->flags&=~UFLAGS_VIS;
			tex_t*const t=getent(&texes,eid);
			if(t){
				t->tex=TEX_NULL;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no texture\n",eid);
			}
			tex_t*const t1=getent(&texes,u->sprt1);
			if(t1){
				t1->tex=TEX_NULL;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no texture\n",u->sprt1);
			}
			tex_t*const t0=getent(&texes,u->sprt0);
			if(t0){
				t0->tex=TEX_NULL;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no texture\n",u->sprt0);
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
	for(lnklst_t*const*i=map,*const*const __restrict end=i+sizeof(map)/8;i<end;i++){
		delnklst(*i);
	}
	const float dx=tp.x-sp.x;
	const float dy=tp.y-sp.y;
	const float d=sqrtf(dx*dx+dy*dy);
	const double smrl=su->morale;
	bool fired=0;
	uint64_t hits=0;
	double mrlchng=0;
	for(const sldr_t*si=su->sldrs.buf,*const __restrict end=si+su->sldrs.nels;si<end;si++){
		double acc;
		uint16_t rpm;
		double ek=-0.1;
		for(const uint64_t*wi=si->wpns,*const __restrict wend=wi+si->nwpns;wi<wend;wi++){
			const wpn_t*const w=unit_wpns+*wi;
			const float rng=w->rng;
			if(d>rng){
				continue;
			}
			const double dor=d/rng;
			const double a=w->acc*smrl*((double)1-dor*dor)*cvr;
			const uint16_t r=w->rpm;
			const double e=a*r;
			if(e>ek){
				ek=e;
				acc=a;
				rpm=r;
				fired=1;
			}
		}
		if(ek<0){
			continue;
		}
		mrlchng-=rpm*.0005;
		const uint32_t acci=acc*RAND_MAX;
		for(uint16_t _=rpm;_;_--){
			if(rand()<=acci){
				hits++;
				mrlchng-=.05;
			}
		}
	}
	if(!fired){
		return E_SUCC;
	}
	if(hits){
		if(hits>=tu->sldrs.nels){
			delent(trgt);
			net_dstr(trgt);
		}else{
			uint64_t kld[hits];
			uint64_t*i=kld;
			arrlst_t*const sldrs=&tu->sldrs;
			for(uint8_t _=hits;_;_--,i++){
				const uint64_t s=(uint64_t)rand()%sldrs->nels;
				*i=s;
				arrlst_del(sldrs,s);
			}
			const uint64_t nsldrs=sldrs->nels;
			tex_t*const tex1=getent(&texes,tu->sprt1);
			if(tex1){
				tex1->tex=nsldrs/10+TEX_0;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no texture\n",tu->sprt1);
			}
			tex_t*const tex0=getent(&texes,tu->sprt0);
			if(tex0){
				tex0->tex=nsldrs%10+TEX_0;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no texture\n",tu->sprt0);
			}
			unit_chmrl(tu,mrlchng);
			net_dmg(trgt,kld,hits,mrlchng);
		}
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
	col_t*const pcol=getent(&cols,unit_panel);
	if(pcol){
		pcol->a=0;
	}else{
		fprintf(stderr,"WARNING: entity %hu has no color\n",unit_panel);
	}
	col_t*const mvrcol=getent(&cols,u->mvr);
	if(mvrcol){
		mvrcol->a=0;
	}else{
		fprintf(stderr,"WARNING: entity %hu has no color\n",u->mvr);
	}
	const uint16_t*const rngs=u->rngs;
	for(const uint16_t*i=rngs,*const __restrict end=i+u->nrngs;i<end;i++){
		delent(*i);
	}
	free(rngs);
	u->rngs=NULL;
	u->nrngs=0;
	col_t*const arrcol=getent(&cols,u->arr);
	if(arrcol){
		arrcol->a=0;
	}else{
		fprintf(stderr,"WARNING: entity %hu has no color\n",u->arr);
	}
	setinvistex(u,flags&UFLAGS_ALLIED?TEX_AXISINF:TEX_ALLIEDINF);
	for(const uint16_t*i=unit_chrs.buf,*const __restrict end=i+unit_chrs.nels;i<end;i++){
		delent(*i);
	}
	unit_chrs.nels=0;
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
	ring_t*const r=getent(&rings,u->mvr);
	if(r){
		r->r=INFMVSPD*mrl;
	}else{
		fprintf(stderr,"WARNING: entity %hu has no radius\n",u->mvr);
	}
}
static int8_t tglsel(const uint16_t eid){
	if(win_frftbtn){
		return E_SUCC;
	}
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
	if(unit_sel){
		unit_deselct(unit_sel);
	}
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
	const uint16_t panl=unit_panel;
	dim_t*const pdim=getent(&dims,panl);
	const sldr_t*const sldrs=u->sldrs.buf;
	const uint64_t nsldrs=u->sldrs.nels;
	const sldr_t*const __restrict send=sldrs+nsldrs;
	if(pdim){
		uint64_t longest=0;
		for(const sldr_t*si=sldrs;si<send;si++){
			const uint64_t nwpns=si->nwpns;
			uint64_t len=strlen(rnames[si->role])+((nwpns-1)<<1)+3;
			for(const uint64_t*wi=si->wpns,*const __restrict wend=wi+nwpns;wi<wend;wi++){
				len+=strlen(unit_wpns[*wi].name);
			}
			if(len>longest){
				longest=len;
			}
		}
		pdim->w=(float)longest*PNLCHRHGHT;
		pdim->h=(float)nsldrs*PNLCHRHGHT*2;
	}else{
		fprintf(stderr,"WARNING: entity %hu has no dimensions\n",panl);
	}
	col_t*const pcol=getent(&cols,panl);
	if(pcol){
		pcol->a=1;
	}else{
		fprintf(stderr,"WARNING: entity %hu has no color\n",panl);
	}
	float y=((float)nsldrs-1)*PNLCHRHGHT+PNLCHRHGHT/2;
	for(const sldr_t*si=sldrs;si<send;si++){
		float x=rndrstr(" - ",rndrstr(rnames[si->role],PNLCHRHGHT/4,y),y);
		for(const uint64_t*wi=si->wpns,*const __restrict lst=wi+si->nwpns-1;wi<=lst;wi++){
			x=rndrstr(unit_wpns[*wi].name,x,y);
			if(wi<lst){
				x=rndrstr(", ",x,y);
			}
		}
		y-=PNLCHRHGHT;
	}
	u->flags=flags;
	col_t*const col=getent(&cols,u->mvr);
	if(col){
		col->a=1;
	}else{
		fprintf(stderr,"WARNING: entity %hu has no color\n",u->mvr);
	}
	lnklst_t*map[8]={};
	const hshmp_t rngst={
		.map=map,
		.nbkts=8,
		.hshfnc=ret,
	};
	for(const sldr_t*si=sldrs;si<send;si++){
		for(const uint64_t*wi=si->wpns,*const __restrict wend=wi+si->nwpns;wi<wend;wi++){
			const double rng=unit_wpns[*wi].rng;
			hshmp_addifabs(&rngst,*((int64_t*)&rng));
		}
	}
	arrlst_t rngs={
		.buf=malloc(16),
		.bs=16,
		.es=2,
	};
	uint16_t neid=neweid;
	for(lnklst_t*const*mi=map,*const*const __restrict mend=mi+sizeof(map)/8;mi<mend;mi++){
		for(const lnklst_t*li=*mi;li;li=li->nxt){
			const relpos_t pos={
				.eid=neid,
				.prnt=eid,
				.z=.9,
			};
			arrlst_add(&relposes,&pos);
			const ring_t ring={
				.eid=neid,
				.r=*((double*)&li->val),
			};
			arrlst_add(&rings,&ring);
			const col_t col={
				.eid=neid,
				.r=.75,
				.a=1,
			};
			arrlst_add(&cols,&col);
			arrlst_add(&rngs,&neid);
			neid++;
		}
	}
	neweid=neid;
	u->rngs=rngs.buf;
	u->nrngs=rngs.nels;
	for(lnklst_t*const*i=map,*const*const __restrict end=i+sizeof(map)/8;i<end;i++){
		delnklst(*i);
	}
	setinvistex(u,flags&UFLAGS_ALLIED?TEX_AXISINFACTD:TEX_ALLIEDINFACTD);
	win_clkoff=(int8_t(*)(int64_t,float,float))unit_deselct;
	win_rclk=(int8_t(*)(int64_t,float,float))unit_move;
	win_clkoffparam=eid;
	win_rclkparam=eid;
	unit_sel=eid;
	return E_SUCC;
}
static float rndrstr(const char*str,float x,const float y){
	uint16_t eid=neweid;
	for(char c;(c=*str);str++){
		if(c==' '){
			x+=PNLCHRHGHT/2;
			continue;
		}else if(c<LOWSTPRNTBL||c>HGHSTPRNTBL){
			fprintf(stderr,"WARNING: character code 0x%hho is not printable",c);
			continue;
		}
		const scrnpos_t pos={
			.eid=eid,
			.x=x,
			.y=c=='g'||c=='p'||c=='q'||c==','?y-.005:y,
			.xancr=XANCR_LEFT,
			.yancr=YANCR_BOT,
		};
		arrlst_add(&scrnposes,&pos);
		const dim_t dim={
			.eid=eid,
			.w=PNLCHRHGHT/2,
			.h=PNLCHRHGHT,
		};
		arrlst_add(&dims,&dim);
		const tex_t tex={
			.eid=eid,
			.tex=(uint64_t)c-LOWSTPRNTBL+TEX_LOWSTPRNTBL,
		};
		arrlst_add(&texes,&tex);
		arrlst_add(&unit_chrs,&eid);
		x+=PNLCHRHGHT/2;
		eid++;
	}
	neweid=eid;
	return x;
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
	return E_SUCC;
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
	delent(u->mvr);
	delent(u->sprt1);
	delent(u->sprt0);
	const uint16_t*rngs=u->rngs;
	if(rngs){
		for(const uint16_t*i=rngs,*const __restrict end=rngs+u->nrngs;i<end;i++){
			delent(*i);
		}
		free(rngs);
	}
	const sldr_t*const sldrs=u->sldrs.buf;
	for(const sldr_t*i=sldrs,*const __restrict end=sldrs+u->sldrs.nels;i<end;i++){
		free(i->wpns);
	}
	free(sldrs);
}
static uint64_t ret(const int64_t x){
	return x;
}
