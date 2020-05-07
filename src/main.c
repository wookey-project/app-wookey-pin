/**
 * @file main.c
 *
 * \brief Main of dummy
 *
 */

#include "autoconf.h"


#include "libc/syscall.h"
#include "libc/stdio.h"
#include "libc/nostd.h"
#include "libc/string.h"

#if MODE_DFU

# if CONFIG_APP_PIN_DFU_INPUT_USART

#  include "libusart.h"
#  include "libconsole.h"

# elif CONFIG_APP_PIN_DFU_INPUT_SCREEN

#  include "gui_pin.h"
#  include "lock2.h"
#  include "fail.h"
#  include "libspi.h"
#  include "libtouch.h"
#  include "libtft.h"
#  include "libgui.h"
#  include "gui.h"

# elif CONFIG_APP_PIN_DFU_INPUT_MOCKUP

#  if CONFIG_APP_PIN_DFU_MOCKUP_SHOW_MENU

#   include "gui_pin.h"
#   include "lock2.h"
#   include "fail.h"
#   include "libspi.h"
#   include "libtouch.h"
#   include "libtft.h"
#   include "libgui.h"
#   include "gui.h"

#  endif

# endif

#else

# if CONFIG_APP_PIN_FW_INPUT_USART

#  include "libusart.h"
#  include "libconsole.h"

# elif CONFIG_APP_PIN_FW_INPUT_SCREEN

#  include "gui_pin.h"
#  include "lock2.h"
#  include "fail.h"
#  include "libspi.h"
#  include "libtouch.h"
#  include "libtft.h"
#  include "libgui.h"
#  include "gui.h"

# elif CONFIG_APP_PIN_FW_INPUT_MOCKUP

#  if CONFIG_APP_PIN_FW_MOCKUP_SHOW_MENU

#   include "gui_pin.h"
#   include "lock2.h"
#   include "fail.h"
#   include "libspi.h"
#   include "libtouch.h"
#   include "libtft.h"
#   include "libgui.h"
#   include "gui.h"

#  endif

# endif

#endif


#include "libfw.h"
#include "wookey_ipc.h"
#include "main.h"
#include "handlers_generic.h"

static uint8_t id_smart;
static uint8_t id_crypto;

uint8_t get_smart_id(void)
{
    return id_smart;
}

uint8_t get_crypto_id(void)
{
    return id_crypto;
}


/******************************************************
 * The task main function, called by do_starttask().
 * This is the user application code entrypoint after
 * the libstd execution.
 * From now on, the SSP is active.
 *****************************************************/
