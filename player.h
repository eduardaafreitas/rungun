#ifndef __PLAYER__
#define __PLAYER__
																						
#include "headers.h"
#include "bullet.h"
#include "enemy.h"

#define PATH_PLAYER "resources/"

typedef enum {idle, walk, run, jump, down, shoot} playerPose;

struct player{
    ALLEGRO_BITMAP* sprites_player[5];
    playerPose atual_pose;
    int shot_flag, special_flag;
    float pos_x, pos_y;
    float speed;
    int score;
    int health_points;
    bullets *bullet;
    float time_since_last_shot;
    float shot_cooldown;
    int enemies_defeated;
};
typedef struct player player;

player *init_player();
void update_player(ALLEGRO_EVENT event, player *character, enemy *enemy_active);
void draw_player(player *character);
void free_player(player *character);

void zizag(bullets* bullet);

void shoot_player(player *character);
void update_bullets_player(player *character, enemy *enemy_active);
void draw_bullets_player(player *character);
bool check_collision(player *character, bullets *bullet);
void check_player_collision(player *character, enemy *enemy_active);


#endif