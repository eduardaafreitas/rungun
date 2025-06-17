#include "sprites.h"

void sprites_player(ALLEGRO_BITMAP *sheet, player *character) {
    int sprite_width = 48;  // Largura de cada sprite
    int sprite_height = 48; // Altura de cada sprite

    // Carrega sprites para a pose IDLE
    for (int i = 0; i < 4; i++) {
        character->sprites_player_idle[i] = al_create_sub_bitmap(sheet, i * sprite_width, 0, sprite_width, sprite_height);
        if (character->sprites_player_idle[i] == NULL) {
            printf("Erro ao criar sub_bitmap para IDLE sprite %d\n", i);
            exit(1);
        }
    }

    // Carrega sprites para a pose WALK
    for (int i = 0; i < 6; i++) {
        character->sprites_player_walk[i] = al_create_sub_bitmap(sheet, i * sprite_width, sprite_height, sprite_width, sprite_height);
        if (character->sprites_player_walk[i] == NULL) {
            printf("Erro ao criar sub_bitmap para WALK sprite %d\n", i);
            exit(1);
        }
    }

    // Carrega sprites para a pose RUN
    for (int i = 0; i < 6; i++) {
        character->sprites_player_run[i] = al_create_sub_bitmap(sheet, i * sprite_width, 2 * sprite_height, sprite_width, sprite_height);
        if (character->sprites_player_run[i] == NULL) {
            printf("Erro ao criar sub_bitmap para RUN sprite %d\n", i);
            exit(1);
        }
    }

    // Carrega sprites para a pose JUMP
    for (int i = 0; i < 4; i++) {
        character->sprites_player_jump[i] = al_create_sub_bitmap(sheet, i * sprite_width, 3 * sprite_height, sprite_width, sprite_height);
        if (character->sprites_player_jump[i] == NULL) {
            printf("Erro ao criar sub_bitmap para JUMP sprite %d\n", i);
            exit(1);
        }
    }

    // Carrega sprites para a pose DOWN
    for (int i = 0; i < 4; i++) {
        character->sprites_player_down[i] = al_create_sub_bitmap(sheet, i * sprite_width, 4 * sprite_height, sprite_width, sprite_height);
        if (character->sprites_player_down[i] == NULL) {
            printf("Erro ao criar sub_bitmap para DOWN sprite %d\n", i);
            exit(1);
        }
    }

    // Carrega sprites para a pose SHOOT
    for (int i = 0; i < 6; i++) {
        character->sprites_player_shoot[i] = al_create_sub_bitmap(sheet, i * sprite_width, 5 * sprite_height, sprite_width, sprite_height);
        if (character->sprites_player_shoot[i] == NULL) {
            printf("Erro ao criar sub_bitmap para SHOOT sprite %d\n", i);
            exit(1);
        }
    }
}


void sprites_enemy(ALLEGRO_BITMAP *sheet, enemy *enemy_active, int type) {
    switch (type) {
        case 1:
        case 6:
            enemy_active->sprite = al_create_sub_bitmap(sheet, 0, 0, 48, 48);
            break;
        case 2:
        case 7:
            enemy_active->sprite = al_create_sub_bitmap(sheet, 0, 0, 103, 70);
            break;
        case 3:
        case 8:
            enemy_active->sprite = al_create_sub_bitmap(sheet, 0, 0, 80, 70);
            break;
        case 4:
        case 9:
            enemy_active->sprite = al_create_sub_bitmap(sheet, 0, 0, 95, 70);
            break;
        case 5:
            enemy_active->sprite = al_create_sub_bitmap(sheet, 0, 0, 172, 172);
            break;
        case 10:
            enemy_active->sprite = al_create_sub_bitmap(sheet, 0, 0, 300, 292);
            break;
            

    }

    if(enemy_active->sprite == NULL){
        printf("Erro ao criar sub_bitmap para o sprite enemy\n");
        exit(1);
    }
}


