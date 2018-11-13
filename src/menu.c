#include "autoconf.h"
#include "menu.h"
#include "libtft.h"
#include "api/string.h"
#include "api/print.h"
#include "api/types.h"
#include "api/syscall.h"
#include "ipc_proto.h"
#include "libtouch.h"
//#include "bg.h"
#include "rtc.h"
#include "pin.h"
#include "main.h"

// images
#include "img/smiley.h"
#include "img/settings.h"
#include "img/wipe.h"
#include "img/lock.h"
#include "img/unlock.h"
#include "img/state.h"
#include "img/pin.h"
#include "img/petpin.h"
#include "img/petname.h"
#include "img/return.h"
#include "img/massstorage.h"
#include "img/smartcard.h"

extern const int font_width;
extern const int font_height;
extern const int font_blankskip;
extern uint8_t id_smart;


static const struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} box_tab[] = {
  { 227,  52, 52 },
  { 132, 194, 74 },
  { 191, 121, 183 }
};

extern const int font_width;
extern const int font_height;
extern const int font_blankskip;


typedef enum {
    BOX_STATUS,
    BOX_SETTINGS,
    BOX_WIPE,
    BOX_LOCK,
    BOX_UNLOCK,
    BOX_STATE,
    BOX_SET_PETPIN,
    BOX_SET_PETNAME,
    BOX_SET_USERPIN,
    BOX_WIPE_AUTHKEYS,
    BOX_WIPE_SMARTCARD,
    BOX_WIPE_STORAGE,
    BOX_RETURN,
    BOX_UNDEFINED
} t_box;

typedef enum {
    MENU_MAIN     = 0,
    MENU_SETTINGS = 1,
    MENU_WIPE     = 2,
    MENU_STATUS   = 3
} t_current_menu;

uint32_t pin_remaining_tries = CONFIG_AUTH_TOKEN_MAX_PIN;

void update_remaining_tries(uint32_t val)
{
    pin_remaining_tries = val;
}

static volatile t_current_menu menu = MENU_MAIN;

t_box get_box(int x, int y)
{
    t_box box = BOX_UNDEFINED;
    switch (menu) {
        case MENU_MAIN:
        {
            if (x > 0 && x < 120 && y > 0 && y < 200) {
                box = BOX_STATUS;
            }
            if (x > 120 && x < 240 && y > 0 && y < 100) {
                box = BOX_SETTINGS;
            }
            if (x > 120 && x < 240 && y > 100 && y < 200) {
                box = BOX_WIPE;
            }
            if (x > 0 && x < 80 && y > 200 && y < 300) {
                box = BOX_LOCK;
            }
            if (x > 80 && x < 160 && y > 200 && y < 300) {
                box = BOX_UNLOCK;
            }
            if (x > 160 && x < 240 && y > 200 && y < 300) {
                box = BOX_STATE;
            }
            break;
        }
        case MENU_STATUS:
        {
            if (x > 0 && x < 240 && y > 270 && y < 320) {
                box = BOX_RETURN;
            }
            break;
        }

        case MENU_SETTINGS:
        {
            if (x > 0 && x < 240 && y > 0 && y < 90) {
                box = BOX_SET_PETPIN;
            }
            if (x > 0 && x < 240 && y > 90 && y < 180) {
                box = BOX_SET_PETNAME;
            }
            if (x > 0 && x < 240 && y > 180 && y < 270) {
                box = BOX_SET_USERPIN;
            }
            if (x > 0 && x < 240 && y > 270 && y < 320) {
                box = BOX_RETURN;
            }
            break;
        }
        case MENU_WIPE:
        {
            if (x > 0 && x < 240 && y > 0 && y < 90) {
                box = BOX_WIPE_AUTHKEYS;
            }
            if (x > 0 && x < 240 && y > 90 && y < 180) {
                box = BOX_WIPE_SMARTCARD;
            }
            if (x > 0 && x < 240 && y > 180 && y < 270) {
                box = BOX_WIPE_STORAGE;
            }
            if (x > 0 && x < 240 && y > 270 && y < 320) {
                box = BOX_RETURN;
            }
            break;
        }

        default:
        {
            break;
        }
    }
    return box;
}

