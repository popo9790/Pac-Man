#include <allegro5/allegro_primitives.h>
#include "pacman_obj.h"
#include "map.h"
#include "shared.h"
#include "ghost.h"
/* Static variables */
static const int start_grid_x = 25, start_grid_y = 25;		// where to put pacman at the beginning
static const int fix_draw_pixel_offset_x = -3, fix_draw_pixel_offset_y = -3;  // draw offset 
static const int draw_region = 30;							// pacman bitmap draw region
static ALLEGRO_SAMPLE_ID PACMAN_MOVESOUND_ID;
// [ NOTE - speed ]
// If you want to implement something regarding speed.
// You may have to modify the basic_speed here.
// But before you modify this, make sure you are 
// totally understand the meaning of speed and function
// `step()` in `scene_game.c`, also the relationship between
// `speed`, `GAME_TICK`, `GAME_TICK_CD`, `objData->moveCD`.
static const int basic_speed = 2;
static int draw_flag = 1;
int sound_type = 0;
/* Shared variables */
extern ALLEGRO_SAMPLE* PACMAN_MOVESOUND;
extern ALLEGRO_SAMPLE* PACMAN_DEATH_SOUND;
extern ALLEGRO_SAMPLE* PACMAN_POWER_SOUND;
extern uint32_t GAME_TICK;
extern uint32_t GAME_TICK_CD;
extern bool game_over;
extern float effect_volume;

void pacman_tele(Pacman* pman, int cur);
/* Declare static function */
static bool pacman_movable(Pacman* pacman, Map* M, Directions targetDirec) {
	// [HACKATHON 1-2]
	// TODO: Determine if the current direction is movable.
	// That is to say, your pacman shouldn't penetrate 'wall' and 'room'
	// , where room is reserved for ghost to set up.
	// 1) For the current direction `targetDirec`, use pre-implemented function
	// `is_wall_block` and `is_room_block` to check if the block is wall or room. (they are both defined in map.c)
	// 2) the coordinate data of pacman is stored in pacman->objData.Coord
	// it is a self-defined pair IntInt type. Trace the code and utilize it.
    int Pacman_x = pacman->objData.Coord.x;
    int Pacman_y = pacman->objData.Coord.y;
	switch (targetDirec)
	{
	case UP:
            Pacman_y--;
            break;
	case DOWN:
            Pacman_y++;
            break;
	case LEFT:
            Pacman_x--;
            break;
	case RIGHT:
            Pacman_x++;
            break;
	default:
		// for none UP, DOWN, LEFT, RIGHT direction u should return false.
		return false;
	}
    
	if (is_wall_block(M, Pacman_x, Pacman_y) || is_room_block(M, Pacman_x, Pacman_y))
		return false;
	
	return true;
}

void pacman_tele(Pacman* pman, int cur){
    int next = (cur + 1) % 3;
    pman->objData.Coord.x = tele_x[next];
    pman->objData.Coord.y = tele_y[next];
    return;
}

Pacman* pacman_create() {

	/*
		[TODO] ok
		Allocate dynamic memory for pman pointer;
	*/
	Pacman* pman = (Pacman*)malloc(sizeof(Pacman));
	if (!pman)
		return NULL;
	/*
		Pacman* pman = ...
		if(!pman)
			return NULL;
	*/
    
	/* Init pman data */
	/* set starting point, Size, */
	/* TODO? */
	/* hint / just put it */
	pman->objData.Coord.x = 24;
	pman->objData.Coord.y = 24;
	pman->objData.Size.x = block_width;
	pman->objData.Size.y = block_height;

	pman->objData.preMove = NONE;
	pman->objData.nextTryMove = NONE;
	pman->speed = basic_speed;

	pman->death_anim_counter = al_create_timer(1.0f / 64);
	pman->powerUp = false;
    
	/* load sprites */
	pman->move_sprite = load_bitmap("Assets/pacman_move.png");
	pman->die_sprite = load_bitmap("Assets/pacman_die.png");
	return pman;

}

void pacman_destroy(Pacman* pman) {
    // [TODO] ok
    // free pacman resource
    al_destroy_bitmap(pman->move_sprite);
    al_destroy_bitmap(pman->die_sprite);
    al_destroy_timer(pman->death_anim_counter);
    free(pman);
}


