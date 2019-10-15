#include "handlers_generic.h"
#include "libgui.h"
#include "libfw.h"
#include "libc/string.h"
#include "img/wait.h"
#include "lock2.h"

# include "img/zz.h"

#define PIN_MAX_LEN      16
#define PETNAME_MAX_LEN  32

/* time (in second) before activating screen saver */
#define IDLE_LOCK 30

extern menu_desc_t main_menu;
extern menu_desc_t dfu_menu;
extern menu_desc_t info_menu;

uint64_t storage_size  = 0;

#if CONFIG_APP_PIN_INPUT_SCREEN
static char se_info[32] = { 0 };
static char storage_info[256];
static char version_info[32];

#endif

void handle_dfu_status(void)
{
#if CONFIG_APP_PIN_INPUT_SCREEN || CONFIG_APP_PIN_MOCKUP_SHOW_MENU
    extern menu_desc_t status_menu;
    extern tile_desc_t status_main_tile;
#endif
    uint8_t ret;
    struct sync_command_data ipc_sync_cmd_data = { 0 };
    uint8_t id = get_smart_id();
    logsize_t size = sizeof(struct sync_command_data);

    ipc_sync_cmd_data.magic = MAGIC_DFU_GET_FW_VERSION;
    ipc_sync_cmd_data.state = SYNC_ASK_FOR_DATA;

    ret = sys_ipc(IPC_SEND_SYNC, id, sizeof(struct sync_command), (char*)&ipc_sync_cmd_data);
    if(ret != SYS_E_DONE){
        return;
    }
    ret = sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd_data);
    if(ret != SYS_E_DONE){
        return;
    }
#if CONFIG_APP_PIN_INPUT_SCREEN || CONFIG_APP_PIN_MOCKUP_SHOW_MENU
    if (ipc_sync_cmd_data.magic == MAGIC_DFU_GET_FW_VERSION) {
        uint8_t dev = ipc_sync_cmd_data.data.u32[0] & 0xff;
        uint8_t patch = (ipc_sync_cmd_data.data.u32[0] >> 8) & 0xff;
        uint8_t middle = (ipc_sync_cmd_data.data.u32[0] >> 16) & 0xff;
        uint8_t major = (ipc_sync_cmd_data.data.u32[0] >> 24) & 0xff;
        memset(storage_info, 0x0, sizeof(version_info));
        snprintf(version_info, 32, "version:\n%d.%d.%d-%d",
                major, middle, patch, dev);
        tile_text_t txt = {
            .text = version_info,
            .align = TXT_ALIGN_LEFT
        };
        gui_set_tile_text(&txt, status_main_tile);
        gui_set_menu(status_menu);
    }
#endif
}


