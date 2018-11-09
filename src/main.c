/**
 * @file main.c
 *
 * \brief Main of dummy
 *
 */

#include "autoconf.h"

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

#define PIN_DEBUG 0


#define PIN_MAX_LEN      16
#define PETNAME_MAX_LEN  32

static uint8_t id_smart;

#define PIN_LENGTH CONFIG_APP_PIN_MAX_PIN_LENGTH

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

typedef enum {
    PIN_MODE_PETPIN,
    PIN_MODE_USERPIN
} t_pin_mode;


/******************************************************************
 * Handle a PIN request from smart, including :
 * - Pin request
 * - Pin response
 * - Pin acknowledge
 * The function loop on the pin request while the pin is not valid
 * which is not a problem as the max pintries is managed by smart
 * while pin will wait for the next pin round.
 *****************************************************************/
static int handle_pin(char *pin, uint8_t pin_len, t_pin_mode mode)
{
    bool valid_pin = false;
    uint8_t id;
    uint8_t ret;
    logsize_t size = 0;

    struct sync_command      ipc_sync_cmd;
    struct sync_command_data ipc_sync_cmd_data;

    id = id_smart;
    size = sizeof(struct sync_command);

    do {
        valid_pin = false;

        /********************************************************
         * First wait for the PIN request from SMART
         *******************************************************/

        sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd);


        /* sanitation */
        if ((mode == PIN_MODE_PETPIN && ipc_sync_cmd.magic != MAGIC_CRYPTO_PETPIN_CMD) ||
            (mode == PIN_MODE_USERPIN && ipc_sync_cmd.magic != MAGIC_CRYPTO_PIN_CMD))
        {
            goto err;
        }


        /********************************************************
        * ask the user for pin, or get mockup value instead
         *******************************************************/

#if PIN_DEBUG
        if (mode == PIN_MODE_PETPIN) {
            printf("smart is asking for Pet pin, asking user...\n");
        } else if(mode == PIN_MODE_USERPIN)  {
            printf("smart is asking for User pin, asking user...\n");
        } 
#endif

#ifdef CONFIG_APP_PIN_INPUT_USART
        if (mode == PIN_MODE_PETPIN) {
            console_log("Enter pet pin code please\n");
        } else if(mode == PIN_MODE_USERPIN)  {
            console_log("Enter user pin code please\n");
        } 
        console_flush();
        shell_readline(&pin, &pin_len); /*FIXME: update API, set string... and size */
        console_log("pin registered!\n");
        console_flush();
#elif CONFIG_APP_PIN_INPUT_SCREEN
        pin_len=CONFIG_APP_PIN_MAX_PIN_LEN;

        if (mode == PIN_MODE_PETPIN) {
            get_pin(" Pet Pin Code ", 14, 0,240,60,320,pin,pin_len);
        } else if(mode == PIN_MODE_USERPIN)  {
            get_pin(" User Pin Code", 14, 0,240,60,320,pin,pin_len);
        }
#elif CONFIG_APP_PIN_INPUT_MOCKUP
        pin_len = CONFIG_APP_PIN_MAX_PIN_LEN;
        memcpy(pin, CONFIG_APP_PIN_MOCKUP_PIN_VALUE, 4);
#else
# error "input type must be set"
#endif


        /********************************************************
         * send back the pin & pin len values to smart
         *******************************************************/
        
        if (mode == PIN_MODE_PETPIN) {
            ipc_sync_cmd_data.magic = MAGIC_CRYPTO_PETPIN_RESP;
        } else if(mode == PIN_MODE_USERPIN)  {
            ipc_sync_cmd_data.magic = MAGIC_CRYPTO_PIN_RESP;
        } else {
            /* defaulting... */
            ipc_sync_cmd_data.magic = MAGIC_CRYPTO_PIN_RESP;
        }
        ipc_sync_cmd_data.state = SYNC_DONE;
        ipc_sync_cmd_data.data_size = (uint8_t)pin_len;
        memset(&ipc_sync_cmd_data.data.u8, 0x0, 32);
        memcpy(&ipc_sync_cmd_data.data.u8, pin, pin_len);

        do {
            size = sizeof(struct sync_command_data);
            ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd_data);
        } while (ret != SYS_E_DONE);

#if PIN_DEBUG
        if (mode == PIN_MODE_PETPIN) {
            printf("Pet Pin sent to smart\n");
        } else if(mode == PIN_MODE_USERPIN)  {
            printf("User Pin sent to smart\n");
        }
#endif


        /********************************************************
         * waiting for smart acknowledgement for the PIN sent
         *******************************************************/

        id = id_smart;
        size = sizeof(struct sync_command);
        sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd);


        /********************************************************
         * Depending on what SMART said, indicate the current status
         *******************************************************/

        if ((ipc_sync_cmd.magic == MAGIC_CRYPTO_PETPIN_RESP && ipc_sync_cmd.state == SYNC_DONE) ||
           (ipc_sync_cmd.magic == MAGIC_CRYPTO_PIN_RESP && ipc_sync_cmd.state == SYNC_DONE))
        {
            printf("Pin has been acknowledge by SMART\n");
            valid_pin = true;
#ifdef CONFIG_APP_PIN_INPUT_USART
            console_log("valid PIN, continuing...\n");
            console_flush();
#elif CONFIG_APP_PIN_INPUT_SCREEN
            tft_fill_rectangle(0,240,0,320,249,249,249);
            tft_set_cursor_pos(20,160);
            tft_setfg(0,0,0);
            tft_setbg(249,249,249);
            tft_puts("Please wait...");

#elif CONFIG_APP_PIN_INPUT_MOCKUP
            /* nothing to do */
#else
# error "input mode must be set"
#endif
        }

        /* If the current PIN is not correct, execute another round */
    } while (!valid_pin);
    return 0;
