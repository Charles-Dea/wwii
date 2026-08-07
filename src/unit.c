//Added to fix Windows brain cancer
#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"grphcs.h"
#include"menu.h"
#include"net.h"
#include"win.h"
#include"unit.h"
#define INFICNSZ .1
#define PNLCHRHGHT .05
#define INFWDTH .001
#define FRGHITPRB .002
#define GRNDMNRNG .004
#define FRGCVR .5
#define HECVR .7
uint32_t unit_sel;
bool unit_allied;
bool unit_pltrn;
uint32_t unit_panel;
arrlst_t unit_chrs={
	.es=4,
};
arrlst_t unit_aoes={
	.es=4,
};
const wpn_t unit_wpns[]={
	{
		.frarm={
			.base={
				.name="M1 Garand",
				.rng=1.5,
				.rpm=30,
				.type=WT_FRARM,
				.flags=WFLAGS_DRCT,
			},
			.acc=.025,
		},
	},
	{
		.frarm={
			.base={
				.name="M1 Carbine",
				.rng=1,
				.rpm=81,
				.type=WT_FRARM,
				.flags=WFLAGS_DRCT,
			},
			.acc=.01625,
		},
	},
	{
		.frarm={
			.base={
				.name="M1918A2 Browning Automatic Rifle",
				.rng=3,
				.rpm=150,
				.type=WT_FRARM,
				.flags=WFLAGS_CREW|WFLAGS_DRCT,
			},
			.acc=.0125,
		},
	},
	{
		.frarm={
			.base={
				.name="M1911A1 Pistol",
				.rng=.15,
				.rpm=11,
				.type=WT_FRARM,
			},
			.acc=.05625,
		},
	},
	{
		.frarm={
			.base={
				.name="M1919A4 Browning Machine Gun",
				.rng=3,
				.rpm=200,
				.type=WT_FRARM,
				.flags=WFLAGS_CREW|WFLAGS_DRCT,
			},
			.acc=.01875,
		},
	},
	{
		.explsv={
			.base={
				.name="M7 Grenade Launcher",
				.rng=.75,
				.rpm=10,
				.type=WT_EXPLSV,
				.flags=WFLAGS_DRCT,
			},
			.sprd=.02,
			.he=1,
			.minrng=.04,
			.nfrags=1150,
		},
	},
	{
		.explsv={
			.base={
				.name="M2 60mm Mortar",
				.rng=4,
				.rpm=18,
				.type=WT_EXPLSV,
				.flags=WFLAGS_CREW,
			},
			.sprd=.2,
			.he=20,
			.minrng=.4,
			.nfrags=2300,
		},
	},
	{
		.frarm={
			.base={
				.name="Karabiner 98 kurz",
				.rng=1.5,
				.rpm=18,
				.type=WT_FRARM,
				.flags=WFLAGS_DRCT,
			},
			.acc=.0375,
		},
	},
	{
		.frarm={
			.base={
				.name="Maschinengewehr 42",
				.rng=4,
				.rpm=225,
				.type=WT_FRARM,
				.flags=WFLAGS_CREW|WFLAGS_DRCT,
			},
			.acc=.0075,
		},
	},
	{
		.frarm={
			.base={
				.name="Maschinenpistole 40",
				.rpm=100,
				.rng=.8,
				.type=WT_FRARM,
				.flags=WFLAGS_DRCT,
			},
			.acc=.005,
		},
	},
	{
		.frarm={
			.base={
				.name="P38",
				.rng=.15,
				.rpm=50,
				.type=WT_FRARM,
				.flags=WFLAGS_DRCT,
			},
			.acc=.0375,
		},
	},
	{
		.frarm={
			.base={
				.name="Maschinengewehr 42",
				.rng=4,
				.rpm=236,
				.type=WT_FRARM,
				.flags=WFLAGS_CREW|WFLAGS_DRCT,
			},
			.acc=.01,
		},
	},
	{
		.explsv={
			.base={
				.name="8cm Granatwerfer 34",
				.rng=4.8,
				.rpm=20,
				.type=WT_EXPLSV,
				.flags=WFLAGS_CREW,
			},
			.sprd=.2,
			.he=20,
			.minrng=.4,
			.nfrags=2300,
		},
	},
};
static const char*const rnames[]={
	"Squad Leader",
	"Lieutenant",
	"Machine Gunner",
	"Assistant Machine Gunner",
	"Rifleman",
	"Ammo Bearer",
	"Rifle Grenadier",
	"Gunner",
	"Assistant Gunner",
};
static int8_t tglsel(uint32_t);
static int8_t selct(uint32_t);
static float rndrstr(const char*,float,float);
static int8_t setinvistex(const udata_t*,bool);
static void
vsblto(posn_t,const udata_t*const*__restrict,uint64_t,const hshmp_t*__restrict);
static bool los(posn_t,posn_t,const hshmp_t*__restrict);
static void inhdgs(const hshmp_t*__restrict,posn_t);
static uint64_t getex(const udata_t*);
static void hitpos(const wpn_t*,double,double,double,double);
static void dmg(udata_t*,uint8_t,double);
static void delunit(const udata_t*);
static uint64_t ret(int64_t);
int8_t mkunit(const float x,const float y,const bool allied,const uint8_t type){
	const uint32_t eid=neweid;
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
	udata_t udata={
		.eid=eid,
		.free=delunit,
		.sldrs={
			.es=sizeof(sldr_t),
		},
		.morale=1,
		.type=type,
		.flags=allied,
	};
	const tex_t tex={
		.eid=eid,
		.tex=getex(&udata),
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
	udata.arr=eid+1;
	const relpos_t arp={
		.eid=udata.arr,
		.prnt=eid,
		.z=.5,
	};
	arrlst_add(&relposes,&arp);
	const col_t acol={
		.eid=udata.arr,
	};
	arrlst_add(&cols,&acol);
	const arrow_t arr={
		.eid=udata.arr,
	};
	arrlst_add(&arrows,&arr);
	udata.mvr=udata.arr+1;
	const relpos_t mr={
		.eid=udata.mvr,
		.prnt=eid,
		.z=.9,
	};
	arrlst_add(&relposes,&mr);
	ring_t ring={
		.eid=udata.mvr,
	};
	const col_t mc={
		.eid=udata.mvr,
		.b=.75,
	};
	arrlst_add(&cols,&mc);
	udata.sprt1=udata.mvr+1;
	const relpos_t pos1={
		.eid=udata.sprt1,
		.prnt=eid,
		.x=3*INFICNSZ/4,
	};
	arrlst_add(&relposes,&pos1);
	const dim_t dim1={
		.eid=udata.sprt1,
		.w=INFICNSZ/2,
		.h=INFICNSZ,
	};
	arrlst_add(&dims,&dim1);
	udata.sprt0=udata.sprt1+1;
	const relpos_t pos0={
		.eid=udata.sprt0,
		.prnt=udata.sprt1,
		.x=INFICNSZ/2,
	};
	arrlst_add(&relposes,&pos0);
	const dim_t dim0={
		.eid=udata.sprt0,
		.w=INFICNSZ/2,
		.h=INFICNSZ,
	};
	arrlst_add(&dims,&dim0);
	sldr_t*sldrs;
	if(allied){
		switch(type){
			case UT_INF:
				sldrs=malloc(12*sizeof(sldr_t));
				sldrs->role=SR_SQUADLEAD;
				uint64_t*slw=malloc(8);
				*slw=WPN_M1;
				sldrs->wpns=slw;
				sldrs->nwpns=1;
				sldr_t*const ltnt=sldrs+1;
				ltnt->role=SR_LIEUTENANT;
				uint64_t*const lw=malloc(8);
				*lw=WPN_M1;
				ltnt->wpns=lw;
				ltnt->nwpns=1;
				sldr_t*mg=ltnt+1;
				mg->role=SR_MGNR;
				uint64_t*const bar=malloc(8);
				*bar=WPN_BAR;
				mg->wpns=bar;
				mg->nwpns=1;
				sldr_t*astmg=mg+1;
				astmg->role=SR_ASTMGNR;
				uint64_t*const amgw=malloc(8);
				*amgw=WPN_M1;
				astmg->wpns=amgw;
				astmg->nwpns=1;
				sldr_t*const ambr=astmg+1;
				ambr->role=SR_AMBRER;
				uint64_t*const abw=malloc(8);
				*abw=WPN_M1;
				ambr->wpns=abw;
				ambr->nwpns=1;
				for(sldr_t*i=ambr+1,*const __restrict last=ambr+2;i<=last;i++){
					i->role=SR_GRNDR;
					uint64_t*const wpns=malloc(16);
					*wpns=WPN_M1;
					wpns[1]=WPN_M7;
					i->wpns=wpns;
					i->nwpns=2;
				}
				for(sldr_t*i=ambr+3,*const __restrict end=sldrs+12;i<end;i++){
					i->role=SR_RFLMAN;
					uint64_t*const wpn=malloc(8);
					*wpn=WPN_M1;
					i->wpns=wpn;
					i->nwpns=1;
				}
				udata.sldrs.nels=12;
				udata.spd=.6;
				ring.r=.6;
				break;
			case UT_MG:
				sldrs=malloc(5*sizeof(sldr_t));
				sldrs->role=SR_SQUADLEAD;
				slw=malloc(8);
				*slw=WPN_M1;
				sldrs->wpns=slw;
				sldrs->nwpns=1;
				mg=sldrs+1;
				mg->role=SR_GNR;
				uint64_t*const m1919=malloc(8);
				*m1919=WPN_M1919;
				mg->wpns=m1919;
				mg->nwpns=1;
				astmg=mg+1;
				astmg->role=SR_ASTGNR;
				uint64_t*m1911=malloc(8);
				*m1911=WPN_M1911;
				astmg->wpns=m1911;
				astmg->nwpns=1;
				for(sldr_t*i=astmg+1,*const __restrict end=sldrs+5;i<end;i++){
					i->role=SR_AMBRER;
					uint64_t*const crbn=malloc(8);
					*crbn=WPN_M1CRBN;
					i->wpns=crbn;
					i->nwpns=1;
				}
				udata.sldrs.nels=5;
				udata.spd=.3;
				ring.r=.3;
				break;
			case UT_MRTR:
				sldrs=malloc(5*sizeof(sldr_t));
				sldrs->role=SR_SQUADLEAD;
				slw=malloc(8);
				*slw=WPN_M1;
				sldrs->wpns=slw;
				sldrs->nwpns=1;
				sldr_t*const gnr=sldrs+1;
				gnr->role=SR_GNR;
				uint64_t*const gnrwpns=malloc(16);
				*gnrwpns=WPN_M2;
				gnrwpns[1]=WPN_M1911;
				gnr->wpns=gnrwpns;
				gnr->nwpns=2;
				sldr_t*const astgnr=gnr+1;
				astgnr->role=SR_ASTGNR;
				m1911=malloc(8);
				*m1911=WPN_M1911;
				astgnr->wpns=m1911;
				astgnr->nwpns=1;
				for(sldr_t*i=astgnr+1,*const __restrict end=sldrs+5;i<end;i++){
					i->role=SR_AMBRER;
					uint64_t*const m1=malloc(8);
					*m1=WPN_M1CRBN;
					i->wpns=m1;
					i->nwpns=1;
				}
				udata.sldrs.nels=5;
				udata.spd=.3;
				ring.r=.3;
				break;
			default:
				fprintf(stderr,"ERROR: %hhu is not a valid unit type\n",type);
				delent(eid);
				delent(udata.arr);
				delent(udata.mvr);
				delent(udata.sprt0);
				delent(udata.sprt1);
				return E_INVALD_UT;
		}
		udata.sldrs.buf=sldrs;
	}else{
		switch(type){
			case UT_INF:
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
				sldr_t*mg=ltnt+1;
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
				udata.sldrs.nels=10;
				udata.spd=.6;
				ring.r=.6;
				break;
			case UT_MG:
				sldrs=malloc(6*sizeof(sldr_t));
				sldrs->role=SR_SQUADLEAD;
				uint64_t*mp40=malloc(8);
				*mp40=WPN_MP40;
				sldrs->wpns=mp40;
				sldrs->nwpns=1;
				mg=sldrs+1;
				mg->role=SR_GNR;
				uint64_t*const mgw=malloc(16);
				*mgw=WPN_MG42TRI;
				mgw[1]=WPN_P38;
				mg->wpns=mgw;
				mg->nwpns=2;
				sldr_t*ast=mg+1;
				ast->role=SR_ASTGNR;
				uint64_t*p38=malloc(8);
				*p38=WPN_P38;
				ast->wpns=p38;
				ast->nwpns=1;
				for(sldr_t*i=ast+1,*const __restrict end=sldrs+6;i<end;i++){
					i->role=SR_RFLMAN;
					uint64_t*const kar98k=malloc(8);
					*kar98k=WPN_KAR98;
					i->wpns=kar98k;
					i->nwpns=1;
				}
				udata.sldrs.nels=6;
				udata.spd=.3;
				ring.r=.3;
				break;
			case UT_MRTR:
				sldrs=malloc(4*sizeof(sldr_t));
				sldrs->role=SR_SQUADLEAD;
				mp40=malloc(8);
				*mp40=WPN_MP40;
				sldrs->wpns=mp40;
				sldrs->nwpns=1;
				sldr_t*const gnr=sldrs+1;
				gnr->role=SR_GNR;
				uint64_t*const gw=malloc(16);
				*gw=WPN_GRW34;
				gw[1]=WPN_P38;
				gnr->wpns=gw;
				gnr->nwpns=2;
				ast=gnr+1;
				ast->role=SR_ASTGNR;
				p38=malloc(8);
				*p38=WPN_P38;
				ast->wpns=p38;
				ast->nwpns=1;
				sldr_t*const ambr=ast+1;
				ambr->role=SR_AMBRER;
				uint64_t*const kar98k=malloc(8);
				*kar98k=WPN_KAR98;
				ambr->wpns=kar98k;
				ambr->nwpns=1;
				udata.sldrs.nels=4;
				udata.spd=.3;
				ring.r=.3;
				break;
			default:
				fprintf(stderr,"ERROR: %hhu is not a valid unit type\n",type);
				delent(eid);
				delent(udata.arr);
				delent(udata.mvr);
				delent(udata.sprt0);
				delent(udata.sprt1);
				return E_INVALD_UT;
		}
	}
	udata.sldrs.buf=sldrs;
	const tex_t tex1={
		.eid=udata.sprt1,
		.tex=udata.sldrs.nels/10+TEX_0,
	};
	arrlst_add(&texes,&tex1);
	const tex_t tex0={
		.eid=udata.sprt0,
		.tex=udata.sldrs.nels%10+TEX_0,
	};
	arrlst_add(&texes,&tex0);
	arrlst_add(&rings,&ring);
	arrlst_add(&udatas,&udata);
	neweid=udata.sprt0+1;
	return E_SUCC;
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
			fprintf(stderr,"WARNING: entity %u has no position\n",(*i)->eid);
			continue;
		}
		vsblto(posn,enmy.buf,enmy.nels,&vsbl);
	}
	for(udata_t*const*i=enmy.buf,*const*const __restrict end=i+enmy.nels;i<end;i++){
		udata_t*const u=*i;
		const uint32_t eid=u->eid;
		if(hshmp_in(&vsbl,eid)){
			u->flags|=UFLAGS_VIS;
			tex_t*const t=getent(&texes,eid);
			if(t){
				t->tex=getex(u);
			}else{
				fprintf(stderr,"WARNING: entity %u has no texture\n",eid);
			}
			const uint64_t nsldrs=u->sldrs.nels;
			tex_t*const t1=getent(&texes,u->sprt1);
			if(t1){
				t1->tex=nsldrs/10+TEX_0;
			}else{
				fprintf(stderr,"WARNING: entity %u has no texture\n",u->sprt1);
			}
			tex_t*const t0=getent(&texes,u->sprt0);
			if(t0){
				t0->tex=nsldrs%10+TEX_0;
			}else{
				fprintf(stderr,"WARNING: entity %u has no texture\n",u->sprt0);
			}
			clkbl_t*const clkbl=getent(&clkbls,eid);
			if(clkbl){
				clkbl->hdn=0;
			}else{
				fprintf(stderr,"WARNING: entity %u has no clkbl\n",eid);
			}
		}else{
			u->flags&=~UFLAGS_VIS;
			tex_t*const t=getent(&texes,eid);
			if(t){
				t->tex=TEX_NULL;
			}else{
				fprintf(stderr,"WARNING: entity %u has no texture\n",eid);
			}
			tex_t*const t1=getent(&texes,u->sprt1);
			if(t1){
				t1->tex=TEX_NULL;
			}else{
				fprintf(stderr,"WARNING: entity %u has no texture\n",u->sprt1);
			}
			tex_t*const t0=getent(&texes,u->sprt0);
			if(t0){
				t0->tex=TEX_NULL;
			}else{
				fprintf(stderr,"WARNING: entity %u has no texture\n",u->sprt0);
			}
			clkbl_t*const clkbl=getent(&clkbls,eid);
			if(clkbl){
				clkbl->hdn=1;
			}else{
				fprintf(stderr,"WARNING: entity %u has no clkbl\n",eid);
			}
		}
	}
}
int8_t unit_move(const uint32_t eid,const float x,const float y){
	if(win_frftbtn){
		return E_SUCC;
	}
	pos_t*const p=getent(&poses,eid);
	if(!p){
		fprintf(stderr,"ERROR: entity %u has no position\n",eid);
		return E_NO_ENT;
	}
	udata_t*const u=getent(&udatas,eid);
	if(!u){
		fprintf(stderr,"ERROR: entity %u has no udata\n",eid);
		return E_NO_ENT;
	}
	const float dx=x-p->x;
	const float dy=y-p->y;
	if(sqrtf(dx*dx+dy*dy)>u->spd*u->morale){
		return E_SUCC;
	}
	p->x=x;
	p->y=y;
	const int8_t err=unit_deselct(eid);
	if(err){
		fprintf(
			stderr,
			"WARNING: failed to deselect entity %u with error code %hhu\n",
			eid,
			err
		);
	}
	const int8_t flags=u->flags|UFLAGS_ACTED;
	u->flags=flags;
	tex_t*const t=getent(&texes,eid);
	if(t){
		t->tex=getex(u);
	}else{
		fprintf(stderr,"WARNING: entity %u has no texture\n",eid);
	}
	net_move(eid,x,y);
	unit_chklos();
	return E_SUCC;
}
int8_t unit_fire(const uint32_t trgt){
	if(win_frftbtn){
		return E_SUCC;
	}
	const uint32_t shtr=unit_sel;
	if(!shtr){
		return E_SUCC;
	}
	udata_t*const su=getent(&udatas,shtr);
	if(!su){
		fprintf(stderr,"ERROR: entity %u has no udata\n",shtr);
		return E_NO_ENT;
	}
	udata_t*const tu=getent(&udatas,trgt);
	if(!tu){
		fprintf(stderr,"ERROR: entity %u has no udata\n",trgt);
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
	double frarmcvr,fragcvr,hecvr;
	if(hshmp_empty(&hdgs)){
		frarmcvr=1;
		fragcvr=1;
		hecvr=1;
	}else{
		frarmcvr=.3;
		fragcvr=FRGCVR;
		hecvr=HECVR;
	}
	const posn_t sp=getposn(shtr,&err);
	inhdgs(&hdgs,sp);
	const bool unblkd=los(sp,tp,&hdgs);
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
	const uint64_t nts=tu->sldrs.nels;
	const double ntsd=nts;
	for(
			const sldr_t*si=su->sldrs.buf,*const __restrict end=si+su->sldrs.nels;
			si<end;
			si++){
		double ek=-0.1;
		const wpn_t*bw;
		for(const uint64_t*wi=si->wpns,*const __restrict wend=wi+si->nwpns;wi<wend;wi++){
			const wpn_t*const w=unit_wpns+*wi;
			const float rng=w->base.rng;
			const int8_t flags=w->base.flags;
			if(d>rng||(!unblkd&&(flags&WFLAGS_DRCT))){
				continue;
			}
			const double dor=d/rng;
			double prof;
			if(flags&WFLAGS_CREW){
				switch(si->role){
					case SR_MGNR:
					case SR_GNR:
						prof=1;
						break;
					case SR_ASTMGNR:
					case SR_ASTGNR:
						prof=.8;
						break;
					default:
						prof=.6;
				}
			}else{
				prof=1;
			}
			double e;
			switch(w->base.type){
				case WT_FRARM:
					const double a=w->frarm.acc*smrl*((double)1-dor*dor)*frarmcvr*prof;
					const uint32_t r=w->base.rpm;
					e=a*r;
					break;
				case WT_EXPLSV:
					if(d<w->explsv.minrng){
						continue;
					}
					const double ed=w->explsv.sprd*dor/prof;
					const double edp2=ed*ed;
					const double cosine=1.0-INFWDTH*INFWDTH/(2*edp2);
					const double angl=acos(cosine);
					const double bp=angl/(2.0*M_PI);
					const uint64_t rpm=(double)w->base.rpm*prof;
					e=bp*rpm*(w->explsv.he*hecvr+w->explsv.nfrags*FRGHITPRB*ntsd*fragcvr);
					break;
				default:
					fprintf(stderr,"WARNING: weapon has invalid type %hhu\n",w->base.type);
					continue;
			}
			if(e>ek){
				ek=e;
				bw=w;
				fired=1;
			}
		}
		if(ek<0){
			continue;
		}
		switch(bw->base.type){
			case WT_FRARM:
				const double dor=d/bw->base.rng;
				double prof;
				if(bw->base.flags&WFLAGS_CREW){
					switch(si->role){
						case SR_MGNR:
							prof=1;
							break;
						case SR_ASTMGNR:
							prof=.8;
							break;
						default:
							prof=.6;
					}
				}else{
					prof=1;
				}
				const double acc=bw->frarm.acc*smrl*((double)1-dor*dor)*frarmcvr*prof;
				const uint16_t rpm=bw->base.rpm;
				mrlchng-=rpm*.0005;
				const uint32_t acci=acc*RAND_MAX;
				for(uint16_t _=rpm;_;_--){
					if(rand()<=acci){
						hits++;
						mrlchng-=.05;
					}
				}
				break;
			case WT_EXPLSV:
				hitpos(bw,sp.x,sp.y,tp.x,tp.y);
				break;
		}
	}
	if(!fired){
		return E_SUCC;
	}
	tex_t*const t=getent(&texes,shtr);
	if(t){
		t->tex=getex(su);
	}else{
		fprintf(stderr,"WARNING: entity %u has no texture\n",shtr);
	}
	dmg(tu,hits,mrlchng);
	err=unit_deselct(shtr);
	if(err){
		fprintf(
			stderr,
			"ERROR: failed to deselect entity %u with error code %hhd\n",
			shtr,
			err
		);
		return err;
	}
	return E_SUCC;
}
int8_t unit_shell(const uint32_t eid,const float x,const float y){
	int8_t err;
	const posn_t posn=getposn(eid,&err);
	if(err){
		fprintf(stderr,"ERROR: entity %u has no position\n",eid);
		return E_NO_ENT;
	}
	const float dx=x-posn.x;
	const float dy=y-posn.y;
	const float d=sqrtf(dx*dx+dy*dy);
	udata_t*const u=getent(&udatas,eid);
	if(!u){
		fprintf(stderr,"ERROR: entity %u has no udata\n",eid);
		return E_NO_ENT;
	}
	bool fired=0;
	for(const sldr_t*si=u->sldrs.buf,*const __restrict end=si+u->sldrs.nels;si<end;si++){
		double ek=-.1;
		const wpn_t*bw=NULL;
		for(const uint64_t*wi=si->wpns,*const __restrict end=wi+si->nwpns;wi<end;wi++){
			const wpn_t*const w=unit_wpns+*wi;
			if(w->base.type!=WT_EXPLSV||d>w->base.rng||d<w->explsv.minrng){
				continue;
			}
			const double e=w->base.rpm*(w->explsv.he+w->explsv.nfrags*FRGHITPRB);
			if(e>ek){
				ek=e;
				bw=w;
			}
		}
		if(bw){
			fired=1;
			hitpos(bw,posn.x,posn.y,x,y);
		}
	}
	if(fired){
		u->flags|=UFLAGS_ACTED;
		unit_deselct(eid);
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
				if(flags&UFLAGS_SEL){
					unit_deselct(i->eid);
				}else{
					tex_t*const tex=getent(&texes,i->eid);
					if(tex){
						tex->tex=getex(i);
					}else{
						fprintf(stderr,"WARNING: entity %u has no texture\n",i->eid);
						continue;
					}
				}
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
		fprintf(stderr,"WARNING: entity %u has no texture\n",menu_nxtrn);
	}
	return E_SUCC;
}
int8_t unit_deselct(const uint32_t eid){
	if(win_frftbtn){
		return E_SUCC;
	}
	tex_t*const t=getent(&texes,eid);
	if(!t){
		fprintf(stderr,"ERROR: entity %u has no texture\n",eid);
		return E_NO_ENT;
	}
	udata_t*const u=getent(&udatas,eid);
	if(!u){
		fprintf(stderr,"ERROR entity %u has no udata\n",eid);
		return E_NO_ENT;
	}
	const int8_t flags=u->flags&~UFLAGS_SEL;
	u->flags=flags;
	t->tex=getex(u);
	col_t*const pcol=getent(&cols,unit_panel);
	if(pcol){
		pcol->a=0;
	}else{
		fprintf(stderr,"WARNING: entity %u has no color\n",unit_panel);
	}
	col_t*const mvrcol=getent(&cols,u->mvr);
	if(mvrcol){
		mvrcol->a=0;
	}else{
		fprintf(stderr,"WARNING: entity %u has no color\n",u->mvr);
	}
	const uint32_t*const rngs=u->rngs;
	for(const uint32_t*i=rngs,*const __restrict end=i+u->nrngs;i<end;i++){
		delent(*i);
	}
	free((void*)rngs);
	u->rngs=NULL;
	u->nrngs=0;
	col_t*const arrcol=getent(&cols,u->arr);
	if(arrcol){
		arrcol->a=0;
	}else{
		fprintf(stderr,"WARNING: entity %u has no color\n",u->arr);
	}
	setinvistex(u,0);
	for(const uint32_t*i=unit_chrs.buf,*const __restrict end=i+unit_chrs.nels;i<end;i++){
		delent(*i);
	}
	unit_chrs.nels=0;
	const uint64_t naoes=unit_aoes.nels;
	if(naoes){
		const uint32_t*const aoeb=unit_aoes.buf;
		for(const uint32_t*i=aoeb,*const __restrict end=aoeb+naoes;i<end;i++){
			delent(*i);
		}
		free((void*)aoeb);
		unit_aoes.nels=0;
	}
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
		r->r=u->spd*mrl;
	}else{
		fprintf(stderr,"WARNING: entity %u has no radius\n",u->mvr);
	}
}
static int8_t tglsel(const uint32_t eid){
	if(win_frftbtn){
		return E_SUCC;
	}
	const udata_t*const u=getent(&udatas,eid);
	if(!u){
		fprintf(stderr,"ERROR: entity %u has no udata\n",eid);
		return E_NO_ENT;
	}
	if(u->flags&UFLAGS_SEL){
		return unit_deselct(eid);
	}
	return selct(eid);
}
static int8_t selct(const uint32_t eid){
	if(unit_sel){
		unit_deselct(unit_sel);
	}
	if(!unit_pltrn){
		return E_SUCC;
	}
	udata_t*const u=getent(&udatas,eid);
	if(!u){
		fprintf(stderr,"ERROR entity %u has no udata\n",eid);
		return E_NO_ENT;
	}
	const int8_t flags=u->flags|UFLAGS_SEL;
	if(flags&UFLAGS_ACTED||(flags&UFLAGS_ALLIED)!=unit_allied){
		return E_SUCC;
	}
	u->flags=flags;
	tex_t*const t=getent(&texes,eid);
	if(t){
		t->tex=getex(u);
	}else{
		fprintf(stderr,"WARNING: entity %u has no texture\n",eid);
	}
	const uint32_t panl=unit_panel;
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
				len+=strlen(unit_wpns[*wi].base.name);
			}
			if(len>longest){
				longest=len;
			}
		}
		pdim->w=(float)longest*PNLCHRHGHT;
		pdim->h=(float)nsldrs*PNLCHRHGHT*2;
	}else{
		fprintf(stderr,"WARNING: entity %u has no dimensions\n",panl);
	}
	col_t*const pcol=getent(&cols,panl);
	if(pcol){
		pcol->a=1;
	}else{
		fprintf(stderr,"WARNING: entity %u has no color\n",panl);
	}
	float y=((float)nsldrs-1)*PNLCHRHGHT+PNLCHRHGHT/2;
	for(const sldr_t*si=sldrs;si<send;si++){
		float x=rndrstr(" - ",rndrstr(rnames[si->role],PNLCHRHGHT/4,y),y);
		for(const uint64_t*wi=si->wpns,*const __restrict lst=wi+si->nwpns-1;wi<=lst;wi++){
			x=rndrstr(unit_wpns[*wi].base.name,x,y);
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
		fprintf(stderr,"WARNING: entity %u has no color\n",u->mvr);
	}
	lnklst_t*map[8]={};
	const hshmp_t rngst={
		.map=map,
		.nbkts=8,
		.hshfnc=ret,
	};
	for(const sldr_t*si=sldrs;si<send;si++){
		for(const uint64_t*wi=si->wpns,*const __restrict wend=wi+si->nwpns;wi<wend;wi++){
			const wpn_t*const w=unit_wpns+*wi;
			const double rng=w->base.rng;
			hshmp_addifabs(&rngst,*((int64_t*)&rng));
			if(w->base.type==WT_EXPLSV){
				const double mrng=w->explsv.minrng;
				hshmp_addifabs(&rngst,*((int64_t*)&mrng));
			}
		}
	}
	arrlst_t rngs={
		.buf=malloc(32),
		.bs=16,
		.es=4,
	};
	uint32_t neid=neweid;
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
	setinvistex(u,1);
	win_clkoff=(int8_t(*)(int64_t,float,float))unit_deselct;
	win_rclk=(int8_t(*)(int64_t,float,float))unit_move;
	win_clkoffparam=eid;
	win_rclkparam=eid;
	unit_sel=eid;
	return E_SUCC;
}
static float rndrstr(const char*str,float x,const float y){
	uint32_t eid=neweid;
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
static int8_t setinvistex(const udata_t*const u,const bool shade){
	int8_t err;
	const posn_t posn=getposn(u->eid,&err);
	if(err){
		fprintf(stderr,"ERROR: entity %u has no position\n",u->eid);
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
		const uint32_t eid=u->eid;
		const int8_t flags=u->flags;
		if(flags&UFLAGS_VIS&&!hshmp_in(&vsbl,eid)){
			tex_t*const tex=getent(&texes,eid);
			if(tex){
				tex->tex=getex(u)+(TEX_AXISINFACTD-TEX_AXISINF)*shade;
			}else{
				fprintf(stderr,"WARNING: entity %u has no texture\n",eid);
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
		const uint32_t eid=(*i)->eid;
		int8_t err;
		const posn_t fp=getposn(eid,&err);
		if(err){
			fprintf(stderr,"WARNING: entity %u has no position\n",eid);
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
			const uint32_t h=i->eid;
			const line_t*const ln=getent(&lines,h);
			if(!ln){
				fprintf(stderr,"WANRING: entity %u has no line\n",h);
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
			const uint32_t h=i->eid;
			if(hshmp_in(hdgs,h)){
				continue;
			}
			const line_t*const ln=getent(&lines,h);
			if(!ln){
				fprintf(stderr,"WARNING: entity %u has no line\n",h);
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
		const uint32_t h=i->eid;
		const line_t*const l=getent(&lines,h);
		if(!l){
			fprintf(stderr,"WARNING: entity %u has no line\n",i->eid);
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
static uint64_t getex(const udata_t*const u){
	uint64_t tex=u->type*(TEX_AXISMG-TEX_AXISINF)+TEX_AXISINF;
	const int8_t flags=u->flags;
	const bool ald=flags&UFLAGS_ALLIED;
	if(flags&UFLAGS_SEL){
		tex+=TEX_AXISINFSEL-TEX_AXISINF;
	}else if(flags&UFLAGS_ACTED){
		tex+=TEX_AXISINFACTD-TEX_AXISINF;
	}
	if(ald){
		tex+=TEX_ALLIEDINF-TEX_AXISINF;
	}
	return tex;
}
static void hitpos(
		const wpn_t*const wpn,
		const double wx,
		const double wy,
		const double tx,
		const double ty){
	const double wtotdx=wx-tx;
	const double wtotdy=wy-ty;
	const double wtotdp2=wtotdx*wtotdx+wtotdy*wtotdy;
	const double wtotd=sqrt(wtotdp2);
	const uint64_t nunits=udatas.nels;
	uint8_t dmgs[nunits]={};
	double mrlchngs[nunits]={};
	udata_t*const units=udatas.buf;
	for(uint32_t _=wpn->base.rpm;_;_--){
		const double dist=wpn->explsv.sprd*wtotd/wpn->base.rng*rand()/(float)RAND_MAX;
		const double angl=(float)RAND_MAX*rand()/(2.0*M_PI);
		const double ex=tx+cos(angl)*dist;
		const double ey=ty+sin(angl)*dist;
		uint8_t*di=dmgs;
		double*mi=mrlchngs;
		for(
				const udata_t*ui=units,*const __restrict end=units+nunits;
				ui<end;
				ui++,di++,mi++
				){
			int8_t err;
			const posn_t up=getposn(ui->eid,&err);
			if(err){
				fprintf(stderr,"WARNING: entity %u has no posistion\n",ui->eid);
				continue;
			}
			const double udx=up.x-ex;
			const double udy=up.y-ey;
			const double udp2=udx*udx+udy*udy;
			const double ud=sqrt(udp2);
			const double he=wpn->explsv.he;
			if(ud>he*.01){
				continue;
			}
			const double bp=acos(1.0-INFWDTH*INFWDTH/(2.0*udp2))/(2.0*M_PI);
			lnklst_t*map[8]={};
			const hshmp_t hdgs={
				.map=map,
				.nbkts=8,
				.hshfnc=ret,
			};
			inhdgs(&hdgs,up);
			double fragcvr,hecvr;
			if(hshmp_empty(&hdgs)){
				fragcvr=1;
				hecvr=1;
			}else{
				fragcvr=FRGCVR;
				hecvr=HECVR;
			}
			const double ehk=bp*he*hecvr;
			uint32_t kls=ehk;
			kls+=rand()<=(ehk-kls)*(double)RAND_MAX;
			const double nsldrs=ui->sldrs.nels;
			for(uint32_t _=bp*wpn->explsv.nfrags;_;_--){
				kls+=(double)rand()<=(double)RAND_MAX*FRGHITPRB*fragcvr*nsldrs;
			}
			*di+=kls;
			*mi-=he/sqrt(udp2)/10;
		}
	}
	udata_t*ui=units;
	const double*mi=mrlchngs;
	for(const uint8_t*di=dmgs,*const __restrict end=dmgs+nunits;di<end;di++,ui++,mi++){
		dmg(ui,*di,*mi);
	}
}
static void dmg(udata_t*const u,const uint8_t hits,double mrlchng){
	if(hits){
		if(hits>=u->sldrs.nels){
			const uint32_t eid=u->eid;
			delent(eid);
			net_dstr(eid);
		}else{
			uint64_t kld[hits];
			uint64_t*i=kld;
			arrlst_t*const sldrs=&u->sldrs;
			for(uint8_t _=hits;_;_--,i++){
				const uint64_t s=(uint64_t)rand()%sldrs->nels;
				*i=s;
				arrlst_del(sldrs,s);
			}
			if(u->flags&UFLAGS_VIS){
				const uint64_t nsldrs=sldrs->nels;
				tex_t*const tex1=getent(&texes,u->sprt1);
				if(tex1){
					tex1->tex=nsldrs/10+TEX_0;
				}else{
					fprintf(stderr,"WARNING: entity %u has no texture\n",u->sprt1);
				}
				tex_t*const tex0=getent(&texes,u->sprt0);
				if(tex0){
					tex0->tex=nsldrs%10+TEX_0;
				}else{
					fprintf(stderr,"WARNING: entity %u has no texture\n",u->sprt0);
				}
			}
			mrlchng-=hits*.1;
			unit_chmrl(u,mrlchng);
			net_dmg(u->eid,kld,hits,mrlchng);
		}
	}
}
static void delunit(const udata_t*const u){
	delent(u->arr);
	delent(u->mvr);
	delent(u->sprt1);
	delent(u->sprt0);
	const uint32_t*rngs=u->rngs;
	if(rngs){
		for(const uint32_t*i=rngs,*const __restrict end=rngs+u->nrngs;i<end;i++){
			delent(*i);
		}
		free((void*)rngs);
	}
	const sldr_t*const sldrs=u->sldrs.buf;
	for(const sldr_t*i=sldrs,*const __restrict end=sldrs+u->sldrs.nels;i<end;i++){
		free(i->wpns);
	}
	free((void*)sldrs);
	if(u->eid==unit_sel){
		for(const uint32_t*i=unit_chrs.buf,*const __restrict end=i+unit_chrs.nels;i<end;i++)
		{
			delent(*i);
		}
		col_t*const col=getent(&cols,unit_panel);
		if(col){
			col->a=0;
		}else{
			fprintf(stderr,"WARNING: entity %u has no color\n",unit_panel);
		}
	}
}
static uint64_t ret(const int64_t x){
	return x;
}