void draw_menubox2(int x1, int x2, int y1, int y2,
                   char *c,
                   uint8_t r, uint8_t g, uint8_t b,
                   const uint8_t *icon_colormap,
                   const uint8_t *icon,
                   const uint32_t icon_size)
{
    const int char_width = font_width/128;
    int posx;
  // create the box color
  tft_fill_rectangle(x1,x2,y1,y2,0,0,0);
  tft_fill_rectangle(x1+2,x2-2,y1+2,y2-2,r,g,b);
  // add the box icon
  tft_rle_image(x1 + (((x2 - x1)-45)/2),
                y1 + (((y2 - y1)-45)/2), 45, 45,
          icon_colormap, icon, icon_size);
  // add the box title
  //
  if (c) {
      posx=(x2-x1-strlen(c)*char_width)/2;
      tft_set_cursor_pos(x1+posx,(y1 + (((y2 - y1)-45)/2 + 48)));
      tft_setfg(255,255,255);
      tft_setbg(r,g,b);
      tft_puts(c);
  }
}

void draw_menu(int x, int y)
{
    const int char_width = font_width/128;
    int cury;
    x = x;
    y = y;
  
    draw_background();
    switch (menu) {
        case MENU_MAIN:
        {
            draw_menubox2(0,120,0,200,
                    "status\0",
                    53,88,157,
                    smiley_colormap,
                    smiley,
                    sizeof(smiley));
            draw_menubox2(120,240,0,100,
                    "setting\0",
                    0,159,155,
                    settings_colormap,
                    settings,
                    sizeof(settings));
            draw_menubox2(120,240,100,200,
                    "wipe\0",
                    231,92,76,
                    wipe_colormap,
                    wipe,
                    sizeof(wipe));
            draw_menubox2(0,80,200,300,
                    "lck\0",
                    141,78,159,
                    nlock_colormap,
                    nlock,
                    sizeof(nlock));
            draw_menubox2(80,160,200,300,
                    "unlck\0",
                    141,78,159,
                    unlock_colormap,
                    unlock,
                    sizeof(unlock));
            draw_menubox2(160,240,200,300,
                    "state\0",
                    49,173,89,
                    state_colormap,
                    state,
                    sizeof(state));
            break;
        }
        case MENU_STATUS:
        {
            cury = 0;
            // print status information
            tft_fill_rectangle(0,240,0,270,53,88,157);
            tft_setfg(255,255,255);
            tft_setbg(53,88,157);
            tft_set_cursor_pos(0,cury);
            tft_puts("crypto:");
            cury += font_height/2;
#if CONFIG_AES256_CBC_ESSIV
            tft_set_cursor_pos(240 - (14*char_width),cury);
            tft_puts("AES_CBC_ESSIV");
#else
            tft_set_cursor_pos(240 - (5*char_width),cury);
            tft_puts("3DES");
#endif
            cury += font_height/2;
            tft_set_cursor_pos(0,cury);
            tft_puts("Update:");

            cury += font_height/2;

            tft_set_cursor_pos(40,cury);
            tft_puts("DFU");
            tft_set_cursor_pos(240 - (4*char_width),cury);
#if CONFIG_FIRMWARE_DFU
            tft_puts(" on");
#else
            tft_puts("off");
#endif

            cury += font_height/2;

            tft_set_cursor_pos(40,cury);
            tft_puts("DB");
            tft_set_cursor_pos(240 - (4*char_width),cury);
#if CONFIG_FIRMWARE_MODE_DUAL_BANK
            tft_puts(" on");
#else
            tft_puts("off");
#endif

            cury += font_height/2;

            tft_set_cursor_pos(0,cury);
            tft_puts("FW version");

            cury += font_height/2;

            tft_set_cursor_pos(240 - (6*char_width),cury);
            tft_puts("0.0.1");

            cury += font_height/2;

            tft_set_cursor_pos(0,cury);
            tft_puts("Pin tries:");
            tft_set_cursor_pos(240 - (2*char_width),cury);
            tft_putc('0'+pin_remaining_tries);

            // return button
            draw_menubox2(0,240,270,320,
                    0,
                    133,135,132,
                    returning_colormap,
                    returning,
                    sizeof(returning));
            break;

        }

        case MENU_SETTINGS:
        {
            draw_menubox2(0,240,0,90,
                    "set pet pin\0",
                    0,159,155,
                    petpin_colormap,
                    petpin,
                    sizeof(petpin));
            draw_menubox2(0,240,90,180,
                    "set pet name\0",
                    0,159,155,
                    petname_colormap,
                    petname,
                    sizeof(petname));
            draw_menubox2(0,240,180,270,
                    "set user pin\0",
                    0,159,155,
                    userpin_colormap,
                    userpin,
                    sizeof(userpin));
            draw_menubox2(0,240,270,320,
                    0,
                    133,135,132,
                    returning_colormap,
                    returning,
                    sizeof(returning));
            break;

        }
        case MENU_WIPE:
        {
            draw_menubox2(0,240,0,90,
                    "Wipe auth keys\0",
                    231,92,76,
                    wipe_colormap,
                    wipe,
                    sizeof(wipe));
            draw_menubox2(0,240,90,180,
                    "Wipe smartcard\0",
                    231,92,76,
                    smartcard_colormap,
                    smartcard,
                    sizeof(smartcard));
            draw_menubox2(0,240,180,270,
                    "Wipe massstorage\0",
                    231,92,76,
                    massstorage_colormap,
                    massstorage,
                    sizeof(massstorage));
            draw_menubox2(0,240,270,320,
                    0,
                    133,135,132,
                    returning_colormap,
                    returning,
                    sizeof(returning));
            break;

        }
        default: {
            break;
        }
    }

}


