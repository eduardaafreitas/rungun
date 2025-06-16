#ifndef __GAME__
#define __GAME__

#include "headers.h"
#include "player.h"
#include "screen.h"

#define PATH_BG1 "resources/bg/bg1.png"
#define PATH_BG2 "resources/bg/bg2.png"
#define PATH_BG3 "resources/bg/bg3.png"

extern int wave_level;
extern float elapsed_time;
extern unsigned char key[ALLEGRO_KEY_MAX];

enum game_state {start, serving, playing, fase2, end_fase, end_game, game_over};
extern enum game_state state;

void inicia_allegro(bool teste, char *descricao);
void inicializando();

void state_start();
void state_serving();
void state_playing();
void state_fase2();
void state_end_game();
void state_game_over();
void state_pause();

void entry_identifyer(unsigned char *key, player *soldier);

//gerenciando ondas de inimigos:
void update_wave_level(player *soldier);
void manage_enemy_wave(enemy* enemy_wave, player *soldier, ALLEGRO_FONT* font);

#endif