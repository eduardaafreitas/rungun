#ifndef __PLAYER__
#define __PLAYER__
																						
#include "headers.h"
#include "bullet.h"
#include "enemy.h"

#define PATH_PLAYER_IDLE "resources/player/2 Punk/Idle1.png"
#define GROUND_LEVEL 320

typedef enum {idle, walk, run, jump, down, shoot} playerPose;

struct player{
    ALLEGRO_BITMAP* sprites_player_idle[4];
    ALLEGRO_BITMAP* sprites_player_walk[6];
    ALLEGRO_BITMAP* sprites_player_run[6];
    ALLEGRO_BITMAP* sprites_player_jump[4];
    ALLEGRO_BITMAP* sprites_player_down[4];
    ALLEGRO_BITMAP* sprites_player_shoot[6]; //walk + arm + gun

    int current_frame;           // índice do frame atual da animação
    double frame_time;           // tempo acumulado desde o último frame
    double frame_duration;       // duração de cada frame em segundos (ex: 0.1 para 10fps)

    playerPose atual_pose;
    int shot_flag, special_flag;
    float pos_x, pos_y;
    float speed;
    float velocity_y;
    float gravity;
    float jump_strength;
    int is_on_ground;
    int score;
    int health_points;
    bullets *bullet;
    float time_since_last_shot;
    float shot_cooldown;
    int enemies_defeated;
};
typedef struct player player;

player *init_player();
void update_player(ALLEGRO_EVENT event, player *character, enemy *enemy_active, double delta_time);
void draw_player(player *character);
void free_player(player *character);

void zizag(bullets* bullet);

void shoot_player(player *character);
void update_bullets_player(player *character, enemy *enemy_active);
void draw_bullets_player(player *character);
bool check_collision(player *character, bullets *bullet);
void check_player_collision(player *character, enemy *enemy_active);


#endif