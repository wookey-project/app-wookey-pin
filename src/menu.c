#include "menu.h"
#include "libtft.h"
#include "api/string.h"
#include "api/print.h"
#include "api/types.h"
#include "api/syscall.h"
#include "libtouch.h"
#include "bg.h"
#include "rtc.h"

extern const int font_width;
extern const int font_height;
extern const int font_blankskip;

/*
 * x & y are screen width & height
 */
void draw_menu(int x, int y, bool horizontal)
{
  /* menu background */
  if (horizontal) {
      tft_fill_rectangle(0,x,0,32,0,0,0);
  } else {
      tft_fill_rectangle(0,32,0,y,0,0,0);
  }
  /* menu button */
  menu_draw_button(false);
}

void draw_menubox(int x1,int x2, int y1, int y2, char *c)
{
  uint8_t r = 235, g = 235, b = 235;

  //const int char_width=font_width/128;
  int posx,posy;
  tft_setbg(r,g,b);
  //tft_setfg(0,0,0);
  tft_fill_rectangle(x1,x2,y1,y2,255-r,255-g,255-b);
  tft_fill_rectangle(x1+1,x2-1,y1+1,y2-1,r,g,b);
  posx=x1 + 10; // left align
  posy=(y2-y1-font_height/2)/2;
  tft_set_cursor_pos(x1+posx,posy+y1);
  tft_setfg(255-r,255-g,255-b);
  tft_puts(c);
}


/*
 * draw as many items as required as an openeded menu list
 */
void draw_menulist(int x, int y, uint8_t size, ...)
{
    va_list args;
    char    *menu_names[10];
    uint8_t cury = y;

    if (size > 10) {
        size = 10;
    }

    va_start(args, size);
    for (uint8_t i = 0; i < size; ++i) {
        menu_names[i] = va_arg(args, char*);
        draw_menubox(x, x+200, cury, cury+40, menu_names[i]);
        cury += 40;
    }
    va_end(args);
}

void draw_background(void)
{

    /* background color homogeneous with bg image */
    tft_fill_rectangle(0,240,0,340,54,36,99);
    tft_rle_image(0,34,bg_width,bg_height,bg_colormap,bg,sizeof(bg));
}

static bool menu_opened = false;

bool menu_is_touched(int posx, int posy)
{
    if (posx >= 0 && posx < 34 && posy >= 0 && posy < 34) {
        return true;
    }
    return false;
}

void menu_draw_button(bool invert)
{
  uint8_t r, g, b;
  if (invert == false) {
      r = 235;
      g = 235;
      b = 235;
  } else {
      r = 20;
      g = 20;
      b = 20;
  }

  //const int char_width=font_width/128;
  tft_setbg(r,g,b);
  //tft_setfg(0,0,0);
  tft_fill_rectangle(1,33,1,33,255-r,255-g,255-b);

  tft_fill_rectangle(3,28,3,5,r,g,b);
  tft_fill_rectangle(3,28,10,12,r,g,b);
  tft_fill_rectangle(3,28,17,19,r,g,b);

}

void unroll_menulist(uint32_t x, uint32_t y, uint8_t size)
{
    tft_rle_image(x,y,bg_width,((40+2)*size),bg_colormap,bg,sizeof(bg));
}

extern uint32_t numexti;

void menu_get_events(void)
{
  uint64_t ts = 0;
  char *timestamp = 0;
  while(1)
  {
    touch_read_X_DFR();/* Ensures that PenIRQ is enabled */
    /*
     * Between touch_read_X_DFR and touch_is_touched, we need to wait a little
     * or touch_is_touched() will return an invalid value
     */
    sys_sleep(10, SLEEP_MODE_INTERRUPTIBLE);

    /* Wait loop for touchscreen to be touched */
    while(!(touch_is_touched())) {
        touch_enable_exti();
        /* Here we sleep for 1 second in order to print the
         * current timestamp on the menubar every second
         * We can be interrupted by the touchscreen if needed
         */
        sys_sleep(1000, SLEEP_MODE_INTERRUPTIBLE);
        sys_get_systick(&ts, PREC_MILLI);
        timestamp = get_timestamp((uint32_t)(ts / 1000));
        tft_set_cursor_pos(115,0);
        tft_setfg(235,235,235);
        tft_setbg(0,0,0);
        tft_puts(timestamp);
    }
    //Refresh the actual positions
    touch_refresh_pos();
    //Follow the motion on the screen
    while (touch_refresh_pos(),touch_is_touched())
    {
        int posx,posy;
        //touch_refresh_pos();
        posy=touch_getx();
        posx=touch_gety();

        menu_draw_button(!menu_opened);
        if (menu_is_touched(posx, posy) && !menu_opened) {
            menu_draw_button(true);
            draw_menulist(10,34, 4, "config", "state", "lock", "info");
        } else {
            menu_draw_button(false);
            unroll_menulist(0,34, 4);
        }
    }
    menu_opened = !menu_opened;
  }
}