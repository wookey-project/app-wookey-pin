/**
 * @file main.c
 *
 * \brief Main of dummy
 *
 */

#include "autoconf.h"


#include "api/syscall.h"
#include "api/print.h"
#include "libusart.h"

#if CONFIG_APP_PIN_INPUT_USART

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

#define PIN_MAX_LEN      16
#define PETNAME_MAX_LEN  32

#ifndef CONFIG_APP_PIN_INPUT_SCREEN
/* when not in screen mode, the authentication mode is hosted locally */
enum authentication_mode {
  FULL_AUTHENTICATION_MODE,
  LITE_AUTHENTICATION_MODE
};
#endif

uint8_t id_smart;

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



static int handle_pin_request(uint8_t mode, uint8_t type)
{
    uint8_t pin_len;

#ifdef CONFIG_APP_PIN_INPUT_SCREEN
    char pin[PIN_MAX_LEN + 1] = { 0 };
    pin_len=CONFIG_APP_PIN_MAX_PIN_LEN;
#elif CONFIG_APP_PIN_INPUT_USART
    char * pin = 0;
    pin_len = CONFIG_APP_PIN_MAX_PIN_LEN;
#elif CONFIG_APP_PIN_INPUT_MOCKUP
    mode = mode;
    type = type;
    char pin[PIN_MAX_LEN + 1] = { 0 };
    pin_len = CONFIG_APP_PIN_MAX_PIN_LEN;
#endif

    bool valid_pin = false;
    uint8_t id;
    uint8_t ret;
    logsize_t size = 0;

    struct sync_command_data ipc_sync_cmd_data;

    id = id_smart;
    size = sizeof(struct sync_command);

    valid_pin = false;

    /********************************************************
     * ask the user for pin, or get mockup value instead
     *******************************************************/

#if PIN_DEBUG
    if (mode == SC_PET_PIN) {
        printf("smart is asking for Pet pin, asking user...\n");
    } else if(mode == SC_USER_PIN)  {
        printf("smart is asking for User pin, asking user...\n");
    } 
#endif

#ifdef CONFIG_APP_PIN_INPUT_USART
    if (mode == SC_PET_PIN) {
        if (type == SC_REQ_AUTHENTICATE) {
            console_log("Enter pet pin code please\n");
        } else if (type == SC_REQ_MODIFY) {
            console_log("Enter new pet pin code please\n");
        }
    } else if(mode == SC_USER_PIN)  {
        if (type == SC_REQ_AUTHENTICATE) {
            console_log("Enter user pin code please\n");
        } else if (type == SC_REQ_MODIFY) {
            console_log("Enter new user pin code please\n");
        }
    } 
    console_flush();
    shell_readline(&pin, (uint32_t*)&pin_len); /*FIXME: update API, set string... and size */
    console_log("pin registered!\n");
    console_flush();
#elif CONFIG_APP_PIN_INPUT_SCREEN

    if (mode == SC_PET_PIN) {
        if (type == SC_REQ_AUTHENTICATE) {
          pin_len = pin_request_digits(" Pet Pin Code ", 14, 0,240,60,320,pin,PIN_MAX_LEN);
        } else if (type == SC_REQ_MODIFY) {
          pin_len = pin_request_digits(" new Pet Pin  ", 14, 0,240,60,320,pin,PIN_MAX_LEN);
        }
    } else if(mode == SC_USER_PIN)  {
        if (type == SC_REQ_AUTHENTICATE) {
          pin_len = pin_request_digits(" User Pin Code", 14, 0,240,60,320,pin,PIN_MAX_LEN);
        } else if (type == SC_REQ_MODIFY) {
          pin_len = pin_request_digits(" New User Pin ", 14, 0,240,60,320,pin,PIN_MAX_LEN);
        }
    }
#elif CONFIG_APP_PIN_INPUT_MOCKUP
    if (mode == SC_PET_PIN) {
        if (type == SC_REQ_AUTHENTICATE) {
            memcpy(pin, CONFIG_APP_PIN_MOCKUP_PET_PIN_VALUE, CONFIG_APP_PIN_MOCKUP_PET_PIN_LEN);
            pin_len = CONFIG_APP_PIN_MOCKUP_PET_PIN_LEN;
        }
    } else if (mode == SC_USER_PIN) {
        if (type == SC_REQ_AUTHENTICATE) {
            memcpy(pin, CONFIG_APP_PIN_MOCKUP_USER_PIN_VALUE, CONFIG_APP_PIN_MOCKUP_USER_PIN_LEN);
            pin_len = CONFIG_APP_PIN_MOCKUP_USER_PIN_LEN;
        }
    }
#else
# error "input type must be set"
#endif


    /********************************************************
     * before sending the pin to smart and wait for ack,
     * print a 'wait message', as smart takes some time
     * to check and initiate the secure channel
     *******************************************************/
#ifdef CONFIG_APP_PIN_INPUT_USART
    console_log("Please wait...\n");
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


    /********************************************************
     * send back the pin & pin len values to smart
     *******************************************************/

    if (pin_len > 32) {
        printf("Pin len is too big ! (%d)", pin_len);
        goto err;
    }
    ipc_sync_cmd_data.magic = MAGIC_CRYPTO_PIN_RESP;
    ipc_sync_cmd_data.state = SYNC_DONE;
    ipc_sync_cmd_data.data_size = (uint8_t)pin_len;
    memset(&ipc_sync_cmd_data.data.u8, 0x0, 32);
    memcpy(&ipc_sync_cmd_data.data.u8, pin, pin_len);

    size = sizeof(struct sync_command_data);
    ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd_data);

