#ifndef PIN_GUI_H
#define PIN_GUI_H

#include "api/types.h"

typedef enum {
    BOX_SET_PETPIN,
    BOX_SET_PETNAME,
    BOX_SET_USERPIN,
    BOX_LOCK,
} t_box;

void init_dfu_gui(void);

void init_fw_gui(void);

#endif/*PIN_GUI_H*/
