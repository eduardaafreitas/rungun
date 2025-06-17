#ifndef __SPRITES__
#define __SPRITES__


#include "headers.h"
#include "player.h"
#include "enemy.h"

void sprites_player(ALLEGRO_BITMAP *sprites, player *character);
void sprites_enemy(ALLEGRO_BITMAP *sheet, enemy *enemy_active, int type);

#endif