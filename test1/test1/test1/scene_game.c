#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <string.h>
#include "game.h"
#include "shared.h"
#include "utility.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "pacman_obj.h"
#include "ghost.h"
#include "map.h"
#include "won_scene.h"
#include "shared.h"
#include <errno.h>
#include <string.h>

// [HACKATHON 2-0]
// Just modify the GHOST_NUM to 1

/* global variables*/
extern const uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern ALLEGRO_TIMER* game_tick_timer;
int game_main_Score = 0;
bool game_over = false;
bool super_gomu_flag = false;
/* Internal variables*/
ALLEGRO_TIMER* power_up_timer;
static ALLEGRO_TIMER* super_gomu;
static ALLEGRO_TIMER* tele_cd;
const int power_up_duration = 10;
static Pacman* pman;
static Map* basic_map;
static Ghost** ghosts;
bool debug_mode = false;
bool cheat_mode = false;
static ALLEGRO_BITMAP* progressline = NULL;
static ALLEGRO_BITMAP* progressline_move = NULL;
static FILE* history_score2;
/* Declare static function prototypes */
static void init(void);
static void step(void);
static void checkItem(void);
static void status_update(void);
static void update(void);
static void draw(void);
static void printinfo(void);
static void destroy(void);
static void on_key_down(int key_code);
static void on_mouse_down(void);
static void render_init_screen(void);
static void draw_hitboxes(void);
static bool CheckColli(int i);
static void sort_score(void);

static void init(void) {
	game_over = false;
	game_main_Score = 0;
	// create map
	basic_map = create_map("Assets/map_nthu.txt");
	// [TODO]
	// Create map from .txt file and design your own map !!
	// basic_map = create_map("Assets/map_nthu.txt");
	if (!basic_map) {
		game_abort("error on creating map");
	}	
	// create pacman
	pman = pacman_create();
	if (!pman) {
		game_abort("error on creating pacamn\n");
	}
	
	// allocate ghost memory
	// [HACKATHON 2-1]
	// TODO: Allocate dynamic memory for ghosts array.
    ghosts = (Ghost**)malloc(sizeof(Ghost*) * GHOST_NUM);
	if(!ghosts){
		game_log("We haven't create any ghosts!\n");
	}
	else {
		// [HACKATHON 2-2]
		// TODO: create a ghost.
		// Try to look the definition of ghost_create and figure out what should be placed here.
		for (int i = 0; i < GHOST_NUM; i++) {
			game_log("creating ghost %d\n", i);
			ghosts[i] = ghost_create(i);
			if (!ghosts[i])
				game_abort("error creating ghost\n");
		}
	}
	GAME_TICK = 0;

	render_init_screen();
	power_up_timer = al_create_timer(1.0f); // 1 tick / sec
	if (!power_up_timer)
        game_abort("Error on create timer\n");
    
    super_gomu = al_create_timer(1.0f);
    if (!super_gomu)
        game_abort("Error on create timer\n");
    
    tele_cd = al_create_timer(1.0f);
    if(!tele_cd)
        game_abort("Error on creat timer\n");
    al_start_timer(tele_cd);
    
    progressline = al_load_bitmap("Assets/progressline.png");
    if(!progressline){
        game_abort("failed to load progressline.png\n");
    }
    
    progressline_move = al_load_bitmap("Assets/progressline.png");
    if(!progressline_move){
        game_abort("failed to load progressline.png\n");
    }
	return ;
}

