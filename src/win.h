#ifndef WIN_H
#define WIN_H
#include<GLFW/glfw3.h>
enum{
	INMODE_NORM,
	INMODE_MOVE,
	INMODE_FIRE,
};
typedef struct{
	double x,y;
}p2d_t;
extern int8_t(*win_clkoff)(int64_t,float,float);
extern int64_t win_clkoffparam;
extern int8_t(*win_rclk)(int64_t,float,float);
extern int64_t win_rclkparam;
extern uint8_t win_inmode;
extern uint16_t win_seltxtbox,win_frftbtn;
GLFWwindow*win_open(int8_t*__restrict);
p2d_t win_glfw2ndc(p2d_t);
void win_close(GLFWwindow*__restrict);
#endif
