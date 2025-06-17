#include "sprites.h"

void sprites_player (ALLEGRO_BITMAP *sheet, player *character){
    for (int i = 0; i < 6; i++) {
        character->sprites_player[i] = al_create_sub_bitmap(sheet, i * 48, 0, 48, 48);
        if (character->sprites_player[i] == NULL) {
            printf("Erro ao criar sub_bitmap para o sprite %d\n", i);
            // Liberar bitmaps alocados anteriormente em caso de erro
            for (int j = 0; j < i; j++) {
                al_destroy_bitmap(character->sprites_player[j]);
            }
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


