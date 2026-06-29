#include<enet/enet.h>
#include<stdio.h>
#include<string.h>
#include"grphcs.h"
#include"main.h"
#include"menu.h"
#include"unit.h"
#include"net.h"
#define PORT 7777
enum{
	STATUS_DSCNCT,
	STATUS_WTNG,
	STATUS_CONCTD,
};
enum{
	PT_STSD,
	PT_MOVE,
	PT_DSTR,
	PT_DMG,
	PT_ETRN,
	PT_EGM,
	PT_ACK,
};
typedef struct{
	uint8_t type;
	int8_t pad;
	uint16_t eid;
	float x,y;
}mvordr_t;
typedef struct{
	uint8_t type;
	int8_t pad;
	uint16_t eid;
}dstr_t;
static ENetHost*host;
static ENetPeer*peer;
static uint8_t status;
static void sndpkt(const void*__restrict,uint64_t);
void net_init(){
	enet_initialize();
	atexit(enet_deinitialize);
}
int8_t net_strthst(){
	ENetAddress adr={
		.host=ENET_HOST_ANY,
		.port=PORT,
	};
	if(host){
		enet_host_destroy(host);
	}
	ENetHost*h=enet_host_create(&adr,1,1,0,0);
	if(!h){
		fputs("ERROR: failed to start host\n",stderr);
		return E_STRTHST_FAIL;
	}
	host=h;
	status=STATUS_WTNG;
	return E_SUCC;
}
int8_t net_join(arrlst_t*const __restrict str){
	ENetHost*const h=enet_host_create(NULL,1,1,0,0);
	if(!h){
		fputs("ERROR: failed to create enet host\n",stderr);
		return E_STRTHST_FAIL;
	}
	host=h;
	const char nb=0;
	arrlst_add(str,&nb);
	ENetAddress ip;
	enet_address_set_host(&ip,str->buf);
	ip.port=PORT;
	ENetPeer*const p=enet_host_connect(h,&ip,1,0);
	ENetEvent event;
	if(enet_host_service(h,&event,100)<=0||event.type!=ENET_EVENT_TYPE_CONNECT){
		fputs("ERROR: failed to connect to host\n",stderr);
		menu_main();
		return E_CONCT_FAIL;
	}
	peer=p;
	status=STATUS_CONCTD;
	return E_SUCC;
}
void net_lstn(){
	uint8_t st=status;
	ENetEvent event;
	while(st!=STATUS_DSCNCT&&enet_host_service(host,&event,1)>0){
		typedef struct{
			uint8_t type;
			bool allied;
		}setside_t;
		if(st==STATUS_WTNG&&event.type==ENET_EVENT_TYPE_CONNECT){
			peer=event.peer;
			const setside_t stsd={
				.type=PT_STSD,
				.allied=!unit_allied,
			};
			sndpkt(&stsd,sizeof(setside_t));
			st=STATUS_CONCTD;
			menu_strtgm();
		}else if(st==STATUS_CONCTD&&event.type==ENET_EVENT_TYPE_RECEIVE){
			const uint8_t*const data=event.packet->data;
			switch(*data){
				case PT_STSD:
					unit_allied=data[1];
					menu_strtgm();
					break;
				case PT_MOVE:
					const mvordr_t*const move=(mvordr_t*)data;
					pos_t*const pos=getent(&poses,move->eid);
					if(pos){
						pos->x=move->x;
						pos->y=move->y;
					}else{
						fprintf(stderr,"WARNING: entity %hu has no position\n",move->eid);
					}
					break;
				case PT_DSTR:
					delent(((dstr_t*)data)->eid);
					break;
				case PT_DMG:
					udata_t*const u=getent(&udatas,*((uint16_t*)(data+2)));
					if(u){
						unit_chmrl(u,*((double*)(data+8)));
						wpn_t*wi=u->wpns;
						uint16_t*const rngs=u->rings+1;
						uint64_t ri=0;
						for(const uint8_t*i=data+16,*const __restrict end=i+u->nwpns;i<end;i++,wi++){
							const uint8_t n=*i;
							if(!n&&wi->num){
								uint16_t*const rng=rngs+ri;
								delent(*rng);
								*rng=0;
							}
							wi->num=n;
							const uint16_t sprt1=wi->sprt1;
							if(sprt1){
								tex_t*const t=getent(&texes,sprt1);
								if(t){
									t->tex=n/10+TEX_0;
								}else{
									fprintf(stderr,"WARNING: entity %hu has no texture\n",sprt1);
								}
							}
							tex_t*const t=getent(&texes,wi->sprt0);
							if(t){
								t->tex=n%10+TEX_0;
							}else{
								fprintf(stderr,"WARNING: entity %hu has no texture\n",wi->sprt0);
							}
							ri++;
						}
					}else{
						fprintf(stderr,"WARNING: entity %hu has no udata\n",*((uint16_t*)(data+2)));
					}
					break;
				case PT_ETRN:
					unit_pltrn=1;
					unit_nxtrn(!unit_allied);
					break;
				case PT_EGM:
					const int8_t ack=PT_ACK;
					sndpkt(&ack,1);
					menu_endscrn(data[1]);
					break;
				case PT_ACK:
					fputs("WARNING: peer acknowledged packet that was not sent\n",stderr);
					break;
				default:
					fprintf(stderr,"WARNING: received invalid packet type %hhu\n",*data);
			}
		}else if(st==STATUS_CONCTD&&event.type==ENET_EVENT_TYPE_DISCONNECT){
			enet_host_destroy(host);
			host=NULL;
			st=STATUS_DSCNCT;
		}
	}
	status=st;
}
void net_move(const uint16_t eid,const float x,const float y){
	const mvordr_t move={
		.type=PT_MOVE,
		.eid=eid,
		.x=x,
		.y=y,
	};
	sndpkt(&move,sizeof(mvordr_t));
}
void net_dstr(const uint16_t eid){
	const dstr_t dstr={
		.type=PT_DSTR,
		.eid=eid,
	};
	sndpkt(&dstr,sizeof(dstr_t));
}
void net_dmg(
		const uint16_t eid,
		const uint8_t*const __restrict nums,
		const uint64_t nn,
		const double mrlchng){
	const uint64_t pktsz=nn+16;
	int8_t pkt[pktsz];
	pkt[0]=PT_DMG;
	*((uint16_t*)(pkt+2))=eid;
	*((double*)(pkt+8))=mrlchng;
	memcpy(pkt+16,nums,nn);
	sndpkt(pkt,pktsz);
}
void net_endtrn(){
	const uint8_t pt=PT_ETRN;
	sndpkt(&pt,1);
}
int8_t net_dscnct(const bool aldvctry){
	if(status==STATUS_CONCTD){
		const int8_t pkt[]={PT_EGM,aldvctry};
		sndpkt(pkt,2);
		ENetEvent event;
		while(enet_host_service(host,&event,15)>0){
			if(event.type==ENET_EVENT_TYPE_RECEIVE&&*event.packet->data==PT_ACK){
				goto dscnct;
			}
		}
		fputs("WARNING: peer did not acknowledge end game packet\n",stderr);
dscnct:
		enet_peer_disconnect(peer,0);
		while(enet_host_service(host,&event,1000)>0){
			if(event.type==ENET_EVENT_TYPE_RECEIVE){
				enet_packet_destroy(event.packet);
			}else if(event.type==ENET_EVENT_TYPE_DISCONNECT){
				enet_host_destroy(host);
				goto end;
			}
		}
		fputs("ERROR: disconnect failed\n",stderr);
		enet_peer_reset(peer);
		return E_DSCNCT_FAIL;
	}else if(status==STATUS_WTNG){
		enet_host_destroy(host);
	}
end:
	host=NULL;
	status=STATUS_DSCNCT;
	return E_SUCC;
}
static void sndpkt(const void*const __restrict data,const uint64_t s){
	enet_peer_send(peer,0,enet_packet_create(data,s,ENET_PACKET_FLAG_RELIABLE));
}
