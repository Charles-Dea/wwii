#include<math.h>
#include<stdio.h>
#include"grphcs.h"
#include"unit.h"
#include"flag.h"
#define FLGHGHT .1
#define FLGCOL .75
void mkflag(const float x,const float y,const float rad,const bool ald){
	const uint16_t eid=neweid;
	const pos_t pos={
		.eid=eid,
		.x=x,
		.y=y,
		.z=.9,
	};
	arrlst_add(&poses,&pos);
	const dim_t dim={
		.eid=eid,
		.w=FLGHGHT*(ald?1.9:10.0/6),
		.h=FLGHGHT,
	};
	arrlst_add(&dims,&dim);
	const tex_t tex={
		.eid=eid,
		.tex=ald?TEX_USFLAG:TEX_GERMFLAG,
	};
	arrlst_add(&texes,&tex);
	const ring_t ring={
		.eid=eid,
		.r=rad,
	};
	arrlst_add(&rings,&ring);
	const col_t col={
		.eid=eid,
		.r=ald?0:FLGCOL,
		.b=ald?FLGCOL:0,
		.a=1
	};
	arrlst_add(&cols,&col);
	const flag_t flag={
		.eid=eid,
		.ald=ald,
	};
	arrlst_add(&flags,&flag);
	neweid=eid+1;
}
void flag_update(){
	flag_t*fi=flags.buf;
	const flag_t*const __restrict fend=fi+flags.nels;
	for(;fi<fend;fi++){
		const uint16_t flag=fi->eid;
		int8_t err;
		const posn_t fp=getposn(flag,&err);
		if(err){
			fprintf(stderr,"WARNING: entity %hu has no position\n",flag);
			continue;
		}
		const ring_t*const ring=getent(&rings,flag);
		if(!ring){
			fprintf(stderr,"WARNING: entity %hu has no ring\n",flag);
			continue;
		}
		const float rad=ring->r;
		int8_t prsnc=0;
		for(const udata_t*ui=udatas.buf,*const __restrict uend=ui+udatas.nels;ui<uend;ui++){
			const bool ald=ui->flags&UFLAGS_ALLIED;
			if(ald){
				if(prsnc&1){
					continue;
				}
			}else if(prsnc&2){
				continue;
			}
			const posn_t up=getposn(ui->eid,&err);
			if(err){
				fprintf(stderr,"WARNING: entity %hu has no position\n",ui->eid);
				continue;
			}
			const float dx=up.x-fp.x;
			const float dy=up.y-fp.y;
			if(sqrtf(dx*dx+dy*dy)<rad){
				if(ald){
					prsnc|=1;
					if(prsnc&2){
						break;
					}
				}else{
					prsnc|=2;
					if(prsnc&1){
						break;
					}
				}
			}
		}
		if(fi->ald){
			if((prsnc&2)&&(~prsnc&1)){
				fi->ald=0;
				tex_t*const tex=getent(&texes,flag);
				if(tex){
					tex->tex=TEX_GERMFLAG;
				}else{
					fprintf(stderr,"WARNING: entity %hu has no texture\n",flag);
				}
				col_t*const col=getent(&cols,flag);
				if(col){
					col->r=FLGCOL;
					col->b=0;
				}else{
					fprintf(stderr,"WARNING: entity %hu has no color\n",flag);
				}
			}
		}else if((prsnc&1)&&(~prsnc&2)){
			fi->ald=1;
			tex_t*const tex=getent(&texes,flag);
			if(tex){
				tex->tex=TEX_USFLAG;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no texture\n",flag);
			}
			col_t*const col=getent(&cols,flag);
			if(col){
				col->r=0;
				col->b=FLGCOL;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no color\n",flag);
			}
		}
	}
}
uint8_t flag_cptrd(){
	uint8_t ret=0;
	const bool ald=unit_allied;
	for(const flag_t*i=flags.buf,*const __restrict end=i+flags.nels;i<end;i++){
		ret+=(~(ald^i->ald))&1;
	}
	return ret;
}