void pacman_draw(Pacman* pman) {
	/*
		[HW-TODO] ok
		Draw Pacman and animations
		hint: use pman->objData.moveCD to determine which frame of the animation to draw, you may refer to discription in ghost_draw in ghost.c
	*/
	RecArea drawArea = getDrawArea(pman->objData, GAME_TICK_CD);

	//Draw default image
	
	int offset = 0;
	if (game_over) {
		/*
			hint: instead of using pman->objData.moveCD, use Pacman's death_anim_counter to create animation
            ok
		*/
        long long DieTimer = al_get_timer_count(pman->death_anim_counter) / 8;
        al_draw_scaled_bitmap(pman->die_sprite, 16 * DieTimer, 0, 16, 16,
                              drawArea.x + fix_draw_pixel_offset_x,
                              drawArea.y + fix_draw_pixel_offset_y, draw_region,
                              draw_region, 0);
        if(DieTimer == 16) die_fin = true;
	}
	else {
        switch(pman->objData.facing)
        {
            case LEFT:
                if(pman->objData.moveCD > 30){
                    al_draw_scaled_bitmap(pman->move_sprite, 48, 0, 16, 16
                                          , drawArea.x + fix_draw_pixel_offset_x,
                                          drawArea.y + fix_draw_pixel_offset_y,
                                          draw_region, draw_region, 0);
                }
                else{
                    al_draw_scaled_bitmap(pman->move_sprite, 32, 0, 16, 16
                                          , drawArea.x + fix_draw_pixel_offset_x,
                                          drawArea.y + fix_draw_pixel_offset_y,
                                          draw_region, draw_region, 0);
                }
                break;
            case RIGHT:
                if(pman->objData.moveCD > 30){
                    al_draw_scaled_bitmap(pman->move_sprite, 0, 0, 16, 16
                                          , drawArea.x + fix_draw_pixel_offset_x,
                                          drawArea.y + fix_draw_pixel_offset_y,
                                          draw_region, draw_region, 0);
                }
                else{
                    al_draw_scaled_bitmap(pman->move_sprite, 16, 0, 16, 16
                                          , drawArea.x + fix_draw_pixel_offset_x,
                                          drawArea.y + fix_draw_pixel_offset_y,
                                          draw_region, draw_region, 0);
                }
                break;
            case UP:
                if(pman->objData.moveCD > 30){
                    al_draw_scaled_bitmap(pman->move_sprite, 64, 0, 16, 16
                                          , drawArea.x + fix_draw_pixel_offset_x,
                                          drawArea.y + fix_draw_pixel_offset_y,
                                          draw_region, draw_region, 0);
                }
                else{
                    al_draw_scaled_bitmap(pman->move_sprite, 80, 0, 16, 16
                                          , drawArea.x + fix_draw_pixel_offset_x,
                                          drawArea.y + fix_draw_pixel_offset_y,
                                          draw_region, draw_region, 0);
                }
                break;
            case DOWN:
                if(pman->objData.moveCD > 30){
                    al_draw_scaled_bitmap(pman->move_sprite, 96, 0, 16, 16
                                          , drawArea.x + fix_draw_pixel_offset_x,
                                          drawArea.y + fix_draw_pixel_offset_y,
                                          draw_region, draw_region, 0);
                }
                else{
                    al_draw_scaled_bitmap(pman->move_sprite, 112, 0, 16, 16
                                          , drawArea.x + fix_draw_pixel_offset_x,
                                          drawArea.y + fix_draw_pixel_offset_y,
                                          draw_region, draw_region, 0);
                }
                break;
            default:
                al_draw_scaled_bitmap(pman->move_sprite, 0, 0,
                    16, 16,
                    drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
                    draw_region, draw_region, 0
                );
                break;
        }
		
	}
}
void pacman_move(Pacman* pacman, Map* M) {
	if (!movetime(pacman->speed))
		return;
	if (game_over)
		return;

	int probe_x = pacman->objData.Coord.x, probe_y = pacman->objData.Coord.y;
	if (pacman_movable(pacman, M, pacman->objData.nextTryMove)) 
		pacman->objData.preMove = pacman->objData.nextTryMove;
	else if (!pacman_movable(pacman, M, pacman->objData.preMove)) 
		return;

	switch (pacman->objData.preMove)
	{
	case UP:
		pacman->objData.Coord.y -= 1;
		pacman->objData.preMove = UP;
		break;
	case DOWN:
		pacman->objData.Coord.y += 1;
		pacman->objData.preMove = DOWN;
		break;
	case LEFT:
		pacman->objData.Coord.x -= 1;
		pacman->objData.preMove = LEFT;
		break;
	case RIGHT:
		pacman->objData.Coord.x += 1;
		pacman->objData.preMove = RIGHT;
		break;
	default:
		break;
	}
	pacman->objData.facing = pacman->objData.preMove;
	pacman->objData.moveCD = GAME_TICK_CD;
}
void pacman_eatItem(Pacman* pacman, const char Item) {
	switch (Item)
	{
	case '.':
        had_score++;
		stop_bgm(PACMAN_MOVESOUND_ID);
        if(sound_type == 0){
            PACMAN_MOVESOUND_ID = play_audio(PACMAN_MOVESOUND, effect_volume);
        }
        else if (sound_type == 1){
            PACMAN_MOVESOUND_ID = play_audio(PACMAN_MOVESOUND_Sec, effect_volume);
        }
		break;
            
    case 'P':
        had_score++;
        stop_bgm(PACMAN_MOVESOUND_ID);
        PACMAN_MOVESOUND_ID = play_audio(PACMAN_POWER_SOUND, effect_volume);
        break;
            
    case 'S':
        had_score++;
        stop_bgm(PACMAN_MOVESOUND_ID);
        PACMAN_MOVESOUND_ID = play_audio(PACMAN_POWER_SOUND, effect_volume);
        break;
    
    case 'T':
        //had_score++;
        //stop_bgm(PACMAN_MOVESOUND_ID);
        //PACMAN_MOVESOUND_ID = play_audio(PACMAN_POWER_SOUND, effect_volume);
	default:
		break;
	}
}

void pacman_NextMove(Pacman* pacman, Directions next) {
	pacman->objData.nextTryMove = next;
}

void pacman_die() {
	stop_bgm(PACMAN_MOVESOUND_ID);
	PACMAN_MOVESOUND_ID = play_audio(PACMAN_DEATH_SOUND, effect_volume);
}