#if PIN_DEBUG
    if (mode == SC_PET_PIN) {
        printf("Pet Pin sent to smart\n");
    } else if(mode == SC_USER_PIN)  {
        printf("User Pin sent to smart\n");
    }
#endif


    /********************************************************
     * waiting for smart acknowledgement for the PIN sent
     *******************************************************/

    id = id_smart;
    size = sizeof(struct sync_command_data);
    sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd_data);


    /********************************************************
     * Depending on what SMART said, indicate the current status
     *******************************************************/

#if CONFIG_APP_PIN_INPUT_SCREEN
    /* Updating remaining tries with what the token said */
    if (ipc_sync_cmd_data.magic == MAGIC_CRYPTO_PIN_RESP) {
        if (mode == SC_USER_PIN && type == SC_REQ_AUTHENTICATE)
        {
            menu_update_remaining_tries(ipc_sync_cmd_data.data.u32[0]);
        }
    }
#endif

    if (ipc_sync_cmd_data.magic == MAGIC_CRYPTO_PIN_RESP && ipc_sync_cmd_data.state == SYNC_ACKNOWLEDGE)
    {
        printf("Pin has been acknowledged by SMART\n");
        valid_pin = true;
#ifdef CONFIG_APP_PIN_INPUT_USART
        console_log("valid PIN, continuing...\n");
        console_flush();
#elif CONFIG_APP_PIN_INPUT_SCREEN
        tft_fill_rectangle(0,240,0,320,249,249,249);
        tft_set_cursor_pos(20,160);
        tft_setfg(0,0,0);
        tft_setbg(249,249,249);
        tft_puts("Pin ok !...");

#elif CONFIG_APP_PIN_INPUT_MOCKUP
        /* nothing to do */
#else
# error "input mode must be set"
#endif
    } else {
        printf("Smart said that PIN is invalid\n");
        goto err;
    }

    return 0;
err:
    return 1;

}

/*
 * Request pet name from user (pet name update)
 */
