// [HACKATHON 3-9]
// TODO: Create scene_settings.h and scene_settings.c.
// No need to do anything for this part. We've already done it for
// you, so this 2 files is like the default scene template.
#include "scene_settings.h"
#include "scene_menu_object.h"
#include "scene_menu.h"
#include "pacman_obj.h"
#include "utility.h"
#include "shared.h"

// Variables and functions with 'static' prefix at the top level of a
// source file is only accessible in that file ("file scope", also
// known as "internal linkage"). If other files has the same variable
// name, they'll be different variables.

/* Define your static vars / function prototypes below. */

// TODO: More variables and functions that will only be accessed
// inside this scene. They should all have the 'static' prefix.

static ALLEGRO_SAMPLE_ID samplesd;

static Button Btnset;

static ALLEGRO_BITMAP* musicnote = NULL;
static ALLEGRO_BITMAP* volume_line = NULL;
static Button volume_plus;
static Button volume_minus;

static ALLEGRO_BITMAP* effectnote = NULL;
static ALLEGRO_BITMAP* effect_line = NULL;
static Button effect_plus;
static Button effect_minus;

static ALLEGRO_BITMAP* choosed_circle = NULL;
static Button choosed_dot_st;
static Button choosed_dot_sec;

static Button choosed_oni;
static ALLEGRO_BITMAP* slider_bar;
static bool slider_lock = false;

static void volume_loading(void);
static void volume_adjustment(void);
static void effect_loading(void);
static void effect_adjustment(void);
static void oni_adjustment(void);

static void init(){
    SettingFont = load_font("Assets/Minecraft.ttf", fontSize);
    SettingBGM = play_bgm(Settingmusic, music_volume);
    Btnset = button_create(730, 20, 50, 50, "Assets/back.png", "Assets/back1.png");
    choosed_dot_st = button_create(53, 453, 43, 43, "Assets/choosed_dot.png", "Assets/choosed_dot.png");
    choosed_dot_sec = button_create(53, 553, 43, 43, "Assets/choosed_dot.png", "Assets/choosed_dot.png");
    choosed_oni = button_create(500, 320 + 60 * GHOST_NUM, 110, 110, "Assets/slider.png", "Assets/slider.png");
    slider_bar = al_load_bitmap("Assets/slider_line.png");
    if(!slider_bar){
        game_abort("failed to load slider_line.png");
    }
    choosed_circle = al_load_bitmap("Assets/choose_circle.png");
    if(!choosed_circle){
        game_abort("failed to load choosed_circle.png");
    }
    
    volume_loading();
    effect_loading();
}

static void draw(void){
	al_clear_to_color(al_map_rgb(0, 0, 0));
    
    drawButton(Btnset);
    drawButton(volume_plus);
    drawButton(volume_minus);
    drawButton(effect_plus);
    drawButton(effect_minus);
    drawButton(choosed_oni);
    if(sound_type == 0){
        drawButton(choosed_dot_st);
    }
    else if (sound_type == 1){
        drawButton(choosed_dot_sec);
    }
    
    al_draw_scaled_bitmap(musicnote, 0, 0, 512, 512,
                          40, 120, 70, 70, 0);
    al_draw_scaled_bitmap(effectnote, 0, 0, 512, 512,
                          40, 220, 70, 70, 0);
    al_draw_scaled_bitmap(choosed_circle, 0, 0, 512, 512,
                          40, 440, 70, 70, 0);
    al_draw_scaled_bitmap(choosed_circle, 0, 0, 512, 512,
                          40, 540, 70, 70, 0);
    al_draw_scaled_bitmap(slider_bar, 0, 0, 56, 512,
                          540, 420, 24, 219.3, 0);
    al_draw_text(
                 SettingFont,
                 al_map_rgb(255, 255, 255),
                 SCREEN_W/2, 30,
                 ALLEGRO_ALIGN_CENTRE,
                 "Settings");
    al_draw_text(
                 SettingFont,
                 al_map_rgb(255, 255, 255),
                 SCREEN_W/2, 360,
                 ALLEGRO_ALIGN_CENTRE,
                 "Effect");
    al_draw_text(
                 SettingFont,
                 al_map_rgb(255, 255, 255),
                 200, 460,
                 ALLEGRO_ALIGN_CENTRE,
                 "Default");
    al_draw_text(
                 SettingFont,
                 al_map_rgb(255, 255, 255),
                 200, 560,
                 ALLEGRO_ALIGN_CENTRE,
                 "Lazer");
    al_draw_text(
                 SettingFont,
                 al_map_rgb(255, 255, 255),
                 700, 420,
                 ALLEGRO_ALIGN_CENTRE,
                 "Ghost 1");
    al_draw_text(
                 SettingFont,
                 al_map_rgb(255, 255, 255),
                 700, 480,
                 ALLEGRO_ALIGN_CENTRE,
                 "Ghost 2");
    al_draw_text(
                 SettingFont,
                 al_map_rgb(255, 255, 255),
                 700, 540,
                 ALLEGRO_ALIGN_CENTRE,
                 "Ghost 3");
    al_draw_text(
                 SettingFont,
                 al_map_rgb(255, 255, 255),
                 700, 600,
                 ALLEGRO_ALIGN_CENTRE,
                 "Ghost 4");
    volume_adjustment();
    effect_adjustment();
}

