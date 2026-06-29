#version 450
layout(location=0)in vec2 pos;
layout(location=1)in vec4 ic;
layout(location=2)in float r;
layout(location=3)in vec2 cp;
layout(location=0)out vec4 oc;
void main(){
	const float dx=pos.x-cp.x;
	const float dy=pos.y-cp.y;
	const float d=sqrt(dx*dx+dy*dy);
	oc=d<=r+.0025&&d>=r-.0025?ic:vec4(0,0,0,0);
}
