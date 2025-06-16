#include "player.h"
#include "sprites.h"
#include "screen.h"
#include "game.h"
#include "collision.h"

player *init_player(ALLEGRO_BITMAP* sheet){
    
    player *soldier;
    soldier = malloc(sizeof(player));
    if (soldier == NULL){
        printf("Erro ao alocar memoria (player)\n");
        exit(1);
    }
    soldier->pos_x = 50;
    soldier->pos_y = SIZE_Y/2;  
    soldier->speed = 5.0;
    soldier->score = 0;
    soldier->health_points = 10;
    
    sprites_player(sheet, soldier);
    soldier->atual_pose = standard;
    soldier->bullet = alloc_bullets(MAX_BULLETS);
    soldier->time_since_last_shot = 0.0;
    soldier->shot_cooldown = 0.25;
    soldier->enemies_defeated = 0;

    return soldier;
}

void update_player(ALLEGRO_EVENT event, player *soldier, enemy *enemy_active) {
    // Atualiza o tempo desde o último disparo
    soldier->time_since_last_shot += 1.0 / 60.0; // Assumindo 60 FPS

    // Movimenta a nave enquanto a tecla está pressionada
    if (key[ALLEGRO_KEY_UP]) soldier->pos_y -= soldier->speed;
    if (key[ALLEGRO_KEY_DOWN]) soldier->pos_y += soldier->speed;
    if (key[ALLEGRO_KEY_LEFT]) soldier->pos_x -= soldier->speed;
    if (key[ALLEGRO_KEY_RIGHT]) soldier->pos_x += soldier->speed;

    // Limitar o jogador dentro da tela
    if (soldier->pos_y < 0) soldier->pos_y = 0;
    if (soldier->pos_y > SIZE_Y - al_get_bitmap_height(soldier->sprites_player[standard])) 
        soldier->pos_y = SIZE_Y - al_get_bitmap_height(soldier->sprites_player[standard]);
    if (soldier->pos_x < 0) soldier->pos_x = 0;
    if (soldier->pos_x > SIZE_X - al_get_bitmap_width(soldier->sprites_player[standard])) 
        soldier->pos_x = SIZE_X - al_get_bitmap_width(soldier->sprites_player[standard]);

    // Dispara tiros contínuos
    if (key[ALLEGRO_KEY_SPACE] && soldier->time_since_last_shot >= soldier->shot_cooldown) {
        shoot_player(soldier);
        soldier->time_since_last_shot = 0; // Reseta o cooldown após disparar
    }

    // Atualiza os tiros
    update_bullets_player(soldier, enemy_active);
}


void draw_player(player *soldier){
    al_draw_bitmap(soldier->sprites_player[soldier->atual_pose], soldier->pos_x, soldier->pos_y, 0);
    draw_bullets_player(soldier);
}

void free_player(player *soldier) {
    if (soldier != NULL) {
        if (soldier->bullet != NULL) {
            free(soldier->bullet);
            soldier->bullet = NULL; // Prevenir uso após liberar
        }
        free(soldier);
        soldier = NULL;
    }
}
void shoot_player(player *soldier){
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!soldier->bullet[i].active) {  // Procura por um tiro inativo
            // Ajuste de posição para o meio da nave
            soldier->bullet[i].pos_x = soldier->pos_x + (al_get_bitmap_width(soldier->sprites_player[standard]) / 2) - 5; // Meio da nave
            soldier->bullet[i].pos_y = soldier->pos_y + (al_get_bitmap_height(soldier->sprites_player[standard]) / 2) - 2; // Meio da altura da nave
            soldier->bullet[i].y_origin = soldier->bullet[i].pos_y;
            //printf("SP pos_y: %2f,  %2f\n", soldier->bullet[i].pos_y, soldier->bullet[i].y_origin);
            soldier->bullet[i].speed = 8.0; // Velocidade do tiro
            soldier->bullet[i].active = true; // Marca o tiro como ativo
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

void update_bullets_player(player *soldier, enemy *enemy_active) {
    
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (soldier->bullet[i].active) {



            // Move o tiro (desloca apenas no eixo X)
            soldier->bullet[i].pos_x += soldier->bullet[i].speed;


            //zizag(&soldier->bullet[i]);
            
            

            // Desativa o tiro se sair da tela
            if (soldier->bullet[i].pos_x > SIZE_X) {
                soldier->bullet[i].active = false;
                continue;
            }

            // Verifica colisão com o inimigo
            float bullet_width = 5;
            float bullet_height = 5;
            float enemy_width = al_get_bitmap_width(enemy_active->sprite);
            float enemy_height = al_get_bitmap_height(enemy_active->sprite);

            if (collision_detect(soldier->bullet[i].pos_x, soldier->bullet[i].pos_y, bullet_width, bullet_height,
                                enemy_active->pos_x, enemy_active->pos_y, enemy_width, enemy_height)) {
                soldier->bullet[i].active = false; // Desativa o tiro
                enemy_active->health_points--;   // Diminui a vida do inimigo

                // Se o inimigo for derrotado, reposicione-o ou aplique lógica adicional
                if (enemy_active->health_points <= 0) {
                    spawn_enemy(enemy_active, false);  // Reposiciona o inimigo
                    soldier->enemies_defeated++;
                    if (enemy_active->type == 5 || enemy_active->type == 10)
                        soldier->enemies_defeated++;
                    soldier->score += 10;        // Incrementa a pontuação do jogador
                }
            }
        }
    }
}



bool check_collision(player *soldier, bullets *bullet) {
    int player_width = 135; 
    int player_height = 135;
    int bullet_width = 5;
    int bullet_height = 5;  

    // Verifica se o retângulo da bala do inimigo colide com o retângulo do player
    return !(soldier->pos_x + player_width < bullet->pos_x || 
             soldier->pos_x > bullet->pos_x + bullet_width || 
             soldier->pos_y + player_height < bullet->pos_y || 
             soldier->pos_y > bullet->pos_y + bullet_height);
}

void draw_bullets_player(player *soldier) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (soldier->bullet[i].active) {
            float bullet_start_x = soldier->bullet[i].pos_x;
            float bullet_start_y = soldier->bullet[i].pos_y;
            al_draw_filled_rectangle(bullet_start_x, bullet_start_y - 2, 
                                     bullet_start_x + 10, bullet_start_y + 3, 
                                     al_map_rgb(255, 255, 0));
        }
    }
}