void handle_external_events(bool *need_gui_refresh)
{
    struct sync_command_data sync_command_ack = { 0 };
    logsize_t size = sizeof(struct sync_command_data);
    uint8_t id = ANY_APP;

    if ((sys_ipc(IPC_RECV_ASYNC, &id, &size, (char*)&sync_command_ack)) == SYS_E_DONE) {

        switch (sync_command_ack.magic) {
            case MAGIC_DFU_HEADER_SEND:
                {
                    if (id != get_smart_id()) {
                        goto end_ext_events;
                    }
                    /* requesting user validation for header, from now on,
                     * the printed menu will be the DFU menu (no access
                     * to settings or status)
                     */
#if PIN_DEBUG
                    printf("receiving ack request for magic %08x, version %d\n", sync_command_ack.data.u32[0], sync_command_ack.data.u32[1]);
#endif
                    handle_dfu_confirmation((uint32_t*)sync_command_ack.data.u32);
                    if (need_gui_refresh) {
                        *need_gui_refresh = true;
                    }
                    break;
                }

            case MAGIC_DFU_DWNLOAD_FINISHED:
                {
                    if (id != get_smart_id()) {
                        goto end_ext_events;
                    }
#if PIN_DEBUG
                    printf("DFU download finished. Going back to main\n");
#endif
#ifdef CONFIG_APP_PIN_INPUT_SCREEN
                    gui_unlock_touch();
                    gui_set_menu(main_menu);
#endif
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
                    if (id != get_smart_id()) {
                        goto end_ext_events;
                    }

#ifdef CONFIG_APP_PIN_INPUT_SCREEN
                    gui_lock_touch();
                    gui_set_menu(dfu_menu);
#endif
#if PIN_DEBUG
                    printf("DFU download started, going to DFU menu\n");
#endif
                    /* The DFU download is now finished (successfully or not)
                     * the user is informed of the result and can go back
                     * to the global menu with a return button
                     */

                    if (need_gui_refresh) {
                        *need_gui_refresh = true;
                    }
                    break;
                }
            case MAGIC_STORAGE_SCSI_BLOCK_NUM_RESP:
                {
                    if (id != get_smart_id()) {
                        goto end_ext_events;
                    }
                    uint32_t gsize = 0;
                    uint32_t gsize_pow = 0;
                    uint32_t block_size = sync_command_ack.data.u32[0];
                    uint32_t block_num = sync_command_ack.data.u32[1];
                    uint64_t storage_size = (uint64_t)block_size;
                    uint64_t storage_size_pow;
                    storage_size *= block_num;
                    storage_size_pow = storage_size;
                    storage_size /= (1000 * 1000 * 1000);
                    storage_size_pow /= (1024 * 1024 * 1024);
                    gsize = (uint32_t)storage_size;
                    gsize_pow = (uint32_t)storage_size_pow;
#ifdef CONFIG_APP_PIN_INPUT_SCREEN
                    extern tile_desc_t storage_main_tile;
                    memset(storage_info, 0x0, sizeof(storage_info));
                    snprintf(storage_info, 255, "storage:\n%d GB,%d GiB\nstorage block\nsize:\n%d bytes", gsize, gsize_pow, block_size);
                    tile_text_t storage_text = {
                        .text = storage_info,
                        .align = TXT_ALIGN_LEFT
                    };
                    gui_set_tile_text(&storage_text, storage_main_tile);
#endif
                    break;
                }
            case MAGIC_INFORMATIONAL_MSG:
                {
                    if (id != get_smart_id()) {
                        goto end_ext_events;
                    }
                   /* receiving msg from smart (info, error...) to be printed
                    * The message is returned to the user and acknowledge
                    * to smart as printed */
#ifdef CONFIG_APP_PIN_INPUT_SCREEN
                    extern tile_desc_t info_main_tile;
                    char *ext_info = NULL;
                    /* sanitation, terminating the string */
                    sync_command_ack.data.c[63] = '\0';
                    ext_info = &(sync_command_ack.data.c[0]);
                    tile_text_t info_text = {
                        .text = ext_info,
                        .align = TXT_ALIGN_LEFT
                    };
                    gui_set_tile_text(&info_text, info_main_tile);
                    gui_set_menu(info_menu);
                    *need_gui_refresh = true;
#endif
                    break;
                }
            case MAGIC_AUTH_STATE_PASSED:
                {
                    if (id != get_crypto_id()) {
                        printf("err: invalid source for magic!\n");
                        goto end_ext_events;
                    }
                    /* here we are handling external event of the gui main loop, this means
                     * that the authentication sequence have been passed. We can acknowledge */
                    size = sizeof(struct sync_command);
                    sync_command_ack.magic = MAGIC_AUTH_STATE_PASSED;
                    sync_command_ack.state = SYNC_ACKNOWLEDGE;

                    if ((sys_ipc(IPC_SEND_SYNC, id, size, (char*)&sync_command_ack)) != SYS_E_DONE) {
                        printf("err: unable to acknowledge third party authentication check\n");
                        goto end_ext_events;
                    }
                    break;
                }
            default:
                {
                    /* no action for others */
                    break;
                }
        }

    }
#ifdef CONFIG_APP_PIN_INPUT_SCREEN
    else {
        if (is_in_fw_mode()) {
            /* no IPC received. Checking for idle time
             * This behavior is for FW mode only, as DFU mode is considered
             * as transient and DFU menu lock the screen */
            extern menu_desc_t idle_menu;
            extern tile_desc_t idle_1st_tile;
            extern tile_desc_t idle_2nd_tile;
            extern tile_desc_t idle_3rd_tile;
            extern tile_desc_t idle_4th_tile;
            extern tile_desc_t idle_5th_tile;
            extern tile_desc_t idle_6th_tile;

            uint64_t idle_time = gui_get_idle_time();
            /* not yet in idle ? */
            if (gui_get_current_menu() != idle_menu) {
                if (idle_time > IDLE_LOCK) {
                    gui_set_menu(idle_menu);
                }
            } else {
                /* handling idle time */
                uint64_t idle_time = gui_get_idle_time();
                /* everty IDLE_LOCK time, update the Zz icon position */
                if ((idle_time % IDLE_LOCK) == 0) {
                    tile_icon_t zz_icon = {
                        .data = zz,
                        .size = sizeof(zz)
                    };
                    uint8_t zztile=(((idle_time / IDLE_LOCK) % 6) + 1);
                    gui_set_tile_icon(0, idle_1st_tile);
                    gui_set_tile_icon(0, idle_2nd_tile);
                    gui_set_tile_icon(0, idle_3rd_tile);
                    gui_set_tile_icon(0, idle_4th_tile);
                    gui_set_tile_icon(0, idle_4th_tile);
                    gui_set_tile_icon(0, idle_5th_tile);
                    gui_set_tile_icon(0, idle_6th_tile);
                    switch (zztile) {
                        case 1:
                            gui_set_tile_icon(&zz_icon, idle_1st_tile);
                            break;
                        case 2:
                            gui_set_tile_icon(&zz_icon, idle_2nd_tile);
                            break;
                        case 3:
                            gui_set_tile_icon(&zz_icon, idle_3rd_tile);
                            break;
                        case 4:
                            gui_set_tile_icon(&zz_icon, idle_4th_tile);
                            break;
                        case 5:
                            gui_set_tile_icon(&zz_icon, idle_5th_tile);
                            break;
                        case 6:
                            gui_set_tile_icon(&zz_icon, idle_6th_tile);
                            break;
                        default:
                            break;
                    }
                    gui_force_refresh();
                }

            }
        }
    }
#endif

end_ext_events:
    return;

}