err:
    return 1;

}

/******************************************************************
 * Handle Pet name check from smart, including
 * - Pet name request to smart
 * - Pet name validation by user (or autovalid in mockup mode)
 * - Pet name validation response (Ack/Nack) to smart
 *****************************************************************/
static int handle_petname(void)
{
    uint8_t id;
    uint8_t ret;
    logsize_t size = 0;

    struct sync_command      ipc_sync_cmd;
    struct sync_command_data ipc_sync_cmd_data;


    ipc_sync_cmd.magic = MAGIC_CRYPTO_PETPIN_CMD;
    ipc_sync_cmd.state = SYNC_ASK_FOR_DATA;

    do {
        size = sizeof(struct sync_command);
        ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
    } while (ret != SYS_E_DONE);

    /* waiting for smart response, to print the pet name on the screen */
    id = id_smart;
    size = sizeof(struct sync_command_data);
    sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd_data);


    if (ipc_sync_cmd_data.magic != MAGIC_CRYPTO_PETPIN_RESP ||
            ipc_sync_cmd_data.state != SYNC_DONE)
    {
        goto err;
    }

    char *pet_name = (char*)ipc_sync_cmd_data.data.u8;
    uint8_t pet_name_len = ipc_sync_cmd_data.data_size;

#ifdef CONFIG_APP_PIN_INPUT_USART
    console_log("Pet pin is \"%s\". Is it Okay (y/n)?\n");
    console_flush();
    shell_readline(&pin, &pin_len); /*FIXME: update API, set string... and size */
    if (pin_len == 1 && pin[0] = 'n') {
        console_log("Invalid pet name !!!\n");
        console_flush();
#elif CONFIG_APP_PIN_INPUT_SCREEN
    if (get_petname_validation(pet_name, pet_name_len)) {
        tft_fill_rectangle(0,240,0,320,249,249,249);
        tft_set_cursor_pos(20,160);
        tft_setfg(0,0,0);
        tft_setbg(249,249,249);
        tft_puts(" Invalid Pet ");
        tft_set_cursor_pos(20,190);
        tft_puts("   Name !    ");
#elif CONFIG_APP_PIN_INPUT_MOCKUP
    if (0) {
        /* mockup mode has no pet name check */
        pet_name = NULL;
        pet_name_len = 0;
#else
# error "input mode must be set"
#endif
        ipc_sync_cmd.magic = MAGIC_CRYPTO_PETPIN_RESP;
        ipc_sync_cmd.state = SYNC_FAILURE;
    } else {
        ipc_sync_cmd.magic = MAGIC_CRYPTO_PETPIN_RESP;
        ipc_sync_cmd.state = SYNC_ACKNOWLEDGE;
    }
    do {
        size = sizeof(struct sync_command);
        ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
    } while (ret != SYS_E_DONE);

    return 0;
err:
    return 1;
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
        while (1)
            ;
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
    char pin[PIN_MAX_LEN + 1] = { 0 };

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
#elif CONFIG_APP_PIN_INPUT_USART
    /*
     * in the specific case of usart, the readline() function allocate
     * the string. The pin variable is then not a table but a string pointer
     */
    char * pin = 0;

    shell_init();
    if (ret == 0) {
        printf("USART4 is now configured !\n");
    } else {
        printf("error during configuration of USART4\n");
    }
    console_log("[USART4] Pin initialized usart...\n");
    console_flush();
#elif CONFIG_APP_PIN_INPUT_MOCKUP
    char pin[PIN_MAX_LEN + 1] = { 0 };
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
    
    uint8_t pin_len;

    /*
     * First handle Pet Pin. This permit to decrypt
     * the authentication keys held in the firmware
     * using the Pet pin given by the user, in
     * interaction with the smartcard (this permit
     * authentication keys confidentiality in case
     * of firmware dumping from flash).
     */
    if (handle_pin(pin, pin_len, PIN_MODE_PETPIN)) {
        printf("Error while handling Pet Pin !\n");
        goto err;
    };

    /*
     * now that a vaild pet pin has been received, we request the pet name from
     * smart, to show it to the user. This pet name must be validated by the user
     * This protect against malicious board replacement,
     * to ensure that the pet pin has been validated
     * by the smartcard before continuing. The pet
     * name must be the one set previously by the
     * user.
     */
    if (handle_petname()) {
        printf("Error while handling Pet name !\n");
        goto err;
    };

    /*
     * Then handle the user Pin. This permit to decrypt
     * the symetric key used to (en|de)crypt the mass-storage
     * content. The Pin is used to authenticate the
     * user. It is not the same as the pet name as
     * the user pin is sent encrypted to the smartcard
     * as a mutual authenticated channel is mounted. The
     * pet pin has been sent, for the first communication
     * part, unencrypted to the smartcard an can then be
     * spied through the communication bus.
     */
    if (handle_pin(pin, pin_len, PIN_MODE_USERPIN)) {
        printf("Error while handling User Pin !\n");
        goto err;
    };


    /*************************************************************
     * End of the full authentication mechanism.
     *
     * Starting of PIN main loop, which permit to support various
     * menus (settings, state, wipe, etc.) 
     ************************************************************/

#ifdef CONFIG_APP_PIN_INPUT_USART

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
err:
    while (1) {
    };

    return 0;
}
