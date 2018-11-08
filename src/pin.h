#ifndef MYPIN_H
#define MYPIN_H
#define HIGHLIGHT_COLOR WHITE
#define WHITE 255,255,255
#define BLACK 0,0,0
#define WOOKEY_BLUE 29,201,255
#define WOOKEY_RED 226,39,92 
#define WOOKEY_GREEN 9,250,57
#define WOOKEY_ORANGE 255,213,19
#define GRAY 226,226,226

void get_txt_pad(const char *title,
                 uint32_t    title_len __attribute__((unused)),
                 int x1,int x2, int y1, int y2, char *mypin, char nb_pin);

void get_pin(const char *title,
             uint32_t    title_len,
             int x1,int x2, int y1, int y2,
             char *mypin, char nb_pin);

void draw_pin(int x1,int x2, int y1, int y2);
void draw_txt_pad(int x1,int x2, int y1, int y2, uint8_t offset);
extern const int font_width;
extern const int font_height;
extern char pin[4];
extern unsigned char nbpin;

uint8_t get_petname_validation(char *petname, uint8_t pet_name_len);
#endif
