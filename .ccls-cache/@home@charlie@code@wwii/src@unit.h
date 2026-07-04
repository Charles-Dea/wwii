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
extern uint16_t unit_sel;
extern bool unit_allied;
extern bool unit_pltrn;
void mkunit(float,float,bool);
void unit_chklos();
int8_t unit_move(uint16_t,float,float);
int8_t unit_fire(uint16_t);
int8_t unit_deselct(uint16_t);
int8_t unit_nxtrn(bool);
void unit_chmrl(udata_t*,double);
#endif
