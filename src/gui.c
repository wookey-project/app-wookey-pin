#include "autoconf.h"
#include "api/types.h"
#include "api/print.h"
#include "libgui.h"
#include "gui.h"
#include "handlers_generic.h"

# include "img/smiley.h"
# include "img/settings.h"
# include "img/wipe.h"
# include "img/lock.h"
# include "img/unlock.h"
# include "img/state.h"
# include "img/pin.h"
# include "img/petpin.h"
# include "img/petname.h"
# include "img/return.h"
# include "img/massstorage.h"
# include "img/smartcard.h"
# include "img/dfu.h"


menu_desc_t main_menu;
menu_desc_t status_menu;
menu_desc_t lock_menu;
menu_desc_t wipe_menu;
menu_desc_t settings_menu;
menu_desc_t dfu_menu;

tile_desc_t main_status_tile;
tile_desc_t main_lock_tile;
tile_desc_t main_settings_tile;
tile_desc_t main_wipe_tile;

tile_desc_t status_main_tile;
tile_desc_t status_return_tile;

tile_desc_t settings_set_userpin_tile;
tile_desc_t settings_set_petpin_tile;
tile_desc_t settings_set_petname_tile;

tile_desc_t dfu_main_tile;

#define TILE_STATUS_BG   .r = 53, .g = 88,  .b = 157
#define TILE_SETTINGS_BG .r =  0, .g = 159, .b = 155
#define TILE_WIPE_BG     .r = 231,.g = 92,  .b = 76
#define TILE_LOCK_BG     .r = 141, .g = 78, .b = 159
#define TILE_STATE_BG     49, 173,  89
#define TILE_DFU_BG      .r = 252, .g = 225, .b =  59
#define TILE_RETURN_BG   .r = 133, .g = 135, .b = 132
#define TILE_FG          .r = 255, .g = 255, .b = 255


static void cb_handle_graphical_event(tile_desc_t tile)
{
    if (tile == settings_set_userpin_tile) {
        handle_settings_request(BOX_SET_USERPIN);
        if (handle_authentication(LITE_AUTHENTICATION_MODE)) {
            printf("fail to handle authentication ! leaving...\n");
        }
        if (handle_full_pin_cmd_request()) {
            printf("fail to handle pin cmd request\n");
        }
    } else if (tile == settings_set_petpin_tile) {
        handle_settings_request(BOX_SET_PETPIN);
        if (handle_authentication(LITE_AUTHENTICATION_MODE)) {
            printf("fail to handle authentication ! leaving...\n");
        }
        if (handle_full_pin_cmd_request()) {
            printf("fail to handle pin cmd request\n");
        }
    } else if (tile ==  settings_set_petname_tile) {
        handle_settings_request(BOX_SET_PETNAME);
        if (handle_authentication(LITE_AUTHENTICATION_MODE)) {
            printf("fail to handle authentication ! leaving...\n");
        }
        if (handle_full_pin_cmd_request()) {
            printf("fail to handle pin cmd request\n");
        }
    } else if (tile == main_lock_tile) {
        handle_settings_request(BOX_LOCK);
    }
}


