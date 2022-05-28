#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
/** \brief funkcja losujaca liczby
 * @param from
 * @param to
 * @return Funkcja zwraca liczbe
 */
int LosoweLiczby(int from, int to)
{
    return from + rand() % to;
}

//* engine

#define PLAYERWIDTH 45
#define PLAYERHEIGHT 45
#define NETSIZE_X 36
#define NETSIZE_Y 14
#define NUMBER_OF_ENEMIES 10
#define SIZE_OF_SQUARE 50
#define BULLETS_NUMBER 4
#define ANIMATION_ARRAY_SIZE 40
#define NUMBER_OF_BONUSES 2
#define MAP "maps/map_1.txt"

enum typyGruntu
{
    EMPTY = 0,
    SCALE,
    DIRT,
    WALL,
};

enum kierunek
{
    LEFT = 0,
    RIGHT,
    UP,
    DOWN,
    UP_and_RIGHT,
};

enum postac
{
    PLAYER = 0,
    ENEMY,
};

enum trybyGry
{
    MAIN_MENU,
    GAME,
};

enum typyBonusu
{
    EXTRA_LIFE,
    EXTRA_SPEED,
};

// struktura przechowywujaca informacje o pojedynczym elemencie siatki mapy
struct kwadrat
{
    int id, type_of_texture;  // zmienna przechowuja liczbe odpowiadajaca typowi textury dla danego kwadratu (np 1 - SCALE)
    int x, y;                 // polozenie kwadratu
    Texture2D square_texture; // zmienna przechowywujaca texture dla kwadratu
};

struct ScianaNaMapie
{
    Rectangle wall;
    int id;
};

// struktura przechowywujaca texture z kierunkiem czolgu
struct tank_direction_textures
{
    Texture2D direction;
};

struct tank
{
    int x, y, direction, speed, movement_counter, lives;
    Rectangle tank_collision_model;
    Texture2D tank_direction;
};

struct menu_textures
{
    Texture2D menu_background, menu_foreground;
    Texture2D tank_1;
    Texture2D button_start_off;
    Texture2D button_start_on;
    Texture2D button_map1_off;
    Texture2D button_map1_on;
    Texture2D button_map2_off;
    Texture2D button_map2_on;
    Texture2D button_map3_off;
    Texture2D button_map3_on;
    int animation_movement_1, animation_movement_2;
    int animation_counter;
};

struct end_window_textures
{
    Texture2D you_win_on;
    Texture2D you_win_off;
    Texture2D you_lose_on;
    Texture2D you_lose_off;
};

struct bonus_textures
{
    Texture2D extra_life_texture;
    Texture2D extra_speed_texture;
};

struct bonus_data
{
    int type_of_bonus;
    Rectangle bonus_collision_mode;
    bool is_used;
};

struct kwadrat pola_siatki[NETSIZE_Y][NETSIZE_X];
extern int gamemode;
extern int which_map;
extern int map_loaded;
extern int bullet_speed;
extern int score;
extern int enemies_alive;
extern int LosoweLiczby(int from, int to);

//* engine koniec
const int screenWidth = 1800;
const int screenHeight = 900;
int bullet_speed = 10;
int walls_number = 0;
int score = 0;
int enemies_alive = NUMBER_OF_ENEMIES;
int gamemode = MAIN_MENU;
int which_map = 3;
int map_loaded = 0;
bool is_game_loaded = false;

//*animations

struct animation_data
{
    int x;
    int y;
    int frames_counter;
    int direction;
    bool is_used;
};

/** \brief funkcja wczytujaca grafike eksplozji
* @param explosion_animation_textures *explosion_textures
* @return Funkcja nie zwraca wartosci
*/
struct explosion_animation_textures
{
    Texture2D stage_1;
    Texture2D stage_2;
    Texture2D stage_3;
    Texture2D stage_4;
    Texture2D stage_5;
    Texture2D stage_6;
    Texture2D stage_7;
};

/** \brief funkcja wyswietlajaca animacje eksplozji
* @param animation_data *animation - infomacja o animcji
* @param explosion_animation_textures explosion_animation - grafika animacji ekspolzji
* @param index
* @return Funkcja nie zwraca wartosci
*/
void draw_explosion_animation(struct animation_data *animation, struct explosion_animation_textures explosion_animation, int index);
/** \brief funkcja wczytujaca grafike eksplozji
* @param explosion_animation_textures *explosion_textures
* @return Funkcja nie zwraca wartosci
*/
void load_explosion_textures(struct explosion_animation_textures *explosion_textures);

/** \brief funkcja usuwajaca z pamieci grafike eksplozji
* @param explosion_animation_textures *explosion_textures - grafika animacji ekspolzji
* @return Funkcja nie zwraca wartosci
*/
void unload_explosion_textures(struct explosion_animation_textures *explosion_textures);

//* animations koniec
//* collisions

/** \brief funkcja tworzy modele kolizyjne scian
* @param ScianaNaMapie *walls - sciany
* @param kwadrat pola_siatki[][NETSIZE_X] - pole siatki
* @return Funkcja nie zwraca wartosci
*/
// funkcja tworzy modele kolizyjne scian
void createWallsColisionModels(struct ScianaNaMapie *walls, struct kwadrat pola_siatki[][NETSIZE_X]);

/** \brief funkcja rysuje modele kolizyjne scian (funkcja pomocnicza)
* @param ScianaNaMapie *walls - sciany
* @param walls_number - numer sciany
* @return Funkcja nie zwraca wartosci
*/

// funkcja rysuje modele kolizyjne scian (funkcja pomocnicza)
void drawWallsColisionModels(struct ScianaNaMapie *walls, int walls_number);
/** \brief funkcja wykrywa kolizje z gory
* @param Rectangle tank - czolg
* @param Rectangle object - objekt
* @param speed - predkosc
* @return Funkcja nie zwraca wartosci
*/

// funkcja wykrywa kolizje z gory
bool AAbb_up(Rectangle tank, Rectangle object, int speed);
/** \brief funkcja wykrywa kolizje z dolu
* @param Rectangle tank - tank
* @param Rectangle object - objekt
* @param speed - predkosc
* @return Funkcja nie zwraca wartosci
*/
// funkcja wykrywa kolizje z dolu
bool AAbb_down(Rectangle tank, Rectangle object, int speed);
/** \brief funkcja wykrywa kolizje z prawej strony
* @param Rectangle tank - czolg
* @param Rectangle object - objekt
* @param speed - predkosc
* @param character
* @return Funkcja nie zwraca wartosci
*/
// funkcja wykrywa kolizje z prawej strony
bool AAbb_right(Rectangle tank, Rectangle object, int speed, int character);
/** \brief funkcja wykrywa kolizje z lewej strony
* @param Rectangle tank - czolg
* @param Rectangle object - objekt
* @param speed - predkosc
* @param character
* @return Funkcja nie zwraca wartosci
*/
// funkcja wykrywa kolizje z lewej strony
bool AAbb_left(Rectangle tank, Rectangle object, int speed, int character);
/** \brief funkcja wykrywa kolizje z granicami
* @param Rectangle *collision_model - model kolizji
* @return Funkcja nie zwraca wartosci
*/
bool checkCollisionWithBorders(Rectangle *collision_model);
/** \brief funkcja wykrywa kolizje ze scianami
* @param Rectangle tank_collision_model - model kolizji czolgu
* @param speed - predkosc
* @param direction - kierunek
* @param ScianaNaMapie *walls - scainy
* @param walls_number - numer sciany
* @return Funkcja nie zwraca wartosci
*/
bool checkCollisionWithWalls(Rectangle tank_collision_model, int speed, int direction, struct ScianaNaMapie *walls, int walls_number);
/** \brief funkcja wykrywa kolizje przeciwnikow z graczem
* @param tank *enemies - czolg wroga
* @param speed - predkosc
* @param direction - kierunek
* @param which_tank - wybor czolgu
* @param Rectangle tank_collision_model - model kolizji czolgu
* @param Rectangle player_collision_model - model kolizji gracza
* @return Funkcja nie zwraca wartosci
*/
bool checkCollisionBetweenTanksAndPlayer(struct tank *enemies, int speed, int direction, int which_tank, Rectangle tank_collision_model, Rectangle player_collision_model);
/** \brief funkcja wykrywa kolizje gracza z przeciwnikami
* @param tank *enemies - czolg przeciwnika
* @param speed - predkosc
* @param direction - kierunek
* @param Rectangle player_collision_model - model kolizji gracza
* @return Funkcja nie zwraca wartosci
*/
bool checkCollisionBetweenPlayerAndOtherTanks(struct tank *enemies, int speed, int direction, Rectangle player_collision_model);
/** \brief funkcja wykrywa kolizje pocisku ze scianami
* @param Rectangle bullet_collision_model - model kolizji pocisku
* @param ScianaNaMapie *walls - sciany
* @param walls_number - numer sciany
* @return Funkcja nie zwraca wartosci
*/
int checkColissionBulletWalls(Rectangle bullet_collision_model, struct ScianaNaMapie *walls, int walls_number);
/** \brief funkcja wykrywa kolizje pocisku z przeciwnikami
* @param Rectangle bullet_collision_model - model kolizji pocisku
* @param tank *enemies - czolg przeciwnika
* @param tank_destroyed_x - wspolrzedna x animacji
* @param tank_destroyed_y - wspolrzedna y animacji
* @param bonus_data *bonuses - informacja o bonusie
* @return Funkcja nie zwraca wartosci
*/
bool checkCollisionBulletEnemies(Rectangle bullet_collision_model, struct tank *enemies, int *tank_destroyed_x, int *tank_destroyed_y, struct bonus_data *bonuses);
/** \brief funkcja wykrywa kolizje pocisku z graczem
* @param Rectangle bullet_collision_model - model kolizji pocisku
* @param tank *player - czolg gracza
* @return Funkcja nie zwraca wartosci
*/
bool checkCollisionBulletPlayer(Rectangle bullet_collision_model, struct tank *player);
/** \brief funkcja wykrywa kolizje bonusu z graczem
* @param tank *player - czolg gracza
* @param bonus_data *bonuses - informacja o bonusie
* @return Funkcja nie zwraca wartosci
*/
bool checkCollisionPlayerBonus(struct tank *player, struct bonus_data *bonuses);