#if CONFIG_APP_PIN_INPUT_MOCKUP
int handle_pin_request(__attribute__((unused)) uint8_t mode, __attribute__((unused)) uint8_t type)
#else
int handle_pin_request(uint8_t mode, uint8_t type)
#endif
{
#if CONFIG_APP_PIN_INPUT_SCREEN
    extern menu_desc_t error_menu;
#endif
    uint8_t pin_len;
    uint8_t id_smart = get_smart_id();
    bool change_ok = false;

#ifdef CONFIG_APP_PIN_INPUT_SCREEN
    char pin[PIN_MAX_LEN + 1] = { 0 };
    char pin2[PIN_MAX_LEN + 1] = { 0 };
    pin_len=CONFIG_APP_PIN_MAX_PIN_LEN;
#elif CONFIG_APP_PIN_INPUT_USART
    char pin[PIN_MAX_LEN + 1] = { 0 };
    pin_len = CONFIG_APP_PIN_MAX_PIN_LEN;
#elif CONFIG_APP_PIN_INPUT_MOCKUP
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
            change_ok = true;
        }
    } else if(mode == SC_USER_PIN)  {
        if (type == SC_REQ_AUTHENTICATE) {
            console_log("Enter user pin code please\n");
        } else if (type == SC_REQ_MODIFY) {
            console_log("Enter new user pin code please\n");
            change_ok = true;
        }
    }
    console_show_prompt();
    console_readline(pin, (uint32_t*)&pin_len, PIN_MAX_LEN); /*FIXME: update API, set string... and size */
    console_log("pin registered!\n");