void draw_menubox(int x1,int x2, int y1, int y2, char *c, uint8_t i)
{
  uint8_t r = 255, g = 255, b = 255;

  //const int char_width=font_width/128;
  int posx,posy;
  tft_setbg(box_tab[i % 3].r,box_tab[i % 3].g,box_tab[i % 3].b);
  //tft_setfg(0,0,0);
  tft_fill_rectangle(x1,x2,y1,y2,255-r,255-g,255-b);
  tft_fill_rectangle(x1+1,x2-1,y1+2,y2-2,box_tab[i % 3].r,box_tab[i % 3].g,box_tab[i % 3].b);

  tft_fill_rectangle(x1+1,x1 + 50,y1 + 2,y1+40,box_tab[i % 3].r - 25,box_tab[i % 3].g - 25,box_tab[i % 3].b - 25);
  tft_fill_rectangle(x1+1,x1 + 50,y1+40,y2-2,box_tab[i % 3].r - 50,box_tab[i % 3].g - 50,box_tab[i % 3].b - 50);

  posx=x1 + 70; // left align
  posy=(y2-y1-font_height/2)/2;
  tft_set_cursor_pos(x1+posx,posy+y1);
  tft_setfg(255-r,255-g,255-b);
  tft_puts(c);
}


void draw_background(void)
{
    tft_fill_rectangle(0,240,0,340,0,0,0);
}


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
  tft_fill_rectangle(0,34,0,34,255-r,255-g,255-b);

  tft_fill_rectangle(3,28,3,5,r,g,b);
  tft_fill_rectangle(3,28,10,12,r,g,b);
  tft_fill_rectangle(3,28,17,19,r,g,b);

  if (invert) {
    tft_fill_rectangle(0,34,28,34,200,0,0);
  }
}

//void unroll_menulist(uint32_t x, uint32_t y, uint8_t size)
//{
//    tft_rle_image(x,y,bg_width,((80+1)*size),bg_colormap,bg,sizeof(bg));
//}

extern uint32_t numexti;

#define MAX_PETNAME_LEN CONFIG_APP_PIN_MAX_PETNAME_LEN + 1