//* colissions koniec

//* main_menu
/** \brief funkcja wczytujaca grafike menu
* @param menu_textures *menu
* @return Funkcja nie zwraca wartosci
*/
void loadMenuTextures(struct menu_textures *menu);
/** \brief funkcja wczytujaca grafike pod koniec gry
* @param end_window_textures *end_window - grafika ekranu koncowego
* @return Funkcja nie zwraca wartosci
*/
void load_end_window(struct end_window_textures *end_window);
/** \brief funkcja usuwajaca grafike menu z pamieci
* @param menu_textures *menu - grafika menu
* @return Funkcja nie zwraca wartosci
*/
void unloadMenuTextures(struct menu_textures *menu);
/** \brief funkcja usuwajaca grafike pod koniec gry z pamieci
* @param end_window_textures *end_window - grafika ekranu koncowego
* @return Funkcja nie zwraca wartosci
*/
void unload_end_window_textures(struct end_window_textures *end_window);
/** \brief funkcja wyswietlajaca animacje w menu
* @param menu_textures *menu - grafika menu
* @return Funkcja nie zwraca wartosci
*/
void drawAnimatedMenu(struct menu_textures *menu);
/** \brief funkcja wyswietlajaca grafike wygrales pod koniec gry
* @param end_window_textures end_window - grafika ekranu koncowego
* @return Funkcja nie zwraca wartosci
*/
bool draw_win_end_window(struct end_window_textures end_window);
/** \brief funkcja wyswietlajaca grafike przegrales pod koniec gry
* @param end_window_textures end_window - grafika ekranu koncowego
* @param tank player - czolg gracza
* @return Funkcja nie zwraca wartosci
*/
bool draw_lose_end_window(struct end_window_textures end_window, struct tank player);

//* main_menu koniec





// * map_menagement
/** \brief funkcja laduje mape z pliku
* @param kwadrat pola_siatki[][NETSIZE_X] - pole siatki
* @param which_map - wybor mapy
* @return Funkcja nie zwraca wartosci
*/
bool loadMapFromFile(struct kwadrat pola_siatki[][NETSIZE_X], int which_map); // funkcja laduje mape z pliku
/** \brief funkcja dobiera textury do mapy
* @param kwadrat pola_siatki[][NETSIZE_X] - pole siatki
* @return Funkcja nie zwraca wartosci
*/
void fillTheMap(struct kwadrat pola_siatki[][NETSIZE_X]); // funkcja dobiera textury do mapy
/** \brief funkcja rysuje mape na ekranie
* @param kwadrat pola_siatki[][NETSIZE_X] - pole siatki
* @return Funkcja nie zwraca wartosci
*/
void drawTheMap(struct kwadrat pola_siatki[][NETSIZE_X]); // funkcja rysuje mape na ekranie
/** \brief usuniecie textur mapy z pamieci RAM
* @param kwadrat pola_siatki[][NETSIZE_X] - pole siatki
* @return Funkcja nie zwraca wartosci
*/
void unloadMap(struct kwadrat pola_siatki[][NETSIZE_X]); // usuniecie textur mapy z pamieci RAM
/** \brief zmiana katow mapy
* @param kwadrat pola_siatki[][NETSIZE_X] - pole siatki
* @param id
* @param kwadrat **pole_siatki - pole siatki
* @return Funkcja nie zwraca wartosci
*/
void replace_map_square(struct kwadrat pola_siatki[][NETSIZE_X], int id, struct kwadrat **pole_siatki);
/** \brief funkcja okresla ile scian jest na mapie
* @param kwadrat pola_siatki[][NETSIZE_X] - pole siatki
* @param walls_number - numer sciany
* @return Funkcja nie zwraca wartosci
*/
int howManyWalls(struct kwadrat pola_siatki[][NETSIZE_X], int walls_number); // funkcja okresla ile scian jest na mapie
/** \brief funkcja okresla granice mapy za ktore czolg nie moze wyjechac
* @param tank *tank - czolg
* @return Funkcja nie zwraca wartosci
*/
// funkcja okresla granice mapy za ktore czolg nie moze wyjechac
void setBorders(struct tank *tank);
/** \brief funkcja rysuje granice mapy na ekranie
* @return Funkcja nie zwraca wartosci
*/
// funkcja rysuje granice mapy na ekranie
void drawTheBorders();

//* map_menagement

//* player_and_enemies
/** \brief funkcja wczytuje textury pozycji czaolgu do tablicy 4 - elementowej, ktora bedzie je przechowywac
* @param tank_direction_textures *player_positions - grafika kierunku czolgu gracza
* @param choice - wybor
* @return Funkcja nie zwraca wartosci
*/
// funkcja wczytuje textury pozycji czaolgu do tablicy 4 - elementowej, ktora bedzie je przechowywac
void load_tank_textures(struct tank_direction_textures *player_positions, int choice);
/** \brief funkcja podmienia tekstury gracza lub przeciwnikow ze wzgledu na kierunek poruszania sie
* @param tank_direction_textures *positions - grafika kierunku czolgu
* @param Texture2D *tank_direction - grafika 2D kierunku czolgu
* @param direction - kierunek
* @return Funkcja nie zwraca wartosci
*/
// funkcja podmienia tekstury gracza lub przeciwnikow ze wzgledu na kierunek poruszania sie
void change_tank_textures(struct tank_direction_textures *positions, Texture2D *tank_direction, int direction);
/** \brief funkcja wczytuje poczatkawa teksture pozycji gracza czolgu
* @param Texture2D start_texture - grafika startowa
* @param Texture2D *tank - grafika czolgu 2D
* @return Funkcja nie zwraca wartosci
*/
// funkcja wczytuje poczatkawa teksture pozycji gracza czolgu
void loadPlayerStartTexture(Texture2D start_texture, Texture2D *tank);
/** \brief inicjacja parametrow gracza
* @param tank_direction_textures *player_positions - grafika kierunku czolgu gracza
* @return Funkcja nie zwraca wartosci
*/
// inicjacja parametrow gracza
void addPlayer(struct tank *player, struct tank_direction_textures *player_positions);
/** \brief inicjacja parametrow przeciwnikow
* @param tank *enemies - czolg gracza
* @param tank_direction_textures *enemy_positions - grafika kierunku czolgu przeciwnika
* @return Funkcja nie zwraca wartosci
*/
// inicjacja parametrow przeciwnikow
void addEnemy(struct tank *enemies, struct tank_direction_textures *enemy_positions);
/** \brief rysowanie przeciwnikow
* @param tank *enemies - czolg przeciwnika
* @return Funkcja nie zwraca wartosci
*/
// rysowanie przeciwnikow
void drawEnemies(struct tank *enemies);
/** \brief zwolnienie tekstur przeciwnikow z pamieci
* @param tank *enemies - czolg przeciwnika
* @return Funkcja nie zwraca wartosci
*/
// zwolnienie tekstur przeciwnikow z pamieci
void unloadEnemyTextures(struct tank *enemies);
/** \brief funkcja odpowiadajaca za ruch przeciwnikow
* @param tank_direction_textures *enemy_positions
* @param tank *enemies - czolg przeciwnika
* @param ScianaNaMapie *walls - sciany
* @param walls_number - numer sciany
* @param Rectangle player
* @return Funkcja nie zwraca wartosci
*/
void enemyMovements(struct tank_direction_textures *enemy_positions, struct tank *enemies, struct ScianaNaMapie *walls, int walls_number, Rectangle player);
/** \brief funkcja odpowiadajaca za wczytywanie bonusow
* @param bonus_textures *bonus_textures - grafika bonusu
* @return Funkcja nie zwraca wartosci
*/
void load_bonus_textures(struct bonus_textures *bonus_textures);
/** \brief funkcja odpowiadajaca za usuwanie z pamieci bonusow
* @param bonus_textures *bonus_textures - grafika bonusu
* @return Funkcja nie zwraca wartosci
*/
void unload_bonus_textures(struct bonus_textures *bonus_textures);
/** \brief funkcja odpowiadajaca za inicjacje bonusow
* @param bonus_data *bonuses - informacja o bonusie
* @return Funkcja nie zwraca wartosci
*/
void initiate_bonuses(struct bonus_data *bonuses);
/** \brief funkcja odpowiadajaca za wyswietlanie bonusow
* @param bonus_textures *bonus_textures - grafika bonusu
* @param bonus_data *bonuses - informacja o bonusie
* @return Funkcja nie zwraca wartosci
*/
void draw_bonuses(struct bonus_textures *bonus_textures, struct bonus_data *bonuses);

