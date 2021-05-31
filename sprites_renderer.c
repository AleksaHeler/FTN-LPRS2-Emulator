#include "sprites_renderer.h"
#include "stdio.h"

///////////////////////////////////////////////////////////////////////////////
// Draws whole sprite at the given location:
void draw_sprite(uint32_t* src_p, uint16_t src_w, uint16_t src_h, uint16_t dst_x, uint16_t dst_y) {
    // Prodje kroz region na ekranu na kom treba biti iscrtano, i za svaki piksel nadje vrednost
    for(uint16_t x = 0; x < src_w; x++){ 
        for(uint16_t y = 0; y < src_h; y++){
            uint32_t dst_idx = (dst_y+y)*SCREEN_W + dst_x+x;
            uint32_t src_idx = (y)*(src_w/8) + x/8;
			
            uint32_t pixel = src_p[src_idx] >> (x%8)*4;
        
            buffer[dst_idx] = pixel;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Draws whole sprite at the given location with specific color from palette as transparent:
void draw_sprite_transparent(uint32_t* src_p, uint16_t src_w, uint16_t src_h, uint16_t dst_x, uint16_t dst_y) {
    // Prodje kroz region na ekranu na kom treba biti iscrtano, i za svaki piksel nadje vrednost
    for(uint16_t x = 0; x < src_w; x++){ 
        for(uint16_t y = 0; y < src_h; y++){
            uint32_t dst_idx = (dst_y+y)*SCREEN_W + dst_x+x;
            uint32_t src_idx = (y)*(src_w/8) + x/8;

            uint32_t pixel = (src_p[src_idx] >> (x%8)*4) & 0xf;
            if(pixel  == 0xd) continue;  // If transparent color skip this pixel
        
            buffer[dst_idx] = pixel;
		}
	}
}

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

void draw_number(uint32_t number, uint16_t pos_x, uint16_t pos_y, uint8_t color){
    uint32_t digit_count = 0;

    // Calculate num of digits
    uint32_t n = number;
    while (n != 0) {
        n /= 10;
        ++digit_count;
    }

    // Draw digit from least significant to most
    while(number > 0){
        uint8_t digit = number % 10; // Get least significant digit
        digit_count--;

        draw_digit(digit, pos_x + digit_count*8, pos_y, color); // Draw it at right place

        number /= 10; // Delete last digit
    }
}

///////////////////////////////////////////////////////////////////////////////
// Sorting sprites by distance to player so that closer ones are rendered last (over the others) 
void swap(int* order1, fp32_t* dist1, int* order2, fp32_t* dist2){
    int temp_order = *order1;
    fp32_t temp_dist = *dist1;

    *order1 = *order2;
    *dist1 = *dist2;

    *order2 = temp_order;
    *dist2 = temp_dist;
}

int partition(int* order, fp32_t* dist, int begin, int end){
    fp32_t pivot_dist = dist[end];

    int i = begin - 1;

    for (int j = begin; j < end; j++){
        if(dist[j] > pivot_dist){
            i++;
            swap(&order[i], &dist[i], &order[j], &dist[j]);
        }
    }

    swap(&order[i + 1], &dist[i + 1], &order[end], &dist[end]);

    return i + 1;
}

void quick_sort(int* order, fp32_t* dist, int begin, int end){
    if(begin < end){
        int index = partition(order, dist, begin, end);

        quick_sort(order, dist, begin, index - 1);
        quick_sort(order, dist, index + 1, end);
    }
}

// From farthest to nearest
void sort_sprites(int* order, fp32_t* dist, player_t* cam, int amount){
    // First calculate the distane frome player for each sprite
    for(int i = 0; i < amount; i++) {
        order[i] = i;
        // Sqrt not taken, only relative distance is enough
        dist[i] = (fp32_mul(cam->pos_x - sprites_data[i].x, cam->pos_x - sprites_data[i].x) + fp32_mul(cam->pos_y - sprites_data[i].y, cam->pos_y - sprites_data[i].y));
    }
    // Then sort them
    quick_sort(order, dist, 0, amount - 1);
}