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