//* player_and_enemies koniec

//* shooting

struct bullet
{
    int x, y, direction;
    Rectangle bullet_collision_model;
    bool is_used;
    bool is_sound_played;
};
/** \brief funkcja odpowiadajaca za inicjacje pociskow
* @param bullet *player_bullet - pocisk gracza
* @param tank player - czolg gracza
* @return Funkcja nie zwraca wartosci
*/
void initiate_bullet(struct bullet *player_bullet, struct tank player);
/** \brief funkcja odpowiadajaca za wczytywanie pociskow
* @param Texture2D *bullet_textures - grafika pocisku 2D
* @return Funkcja nie zwraca wartosci
*/
void load_bullet_textures(Texture2D *bullet_textures);
/** \brief funkcja odpowiadajaca za ruch pociskow
* @param bullet *player_bullet - grafika pocisku
* @return Funkcja nie zwraca wartosci
*/
void move_bullet(struct bullet *player_bullet);
/** \brief funkcja odpowiadajaca za wyswietlanie pociskow
* @param bullet *bullet - pocisk
* @param Texture2D *bullet_textures - grafika pocisku 2D
* @return Funkcja nie zwraca wartosci
*/
void drawBullet(struct bullet *bullet, Texture2D *bullet_textures);
/** \brief funkcja odpowiadajaca za usuwanie z pamieci pociskow
* @param Texture2D *bullet_texture - grafika pocisku 2D
* @return Funkcja nie zwraca wartosci
*/
void unload_bullets(Texture2D *bullet_texture);
/** \brief funkcja odpowiadajaca za strzaly przeciwnikow
* @param tank *enemies - czolg przeciwnka
* @param bullet *enemy_bullet - pocisk przeciwinka
* @param animation_data *shoot_animations - animacja strzalu
* @return Funkcja nie zwraca wartosci
*/
void enemyShooting(struct tank *enemies, struct bullet *enemy_bullet);
/** \brief funkcja odpowiadajaca za zarzadzanie pociskami
* @param bullet *player_bullet - pocisk gracza
* @param bullet *enemy_bullet - pocisk przeciwnika
* @param kwadrat pola_siatki[][NETSIZE_X] - pole siatki
* @param ScianaNaMapie *walls - sciany
* @param walls_number - numer sciany
* @param tank *player - czolg gracza
* @param tank *enemies - czolg przeciwnika
* @param animation_data *explosion_animations - informacja o animacji wybuchu
* @param animation_data *shoot_animations - informacja o animacji strzalu
* @param bonus_data *bonuses - informacja o bonusie
* @return Funkcja nie zwraca wartosci
*/
void manage_bullets(struct bullet *player_bullet, struct bullet *enemy_bullet, struct kwadrat pola_siatki[][NETSIZE_X], struct ScianaNaMapie *walls,
                    int walls_number, struct tank *player, struct tank *enemies, struct animation_data *explosion_animations, struct bonus_data *bonuses);

//* shooting