int handle_petname_request(void)
{
    /*******************************************
     * get pet name from user
     ******************************************/
    struct sync_command_data ipc_sync_cmd_data = { 0 };
    //logsize_t size = 0;
    //uint8_t id = 0;
    uint8_t ret = 0;

#ifdef CONFIG_APP_PIN_INPUT_USART
    char *petname = 0;
    uint8_t petname_len = 0;
    char *ack = 0;
    uint8_t ack_len = 0;

    bool validated=false;
    do {
        console_log("Please enter new pet name:\n");
        console_flush();
        shell_readline(&petname, (uint32_t*)&petname_len);
        console_log("Confirm pet name ? (y/n)\n");
        console_flush();
        shell_readline(&ack, (uint32_t*)&ack_len);
        if (ack_len == 1 && ack[0] == 'y') {
            validated=true;
        }
    } while (!validated);
#elif CONFIG_APP_PIN_INPUT_SCREEN

    char petname[24 + 1] = { 0 };
    uint8_t max_petname_len = 24;

    pin_request_string("New pet name", 12, 0,240, 0, 320, petname, max_petname_len);

    tft_fill_rectangle(0,240,0,320,249,249,249);
    tft_set_cursor_pos(20,160);
    tft_setfg(0,0,0);
    tft_setbg(249,249,249);
    tft_puts(" Pet name ");
    tft_set_cursor_pos(20,190);
    tft_puts("  registered ");
#elif CONFIG_APP_PIN_INPUT_MOCKUP
    char petname[24 + 1] = { 0 };
    memcpy(petname, "Dark Vador", 10);
    /* mockup mode has no pet name check */
    //const char *petname = "johny";
    //uint8_t pet_name_len = 5;
#else
# error "input mode must be set"
#endif

    printf("new petname is %s\n", petname);
    /*******************************************
     * send pet name to SMART
     ******************************************/
    ipc_sync_cmd_data.magic = MAGIC_CRYPTO_PIN_RESP;
    ipc_sync_cmd_data.state = SYNC_DONE;
    // TODO: set data_size please
    ipc_sync_cmd_data.data_size = strlen(petname);
    memcpy(ipc_sync_cmd_data.data.u8, petname, strlen(petname));

    do {
      ret = sys_ipc(IPC_SEND_SYNC, id_smart, sizeof(struct sync_command_data), (char*)&ipc_sync_cmd_data);
    } while (ret == SYS_E_BUSY);

    /*******************************************
     * wait for SMART pet name update acknowledge
     ******************************************/
    // TODO: SMART does not acknowledge pet name update on the token
    // This part is deactivated by now.
#if 0
    id = id_smart;
    size = sizeof(struct sync_command_data);

    sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd_data);


    /********************************************************
     * Depending on what SMART said, indicate the current status
     *******************************************************/

    if (ipc_sync_cmd_data.magic == MAGIC_CRYPTO_PIN_RESP && ipc_sync_cmd_data.state == SYNC_ACKNOWLEDGE)
    {
#endif
        printf("Pet name update has been acknowledged by SMART\n");
#ifdef CONFIG_APP_PIN_INPUT_USART
        console_log("pet name update done\n");
        console_flush();
#elif CONFIG_APP_PIN_INPUT_SCREEN
        tft_fill_rectangle(0,240,0,320,249,249,249);
        tft_set_cursor_pos(20,160);
        tft_setfg(0,0,0);
        tft_setbg(249,249,249);
        tft_puts("Pet name");
        tft_set_cursor_pos(20,190);
        tft_puts("updated !");

#elif CONFIG_APP_PIN_INPUT_MOCKUP
        /* nothing to do */
#else
# error "input mode must be set"
#endif

#if 0
    } else {
        printf("Smart said that Pet name update failed\n");
        goto err;
    }
#endif

    return 0;
#if 0
err:
    return 1;
#endif
}


/******************************************************************
 * Handle Pet name check from smart, including
 * - Pet name request to smart
 * - Pet name validation by user (or autovalid in mockup mode)
 * - Pet name validation response (Ack/Nack) to smart
 *****************************************************************/
