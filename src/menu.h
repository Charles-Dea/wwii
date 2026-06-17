#ifndef MENU_H
#define MENU_H
#include<stdbool.h>
#include<stdint.h>
enum{
	SCENE_MAINMEN,
	SCENE_CHSD,
	SCENE_WTNG,
	SCENE_INPTIP,
	SCENE_GAME,
};
extern uint8_t menu_scene;
void menu_main();
void menu_strtgm();
#endif