int main(void)
{
    // Inicjacja
    //--------------------------------------------------------------------------------------
    srand(time(NULL));

    InitWindow(screenWidth, screenHeight, "TANK");

    struct kwadrat pola_siatki[NETSIZE_Y][NETSIZE_X];    // tworzenie siatki mapy
    struct ScianaNaMapie *walls;                         // tworzenie tablicy modeli kolizyjnych scian
    struct tank_direction_textures player_positions[4]; // tworzenie struktury przechowujacej textury pozycji czolgu gracza
    struct tank_direction_textures enemy_positions[4];  // tworzenie struktury przechowujacej textury pozycji czolgow przeciwnikow
    struct tank player;                                 // utworzenie struktury przechowujacej informacje o graczu
    struct tank enemies[NUMBER_OF_ENEMIES];             // stworzenie tablicy przechowujacej przeciwnikow
    struct menu_textures menu;
    loadMenuTextures(&menu);
    struct end_window_textures end_window;
    load_end_window(&end_window);
    struct bullet bullet[BULLETS_NUMBER];
    struct bullet enemy_bullet[BULLETS_NUMBER];
    struct animation_data explosion_animations[ANIMATION_ARRAY_SIZE];
    struct explosion_animation_textures explosion_textures;
    struct bonus_textures bonus_textures;
    struct bonus_data bonuses[NUMBER_OF_ENEMIES];
    Texture2D bullet_textures[4]; // tymczasowe przechowanie dla textury pociskow
    Texture2D bottom_pannel;

    int i;
    int frames_counter = 0;

    InitAudioDevice();

    SetTargetFPS(60); // Ustawienie limitu 60 klatek
    //--------------------------------------------------------------------------------------

    // Glowna petla gry
    while (!WindowShouldClose()) // Okno gry bedzie otwarte do czasu nacisniecia klawisza ESC lub przycisku zamykajacego okno
    {
        while (gamemode == MAIN_MENU) // wyswietlanie menu
        {
            if (!WindowShouldClose())
            {
                drawAnimatedMenu(&menu);
            }
            else
            {
                for (i = 0; i < 4; i++)
                {
                    UnloadTexture(player_positions[i].direction);
                    UnloadTexture(enemy_positions[i].direction);
                }

                UnloadTexture(player.tank_direction);
                unloadEnemyTextures(enemies);
                unloadMenuTextures(&menu);
                unloadMap(pola_siatki);
                free(walls);
                UnloadTexture(player.tank_direction);
                unload_bullets(bullet_textures);
                unload_explosion_textures(&explosion_textures);

                unload_bonus_textures(&bonus_textures);
                CloseAudioDevice();
                CloseWindow();

                return 0;
            }
        }

        while (gamemode == GAME)
        {

            if (!is_game_loaded || map_loaded != which_map) // osbluga rozpoczecia gry
            {
                if (!loadMapFromFile(pola_siatki, which_map)) // ladowanie mapy z pliku
                    return 0;
                fillTheMap(pola_siatki);                                   // przypisywanie tekstur do kwadratow mapy
                walls_number = howManyWalls(pola_siatki, walls_number);    // zliczenie ilosci scian na mapie
                walls = calloc(walls_number, sizeof(struct ScianaNaMapie)); // tworzenie tablicy modeli kolizyjnych scian
                if (walls == NULL)
                    return 0;
                createWallsColisionModels(walls, pola_siatki); // tworzenie modeli kolizyjnych scian

                load_tank_textures(player_positions, PLAYER); // zaladowanie do struktury grafik pozycji czolgu gracza
                load_tank_textures(enemy_positions, ENEMY);   // zaladowanie do struktury grafik pozycji czolgu przeciwnikow
                addPlayer(&player, player_positions);         // zaladowanie informacji o graczu
                addEnemy(enemies, enemy_positions);           // poczatkowa inicjacja przeciwnikow

                for (i = 0; i < BULLETS_NUMBER; i++)
                {
                    bullet[i].is_used = false;
                    enemy_bullet[i].is_used = false;
                }
                load_bullet_textures(bullet_textures);

                for (i = 0; i < ANIMATION_ARRAY_SIZE; i++)
                {
                    explosion_animations[i].x = explosion_animations[i].y = 0;
                    explosion_animations[i].is_used = false;
                }
                load_explosion_textures(&explosion_textures);

                load_bonus_textures(&bonus_textures);
                initiate_bonuses(bonuses);

                bottom_pannel = LoadTexture("img/DolnyPanel.png");

                score = 0;
                enemies_alive = NUMBER_OF_ENEMIES;

                map_loaded = which_map;
                is_game_loaded = true;
            }

            if (!WindowShouldClose())
            {
                frames_counter++;

                if (IsKeyPressed(KEY_B))
                    gamemode = MAIN_MENU;

                // Poruszanie sie czolgu po mapie
                if (IsKeyDown(KEY_D) && !checkCollisionWithWalls(player.tank_collision_model, player.speed, RIGHT, walls, walls_number) && !checkCollisionBetweenPlayerAndOtherTanks(enemies, player.speed, RIGHT, player.tank_collision_model)) // ruch w prawo
                {
                    player.direction = RIGHT;
                    player.x = player.tank_collision_model.x += player.speed;
                    if (IsKeyPressed(KEY_D) || (IsKeyReleased(KEY_W) || IsKeyReleased(KEY_S)))
                        change_tank_textures(player_positions, &player.tank_direction, RIGHT);
                }
                if (IsKeyDown(KEY_A) && !checkCollisionWithWalls(player.tank_collision_model, player.speed, LEFT, walls, walls_number) && !checkCollisionBetweenPlayerAndOtherTanks(enemies, player.speed, LEFT, player.tank_collision_model)) // ruch w lewo
                {
                    player.direction = LEFT;
                    player.x = player.tank_collision_model.x -= player.speed;
                    if (IsKeyPressed(KEY_A) || (IsKeyReleased(KEY_W) || IsKeyReleased(KEY_S)))
                        change_tank_textures(player_positions, &player.tank_direction, LEFT);
                }
                if (IsKeyDown(KEY_W) && !checkCollisionWithWalls(player.tank_collision_model, player.speed, UP, walls, walls_number) && !checkCollisionBetweenPlayerAndOtherTanks(enemies, player.speed, UP, player.tank_collision_model)) // ruch w gore
                {
                    player.direction = UP;
                    player.y = player.tank_collision_model.y -= player.speed;
                    if (IsKeyPressed(KEY_W) || (IsKeyReleased(KEY_D) || IsKeyReleased(KEY_A)))
                        change_tank_textures(player_positions, &player.tank_direction, UP);
                }
                if (IsKeyDown(KEY_S) && !checkCollisionWithWalls(player.tank_collision_model, player.speed, DOWN, walls, walls_number) && !checkCollisionBetweenPlayerAndOtherTanks(enemies, player.speed, DOWN, player.tank_collision_model)) // ruch w dol
                {
                    player.direction = DOWN;
                    player.y = player.tank_collision_model.y += player.speed;
                    if (IsKeyPressed(KEY_S) || (IsKeyReleased(KEY_D) || IsKeyReleased(KEY_A)))
                        change_tank_textures(player_positions, &player.tank_direction, DOWN);
                }
                //---------------------------------------------------------------------
                // okreslenie granic mapy za ktore czolg nie moze wyjechac
                setBorders(&player);

                manage_bullets(bullet, enemy_bullet, pola_siatki, walls, walls_number, &player, enemies, explosion_animations, bonuses); // zarzadzanie pociskami
                if (frames_counter % 100 == 0)
                    enemyShooting(enemies, enemy_bullet);

                enemyMovements(enemy_positions, enemies, walls, walls_number, player.tank_collision_model); // poruszanie sie przeciwnikow

                BeginDrawing(); // rysowanie obiektow

                ClearBackground(BLACK);  // nadanie koloru czarnego dla tla
                drawTheMap(pola_siatki); // wywowalnie funkcji rysujacej mape
                drawTheBorders();        // wywolanie funkcji rysujacej granice mapy
                if (player.lives > 0)
                    DrawTextureEx(player.tank_direction, (Vector2){player.x, player.y}, 0, 1, RAYWHITE); // rysowanie textury gracza
                drawEnemies(enemies);
                DrawTexture(bottom_pannel, 0, 200, RAYWHITE);

                drawBullet(bullet, bullet_textures);
                drawBullet(enemy_bullet, bullet_textures);
                DrawFPS(20, 20); // wypisywanie na ekranie wartosci FPS
                checkCollisionPlayerBonus(&player, bonuses);
                draw_bonuses(&bonus_textures, bonuses);

                DrawText(TextFormat("x = %d", player.x), 70, 795, 28, DARKGRAY);  // wypisywanie na ekranie pozycji x czolgu
                DrawText(TextFormat("y = %d", player.y), 270, 795, 28, DARKGRAY); // wypisywanie na ekranie pozycji y czolgu
                DrawText(TextFormat("Screen Size: [%i, %i]", GetScreenWidth(), GetScreenHeight()), 1400, 795, 28, DARKGRAY);
                DrawText(TextFormat("LIVES: %d", player.lives), 70 + 230 + 150 + 50, 795, 28, DARKGRAY);
                DrawText(TextFormat("SCORE: %d", score), 300 + 230 + 150 + 50, 795, 28, DARKGRAY);
                DrawText(TextFormat("ENEMIES: %d", enemies_alive), 300 + 230 + 230 + 150 + 50, 795, 28, DARKGRAY);
                DrawText(TextFormat("B: Menu"), 300 + 230 + 230 + 230 + 150 + 50, 795, 28, DARKGRAY);
                draw_win_end_window(end_window);

                draw_lose_end_window(end_window, player);

                for (i = 0; i < ANIMATION_ARRAY_SIZE; i++)
                    if (explosion_animations[i].is_used)
                    {
                        draw_explosion_animation(explosion_animations, explosion_textures, i);
                    }

                for (i = 0; i < BULLETS_NUMBER; i++)
                    if (bullet[i].is_used == true && bullet[i].is_sound_played == true)
                    {

                        bullet[i].is_sound_played = false;
                    }

                for (i = 0; i < BULLETS_NUMBER; i++)
                    if (enemy_bullet[i].is_used && enemy_bullet[i].is_sound_played == true)
                    {

                        enemy_bullet[i].is_sound_played = false;
                    }

                EndDrawing(); // koniec rysowania obiektow
            }
            else
            {
                for (i = 0; i < 4; i++)
                {
                    UnloadTexture(player_positions[i].direction);
                    UnloadTexture(enemy_positions[i].direction);
                }
                unloadMenuTextures(&menu);
                unloadEnemyTextures(enemies);
                unloadMenuTextures(&menu);
                unloadMap(pola_siatki);
                free(walls);
                UnloadTexture(player.tank_direction);
                unload_bullets(bullet_textures);
                unload_explosion_textures(&explosion_textures);

                unload_bonus_textures(&bonus_textures);
                CloseAudioDevice();
                CloseWindow();
                return 0;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    for (i = 0; i < 4; i++)
    {
        UnloadTexture(player_positions[i].direction);
        UnloadTexture(enemy_positions[i].direction);
    }
    unloadMenuTextures(&menu);
    unloadEnemyTextures(enemies);
    unloadMenuTextures(&menu);
    unloadMap(pola_siatki);
    free(walls);
    UnloadTexture(player.tank_direction);
    unload_bullets(bullet_textures);
    unload_explosion_textures(&explosion_textures);

    unload_bonus_textures(&bonus_textures);
    CloseWindow();
    return 0;
}

// *animations
void load_explosion_textures(struct explosion_animation_textures *explosion_textures)
{
    Image explosion_texture = LoadImage("img/AnimacjaEksplozji.png");

    Image tmp = ImageFromImage(explosion_texture, (Rectangle){0, 0, 50, 50});
    explosion_textures->stage_1 = LoadTextureFromImage(tmp);

    tmp = ImageFromImage(explosion_texture, (Rectangle){50, 0, 50, 50});
    explosion_textures->stage_2 = LoadTextureFromImage(tmp);

    tmp = ImageFromImage(explosion_texture, (Rectangle){100, 0, 50, 50});
    explosion_textures->stage_3 = LoadTextureFromImage(tmp);

    tmp = ImageFromImage(explosion_texture, (Rectangle){150, 0, 50, 50});
    explosion_textures->stage_4 = LoadTextureFromImage(tmp);

    tmp = ImageFromImage(explosion_texture, (Rectangle){200, 0, 50, 50});
    explosion_textures->stage_5 = LoadTextureFromImage(tmp);

    tmp = ImageFromImage(explosion_texture, (Rectangle){250, 0, 50, 50});
    explosion_textures->stage_6 = LoadTextureFromImage(tmp);

    tmp = ImageFromImage(explosion_texture, (Rectangle){300, 0, 50, 50});
    explosion_textures->stage_7 = LoadTextureFromImage(tmp);
}

void unload_explosion_textures(struct explosion_animation_textures *explosion_textures)
{
    UnloadTexture(explosion_textures->stage_1);
    UnloadTexture(explosion_textures->stage_2);
    UnloadTexture(explosion_textures->stage_3);
    UnloadTexture(explosion_textures->stage_4);
    UnloadTexture(explosion_textures->stage_5);
    UnloadTexture(explosion_textures->stage_6);
    UnloadTexture(explosion_textures->stage_7);
}

void draw_explosion_animation(struct animation_data *animation, struct explosion_animation_textures explosion_animation, int index)
{
    animation[index].frames_counter++;
    int frames_limit = 10;

    if (animation[index].frames_counter < frames_limit)
        DrawTexture(explosion_animation.stage_1, animation[index].x, animation[index].y, RAYWHITE);
    else if (animation[index].frames_counter >= frames_limit && animation[index].frames_counter < frames_limit * 2)
        DrawTexture(explosion_animation.stage_2, animation[index].x, animation[index].y, RAYWHITE);
    else if (animation[index].frames_counter >= frames_limit * 2 && animation[index].frames_counter < frames_limit * 3)
        DrawTexture(explosion_animation.stage_3, animation[index].x, animation[index].y, RAYWHITE);
    else if (animation[index].frames_counter >= frames_limit * 3 && animation[index].frames_counter < frames_limit * 4)
        DrawTexture(explosion_animation.stage_4, animation[index].x, animation[index].y, RAYWHITE);
    else if (animation[index].frames_counter >= frames_limit * 4 && animation[index].frames_counter < frames_limit * 5)
        DrawTexture(explosion_animation.stage_5, animation[index].x, animation[index].y, RAYWHITE);
    else if (animation[index].frames_counter >= frames_limit * 5 && animation[index].frames_counter < frames_limit * 6)
        DrawTexture(explosion_animation.stage_6, animation[index].x, animation[index].y, RAYWHITE);
    else if (animation[index].frames_counter >= frames_limit * 6 && animation[index].frames_counter < frames_limit * 7)
        DrawTexture(explosion_animation.stage_7, animation[index].x, animation[index].y, RAYWHITE);
    else
    {
        animation[index].frames_counter = 0;
        animation[index].is_used = false;
    }
}

//*animations koniec

//* collisions
// zakres pociskow
bool checkCollisionWithBorders(Rectangle *collision_model)
{
    if (collision_model->x <= 0 || collision_model->y <= 0 || collision_model->x + collision_model->width >= 1800 ||
        collision_model->y + collision_model->height >= 700)
        return true;
    return false;
}

// funkcja tworzy modele kolizyjne scian
void createWallsColisionModels(struct ScianaNaMapie *walls, struct kwadrat pola_siatki[][NETSIZE_X])
{
    int i, j, licznik = 0;
    for (i = 0; i < NETSIZE_Y; i++)
    {
        for (j = 0; j < NETSIZE_X; j++)
        {
            if (pola_siatki[i][j].type_of_texture == WALL)
            {
                walls[licznik].wall.x = pola_siatki[i][j].x;
                walls[licznik].wall.y = pola_siatki[i][j].y;
                walls[licznik].wall.width = walls[licznik].wall.height = SIZE_OF_SQUARE;
                walls[licznik].id = pola_siatki[i][j].id;
                licznik++;
            }
        }
    }
}

// funkcja rysuje modele kolizyjne scian (funkcja pomocnicza)
void drawWallsColisionModels(struct ScianaNaMapie *walls, int walls_number)
{
    int i;
    for (i = 0; i < walls_number; i++)
        DrawRectangle(walls[i].wall.x, walls[i].wall.y, walls[i].wall.width, walls[i].wall.height, BLUE);
}

// funkcja wykrywa kolizje z gory
bool AAbb_up(Rectangle tank, Rectangle object, int speed)
{

    if (tank.x + PLAYERWIDTH - speed > object.x && tank.x + speed < object.x + object.width && tank.y + PLAYERHEIGHT - speed > object.y && tank.y < object.y + object.height)
        return true;
    else
        return false; // brak kolizji
}

// funkcja wykrywa kolizje z dolu
bool AAbb_down(Rectangle tank, Rectangle object, int speed)
{
    if (tank.x + PLAYERWIDTH - speed > object.x && tank.x + speed < object.x + object.width && tank.y + PLAYERHEIGHT >= object.y && tank.y + speed < object.y + object.height)
        return true;
    else
        return false; // brak kolizji
}

// funkcja wykrywa kolizje z prawej strony
bool AAbb_right(Rectangle tank, Rectangle object, int speed, int character)
{
    if (tank.x + PLAYERWIDTH >= object.x && tank.x + speed * (character + 1) < object.x + object.width && tank.y + PLAYERHEIGHT - speed * (character + 1) > object.y && tank.y + speed * (character + 1) < object.y + object.height)
        return true;

    return false; // brak kolizji
}

// funkcja wykrywa kolizje z lewej strony
bool AAbb_left(Rectangle tank, Rectangle object, int speed, int character)
{
    if (tank.x + PLAYERWIDTH - speed * (character + 1) > object.x && tank.x <= object.x + object.width && tank.y + PLAYERHEIGHT - speed * (character + 1) > object.y && tank.y + speed * (character + 1) < object.y + object.height)
        return true;

    return false; // brak kolizji
}

bool checkCollisionWithWalls(Rectangle tank_collision_model, int speed, int direction, struct ScianaNaMapie *walls, int walls_number)
{
    int i;
    for (i = 0; i < walls_number; i++)
    {
        switch (direction)
        {
        case UP:
            if (AAbb_up(tank_collision_model, walls[i].wall, speed))
                return true;
            break;
        case DOWN:
            if (AAbb_down(tank_collision_model, walls[i].wall, speed))
                return true;
            break;
        case RIGHT:
            if (AAbb_right(tank_collision_model, walls[i].wall, speed, 0))
                return true;
            break;
        case LEFT:
            if (AAbb_left(tank_collision_model, walls[i].wall, speed, 0))
                return true;
            break;
        default:
            break;
        }
    }
    return false;
}

bool checkCollisionBetweenTanksAndPlayer(struct tank *enemies, int speed, int direction, int which_tank, Rectangle tank_collision_model, Rectangle player_collision_model)
{
    int i;
    for (i = 0; i < NUMBER_OF_ENEMIES; i++)
    {
        if (which_tank == i)
            continue;

        switch (direction)
        {
        case UP:
            if (AAbb_up(tank_collision_model, enemies[i].tank_collision_model, speed) || AAbb_up(tank_collision_model, player_collision_model, speed))
                return true;
            break;
        case DOWN:
            if (AAbb_down(tank_collision_model, enemies[i].tank_collision_model, speed) || AAbb_down(tank_collision_model, player_collision_model, speed))
                return true;
            break;
        case RIGHT:
            if (AAbb_right(tank_collision_model, enemies[i].tank_collision_model, speed, 1) || AAbb_right(tank_collision_model, player_collision_model, speed, 1))
                return true;
            break;
        case LEFT:
            if (AAbb_left(tank_collision_model, enemies[i].tank_collision_model, speed, 1) || AAbb_left(tank_collision_model, player_collision_model, speed, 1))
                return true;
            break;
        default:
            break;
        }
    }
    return false;
}

bool checkCollisionBetweenPlayerAndOtherTanks(struct tank *enemies, int speed, int direction, Rectangle player_collision_model)
{
    int i;
    for (i = 0; i < NUMBER_OF_ENEMIES; i++)
    {
        switch (direction)
        {
        case UP:
            if (AAbb_up(player_collision_model, enemies[i].tank_collision_model, speed))
                return true;
            break;
        case DOWN:
            if (AAbb_down(player_collision_model, enemies[i].tank_collision_model, speed))
                return true;
            break;
        case RIGHT:
            if (AAbb_right(player_collision_model, enemies[i].tank_collision_model, speed, 1))
                return true;
            break;
        case LEFT:
            if (AAbb_left(player_collision_model, enemies[i].tank_collision_model, speed, 1))
                return true;
            break;
        default:
            break;
        }
    }
    return false;
}

int checkColissionBulletWalls(Rectangle bullet_collision_model, struct ScianaNaMapie *walls, int walls_number)
{
    int i;
    for (i = 0; i < walls_number; i++)
        if (CheckCollisionRecs(bullet_collision_model, walls[i].wall))
        {
            walls[i].wall.x = walls[i].wall.y = -300;
            return walls[i].id;
        }

    return -1;
}

bool checkCollisionPlayerBonus(struct tank *player, struct bonus_data *bonuses)
{
    int i;
    for (i = 0; i < NUMBER_OF_ENEMIES; i++)
    {
        if (bonuses[i].is_used && CheckCollisionRecs(player->tank_collision_model, bonuses[i].bonus_collision_mode))
        {
            bonuses[i].is_used = false;
            switch (bonuses[i].type_of_bonus)
            {
            case EXTRA_LIFE:
                player->lives += 1;
                break;
            case EXTRA_SPEED:
                player->speed += 1;
                break;
            default:
                break;
            }
            return true;
        }
    }
    return false;
}

bool checkCollisionBulletEnemies(Rectangle bullet_collision_model, struct tank *enemies, int *tank_destroyed_x, int *tank_destroyed_y, struct bonus_data *bonuses)
{
    int i, j;
    for (i = 0; i < NUMBER_OF_ENEMIES; i++)
    {
        if (CheckCollisionRecs(bullet_collision_model, enemies[i].tank_collision_model))
        {
            enemies[i].lives--;
            *tank_destroyed_x = enemies[i].x;
            *tank_destroyed_y = enemies[i].y;
            if (enemies[i].lives <= 0)
            {
                for (j = 0; j < NUMBER_OF_ENEMIES; j++)
                    if (bonuses[j].is_used == false)
                    {
                        bonuses[j].is_used = true;
                        bonuses[j].type_of_bonus = GetRandomValue(0, NUMBER_OF_BONUSES * 2);
                        bonuses[j].bonus_collision_mode.x = enemies[i].x;
                        bonuses[j].bonus_collision_mode.y = enemies[i].y;
                        break;
                    }
                score += 100;
                enemies_alive--;
                enemies[i].tank_collision_model = (Rectangle){-300, -300, -1, -1};
                enemies[i].speed = 0;
                enemies[i].x = -300;
                enemies[i].y = -300;
            }
            return true;
        }
    }
    return false;
}

bool checkCollisionBulletPlayer(Rectangle bullet_collision_model, struct tank *player)
{
    if (CheckCollisionRecs(bullet_collision_model, player->tank_collision_model))
    {
        player->lives--;
        if (player->lives > 0)
        {
            player->tank_collision_model = (Rectangle){150, 300, PLAYERWIDTH, PLAYERHEIGHT};
            player->x = 150;
            player->y = 300;
            return true;
        }
        else
        {
            player->tank_collision_model = (Rectangle){-300, -300, -1, -1};
            player->speed = 0;
            player->x = -300;
            player->y = -300;
        }
        return true;
    }
    return false;
}

//* collisions koniec

//* main_menu

void loadMenuTextures(struct menu_textures *menu)
{
    menu->button_start_off = LoadTexture("img/start_button.png");
    menu->button_start_on = LoadTexture("img/start_button2.png");
    menu->button_map1_off = LoadTexture("img/mapa1off.png");
    menu->button_map1_on = LoadTexture("img/mapa1on.png");
    menu->button_map2_off = LoadTexture("img/mapa2off.png");
    menu->button_map2_on = LoadTexture("img/mapa2on.png");
    menu->button_map3_off = LoadTexture("img/mapa3off.png");
    menu->button_map3_on = LoadTexture("img/mapa3on.png");
    menu->menu_background = LoadTexture("img/MenuTlo.png");
    menu->menu_foreground = LoadTexture("img/MenuTloNachodz.png");

    menu->animation_movement_1 = menu->animation_movement_2 = 0;
}

void load_end_window(struct end_window_textures *end_window)
{
    end_window->you_win_off = LoadTexture("img/you_win_off.png");
    end_window->you_win_on = LoadTexture("img/you_win_on.png");
    end_window->you_lose_off = LoadTexture("img/you_lose_off.png");
    end_window->you_lose_on = LoadTexture("img/you_lose_on.png");
}

void unloadMenuTextures(struct menu_textures *menu)
{
    UnloadTexture(menu->button_start_off);
    UnloadTexture(menu->button_start_on);
    UnloadTexture(menu->button_map1_off);
    UnloadTexture(menu->button_map1_on);
    UnloadTexture(menu->button_map2_off);
    UnloadTexture(menu->button_map2_on);
    UnloadTexture(menu->button_map3_off);
    UnloadTexture(menu->button_map3_on);

    UnloadTexture(menu->menu_background);
    UnloadTexture(menu->menu_foreground);
}

void unload_end_window_textures(struct end_window_textures *end_window)
{
    UnloadTexture(end_window->you_lose_off);
    UnloadTexture(end_window->you_lose_on);
    UnloadTexture(end_window->you_win_off);
    UnloadTexture(end_window->you_win_on);
}

bool draw_win_end_window(struct end_window_textures end_window)
{
    if (enemies_alive <= 0)
    {
        // PlaySound(win_sound);
        if (GetMouseX() >= 650 + 180 && GetMouseX() <= 650 + 180 + 140 && GetMouseY() >= 250 + 105 && GetMouseY() <= 65 + 250 + 105)
        {
            DrawTexture(end_window.you_win_on, 650, 250, RAYWHITE);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                map_loaded = 0;
                gamemode = MAIN_MENU;
            }
        }
        else
            DrawTexture(end_window.you_win_off, 650, 250, RAYWHITE);
        return true;
    }
    return false;
}

bool draw_lose_end_window(struct end_window_textures end_window, struct tank player)
{
    if (player.lives <= 0)
    {
        if (GetMouseX() >= 650 + 180 && GetMouseX() <= 650 + 180 + 140 && GetMouseY() >= 250 + 105 && GetMouseY() <= 65 + 250 + 105)
        {
            DrawTexture(end_window.you_lose_on, 650, 250, RAYWHITE);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                map_loaded = 0;
                gamemode = MAIN_MENU;
            }
        }
        else
            DrawTexture(end_window.you_lose_off, 650, 250, RAYWHITE);
        return true;
    }
    return false;
}

void drawAnimatedMenu(struct menu_textures *menu)
{
    BeginDrawing();
    DrawTexture(menu->menu_background, 0, 0, RAYWHITE);
    DrawTexture(menu->menu_foreground, 0, 200, RAYWHITE);

    if (GetMouseX() >= 40 && GetMouseX() <= 40 + 350 && GetMouseY() >= 40 && GetMouseY() <= 40 + 150)
    {
        DrawTexture(menu->button_start_on, 40, 40, RAYWHITE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            EndDrawing();
            gamemode = GAME;
            return;
        }
    }
    else
        DrawTexture(menu->button_start_off, 40, 40, RAYWHITE);

    if (GetMouseX() >= 470 && GetMouseX() <= 470 + 245 && GetMouseY() >= 70 && GetMouseY() <= 70 + 105)
    {
        DrawTexture(menu->button_map1_on, 470, 70, RAYWHITE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            switch (which_map)
            {
            case 1:
                break;
            default:
                which_map = 1;
                break;
            }
        }
    }
    else if (which_map == 1)
    {
        DrawTexture(menu->button_map1_on, 470, 70, RAYWHITE);
    }
    else
    {
        DrawTexture(menu->button_map1_off, 470, 70, RAYWHITE);
    }

    if (GetMouseX() >= 730 && GetMouseX() <= 730 + 245 && GetMouseY() >= 70 && GetMouseY() <= 70 + 105)
    {
        DrawTexture(menu->button_map2_on, 730, 70, RAYWHITE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            switch (which_map)
            {
            case 2:
                break;
            default:
                which_map = 2;
                break;
            }
        }
    }
    else if (which_map == 2)
    {
        DrawTexture(menu->button_map2_on, 730, 70, RAYWHITE);
    }
    else
    {
        DrawTexture(menu->button_map2_off, 730, 70, RAYWHITE);
    }

    if (GetMouseX() >= 990 && GetMouseX() <= 990 + 245 && GetMouseY() >= 70 && GetMouseY() <= 70 + 105)
    {
        DrawTexture(menu->button_map3_on, 990, 70, RAYWHITE);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            switch (which_map)
            {
            case 3:
                break;
            default:
                which_map = 3;
                break;
            }
        }
    }
    else if (which_map == 3)
    {
        DrawTexture(menu->button_map3_on, 990, 70, RAYWHITE);
    }
    else
    {
        DrawTexture(menu->button_map3_off, 990, 70, RAYWHITE);
    }

    EndDrawing();

    menu->animation_movement_1 += 2;
    menu->animation_movement_2 += 3;
    if (menu->animation_movement_1 >= 1290)
        menu->animation_movement_1 = -400;
    if (menu->animation_movement_2 >= 1700)
        menu->animation_movement_2 = -400;
}

