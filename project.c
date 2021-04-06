#include "renderer.h"
#include "player.h"

int main(void) {
    renderer_init();
	player_init();
	
	// TODO: add main menu here

	while(1){
		player_update();
        renderer_render(&player_camera);
	}
	return 0;
}
