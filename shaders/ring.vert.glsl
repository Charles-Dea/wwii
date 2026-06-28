#version 450
layout(location=0)in vec3 ip;
layout(location=1)in vec4 ic;
layout(location=2)in float ir;
layout(location=3)in vec2 icp;
layout(location=0)uniform float sr;
layout(location=1)uniform vec3 cp;
layout(location=0)out vec2 op;
layout(location=1)out vec4 oc;
layout(location=2)out float or;
layout(location=3)out vec2 ocp;
void main(){
	vec2 pos=(ip.xy-cp.xy)*cp.z;
	gl_Position=vec4(pos.x*sr,pos.y,ip.z,1);
	op=ip.xy;
	oc=ic;
	or=ir;
	ocp=icp;
}
