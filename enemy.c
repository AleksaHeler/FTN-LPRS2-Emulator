#include "enemy.h"


void enemies_init(){
    for(int i = 0; i < num_enemies; i++){
        enemies_data[i].sprite->visible = 1;
        enemies_data[i].hp = DEFAULT_ENEMY_HP;
        // Reset position
        enemies_data[i].sprite->x = enemies_data[i].original_x;
        enemies_data[i].sprite->y = enemies_data[i].original_y;
    }
}

void enemy_update(player_t* player){
    // Animate all enemiesprint 
    for(int i = 0; i < num_enemies; i++){
        if (enemies_data[i].hp == 0) continue;
        // Distances from enemy struct, squared so we dont have to use sqrt() for distance
        fp32_t enemy_to_player_dist = fp32_mul(enemies_data[i].dist_to_player, enemies_data[i].dist_to_player);
        fp32_t enemy_max_shot_distance = fp32_mul(enemies_data[i].max_shot_distance, enemies_data[i].max_shot_distance);
        fp32_t enemy_view_distance = fp32_mul(enemies_data[i].view_distance, enemies_data[i].view_distance);

        /////////////// Calculate distance to player ///////////////
        // { dist_x, dist_y } is a vector pointing from enemy to player
        fp32_t dist_x = player->pos_x - enemies_data[i].sprite->x;
        fp32_t dist_y = player->pos_y - enemies_data[i].sprite->y;
        fp32_t dist_to_player = fp32_mul(dist_x, dist_x) + fp32_mul(dist_y, dist_y);

        /////////////// Use DDA alg. to find wall we are looking at ///////////////
        int hit = 0, side, step_x, step_y;
        int map_x = fp32_to_int(enemies_data[i].sprite->x);
        int map_y = fp32_to_int(enemies_data[i].sprite->y);
        fp32_t side_dist_x, side_dist_y, dist_to_wall;
        fp32_t delta_dist_x = fp32_abs(fp32_div(FP32(1), dist_x));
        fp32_t delta_dist_y = fp32_abs(fp32_div(FP32(1), dist_y));
        if(dist_x < 0) {
            step_x = -1;
            side_dist_x = fp32_mul(enemies_data[i].sprite->x - fp32_from_int(map_x), delta_dist_x);
        } else {
            step_x = 1;
            side_dist_x = fp32_mul(fp32_from_int(map_x + 1) - enemies_data[i].sprite->x, delta_dist_x);
        }
        if(dist_y < 0) {
            step_y = -1;
            side_dist_y = fp32_mul(enemies_data[i].sprite->y - fp32_from_int(map_y), delta_dist_y);
        } else {
            step_y = 1;
            side_dist_y = fp32_mul(fp32_from_int(map_y + 1) - enemies_data[i].sprite->y, delta_dist_y);
        }
        dda(&hit, &map_x, &map_y, &step_x, &step_y, &side_dist_x, &side_dist_y, &delta_dist_x, &delta_dist_y, &side, &dist_to_wall);
        
        /////////////// Compare coordinates of wall the enemy is looking at when looking at player and player coordinates
        // In case the player is in front of the wall -> we are looking at player
        int player_map_x = fp32_to_int(player->pos_x);
        int player_map_y = fp32_to_int(player->pos_y);
        int player_to_wall_vector_x = map_x - player_map_x;
        int player_to_wall_vector_y = map_y - player_map_y;
        int looking_at_player = 1;  // Assume we are looking at player
        if(dist_x < 0 && player_to_wall_vector_x > 0) // Check for each direction
            looking_at_player = 0;
        else if(dist_x > 0 && player_to_wall_vector_x < 0)
            looking_at_player = 0;
        if(dist_y < 0 && player_to_wall_vector_y > 0)
            looking_at_player = 0;
        else if(dist_y > 0 && player_to_wall_vector_y < 0)
            looking_at_player = 0;

        /////////////// If we are looking at the player (and not a wall), and he is in view distance 
        if(looking_at_player && dist_to_player < enemy_view_distance){
            //////// If distance is in some range (a to b) -> move towards player until distance is 'a'
            if(dist_to_player - FP32F(0.1) > enemy_to_player_dist){
                fp32_t speed = enemies_data[i].speed;
                enemies_data[i].sprite->x += fp32_mul(dist_x, fp32_mul(SECONDS_PER_FRAME, speed));
                enemies_data[i].sprite->y += fp32_mul(dist_y, fp32_mul(SECONDS_PER_FRAME, speed));
            }

            //////// If distance is too close (less than 'a') -> move backwards if there is no wall there
            if (dist_to_player + FP32F(0.1) < enemy_to_player_dist) {
                // Check if there is a wall behind this position
                // Check point is further from next position point, so we dont get too close to the wall
                fp32_t new_pos_x = enemies_data[i].sprite->x - fp32_mul(dist_x, FP32F(0.015));
                fp32_t new_pos_y = enemies_data[i].sprite->y - fp32_mul(dist_y, FP32F(0.015));
                int check_x = fp32_to_int(enemies_data[i].sprite->x - fp32_mul(dist_x, FP32F(0.4)));
                int check_y = fp32_to_int(enemies_data[i].sprite->y - fp32_mul(dist_y, FP32F(0.4)));
                if(world_map[check_x][check_y] == 0){
                    enemies_data[i].sprite->x = new_pos_x;
                    enemies_data[i].sprite->y = new_pos_y;
                }
            }

            //////// If distance to player is in shooting range (a to b) -> shoot at player on regular interval
            if (dist_to_player < enemy_max_shot_distance) {

                if(frame_count % enemies_data[i].shoot_interval == 0){
                    player_take_damage(enemies_data[i].damage);
                }
            }
        }
    }
}

void enemy_take_damage(enemy_t* enemy, uint8_t damage){
    if (enemy->hp > damage) {
        enemy->hp -= damage;
    } else {
        enemy->hp = 0;
        enemy->sprite->visible = 0;
        player.score += enemy->damage;
    }
}

enemy_t* find_enemy_by_sprite(sprite_t* sprite) {
    for (int i = 0; i < num_enemies; i++) {
        if (enemies_data[i].sprite == sprite) return &enemies_data[i];
    }
    return 0;
}