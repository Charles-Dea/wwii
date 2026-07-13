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
	WPN_M1,
	WPN_BAR,
	WPN_KAR98,
	WPN_MG42,
	WPN_MP40,
	WPN_P38,
};
enum{
	WT_MG,
	WT_SMG,
	WT_RFL,
	WT_PSTL,
};
enum{
	SR_SQUADLEAD,
	SR_LIEUTENANT,
	SR_MGNR,
	SR_ASTMGNR,
	SR_RFLMAN,
};
typedef struct{
	const char*name;
	double acc;
	float rng;
	uint16_t rpm;
	uint8_t type;
	int8_t pad[9];
}wpn_t;
extern uint16_t unit_sel;
extern bool unit_allied;
extern bool unit_pltrn;
extern uint16_t unit_panel;
extern arrlst_t unit_chrs;
extern const wpn_t unit_wpns[];
void mkunit(float,float,bool);
void unit_chklos();
int8_t unit_move(uint16_t,float,float);
int8_t unit_fire(uint16_t);
int8_t unit_deselct(uint16_t);
int8_t unit_nxtrn(bool);
void unit_chmrl(udata_t*,double);
#endif
