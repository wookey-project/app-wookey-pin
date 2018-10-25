/**
 * @file main.c
 *
 * \brief Main of dummy
 *
 */

#define CONFIG_LOGO_LOCK 0

#include "api/syscall.h"
#include "api/print.h"
#include "libusart.h"

#ifdef CONFIG_APP_PIN_INPUT_USART
# include "libconsole.h"
#else
# include "pin.h"
# if CONFIG_LOGO_LOCK
#  include "lock.h"
# else
#  include "lock2.h"
# endif
# include "menu.h"
# include "fail.h"
//#include "peur.h"
# include "libspi.h"
# include "libtouch.h"
# include "libtft.h"
#endif

#include "libshell.h"
#include "ipc_proto.h"
#include "autoconf.h"


#define PIN_LENGTH 4

static void my_irq_handler(void);

void cb_usart_irq(uint32_t sr, uint32_t dr)
{
    sr = sr;
    dr = dr;
    // nothing done for input data from USART...
}

/************************************
 * IPC Communication protocol, basics
 ***********************************/


/*
 * We use the local -fno-stack-protector flag for main because
 * the stack protection has not been initialized yet.
 */
int _main(uint32_t task_id)
{
    uint8_t id_smart;
    uint8_t id;
    uint8_t ret;

    struct sync_command      ipc_sync_cmd;
    struct sync_command_data ipc_sync_cmd_data;

    printf("Hello ! I'm pin, my id is %x\n", task_id);

#ifdef CONFIG_APP_PIN_INPUT_USART
    ret = shell_early_init(CONFIG_APP_PIN_INPUT_USART_ID, 115200);
    if (ret != SYS_E_DONE) {
        printf("ERROR: registering USART through libshell failed.\n");
        while (1)
            ;
    }
    printf("Registered USART through libshell %d\n", ret);
#else /* graphical touchscreen */
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
#endif

    ret = sys_init(INIT_GETTASKID, "smart", &id_smart);
    if (ret != SYS_E_DONE) {
      printf("gettaskid fails with %s\n", strerror(ret));
    } else {
      printf("gettaskid ends with %s\n", strerror(ret));
      printf("smart is task %x !\n", id_smart);
    }

    printf("set init as done\n");
    ret = sys_init(INIT_DONE);
    printf("sys_init returns %s !\n", strerror(ret));


    /*******************************************
     * Initialize the screen and print logo at boot time
     *******************************************/

#ifdef CONFIG_APP_PIN_INPUT_SCREEN
    // FIXME fixed to 8 char max... should be configurable
    char pin[9] = { 0 };

    if (tft_init()) {
        printf("error during TFT initialization!\n");
    }
    if (touch_init()) {
        printf("error during Touch initialization!\n");
    }

    tft_fill_rectangle(0,240,0,320,249,249,249);
    tft_rle_image(0,0,lock_width,lock_height,lock_colormap,lock,sizeof(lock));

# if CONFIG_LOGO_LOCK
	tft_setfg(0,0,0);
	tft_setbg(249,249,249);
	tft_set_cursor_pos(0,260);
	tft_puts(" Wookey LOCKED");
# endif
#else
    char * pin = 0;

    shell_init();
    if (ret == 0) {
        printf("USART4 is now configured !\n");
    } else {
        printf("error during configuration of USART4\n");
    }
    console_log("[USART4] Pin initialized usart...\n");
    console_flush();
#endif


    /*******************************************
     * let's syncrhonize with other tasks
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
     * End of full task end_of_init synchronization
     *******************************************/



    /*******************************************
     * Wait for smart to ask for PIN code
     * (when Smartcard is connected)
     *******************************************/
    uint32_t pin_len = 0;
    /* FIXME: pin size 8 */

    id = id_smart;
    size = sizeof (struct sync_command);
    do {
        ret = sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd);
    } while (ret != SYS_E_DONE);

    if (   ipc_sync_cmd.magic == MAGIC_CRYPTO_PIN_CMD
        && ipc_sync_cmd.state == SYNC_ASK_FOR_DATA) {
        printf("smart is asking for pin, asking user...\n");
    }

#ifdef CONFIG_APP_PIN_INPUT_USART
    console_log("Enter pin code please\n");
    console_flush();
    shell_readline(&pin, &pin_len); /*FIXME: update API, set string... and size */
    console_log("pin registered!\n");
    console_flush();
#else
    tft_fill_rectangle_unlocked(0,240,0,320,0,255,0);
    tft_fill_rectangle_unlocked(0,240,0,320,255,0,0);
    //tft_fill_rectangle_unlocked(0,240,0,320,0,255,0);
    //tft_fill_rectangle_unlocked(0,240,0,320,0,0,255);
    tft_setfg(200,200,200);
    tft_setbg(5,0,5);
    tft_set_cursor_pos(0,29);
	
	tft_fill_rectangle(0,240,0,320,0,0,0);
	tft_setfg(200,200,200);
	tft_setbg(5,0,5);
	tft_set_cursor_pos(0,0);
	tft_puts("  Please enter ");
	tft_set_cursor_pos(0,29);
	tft_puts("    PIN CODE");

    pin_len=PIN_LENGTH;

	get_pin(0,240,60,320,pin,pin_len);
	{
	  printf("Pin GIVEN!\n");
      printf("pin is %s\n", pin);
	}
    // FIXME win vs loozer screen, should be printed after
    // response from SMART task

#endif
    ipc_sync_cmd_data.magic = MAGIC_CRYPTO_PIN_RESP;
    ipc_sync_cmd_data.state = SYNC_DONE;
    ipc_sync_cmd_data.data_size = (uint8_t)pin_len;
    memset(&ipc_sync_cmd_data.data.u8, 0x0, 32);
    memcpy(&ipc_sync_cmd_data.data.u8, pin, pin_len);

    do {
        size = sizeof(struct sync_command_data);
        ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd_data);
    } while (ret != SYS_E_DONE);
    printf("Pin sent to smart\n");


    // waiting for smart response, to print the result on the screen
    id = id_smart;
    size = sizeof(struct sync_command);
    sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd);
   
    if (ipc_sync_cmd.magic == MAGIC_CRYPTO_PIN_RESP && ipc_sync_cmd.state == SYNC_DONE)
    {
#ifdef CONFIG_APP_PIN_INPUT_USART
        console_log("valid PIN, continuing...\n");
        console_flush();
#else
		tft_fill_rectangle(0,240,0,320,249,249,249);
        draw_background();
	  //tft_fill_rectangle(0,240,0,320,0,250,0);
	  //tft_set_cursor_pos(50,130);
	  //tft_puts("Winner!");
#endif
    }
	else
	{
#ifdef CONFIG_APP_PIN_INPUT_USART
        console_log("invalid PIN !!!\n");
        console_flush();
#else
    tft_fill_rectangle(0,240,0,320,249,249,249);
    tft_rle_image(0,0,lock_width,lock_height,lock_colormap,lock,sizeof(lock));
//    tft_fill_rectangle(20,20+fail_height,150,150+fail_width,255,255,255);
    tft_rle_image(150,20,fail_width,fail_height,fail_colormap,fail,sizeof(fail));
#endif
	}

    /*************************************************************
     * Starting of PIN main loop, now that PIN has been delivered
     ************************************************************/

    draw_background();
    draw_menu(240,320, true);

    /* avoid exiting main thread */
    menu_get_events();
    /* should return to do_endoftask() */
    return 0;
}