void init_dfu_gui(void)
{
    tile_action_t   action;
    gui_error_t       ret;

    /************ decaring menus ************/
    ret = gui_declare_menu("MAIN", &main_menu);
    if (ret != GUI_ERR_NONE) {
        printf("error while declaring menu: %d\n", ret);
    }
    ret = gui_declare_menu("STATUS", &status_menu);
    if (ret != GUI_ERR_NONE) {
        printf("error while declaring menu: %d\n", ret);
    }
    ret = gui_declare_menu("LOCK", &lock_menu);
    if (ret != GUI_ERR_NONE) {
        printf("error while declaring menu: %d\n", ret);
    }
    ret = gui_declare_menu("WIPE", &wipe_menu);
    if (ret != GUI_ERR_NONE) {
        printf("error while declaring menu: %d\n", ret);
    }
    ret = gui_declare_menu("SETS", &settings_menu);
    if (ret != GUI_ERR_NONE) {
        printf("error while declaring menu: %d\n", ret);
    }
    ret = gui_declare_menu("DFU", &dfu_menu);
    if (ret != GUI_ERR_NONE) {
        printf("error while declaring menu: %d\n", ret);
    }





    /************ decaring tiles ************/

    /*
     * main menu tiles
     */
    {
        tile_colormap_t colormap[2] = {
            { TILE_STATUS_BG },
            { TILE_FG }
        };

        tile_icon_t icon = {
            .data = smiley,
            .size = sizeof(smiley)
        };

        action.type = TILE_ACTION_MENU;
        action.target.menu = status_menu;

        ret = gui_declare_tile(main_menu, colormap, TILE_WIDTH_FULL, TILE_HEIGHT_DOUBLE, &action, "status", &icon, &main_lock_tile);
        if (ret != GUI_ERR_NONE) {
            printf("error while declaring tile: %d\n", ret);
        }
    }
    {
        tile_colormap_t colormap[2] = {
            { TILE_LOCK_BG },
            { TILE_FG }
        };

        tile_icon_t icon = {
            .data = nlock,
            .size = sizeof(nlock)
        };

        action.type = TILE_ACTION_CB;
        action.target.callback = cb_handle_graphical_event;


        ret = gui_declare_tile(main_menu, colormap, TILE_WIDTH_THIRD, TILE_HEIGHT_STD, &action, "lock", &icon, &main_lock_tile);
        if (ret != GUI_ERR_NONE) {
            printf("error while declaring tile: %d\n", ret);
        }
    }
    {
        tile_colormap_t colormap[2] = {
            { TILE_SETTINGS_BG },
            { TILE_FG }
        };

        tile_icon_t icon = {
            .data = settings,
            .size = sizeof(settings)
        };

        action.type = TILE_ACTION_MENU;
        action.target.menu = settings_menu;


        ret = gui_declare_tile(main_menu, colormap, TILE_WIDTH_THIRD, TILE_HEIGHT_STD, &action, "sets", &icon, &main_settings_tile);
        if (ret != GUI_ERR_NONE) {
            printf("error while declaring tile: %d\n", ret);
        }
    }
    {
        tile_colormap_t colormap[2] = {
            { TILE_WIPE_BG },
            { TILE_FG }
        };

        tile_icon_t icon = {
            .data = wipe,
            .size = sizeof(wipe)
        };

        action.type = TILE_ACTION_MENU;
        action.target.menu = wipe_menu;


        ret = gui_declare_tile(main_menu, colormap, TILE_WIDTH_THIRD, TILE_HEIGHT_STD, &action, "wipe", &icon, &main_wipe_tile);
        if (ret != GUI_ERR_NONE) {
            printf("error while declaring tile: %d\n", ret);
        }
    }

    /*
     * status menu tiles
     */
    {
        tile_colormap_t colormap[2] = {
            { TILE_STATUS_BG },
            { TILE_FG }
        };

        action.type = TILE_ACTION_NONE;

        ret = gui_declare_tile(status_menu, colormap, TILE_WIDTH_FULL, TILE_HEIGHT_DOUBLE, &action, "Ceci est un test de texte sur\nplusieurs lignes", 0, &status_main_tile);
        if (ret != GUI_ERR_NONE) {
            printf("error while declaring tile: %d\n", ret);
        }
    }
    {
        tile_colormap_t colormap[2] = {
            { TILE_RETURN_BG },
            { TILE_FG }
        };

        tile_icon_t icon = {
            .data = returning,
            .size = sizeof(returning)
        };

        action.type = TILE_ACTION_MENU;
        action.target.menu = main_menu;

        ret = gui_declare_tile(status_menu, colormap, TILE_WIDTH_FULL, TILE_HEIGHT_STD, &action, 0, &icon, &status_return_tile);
        if (ret != GUI_ERR_NONE) {
            printf("error while declaring tile: %d\n", ret);
        }
    }

    /*
     * settings menu tiles
     */
    {
        tile_colormap_t colormap[2] = {
            { TILE_SETTINGS_BG },
            { TILE_FG }
        };

        tile_icon_t icon = {
            .data = userpin,
            .size = sizeof(userpin)
        };


        action.type = TILE_ACTION_CB;
        action.target.callback = cb_handle_graphical_event;

        ret = gui_declare_tile(settings_menu, colormap, TILE_WIDTH_FULL, TILE_HEIGHT_STD, &action, "set user pin", &icon, &settings_set_userpin_tile);
        if (ret != GUI_ERR_NONE) {
            printf("error while declaring tile: %d\n", ret);
        }
    }
    {
        tile_colormap_t colormap[2] = {
            { TILE_SETTINGS_BG },
            { TILE_FG }
        };

        tile_icon_t icon = {
            .data = petpin,
            .size = sizeof(petpin)
        };


        action.type = TILE_ACTION_CB;
        action.target.callback = cb_handle_graphical_event;

        ret = gui_declare_tile(settings_menu, colormap, TILE_WIDTH_FULL, TILE_HEIGHT_STD, &action, "set pet pin", &icon, &settings_set_petpin_tile);
        if (ret != GUI_ERR_NONE) {
            printf("error while declaring tile: %d\n", ret);
        }
    }
    {
        tile_colormap_t colormap[2] = {
            { TILE_SETTINGS_BG },
            { TILE_FG }
        };

        tile_icon_t icon = {
            .data = petname,
            .size = sizeof(petname)
        };

        action.type = TILE_ACTION_CB;
        action.target.callback = cb_handle_graphical_event;

        ret = gui_declare_tile(settings_menu, colormap, TILE_WIDTH_FULL, TILE_HEIGHT_STD, &action, "set pet name", &icon, &settings_set_petname_tile);
        if (ret != GUI_ERR_NONE) {
            printf("error while declaring tile: %d\n", ret);
        }
    }

    {
        tile_colormap_t colormap[2] = {
            { TILE_RETURN_BG },
            { TILE_FG }
        };

        tile_icon_t icon = {
            .data = returning,
            .size = sizeof(returning)
        };

        action.type = TILE_ACTION_MENU;
        action.target.menu = main_menu;

        ret = gui_declare_tile(settings_menu, colormap, TILE_WIDTH_FULL, TILE_HEIGHT_STD, &action, 0, &icon, &status_return_tile);
        if (ret != GUI_ERR_NONE) {
            printf("error while declaring tile: %d\n", ret);
        }
    }
    /*
     * dfu menu tiles
     */
    {
        tile_colormap_t colormap[2] = {
            { TILE_DFU_BG },
            { TILE_FG }
        };

        tile_icon_t icon = {
            .data = dfu,
            .size = sizeof(dfu)
        };


        action.type = TILE_ACTION_NONE;

        ret = gui_declare_tile(dfu_menu, colormap, TILE_WIDTH_FULL, TILE_HEIGHT_TRIPLE, &action, "dfu in progress", &icon, &dfu_main_tile);
        if (ret != GUI_ERR_NONE) {
            printf("error while declaring tile: %d\n", ret);
        }
    }


    ret = gui_declare_default_menu(main_menu);

    return;
}
