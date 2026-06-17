#include<GL/glew.h>
#include<math.h>
#include<stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>
#include"main.h"
#include"grphcs.h"
static uint32_t sprtshad,sprvbo,sprvao,sprebo,ringshad,ringvbo,ringvao,ringebo,arrshad,arrvbo,arrvao,rectshad,rectvbo,rectvao,rectebo;
static int32_t buildshad(const char*__restrict,const char*__restrict);
static int32_t compshad(uint32_t,uint32_t,const char*__restrict);
static int8_t loadtex(const char*__restrict);
static uint32_t mkbuf(uint32_t);
void grphcs_init(){
	glewInit();
	const int32_t ss=buildshad("shaders/sprite.vert.glsl","shaders/sprite.frag.glsl");
	const float sr=scrnhght/scrnwdth;
	if(ss&0x80000000){
		fputs("WARNING: failed to build sprite shader program\n",stderr);
	}else{
		glUseProgram(ss);
		glUniform1f(0,sr);
		sprtshad=ss;
	}
	const int32_t rs=buildshad("shaders/ring.vert.glsl","shaders/ring.frag.glsl");
	if(rs&0x80000000){
		fputs("WARNING: failed to build ring shader program\n",stderr);
	}else{
		glUseProgram(rs);
		glUniform1f(0,sr);
		ringshad=rs;
	}
	const int32_t as=buildshad("shaders/arrow.vert.glsl","shaders/arrow.frag.glsl");
	if(as&0x80000000){
		fputs("WARNING: failed to build arrow shader program\n",stderr);
	}else{
		glUseProgram(as);
		glUniform1f(0,sr);
		arrshad=as;
	}
	const int32_t res=buildshad("shaders/rect.vert.glsl","shaders/rect.frag.glsl");
	if(res&0x80000000){
		fputs("WARNING: failed to build rect shader program\n",stderr);
	}else{
		glUseProgram(res);
		glUniform1f(0,sr);
		rectshad=res;
	}
	if(loadtex("res/alliedinf.png")){
		fputs("WARNING: failed to load texture for Allied infantry\n",stderr);
	}
	if(loadtex("res/axisinf.png")){
		fputs("WARNING: failed to load texture for Axis infantry\n",stderr);
	}
	if(loadtex("res/alliedinfsel.png")){
		fputs("WARNING: failed to load texture for selected Allied infantry\n",stderr);
	}
	if(loadtex("res/axisinfsel.png")){
		fputs("WARNING: failed to load texture for selected Axis infantry\n",stderr);
	}
	if(loadtex("res/alliedinfactd.png")){
		fputs("WARNING: failed to load texture for acted Allied infantry\n",stderr);
	}
	if(loadtex("res/axisinfactd.png")){
		fputs("WARNING: failed to load texture for acted Axis infantry\n",stderr);
	}
	if(loadtex("res/usflag.png")){
		fputs("WARNING: failed to load texture for American Flag\n",stderr);
	}
	if(loadtex("res/germflag.png")){
		fputs("WARNING: failed to load texture for German Flag\n",stderr);
	}
	if(loadtex("res/host.png")){
		fputs("WARNING: failed to load texture for host game button\n",stderr);
	}
	if(loadtex("res/join.png")){
		fputs("WARNING: failed to load texture for join game button\n",stderr);
	}
	if(loadtex("res/waiting.png")){
		fputs("WARNING: failed to load texture for waiting sprite\n",stderr);
	}
	char path[]="res/0.png";
	for(char i='0';i<='9';){
		if(loadtex(path)){
			fprintf(stderr,"WARNING: failed to load texture for digit %c\n",i);
		}
		path[4]=++i;
	}
	if(loadtex("res/dot.png")){
		fputs("WARNING: failed to load texture for period character\n",stderr);
	}
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glClearDepth(1);
	sprvbo=mkbuf(GL_ARRAY_BUFFER);
	uint32_t va;
	glGenVertexArrays(1,&va);
	glBindVertexArray(va);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,0,(3+2)*4,0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,2,GL_FLOAT,0,(3+2)*4,(void*)(3*4));
	sprvao=va;
	sprebo=mkbuf(GL_ELEMENT_ARRAY_BUFFER);
	ringvbo=mkbuf(GL_ARRAY_BUFFER);
	glGenVertexArrays(1,&va);
	glBindVertexArray(va);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,0,(3+4+1+2)*4,0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,4,GL_FLOAT,0,(3+4+1+2)*4,(void*)(3*4));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,1,GL_FLOAT,0,(3+4+1+2)*4,(void*)((3+4)*4));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3,2,GL_FLOAT,0,(3+4+1+2)*4,(void*)((3+4+1)*4));
	ringvao=va;
	ringebo=mkbuf(GL_ELEMENT_ARRAY_BUFFER);
	arrvbo=mkbuf(GL_ARRAY_BUFFER);
	glGenVertexArrays(1,&va);
	glBindVertexArray(va);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,0,(3+4)*4,0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,4,GL_FLOAT,0,(3+4)*4,(void*)(3*4));
	arrvao=va;
	rectvbo=mkbuf(GL_ARRAY_BUFFER);
	glGenVertexArrays(1,&va);
	glBindVertexArray(va);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,0,(3+4)*4,0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,4,GL_FLOAT,0,(3+4)*4,(void*)(3*4));
	rectvao=va;
	rectebo=mkbuf(GL_ELEMENT_ARRAY_BUFFER);
}
void grphcs_draw(GLFWwindow*const __restrict win){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	typedef struct{
		float x,y,z,w,h,r,g,b,a;
	}rct_t;
	const uint64_t nsqs=rects.nels;
	const uint64_t bs=nsqs*sizeof(rct_t);
	arrlst_t rcts={
		.buf=malloc(bs),
		.bs=bs,
		.es=sizeof(rct_t),
	};
	for(const rect_t*i=rects.buf,*const __restrict end=i+nsqs;i<end;i++){
		const uint16_t eid=i->eid;
		int8_t err;
		const posn_t posn=getposn(eid,&err);
		if(err){
			fprintf(stderr,"WARNING: entity %hu has no position\n",eid);
			continue;
		}
		const dim_t*const dim=getent(&dims,eid);
		if(!dim){
			fprintf(stderr,"WARNING: entity %hu has no dimensions\n",eid);
			continue;
		}
		const col_t*const col=getent(&cols,eid);
		if(!col){
			fprintf(stderr,"WARNING: entity %hu has no colors\n",eid);
			continue;
		}
		const rct_t rct={
			.x=posn.x,
			.y=posn.y,
			.z=posn.z,
			.w=dim->w,
			.h=dim->h,
			.r=col->r,
			.g=col->g,
			.b=col->b,
			.a=col->a,
		};
		arrlst_add(&rcts,&rct);
	}
	typedef struct{
		float x,y,z,r,g,b,a;
	}rectvert_t;
	typedef struct{
		rectvert_t tl,tr,bl,br;
	}vborect_t;
	const uint64_t vborectsz=rcts.nels*sizeof(vborect_t);
	vborect_t*const vborects=malloc(vborectsz);
	typedef struct{
		uint32_t tl,tr0,bl0,tr1,bl1,br;
	}eborect_t;
	const uint64_t eborectsz=rcts.nels*sizeof(eborect_t);
	eborect_t*const eborects=malloc(eborectsz);
	for(uint64_t i=0;i<rcts.nels;i++){
		const rct_t*const rct=rcts.buf+i*rcts.es;
		const float x=rct->x;
		const float y=rct->y;
		const float z=rct->z;
		const float hw=rct->w/2;
		const float hh=rct->h/2;
		const float r=rct->r;
		const float g=rct->g;
		const float b=rct->b;
		const float a=rct->a;
		const float left=x-hw;
		const float rght=x+hw;
		const float top=y+hh;
		const float bot=y-hh;
		vborect_t*const vr=vborects+i;
		vr->tl.x=left;
		vr->tl.y=top;
		vr->tl.z=z;
		vr->tl.r=r;
		vr->tl.g=g;
		vr->tl.b=b;
		vr->tl.a=a;
		vr->tr.x=rght;
		vr->tr.y=top;
		vr->tr.z=z;
		vr->tr.r=r;
		vr->tr.g=g;
		vr->tr.b=b;
		vr->tr.a=a;
		vr->bl.x=left;
		vr->bl.y=bot;
		vr->bl.z=z;
		vr->bl.r=r;
		vr->bl.g=g;
		vr->bl.b=b;
		vr->bl.a=a;
		vr->br.x=rght;
		vr->br.y=bot;
		vr->br.z=z;
		vr->br.r=r;
		vr->br.g=g;
		vr->br.b=b;
		vr->br.a=a;
		const uint32_t tl=i*4;
		const uint32_t tr=tl+1;
		const uint32_t bl=tr+1;
		const uint32_t br=bl+1;
		eborect_t*const er=eborects+i;
		er->tl=tl;
		er->tr0=tr;
		er->bl0=bl;
		er->tr1=tr;
		er->bl1=bl;
		er->br=br;
	}
	glUseProgram(rectshad);
	glBindBuffer(GL_ARRAY_BUFFER,rectvbo);
	glBindVertexArray(rectvao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,rectebo);
	glBufferData(GL_ARRAY_BUFFER,vborectsz,vborects,GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,eborectsz,eborects,GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES,rcts.nels*6,GL_UNSIGNED_INT,0);
	free(vborects);
	free(eborects);
	free(rcts.buf);
	arrlst_t sprites[TEX_MAX];
	typedef struct{
		float x,y,z,w,h;
		int32_t tex;
		int8_t pad[8];
	}sprite_t;
	const arrlst_t*const __restrict send=sprites+TEX_MAX;
	for(arrlst_t*i=sprites+1;i<send;i++){
		i->buf=malloc(8*sizeof(sprite_t));
		i->nels=0;
		i->bs=8*sizeof(sprite_t);
		i->es=sizeof(sprite_t);
	}
	const tex_t*const txs=texes.buf;
	const tex_t*const tend=txs+texes.nels;
	for(const tex_t*i=txs;i<tend;i++){
		const uint16_t eid=i->eid;
		int8_t err;
		const posn_t posn=getposn(eid,&err);
		if(err){
			fprintf(stderr,"WARNING: entity %hu has no position\n",eid);
			continue;
		}
		const dim_t*const dim=getent(&dims,eid);
		float w,h;
		if(dim){
			w=dim->w;
			h=dim->h;
		}else{
			fprintf(stderr,"WARNING: entity %hu does not have dimensions\n",eid);
			continue;
		}
		const uint64_t tex=i->tex;
		const sprite_t sprt={
			.x=posn.x,
			.y=posn.y,
			.z=posn.z,
			.w=w,
			.h=h,
			.tex=tex,
		};
		arrlst_add(sprites+tex,&sprt);
	}
	glUseProgram(sprtshad);
	for(uint64_t i=1;i<TEX_MAX;i++){
		glBindTexture(GL_TEXTURE_2D,i);
		typedef struct{
			float x,y,z,tx,ty;
		}vert_t;
		typedef struct{
			vert_t tl,tr,bl,br;
		}vbosq_t;
		const arrlst_t*const a=sprites+i;
		const uint64_t nels=a->nels;
		const uint64_t vbosqsz=nels*sizeof(vbosq_t);
		vbosq_t*const vbosqs=malloc(vbosqsz);
		typedef struct{
			uint32_t tl,tr1,bl1,tr2,bl2,br;
		}ebosq_t;
		const uint64_t ebosqsz=nels*sizeof(ebosq_t);
		ebosq_t*const ebosqs=malloc(ebosqsz);
		const sprite_t*const sprts=a->buf;
		glBindBuffer(GL_ARRAY_BUFFER,sprvbo);
		glBindVertexArray(sprvao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,sprebo);
		for(uint64_t j=0;j<nels;j++){
			const sprite_t*const s=sprts+j;
			const float x=s->x;
			const float y=s->y;
			const float z=s->z;
			const float hw=s->w/2;
			const float hh=s->h/2;
			const float l=x-hw;
			const float r=x+hw;
			const float t=y+hh;
			const float b=y-hh;
			vbosq_t*const vbosq=vbosqs+j;
			vbosq->tl.x=l;
			vbosq->tl.y=t;
			vbosq->tl.z=z;
			vbosq->tl.tx=0;
			vbosq->tl.ty=0;
			vbosq->tr.x=r;
			vbosq->tr.y=t;
			vbosq->tr.z=z;
			vbosq->tr.tx=1;
			vbosq->tr.ty=0;
			vbosq->bl.x=l;
			vbosq->bl.y=b;
			vbosq->bl.z=z;
			vbosq->bl.tx=0;
			vbosq->bl.ty=1;
			vbosq->br.x=r;
			vbosq->br.y=b;
			vbosq->br.z=z;
			vbosq->br.tx=1;
			vbosq->br.ty=1;
			const uint32_t base=j*4;
			ebosq_t*const ebosq=ebosqs+j;
			ebosq->tl=base;
			ebosq->tr1=base+1;
			ebosq->bl1=base+2;
			ebosq->tr2=base+1;
			ebosq->bl2=base+2;
			ebosq->br=base+3;
		}
		glBufferData(GL_ARRAY_BUFFER,vbosqsz,vbosqs,GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,ebosqsz,ebosqs,GL_STATIC_DRAW);
		glDrawElements(GL_TRIANGLES,nels*6,GL_UNSIGNED_INT,0);
		free(vbosqs);
		free(ebosqs);
		free(sprts);
	}
	typedef struct{
		float x,y,z,r,g,b,a,rad;
	}crcl_t;
	arrlst_t crcls={
		.buf=malloc(8*sizeof(crcl_t)),
		.bs=8*sizeof(crcl_t),
		.es=sizeof(crcl_t),
	};
	const ring_t*i=rings.buf;
	const ring_t*const __restrict end=i+rings.nels;
	for(;i<end;i++){
		const uint16_t eid=i->eid;
		int8_t err;
		const posn_t posn=getposn(eid,&err);
		if(err){
			fprintf(stderr,"WARNING: entity %hu has no position\n",eid);
			continue;
		}
		const col_t*const col=getent(&cols,eid);
		float r,g,b,a;
		if(col){
			a=col->a;
			if(!a){
				continue;
			}
			r=col->r;
			g=col->g;
			b=col->b;
		}else{
			fprintf(stderr,"WARNING: entity %hu has no color\n",eid);
			r=1;
			g=1;
			b=1;
			a=1;
		}
		const crcl_t crcl={
			.x=posn.x,
			.y=posn.y,
			.z=posn.z,
			.r=r,
			.g=g,
			.b=b,
			.a=a,
			.rad=i->r,
		};
		arrlst_add(&crcls,&crcl);
	}
	typedef struct{
		float x,y,z,r,g,b,a,rad,cx,cy;
	}cvert_t;
	typedef struct{
		cvert_t tl,tr,bl,br;
	}vbosq_t;
	const uint64_t vbosqsz=crcls.nels*sizeof(vbosq_t);
	vbosq_t*const vbosqs=malloc(vbosqsz);
	typedef struct{
		uint32_t tl,tr1,bl1,tr2,bl2,br;
	}ebosq_t;
	const uint64_t ebosqsz=crcls.nels*sizeof(ebosq_t);
	ebosq_t*const ebosqs=malloc(ebosqsz);
	for(uint64_t i=0;i<crcls.nels;i++){
		const crcl_t*const c=(crcl_t*)crcls.buf+i;
		const float x=c->x;
		const float y=c->y;
		const float z=c->z;
		const float r=c->r;
		const float g=c->g;
		const float b=c->b;
		const float a=c->a;
		const float rad=c->rad;
		const float hl=rad+.0025;
		const float left=x-hl;
		const float rght=x+hl;
		const float top=y+hl;
		const float btm=y-hl;
		vbosq_t*const v=vbosqs+i;
		v->tl.x=left;
		v->tl.y=top;
		v->tl.z=z;
		v->tl.r=r;
		v->tl.g=g;
		v->tl.b=b;
		v->tl.a=a;
		v->tl.rad=rad;
		v->tl.cx=x;
		v->tl.cy=y;
		v->tr.x=rght;
		v->tr.y=top;
		v->tr.z=z;
		v->tr.r=r;
		v->tr.g=g;
		v->tr.b=b;
		v->tr.a=a;
		v->tr.rad=rad;
		v->tr.cx=x;
		v->tr.cy=y;
		v->bl.x=left;
		v->bl.y=btm;
		v->bl.z=z;
		v->bl.r=r;
		v->bl.g=g;
		v->bl.b=b;
		v->bl.a=a;
		v->bl.rad=rad;
		v->bl.cx=x;
		v->bl.cy=y;
		v->br.x=rght;
		v->br.y=btm;
		v->br.z=z;
		v->br.r=r;
		v->br.g=g;
		v->br.b=b;
		v->br.a=a;
		v->br.rad=rad;
		v->br.cx=x;
		v->br.cy=y;
		const uint32_t base=i*4;
		ebosq_t*const e=ebosqs+i;
		e->tl=base;
		e->tr1=base+1;
		e->bl1=base+2;
		e->tr2=base+1;
		e->bl2=base+2;
		e->br=base+3;
	}
	glUseProgram(ringshad);
	glBindBuffer(GL_ARRAY_BUFFER,ringvbo);
	glBindVertexArray(ringvao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ringebo);
	glBufferData(GL_ARRAY_BUFFER,vbosqsz,vbosqs,GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,ebosqsz,ebosqs,GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES,crcls.nels*6,GL_UNSIGNED_INT,0);
	free(vbosqs);
	free(ebosqs);
	free(crcls.buf);
	typedef struct{
		float x,y,z,r,g,b,a;
	}arr_t;
	arrlst_t arrs={
		.buf=malloc(8*sizeof(arr_t)),
		.bs=8*sizeof(arr_t),
		.es=sizeof(arr_t),
	};
	const arrow_t*ai=arrows.buf;
	const arrow_t*const __restrict aend=ai+arrows.nels;
	for(;ai<aend;ai++){
		const uint16_t eid=ai->eid;
		const col_t*const col=getent(&cols,eid);
		float r,g,b,a;
		if(col){
			a=col->a;
			if(!a){
				continue;
			}
			r=col->r;
			g=col->g;
			b=col->b;
		}else{
			fprintf(stderr,"WARNING: entity %hu has no color\n",eid);
			r=1;
			g=1;
			b=1;
			a=1;
		}
		int8_t err;
		const posn_t posn=getposn(eid,&err);
		if(err){
			fprintf(stderr,"WARNING: entity %hu has no position\n",eid);
			continue;
		}
		const arr_t arr={
			.x=posn.x,
			.y=posn.y,
			.z=posn.z,
			.r=r,
			.g=g,
			.b=b,
			.a=a,
		};
		arrlst_add(&arrs,&arr);
	}
	typedef arr_t arrvert_t;
	typedef struct{
		arrvert_t a,b,c;
	}tri_t;
	const uint64_t trisz=arrs.nels*sizeof(tri_t);
	tri_t*const tris=malloc(trisz);
	for(uint64_t i=0;i<arrs.nels;i++){
		const arr_t*const ar=arrs.buf+i*sizeof(arr_t);
		const float x=ar->x;
		const float y=ar->y;
		const float z=ar->z;
		const float r=ar->r;
		const float g=ar->g;
		const float b=ar->b;
		const float a=ar->a;
		double dcx,dcy;
		glfwGetCursorPos(win,&dcx,&dcy);
		const float sh=scrnhght;
		const float hfsh=sh/2;
		const float cx=dcx/hfsh-(float)scrnwdth/sh;
		const float cy=-(dcy/hfsh-1);
		const float angl=atan2f(cy-y,cx-x);
		const float angla=angl+M_PI/2;
		const float anglb=angl+3*M_PI/2;
		tri_t*const t=tris+i;
		t->a.x=cosf(angla)*.01+x;
		t->a.y=sinf(angla)*.01+y;
		t->a.z=z;
		t->a.r=r;
		t->a.g=g;
		t->a.b=b;
		t->a.a=a;
		t->b.x=cosf(anglb)*.01+x;
		t->b.y=sinf(anglb)*.01+y;
		t->b.z=z;
		t->b.r=r;
		t->b.g=g;
		t->b.b=b;
		t->b.a=a;
		t->c.x=cx;
		t->c.y=cy;
		t->c.z=z;
		t->c.r=r;
		t->c.g=g;
		t->c.b=b;
		t->c.a=a;
	}
	glUseProgram(arrshad);
	glBindBuffer(GL_ARRAY_BUFFER,arrvbo);
	glBufferData(GL_ARRAY_BUFFER,trisz,tris,GL_DYNAMIC_DRAW);
	glBindVertexArray(arrvao);
	glDrawArrays(GL_TRIANGLES,0,arrs.nels*3);
	free(tris);
	glfwSwapBuffers(win);
}
void grphcs_term(){
	uint32_t texes[TEX_MAX-1];
	for(uint64_t i=0;i<TEX_MAX-1;i++){
		texes[i]=i+1;
	}
	glDeleteTextures(TEX_MAX-1,texes);
	const uint32_t bufs[]={sprvbo,sprebo,ringvbo,ringebo,arrvbo};
	glDeleteBuffers(sizeof(bufs)/4,bufs);
	glBindVertexArray(sprvao);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glDeleteVertexArrays(1,&sprvao);
	glBindVertexArray(ringvao);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glDeleteVertexArrays(1,&ringvao);
	glBindVertexArray(arrvao);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glDeleteVertexArrays(1,&arrvao);
	glDeleteProgram(sprtshad);
	glDeleteProgram(ringshad);
	glDeleteProgram(arrshad);
}
static int32_t buildshad(const char*const __restrict vs,const char*const __restrict fs){
	const int32_t prog=glCreateProgram();
	const int32_t vert=compshad(prog,GL_VERTEX_SHADER,vs);
	if(vert&0x80000000){
		fprintf(stderr,"ERROR: compshad failed to compile %s\n",vs);
		return vert;
	}
	const int32_t frag=compshad(prog,GL_FRAGMENT_SHADER,fs);
	if(frag&0x80000000){
		fprintf(stderr,"ERROR: compshad failed to compile %s\n",fs);
		return frag;
	}
	glLinkProgram(prog);
	glDeleteShader(vert);
	glDeleteShader(frag);
	return prog;
}
static int32_t compshad(const uint32_t prog,const uint32_t type,const char*const __restrict fn){
	const int32_t shad=glCreateShader(type);
	FILE*const __restrict file=fopen(fn,"r");
	if(!file){
		fprintf(stderr,"ERROR: failed to open %s\n",fn);
		return E_FOPEN_FAIL;
	}
	fseek(file,0,SEEK_END);
	const uint64_t len=ftell(file);
	fseek(file,0,SEEK_SET);
	char code[len+1];
	fread(code,1,len,file);
	fclose(file);
	code[len]=0;
	const char*const c=code;
	glShaderSource(shad,1,&c,0);
	glCompileShader(shad);
	int32_t cmpd;
	glGetShaderiv(shad,GL_COMPILE_STATUS,&cmpd);
	if(!cmpd){
		int32_t infloglen;
		glGetShaderiv(shad,GL_INFO_LOG_LENGTH,&infloglen);
		char err[infloglen];
		glGetShaderInfoLog(shad,infloglen,&infloglen,err);
		fprintf(stderr,"ERROR: %s compiled with error(s):\n%s\n",fn,err);
		return E_SHAD_COMP_FAIL;
	}
	glAttachShader(prog,shad);
	return shad;
}
static int8_t loadtex(const char*const __restrict fn){
	int32_t width,height,comp;
	const uint8_t*const __restrict img=stbi_load(fn,&width,&height,&comp,4);
	if(!img){
		fprintf(stderr,"ERROR: failed to load %s\n",fn);
		return E_STBI_FAIL;
	}
	uint32_t tex;
	glGenTextures(1,&tex);
	glBindTexture(GL_TEXTURE_2D,tex);
	glTextureParameteri(tex,GL_TEXTURE_MAX_LEVEL,0);
	glTextureParameteri(tex,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTextureParameteri(tex,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,img);
	free(img);
	return E_SUCC;
}
static uint32_t mkbuf(const uint32_t type){
	uint32_t buf;
	glGenBuffers(1,&buf);
	glBindBuffer(type,buf);
	return buf;
}
/*static posn_t getposn(const uint16_t eid,int8_t*const __restrict err){
	const pos_t*const pos=getent(&poses,eid);
	if(pos){
		const posn_t posn={
			.x=pos->x,
			.y=pos->y,
			.z=pos->z,
		};
		*err=E_SUCC;
		return posn;
	}
	const relpos_t*const relpos=getent(&relposes,eid);
	if(!relpos){
		*err=E_NO_ENT;
		posn_t posn={};
		return posn;
	}
	const posn_t prnt=getposn(relpos->prnt,err);
	if(*err){
		posn_t posn={};
		return posn;
	}
	const posn_t posn={
		.x=prnt.x+relpos->x,
		.y=prnt.y+relpos->y,
		.z=prnt.z+relpos->z,
	};
	return posn;
}*/