//* main_menu koniec

//* map_menagement

bool loadMapFromFile(struct kwadrat pola_siatki[][NETSIZE_X], int which_map) // funkcja laduje mape z pliku
{
    int i, j;
    char c;
    FILE *map;

    switch (which_map)
    {
    case 1:
        map = fopen("maps/map_1.txt", "r");
        break;
    case 2:
        map = fopen("maps/map_2.txt", "r");
        break;
    case 3:
        map = fopen("maps/map_3.txt", "r");
        break;
    default:
        map = fopen("maps/map_1.txt", "r");
        break;
    }
    if (map != NULL)
    {
        for (i = 0; i < NETSIZE_Y; i++)
            for (j = 0; j < NETSIZE_X; j++)
            {
                c = pola_siatki[i][j].type_of_texture = fgetc(map);
                if (c != EOF)
                {
                    if (c != '\n')
                        pola_siatki[i][j].type_of_texture = c - 48;
                    else
                        j--;
                }
                else
                    pola_siatki[i][j].type_of_texture = EMPTY;
            }
        fclose(map);
        return true;
    }

    return false;
}

// funkcja dobiera textury do mapy
void fillTheMap(struct kwadrat pola_siatki[][NETSIZE_X])
{
    int i, j, k = 0;
    for (i = 0; i < NETSIZE_Y; i++)
    {
        for (j = 0; j < NETSIZE_X; j++)
        {

            switch (pola_siatki[i][j].type_of_texture)
            {
            case EMPTY:
                pola_siatki[i][j].square_texture = LoadTexture("img/puste.png");
                break;
            case SCALE:
                pola_siatki[i][j].square_texture = LoadTexture("img/skala.png");
                break;
            case DIRT:
                pola_siatki[i][j].square_texture = LoadTexture("img/nawierzchnia.png");
                break;
            case WALL:
                pola_siatki[i][j].square_texture = LoadTexture("img/sciana.png");
                break;
            default:
                pola_siatki[i][j].square_texture = LoadTexture("img/puste.png");
                break;
            }
            pola_siatki[i][j].y = SIZE_OF_SQUARE * i;
            pola_siatki[i][j].x = SIZE_OF_SQUARE * j;
            pola_siatki[i][j].id = k;
            k++;
        }
    }
}

