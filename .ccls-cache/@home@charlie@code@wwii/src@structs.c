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
lnklst_t*cpylnklst(const lnklst_t*const __restrict ll){
	if(ll){
		lnklst_t*__restrict new=malloc(sizeof(lnklst_t));
		new->val=ll->val;
		new->nxt=cpylnklst(ll->nxt);
		return new;
	}else{
		return 0;
	}
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
void lnklst_map(const lnklst_t*const __restrict ll,void(*const func)(int64_t)){
	if(ll){
		func(ll->val);
		lnklst_map(ll->nxt,func);
	}
}
void delnklst(const lnklst_t*const __restrict ll){
	if(ll){
		delnklst(ll->nxt);
		free(ll);
	}
}
hshmp_t cpyhshmp(const hshmp_t*const __restrict hm){
	const uint64_t nbkts=hm->nbkts;
	const hshmp_t new={
		.map=malloc(nbkts*8),
		.nbkts=nbkts,
		.hshfnc=hm->hshfnc,
	};
	for(
			lnklst_t**ni=new.map,*const*hi=hm->map,*const*const __restrict end=ni+nbkts;
			ni<end;
			ni++,hi++
			){
		*ni=cpylnklst(*hi);
	}
	return new;
}
void hshmp_addifabs(const hshmp_t*const __restrict hm,const int64_t val){
	if(!hshmp_in(hm,val)){
		hshmp_add(hm,val);
	}
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
bool hshmp_empty(const hshmp_t*const __restrict hm){
	for(lnklst_t*const*i=hm->map,*const*const __restrict end=i+hm->nbkts;i<end;i++){
		if(*i){
			return 0;
		}
	}
	return 1;
}
void hshmp_map(const hshmp_t*const __restrict hm,void(*const func)(int64_t)){
	for(lnklst_t*const*i=hm->map,*const*const __restrict end=i+hm->nbkts;i<end;i++){
		lnklst_map(*i,func);
	}
}
void delhshmp(const hshmp_t*const __restrict hm){
	lnklst_t*const*const map=hm->map;
	for(lnklst_t*const*i=map,*const*const __restrict end=i+hm->nbkts;i<end;i++){
		delnklst(*i);
	}
	free(map);
}
static uint64_t hshmp_indx(const hshmp_t*const __restrict hm,const int64_t val){
	return hm->hshfnc(val)%hm->nbkts;
}