static void step(void) {
    if (pman->objData.moveCD > 0){
        if(pman->objData.moveCD < pman->speed){
            pman->objData.moveCD = 0;
        }
        else{
            pman->objData.moveCD -= pman->speed;
        }
    }
		
	for (int i = 0; i < GHOST_NUM; i++) {
		// important for movement
		if (ghosts[i]->objData.moveCD > 0)
			ghosts[i]->objData.moveCD -= ghosts[i]->speed;
	}
}
static void checkItem(void) {
	int Grid_x = pman->objData.Coord.x, Grid_y = pman->objData.Coord.y;
	if (Grid_y >= basic_map->row_num - 1 || Grid_y <= 0 || Grid_x >= basic_map->col_num - 1 || Grid_x <= 0)
		return;
	// [HACKATHON 1-3]
	// TODO: check which item you are going to eat and use `pacman_eatItem` to deal with it.
	switch (basic_map->map[Grid_y][Grid_x])
	{
        case '.':
            pacman_eatItem(pman,basic_map->map[Grid_y][Grid_x]);
            break;;
        case 'P':
            pman->powerUp = true;
            al_stop_timer(power_up_timer);
            al_set_timer_count(power_up_timer, 0);
            for(int i=0; i<GHOST_NUM; ++i){
                ghost_toggle_FLEE(ghosts[i], pman->powerUp);
            }
            pacman_eatItem(pman, basic_map->map[Grid_y][Grid_x]);
            break;
        case 'S':
            al_start_timer(super_gomu);
            super_gomu_flag = true;
            pacman_eatItem(pman, basic_map->map[Grid_y][Grid_x]);
            break;
        case 'T':
            for(int i=0; i<tele_num; ++i){
                if(tele_x[i] == pman->objData.Coord.x && tele_y[i] == pman->objData.Coord.y){
                    if(al_get_timer_count(tele_cd) > 3){
                        TELE_SOUND_ID = play_audio(TELE_SOUND, effect_volume);
                        pacman_tele(pman, i);
                        al_set_timer_count(tele_cd, 0);
                    }
                }
            }
            break;
	default:
		break;
	}
	
	// [HACKATHON 1-4]
	// erase the item you eat from map
	// be careful no erasing the wall block.
    if(basic_map->map[Grid_y][Grid_x] != 'T'){
        basic_map->map[Grid_y][Grid_x] = ' ';
    }
}
static void status_update(void) {
	for (int i = 0; i < GHOST_NUM; i++) {
        if (ghosts[i]->status == GO_IN){
            continue;
        }
        
        if(pman->powerUp){
            al_start_timer(power_up_timer);
        }
        if(al_get_timer_count(power_up_timer) == power_up_duration){
            al_stop_timer(power_up_timer);
            al_set_timer_count(power_up_timer, 0);
            for(int j=0; j<GHOST_NUM; ++j){
                ghost_toggle_FLEE(ghosts[j], 0);
            }
            pman->powerUp = false;
        }
        
        if(super_gomu_flag){
            pman->speed = 3;
        }
        if(al_get_timer_count(super_gomu) == 3){
            al_stop_timer(super_gomu);
            al_set_timer_count(super_gomu, 0);
            pman->speed = 2;
            super_gomu_flag = false;
        }
        
        if(!cheat_mode && CheckColli(i))
        {
            if(ghosts[i]->status == FLEE){
                ghost_collided(ghosts[i]);
            }
            else{
                game_log("collide with ghost\n");
                al_rest(1.0);
                pacman_die();
                game_over = true;
                break;
            }
        }
        
		// [TODO] ok
		// use `getDrawArea(..., GAME_TICK_CD)` and `RecAreaOverlap(..., GAME_TICK_CD)` functions to detect
		// if pacman and ghosts collide with each other.
		// And perform corresponding operations.
		// [NOTE]
		// You should have some branch here if you want to implement power bean mode.
		// Uncomment Following Code
	}
}

static bool CheckColli(int i){
    return RecAreaOverlap(getDrawArea(pman->objData, GAME_TICK_CD), getDrawArea(ghosts[i]->objData, GAME_TICK_CD));
}

int compare(const void *a, const void *b)//這函式是 qsort 所需的比較函式
{
    int c = *(int *)a;
    int d = *(int *)b;
    return d-c;
}

static void sort_score(void){
    int errNum;
    history_score2 = fopen("Assets/history_score.txt", "w");
    if(!history_score2){
        errNum = errno;
        printf("open fail errno = %d reason = %s \n", errNum, strerror(errNum));
        game_abort("failed to open history_score.txt");
    }
    
    high_score[3] = had_score;
    qsort(high_score, 4, sizeof(int), compare);
    for(int i=0; i<3; ++i){
        int a = fprintf(history_score2, "%d\n",high_score[i]);
        printf("%d\n",a);
    }
    fclose(history_score2);
}
static void update(void) {
    if(!cheat_mode){
        if(had_score == basic_map->beansNum){
            sort_score();
            had_score = 0;
            al_stop_timer(super_gomu);
            al_set_timer_count(super_gomu, 0);
            super_gomu_flag = false;
            game_change_scene(scene_won_create());
            return;
        }
    }
    else{
        if(had_score == 10){
            sort_score();
            had_score = 0;
            al_stop_timer(super_gomu);
            al_set_timer_count(super_gomu, 0);
            super_gomu_flag = false;
            game_change_scene(scene_won_create());
            return;
        }
    }
    
    
	if (game_over) {
        //[TODO] ok
        //start pman->death_anim_counter and schedule a game-over event (e.g change scene to menu) after Pacman's death animation finished
        al_start_timer(pman->death_anim_counter);
        if(die_fin){
            die_fin = false;
            sort_score();
            had_score = 0;
            al_stop_timer(super_gomu);
            al_set_timer_count(super_gomu, 0);
            super_gomu_flag = false;
            game_change_scene(scene_menu_create());
        }
		return;
	}

	step();
	checkItem();
	status_update();
	pacman_move(pman, basic_map);
	for (int i = 0; i < GHOST_NUM; i++) 
		ghosts[i]->move_script(ghosts[i], basic_map, pman);
}

