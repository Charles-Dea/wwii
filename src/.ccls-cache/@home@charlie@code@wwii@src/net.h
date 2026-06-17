#ifndef NET_H
#define NET_H
#include<stdint.h>
#include"structs.h"
void net_init();
int8_t net_strthst();
int8_t net_join(arrlst_t*__restrict);
void net_lstn();
void net_move(uint16_t,float,float);
void net_dstr(uint16_t);
void net_dmg(uint16_t,const uint8_t*__restrict,uint64_t);
void net_endtrn();
int8_t net_dscnct();
#endif