#elif CONFIG_APP_PIN_INPUT_SCREEN

    if (mode == SC_PET_PIN) {
        if (type == SC_REQ_AUTHENTICATE) {
          pin_len = pin_request_digits(" Pet Pin Code ", 14, 0,240,60,320,pin,PIN_MAX_LEN);
        } else if (type == SC_REQ_MODIFY) {
          pin_len = pin_request_digits(" new Pet Pin  ", 14, 0,240,60,320,pin,PIN_MAX_LEN);
          pin_len = pin_request_digits("new Pet Pin /2", 14, 0,240,60,320,pin2,PIN_MAX_LEN);
          if (strcmp(pin, pin2) == 0) {
              change_ok = true;
          }
        }
    } else if(mode == SC_USER_PIN)  {
        if (type == SC_REQ_AUTHENTICATE) {
          pin_len = pin_request_digits(" User Pin Code", 14, 0,240,60,320,pin,PIN_MAX_LEN);
        } else if (type == SC_REQ_MODIFY) {
          pin_len = pin_request_digits(" New User Pin ", 14, 0,240,60,320,pin,PIN_MAX_LEN);
          pin_len = pin_request_digits("New User Pin /2", 15, 0,240,60,320,pin2,PIN_MAX_LEN);
          if (strcmp(pin, pin2) == 0) {
              change_ok = true;
          }
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
#elif CONFIG_APP_PIN_INPUT_SCREEN
    tft_set_cursor_pos(20,160);
    if (is_in_fw_mode()) {
        tft_fill_rectangle(0,240,0,320,249,249,249);
        tft_rle_image(97, 137, 45, 45, wait_colormap, wait, sizeof(wait));
    }
    if (is_in_dfu_mode()) {
        tft_fill_rectangle(0,240,0,320,194,167,214);
        tft_rle_image(97, 137, 45, 45, wait_dfu_colormap, wait, sizeof(wait));
    }

#elif CONFIG_APP_PIN_INPUT_MOCKUP
    /* nothing to do */
#else
# error "input mode must be set"
#endif


    /********************************************************
     * send back the pin & pin len values to smart
     *******************************************************/

    ipc_sync_cmd_data.magic = MAGIC_CRYPTO_PIN_RESP;
    if (type == SC_REQ_MODIFY) {
        if (change_ok == false) {
            /* pin typing error */
            ipc_sync_cmd_data.state = SYNC_FAILURE;
        } else {
            ipc_sync_cmd_data.state = SYNC_DONE;
        }
    } else {
        ipc_sync_cmd_data.state = SYNC_DONE;
    }
    if (pin_len > CONFIG_APP_PIN_MAX_PIN_LEN) {
        ipc_sync_cmd_data.state = SYNC_FAILURE;
    }
    ipc_sync_cmd_data.data_size = (uint8_t)pin_len;
    memset(&ipc_sync_cmd_data.data.u8, 0x0, 32);
    memcpy(&ipc_sync_cmd_data.data.u8, pin, pin_len);

    size = sizeof(struct sync_command_data);
    ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd_data);
    if(ret != SYS_E_DONE){
        goto err;
    }

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
    ret = sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd_data);
    if(ret != SYS_E_DONE){
        goto err;
    }


    /********************************************************
     * Depending on what SMART said, indicate the current status
     *******************************************************/

#if CONFIG_APP_PIN_INPUT_SCREEN
    /* Updating remaining tries with what the token said */
    if (ipc_sync_cmd_data.magic == MAGIC_CRYPTO_PIN_RESP) {
        if ((mode == SC_USER_PIN) && (type == SC_REQ_AUTHENTICATE))
        {
            extern tile_desc_t status_se_tile;
            uint8_t remaining_tries = (uint8_t)ipc_sync_cmd_data.data.u32[0];
            snprintf(se_info, 31, "SE PIN tries:\n%d", remaining_tries);

            tile_text_t text = {
                .text = se_info,
                .align = TXT_ALIGN_LEFT
            };

            gui_set_tile_text(&text, status_se_tile);
        }
    }
