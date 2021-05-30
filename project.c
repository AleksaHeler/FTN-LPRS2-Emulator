#include "raycast_renderer.h"
#include "player.h"
#include "enemy.h"
#include "stdio.h"

int main(void) {
	int game_state = 0;

    renderer_init(sprites_data, SPRITES_MAX_NUM);
	player_init();
	
	// Main menu: wait for player to press 'A' or 'B' button
	while(1){
		int input = player_menu();		// Get input
		if(input == 1) break; 			// Play the game (start)
		if(input == -1) return -1;		// Quit (B)
        renderer_menu();				// Render screen
	}

	// This loop holds game loop and game over loop
	while(1){
		while(1){ // Game loop
			game_state = player_update();
			if(game_state != 0) break;
			enemy_update(&player);
			renderer_render(&player);
		}

		// Game over menu loop: wait for player to press 'A' or 'B' button
		if(game_state < 0){ // LOSE
			printf("You lose!\n");
			while(1){
				int input = player_menu();		// Get input
				if(input == 1) break; 			// Restart the game (A)
				if(input == -1) return -1;		// Quit (B)
				renderer_game_over(-1);			// Render screen
			}
		} else { // WIN
			printf("You win!\n");
			while(1){
				int input = player_menu();		// Get input
				if(input == 1) break; 			// Restart the game (A)
				if(input == -1) return -1;		// Quit (B)
				renderer_game_over(-1);			// Render screen
			}
		}
	}
	
	return 0;
}
