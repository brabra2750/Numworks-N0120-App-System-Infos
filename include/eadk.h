#ifndef EADK_H
#define EADK_H
#include <stdint.h>
#include <stdbool.h>

typedef uint16_t eadk_color_t;
typedef struct { uint16_t x; uint16_t y; } eadk_point_t;
typedef struct { uint16_t x; uint16_t y; uint16_t width; uint16_t height; } eadk_rect_t;
typedef uint64_t eadk_keyboard_state_t;
typedef enum {
  eadk_key_left=0, eadk_key_up=1, eadk_key_down=2, eadk_key_right=3,
  eadk_key_ok=4, eadk_key_back=5, eadk_key_shift=6, eadk_key_alpha=7,
  eadk_key_seven=21, eadk_key_eight=22, eadk_key_nine=23,
  eadk_key_four=26, eadk_key_five=27, eadk_key_six=28,
  eadk_key_one=31, eadk_key_two=32, eadk_key_three=33,
  eadk_key_zero=36, eadk_key_dot=37, eadk_key_exe=40
} eadk_key_t;

void eadk_display_push_rect(eadk_rect_t rect, const eadk_color_t * pixels);
void eadk_display_push_rect_uniform(eadk_rect_t rect, eadk_color_t color);
void eadk_display_draw_string(const char * text, eadk_point_t point, bool large_font, eadk_color_t text_color, eadk_color_t background_color);
eadk_keyboard_state_t eadk_keyboard_scan(void);

__attribute__((always_inline)) static inline bool eadk_keyboard_key_down(eadk_keyboard_state_t state, eadk_key_t key) {
  return (state & ((uint64_t)1 << key)) != 0;
}

uint64_t eadk_timing_millis(void);
void eadk_timing_msleep(uint32_t ms);
#endif
