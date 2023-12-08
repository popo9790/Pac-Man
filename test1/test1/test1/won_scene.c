#include "won_scene.h"
#include "scene_game.h"
#include "scene_settings.h"
#include "scene_menu.h"
#include "utility.h"
#include "shared.h"

static ALLEGRO_BITMAP* youwon;

static void init(){
    youwon = al_load_bitmap("Assets/youwon.png");
    if(!youwon){
        game_abort("failed to load youwon.png");
    }
    
    wonFont = load_font("Assets/Minecraft.ttf", 40);
    if(!wonFont){
        game_abort("failed to load wonFont");
    }
    
    WonBGM = play_bgm(wontheme, music_volume);
    
}

static int Titletime = 0;
static void draw(void){
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_scaled_bitmap(youwon, 0, 0, 50, 50,
                          SCREEN_W/2 - 150, SCREEN_H/2 - 150,
                          300, 300, 0);
    al_draw_text(
                 wonFont,
                 al_map_rgb(255, 255, 255),
                 SCREEN_W/2,
                 100 + Titletime,
                 ALLEGRO_ALIGN_CENTRE,
                 "! You Won !");
    al_draw_text(
                 wonFont,
                 al_map_rgb(255, 255, 255),
                 SCREEN_W/2,
                 650+ Titletime,
                 ALLEGRO_ALIGN_CENTRE,
                 "Press \"R\" to play again");
    al_draw_text(
                 wonFont,
                 al_map_rgb(255, 255, 255),
                 SCREEN_W/2,
                 690+ Titletime,
                 ALLEGRO_ALIGN_CENTRE,
                 "Press \"Enter\" to go back to menu");
    
    Titletime-=1.25;
    if(Titletime < -35)
        Titletime *= -1;
}

static void on_key_down(int keycode) {

    switch (keycode) {
        case ALLEGRO_KEY_ENTER:
            game_change_scene(scene_menu_create());
            break;
        case ALLEGRO_KEY_R:
            game_change_scene(scene_main_create());
            break;
        default:
            break;
    }
}

static void destory(){
    stop_bgm(WonBGM);
    al_destroy_font(wonFont);
    al_destroy_bitmap(youwon);
}


Scene scene_won_create(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    scene.name = "Won";
    scene.initialize = &init;
    scene.draw = &draw;
    scene.destroy = &destory;
    // TODO: Register more event callback functions such as keyboard, mouse, ...
    scene.on_key_down = &on_key_down;
    
    
    game_log("Won scene created");
    return scene;
}
