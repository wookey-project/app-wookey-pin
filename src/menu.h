#ifndef MENU_H_
#define MENU_H_

#include "api/types.h"
#include <stdarg.h>

void draw_menu(int x, int y);

void draw_menulist(int x, int y, uint8_t size, ...);

void menu_get_events(void);

void menu_draw_button(bool invert);

bool menu_is_touched(int posx, int posy);

void draw_background(void);

void update_remaining_tries(uint32_t val);

#endif