static int handle_petname_confirmation(const char *petname)
{
    uint8_t ret;
    logsize_t size = 0;

    struct sync_command      ipc_sync_cmd;

#ifdef CONFIG_APP_PIN_INPUT_USART
    char *ack = 0;
    uint8_t ack_len = 0;

    console_log("Pet name is \"%s\". Is it Okay (y/n)?\n", petname);
    console_flush();
    shell_readline(&ack, (uint32_t*)&ack_len); /*FIXME: update API, set string... and size */
    if (ack_len == 1 && ack[0] == 'n') {
        console_log("Invalid pet name !!!\n");
        console_flush();
#elif CONFIG_APP_PIN_INPUT_SCREEN
    uint8_t pet_name_len = strlen(petname);

    if (pin_request_string_validation("pet name", petname, pet_name_len)) {
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
        petname = petname;
#else
# error "input mode must be set"
#endif
        ipc_sync_cmd.magic = MAGIC_CRYPTO_PIN_RESP;
        ipc_sync_cmd.state = SYNC_FAILURE;
        size = sizeof(struct sync_command);
        ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
        goto err;

    } else {
        ipc_sync_cmd.magic = MAGIC_CRYPTO_PIN_RESP;
        ipc_sync_cmd.state = SYNC_ACKNOWLEDGE;
        size = sizeof(struct sync_command);
        ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
    }

    return 0;
err:
    return 1;
}

/******************************************************************
 * Handle a PIN request from smart, including :
 * - Pin request to user (usart, graphical, mockup)
 * - Pin response to smart
 * - Pin acknowledge from smart
 *****************************************************************/

uint8_t handle_full_pin_cmd_request(void)
{
    struct sync_command_data  ipc_sync_cmd = { 0 };
    uint8_t id = id_smart;
    logsize_t size = sizeof(struct sync_command_data);

    /*
     * get the PIN CMD request from SMART. Should be one of:
     * - Pet Pin modify
     * - User Pin modify
     * - Pet name modify
     */
    sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd);

    /* handling pet pin update request */
    if (   ipc_sync_cmd.magic            == MAGIC_CRYPTO_PIN_CMD
        && ipc_sync_cmd.data.req.sc_type == SC_PET_PIN
        && ipc_sync_cmd.data.req.sc_req  == SC_REQ_MODIFY) {
        if (handle_pin_request(SC_PET_PIN, SC_REQ_MODIFY)) {
            printf("Error while handling Pet Pin !\n");
        } else {
            printf("Pet pin update done\n");
        }
    /* handling pet name update request */
    } else if (   ipc_sync_cmd.magic            == MAGIC_CRYPTO_PIN_CMD
               && ipc_sync_cmd.data.req.sc_type == SC_PET_NAME
               && ipc_sync_cmd.data.req.sc_req  == SC_REQ_MODIFY) {

        if (handle_petname_request()) {
            printf("Error while handling Pet name !\n");
        } else {
            printf("Pet name update done\n");
        }
    /* handling user pin update request */
    } else if (   ipc_sync_cmd.magic            == MAGIC_CRYPTO_PIN_CMD
               && ipc_sync_cmd.data.req.sc_type == SC_USER_PIN
               && ipc_sync_cmd.data.req.sc_req  == SC_REQ_MODIFY) {
        if (handle_pin_request(SC_USER_PIN, SC_REQ_MODIFY)) {
            printf("Error while handling User Pin !\n");
        } else {
            printf("User pin update done\n");
        }
    } else {
        printf("Invalid command received ! Magic:%d\n", ipc_sync_cmd.magic);
        return 1;
    }
    return 0;
}


