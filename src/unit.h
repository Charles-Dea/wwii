#ifndef UNIT_H
#define UNIT_H
#include<stdbool.h>
#include<stdint.h>
#include"main.h"
enum{
	UFLAGS_ALLIED=0b1,
	UFLAGS_SEL=0b10,
	UFLAGS_ACTED=0b100,
	UFLAGS_VIS=0b1000,
};
enum{
	UT_INF,
	UT_MG,
	UT_MRTR,
};
enum{
	WPN_M1,
	WPN_M1CRBN,
	WPN_BAR,
	WPN_M1911,
	WPN_M1919,
	WPN_M7,
	WPN_M2,
	WPN_KAR98,
	WPN_MG42,
	WPN_MP40,
	WPN_P38,
	WPN_MG42TRI,
	WPN_GRW34,
};
enum{
	WT_FRARM,
	WT_EXPLSV,
};
enum{
	WFLAGS_CREW=0b1,
	WFLAGS_DRCT=0b10,
};
enum{
	SR_SQUADLEAD,
	SR_LIEUTENANT,
	SR_MGNR,
	SR_ASTMGNR,
	SR_RFLMAN,
	SR_AMBRER,
	SR_GRNDR,
	SR_GNR,
	SR_ASTGNR,
};
typedef struct{
	const char*name;
	float rng;
	uint16_t rpm;
	uint8_t type;
	int8_t flags;
}wpnbase_t;
typedef struct{
	wpnbase_t base;
	double acc;
	int8_t pad[8];
}frarm_t;
typedef struct{
	wpnbase_t base;
	double sprd,he,minrng;
	uint16_t nfrags;
	int8_t pad[22];
}explsv_t;
typedef union{
	wpnbase_t base;
	frarm_t frarm;
	explsv_t explsv;
}wpn_t;
extern uint32_t unit_sel;
extern bool unit_allied;
extern bool unit_pltrn;
extern uint32_t unit_panel;
extern arrlst_t unit_chrs;
extern arrlst_t unit_aoes;
extern const wpn_t unit_wpns[];
int8_t mkunit(float,float,bool,uint8_t);
void unit_chklos();
int8_t unit_move(uint32_t,float,float);
int8_t unit_fire(uint32_t);
int8_t unit_shell(uint32_t,float,float);
int8_t unit_deselct(uint32_t);
int8_t unit_nxtrn(bool);
void unit_chmrl(udata_t*,double);
#endif
