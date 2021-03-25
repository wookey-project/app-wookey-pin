/*
 *
 * Copyright 2019 The wookey project team <wookey@ssi.gouv.fr>
 *   - Ryad     Benadjila
 *   - Arnauld  Michelizza
 *   - Mathieu  Renard
 *   - Philippe Thierry
 *   - Philippe Trebuchet
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * the Free Software Foundation; either version 3 of the License, or (at
 * ur option) any later version.
 *
 * This package is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this package; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#ifndef HANDLERS_GENERIC_H_
#define HANDLERS_GENERIC_H_

#include "autoconf.h"
#include "libc/syscall.h"
#include "libc/stdio.h"
#include "libc/nostd.h"
#include "libc/types.h"
#include "wookey_ipc.h"
#include "main.h"

#if MODE_DFU

#if CONFIG_APP_PIN_DFU_INPUT_USART

# include "libusart.h"
# include "libconsole.h"

#elif CONFIG_APP_PIN_DFU_INPUT_SCREEN || CONFIG_APP_PIN_DFU_MOCKUP_SHOW_MENU

# include "gui_pin.h"
# include "libspi.h"
# include "libtouch.h"
# include "libtft.h"
# include "gui.h"

#elif CONFIG_APP_PIN_DFU_INPUT_MOCKUP
  /* nothing to include */
#else
# error "please specify input mode"
#endif

#else

#if CONFIG_APP_PIN_FW_INPUT_USART

# include "libusart.h"
# include "libconsole.h"

#elif CONFIG_APP_PIN_FW_INPUT_SCREEN || CONFIG_APP_PIN_FW_MOCKUP_SHOW_MENU

# include "gui_pin.h"
# include "libspi.h"
# include "libtouch.h"
# include "libtft.h"
# include "gui.h"

#elif CONFIG_APP_PIN_FW_INPUT_MOCKUP
  /* nothing to include */
#else
# error "please specify input mode"
#endif


#endif

enum authentication_mode {
  FULL_AUTHENTICATION_MODE,
  LITE_AUTHENTICATION_MODE
};


int handle_pin_request(uint8_t mode, uint8_t type);

int handle_petname_request(void);

int handle_petname_confirmation(const char *petname);

uint8_t handle_full_pin_cmd_request(void);

int handle_dfu_confirmation(uint32_t *dfuhdr);

void handle_dfu_status(void);

uint8_t handle_authentication(enum authentication_mode authmode);

void handle_external_events(bool *need_gui_refresh);

#if (CONFIG_APP_PIN_FW_INPUT_SCREEN && !defined(MODE_DFU)) || (CONFIG_APP_PIN_FW_MOCKUP_SHOW_MENU && !defined(MODE_DFU))
uint8_t handle_settings_request(t_box signal);
#elif (CONFIG_APP_PIN_DFU_INPUT_SCREEN && defined(MODE_DFU)) || (CONFIG_APP_PIN_DFU_MOCKUP_SHOW_MENU && defined(MODE_DFU))
uint8_t handle_settings_request(t_box signal);
#endif

#endif/*!HANDLERS_GENERIC_H_*/
