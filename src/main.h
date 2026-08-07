#ifndef MAIN_H
#define MAIN_H
#include<stdbool.h>
#include"structs.h"
#define LOWSTPRNTBL 0x21
#define HGHSTPRNTBL 0x7e
enum{
	E_SUCC=0,
	E_GLFW_INIT_FAIL=-1,
	E_CREAT_WIN_FAIL=-2,
	E_FOPEN_FAIL=-3,
	E_SHAD_COMP_FAIL=-4,
	E_STBI_FAIL=-5,
	E_NO_ENT=-6,
	E_OUT_OF_RANGE=-7,
	E_STRTHST_FAIL=-8,
	E_CONCT_FAIL=-9,
	E_DSCNCT_FAIL=-10,
	E_INVALD_UT=-11,
	E_NO_ACK=-12,
};
enum{
	XANCR_CENT,
	XANCR_LEFT,
	XANCR_RGHT,
};
enum{
	YANCR_CENT,
	YANCR_TOP,
	YANCR_BOT,
};
typedef struct{
	float x,y,z;
}posn_t;
typedef struct{
	uint64_t role;
	uint64_t*wpns;
	uint64_t nwpns;
	int8_t pad[8];
}sldr_t;
typedef struct breid_t{
	uint32_t eid;
	int8_t pad[4];
	void(*free)(const struct breid_t*);
}breid_t;
typedef breid_t arrow_t;
typedef breid_t rect_t;
typedef breid_t hedge_t;
typedef struct pos_t{
	uint32_t eid;
	float x;
	void(*free)(const struct pos_t*);
	float y,z;
	int8_t pad1[8];
}pos_t;
typedef struct dim_t{
	uint32_t eid;
	float w;
	void(*free)(const struct dim_t*);
	float h;
	int8_t pad1[4];
}dim_t;
typedef struct tex_t{
	uint32_t eid;
	int8_t pad0[4];
	void(*free)(const struct tex_t*);
	uint64_t tex;
	int8_t pad1[8];
}tex_t;
typedef struct clkbl_t{
	uint32_t eid;
	bool hdn;
	int8_t pad[3];
	void(*free)(const struct clkbl_t*);
	int8_t(*func)(int64_t);
	int64_t param;
}clkbl_t;
typedef clkbl_t rclkbl_t;
typedef struct udata_t{
	uint32_t eid,arr;
	void(*free)(const struct udata_t*);
	arrlst_t sldrs;
	double morale;
	uint32_t*rngs;
	uint64_t nrngs;
	uint32_t*chars;
	uint64_t nchars;
	float spd;
	uint32_t sprt1,sprt0,mvr;
	uint8_t type;
	int8_t flags;
	int8_t pad[22];
}udata_t;
typedef struct relpos_t{
	uint32_t eid,prnt;
	void(*free)(const struct relpos_t*);
	float x,y,z;
	int8_t pad[4];
}relpos_t;
typedef struct ring_t{
	uint32_t eid;
	float r;
	void(*free)(const struct ring_t*);
}ring_t;
typedef struct col_t{
	uint32_t eid;
	float r;
	void(*free)(const struct col_t*);
	float g,b,a;
	int8_t pad1[4];
}col_t;
typedef struct txtbox_t{
	uint32_t eid,crsr;
	void(*free)(const struct txtbox_t*);
	arrlst_t str,chrs;
	bool*acs;
	int8_t(*func)(arrlst_t*);
	uint64_t cp,ml;
	int8_t pad1[16];
}txtbox_t;
typedef struct flag_t{
	uint32_t eid;
	bool ald;
	int8_t pad0[3];
	void(*free)(const struct flag_t*);
}flag_t;
typedef struct line_t{
	uint32_t eid;
	float x0;
	void(*free)(const struct line_t*);
	float y0,z0,x1,y1,z1,width;
	int8_t pad1[24];
}line_t;
typedef struct scrnpos_t{
	uint32_t eid;
	float x;
	void(*free)(const struct scrnpos_t*);
	float y,z;
	uint8_t xancr,yancr;
	int8_t pad[6];
}scrnpos_t;
typedef struct crsrpos_t{
	uint32_t eid;
	float z;
	void(*free)(const struct crsrpos_t*);
}crsrpos_t;
extern double scrnwdth,scrnhght;
extern uint32_t neweid;
extern bool running;
extern uint8_t reqcptr;
extern arrlst_t poses;
extern arrlst_t dims;
extern arrlst_t texes;
extern arrlst_t clkbls;
extern arrlst_t rclkbls;
extern arrlst_t udatas;
extern arrlst_t relposes;
extern arrlst_t rings;
extern arrlst_t cols;
extern arrlst_t arrows;
extern arrlst_t rects;
extern arrlst_t txtboxes;
extern arrlst_t flags;
extern arrlst_t lines;
extern arrlst_t hedges;
extern arrlst_t scrnposes;
extern arrlst_t crsrposes;
posn_t getposn(uint32_t,int8_t*__restrict);
void delent(uint32_t);
void*getent(const arrlst_t*,uint32_t);
uint64_t getenti(const arrlst_t*,uint32_t);
void termscene(void);
#endif