static void destory(){
    stop_bgm(SettingBGM);
    al_destroy_font(SettingFont);
    
    al_destroy_bitmap(Btnset.hovered_img);
    al_destroy_bitmap(Btnset.default_img);
    
    al_destroy_bitmap(musicnote);
    al_destroy_bitmap(volume_line);
    al_destroy_bitmap(volume_plus.default_img);
    al_destroy_bitmap(volume_plus.hovered_img);
    al_destroy_bitmap(volume_minus.default_img);
    al_destroy_bitmap(volume_minus.hovered_img);
    
    al_destroy_bitmap(effectnote);
    al_destroy_bitmap(effect_line);
    al_destroy_bitmap(effect_plus.default_img);
    al_destroy_bitmap(effect_plus.hovered_img);
    al_destroy_bitmap(effect_minus.default_img);
    al_destroy_bitmap(effect_minus.hovered_img);
    
    al_destroy_bitmap(choosed_circle);
    al_destroy_bitmap(choosed_dot_st.hovered_img);
    al_destroy_bitmap(choosed_dot_st.default_img);
    al_destroy_bitmap(choosed_dot_sec.hovered_img);
    al_destroy_bitmap(choosed_dot_sec.default_img);
    
    al_destroy_bitmap(slider_bar);
    al_destroy_bitmap(choosed_oni.default_img);
    al_destroy_bitmap(choosed_oni.hovered_img);
}

static void on_mouse_down() {
    if (Btnset.hovered && mouse_state[1])
        game_change_scene(scene_menu_create());
    
    if(volume_plus.hovered && mouse_state[1]){
        if(music_volume <=8){
            music_volume+=1.5;
            stop_bgm(SettingBGM);
            SettingBGM = play_bgm(Settingmusic, music_volume);
        }
    }
    if(volume_minus.hovered && mouse_state[1]){
        if(music_volume >=2){
            music_volume-=1.5;
            stop_bgm(SettingBGM);
            SettingBGM = play_bgm(Settingmusic, music_volume);
        }
    }
    
    if(effect_plus.hovered && mouse_state[1]){
        if(effect_volume <=8){
            effect_volume+=1.5;
            stop_bgm(samplesd);
            samplesd = play_audio(PACMAN_MOVESOUND, effect_volume);
            
        }
    }
    if(effect_minus.hovered && mouse_state[1]){
        if(effect_volume >=2){
            effect_volume-=1.5;
            stop_bgm(samplesd);
            samplesd = play_audio(PACMAN_MOVESOUND, effect_volume);
            
        }
    }
    
    if(choosed_dot_st.hovered && mouse_state[1]){
        sound_type = 1;
    }
    if(choosed_dot_sec.hovered && mouse_state[1]){
        sound_type = 0;
    }
    
    if(choosed_oni.hovered && mouse_state[1]){
        slider_lock = true;
    }
    else{
        slider_lock = false;
    }
}

static void on_mouse_up(){
    slider_lock = false;
}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
    Btnset.hovered = buttonHover(Btnset, mouse_x, mouse_y);
    
    volume_plus.hovered = buttonHover(volume_plus, mouse_x, mouse_y);
    volume_minus.hovered = buttonHover(volume_minus, mouse_x, mouse_y);
    
    effect_plus.hovered = buttonHover(effect_plus, mouse_x, mouse_y);
    effect_minus.hovered = buttonHover(effect_minus, mouse_x, mouse_y);
    
    choosed_dot_st.hovered = buttonHover(choosed_dot_st, mouse_x, mouse_y);
    choosed_dot_sec.hovered = buttonHover(choosed_dot_sec, mouse_x, mouse_y);
    
    choosed_oni.hovered = buttonHover(choosed_oni, mouse_x, mouse_y);
    if(slider_lock){
        if(mouse_y >= 430 && mouse_y <= 630){
            choosed_oni.body.y = mouse_y - 55;
            oni_adjustment();
        }
    }
}

