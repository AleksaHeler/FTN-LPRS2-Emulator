#include "raycast_renderer.h"
#include "player.h"
#include "enemy.h"
#include "stdio.h"

int main(void) {
    renderer_init(sprites_data, SPRITES_MAX_NUM);
	player_init();
	
	// Main menu: wait for player to press 'A' or 'B' button
	/*while(1){
		int input = player_menu();		// Get input
		if(input == 1) break; 			// Play the game (start)
		if(input == -1) return -1;			// Quit (B)
        renderer_menu();				// Render screen
	}*/

	while(1){
		player_update();
		enemy_update(&player);
        renderer_render(&player);
	}
	return 0;
}
