#version 450
layout(location=0)in vec2 tc;
uniform sampler2D tex;
layout(location=0)out vec4 col;
void main(){
	col=texture(tex,tc).rgba;
}
