#include "player.h"
#include "sprites.h"
#include "screen.h"
#include "game.h"
#include "collision.h"

player *init_player(ALLEGRO_BITMAP* sheet){
    
    player *character;
    character = malloc(sizeof(player));
    if (character == NULL){
        printf("Erro ao alocar memoria (player)\n");
        exit(1);
    }

    character->pos_x = 50;
    character->pos_y = 0;  
    character->speed = 5.0;
    character->gravity = 0.5;
    character->jump_strength = -10;
    character->score = 0;
    character->is_on_ground = 1;
    character->health_points = 10;
    character->velocity_y = 0;

    sprites_player(sheet, character);

    character->atual_pose = idle;
    character->bullet = alloc_bullets(MAX_BULLETS);
    character->time_since_last_shot = 0.0;
    character->shot_cooldown = 0.25;
    character->enemies_defeated = 0;

    return character;
}

void update_player(ALLEGRO_EVENT event, player *character, enemy *enemy_active) {
    // Atualiza o tempo desde o último disparo
    character->time_since_last_shot += 1.0 / 60.0; // Assumindo 60 FPS

    // Movimenta a nave enquanto a tecla está pressionada (desabilitado "voar")
    if (key[ALLEGRO_KEY_LEFT]) character->pos_x -= character->speed;
    if (key[ALLEGRO_KEY_RIGHT]) character->pos_x += character->speed;

    // Pulo
    if (key[ALLEGRO_KEY_UP] && character->is_on_ground) {
        character->velocity_y = character->jump_strength;
        character->is_on_ground = 0; // Não está mais no chão
    }

    // Aplicar gravidade e atualizar posição
    character->velocity_y += character->gravity;
    character->pos_y += character->velocity_y;

    // Verificar se o player tocou o chão
    if (character->pos_y >= GROUND_LEVEL) {
        character->pos_y = GROUND_LEVEL; // Corrige a posição no chão
        character->velocity_y = 0;      // Reseta a velocidade vertical
        character->is_on_ground = 1;    // Marca que está no chão
    }

    // Limitar o jogador dentro da tela no eixo X
    if (character->pos_x < 0) character->pos_x = 0;
    if (character->pos_x > SIZE_X - al_get_bitmap_width(character->sprites_player[idle])) 
        character->pos_x = SIZE_X - al_get_bitmap_width(character->sprites_player[idle]);

    // Dispara tiros contínuos
    if (key[ALLEGRO_KEY_SPACE] && character->time_since_last_shot >= character->shot_cooldown) {
        shoot_player(character);
        character->time_since_last_shot = 0; // Reseta o cooldown após disparar
    }

    // Atualiza os tiros
    update_bullets_player(character, enemy_active);
}




void draw_player(player *character){
    al_draw_bitmap(character->sprites_player[character->atual_pose], character->pos_x, character->pos_y, 0);
    draw_bullets_player(character);
}


void free_player(player *character) {
    if (character != NULL) {
        if (character->bullet != NULL) {
            free(character->bullet);
            character->bullet = NULL; // Prevenir uso após liberar
        }
        free(character);
        character = NULL;
    }
}
void shoot_player(player *character) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!character->bullet[i].active) {  // Procura por um tiro inativo
            // Ajuste de posição para o meio
            character->bullet[i].pos_x = character->pos_x + (al_get_bitmap_width(character->sprites_player[idle]) / 2) - 5; // Meio do player
            character->bullet[i].pos_y = character->pos_y + (al_get_bitmap_height(character->sprites_player[idle]) / 2) - 2; // Meio da altura 
            character->bullet[i].y_origin = character->bullet[i].pos_y;
            //printf("SP pos_y: %2f,  %2f\n", character->bullet[i].pos_y, character->bullet[i].y_origin);
            character->bullet[i].speed = 8.0; // Velocidade do tiro
            character->bullet[i].active = true; // Marca o tiro como ativo
            break;
        }
    }
}


void zizag(bullets* bullet){
    float max_top = bullet->y_origin - 12;
    float max_down = bullet->y_origin + 12;

    // printf("dir: %d\n", bullet->dir);
    // printf("pos_y: %2f,  %2f\n", bullet->pos_y, bullet->y_origin);

    if(bullet->dir == 0){ //cima 
        if ((max_top < bullet->pos_y)){
            bullet->pos_y--;
        }
        if (bullet->pos_y == max_top){
            bullet->dir = 1;
        }
    } else {
        if ((max_down > bullet->pos_y)){
            bullet->pos_y++;
        }
        if (bullet->pos_y == max_down){
            bullet->dir = 0;
        }
    }

}

void update_bullets_player(player *character, enemy *enemy_active) {
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (character->bullet[i].active) {
            // Move o tiro (desloca apenas no eixo X)
            character->bullet[i].pos_x += character->bullet[i].speed;

            //zizag(&character->bullet[i]);

            // Desativa o tiro se sair da tela
            if (character->bullet[i].pos_x > SIZE_X) {
                character->bullet[i].active = false;
                continue;
            }

            // Verifica colisão com o inimigo
            float bullet_width = 5;
            float bullet_height = 5;
            float enemy_width = al_get_bitmap_width(enemy_active->sprite[0]);
            float enemy_height = al_get_bitmap_height(enemy_active->sprite[0]);

            if (collision_detect(character->bullet[i].pos_x, character->bullet[i].pos_y, bullet_width, bullet_height,
                                enemy_active->pos_x, enemy_active->pos_y, enemy_width, enemy_height)) {
                character->bullet[i].active = false; // Desativa o tiro
                enemy_active->health_points--;   // Diminui a vida do inimigo

                // Se o inimigo for derrotado, reposicione-o ou aplique lógica adicional
                if (enemy_active->health_points <= 0) {
                    spawn_enemy(enemy_active, false);  // Reposiciona o inimigo
                    character->enemies_defeated++;
                    if (enemy_active->type == 5 || enemy_active->type == 10)
                        character->enemies_defeated++;
                    character->score += 10;        // Incrementa a pontuação do jogador
                }
            }
        }
    }
}

bool check_collision(player *character, bullets *bullet) {
    int player_width = 135; 
    int player_height = 135;
    int bullet_width = 5;
    int bullet_height = 5;  

    // Verifica se o retângulo da bala do inimigo colide com o retângulo do player
    return !(character->pos_x + player_width < bullet->pos_x || 
             character->pos_x > bullet->pos_x + bullet_width || 
             character->pos_y + player_height < bullet->pos_y || 
             character->pos_y > bullet->pos_y + bullet_height);
}

void draw_bullets_player(player *character) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (character->bullet[i].active) {
            float bullet_start_x = character->bullet[i].pos_x;
            float bullet_start_y = character->bullet[i].pos_y;
            al_draw_filled_rectangle(bullet_start_x, bullet_start_y - 2, 
                                     bullet_start_x + 10, bullet_start_y + 3, 
                                     al_map_rgb(255, 255, 0));
        }
    }
}
