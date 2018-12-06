#ifndef MAIN_H_
#define MAIN_H_

#define PIN_DEBUG 0

typedef enum {
    MODE_FW = 0,
    MODE_DFU = 1
} t_boot_mode;

t_boot_mode get_mode(void);

uint8_t get_smart_id(void);

#endif
