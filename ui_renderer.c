#include "ui_renderer.h"

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

// Helper functions
// Draw a number on screen
void draw_num(uint16_t number, uint16_t x, uint16_t y) {
    uint32_t u = number;

    while (u > 0) {
        uint8_t d = 0;
        u = divmod10(u, &d);
        draw_sprite(sprite_digits[d], 8, 16, x, y);
        x += 10;
    }
}

void draw_hud(player_t *player) {
    draw_sprite(background__p, background__w, 40, 0, 200);
    draw_num(111, 30, 220);
}
