#ifndef HANDLERS_GENERIC_H_
#define HANDLERS_GENERIC_H_

#include "autoconf.h"
#include "api/syscall.h"
#include "api/print.h"
#include "api/types.h"
#include "wookey_ipc.h"
#include "main.h"

#if CONFIG_APP_PIN_INPUT_USART

# include "libusart.h"
# include "libconsole.h"
# include "libshell.h"

#elif CONFIG_APP_PIN_INPUT_SCREEN || CONFIG_APP_PIN_MOCKUP_SHOW_MENU

# include "gui_pin.h"
# include "libspi.h"
# include "libtouch.h"
# include "libtft.h"
# include "gui.h"

#elif CONFIG_APP_PIN_INPUT_MOCKUP
  /* nothing to include */
#else
# error "please specify input mode"
#endif


enum authentication_mode {
  FULL_AUTHENTICATION_MODE,
  LITE_AUTHENTICATION_MODE
};


int handle_pin_request(uint8_t mode, uint8_t type);

int handle_petname_request(void);

int handle_petname_confirmation(const char *petname);

uint8_t handle_full_pin_cmd_request(void);

int handle_dfu_confirmation(char *dfuhdr);

uint8_t handle_authentication(enum authentication_mode authmode);

void handle_external_events(bool *need_gui_refresh);

#if CONFIG_APP_PIN_INPUT_SCREEN || CONFIG_APP_PIN_MOCKUP_SHOW_MENU
uint8_t handle_settings_request(t_box signal);
#endif

#endif/*!HANDLERS_GENERIC_H_*/
