#include "ui_renderer.h"

///////////////////////////////////////////////////////////////////////////////
// Draws a digit at a given place:
void draw_digit(uint8_t digit, uint16_t pos_x, uint16_t pos_y, uint8_t color) {
    int src_x = digit * 8; // 0 is at pixel 0, 1 is at pixel 8...
    int src_y = 0;

    for (uint16_t x = 0; x < 8; x++) {
        for (uint16_t y = 0; y < digits__h; y++) {
            uint32_t dst_idx = (pos_y + y) * SCREEN_W + pos_x + x;
            uint32_t src_idx = (y + src_y) * (digits__w / 8) + (src_x + x) / 8;

            uint32_t pixel = (digits__p[src_idx] >> ((src_x + x) % 8) * 4) & 0xf;
            if (pixel == 0xd) continue; // If transparent color skip this pixel

            buffer[dst_idx] = color;
        }
    }
}

// Divide by 10 with modulo
uint32_t divmod10(uint32_t in, uint8_t* mod) {
    // q = in * 0.8;
    uint32_t q = (in >> 1) + (in >> 2);
    q = q + (q >> 4);
    q = q + (q >> 8);
    q = q + (q >> 16); // not needed for 16 bit version

    // q = q / 8;  ==> q = in * 0.1;
    q = q >> 3;

    // determine error
    uint32_t r = in - ((q << 3) + (q << 1)); // r = in - q*10;
    if (r > 9)
        *mod = r - 10;
    else
        *mod = r;
    return q + (r > 9);
}

// Draw a number on screen
void draw_num(uint16_t number, uint16_t x, uint16_t y, uint8_t color) {
    static char reverse_digits[5] = {0};
    uint32_t u = number;
    uint8_t digit_count = 0;

    while (u > 0) {
        uint8_t d = 0;
        u = divmod10(u, &d);
        reverse_digits[digit_count] = d;
        digit_count++;
    }

    for (unsigned i = 0; i < digit_count; i++) {
        draw_digit(reverse_digits[digit_count - i - 1], x, y, color);
        x += 10;
    }
}

// TODO add player hp when converted to int
void draw_hud(player_t *player) {
    draw_sprite(background__p, background__w, 40, 0, 200);
    draw_num(100, 25, 210, 5);
}
