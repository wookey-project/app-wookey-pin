#ifndef MAIN_H_
#define MAIN_H_

enum authentication_mode {
  FULL_AUTHENTICATION_MODE,
  LITE_AUTHENTICATION_MODE
};

uint8_t handle_authentication(enum authentication_mode authmode);

int handle_pin_request(uint8_t pin_type, uint8_t pin_mode);

int handle_petname_request(void);

int handle_full_pin_cmd_request(void);

#endif