// funkcja rysuje mape na ekranie
void drawTheMap(struct kwadrat pola_siatki[][NETSIZE_X])
{
    int i, j;
    for (i = 0; i < NETSIZE_Y; i++)
        for (j = 0; j < NETSIZE_X; j++)
            DrawTexture(pola_siatki[i][j].square_texture, pola_siatki[i][j].x, pola_siatki[i][j].y, RAYWHITE);
}

// funkcja usuwa textury mapy z pamieci RAM
void unloadMap(struct kwadrat pola_siatki[][NETSIZE_X]) // usuniecie textur mapy z pamieci RAM
{
    int i, j;
    for (i = 0; i < NETSIZE_Y; i++)
        for (j = 0; j < NETSIZE_X; j++)
            UnloadTexture(pola_siatki[i][j].square_texture);
}

void replace_map_square(struct kwadrat pola_siatki[][NETSIZE_X], int id, struct kwadrat **pole_siatki)
{
    int i, j;
    for (i = 0; i < NETSIZE_Y; i++)
    {
        for (j = 0; j < NETSIZE_X; j++)
            if (id == pola_siatki[i][j].id)
            {
                pola_siatki[i][j].type_of_texture = 1;
                pola_siatki[i][j].square_texture = LoadTexture("img/skala.png");
                *pole_siatki = &pola_siatki[i][j];
                return;
            }
    }
}

// funkcja okresla ile scian jest na mapie
int howManyWalls(struct kwadrat pola_siatki[][NETSIZE_X], int walls_number)
{
    int i, j;
    for (i = 0; i < NETSIZE_Y; i++)
    {
        for (j = 0; j < NETSIZE_X; j++)
        {
            if (pola_siatki[i][j].type_of_texture == WALL)
                walls_number++;
        }
    }
    return walls_number;
}

