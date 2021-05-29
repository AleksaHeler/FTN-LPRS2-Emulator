#include "raycast_renderer.h"
#include "player.h"

int main(void) {

	#ifdef DEBUG
		// Keeping track of when frame rendering started an how many milliseconds passed
		clock_t frame_start = clock();
		double passed_ms;
	#endif

    renderer_init(sprites_data, SPRITES_MAX_NUM);
	player_init();
	
	// Main menu: wait for player to press 'A' or 'B' button
	while(1){
		int input = player_menu();		// Get input
		if(input == 1) break; 			// Play the game (start)
		if(input == -1) return -1;			// Quit (B)
        renderer_menu();				// Render screen
	}


	while(1){
	
		player_update();
        renderer_render(&player_camera);

		#ifdef DEBUG
			//// FPS LIMITER ////////////////////////////////////////////////////////////
			do{ // Waits until at least 16.67ms passed (1/60 for max of 60fps)
				passed_ms = 1000*((double)(clock() - frame_start)) / CLOCKS_PER_SEC;
			} while(passed_ms <= 1000.0/MAX_FPS);
			printf("FPS: %.2f\n", 1000.0/passed_ms);
			frame_start = clock();
			/////////////////////////////////////////////////////////////////////////////
		#endif
	}
	return 0;
}
