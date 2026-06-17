#ifndef MAIN_H
#define MAIN_H
#include<stdbool.h>
#include"structs.h"
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
};
typedef struct{
	double acc;
	float rng;
	uint16_t rpm,sprt1,sprt0;
	uint8_t num;
	int8_t pad[7];
}wpn_t;
typedef struct breid_t{
	uint16_t eid;
	int8_t pad[6];
	void(*free)(const struct breid_t*);
}breid_t;
typedef breid_t arrow_t;
typedef breid_t rect_t;
typedef struct pos_t{
	uint16_t eid;
	int8_t pad0[2];
	float x;
	void(*free)(const struct pos_t*);
	float y,z;
	int8_t pad1[8];
}pos_t;
typedef struct dim_t{
	uint16_t eid;
	int8_t pad0[2];
	float w;
	void(*free)(const struct dim_t*);
	float h;
	int8_t pad1[4];
}dim_t;
typedef struct tex_t{
	uint16_t eid;
	int8_t pad0[6];
	void(*free)(const struct tex_t*);
	uint64_t tex;
	int8_t pad1[8];
}tex_t;
typedef struct clkbl_t{
	uint16_t eid;
	int8_t pad0[6];
	void(*free)(const struct clkbl_t*);
	int8_t(*func)(int64_t);
	int64_t param;
}clkbl_t;
typedef clkbl_t rclkbl_t;
typedef struct udata_t{
	uint16_t eid,arr;
	int8_t flags;
	int8_t pad0[3];
	void(*free)(const struct udata_t*);
	wpn_t*wpns;
	uint64_t nwpns;
	uint16_t*rings;
	uint64_t nrings;
	int8_t pad1[16];
}udata_t;
typedef struct relpos_t{
	uint16_t eid,prnt;
	float x;
	void(*free)(const struct relpos_t*);
	float y,z;
}relpos_t;
typedef struct ring_t{
	uint16_t eid;
	int8_t pad[2];
	float r;
	void(*free)(const struct ring_t*);
}ring_t;
typedef struct col_t{
	uint16_t eid;
	int8_t pad0[2];
	float r;
	void(*free)(const struct col_t*);
	float g,b,a;
	int8_t pad1[4];
}col_t;
typedef struct txtbox_t{
	uint16_t eid,crsr;
	char pad0[4];
	void(*free)(const struct txtbox_t*);
	arrlst_t str,chrs;
	bool*acs;
	int8_t(*func)(arrlst_t*);
	uint64_t cp,ml;
	char pad1[16];
}txtbox_t;
typedef struct{
	float x,y,z;
}posn_t;
extern double scrnwdth,scrnhght;
extern arrlst_t poses,dims,texes,clkbls,rclkbls,udatas,relposes,rings,cols,arrows,rects,txtboxes;
extern uint16_t neweid;
extern bool running;
posn_t getposn(uint16_t,int8_t*__restrict);
void delent(uint16_t);
void*getent(const arrlst_t*,uint16_t);
uint64_t getenti(const arrlst_t*,uint16_t);
void termscene();
#endif
