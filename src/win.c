#include<stdio.h>
#include"grphcs.h"
#include"main.h"
#include"menu.h"
#include"net.h"
#include"unit.h"
#include"win.h"
#define TXTBXSTRT -.875
int8_t(*win_clkoff)(int64_t,float,float);
int64_t win_clkoffparam;
int8_t(*win_rclk)(int64_t,float,float);
int64_t win_rclkparam;
uint8_t win_inmode=INMODE_NORM;
uint16_t win_seltxtbox;
uint16_t win_frftbtn;
static void mousin(GLFWwindow*__restrict,int32_t,int32_t,int32_t);
static p2d_t glfw2ndc(p2d_t);
static p2d_t camadj(p2d_t);
static void keyin(GLFWwindow*__restrict,int32_t,int32_t,int32_t,int32_t);
static void scrollin(GLFWwindow*__restrict,double,double);
static void hndlerr(int32_t,const char*__restrict);
static void addchr(txtbox_t*,uint16_t,uint32_t,uint64_t,uint64_t);
static int8_t exitmode(uint16_t);
static int8_t fire(float,float);
static int8_t endgm(bool);
GLFWwindow*win_open(int8_t*__restrict err){
	if(!glfwInit()){
		*err=E_GLFW_INIT_FAIL;
		return NULL;
	}
	glfwSetErrorCallback(hndlerr);
	GLFWmonitor*const __restrict mon=glfwGetPrimaryMonitor();
	const GLFWvidmode*const __restrict mode=glfwGetVideoMode(mon);
	const int32_t w=mode->width;
	const int32_t h=mode->height;
	scrnwdth=w;
	scrnhght=h;
	GLFWwindow*const __restrict win
		=glfwCreateWindow(mode->width,mode->height,"WWII",mon,NULL);
	if(!win){
		*err=E_CREAT_WIN_FAIL;
		return NULL;
	}
	glfwMakeContextCurrent(win);
	glfwSetMouseButtonCallback(win,mousin);
	glfwSetKeyCallback(win,keyin);
	glfwSetScrollCallback(win,scrollin);
	return win;
}
p2d_t win_glfw2ndc(const p2d_t pos){
	return camadj(glfw2ndc(pos));
}
void win_close(GLFWwindow*const __restrict win){
	glfwDestroyWindow(win);
	glfwTerminate();
}
static void mousin(
		GLFWwindow*const __restrict win,
		const int32_t btn,
		const int32_t act,
		const int32_t mods
		){
	if(act==GLFW_RELEASE){
		p2d_t mp;
		glfwGetCursorPos(win,&mp.x,&mp.y);
		mp=glfw2ndc(mp);
		const p2d_t mpa=camadj(mp);
		if(btn==GLFW_MOUSE_BUTTON_LEFT){
			if(win_inmode==INMODE_NORM){
				const clkbl_t*i=clkbls.buf;
				const clkbl_t*const end=i+clkbls.nels;
				const clkbl_t*top=NULL;
				float topz=1;
				for(;i<end;i++){
					const uint16_t eid=i->eid;
					int8_t err;
					const posn_t p=getposn(eid,&err);
					double x,y;
					float z;
					p2d_t msps;
					if(err){
						const scrnpos_t*const sp=getent(&scrnposes,eid);
						if(sp){
							z=sp->z/2;
							if(z>topz){
								continue;
							}
							switch(sp->xancr){
								case XANCR_CENT:
									x=sp->x;
									break;
								case XANCR_LEFT:
									x=(double)sp->x-scrnwdth/scrnhght;
									break;
								case XANCR_RGHT:
									x=(double)sp->x+scrnwdth/scrnhght;
									break;
								default:
									fprintf(
										stderr,
										"WARNING: entity %hu has invalid xanchor %hhu",
										eid,
										sp->xancr
									);
									continue;
							}
							switch(sp->yancr){
								case YANCR_CENT:
									y=sp->y;
									break;
								case YANCR_TOP:
									y=(double)sp->y+1;
									break;
								case YANCR_BOT:
									y=(double)sp->y-1;
									break;
								default:
									fprintf(
										stderr,
										"WARNING: entity %hu has invalid yanchor %hhu",
										eid,
										sp->yancr
									);
									continue;
							}
							msps=mp;
						}else{
							fprintf(stderr,"WARNING: entity %hu has no position",eid);
							continue;
						}
					}else{
						z=p.z/2+.5;
						if(z>topz){
							continue;
						}
						x=p.x;
						y=p.y;
						msps=mpa;
					}
					const dim_t*const d=getent(&dims,eid);
					if(!d){
						fprintf(stderr,"WARNING: entity %hu has no dimensions\n",eid);
						continue;
					}
					const double hw=(double)d->w/2;
					const double hh=(double)d->h/2;
					if(msps.x>=x-hw&&msps.x<=x+hw&&msps.y>=y-hh&&msps.y<=y+hh){
						top=i;
						topz=z;
					}
				}
				if(top){
					const int8_t err=top->func(top->param);
					if(err){
						fprintf(
							stderr,
							"WARNING: func failed for clickable entity %hu with error code %hhd\n",
							top->eid,
							err
						);
					}else{
						return;
					}
				}
			}
			if(win_clkoff){
				const int8_t err=win_clkoff(win_clkoffparam,mpa.x,mpa.y);
				if(err){
					fprintf(stderr,"ERROR: win_clkoff failed with error code %hhd\n",err);
				}
			}
		}else if(btn==GLFW_MOUSE_BUTTON_RIGHT){
			const rclkbl_t*i=rclkbls.buf;
			const rclkbl_t*const __restrict end=i+rclkbls.nels;
			for(;i<end;i++){
				const uint16_t eid=i->eid;
				int8_t err;
				const posn_t p=getposn(eid,&err);
				if(err){
					fprintf(stderr,"WARNING: entity %hu has no position\n",eid);
					continue;
				}
				const dim_t*const d=getent(&dims,eid);
				if(!d){
					fprintf(stderr,"WARNING: entity %hu has no dimensions\n",eid);
					continue;
				}
				const double x=p.x;
				const double y=p.y;
				const double hw=(double)d->w/2;
				const double hh=(double)d->h/2;
				if(mpa.x>=x-hw&&mpa.x<=x+hw&&mpa.y>=y-hh&&mpa.y<=y+hh){
					const int8_t err=i->func(i->param);
					if(err){
						fprintf(
							stderr,
							"WARNING: func failed for clickable entity %hu with error code %hhd\n",
							eid,
							err
						);
					}else{
						return;
					}
				}
			}
			int8_t(*const rclk)(int64_t,float,float)=win_rclk;
			if(rclk){
				const int8_t err=rclk(win_rclkparam,mpa.x,mpa.y);
				if(err){
					fprintf(stderr,"ERROR: win_rclk failed with error code %hhd\n",err);
				}
			}
		}
	}
}
static p2d_t glfw2ndc(p2d_t pos){
	const double sh=scrnhght;
	const double hfsh=sh/2;
	pos.x=pos.x/hfsh-scrnwdth/sh;
	pos.y=-pos.y/hfsh+1;
	return pos;
}
static p2d_t camadj(p2d_t pos){
	const double zoom=grphcs_zoom;
	pos.x=pos.x/zoom+grphcs_camx;
	pos.y=pos.y/zoom+grphcs_camy;
	return pos;
}
static void keyin(
		GLFWwindow*const __restrict win,
		const int32_t key,
		const int32_t sc,
		const int32_t act,
		const int32_t mods
		){
	if(act==GLFW_PRESS){
		if(win_seltxtbox){
			const uint16_t stb=win_seltxtbox;
			txtbox_t*const t=getent(&txtboxes,stb);
			if(!t){
				fprintf(stderr,"ERROR: entity %hu has not txtbox\n",win_seltxtbox);
			}
			const uint64_t cp=t->cp;
			if(key==GLFW_KEY_LEFT&&cp){
				t->cp=cp-1;
				relpos_t*const rp=getent(&relposes,t->crsr);
				if(rp){
					rp->x-=.125;
				}else{
					fprintf(stderr,"WARNING: entity %hu has no position\n",t->crsr);
				}
			}else if(key==GLFW_KEY_RIGHT&&cp<t->str.nels){
				t->cp=cp+1;
				relpos_t*const rp=getent(&relposes,t->crsr);
				if(rp){
					rp->x+=.125;
				}else{
					fprintf(stderr,"WARNING: entity %hu has no position\n",t->crsr);
				}
			}else if(key==GLFW_KEY_BACKSPACE&&cp){
				const uint64_t ncp=cp-1;
				arrlst_del(&t->str,ncp);
				uint16_t*chrs=t->chrs.buf;
				delent(chrs[ncp]);
				arrlst_del(&t->chrs,ncp);
				if(ncp<t->chrs.nels){
					if(ncp){
						relpos_t*const rp=getent(&relposes,chrs[ncp]);
						if(rp){
							rp->prnt=chrs[ncp-1];
						}else{
							fprintf(stderr,"WARNING: entity %hu has no position\n",chrs[ncp]);
						}
					}else{
						relpos_t*const rp=getent(&relposes,*chrs);
						if(rp){
							rp->prnt=stb;
							rp->x=TXTBXSTRT;
							rp->z=-.5;
						}else{
							fprintf(stderr,"WARNING: entity %hu has no position\n",*chrs);
						}
					}
				}
				relpos_t*const rp=getent(&relposes,t->crsr);
				if(rp){
					rp->x-=.125;
				}else{
					fprintf(stderr,"WARNING: entity %hu has no position\n",t->crsr);
				}
				t->cp=ncp;
			}else if(key==GLFW_KEY_DELETE&&cp<=t->str.nels){
				arrlst_del(&t->str,cp);
				uint16_t*chrs=t->chrs.buf;
				delent(chrs[cp]);
				arrlst_del(&t->chrs,cp);
				if(cp<t->chrs.nels){
					if(cp){
						relpos_t*const rp=getent(&relposes,chrs[cp]);
						if(rp){
							rp->prnt=chrs[cp-1];
						}else{
							fprintf(stderr,"WARNING: entity %hu has no position\n",*chrs);
						}
					}else{
						relpos_t*const rp=getent(&relposes,*chrs);
						if(rp){
							rp->prnt=stb;
							rp->x=TXTBXSTRT;
							rp->z=-.5;
						}else{
							fprintf(stderr,"WARNING: entity %hu has no position\n",*chrs);
						}
					}
				}
			}else if(key>=GLFW_KEY_0&&key<=GLFW_KEY_9&&t->str.nels<t->ml){
				addchr(t,stb,key-GLFW_KEY_0+'0',key-GLFW_KEY_0+TEX_0,cp);
			}else if(key>='A'&&key<='Z'&&t->str.nels<t->ml){
				addchr(t,stb,key,key-'A'+TEX_a,cp);
			}else if(key==GLFW_KEY_PERIOD&&t->str.nels<t->ml){
				addchr(t,stb,'.',TEX_DOT,cp);
			}else if(key==GLFW_KEY_ENTER){
				const int8_t err=t->func(&t->str);
				if(err){
					fprintf(
						stderr,
						"WARNING: txtbox func in entity %hu failed with error %hhi\n",
						stb,
						err
					);
				}
			}else if(key==GLFW_KEY_ESCAPE){
				if(menu_scene==SCENE_MAINMEN){
					running=0;
				}else{
					menu_main();
				}
			}
		}else{
			switch(key){
				case'M':
					if(win_inmode==INMODE_MOVE){
						exitmode(unit_sel);
					}else if(unit_sel){
						win_inmode=INMODE_MOVE;
						win_clkoff=(int8_t(*)(int64_t,float,float))unit_move;
						win_clkoffparam=unit_sel;
						win_rclk=(int8_t(*)(int64_t,float,float))exitmode;
						const uint16_t sel=unit_sel;
						win_rclkparam=sel;
						const udata_t*const udata=getent(&udatas,sel);
						if(udata){
							col_t*const col=getent(&cols,udata->arr);
							if(col){
								col->r=0;
								col->g=0;
								col->b=1;
								col->a=1;
							}else{
								fprintf(stderr,"WARNING: entity %hu has no color\n",udata->arr);
							}
						}else{
							fprintf(stderr,"WARNING: entity %hu has no udata\n",unit_sel);
						}
					}
					return;
				case'F':
					if(win_inmode==INMODE_FIRE){
						exitmode(unit_sel);
					}else if(unit_sel){
						win_inmode=INMODE_FIRE;
						win_clkoff=(int8_t(*)(int64_t,float,float))fire;
						win_rclk=(int8_t(*)(int64_t,float,float))exitmode;
						const uint16_t sel=unit_sel;
						win_rclkparam=sel;
						const udata_t*const udata=getent(&udatas,sel);
						if(udata){
							col_t*const col=getent(&cols,udata->arr);
							if(col){
								col->r=1;
								col->g=0;
								col->b=0;
								col->a=1;
							}else{
								fprintf(stderr,"WARNING: entity %hu has no color\n",udata->arr);
							}
						}else{
							fprintf(stderr,"WARNING: entity %hu has no udata\n",unit_sel);
						}
					}
					return;
				case GLFW_KEY_ENTER:
					if(menu_scene==SCENE_GAME){
						unit_nxtrn(unit_allied);
					}
					return;
				case GLFW_KEY_ESCAPE:
					switch(menu_scene){
						case SCENE_MAINMEN:
							running=0;
							break;
						case SCENE_GAME:
							if(win_frftbtn){
								delent(win_frftbtn);
								win_frftbtn=0;
							}else{
								const uint16_t eid=neweid;
								const scrnpos_t pos={
									.eid=eid,
									.xancr=XANCR_CENT,
									.yancr=YANCR_CENT,
								};
								arrlst_add(&scrnposes,&pos);
								const dim_t dim={
									.eid=eid,
									.w=2,
									.h=.5,
								};
								arrlst_add(&dims,&dim);
								const tex_t tex={
									.eid=eid,
									.tex=TEX_FORFEIT,
								};
								arrlst_add(&texes,&tex);
								const clkbl_t clkbl={
									.eid=eid,
									.func=(int8_t(*)(int64_t))endgm,
									.param=!unit_allied,
								};
								arrlst_add(&clkbls,&clkbl);
								win_frftbtn=eid;
								neweid=eid+1;
							}
							break;
						default:
							menu_main();
							net_dscnct(0);
					}
					return;
			}
		}
	}
}
static void scrollin(GLFWwindow*const __restrict win,const double xo,const double yo){
	if(menu_scene==SCENE_GAME&&!win_frftbtn){
		double zoom=grphcs_zoom;
		zoom=zoom+yo*zoom*.1;
		if(zoom>0){
			grphcs_zoom=zoom;
		}else{
			grphcs_zoom=0;
		}
	}
}
static void hndlerr(const int32_t code,const char*const __restrict descript){
	fprintf(
		stderr,
		"ERROR: a glfw function has generated an error with code %d and description %s\n",
		code,
		descript
	);
}
static void addchr(
		txtbox_t*const tb,
		const uint16_t eid,
		const uint32_t c,
		const uint64_t t,
		const uint64_t cp
		){
	if(tb->acs[c]){
		const char c8=c;
		arrlst_insrt(&tb->str,&c8,cp);
		const uint16_t neid=neweid;
		arrlst_insrt(&tb->chrs,&neid,cp);
		const dim_t dim={
			.eid=neid,
			.w=.125,
			.h=.25,
		};
		arrlst_add(&dims,&dim);
		const tex_t tex={
			.eid=neid,
			.tex=t,
		};
		arrlst_add(&texes,&tex);
		const uint16_t*const chrs=tb->chrs.buf;
		if(cp){
			const relpos_t rp={
				.eid=neid,
				.prnt=chrs[cp-1],
				.x=.125,
			};
			arrlst_add(&relposes,&rp);
		}else{
			const relpos_t rp={
				.eid=neid,
				.prnt=eid,
				.x=-.875,
				.z=-.5,
			};
			arrlst_add(&relposes,&rp);
		}
		if(cp+1<tb->chrs.nels){
			relpos_t*const rp=getent(&relposes,chrs[cp+1]);
			if(rp){
				rp->prnt=neid;
				rp->z=0;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no position\n",chrs[cp+1]);
			}
		}
		neweid=neid+1;
		tb->cp=cp+1;
		relpos_t*const rp=getent(&relposes,tb->crsr);
		if(rp){
			rp->x+=.125;
		}else{
			fprintf(stderr,"WARNING: entity %hu has no position\n",tb->crsr);
		}
	}
}
static int8_t exitmode(const uint16_t eid){
	if(eid){
		win_clkoff=(int8_t(*)(int64_t,float,float))unit_deselct;
		win_rclk=(int8_t(*)(int64_t,float,float))unit_move;
		const udata_t*const udata=getent(&udatas,eid);
		if(udata){
			col_t*const col=getent(&cols,udata->arr);
			if(col){
				col->a=0;
			}else{
				fprintf(stderr,"WARNING: entity %hu has no color\n",udata->arr);
			}
		}else{
			fprintf(stderr,"WARNING: entity %hu has no color\n",eid);
		}
	}else{
		fprintf(stderr,"WARNING: win_inmode was set to %hhu despite no unit being selected\n",win_inmode);
		win_clkoff=NULL;
		win_rclk=NULL;
	}
	win_inmode=INMODE_NORM;
	return E_SUCC;
}
static int8_t fire(const float mx,const float my){
	for(const udata_t*i=udatas.buf,*const __restrict end=i+udatas.nels;i<end;i++){
		const uint16_t eid=i->eid;
		int8_t err;
		const posn_t p=getposn(eid,&err);
		if(err){
			fprintf(stderr,"WARNING: entity %hu has no position\n",eid);
			continue;
		}
		const dim_t*const d=getent(&dims,eid);
		if(!d){
			fprintf(stderr,"WARNING: entity %hu has no dimensions\n",eid);
			continue;
		}
		const float x=p.x;
		const float y=p.y;
		const float hw=d->w/2;
		const float hh=d->h/2;
		if(mx>=x-hw&&mx<=x+hw&&my>=y-hh&&my<=y+hh){
			return unit_fire(eid);
		}
	}
	return E_SUCC;
}
static int8_t endgm(const bool aldvctry){
	menu_endscrn(aldvctry);
	return net_dscnct(aldvctry);
}
