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

#ifndef MAIN_H_
#define MAIN_H_

#include "autoconf.h"
#include "libc/types.h"

#define PIN_DEBUG 0

#define APP_PIN_INPUT_SCREEN \
  ((CONFIG_APP_PIN_DFU_INPUT_SCREEN && MODE_DFU) || (CONFIG_APP_PIN_FW_INPUT_SCREEN && !MODE_DFU))


#define APP_PIN_INPUT_USART \
  ((CONFIG_APP_PIN_DFU_INPUT_USART && MODE_DFU) || (CONFIG_APP_PIN_FW_INPUT_USART && !MODE_DFU))

#define APP_PIN_INPUT_MOCKUP \
  ((CONFIG_APP_PIN_DFU_INPUT_MOCKUP && MODE_DFU) || (CONFIG_APP_PIN_FW_INPUT_MOCKUP && !MODE_DFU))


uint8_t get_smart_id(void);

uint8_t get_crypto_id(void);

#endif
