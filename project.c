#include "renderer.h"
#include "player.h"

int main(void) {
    renderer_init();
	player_init();
	
	while(1){
		player_update();
        renderer_render(&playerCamera);
        // Draw an example image over everything just to test how it works
        //draw_sprite(images[0], 64, 64, 30, 30);
	}
	return 0;
}