static void draw(void) {

	al_clear_to_color(al_map_rgb(0, 0, 0));


	//	[TODO] ok
	//	Draw scoreboard, something your may need is sprinf();
	draw_map(basic_map);

	pacman_draw(pman);
	if (game_over)
		return;
	// no drawing below when game over
	for (int i = 0; i < GHOST_NUM; i++)
		ghost_draw(ghosts[i]);
    
    char score[200];
    sprintf(score,"Score : %05d",had_score);
    al_draw_text(menuFont, al_map_rgb(255, 255, 255), 35, 15, 0, score);
    
    al_draw_scaled_bitmap(progressline, 0, 0, 182, 5, 35, 700, 728, 20, 0);
    float progressline_rate = (had_score / (float)basic_map->beansNum);
    al_draw_scaled_bitmap(progressline_move, 0, 6, 182 * progressline_rate, 5,
                          35, 700, 728 *progressline_rate, 20, 0);
    char progressline_situation[200];
    sprintf(progressline_situation,"Complete rate : %d %%",(int)(progressline_rate*100));
    al_draw_text(menuFont, al_map_rgb(255, 255, 255), 40, 750, 0, progressline_situation);
    
    if(al_get_timer_count(tele_cd) > 3){
        al_draw_text(menuFont, al_map_rgb(255, 255, 255)
                     , 580, 750, 0, "Teleport CD");
    }
    else{
        al_draw_text(menuFont, al_map_rgb(192, 192, 192)
                     , 580, 750, 0, "Teleport CD");
    }
	//debugging mode
	if (debug_mode) {
		draw_hitboxes();
	}

}

static void draw_hitboxes(void) {
	RecArea pmanHB = getDrawArea(pman->objData, GAME_TICK_CD);
	al_draw_rectangle(
		pmanHB.x, pmanHB.y,
		pmanHB.x + pmanHB.w, pmanHB.y + pmanHB.h,
		al_map_rgb_f(1.0, 0.0, 0.0), 2
	);

	for (int i = 0; i < GHOST_NUM; i++) {
		RecArea ghostHB = getDrawArea(ghosts[i]->objData, GAME_TICK_CD);
		al_draw_rectangle(
			ghostHB.x, ghostHB.y,
			ghostHB.x + ghostHB.w, ghostHB.y + ghostHB.h,
			al_map_rgb_f(1.0, 0.0, 0.0), 2
		);
	}

}

static void printinfo(void) {
	game_log("pacman:\n");
	game_log("coord: %d, %d\n", pman->objData.Coord.x, pman->objData.Coord.y);
	game_log("PreMove: %d\n", pman->objData.preMove);
	game_log("NextTryMove: %d\n", pman->objData.nextTryMove);
	game_log("Speed: %f\n", pman->speed);
}


static void destroy(void) {
	/*
		[TODO]
		free map array, Pacman and ghosts
	*/
    for(int i=0; i<GHOST_NUM; ++i){
        free(*(ghosts+i));
    }
    free(ghosts);
    free(pman);
    
    for(int i=0; i<basic_map->row_num; ++i){
        free(*(basic_map->map+i));
    }
    free(basic_map->map);
    free(basic_map);
    al_destroy_bitmap(progressline);
    al_destroy_bitmap(progressline_move);
    al_destroy_timer(super_gomu);
    al_destroy_timer(tele_cd);
}

static void on_key_down(int key_code) {
	switch (key_code)
	{
		// [HACKATHON 1-1]	
		// TODO: Use allegro pre-defined enum ALLEGRO_KEY_<KEYNAME> to controll pacman movement
		// we provided you a function `pacman_NextMove` to set the pacman's next move direction.
		case ALLEGRO_KEY_W:
			pacman_NextMove(pman,UP);
			break;
		case ALLEGRO_KEY_A:
			pacman_NextMove(pman,LEFT);
			break;
		case ALLEGRO_KEY_S:
			pacman_NextMove(pman,DOWN);
			break;
		case ALLEGRO_KEY_D:
			pacman_NextMove(pman,RIGHT);
			break;
		case ALLEGRO_KEY_C:
			cheat_mode = !cheat_mode;
			if (cheat_mode)
				printf("cheat mode on\n");
			else 
				printf("cheat mode off\n");
			break;
		case ALLEGRO_KEY_G:
			debug_mode = !debug_mode;
			break;
        default:
            break;
	}

}

static void on_mouse_down(void) {
	// nothing here

}

static void render_init_screen(void) {
	al_clear_to_color(al_map_rgb(0, 0, 0));

	draw_map(basic_map);
	pacman_draw(pman);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_draw(ghosts[i]);
	}

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 0),
		400, 400,
		ALLEGRO_ALIGN_CENTER,
		"READY!"
	);

	al_flip_display();
	al_rest(2.0);

}
// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_main_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Start";
	scene.initialize = &init;
	scene.update = &update;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_down = &on_mouse_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Start scene created");
	return scene;
}