// funkcja okresla granice mapy za ktore czolg nie moze wyjechac
void setBorders(struct tank *tank)
{
    if (tank->x > 1800 - PLAYERWIDTH)
        tank->x = tank->tank_collision_model.x = 1800 - PLAYERWIDTH;
    if (tank->y > 700 - PLAYERHEIGHT)
        tank->y = tank->tank_collision_model.y = 700 - PLAYERWIDTH;
    if (tank->x < 0)
        tank->x = tank->tank_collision_model.x = 0;
    if (tank->y < 0)
        tank->y = tank->tank_collision_model.y = 0;
}

// funkcja rysuje granice mapy na ekranie
void drawTheBorders()
{
    DrawRectangle(0, 0, 1800 - 5, 5, DARKGRAY);
    DrawRectangle(0, 700 - 5, 1800, 5, DARKGRAY);
    DrawRectangle(0, 0, 5, 700 - 5, DARKGRAY);
    DrawRectangle(1800 - 5, 0, 5, 700 - 5, DARKGRAY);
    return;
}

//* map_menagement koniec

//* player_end_enemies

// funkcja wczytuje textury pozycji czaolgu do tablicy 4 - elementowej, ktora bedzie je przechowywac
void load_tank_textures(struct tank_direction_textures *player_positions, int choice)
{
    Image temp, tmp;
    switch (choice)
    {
    case PLAYER:
        temp = LoadImage("img/CzolgGracza.png");
        break;
    case ENEMY:
        temp = LoadImage("img/CzolgPrzeciwnika.png");
        break;
    default:
        temp = LoadImage("img/CzolgPrzeciwnika.png");
        break;
    }
    tmp = ImageFromImage(temp, (Rectangle){0, 0, PLAYERWIDTH, PLAYERHEIGHT});
    player_positions[0].direction = LoadTextureFromImage(tmp);
    tmp = ImageFromImage(temp, (Rectangle){45, 0, PLAYERHEIGHT, PLAYERWIDTH});
    player_positions[1].direction = LoadTextureFromImage(tmp);
    tmp = ImageFromImage(temp, (Rectangle){90, 0, PLAYERHEIGHT, PLAYERWIDTH});
    player_positions[2].direction = LoadTextureFromImage(tmp);
    tmp = ImageFromImage(temp, (Rectangle){135, 0, PLAYERWIDTH, PLAYERHEIGHT});
    player_positions[3].direction = LoadTextureFromImage(tmp);

    UnloadImage(temp);
    UnloadImage(tmp);
}

void change_tank_textures(struct tank_direction_textures *positions, Texture2D *tank_direction, int direction)
{

    switch (direction)
    {
    case DOWN:
        *tank_direction = positions[0].direction;
        break;
    case LEFT:
        *tank_direction = positions[1].direction;
        break;
    case RIGHT:
        *tank_direction = positions[2].direction;
        break;
    case UP:
        *tank_direction = positions[3].direction;
        break;
    default:
        *tank_direction = positions[0].direction;
        break;
    }
}

// funkcja wczytuje player_positions czolgu
void loadPlayerStartTexture(Texture2D start_texture, Texture2D *tank)
{
    *tank = start_texture;
}

void addPlayer(struct tank *player, struct tank_direction_textures *player_positions)
{
    player->x = 800;
    player->y = 350;
    player->direction = DOWN;
    player->speed = 3;
    player->movement_counter = GetRandomValue(0, 3);
    player->lives = 1;
    player->tank_collision_model = (Rectangle){player->x, player->y, PLAYERWIDTH, PLAYERHEIGHT};
    player->tank_direction = player_positions[0].direction;
}

void addEnemy(struct tank *enemies, struct tank_direction_textures *enemy_positions)
{
    int i;
    for (i = 0; i < NUMBER_OF_ENEMIES; i++)
    {
        enemies[i].x = 1622;
        enemies[i].y = (i + 2) * 45;
        enemies[i].direction = GetRandomValue(0, 3);
        enemies[i].speed = 2;
        enemies[i].movement_counter = GetRandomValue(0, 5);
        enemies[i].lives = 1;
        enemies[i].tank_collision_model = (Rectangle){enemies[i].x, enemies[i].y, PLAYERWIDTH, PLAYERHEIGHT};
        enemies[i].tank_direction = enemy_positions[0].direction;
    }
}

void unloadEnemyTextures(struct tank *enemies)
{
    int i;
    for (i = 0; i < NUMBER_OF_ENEMIES; i++)
        UnloadTexture(enemies[i].tank_direction);
}

void drawEnemies(struct tank *enemies)
{
    int i;
    for (i = 0; i < NUMBER_OF_ENEMIES; i++)
        if (enemies[i].lives > 0)
            DrawTextureEx(enemies[i].tank_direction, (Vector2){enemies[i].x, enemies[i].y}, 0, 1, RAYWHITE);
}

void enemyMovements(struct tank_direction_textures *enemy_positions, struct tank *enemies, struct ScianaNaMapie *walls, int walls_number, Rectangle player)
{

    int i;
    for (i = 0; i < NUMBER_OF_ENEMIES; i++)
    {
        switch (enemies[i].direction)
        {
        case LEFT:
            if (!checkCollisionWithWalls(enemies[i].tank_collision_model, enemies[i].speed, LEFT, walls, walls_number) &&
                !checkCollisionBetweenTanksAndPlayer(enemies, enemies[i].speed, LEFT, i, enemies[i].tank_collision_model, player) && enemies[i].movement_counter != 30)
            {
                change_tank_textures(enemy_positions, &enemies[i].tank_direction, LEFT);
                enemies[i].tank_collision_model.x -= enemies[i].speed;
                enemies[i].x -= enemies[i].speed;
                enemies[i].movement_counter++;
            }
            else
            {
                enemies[i].direction = LosoweLiczby(0, 4);
                enemies[i].movement_counter = 0;
            }
            break;
        case RIGHT:
            if (!checkCollisionWithWalls(enemies[i].tank_collision_model, enemies[i].speed, RIGHT, walls, walls_number) &&
                !checkCollisionBetweenTanksAndPlayer(enemies, enemies[i].speed, RIGHT, i, enemies[i].tank_collision_model, player) && enemies[i].movement_counter != 30)
            {
                change_tank_textures(enemy_positions, &enemies[i].tank_direction, RIGHT);
                enemies[i].tank_collision_model.x += enemies[i].speed;
                enemies[i].x += enemies[i].speed;
                enemies[i].movement_counter++;
            }
            else
            {
                enemies[i].direction = LosoweLiczby(0, 4);
                enemies[i].movement_counter = 0;
            }
            break;
        case DOWN:
            if (!checkCollisionWithWalls(enemies[i].tank_collision_model, enemies[i].speed, DOWN, walls, walls_number) &&
                !checkCollisionBetweenTanksAndPlayer(enemies, enemies[i].speed, DOWN, i, enemies[i].tank_collision_model, player) && enemies[i].movement_counter != 30)
            {
                change_tank_textures(enemy_positions, &enemies[i].tank_direction, DOWN);
                enemies[i].tank_collision_model.y += enemies[i].speed;
                enemies[i].y += enemies[i].speed;
                enemies[i].movement_counter++;
            }
            else
            {
                enemies[i].direction = LosoweLiczby(0, 4);
                enemies[i].movement_counter = 0;
            }
            break;
        case UP:
            if (!checkCollisionWithWalls(enemies[i].tank_collision_model, enemies[i].speed, UP, walls, walls_number) &&
                !checkCollisionBetweenTanksAndPlayer(enemies, enemies[i].speed, DOWN, i, enemies[i].tank_collision_model, player) && enemies[i].movement_counter != 30)
            {
                change_tank_textures(enemy_positions, &enemies[i].tank_direction, UP);
                enemies[i].tank_collision_model.y -= enemies[i].speed;
                enemies[i].y -= enemies[i].speed;
                enemies[i].movement_counter++;
            }
            else
            {
                enemies[i].direction = LosoweLiczby(0, 4);
                enemies[i].movement_counter = 0;
            }
            break;
        default:
            enemies[i].direction = LosoweLiczby(0, 4);
            enemies[i].movement_counter = 0;
            break;
        }

        if (enemies[i].x > 1795 - PLAYERWIDTH)
        {
            enemies[i].x = enemies[i].tank_collision_model.x = 1795 - PLAYERWIDTH;
            enemies[i].direction = LosoweLiczby(0, 4);
        }
        if (enemies[i].y > 695 - PLAYERHEIGHT)
        {
            enemies[i].y = enemies[i].tank_collision_model.y = 695 - PLAYERWIDTH;
            enemies[i].direction = LosoweLiczby(0, 4);
        }
        if (enemies[i].x < 0)
        {
            enemies[i].x = enemies[i].tank_collision_model.x = 0;
            enemies[i].direction = LosoweLiczby(0, 4);
        }
        if (enemies[i].y < 0)
        {
            enemies[i].y = enemies[i].tank_collision_model.y = 0;
            enemies[i].direction = LosoweLiczby(0, 4);
        }
    }
}

void load_bonus_textures(struct bonus_textures *bonus_textures)
{
    bonus_textures->extra_life_texture = LoadTexture("img/DodatkoweZycie.png");
    bonus_textures->extra_speed_texture = LoadTexture("img/ZwiekszonaPredkosc.png");
}

