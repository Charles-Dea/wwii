#ifndef STRUCTS_H
#define STRUCTS_H
#include<stdbool.h>
#include<stdint.h>
typedef struct{
	void*buf;
	uint64_t nels,bs,es;
}arrlst_t;
struct lnklst_t{
	int64_t val;
	struct lnklst_t*nxt;
};
typedef struct lnklst_t lnklst_t;
typedef struct{
	lnklst_t**map;
	uint64_t nbkts;
	uint64_t(*hshfnc)(int64_t);
}hshmp_t;
void arrlst_add(arrlst_t*__restrict,const void*__restrict);
int8_t arrlst_insrt(arrlst_t*__restrict,const void*__restrict,uint64_t);
int8_t arrlst_del(arrlst_t*__restrict,uint64_t);
lnklst_t*lnklst_appndfrnt(lnklst_t*,int64_t);
bool lnklst_in(const lnklst_t*__restrict,int64_t);
void hshmp_add(const hshmp_t*__restrict,int64_t);
bool hshmp_in(const hshmp_t*__restrict,int64_t);
#endif
