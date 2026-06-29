#include<malloc.h>
#include<string.h>
#include"main.h"
#include"structs.h"
static uint64_t hshmp_indx(const hshmp_t*__restrict,int64_t);
void arrlst_add(arrlst_t*const __restrict arr,const void*const __restrict new){
	const uint64_t els=arr->nels;
	const uint64_t ne=els+1;
	const uint64_t es=arr->es;
	if(ne*es>arr->bs){
		const uint64_t ns=arr->bs<<1;
		void*const __restrict tmp=malloc(ns);
		const void*const b=arr->buf;
		memcpy(tmp,b,els*es);
		free(b);
		arr->buf=tmp;
		arr->bs=ns;
	}
	memcpy(arr->buf+els*es,new,es);
	arr->nels=ne;
}
int8_t arrlst_insrt(
		arrlst_t*const __restrict arr,
		const void*const __restrict new,
		const uint64_t idx
		){
	const uint64_t els=arr->nels;
	if(idx>els){
		return E_OUT_OF_RANGE;
	}
	const uint64_t ne=els+1;
	const uint64_t es=arr->es;
	void*const b=arr->buf;
	const uint64_t idxes=idx*es;
	void*const bpidxes=b+idxes;
	const uint64_t aftridx=(els-idx)*es;
	if(ne*es>arr->bs){
		const uint64_t ns=arr->bs<<1;
		void*const tmp=malloc(ns);
		memcpy(tmp,b,idxes);
		void*const newp=tmp+idxes;
		memcpy(newp,new,es);
		memcpy(newp+es,b+idxes,aftridx);
		free(b);
		arr->buf=b;
		arr->bs=ns;
	}else{
		memmove(bpidxes+es,bpidxes,aftridx);
		memcpy(bpidxes,new,es);
	}
	arr->nels=els+1;
	return E_SUCC;
}
int8_t arrlst_del(arrlst_t*const __restrict arr,const uint64_t i){
	const uint64_t nels=arr->nels;
	if(i>nels){
		return E_OUT_OF_RANGE;
	}
	const uint64_t newnels=nels-1;
	if(i!=nels){
		const uint64_t es=arr->es;
		void*const bi=arr->buf+i*es;
		memmove(bi,bi+es,(newnels-i)*es);
	}
	arr->nels=newnels;
	return E_SUCC;
}
lnklst_t*lnklst_appndfrnt(lnklst_t*const ll,const int64_t v){
	lnklst_t*const __restrict new=malloc(sizeof(lnklst_t));
	new->val=v;
	new->nxt=ll;
	return new;
}
bool lnklst_in(const lnklst_t*const __restrict ll,const int64_t val){
	if(!ll){
		return 0;
	}
	if(val==ll->val){
		return 1;
	}
	return lnklst_in(ll->nxt,val);
}
void hshmp_add(const hshmp_t*const __restrict hm,const int64_t val){
	lnklst_t**const bkt=hm->map+hshmp_indx(hm,val);
	lnklst_t*const ll=*bkt;
	if(!lnklst_in(ll,val)){
		*bkt=lnklst_appndfrnt(ll,val);
	}
}
bool hshmp_in(const hshmp_t*const __restrict hm,const int64_t val){
	return lnklst_in(hm->map[hshmp_indx(hm,val)],val);
}
static uint64_t hshmp_indx(const hshmp_t*const __restrict hm,const int64_t val){
	return hm->hshfnc(val)%hm->nbkts;
}
