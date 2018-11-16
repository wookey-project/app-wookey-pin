/**
 * @file main.c
 *
 * \brief Main of dummy
 *
 */

#include "autoconf.h"


#include "api/syscall.h"
#include "api/print.h"

#if CONFIG_APP_PIN_INPUT_USART

# include "libusart.h"
# include "libconsole.h"
# include "libshell.h"

#elif CONFIG_APP_PIN_INPUT_SCREEN

# include "gui_pin.h"
# include "gui_menu.h"
#ifdef MODE_FW
# include "lock2.h"
#endif
#ifdef MODE_FW
# include "fail.h"
#endif
# include "libspi.h"
# include "libtouch.h"
# include "libtft.h"

#elif CONFIG_APP_PIN_INPUT_MOCKUP
  /* nothing to include */
#else
# error "please specify input mode"
#endif


#include "wookey_ipc.h"
#include "main.h"
#include "handlers_generic.h"

static uint8_t id_smart;

uint8_t get_smart_id(void)
{
    return id_smart;
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

#ifdef CONFIG_APP_PIN_INPUT_USART

    /* USART mode: eclaring an USART device to interact */

    ret = shell_early_init(CONFIG_APP_PIN_INPUT_USART_ID, 115200);
    if (ret != SYS_E_DONE) {
        printf("ERROR: registering USART through libshell failed.\n");
        while (1);
    }
    printf("Registered USART through libshell %d\n", ret);

#elif CONFIG_APP_PIN_INPUT_SCREEN

    /* graphical mode: declaring graphical devices */

    if (spi1_early_init()) {
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
#elif CONFIG_APP_PIN_INPUT_MOCKUP

    /* mockup mode: no external I/O */

#else
# error "input mode must be set"
#endif

    /* get back smart task id, as we communicate with it */
    ret = sys_init(INIT_GETTASKID, "smart", &id_smart);
    if (ret != SYS_E_DONE) {
        // DFU mode ?
        printf("DFU mode, looking for smart\n");
        ret = sys_init(INIT_GETTASKID, "dfusmart", &id_smart);
    } else {
        printf("FW mode, looking for smart\n");
    }
    if (ret != SYS_E_DONE) {
      printf("gettaskid fails with %s\n", strerror(ret));
      goto err;
    } else {
      printf("gettaskid ends with %s\n", strerror(ret));
      printf("smart is task %x !\n", id_smart);
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

#ifdef CONFIG_APP_PIN_INPUT_SCREEN
    if (tft_init()) {
        printf("error during TFT initialization!\n");
    }
    if (touch_init()) {
        printf("error during Touch initialization!\n");
    }
    cb_menu_callbacks_t callbacks = {
        .handle_settings = handle_settings_request,
        .handle_auth     = handle_authentication,
        .handle_pin_cmd  = handle_full_pin_cmd_request
    };
    menu_init(240, 320, &callbacks);

    tft_fill_rectangle(0,240,0,320,249,249,249);
#ifdef MODE_FW
    tft_rle_image(0,0,lock_width,lock_height,lock_colormap,lock,sizeof(lock));
#else
	tft_setfg(0,0,0);
	tft_setbg(249,249,249);
	tft_set_cursor_pos(0,260);
	tft_puts(" Wookey LOCKED");
#endif

#elif CONFIG_APP_PIN_INPUT_USART
    /*
     * in the specific case of usart, the readline() function allocate
     * the string. The pin variable is then not a table but a string pointer
     */
    shell_init();
    if (ret == 0) {
        printf("USART4 is now configured !\n");
    } else {
        printf("error during configuration of USART4\n");
    }
    console_log("[USART4] Pin initialized usart...\n");
    console_flush();
#elif CONFIG_APP_PIN_INPUT_MOCKUP
    /* nothing to do */
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
    if (   ipc_sync_cmd.magic == MAGIC_TASK_STATE_RESP
        && ipc_sync_cmd.state == SYNC_ACKNOWLEDGE) {
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

#if    CONFIG_APP_PIN_INPUT_USART

    /*
     * nothing to do. The advanced settings and user interaction loop
     * is only supported in graphical mode.
     */
    do {
    sys_yield();
    } while (1);

#elif CONFIG_APP_PIN_INPUT_SCREEN

    /*
     * initialize the nominal graphic interace and wait for user
     * events. This is the pin task main loop, managing all
     * user interactions on screen
     */

    menu_get_events();
    /* should return to do_endoftask() */

#elif CONFIG_APP_PIN_INPUT_MOCKUP

    /* nothing to do */
    do {
       sys_yield();
    } while (1);

#else
# error "input mode must be set"
#endif
    return 0;
err:
    while (1);
    return 1;
}