uint8_t handle_authentication(enum authentication_mode authmode)
{
    uint8_t step = 0;
    bool authenticated = false;
    uint8_t id;
    logsize_t size = 0;
    struct sync_command_data  ipc_sync_cmd = { 0 };

    id = id_smart;
    size = sizeof(struct sync_command_data);

    while (!authenticated) {
        sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd);
#if 1
        printf("req.sc_type: %d\n", ipc_sync_cmd.data.req.sc_type);
        printf("req.sc_req: %d\n", ipc_sync_cmd.data.req.sc_req);
#endif

        if (ipc_sync_cmd.magic != MAGIC_CRYPTO_PIN_CMD) {
            printf("smart request is not a PIN CMD\n");
            goto err;
        }
        /* full mode, pet pin, pet name check and user pin are requested */
        if (authmode == FULL_AUTHENTICATION_MODE) {
            if (ipc_sync_cmd.data.req.sc_type == SC_PET_PIN && ipc_sync_cmd.data.req.sc_req == SC_REQ_AUTHENTICATE) {
                printf("smart requesting pet pin\n");
                if (handle_pin_request(SC_PET_PIN, SC_REQ_AUTHENTICATE)) {
                    printf("Error while handling Pet Pin !\n");
                } else {
                    step++;
                }
            }
            if (ipc_sync_cmd.data.req.sc_type == SC_PET_NAME && ipc_sync_cmd.data.req.sc_req == SC_REQ_AUTHENTICATE) {
                printf("smart requesting pet name confirmation\n");
                if (handle_petname_confirmation(ipc_sync_cmd.data.req.sc_petname)) {
                    printf("Error while handling Pet name !\n");
                    goto err;
                } else {
                    step++;
                }
            }
            if (ipc_sync_cmd.data.req.sc_type == SC_USER_PIN && ipc_sync_cmd.data.req.sc_req == SC_REQ_AUTHENTICATE) {
                printf("smart requesting user pin\n");
                if (handle_pin_request(SC_USER_PIN, SC_REQ_AUTHENTICATE)) {
                    printf("Error while handling User Pin !\n");
                    goto err;
                } else {
                    step++;
                }
            }

            if (step == 3) {
                authenticated = true;
                break;
            }
        }
        /* Lite mode, only user Pin is requested */
        else if (authmode == LITE_AUTHENTICATION_MODE) {
            if (ipc_sync_cmd.data.req.sc_type == SC_USER_PIN && ipc_sync_cmd.data.req.sc_req == SC_REQ_AUTHENTICATE) {
                printf("smart requesting user pin\n");
                if (handle_pin_request(SC_USER_PIN, SC_REQ_AUTHENTICATE)) {
                    printf("Error while handling User Pin !\n");
                    goto err;
                };
                step++;
            }
            if (step == 1) {
                authenticated = true;
                break;
            }
        } else {
            goto err;
        }
    };
    return 0;
err:
    return 1;
}

#if CONFIG_APP_PIN_INPUT_SCREEN
uint8_t handle_settings_request(t_box signal)
{
    struct sync_command_data ipc_sync_cmd = { 0 };
    logsize_t size = 0;
    uint8_t ret = 0;
    ipc_sync_cmd.magic = MAGIC_SETTINGS_CMD;
    ipc_sync_cmd.state = SYNC_WAIT;
    size = sizeof(struct sync_command_data);

    switch (signal) {
        case BOX_SET_PETPIN:
        {
            ipc_sync_cmd.data.req.sc_type = SC_PET_PIN;
            ipc_sync_cmd.data.req.sc_req = SC_REQ_MODIFY;
            break;
        }
        case BOX_SET_PETNAME:
        {
            ipc_sync_cmd.data.req.sc_type = SC_PET_NAME;
            ipc_sync_cmd.data.req.sc_req = SC_REQ_MODIFY;
            break;
        }
        case BOX_SET_USERPIN:
        {
            ipc_sync_cmd.data.req.sc_type = SC_USER_PIN;
            ipc_sync_cmd.data.req.sc_req = SC_REQ_MODIFY;
            break;
        }
        case BOX_LOCK:
        {
            ipc_sync_cmd.magic = MAGIC_SETTINGS_LOCK;
            size = sizeof(struct sync_command);
            break;
        }
        default:
        {
            printf("unsupported signal %d!\n", signal);
            goto err;
        }
    }

    do {
        ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
    } while (ret != SYS_E_DONE);
    return 0;
err:
    return 1;
}
#endif


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