int _main(uint32_t task_id)
{
    uint8_t id;
    uint8_t ret;

    struct sync_command      ipc_sync_cmd;

    /*******************************************
     * initialization phase: declaring devices
     *******************************************/

    printf("Hello ! I'm pin, my id is %x\n", task_id);

#if APP_PIN_INPUT_USART

    /* USART mode: eclaring an USART device to interact */

    ret = console_early_init(CONFIG_APP_PIN_INPUT_USART_ID, 115200);
    if (ret != SYS_E_DONE) {
        printf("ERROR: registering USART through libshell failed.\n");
        while (1);
    }
    printf("Registered USART through libshell %d\n", ret);

#elif APP_PIN_INPUT_SCREEN

    /* graphical mode: declaring graphical devices */

#if CONFIG_WOOKEY_V1
    if (spi1_early_init()) {
#elif defined(CONFIG_WOOKEY_V2) || defined(CONFIG_WOOKEY_V3)
    if (spi2_early_init()) {
#else
# error "unsupported board for graphical interface"
#endif
        printf("ERROR: registering SPI1 failed.\n");
        while (1)
            ;
    }
    if (tft_early_init()) {
        printf("ERROR: registering TFT failed.\n");
        while (1)
            ;
    }
    if (touch_early_init()) {
        printf("ERROR: registering Touchscreen failed.\n");
        while (1)
            ;
    }

    printf("Registered SPI1, Touchscreen and TFT.\n");
#elif APP_PIN_INPUT_MOCKUP
    /* mockup mode: external I/O only if requested */

#else
# error "input mode must be set"
#endif

    /* get back smart task id, as we communicate with it */

#ifndef MODE_DFU
        printf("current mode is FW mode\n");
        ret = sys_init(INIT_GETTASKID, "smart", &id_smart);
        if (ret != SYS_E_DONE) {
            goto err;
        }
        ret = sys_init(INIT_GETTASKID, "crypto", &id_crypto);
        if (ret != SYS_E_DONE) {
            goto err;
        }
#else
        printf("current mode is DFU mode\n");
        ret = sys_init(INIT_GETTASKID, "dfusmart", &id_smart);
        if (ret != SYS_E_DONE) {
            goto err;
        }
        ret = sys_init(INIT_GETTASKID, "dfucrypto", &id_crypto);
        if (ret != SYS_E_DONE) {
            goto err;
        }
#endif
    if (id_smart == 0) {
        printf("error while getting id smart!\n");
        goto err;
    }

    /*******************************************
     * end of the initialization phase
     * no ressource can be declared from now on
     *******************************************/

    printf("set init as done\n");
    if ((ret = sys_init(INIT_DONE))) {
        printf("sys_init returns %s !\n", strerror(ret));
        goto err;
    }

    /*******************************************
     * Nominal phase startup: initialize devices,
     * which are now memory-mapped
     *******************************************/

#if APP_PIN_INPUT_SCREEN
    if (tft_init()) {
        printf("error during TFT initialization!\n");
    }
    if (touch_init()) {
        printf("error during Touch initialization!\n");
    }
    gui_init(240,320, handle_external_events);
#ifdef MODE_DFU
        rgb_color_t clr;
        /* in DFU mode, we update the pin pad colormap */
        clr.r = 179; clr.g = 118; clr.b = 197;
        pin_set_pad_color(&clr);
        clr.r = 252; clr.g = 225; clr.b = 59;
        pin_set_nextprev_color(&clr);
        clr.r = 132; clr.g = 2; clr.b = 180;
        pin_set_bg_color(&clr);
        init_dfu_gui();
#else
        init_fw_gui();
#endif

    if (is_in_fw_mode()) {
        tft_fill_rectangle(0,240,0,320,lock_colormap[0],lock_colormap[1],lock_colormap[2]);
        tft_rle_image(0,0,lock_width,lock_height,lock_colormap,lock,sizeof(lock));
    } else {
        tft_fill_rectangle(0,240,0,320,lock_dfu_colormap[0],lock_dfu_colormap[1],lock_dfu_colormap[2]);
        tft_rle_image(0,0,lock_width,lock_height,lock_dfu_colormap,lock,sizeof(lock));
    }

#elif APP_PIN_INPUT_USART
    /*
     * in the specific case of usart, the readline() function allocate
     * the string. The pin variable is then not a table but a string pointer
     */
    console_init();
    if (ret == 0) {
        printf("USART4 is now configured !\n");
    } else {
        printf("error during configuration of USART4\n");
    }
    console_log("[USART4] Pin initialized usart...\n");
#elif APP_PIN_INPUT_MOCKUP
 /* no screen config in mokcup */
#else
# error "input type must be set"
#endif


    /*******************************************
     * let's syncrhonize with other tasks
     * This is the first pass of the syncrhonization
     * mechanism of wookey. All tasks synchronize themselves
     * at their end of init. Pin & Smart are the first
     * tasks starting their execution, other tasks will
     * be executed later, waiting for smart to allow them
     * to continue their execution. Here, smart is
     * the lonely manager of the overall task scheduling
     * through IPC interactions.
     * This behavior stops at the end of the authentication
     * phase.
     *******************************************/
    logsize_t size = sizeof (struct sync_command);

    printf("sending end_of_init syncrhonization to smart\n");
    ipc_sync_cmd.magic = MAGIC_TASK_STATE_CMD;
    ipc_sync_cmd.state = SYNC_READY;

    do {
        ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
    } while (ret != SYS_E_DONE);

    /* Now wait for Acknowledge from Smart */
    id = id_smart;

    do {
        ret = sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd);
    } while (ret != SYS_E_DONE);
    if (   (ipc_sync_cmd.magic == MAGIC_TASK_STATE_RESP)
        && (ipc_sync_cmd.state == SYNC_ACKNOWLEDGE)) {
        printf("smart has acknowledge end_of_init, continuing\n");
    }


    /*******************************************
     * Starting authentication phase
     *******************************************/

    if (handle_authentication(FULL_AUTHENTICATION_MODE)) {
        goto err;
    }


    /*************************************************************
     * End of the full authentication mechanism.
     *
     * Starting of PIN main loop, which permit to support various
     * menus (settings, state, wipe, etc.)
     ************************************************************/


#if    APP_PIN_INPUT_USART

    /*
     * nothing to do. The advanced settings and user interaction loop
     * is only supported in graphical mode.
     */
    bool refresh = false;
    while (1) {
        handle_external_events(&refresh);
        sys_yield();
    }

#elif APP_PIN_INPUT_SCREEN

    /*
     * initialize the nominal graphic interace and wait for user
     * events. This is the pin task main loop, managing all
     * user interactions on screen
     */
    gui_get_events();

//        menu_get_events();
    /* should return to do_endoftask() */

#elif APP_PIN_INPUT_MOCKUP

    /* nothing to do except handling IPC */
    bool refresh = false;
    while (1) {
        handle_external_events(&refresh);
        sys_sleep(1000, SLEEP_MODE_INTERRUPTIBLE);
    }

#else
# error "input mode must be set"
#endif

    return 0;
err:
    while (1);
    return 1;
}
