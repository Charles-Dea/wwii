#ifndef GRPHCS_H
#define GRPHCS_H
#include"win.h"
#define GRPHCS_CAMSPD .1
enum{
	TEX_NULL,
	TEX_ALLIEDINF,
	TEX_AXISINF,
	TEX_ALLIEDINFSEL,
	TEX_AXISINFSEL,
	TEX_ALLIEDINFACTD,
	TEX_AXISINFACTD,
	TEX_USFLAG,
	TEX_GERMFLAG,
	TEX_HOST,
	TEX_JOIN,
	TEX_WTNG,
	TEX_AMRCN,
	TEX_GRMN,
	TEX_VCTRY,
	TEX_0,
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_4,
	TEX_5,
	TEX_6,
	TEX_7,
	TEX_8,
	TEX_9,
	TEX_DOT,
	TEX_MAX
};
extern double grphcs_camx,grphcs_camy,grphcs_zoom;
void grphcs_init(void);
void grphcs_mvcam(GLFWwindow*__restrict);
void grphcs_draw(GLFWwindow*__restrict);
void grphcs_term(void);
#endif
