#include "handlers_generic.h"
#include "libgui.h"

#define PIN_MAX_LEN      16
#define PETNAME_MAX_LEN  32

extern menu_desc_t main_menu;
extern menu_desc_t dfu_menu;

void handle_external_events(bool *need_gui_refresh)
{
    struct sync_command_data sync_command_ack = { 0 };
    logsize_t size = sizeof(struct sync_command_data);
    uint8_t id = get_smart_id();

    if ((sys_ipc(IPC_RECV_ASYNC, &id, &size, (char*)&sync_command_ack)) == SYS_E_DONE) {

        switch (sync_command_ack.magic) {
            case MAGIC_DFU_HEADER_SEND:
                {
                    /* requesting user validation for header, from now on,
                     * the printed menu will be the DFU menu (no access
                     * to settings or status)
                     */
                    handle_dfu_confirmation((char*)sync_command_ack.data.u8);
                    if (need_gui_refresh) {
                        *need_gui_refresh = true;
                    }
                    break;
                }

            case MAGIC_DFU_DWNLOAD_FINISHED:
                {
                    printf("DFU download finished. Going back to main\n");
                    gui_unlock_touch();
                    gui_set_menu(main_menu);

                    if (need_gui_refresh) {
                        *need_gui_refresh = true;
                    }
                    /* The DFU download is now finished (successfully or not)
                     * the user is informed of the result and can go back
                     * to the global menu with a return button
                     */
                    break;
                }
            case MAGIC_DFU_DWNLOAD_STARTED:
                {
                    gui_lock_touch();
                    gui_set_menu(dfu_menu);
                    printf("DFU download started, going to DFU menu\n");
                    /* The DFU download is now finished (successfully or not)
                     * the user is informed of the result and can go back
                     * to the global menu with a return button
                     */

                    if (need_gui_refresh) {
                        *need_gui_refresh = true;
                    }
                    break;
                }
        }
    }
    return;

}

int handle_pin_request(uint8_t mode, uint8_t type)
{
    uint8_t pin_len;
    uint8_t id_smart = get_smart_id();

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
            if (get_mode() == MODE_FW) {
                memcpy(pin, CONFIG_APP_PIN_MOCKUP_PET_PIN_VALUE, CONFIG_APP_PIN_MOCKUP_PET_PIN_LEN);
                pin_len = CONFIG_APP_PIN_MOCKUP_PET_PIN_LEN;
            } else { /* mode DFU */
                memcpy(pin, CONFIG_APP_PIN_MOCKUP_DFU_PET_PIN_VALUE, CONFIG_APP_PIN_MOCKUP_DFU_PET_PIN_LEN);
                pin_len = CONFIG_APP_PIN_MOCKUP_DFU_PET_PIN_LEN;
            }
        }
    } else if (mode == SC_USER_PIN) {
        if (type == SC_REQ_AUTHENTICATE) {
            if (get_mode() == MODE_FW) {
                memcpy(pin, CONFIG_APP_PIN_MOCKUP_USER_PIN_VALUE, CONFIG_APP_PIN_MOCKUP_USER_PIN_LEN);
                pin_len = CONFIG_APP_PIN_MOCKUP_USER_PIN_LEN;
            } else {
                memcpy(pin, CONFIG_APP_PIN_MOCKUP_DFU_USER_PIN_VALUE, CONFIG_APP_PIN_MOCKUP_DFU_USER_PIN_LEN);
                pin_len = CONFIG_APP_PIN_MOCKUP_DFU_USER_PIN_LEN;
            }
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
            //menu_update_remaining_tries(ipc_sync_cmd_data.data.u32[0]);
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
    uint8_t id_smart = get_smart_id();
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
struct __packed dfuhdr_t {
    uint32_t magic;
    uint32_t version;
};


/******************************************************************
 * Handle DFU firmware confirmation from smart, including
 * - Pet name validation by user (or autovalid in mockup mode)
 * - Pet name validation response (Ack/Nack) to smart
 *****************************************************************/
int handle_dfu_confirmation(char *dfuhdr)
{
    uint8_t id_smart = get_smart_id();
    uint8_t ret;
    logsize_t size = 0;

    struct sync_command      ipc_sync_cmd;
    uint8_t dfu_hdr_len = 8;
    for (uint8_t i = 0; i < dfu_hdr_len; ++i) {
        dfuhdr[i] += '0'; /* passing to printable value */
    }
    dfuhdr[dfu_hdr_len] = 0;

    printf("DFU: requesting user confirmation for %s\n", dfuhdr);
#ifdef CONFIG_APP_PIN_INPUT_USART
    char *ack = 0;
    uint8_t ack_len = 0;
    dfuhdr_t *hdr = dfuhdr;

    console_log("DFU header is:\n");
    console_log("- Magic: %x\n", hdr->magic);
    console_log("- version: %x\n", hdr->version);
    console_log("Is it Okay (y/n)?\n");
    console_flush();
    shell_readline(&ack, (uint32_t*)&ack_len); /*FIXME: update API, set string... and size */
    if (ack_len == 1 && ack[0] == 'n') {
        console_log("Invalid DFU file !!!\n");
        console_flush();
#elif CONFIG_APP_PIN_INPUT_SCREEN
    if (pin_request_string_validation("DFU header", dfuhdr, dfu_hdr_len)) {
        tft_fill_rectangle(0,240,0,320,249,249,249);
        tft_set_cursor_pos(20,160);
        tft_setfg(0,0,0);
        tft_setbg(249,249,249);
        tft_puts(" Invalid DFU ");
        tft_set_cursor_pos(20,190);
        tft_puts("   Header!   ");
#elif CONFIG_APP_PIN_INPUT_MOCKUP
    if (0) {
        /* mockup mode has no pet name check */
        dfuhdr = dfuhdr;
#else
# error "input mode must be set"
#endif
        printf("user said DFU hdr is invalid!\n");
        ipc_sync_cmd.magic = MAGIC_DFU_HEADER_INVALID;
        ipc_sync_cmd.state = SYNC_FAILURE;
        size = sizeof(struct sync_command);
        ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
        goto err;

    } else {
        printf("user said DFU hdr is valid!\n");
        ipc_sync_cmd.magic = MAGIC_DFU_HEADER_VALID;
        ipc_sync_cmd.state = SYNC_ACKNOWLEDGE;
        size = sizeof(struct sync_command);
        ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
    }

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
int handle_petname_confirmation(const char *petname)
{
    uint8_t id_smart = get_smart_id();
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
    uint8_t id_smart = get_smart_id();
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
    uint8_t id_smart = get_smart_id();
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

#if CONFIG_APP_PIN_INPUT_SCREEN || CONFIG_APP_PIN_MOCKUP_SHOW_MENU
uint8_t handle_settings_request(t_box signal)
{
    uint8_t id_smart = get_smart_id();
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

    ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
    if (ret != SYS_E_DONE) {
        printf("unable to send settings request to smart!\n");
    }
    return 0;
err:
    return 1;
}
#endif