#endif

    if ((ipc_sync_cmd_data.magic == MAGIC_CRYPTO_PIN_RESP) && (ipc_sync_cmd_data.state == SYNC_ACKNOWLEDGE))
    {
        printf("Pin has been acknowledged by SMART\n");
        valid_pin = true;
#ifdef CONFIG_APP_PIN_INPUT_USART
        console_log("valid PIN, continuing...\n");
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
#if CONFIG_APP_PIN_INPUT_SCREEN
        gui_set_menu(error_menu);
#endif
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
    uint8_t ret = 0;

#ifdef CONFIG_APP_PIN_INPUT_USART
    char petname[24 + 1] = { 0 };
    uint8_t petname_len = 0;
    char ack[2] = { 0 };
    uint8_t ack_len = 0;

    bool validated=false;
    do {
        console_log("Please enter new pet name:\n");
        console_show_prompt();
        console_readline(petname, (uint32_t*)&petname_len, 24);
        console_log("Confirm pet name ? (y/n)\n");
        console_show_prompt();
        console_readline(ack, (uint32_t*)&ack_len, 2);
        if ((ack_len == 1) && (ack[0] == 'y')) {
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
    } while (ret != SYS_E_DONE);

    printf("Pet name update has been acknowledged by SMART\n");
#ifdef CONFIG_APP_PIN_INPUT_USART
    console_log("pet name update done\n");
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

    return 0;
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
int handle_dfu_confirmation(__attribute__((unused))uint32_t *dfuhdr)
{
    uint8_t id_smart = get_smart_id();
    uint8_t ret;
    struct sync_command      ipc_sync_cmd;

    logsize_t size = 0;

#if CONFIG_APP_PIN_INPUT_SCREEN
    uint32_t magic = dfuhdr[0];
    uint32_t version = dfuhdr[1];

    memset(storage_info, 0x0, 256);
    snprintf(storage_info, 255, "header:        magic: %d\n   version:\n     %01d.%01d.%01d-%01d\n", magic,
            (version >> 24) & 0xff,
            (version >> 16) & 0xff,
            (version >>  8) & 0xff,
            version       & 0xff);
#endif

#ifdef CONFIG_APP_PIN_INPUT_USART
    char ack[2] = { 0 };
    uint32_t ack_len = 0;

    console_log("DFU header is:\n");
    console_log("- Magic: %x\n", magic);
    console_log("- version: %x\n", version);
    console_log("Is it Okay (y/n)?\n");
    console_show_prompt();
    console_readline(ack, &ack_len, 2); /*FIXME: update API, set string... and size */
    if ((ack_len == 1) && (ack[0] == 'n')) {
        console_log("Invalid DFU file !!!\n");
#elif CONFIG_APP_PIN_INPUT_SCREEN
    if (pin_request_string_validation("DFU header", storage_info, strlen(storage_info))) {
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
#else
# error "input mode must be set"
#endif
        printf("user said DFU hdr is invalid!\n");
#if CONFIG_APP_PIN_INPUT_SCREEN
        gui_force_refresh();
#endif
        ipc_sync_cmd.magic = MAGIC_DFU_HEADER_INVALID;
        ipc_sync_cmd.state = SYNC_FAILURE;
        size = sizeof(struct sync_command);
        ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
        goto err;

    } else {
        printf("user said DFU hdr is valid!\n");
#if CONFIG_APP_PIN_INPUT_SCREEN
        gui_force_refresh();
#endif
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
#if CONFIG_APP_PIN_INPUT_MOCKUP
int handle_petname_confirmation(__attribute__((unused)) const char *petname)
#else
int handle_petname_confirmation(const char *petname)
#endif
{
    uint8_t id_smart = get_smart_id();
    uint8_t ret;
    logsize_t size = 0;

    struct sync_command      ipc_sync_cmd;

#ifdef CONFIG_APP_PIN_INPUT_USART
    char ack[2] = { 0 };
    uint32_t ack_len = 0;

    console_log("Pet name is \"%s\". Is it Okay (y/n)?\n", petname);
    console_show_prompt();
    console_readline(ack, &ack_len, 2); /*FIXME: update API, set string... and size */
    if ((ack_len == 1) && (ack[0] == 'n')) {
        console_log("Invalid pet name !!!\n");
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
#else
# error "input mode must be set"
#endif
        ipc_sync_cmd.magic = MAGIC_CRYPTO_PIN_RESP;
        ipc_sync_cmd.state = SYNC_FAILURE;
        size = sizeof(struct sync_command);
        sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
        /* No need to check IPC ret since we go to error anyways ... */
        goto err;

    } else {
        ipc_sync_cmd.magic = MAGIC_CRYPTO_PIN_RESP;
        ipc_sync_cmd.state = SYNC_ACKNOWLEDGE;
        size = sizeof(struct sync_command);
        ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
        if(ret != SYS_E_DONE){
            goto err;
        }
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
    uint8_t ret;

    /*
     * get the PIN CMD request from SMART. Should be one of:
     * - Pet Pin modify
     * - User Pin modify
     * - Pet name modify
     */
    ret = sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd);
    if(ret != SYS_E_DONE){
        goto err;
    }

    /* handling pet pin update request */
    if (   (ipc_sync_cmd.magic            == MAGIC_CRYPTO_PIN_CMD)
        && (ipc_sync_cmd.data.req.sc_type == SC_PET_PIN)
        && (ipc_sync_cmd.data.req.sc_req  == SC_REQ_MODIFY)) {
        if (handle_pin_request(SC_PET_PIN, SC_REQ_MODIFY)) {
            printf("Error while handling Pet Pin !\n");
        } else {
            printf("Pet pin update done\n");
        }
    /* handling pet name update request */
    } else if (   (ipc_sync_cmd.magic            == MAGIC_CRYPTO_PIN_CMD)
               && (ipc_sync_cmd.data.req.sc_type == SC_PET_NAME)
               && (ipc_sync_cmd.data.req.sc_req  == SC_REQ_MODIFY)) {

        if (handle_petname_request()) {
            printf("Error while handling Pet name !\n");
        } else {
            printf("Pet name update done\n");
        }
    /* handling user pin update request */
    } else if (   (ipc_sync_cmd.magic            == MAGIC_CRYPTO_PIN_CMD)
               && (ipc_sync_cmd.data.req.sc_type == SC_USER_PIN)
               && (ipc_sync_cmd.data.req.sc_req  == SC_REQ_MODIFY)) {
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
err:
    return 1;
}


uint8_t handle_authentication(enum authentication_mode authmode)
{
    uint8_t id_smart = get_smart_id();
    uint8_t step = 0;
    bool authenticated = false;
    uint8_t id;
    logsize_t size = 0;
    struct sync_command_data  ipc_sync_cmd = { 0 };
    uint8_t ret;

    id = id_smart;
    size = sizeof(struct sync_command_data);

    while (!authenticated) {
        ret = sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd);
        if(ret != SYS_E_DONE){
            goto err;
        }
#if PIN_DEBUG
        printf("req.sc_type: %d\n", ipc_sync_cmd.data.req.sc_type);
        printf("req.sc_req: %d\n", ipc_sync_cmd.data.req.sc_req);
#endif

        if (ipc_sync_cmd.magic != MAGIC_CRYPTO_PIN_CMD) {
            printf("smart request is not a PIN CMD\n");
            goto err;
        }
        /* full mode, pet pin, pet name check and user pin are requested */
        if (authmode == FULL_AUTHENTICATION_MODE) {
            if ((ipc_sync_cmd.data.req.sc_type == SC_PET_PIN) && (ipc_sync_cmd.data.req.sc_req == SC_REQ_AUTHENTICATE)) {
                printf("smart requesting pet pin\n");
                if (handle_pin_request(SC_PET_PIN, SC_REQ_AUTHENTICATE)) {
                    printf("Error while handling Pet Pin !\n");
                } else {
                    step++;
                }
            }
            if ((ipc_sync_cmd.data.req.sc_type == SC_PET_NAME) && (ipc_sync_cmd.data.req.sc_req == SC_REQ_AUTHENTICATE)) {
                printf("smart requesting pet name confirmation\n");
                if (handle_petname_confirmation(ipc_sync_cmd.data.req.sc_petname)) {
                    printf("Error while handling Pet name !\n");
                    goto err;
                } else {
                    step++;
                }
            }
            if ((ipc_sync_cmd.data.req.sc_type == SC_USER_PIN) && (ipc_sync_cmd.data.req.sc_req == SC_REQ_AUTHENTICATE)) {
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
            if ((ipc_sync_cmd.data.req.sc_type == SC_USER_PIN) && (ipc_sync_cmd.data.req.sc_req == SC_REQ_AUTHENTICATE)) {
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
            /* first we update the current graphical content */
            tft_fill_rectangle(0,240,0,320,lock_colormap[0],lock_colormap[1],lock_colormap[2]);
            tft_rle_image(0,0,lock_width,lock_height,lock_colormap,lock,sizeof(lock));
            /* then we prepare the IPC for SMART */
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
        goto err;
    }
    return 0;
err:
    return 1;
}
#endif


