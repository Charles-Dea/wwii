#version 450
layout(location=0)in vec3 ip;
layout(location=1)in vec4 ic;
layout(location=0)uniform float sr;
layout(location=1)uniform vec3 campos;
layout(location=0)out vec4 oc;
void main(){
	vec2 pos=(ip.xy-campos.xy)*campos.z;
	gl_Position=vec4(pos.x*sr,pos.y,ip.z,1);
	oc=ic;
}
