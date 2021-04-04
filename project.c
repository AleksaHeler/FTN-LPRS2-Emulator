#include "renderer.h"
#include "player.h"

int main(void) {
    renderer_init();
	player_init();
	
	while(1){
		player_update();
        renderer_render(&playerCamera);
	}

	return 0;
}
