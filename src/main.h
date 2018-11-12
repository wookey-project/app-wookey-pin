#ifndef MAIN_H_
#define MAIN_H_

enum authentication_mode {
  FULL_AUTHENTICATION_MODE,
  LITE_AUTHENTICATION_MODE
};

typedef enum {
    PIN_MODE_PETPIN,
    PIN_MODE_USERPIN
} t_pin_mode;



uint8_t handle_authentication(enum authentication_mode authmode);

int handle_pin(t_pin_mode mode);

#endif
