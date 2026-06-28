#version 450
layout(location=0)in vec3 ip;
layout(location=1)in vec2 itc;
layout(location=0)uniform float sr;
layout(location=1)uniform vec3 cp;
layout(location=0)out vec2 otc;
void main(){
	vec2 pos=(ip.xy-cp.xy)*cp.z;
	gl_Position=vec4(pos.x*sr,pos.y,ip.z,1);
	otc=itc;
}
