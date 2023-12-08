// [shared.c]
// you should define the shared variable declared in the header here.

#include "shared.h"
#include "utility.h"
#include "game.h"
// #include "scene_menu.h"
int GHOST_NUM = 4;

ALLEGRO_FONT* font_pirulen_32;
ALLEGRO_FONT* font_pirulen_24;
ALLEGRO_SAMPLE* themeMusic = NULL;
ALLEGRO_SAMPLE* PACMAN_MOVESOUND = NULL;
ALLEGRO_SAMPLE* PACMAN_MOVESOUND_Sec = NULL;
ALLEGRO_SAMPLE* PACMAN_MOVESOUND_thr = NULL;
ALLEGRO_SAMPLE* PACMAN_DEATH_SOUND = NULL;
ALLEGRO_SAMPLE* PACMAN_POWER_SOUND = NULL;
ALLEGRO_SAMPLE* Settingmusic = NULL;
ALLEGRO_SAMPLE_ID SettingBGM;
ALLEGRO_FONT* SettingFont;
ALLEGRO_SAMPLE* wontheme = NULL;
ALLEGRO_SAMPLE_ID WonBGM;
ALLEGRO_FONT* menuFont = NULL;
ALLEGRO_FONT* wonFont = NULL;
ALLEGRO_SAMPLE_ID TELE_SOUND_ID;
ALLEGRO_SAMPLE* TELE_SOUND = NULL;

int had_score = 0;
int fontSize = 30;
float music_volume = 0.5;
float effect_volume = 0.5;
bool gameDone = false;
bool die_fin = false;

int tele_num = 0;
int* tele_x;
int* tele_y;

int high_score[4];

/*
	A way to accelerate is load assets once.
	And delete them at the end.
	This method does provide you better management of memory.
	
*/
void shared_init(void) {
	menuFont = load_font("Assets/Minecraft.ttf", fontSize);
	themeMusic = load_audio("Assets/Music/original_theme.ogg");
    Settingmusic = load_audio("Assets/Music/SettingsMusic.ogg");
    wontheme = load_audio("Assets/Music/wonsong.ogg");
	PACMAN_MOVESOUND = load_audio("Assets/Music/pacman-chomp.ogg");
	PACMAN_DEATH_SOUND = load_audio("Assets/Music/pacman_death.ogg");
    PACMAN_POWER_SOUND = load_audio("Assets/Music/pacman_eatfruit.ogg");
    PACMAN_MOVESOUND_Sec = load_audio("Assets/Music/pacman_movesound2.ogg");
    TELE_SOUND = load_audio("Assets/Music/tele_sound.ogg");
}

void shared_destroy(void) {

	al_destroy_font(menuFont);
	al_destroy_sample(themeMusic);
	al_destroy_sample(PACMAN_MOVESOUND);
	al_destroy_sample(PACMAN_DEATH_SOUND);
    al_destroy_sample(PACMAN_MOVESOUND_Sec);
    al_destroy_sample(Settingmusic);
    al_destroy_sample(wontheme);
    al_destroy_sample(TELE_SOUND);
    free(tele_x);
    free(tele_y);
}
