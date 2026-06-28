#version 450
layout(location=0)in vec3 pos;
layout(location=1)in vec4 ic;
layout(location=2)in float r;
layout(location=3)in vec2 cenpos;
layout(location=0)uniform float sr;
layout(location=1)uniform vec3 campos;
layout(location=0)out vec4 oc;
void main(){
	vec2 ds=pos.xy-cenpos;
	float d=sqrt(ds.x*ds.x+ds.y*ds.y);
	float a=atan(ds.y,ds.x)+r;
	vec2 p=vec2(cos(a)*d+cenpos.x,sin(a)*d+cenpos.y);
	p=(p-campos.xy)*campos.z;
	gl_Position=vec4(p.x*sr,p.y,1,1);
	oc=ic;
}
