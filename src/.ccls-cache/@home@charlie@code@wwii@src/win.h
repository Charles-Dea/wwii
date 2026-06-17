#ifndef WIN_H
#define WIN_H
#ifndef WLND
#define GLFW_USE_WAYLAND OFF
#endif
#include<GLFW/glfw3.h>
#include<stdint.h>
enum{
	INMODE_NORM,
	INMODE_MOVE,
	INMODE_FIRE,
};
extern int8_t(*win_clkoff)(int64_t,float,float);
extern int64_t win_clkoffparam;
extern int8_t(*win_rclk)(int64_t,float,float);
extern int64_t win_rclkparam;
extern uint8_t win_inmode;
extern uint16_t win_seltxtbox;
GLFWwindow*win_open(int8_t*__restrict);
void win_close(GLFWwindow*__restrict);
#endif
