#include "raycast_renderer.h"
#include "player.h"


// Game engine functions
void engine_init();
int engine_main_menu();
void engine_game_loop();


int main(void) {

	// Initialize stuff
	engine_init();

	// Show main menu (play/quit game)
	if(engine_main_menu() != 0) return -1;
	
	// Process each frame
	engine_game_loop();
	
	return 0;
}


void engine_init(){
    renderer_init(sprites_data, SPRITES_MAX_NUM);
	player_init();
}

// Main menu: show screen and wait for player to press a button certain
int engine_main_menu(){
	while(1){
		int input = player_menu();		// Get input
		if(input == 1) break; 			// Play the game (a/start)
		if(input == -1) return -1;		// Quit (b/z)
        renderer_menu();				// Render screen
	}
	return 0;
}

// Loops every frame, and when game is over displays right screens and restarts/quits the game
void engine_game_loop(){
	uint32_t game_state = 0;

	while(1){
		while(1){ // Game loop
			game_state = player_update();
			if(game_state != 0) break;
			enemy_update(&player);
			renderer_render(&player);
		}

		while(1){ // Game over loop
			int input = player_menu();		// Get input
			if(input == 1) {	 			// Restart the game (a/start)
				engine_init();
				break;
			}
			if(input == -1) return;			// Quit (b/z)
			renderer_game_over(&player);	// Render game over screen
		}
	}
}