void menu_get_events(void)
{
    // initial menu draw

    char petpin_val[17] = { 0 };

//    char userpin_val[17] = { 0 };

    struct sync_command_data      ipc_sync_cmd = { 0 };
    logsize_t size;
    uint8_t ret;

    const int char_width = font_width/128;

    t_current_menu nextmenu = menu;

    while(1)
    {
        draw_menu(240,320);
        touch_read_X_DFR();/* Ensures that PenIRQ is enabled */
        /*
         * Between touch_read_X_DFR and touch_is_touched, we need to wait a little
         * or touch_is_touched() will return an invalid value
         */
        sys_sleep(10, SLEEP_MODE_INTERRUPTIBLE);

        /* Wait loop for touchscreen to be touched */
        while (!(touch_refresh_pos(),touch_is_touched())) {
            touch_enable_exti();
            if (menu == MENU_STATUS) {
                //printf("[not touched] main menu\n");
                uint64_t ts = 0;
                char *timestamp = 0;
                /* Here we sleep for 1 second in order to print the
                 * current timestamp on the menubar every second
                 * We can be interrupted by the touchscreen if needed
                 */
                sys_sleep(1000, SLEEP_MODE_INTERRUPTIBLE);
                //printf("[not touched] print timestamp\n");
                sys_get_systick(&ts, PREC_MILLI);
                timestamp = get_timestamp((uint32_t)(ts / 1000));
                tft_set_cursor_pos(120 - ((8*char_width)/2),240);
                tft_setfg(235,235,235);
                tft_setbg(53,88,157);
                tft_puts(timestamp);
            } else {
                sys_yield();
            }
        }
        //Follow the motion on the screen
        while (touch_refresh_pos(),touch_is_touched())
        {

            int posx,posy;
            //touch_refresh_pos();
            posy=touch_getx();
            posx=touch_gety();
            //printf("[touched] get pos: x:%d, y:%d\n", posx, posy);

            t_box box = get_box(posx, posy); 

            switch (box) {
                case BOX_SETTINGS:
                    {
                        printf("[touched] box settings pushed !\n");
                        nextmenu = MENU_SETTINGS;
                        break;
                    }
                case BOX_STATUS:
                    {
                        printf("[touched] box status pushed !\n");
                        nextmenu = MENU_STATUS;
                        break;
                    }

                case BOX_WIPE:
                    {
                        printf("[touched] box wipe pushed !\n");
                        nextmenu = MENU_WIPE;
                        break;
                    }
                case BOX_LOCK:
                    {
                        /* ask smart for lock. Smart will reset the board */
                        printf("[touched] box lock pushed !\n");
                        ipc_sync_cmd.magic = MAGIC_SETTINGS_LOCK;
                        ipc_sync_cmd.state = SYNC_WAIT;
                        size = sizeof(struct sync_command);

                        do {
                            ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
                        } while (ret != SYS_E_DONE);
                        break;
                    }
                case BOX_RETURN:
                    {
                        printf("[touched] box return pushed !\n");
                        nextmenu = MENU_MAIN;
                        break;
                    }
                case BOX_SET_PETPIN:
                    {
                        printf("[touched] box set petpin pushed !\n");
                        memset(petpin_val, 0x0, 17);


                        /* inform SMART that an authentication phase is requested */
                        ipc_sync_cmd.magic = MAGIC_SETTINGS_CMD;
                        ipc_sync_cmd.state = SYNC_WAIT;
                        ipc_sync_cmd.data.req.sc_type = SC_PET_PIN;
                        ipc_sync_cmd.data.req.sc_req = SC_REQ_MODIFY;
                        size = sizeof(struct sync_command_data);

                        do {
                            ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
                        } while (ret != SYS_E_DONE);

                        /* handle settings */
                        /* handle the authentication phase with smart */
                        if (handle_authentication(LITE_AUTHENTICATION_MODE)) {
                            printf("fail to handle authentication ! leaving...\n");
                            continue;
                        }
                        if (handle_full_pin_cmd_request()) {
                            printf("fail to handle pin cmd request\n");
                            continue;
                        }
                        break;
                    }
                case BOX_SET_PETNAME:
                    {
                        printf("[touched] box set petname pushed !\n");
                        memset(petpin_val, 0x0, 33);

                        /* inform SMART that an authentication phase is requested */
                        ipc_sync_cmd.magic = MAGIC_SETTINGS_CMD;
                        ipc_sync_cmd.state = SYNC_WAIT;
                        ipc_sync_cmd.data.req.sc_type = SC_PET_NAME;
                        ipc_sync_cmd.data.req.sc_req = SC_REQ_MODIFY;
                        size = sizeof(struct sync_command_data);

                        do {
                            ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
                        } while (ret != SYS_E_DONE);

                        /* handle the authentication phase with smart */
                        if (handle_authentication(LITE_AUTHENTICATION_MODE)) {
                            printf("fail to handle authentication ! leaving...\n");
                            continue;
                        }
                        if (handle_full_pin_cmd_request()) {
                            printf("fail to handle pin cmd request\n");
                            continue;
                        }
                        break;
                    }
                case BOX_SET_USERPIN:
                    {
                        printf("[touched] box set userpin pushed !\n");
//                        memset(userpin_val, 0x0, 17);

                        /* inform SMART that an authentication phase is requested */
                        ipc_sync_cmd.magic = MAGIC_SETTINGS_CMD;
                        ipc_sync_cmd.state = SYNC_WAIT;
                        ipc_sync_cmd.data.req.sc_type = SC_USER_PIN;
                        ipc_sync_cmd.data.req.sc_req = SC_REQ_MODIFY;
                        size = sizeof(struct sync_command_data);

                        do {
                            ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
                        } while (ret != SYS_E_DONE);

                        /* handle the authentication phase with smart */
                        if (handle_authentication(LITE_AUTHENTICATION_MODE)) {
                            printf("fail to handle authentication ! leaving...\n");
                            continue;
                        }
                        if (handle_full_pin_cmd_request()) {
                            printf("fail to handle pin cmd request\n");
                            continue;
                        }
                        break;
                    }

                default:
                    {
                        break;
                    }
            }
        }
        menu = nextmenu;
    }
}