static void on_key_down(int keycode) {
    switch (keycode) {
        default:
            break;
    }
}

static void volume_loading(){
    musicnote = al_load_bitmap("Assets/volume.png");
    if(!musicnote){
        game_abort("failed to load musicnote.png");
    }
    volume_line = al_load_bitmap("Assets/volume_line.png");
    if(!volume_line){
        game_abort("failed to load volume_line.png");
    }
    volume_plus = button_create(660, 120, 60, 60, "Assets/volume_plus.png", "Assets/volume_plus.png");
    volume_minus = button_create(160, 120, 60, 60, "Assets/volume_minus.png", "Assets/volume_minus.png");
}

static void volume_adjustment(){
    switch ((int)(music_volume*10)) {
        case 95:
            al_draw_scaled_bitmap(volume_line, 0, 0, 512, 512,
                                  590, 120, 60, 60, 0);
        case 80:
            al_draw_scaled_bitmap(volume_line, 0, 0, 512, 512,
                                  530, 120, 60, 60, 0);
        case 65:
            al_draw_scaled_bitmap(volume_line, 0, 0, 512, 512,
                                  470, 120, 60, 60, 0);
        case 50:
            al_draw_scaled_bitmap(volume_line, 0, 0, 512, 512,
                                  410, 120, 60, 60, 0);
        case 35:
            al_draw_scaled_bitmap(volume_line, 0, 0, 512, 512,
                                  350, 120, 60, 60, 0);
        case 20:
            al_draw_scaled_bitmap(volume_line, 0, 0, 512, 512,
                                  290, 120, 60, 60, 0);
        case 5:
            al_draw_scaled_bitmap(volume_line, 0, 0, 512, 512,
                                  230, 120, 60, 60, 0);
        default:
            break;
    }
}

static void effect_loading(){
    effectnote = al_load_bitmap("Assets/effectnote.png");
    if(!effectnote){
        game_abort("failed to load effectnote.png");
    }
    effect_line = al_load_bitmap("Assets/volume_line.png");
    if(!volume_line){
        game_abort("failed to load effect_line.png");
    }
    effect_plus = button_create(660, 220, 60, 60, "Assets/volume_plus.png", "Assets/volume_plus.png");
    effect_minus = button_create(160, 220, 60, 60, "Assets/volume_minus.png", "Assets/volume_minus.png");
}

static void effect_adjustment(){
    switch ((int)(effect_volume*10)) {
        case 95:
            al_draw_scaled_bitmap(effect_line, 0, 0, 512, 512,
                                  590, 220, 60, 60, 0);
        case 80:
            al_draw_scaled_bitmap(effect_line, 0, 0, 512, 512,
                                  530, 220, 60, 60, 0);
        case 65:
            al_draw_scaled_bitmap(effect_line, 0, 0, 512, 512,
                                  470, 220, 60, 60, 0);
        case 50:
            al_draw_scaled_bitmap(effect_line, 0, 0, 512, 512,
                                  410, 220, 60, 60, 0);
        case 35:
            al_draw_scaled_bitmap(effect_line, 0, 0, 512, 512,
                                  350, 220, 60, 60, 0);
        case 20:
            al_draw_scaled_bitmap(effect_line, 0, 0, 512, 512,
                                  290, 220, 60, 60, 0);
        case 5:
            al_draw_scaled_bitmap(effect_line, 0, 0, 512, 512,
                                  230, 220, 60, 60, 0);
        default:
            break;
    }
}

static void oni_adjustment(){
    if(choosed_oni.body.y < 440){
        GHOST_NUM = 1;
    }
    else if (choosed_oni.body.y >= 440 && choosed_oni.body.y < 500){
        GHOST_NUM = 2;
    }
    else if (choosed_oni.body.y >= 500 && choosed_oni.body.y < 540){
        GHOST_NUM = 3;
    }
    else{
        GHOST_NUM = 4;
    }
}

// The only function that is shared across files.
Scene scene_settings_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Settings";
    scene.initialize = &init;
	scene.draw = &draw;
    scene.destroy = &destory;
    scene.on_mouse_up = &on_mouse_up;
    scene.on_mouse_move = &on_mouse_move;
    scene.on_mouse_down = &on_mouse_down;
    scene.on_key_down = &on_key_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
    
    
	game_log("Settings scene created");
	return scene;
}
