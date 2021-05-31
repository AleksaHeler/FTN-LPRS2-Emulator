#include "ui_renderer.h"

///////////////////////////////////////////////////////////////////////////////
// Draws a digit at a given place:
void draw_digit(uint8_t digit, uint16_t pos_x, uint16_t pos_y, uint8_t color){
    printf("Drawing a digit...\n");
    int src_x = digit * 8; // 0 is at pixel 0, 1 is at pixel 8...
    int src_y = 0;

    for(uint16_t x = 0; x < 8; x++){ 
        for(uint16_t y = 0; y < digits__h; y++){
            uint32_t dst_idx = (pos_y+y)*SCREEN_W + pos_x+x;
            uint32_t src_idx = (y)*(digits__w/8) + (src_x+x)/8;

            uint32_t pixel = (digits__p[src_idx] >> ((src_x+x)%8)*4) & 0xf;
            if(pixel  == 0xd) continue;  // If transparent color skip this pixel
        
            buffer[dst_idx] = color;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Draws a number by calling function draw_digi() for every digit:
void draw_number(uint32_t number, uint16_t pos_x, uint16_t pos_y, uint8_t color){

    uint32_t digit_count = 0;

    if(number == 0){ // 0 has one digit
        digit_count = 1;   
    }
    else{ // Calculate num of digits
        
        uint32_t n = number;
        while (n != 0) {
            n /= 10;
            ++digit_count;
        }
    }

    if(number == 0)
        draw_digit(0, pos_x, pos_y, color);
    // Draw digit from least significant to most
    while(number > 0){
        uint8_t digit = number % 10; // Get least significant digit
        digit_count--;

        draw_digit(digit, pos_x + digit_count*8, pos_y, color); // Draw it at right place

        number /= 10; // Delete last digit
    }
}

void draw_hud(player_t *player) {
    draw_sprite_transparent(game_ui__p, game_ui__w, game_ui__h, 0, 0);
    draw_number(player->hp, 41, 213, 0xd);
    draw_number(player->ammo, 148, 213, 0xd);
    draw_number(player->score, 260, 213, 0xd);
}