void unload_bonus_textures(struct bonus_textures *bonus_textures)
{
    UnloadTexture(bonus_textures->extra_life_texture);
    UnloadTexture(bonus_textures->extra_speed_texture);
}

void initiate_bonuses(struct bonus_data *bonuses)
{
    int i;
    for (i = 0; i < NUMBER_OF_ENEMIES; i++)
    {
        bonuses[i].bonus_collision_mode.height = bonuses[i].bonus_collision_mode.width = 30;
        bonuses[i].bonus_collision_mode.x = bonuses[i].bonus_collision_mode.y = -1;
        bonuses[i].is_used = false;
        bonuses[i].type_of_bonus = -1;
    }
}

void draw_bonuses(struct bonus_textures *bonus_textures, struct bonus_data *bonuses)
{
    int i;
    for (i = 0; i < NUMBER_OF_ENEMIES; i++)
        if (bonuses[i].is_used == true)
        {
            switch (bonuses[i].type_of_bonus)
            {
            case EXTRA_LIFE:
                DrawTexture(bonus_textures->extra_life_texture, bonuses[i].bonus_collision_mode.x, bonuses[i].bonus_collision_mode.y, RAYWHITE);
                break;
            case EXTRA_SPEED:
                DrawTexture(bonus_textures->extra_speed_texture, bonuses[i].bonus_collision_mode.x, bonuses[i].bonus_collision_mode.y, RAYWHITE);
                break;
            default:
                break;
            }
        }
}

//* player_end_enemies koniec

//* shooting

void initiate_bullet(struct bullet *bullet, struct tank player)
{
    int i;
    for (i = 0; i < BULLETS_NUMBER; i++)
    {
        if (bullet[i].is_used == false)
        {
            switch (player.direction)
            {
            case UP:
                bullet[i].x = player.x + 18;
                bullet[i].y = player.y - 5;
                break;
            case DOWN:
                bullet[i].x = player.x + 18;
                bullet[i].y = player.y + 35;
                break;
            case LEFT:
                bullet[i].x = player.x - 5;
                bullet[i].y = player.y + 18;
                break;
            case RIGHT:
                bullet[i].x = player.x + 35;
                bullet[i].y = player.y + 18;
                break;
            default:
                bullet->x = player.x;
                bullet->y = player.y;
                break;
            }
            bullet[i].direction = player.direction;
            Rectangle bullet_model = {bullet[i].x, bullet[i].y, 20, 20};
            bullet[i].bullet_collision_model = bullet_model;
            bullet[i].is_used = true;
            bullet[i].is_sound_played = true;
            break;
        }
    }
}

void load_bullet_textures(Texture2D *bullet_textures)
{
    Image temp = LoadImage("img/pociskTestowy.png");

    Image temp_2 = ImageFromImage(temp, (Rectangle){0, 0, 10, 10});
    bullet_textures[0] = LoadTextureFromImage(temp_2);
    temp_2 = ImageFromImage(temp, (Rectangle){10, 0, 10, 10});
    bullet_textures[1] = LoadTextureFromImage(temp_2);
    UnloadImage(temp_2);
    temp_2 = ImageFromImage(temp, (Rectangle){20, 0, 10, 10});
    bullet_textures[2] = LoadTextureFromImage(temp_2);
    temp_2 = ImageFromImage(temp, (Rectangle){30, 0, 10, 10});
    bullet_textures[3] = LoadTextureFromImage(temp_2);

    UnloadImage(temp_2);
    UnloadImage(temp);
}

void move_bullet(struct bullet *bullet)
{
    switch (bullet->direction)
    {
    case LEFT:
        bullet->x -= bullet_speed;
        bullet->bullet_collision_model.x -= bullet_speed;
        break;
    case RIGHT:
        bullet->x += bullet_speed;
        bullet->bullet_collision_model.x += bullet_speed;
        break;
    case UP:
        bullet->y -= bullet_speed;
        bullet->bullet_collision_model.y -= bullet_speed;
        break;
    case DOWN:
        bullet->y += bullet_speed;
        bullet->bullet_collision_model.y += bullet_speed;
        break;
    default:
        break;
    }
}

void drawBullet(struct bullet *bullet, Texture2D *bullet_textures)
{
    // BeginDrawing();
    int i;
    for (i = 0; i < BULLETS_NUMBER; i++)
    {
        if (bullet[i].is_used == true)
        {
            switch (bullet[i].direction)
            {
            case UP:
                DrawTexture(bullet_textures[3], bullet[i].x, bullet[i].y, RAYWHITE);
                break;
            case DOWN:
                DrawTexture(bullet_textures[2], bullet[i].x, bullet[i].y, RAYWHITE);
                break;
            case LEFT:
                DrawTexture(bullet_textures[0], bullet[i].x, bullet[i].y, RAYWHITE);
                break;
            case RIGHT:
                DrawTexture(bullet_textures[1], bullet[i].x, bullet[i].y, RAYWHITE);
                break;
            default:
                break;
            }
        }
    }

    // EndDrawing();
}

void unload_bullets(Texture2D *bullet_texture)
{
    int i;
    for (i = 0; i < 4; i++)
        UnloadTexture(bullet_texture[i]);
}

void enemyShooting(struct tank *enemies, struct bullet *enemy_bullet)
{
    int i;
    for (i = 0; i < NUMBER_OF_ENEMIES; i++)
        if (enemies[i].lives > 0)
        {
            if (GetRandomValue(0, NUMBER_OF_ENEMIES) > NUMBER_OF_ENEMIES / 2)
            {
                initiate_bullet(enemy_bullet, enemies[i]);
            }
        }
}

void manage_bullets(struct bullet *player_bullet, struct bullet *enemy_bullet, struct kwadrat pola_siatki[][NETSIZE_X], struct ScianaNaMapie *walls,
                    int walls_number, struct tank *player, struct tank *enemies, struct animation_data *explosion_animations, struct bonus_data *bonuses)
{
    int i, j, which_wall;
    struct kwadrat *pole_siatki = NULL;
    int tank_destroyed_x = 0;
    int tank_destroyed_y = 0;
    bool is_collision_with_enemy = false;
    if (IsKeyPressed(KEY_ENTER) && player->lives > 0)
    {
        initiate_bullet(player_bullet, *player);
    }

    for (i = 0; i < BULLETS_NUMBER; i++)
    {
        if (player_bullet[i].is_used)
        {
            is_collision_with_enemy = checkCollisionBulletEnemies(player_bullet[i].bullet_collision_model, enemies, &tank_destroyed_x, &tank_destroyed_y, bonuses);
            which_wall = checkColissionBulletWalls(player_bullet[i].bullet_collision_model, walls, walls_number);
            if (which_wall == -1 && !is_collision_with_enemy)
            {
                move_bullet(&player_bullet[i]);
            }
            else
            {
                player_bullet[i].is_used = false;
                player_bullet[i].bullet_collision_model.x = player_bullet[i].bullet_collision_model.y = -100;
                if (which_wall != -1)
                    replace_map_square(pola_siatki, which_wall, &pole_siatki);

                for (j = 0; j < ANIMATION_ARRAY_SIZE; j++)
                    if (explosion_animations[j].is_used == false)
                    {
                        if (pole_siatki != NULL)
                        {
                            explosion_animations[j].is_used = true;
                            explosion_animations[j].x = pole_siatki->x;
                            explosion_animations[j].y = pole_siatki->y;
                            pole_siatki = NULL;
                        }

                        if (is_collision_with_enemy)
                        {
                            explosion_animations[j].is_used = true;
                            explosion_animations[j].x = tank_destroyed_x;
                            explosion_animations[j].y = tank_destroyed_y;
                        }
                    }
            }

            if (checkCollisionWithBorders(&player_bullet[i].bullet_collision_model))
            {
                player_bullet[i].is_used = false;
                player_bullet[i].bullet_collision_model.x = player_bullet[i].bullet_collision_model.y = -100;
            }
        }

        if (enemy_bullet[i].is_used)
        {
            which_wall = checkColissionBulletWalls(enemy_bullet[i].bullet_collision_model, walls, walls_number);
            if (which_wall == -1 && !checkCollisionBulletPlayer(enemy_bullet[i].bullet_collision_model, player))
            {
                move_bullet(&enemy_bullet[i]);
            }
            else
            {
                enemy_bullet[i].is_used = false;
                enemy_bullet[i].bullet_collision_model.x = enemy_bullet[i].bullet_collision_model.y = -100;
                if (which_wall != -1)
                    replace_map_square(pola_siatki, which_wall, &pole_siatki);

                for (j = 0; j < ANIMATION_ARRAY_SIZE; j++)
                    if (explosion_animations[j].is_used == false)
                    {
                        if (pole_siatki != NULL)
                        {
                            explosion_animations[j].is_used = true;
                            explosion_animations[j].x = pole_siatki->x;
                            explosion_animations[j].y = pole_siatki->y;
                            pole_siatki = NULL;
                        }
                        break;
                    }
            }

            if (checkCollisionWithBorders(&enemy_bullet[i].bullet_collision_model))
            {
                enemy_bullet[i].is_used = false;
                enemy_bullet[i].bullet_collision_model.x = enemy_bullet[i].bullet_collision_model.y = -100;
            }
        }
    }
}

//* shooting
