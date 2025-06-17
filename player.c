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

    character->current_frame = 0;
    character->frame_time = 0.0;
    character->frame_duration = 0.1; // 10 frames por segundo, por exemplo


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

void update_player(ALLEGRO_EVENT event, player *character, enemy *enemy_active, double delta_time) {
    // Atualiza o tempo desde o último disparo
    character->time_since_last_shot += delta_time;

    // Atualiza o tempo da animação
    character->frame_time += delta_time;
    if (character->frame_time >= character->frame_duration) {
        character->frame_time -= character->frame_duration;
        character->current_frame++;
        int max_frames = 1; // padrão, será alterado abaixo

        // Define max_frames de acordo com a pose atual
        switch (character->atual_pose) {
            case idle: max_frames = 4; break;
            case walk: max_frames = 6; break;
            case run: max_frames = 6; break;
            case jump: max_frames = 4; break;
            case down: max_frames = 4; break;
            case shoot: max_frames = 6; break;
            default: max_frames = 4; break;
        }

        if (character->current_frame >= max_frames)
            character->current_frame = 0;
    }
    // Atualiza o tempo desde o último disparo
    character->time_since_last_shot += 1.0 / 60.0; // Assumindo 60 FPS

    // Flags para determinar o estado do jogador
    int moving_horizontal = 0;

    // Aplica gravidade se não estiver no chão
    if (!character->is_on_ground) {
        character->velocity_y += character->gravity; // Aceleração devido à gravidade
        character->pos_y += character->velocity_y;  // Atualiza a posição vertical
    }

    // Verifica se o jogador atingiu o chão
    if (character->pos_y >= GROUND_LEVEL) {
        character->pos_y = GROUND_LEVEL;            // Ajusta a posição ao nível do chão
        character->velocity_y = 0;                 // Reseta a velocidade vertical
        character->is_on_ground = 1;               // Marca como no chão
    }

    // Movimenta o jogador horizontalmente
    if (key[ALLEGRO_KEY_LEFT]) {
        character->pos_x -= character->speed;
        character->atual_pose = walk; // Ajusta pose para andar
        moving_horizontal = 1;
    }
    if (key[ALLEGRO_KEY_RIGHT]) {
        character->pos_x += character->speed;
        character->atual_pose = walk; // Ajusta pose para andar
        moving_horizontal = 1;
    }

    // Lógica do pulo
    if (key[ALLEGRO_KEY_UP] && character->is_on_ground) {
        character->velocity_y = character->jump_strength; // Aplica força do pulo
        character->is_on_ground = 0;                     // Marca como fora do chão
        character->atual_pose = jump;                    // Ajusta a pose para pulo
    }

    // Limitar o jogador dentro da tela
    if (character->pos_y < 0) character->pos_y = 0;
    if (character->pos_y > SIZE_Y - al_get_bitmap_height(character->sprites_player_idle[0]))
        character->pos_y = SIZE_Y - al_get_bitmap_height(character->sprites_player_idle[0]);
    if (character->pos_x < 0) character->pos_x = 0;
    if (character->pos_x > SIZE_X - al_get_bitmap_width(character->sprites_player_idle[0]))
        character->pos_x = SIZE_X - al_get_bitmap_width(character->sprites_player_idle[0]);

    // Caso o jogador esteja parado, volte à pose padrão
    if (!moving_horizontal && character->is_on_ground) {
        character->atual_pose = idle;
    }

    // Dispara tiros contínuos
    if (key[ALLEGRO_KEY_SPACE] && character->time_since_last_shot >= character->shot_cooldown) {
        shoot_player(character);
        character->atual_pose = shoot; // Ajusta pose para atirar
        character->time_since_last_shot = 0;      // Reseta o cooldown após disparar
    }

    // Atualiza os tiros
    update_bullets_player(character, enemy_active);
}



void draw_player(player *character) {
    ALLEGRO_BITMAP *current_frame = NULL;

    // Seleciona o quadro atual da animação com base na pose
    switch (character->atual_pose) {
        case idle:
            current_frame = character->sprites_player_idle[0]; 
            break;
        case walk:
            current_frame = character->sprites_player_walk[0]; 
            break;
        case run:
            current_frame = character->sprites_player_run[0]; 
            break;
        case jump:
            current_frame = character->sprites_player_jump[0]; 
            break;
        case down:
            current_frame = character->sprites_player_down[0]; 
            break;
        case shoot:
            current_frame = character->sprites_player_shoot[0]; 
            break;
        default:
            current_frame = character->sprites_player_idle[0]; // Caso padrão
            break;
    }

    // Desenha o quadro atual
    if (current_frame) {
        al_draw_bitmap(current_frame, character->pos_x, character->pos_y, 0);
    }

    // Desenha os tiros do jogador
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
            // Determina a largura e altura do sprite atual com base na pose
            ALLEGRO_BITMAP *current_sprite = NULL;

            switch (character->atual_pose) {
                case idle:
                    current_sprite = character->sprites_player_idle[0]; // Primeiro frame da pose idle
                    break;
                case walk:
                    current_sprite = character->sprites_player_walk[0]; // Primeiro frame da pose walk
                    break;
                case run:
                    current_sprite = character->sprites_player_run[0]; // Primeiro frame da pose run
                    break;
                case jump:
                    current_sprite = character->sprites_player_jump[0]; // Primeiro frame da pose jump
                    break;
                case down:
                    current_sprite = character->sprites_player_down[0]; // Primeiro frame da pose down
                    break;
                case shoot:
                    current_sprite = character->sprites_player_shoot[0]; // Primeiro frame da pose shoot
                    break;
                default:
                    current_sprite = character->sprites_player_idle[0]; // Caso padrão
                    break;
            }

            // Ajuste de posição para o centro do sprite atual
            if (current_sprite) {
                character->bullet[i].pos_x = character->pos_x + (al_get_bitmap_width(current_sprite) / 2) - 5; // Meio do sprite
                character->bullet[i].pos_y = character->pos_y + (al_get_bitmap_height(current_sprite) / 2) - 2; // Meio da altura do sprite
            } else {
                // Caso algo dê errado, posiciona o tiro no meio da tela como fallback
                character->bullet[i].pos_x = character->pos_x + 10;
                character->bullet[i].pos_y = character->pos_y + 10;
            }

            // Define propriedades do tiro
            character->bullet[i].y_origin = character->bullet[i].pos_y;
            character->bullet[i].speed = 8.0;        // Velocidade do tiro
            character->bullet[i].active = true;     // Marca o tiro como ativo

            // Ajusta a pose para indicar o disparo (se aplicável)
            character->atual_pose = shoot;

            break; // Encontra o primeiro tiro inativo e sai do loop
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
            float enemy_width = al_get_bitmap_width(enemy_active->sprite);
            float enemy_height = al_get_bitmap_height(enemy_active->sprite);

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
