#version 450
layout(location=0)in vec3 pos;
layout(location=1)in vec2 itc;
layout(location=0)uniform float sr;
layout(location=0)out vec2 otc;
void main(){
	gl_Position=vec4(pos.x*sr,pos.yz,1);
	otc=itc;
}
