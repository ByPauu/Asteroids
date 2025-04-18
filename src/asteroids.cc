#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define PI 3.14159265
//PLAYER
struct Player{
  esat::Vec2 center;
  esat::Vec2 speed;
  float* mainpoints;
  float* fire;
  float angle;
  int stage;
  int lives;
  int counter;
};
struct Fakeplayer{
  esat::Vec2 center;
  float* points;
};
struct Bullet{
  esat::Vec2 center;
  esat::Vec2 speed;
  bool active;
  int counter;
};
//ASTEROIDS
struct Asteroid{
  esat::Vec2 center;
  esat::Vec2 speed;
  esat::Vec2* points;
  int kNpoints;
  int stage;
  int type;
  int counter;
};
struct Faketeroid{
  esat::Vec3* points;
  int kNpoints;
};
struct UFO{
  esat::Vec2 center;
  esat::Vec2 speed;
  esat::Vec3* basepoints;
  esat::Vec2* points;
  int stage;
  int counter;
};
//GAME MANAGER
struct Strings{
  esat::Vec2 center;
  int number;
  char* string;
};

enum GameState{
  unlogged = 0,
  login,
  registr,
  scoreboard,
  mainmenu,
  fullgame,
  credits,
  error,
};

GameState gamestate = GameState::unlogged;

const int kWindowHeight = 896;
const int kWindowWidth = 1024;
unsigned char fps = 60; //Control de frames por segundo
int frameCounter = 0;
double deltaTime = 0.0, current_time = 0.0, last_time = 0.0;
double bullet_time = 0.0, current_bullet_time = 0.0;

int kNAsteroids = 50;
int AstInGame = 0;
int kNBullets = 5;
int gamecounter = 0;

Strings score;
Strings actualround;

float* bullet_points = NULL;

char* username = NULL;
char* user_password = NULL;
char* auxstring = NULL;
char* auxstring2 = NULL;

//GAME MANAGER
void InitGame(Player** player, Bullet** bullet, Asteroid** asteroid, Faketeroid** faketeroid,
   Fakeplayer** fakeplayer, UFO** ufo, Bullet** ufo_bullet);
void BorderColide(Player* player, Bullet* bullet, Asteroid* asteroid, UFO* ufo);
int CheckPerimeter(float lado1x,float lado1y,float lado2x,float lado2y,float puntox,float puntoy);
void FreeMemory(Player* player, Bullet* bullet, Asteroid* asteroid, Faketeroid* faketeroid,
   Fakeplayer* fakeplayer, UFO* ufo, Bullet* ufo_bullet);
void KYS(void* punterito);

void UpdateDrawScore(Player* player);
void Gameplaygamestate(Player* player, Asteroid* asteroid, Faketeroid* faketeroid, Fakeplayer* fakeplayer);

//PLAYER
void InputPlayer(Player* player, Bullet* bullet);
void PlayerShoot(Player* player, Bullet* bullet);
void UpdatePlayer(Player* player, float posx, float posy);
void DrawPlayer(Player* player,Fakeplayer* fakeplayer);
void PlayerRespawn(Player* player, Fakeplayer* fakeplayer, Asteroid* asteroid);

void PlayerColides(Player* player, Asteroid* asteroid, Faketeroid* faketeroid);
void PlayerDying(Player* player, Fakeplayer* fakeplayer);

//ARROW
void DefineBullet(Player* player, Bullet* bullet);
void UpdateBullet(Bullet* bullet);
void DrawBullet(Bullet* bullet);

//ASTEROIDS
void AsteroidGeneration(Asteroid* asteroid, Faketeroid* faketeroid);
void UpdateAsteroid(Asteroid* asteroid, Faketeroid* faketeroid);
void DrawAsteroid(Asteroid* asteroid);

void AsteroidColides(Asteroid* asteroid,Faketeroid* faketeroid, Bullet* bullet);
bool AsteroidTriangulate(Asteroid* asteroid, int type, float pointx, float pointy);
void AsteroidOblivion(Asteroid* asteroid,Faketeroid* faketeroid, int newstage, float centerx, float centery);
void AsteroidExplosion(Asteroid* asteroid,Faketeroid* faketeroid);

//UFO
void UFOGeneration(UFO* ufo, Bullet* bullet);
void DrawUFO(UFO* ufo);
void UpdateUFO(UFO* ufo);
void UFOAttacks(UFO* ufo, Bullet* ufo_bullet, Player* player);
void UFOColides(UFO* ufo, Bullet* ufo_bullet, Player* player, Bullet* bullet);
//Menu
void MenuAsteroids(Asteroid* asteroid, Faketeroid* faketeroid);
void DrawCalcMenu(Asteroid* asteroid, Faketeroid* faketeroid);
void EndMenu(Asteroid* asteroid);
void ScoreBoard(Asteroid* asteroid, Faketeroid* faketeroid);

void InitMenu();
void UnLoggedMenu();
void LoginMenu();
void RegistrterMenu();


int esat::main(int argc, char **argv) {
  Player* player = NULL;
  Bullet* bullet = NULL;
  Asteroid* asteroid = NULL;

  Faketeroid* faketeroid = NULL;
  Fakeplayer* fakeplayer = NULL;
  UFO* ufo = NULL;
  Bullet* ufo_bullet = NULL;

  bullet_points = (float*) malloc(sizeof(float) * 8);
  KYS(bullet_points);

  username = (char*) calloc(12, sizeof(char));
  KYS(username);
  user_password = (char*) calloc(12, sizeof(char));
  KYS(user_password);
  auxstring = (char*) calloc(12, sizeof(char));
  KYS(auxstring);
  auxstring2 = (char*) calloc(12, sizeof(char));
  KYS(auxstring2);

  InitGame(&player, &bullet, &asteroid, &faketeroid, &fakeplayer, &ufo, &ufo_bullet);
/*.___  ___.      ___       __  .__   __.     _______  __    __  .__   __.   ______ .___________. __    ______   .__   __. 
  |   \/   |     /   \     |  | |  \ |  |    |   ____||  |  |  | |  \ |  |  /      ||           ||  |  /  __  \  |  \ |  | 
  |  \  /  |    /  ^  \    |  | |   \|  |    |  |__   |  |  |  | |   \|  | |  ,----'`---|  |----`|  | |  |  |  | |   \|  | 
  |  |\/|  |   /  /_\  \   |  | |  . `  |    |   __|  |  |  |  | |  . `  | |  |         |  |     |  | |  |  |  | |  . `  | 
  |  |  |  |  /  _____  \  |  | |  |\   |    |  |     |  `--'  | |  |\   | |  `----.    |  |     |  | |  `--'  | |  |\   | 
  |__|  |__| /__/     \__\ |__| |__| \__|    |__|      \______/  |__| \__|  \______|    |__|     |__|  \______/  |__| \__| 
*/
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    //Control time & fps
    do{
      current_time = esat::Time();
      deltaTime = current_time - last_time;
    } while((deltaTime) <= 1000.0 / fps);
    frameCounter = (frameCounter>=fps)?0:frameCounter +1;
    last_time = esat::Time();

    switch(gamestate){
      case GameState::mainmenu:{
        MenuAsteroids(asteroid,faketeroid);
        DrawCalcMenu(asteroid,faketeroid);
        BorderColide(player, bullet, asteroid, ufo);
        break;
      }
      case GameState::unlogged:{
        UnLoggedMenu();
        break;
      }
      case GameState::login:{
        LoginMenu();
        break;
      }
      case GameState::registr:{
        RegistrterMenu();
        break;
      }
      case GameState::scoreboard:{
        
        break;
      }
      case GameState::fullgame:{
        //COLIDES
        AsteroidColides(asteroid, faketeroid, bullet);
        PlayerColides(player,asteroid, faketeroid);
        UFOColides(ufo, ufo_bullet, player, bullet);
        AsteroidExplosion(asteroid, faketeroid);
        //INPUT
        InputPlayer(player, bullet);
        UFOGeneration(ufo, bullet);
        UFOAttacks(ufo, ufo_bullet, player);
        //UPDATES
        UpdateAsteroid(asteroid, faketeroid);
        UpdateBullet(bullet);
        UpdateBullet(ufo_bullet);
        UpdatePlayer(player, player->center.x, player->center.y);
        UpdateUFO(ufo);
        //DRAW BEGIN
        esat::DrawBegin();
        esat::DrawClear(0,0,0);

        esat::DrawSetStrokeColor(255,255,255);
        esat::DrawSetFillColor(255,255,255,0);
        
        DrawAsteroid(asteroid);
        DrawBullet(bullet);
        DrawBullet(ufo_bullet);
        DrawUFO(ufo);
        esat::DrawSetFillColor(0,0,0,255);
        DrawPlayer(player, fakeplayer);

        BorderColide(player, bullet, asteroid, ufo);
        UpdateDrawScore(player);
        esat::DrawEnd();
        //DRAW END

        PlayerRespawn(player, fakeplayer, asteroid);
        Gameplaygamestate(player,asteroid, faketeroid, fakeplayer);
        break;
      }
      case GameState::credits:{
        MenuAsteroids(asteroid,faketeroid);
        ScoreBoard(asteroid,faketeroid);
        BorderColide(player, bullet, asteroid, ufo);
        break;
      }
      case GameState::error:{
        FreeMemory(player, bullet, asteroid, faketeroid, fakeplayer, ufo, ufo_bullet);
        esat::DrawBegin();
        esat::DrawClear(0,0,0);
        esat::DrawEnd();
        esat::WindowDestroy();
        break;
      }
    }
    esat::WindowFrame();
  }

  FreeMemory(player, bullet, asteroid, faketeroid, fakeplayer, ufo, ufo_bullet);
  printf("\nFINALIZADO CORRECTAMENTE\n");
  esat::WindowDestroy();
  return 0;
}

void InitGame(Player** player, Bullet** bullet, Asteroid** asteroid, Faketeroid** faketeroid, Fakeplayer** fakeplayer, UFO** ufo, Bullet** ufo_bullet){
  esat::WindowInit(kWindowWidth,kWindowHeight);
  esat::WindowSetMouseVisibility(true);

  esat::DrawSetTextSize(50);
	esat::DrawSetTextFont("fonts/hyperspace.otf");
  
  srand(time(NULL));

  //STRINGS FROM SYSTEM
  score.string = (char*) malloc(sizeof(char) * 7);
  score.number = 0;
  actualround.string = (char*) malloc(sizeof(char) * 3);
  actualround.number = 0;

  //PLAYER THINGS
  *player = (Player*) malloc(sizeof(Player) * 1);
  KYS(*player);
  (*player)->center.x = kWindowWidth / 2;
  (*player)->center.y = kWindowHeight / 2;
  (*player)->speed.x = 0;
  (*player)->speed.y = 0;
  (*player)->angle = -90 * PI / 180;
  (*player)->lives = 3;
  (*player)->stage = 1;
  (*player)->counter = 0;
  (*player)->mainpoints = (float*) malloc(sizeof(float) * 12); //Adding Slots into array
  (*player)->fire = (float*) malloc(sizeof(float) * 6); //Adding Slots into array
  KYS((*player)->mainpoints);
  KYS((*player)->fire);

  //FAKE PLAYER THINGS
  *fakeplayer = (Fakeplayer*) malloc(sizeof(Fakeplayer) * 3);
  KYS(*fakeplayer);
  int posx = 50, posy = 80;
  for(int i = 0; i < 3; i++){
    (*fakeplayer + i)->points = (float*) malloc(sizeof(float) * 12);
    KYS((*fakeplayer + i)->points);
    //HEAD
    *((*fakeplayer + i)->points + 0) = cosf(((0 * PI) / 180) - 90 * PI / 180) * 18 + posx + i * 30;
    *((*fakeplayer + i)->points + 1) = sinf(((0 * PI) / 180) - 90 * PI / 180) * 18 + posy;
    //SIDES
    *((*fakeplayer + i)->points + 2) = cosf((( 153 * PI) / 180) - 90 * PI / 180) * 25 + posx + i * 30;
    *((*fakeplayer + i)->points + 3) = sinf((( 153 * PI) / 180) - 90 * PI / 180) * 25 + posy;
    *((*fakeplayer + i)->points + 8) = cosf(((-153 * PI) / 180) - 90 * PI / 180) * 25 + posx + i * 30;
    *((*fakeplayer + i)->points + 9) = sinf(((-153 * PI) / 180) - 90 * PI / 180) * 25 + posy;
    //BACK
    *((*fakeplayer + i)->points + 4) = cosf((( 160 * PI) / 180) - 90 * PI / 180) * 18 + posx + i * 30;
    *((*fakeplayer + i)->points + 5) = sinf((( 160 * PI) / 180) - 90 * PI / 180) * 18 + posy;
    *((*fakeplayer + i)->points + 6) = cosf(((-160 * PI) / 180) - 90 * PI / 180) * 18 + posx + i * 30;
    *((*fakeplayer + i)->points + 7) = sinf(((-160 * PI) / 180) - 90 * PI / 180) * 18 + posy;
  }
  //PLAYER BULLET
  *bullet = (Bullet*) malloc(sizeof(Bullet) * kNBullets);
  KYS(*bullet);
  for(int i = 0; i < 8; i++){
    (*bullet + i)->active = false;
    (*bullet + i)->counter = 0;
  }
  //UFO
  *ufo = (UFO*) malloc(sizeof(UFO) * 1);
  KYS(*ufo);
  (*ufo)->center.x = 0;
  (*ufo)->center.y = 0;
  (*ufo)->stage = 0;
  (*ufo)->points = (esat::Vec2*) malloc(sizeof(esat::Vec2) * 8);
  KYS((*ufo)->points);
  (*ufo)->basepoints = (esat::Vec3*) malloc(sizeof(esat::Vec3) * 8);
  KYS((*ufo)->basepoints);
  *((*ufo)->basepoints + 0) = {-15, - 3, 1.0f};
  *((*ufo)->basepoints + 1) = {- 6,   3, 1.0f};
  *((*ufo)->basepoints + 2) = {  7,   3, 1.0f};
  *((*ufo)->basepoints + 3) = { 16, - 3, 1.0f};
  *((*ufo)->basepoints + 4) = {  7, - 9, 1.0f};
  *((*ufo)->basepoints + 5) = {  4, -15, 1.0f};
  *((*ufo)->basepoints + 6) = {- 3, -15, 1.0f};
  *((*ufo)->basepoints + 7) = {- 6, - 9, 1.0f};

  //UFO BULLET
  *ufo_bullet = (Bullet*) malloc(sizeof(Bullet) * kNBullets);
  KYS(*ufo_bullet);
  for(int i = 0; i < kNBullets; i++){
    (*ufo_bullet + i)->active = false;
    (*ufo_bullet + i)->counter = 0;
  }

  //ASTEROIDS
  *asteroid = (Asteroid*) malloc(sizeof(Asteroid) * kNAsteroids);
  KYS(*asteroid);
  for(int i = 0; i < kNAsteroids; i++){
    (*asteroid + i)->points = (esat::Vec2*) malloc(sizeof(esat::Vec2) * 12); 
    KYS((*asteroid + i)->points);
    (*asteroid + i)->stage = 0;
    (*asteroid + i)->counter = 0;
  }

  //FAKETEROIDS
  *faketeroid = (Faketeroid*) malloc(sizeof(Faketeroid) * 4);
  KYS(*faketeroid);

  (*faketeroid + 0)->kNpoints = 12;
  (*faketeroid + 1)->kNpoints = 10;
  (*faketeroid + 2)->kNpoints = 12;
  (*faketeroid + 3)->kNpoints = 11;

  for(int i = 0; i < 4; i++){
    (*faketeroid + i)->points = (esat::Vec3*) malloc(sizeof(esat::Vec3) * (*faketeroid + i)->kNpoints);
    KYS((*faketeroid + i)->points);
  }

  //FIRST faketeroid
  *((*faketeroid + 0)->points + 0) = {-13, -40, 1.0f};
  *((*faketeroid + 0)->points + 1) = {-35, -22, 1.0f};
  *((*faketeroid + 0)->points + 2) = {-28, -2, 1.0f};
  *((*faketeroid + 0)->points + 3) = {-40, 18, 1.0f};
  *((*faketeroid + 0)->points + 4) = {-24, 40, 1.0f};
  *((*faketeroid + 0)->points + 5) = {-12, 33, 1.0f};
  *((*faketeroid + 0)->points + 6) = {14, 45, 1.0f};
  *((*faketeroid + 0)->points + 7) = {37, 19, 1.0f};
  *((*faketeroid + 0)->points + 8) = {22, -4, 1.0f};
  *((*faketeroid + 0)->points + 9) = {41, -12, 1.0f};
  *((*faketeroid + 0)->points + 10) = {25, -34, 1.0f};
  *((*faketeroid + 0)->points + 11) = {5, -27, 1.0f};
  //SECOND faketeroid
  *((*faketeroid + 1)->points + 0) = {-6, -40, 1.0f};
  *((*faketeroid + 1)->points + 1) = {-28, -27, 1.0f};
  *((*faketeroid + 1)->points + 2) = {-40, 11, 1.0f};
  *((*faketeroid + 1)->points + 3) = {-28, 36, 1.0f};
  *((*faketeroid + 1)->points + 4) = {0, 46, 1.0f};
  *((*faketeroid + 1)->points + 5) = {33, 36, 1.0f};
  *((*faketeroid + 1)->points + 6) = {36, 13, 1.0f};
  *((*faketeroid + 1)->points + 7) = {44, -3, 1.0f};
  *((*faketeroid + 1)->points + 8) = {32, -28, 1.0f};
  *((*faketeroid + 1)->points + 9) = {9, -16, 1.0f};
  //THIRD faketeroid
  *((*faketeroid + 2)->points + 0) =  { -6, -40, 1.0f};
  *((*faketeroid + 2)->points + 1) =  {-29, -22, 1.0f};
  *((*faketeroid + 2)->points + 2) =  {-40,  11, 1.0f};
  *((*faketeroid + 2)->points + 3) =  {-17,  30, 1.0f};
  *((*faketeroid + 2)->points + 4) =  {-18,  44, 1.0f};
  *((*faketeroid + 2)->points + 5) =  {  8,  49, 1.0f};
  *((*faketeroid + 2)->points + 6) =  {  8,  13, 1.0f};
  *((*faketeroid + 2)->points + 7) =  { 31,  31, 1.0f};
  *((*faketeroid + 2)->points + 8) =  { 39,  24, 1.0f};
  *((*faketeroid + 2)->points + 9) =  { 39, -12, 1.0f};
  *((*faketeroid + 2)->points + 10) = { 21, -35, 1.0f};
  *((*faketeroid + 2)->points + 11) = { 11, -17, 1.0f};
  //FOURTH faketeroid
  *((*faketeroid + 3)->points + 0) = {-8, -40, 1.0f};
  *((*faketeroid + 3)->points + 1) = {-8, -6, 1.0f};
  *((*faketeroid + 3)->points + 2) = {-26, -28, 1.0f};
  *((*faketeroid + 3)->points + 3) = {-40, -18, 1.0f};
  *((*faketeroid + 3)->points + 4) = {-38, 0, 1.0f};
  *((*faketeroid + 3)->points + 5) = {-24, 36, 1.0f};
  *((*faketeroid + 3)->points + 6) = {-8, 48, 1.0f};
  *((*faketeroid + 3)->points + 7) = {28, 30, 1.0f};
  *((*faketeroid + 3)->points + 8) = {34, -14, 1.0f};
  *((*faketeroid + 3)->points + 9) = {16, -10, 1.0f};
  *((*faketeroid + 3)->points + 10) = {22, -30, 1.0f};

}
/*
.______    __          ___   ____    ____  _______ .______      
|   _  \  |  |        /   \  \   \  /   / |   ____||   _  \     
|  |_)  | |  |       /  ^  \  \   \/   /  |  |__   |  |_)  |    
|   ___/  |  |      /  /_\  \  \_    _/   |   __|  |      /     
|  |      |  `----./  _____  \   |  |     |  |____ |  |\  \----.
| _|      |_______/__/     \__\  |__|     |_______|| _| `._____|
*/
void UpdatePlayer(Player* player, float posx, float posy){
  if(player->stage == 1){
    player->center.x += player->speed.x;
    player->center.y += player->speed.y;

    //HEAD
    *(player->mainpoints + 0) = cosf(((0 * PI) / 180) + player->angle) * 18 + posx;
    *(player->mainpoints + 1) = sinf(((0 * PI) / 180) + player->angle) * 18 + posy;

    //SIDES
    *(player->mainpoints + 2) = cosf((( 153 * PI) / 180) +  player->angle) * 25 + posx;
    *(player->mainpoints + 3) = sinf((( 153 * PI) / 180) + player->angle) * 25 + posy;
    *(player->mainpoints + 8) = cosf(((-153 * PI) / 180) + player->angle) * 25 + posx;
    *(player->mainpoints + 9) = sinf(((-153 * PI) / 180) + player->angle) * 25 + posy;

    //BACK
    *(player->mainpoints + 4) = cosf((( 160 * PI) / 180) + player->angle) * 18 + posx;
    *(player->mainpoints + 5) = sinf((( 160 * PI) / 180) + player->angle) * 18 + posy;
    *(player->mainpoints + 6) = cosf(((-160 * PI) / 180) + player->angle) * 18 + posx;
    *(player->mainpoints + 7) = sinf(((-160 * PI) / 180) + player->angle) * 18 + posy;

    *(player->mainpoints + 10) = player->center.x;
    *(player->mainpoints + 11) = player->center.y;

    if(esat::IsKeyPressed('W')){
      //FIRE
      *(player->fire + 0) = cosf((( 164 * PI) / 180) + player->angle) * 18 + posx;
      *(player->fire + 1) = sinf((( 164 * PI) / 180) + player->angle) * 18 + posy;
      *(player->fire + 2) = cosf((( 180 * PI) / 180) + player->angle) * 25 + posx;
      *(player->fire + 3) = sinf((( 180 * PI) / 180) + player->angle) * 25 + posy;
      *(player->fire + 4) = cosf(((-164 * PI) / 180) + player->angle) * 18 + posx;
      *(player->fire + 5) = sinf(((-164 * PI) / 180) + player->angle) * 18 + posy;
    }
  }
}
void DrawPlayer(Player* player, Fakeplayer* fakeplayer){
  if(player->stage == 1){    
  
    //DRAW SHIP && ROTATION POINT
    esat::DrawSolidPath(player->mainpoints, 5);
  
    //DRAW FIRE
    if(esat::IsKeyPressed('W') && (frameCounter % 2 == 0)){
      esat::DrawSolidPath(player->fire, 3);
    }
  }  

  for(int i = 0; i < player->lives; i++){
    esat::DrawSolidPath((fakeplayer + i)->points, 5);
  }
  PlayerDying(player, fakeplayer);
}
/*
 _____                  _   
|_   _|                | |  
  | | _ __  _ __  _   _| |_ 
  | || '_ \| '_ \| | | | __|
 _| || | | | |_) | |_| | |_ 
 \___/_| |_| .__/ \__,_|\__|
           | |              
           |_|              
*/
void InputPlayer(Player* player, Bullet* bullet){
  if(player->stage == 1){
    if(player->speed.x != 0 && !esat::IsKeyPressed('W')){
      player->speed.x *= 0.965;
    }
    if(player->speed.y != 0 && !esat::IsKeyPressed('W')){
      player->speed.y *= 0.965;
    }

    if(esat::IsKeyPressed('A')){
      player->angle -= 0.065;
    }
    if(esat::IsKeyPressed('D')){
      player->angle += 0.065;
    }

    if(esat::IsKeyPressed('W')){
      if(player->speed.x < 15 && player->speed.x > -15){
        player->speed.x += cosf(player->angle) * 0.1;
      }
      if(player->speed.y < 15 && player->speed.y > -15){
        player->speed.y += sinf(player->angle) * 0.1;
      }
    }
    current_bullet_time = esat::Time();
    if(esat::IsSpecialKeyDown(esat::kSpecialKey_Space) && current_bullet_time - bullet_time > 70){
      PlayerShoot(player, bullet);
      bullet_time = current_bullet_time;
    }
    if(esat::IsKeyDown('G')){
      player->center.x = (rand() % (kWindowWidth - 200)) + 100;
      player->center.y = (rand() % (kWindowHeight - 200)) + 100;
      player->angle = rand() % 3600;
      if(rand() % 20 == 0){
        player->lives--;
        player->stage = 2;
      }
    }
  }
}

void PlayerShoot(Player* player, Bullet* bullet){
  int ArrowCounter = 0;
  while((bullet + ArrowCounter)->active != false && ArrowCounter < kNBullets -1){
    ArrowCounter++;
  }
  if((bullet + ArrowCounter)->active == false){
    (bullet + ArrowCounter)->active = true;
    DefineBullet(player, (bullet + ArrowCounter));
  }
}
void PlayerRespawn(Player* player, Fakeplayer* fakeplayer, Asteroid* asteroid){
  static int lastlive = player->lives;
  if(player->lives != lastlive){
    player->stage = 2;
    player->counter++;
    int asteroid_near = 0;
    for(int i = 0; i < kNAsteroids; i++){
      if((asteroid + i)->stage > 0 && (asteroid + i)->stage < 4){
        float vec1 = (asteroid + i)->center.x - player->center.x;
        float vec2 = (asteroid + i)->center.y - player->center.y;
        if(sqrtf(vec1 * vec1 + vec2 * vec2) < 100){
          asteroid_near++;
        }
      }
    }

    if(player->counter >= 120 && asteroid_near == 0){
      lastlive = player->lives;
      player->counter = 0;
      player->stage = 1;
      player->angle = -90 * PI / 180;
      player->center.x = kWindowWidth >> 1;
      player->center.y = kWindowHeight >> 1;
      player->speed.x = 0;
      player->speed.y = 0;
    }
    
    if(player->lives == 0 && player->stage == 1){
      gamestate = GameState::credits;
    }

    if(player->counter == 1){
      for(int i = 0; i < 10; i++){
        //HEAD
        *((fakeplayer + player->lives)->points + i) = *(player->mainpoints + i);
      }
      *((fakeplayer + player->lives)->points + 10) = *(player->mainpoints + 0);
      *((fakeplayer + player->lives)->points + 11) = *(player->mainpoints + 1);
    }
  }
}
void PlayerColides(Player* player, Asteroid* asteroid, Faketeroid* faketeroid){
  if(player->stage == 1){
    for(int i = 0; i < kNAsteroids; i++){
      for(int e = 0; e < 6; e++){
        if((asteroid + i)->stage > 0 && (asteroid + i)->stage < 4 && (asteroid + i)->stage < 4){
          float vec1 = (asteroid + i)->center.x - *(player->mainpoints + e * 2);
          float vec2 = (asteroid + i)->center.y - *(player->mainpoints + e * 2 + 1);
          float distance = sqrtf(vec1 * vec1 + vec2 * vec2);
          bool near = false;
          bool colide = false;

          switch((asteroid + i)->stage){
            case 1:{
              if(distance <= 50){
                near = true;
              }
              break;
            }
            case 2:{
              if(distance <= 25){
                near = true;
              }
              break;
            }
            case 3:{
              if(distance <= 15){
                near = true;

              }
              break;
            }
          }
          if(near == true){
            colide = AsteroidTriangulate((asteroid + i), (asteroid + i)->type, *(player->mainpoints + e * 2),*(player->mainpoints + e * 2 + 1));
            if(colide == true){
              player->lives--;
              player->stage = 2;
              AstInGame--;
              AsteroidOblivion(asteroid, faketeroid, (asteroid + i)->stage, (asteroid + i)->center.x, (asteroid + i)->center.y);
              (asteroid + i)->stage *= 10;
              (asteroid + i)->counter++;
            }
          } 
        }
      } 
    }
  }
}

void PlayerDying(Player* player, Fakeplayer* fakeplayer){
  if(player->stage == 2 && player->counter < 50){
    esat::DrawLine(*((fakeplayer + player->lives)->points + 0),*((fakeplayer + player->lives)->points + 1),
                  *((fakeplayer + player->lives)->points + 2),*((fakeplayer + player->lives)->points + 3));
    esat::DrawLine(*((fakeplayer + player->lives)->points + 10),*((fakeplayer + player->lives)->points + 11),
                  *((fakeplayer + player->lives)->points + 8),*((fakeplayer + player->lives)->points + 9));
    esat::DrawLine(*((fakeplayer + player->lives)->points + 4),*((fakeplayer + player->lives)->points + 5),
                  *((fakeplayer + player->lives)->points + 6),*((fakeplayer + player->lives)->points + 7)); 
                    
    *(((fakeplayer + player->lives)->points) + 0) += 0.5;
    *(((fakeplayer + player->lives)->points) + 1) += 0.3;
    *(((fakeplayer + player->lives)->points) + 2) += 0.5;
    *(((fakeplayer + player->lives)->points) + 3) += 0.4;

    *(((fakeplayer + player->lives)->points) + 10) -= 0.5;
    *(((fakeplayer + player->lives)->points) + 11) -= 0.4;
    *(((fakeplayer + player->lives)->points) + 8) += 0.3;
    *(((fakeplayer + player->lives)->points) + 9) += 0.2;

    *(((fakeplayer + player->lives)->points) + 4) -= 0.3;
    *(((fakeplayer + player->lives)->points) + 6) += 0.5;
    *(((fakeplayer + player->lives)->points) + 6) -= 0.2;
    *(((fakeplayer + player->lives)->points) + 7) += 0.1;

  }
}
/*
.______    __    __   __       __       _______ .___________.
|   _  \  |  |  |  | |  |     |  |     |   ____||           |
|  |_)  | |  |  |  | |  |     |  |     |  |__   `---|  |----`
|   _  <  |  |  |  | |  |     |  |     |   __|      |  |     
|  |_)  | |  `--'  | |  `----.|  `----.|  |____     |  |     
|______/   \______/  |_______||_______||_______|    |__|                                                                 
*/
void DefineBullet(Player* player, Bullet* bullet){
  bullet->center.x = *(player->mainpoints + 0);
  bullet->center.y = *(player->mainpoints + 1);

  
  bullet->speed.x = cosf(player->angle) * 15;
  bullet->speed.y = sinf(player->angle) * 15;
  bullet->counter = 0;
}

void UpdateBullet(Bullet* bullet){
  for(int i = 0; i < kNBullets; i++){
    if((bullet + i)->active == true){
      (bullet + i)->center.x += (bullet + i)->speed.x;
      (bullet + i)->center.y += (bullet + i)->speed.y;
      (bullet + i)->counter++;
      if((bullet + i)->counter > 60){
        (bullet + i)->active = false;
        (bullet + i)->counter = 0;
      }
    }
  }
}
void DrawBullet(Bullet* bullet){
  for(int i = 0; i < kNBullets; i++){
    if((bullet + i)->active == true){

      *(bullet_points + 0) = (bullet + i)->center.x - 1;
      *(bullet_points + 1) = (bullet + i)->center.y;
      *(bullet_points + 2) = (bullet + i)->center.x;
      *(bullet_points + 3) = (bullet + i)->center.y + 1;
      *(bullet_points + 4) = (bullet + i)->center.x + 1;
      *(bullet_points + 5) = (bullet + i)->center.y;
      *(bullet_points + 6) = (bullet + i)->center.x;
      *(bullet_points + 7) = (bullet + i)->center.y - 1;

      esat::DrawSolidPath(bullet_points, 4);
      esat::DrawSolidPath(bullet_points, 4);
    }
  }
}
/*   ___           _______.___________. _______ .______        ______    __   _______       _______.
    /   \         /       |           ||   ____||   _  \      /  __  \  |  | |       \     /       |
   /  ^  \       |   (----`---|  |----`|  |__   |  |_)  |    |  |  |  | |  | |  .--.  |   |   (----`
  /  /_\  \       \   \       |  |     |   __|  |      /     |  |  |  | |  | |  |  |  |    \   \    
 /  _____  \  .----)   |      |  |     |  |____ |  |\  \----.|  `--'  | |  | |  '--'  |.----)   |   
/__/     \__\ |_______/       |__|     |_______|| _| `._____| \______/  |__| |_______/ |_______/                                                                                                       
*/

void AsteroidGeneration(Asteroid* asteroid, Faketeroid* faketeroid){
  int counter = 0;
  while((asteroid + counter)->stage != 0 && counter < kNAsteroids - 1){
    counter++;
  }
  if((asteroid + counter)->stage == 0){
    AstInGame++;
    (asteroid + counter)->stage = 1;
    (asteroid + counter)->counter = 0;
    switch(rand()%4){
      case 0:{
        (asteroid + counter)->center.x = rand() % kWindowWidth;
        (asteroid + counter)->center.y = -50;

        (asteroid + counter)->speed.x = ((rand()% 200)  + 200) / 100 * ((rand() % 2)* 2 - 1);
        (asteroid + counter)->speed.y = ((rand()% 200)  + 200) / 100;
        break;
      }
      case 1:{
        (asteroid + counter)->center.x = rand() % kWindowWidth;
        (asteroid + counter)->center.y = kWindowHeight + 50;
        
        (asteroid + counter)->speed.x = ((rand()% 200)  + 200) / 100 * ((rand() % 2)* 2 - 1);
        (asteroid + counter)->speed.y = ((rand()% 200)  + 200) / 100 * -1;
        break;
      }
      case 2:{
        (asteroid + counter)->center.x = -50;
        (asteroid + counter)->center.y = rand() % kWindowWidth;
        
        (asteroid + counter)->speed.x = ((rand()% 200)  + 200) / 100;
        (asteroid + counter)->speed.y = ((rand()% 200)  + 200) / 100 * ((rand() % 2)* 2 - 1);
        break;
      }
      case 3:{
        (asteroid + counter)->center.x = kWindowWidth + 50;
        (asteroid + counter)->center.y = rand() % kWindowWidth;
        
        (asteroid + counter)->speed.x = ((rand()% 200)  + 200) / 100 * -1;
        (asteroid + counter)->speed.y = ((rand()% 200)  + 200) / 100 * ((rand() % 2)* 2 - 1);
        break;
      }
    }

    (asteroid + counter)->type = rand() % 4;
    (asteroid + counter)->kNpoints = (faketeroid + (asteroid + counter)->type)->kNpoints;
  }
}

void UpdateAsteroid(Asteroid* asteroid, Faketeroid* faketeroid){
  for(int i = 0; i < kNAsteroids; i++){
    if((asteroid + i)->stage > 0 && (asteroid + i)->stage < 4){
      (asteroid + i)->center.x += (asteroid + i)->speed.x;
      (asteroid + i)->center.y += (asteroid + i)->speed.y;
      esat::Mat3 m = esat::Mat3Identity();
      switch ((asteroid + i)->stage) {
        case 1: {
          m = esat::Mat3Multiply(esat::Mat3Scale(1.0f, 1.0f), m);
          break;
        }
        case 2: {
          m = esat::Mat3Multiply(esat::Mat3Scale(0.5f, 0.5f), m);
          break;
        }
        case 3: {
          m = esat::Mat3Multiply(esat::Mat3Scale(0.30f, 0.30f), m);
          break;
        }
      }
      m = esat::Mat3Multiply(esat::Mat3Rotate(0.0f), m);
      m = esat::Mat3Multiply(esat::Mat3Translate((asteroid + i)->center.x, (asteroid + i)->center.y), m);
      for (int e = 0; e < (asteroid + i)->kNpoints; e++) {
        esat::Vec3 tmp = esat::Mat3TransformVec3(m, *((faketeroid + (asteroid + i)->type)->points + e));
        *((asteroid + i)->points + e) = { tmp.x, tmp.y };
      }
    }else{
      if((asteroid + i)->stage >= 10){
        esat::Mat3 m = esat::Mat3Identity();
        double aumentasion = (asteroid + i)->counter / 10;
        switch ((asteroid + i)->stage) {
          case 10: {
            m = esat::Mat3Multiply(esat::Mat3Scale(aumentasion * 0.8, aumentasion * 0.8), m);
            break;
          }
          case 20: {
            m = esat::Mat3Multiply(esat::Mat3Scale(aumentasion * 0.5, aumentasion * 0.5), m);
            break;
          }
          case 30: {
            m = esat::Mat3Multiply(esat::Mat3Scale(aumentasion * 0.3 ,aumentasion * 0.3), m);
            break;
          }
        }
        m = esat::Mat3Multiply(esat::Mat3Rotate(0.0f), m);
        m = esat::Mat3Multiply(esat::Mat3Translate((asteroid + i)->center.x, (asteroid + i)->center.y), m);
        for (int e = 0; e < (asteroid + i)->kNpoints; e++) {
          esat::Vec3 tmp = esat::Mat3TransformVec3(m, *((faketeroid + (asteroid + i)->type)->points + e));
          *((asteroid + i)->points + e) = { tmp.x, tmp.y };
        }
      }
    }
  }
}

void DrawAsteroid(Asteroid* asteroid){
  for(int i = 0; i < kNAsteroids; i++){
    if((asteroid + i)->stage > 0 && (asteroid + i)->stage < 4){
      esat::DrawSolidPath( &(asteroid + i)->points->x, (asteroid + i)->kNpoints, true);
    }
  }
}

void AsteroidOblivion(Asteroid* asteroid,Faketeroid* faketeroid, int newstage, float centerx, float centery){ 
  for(int k = 0; k < 2; k++){
    int counter = 0;
    while((asteroid + counter)->stage != 0 && counter < kNAsteroids - 1){
      counter++;
    }

    if((asteroid + counter)->stage == 0){
      if(newstage != 3){
        (asteroid + counter)->stage = newstage + 1;
        (asteroid + counter)->counter = 0;
        AstInGame++;
        switch(rand()%4){
          case 0:{
            (asteroid + counter)->speed.x = ((rand()% 200)  + 200) / 100 * ((rand() % 2)* 2 - 1);
            (asteroid + counter)->speed.y = ((rand()% 200)  + 200) / 100;
            break;
          }
          case 1:{
            (asteroid + counter)->speed.x = ((rand()% 200)  + 200) / 100 * ((rand() % 2)* 2 - 1);
            (asteroid + counter)->speed.y = ((rand()% 200)  + 200) / 100 * -1;
            break;
          }
          case 2:{
            (asteroid + counter)->speed.x = ((rand()% 200)  + 200) / 100;
            (asteroid + counter)->speed.y = ((rand()% 200)  + 200) / 100 * ((rand() % 2)* 2 - 1);
            break;
          }
          case 3:{
            (asteroid + counter)->speed.x = ((rand()% 200)  + 200) / 100 * -1;
            (asteroid + counter)->speed.y = ((rand()% 200)  + 200) / 100 * ((rand() % 2)* 2 - 1);
            break;
          }
        }
  
        (asteroid + counter)->center.x = centerx;
        (asteroid + counter)->center.y = centery;
  
        (asteroid + counter)->type = rand() % 4;
        (asteroid + counter)->kNpoints = (faketeroid + (asteroid + counter)->type)->kNpoints;
      }
    }
  }
}
void AsteroidExplosion(Asteroid* asteroid,Faketeroid* faketeroid){
  for(int i = 0; i < kNAsteroids; i++){
    if((asteroid + i)->counter > 0 && (asteroid + i)->stage > 4){
      (asteroid + i)->counter++;
      if((asteroid + i)->counter < 30){
        for(int e = 0; e < (asteroid + i)->kNpoints; e++){
          esat::DrawLine(((asteroid + i)->points + e)->x, ((asteroid + i)->points + e)->y,
                         ((asteroid + i)->points + e)->x + 1,((asteroid + i)->points + e)->y + 1);
        }
      }else{
        (asteroid + i)->counter = 0;
        (asteroid + i)->stage = 0;
      }
    }
  }
}
void AsteroidColides(Asteroid* asteroid,Faketeroid* faketeroid, Bullet* bullet){
  for(int i = 0; i < kNAsteroids; i++){
    for(int e = 0; e < kNBullets; e++){
      if((asteroid + i)->stage > 0 && (asteroid + i)->stage < 4 && (bullet + e)->active != false){
        float distance = sqrtf(((asteroid + i)->center.x - (bullet + e)->center.x) * ((asteroid + i)->center.x - (bullet + e)->center.x) 
                    + ((asteroid + i)->center.y - (bullet + e)->center.y) * ((asteroid + i)->center.y - (bullet + e)->center.y));
        bool near = false;
        switch((asteroid + i)->stage){
          case 1:{
            if(distance <= 50){
              near = true;
            }
            break;
          }
          case 2:{
            if(distance <= 25){
              near = true;
            }
            break;
          }
          case 3:{
            if(distance <= 15){
              near = true;
            }
            break;
          }
        }

        bool colide = false;
        if(near == true){
          colide = AsteroidTriangulate(asteroid + i, (asteroid + i)->type, (bullet + e)->center.x,(bullet + e)->center.y);
          if(colide == true){
            switch((asteroid + i)->stage){
              case 1:{
                AsteroidOblivion(asteroid, faketeroid, (asteroid + i)->stage, (asteroid + i)->center.x, (asteroid + i)->center.y);
                score.number -= 80;
                break;
              }
              case 2:{
                AsteroidOblivion(asteroid, faketeroid, (asteroid + i)->stage, (asteroid + i)->center.x, (asteroid + i)->center.y);
                score.number -= 50;
                break;
              }
            }
            score.number += 100;
            (bullet + e)->active = false;
            (asteroid + i)->stage *= 10;
            (asteroid + i)->counter++;
            AstInGame--;
          }
        } 
      }   
    }
  }
}
/* __    __   _______   ______           ___   .______        ___           _______.___________.    ___      .______       _______  
  |  |  |  | |   ____| /  __  \         /  /   |   _  \      /   \         /       |           |   /   \     |   _  \     |       \ 
  |  |  |  | |  |__   |  |  |  |       /  /    |  |_)  |    /  ^  \       |   (----`---|  |----`  /  ^  \    |  |_)  |    |  .--.  |
  |  |  |  | |   __|  |  |  |  |      /  /     |   _  <    /  /_\  \       \   \       |  |      /  /_\  \   |      /     |  |  |  |
  |  `--'  | |  |     |  `--'  |     /  /      |  |_)  |  /  _____  \  .----)   |      |  |     /  _____  \  |  |\  \----.|  '--'  |
   \______/  |__|      \______/     /__/       |______/  /__/     \__\ |_______/       |__|    /__/     \__\ | _| `._____||_______/ 
*/
void UFOGeneration(UFO* ufo, Bullet* bullet){
  int dead_bullet = 0;
  for(int i = 0; i < kNBullets; i++){
    if((bullet + i)->active != 0){
      dead_bullet++;
    }
  }
  if((AstInGame < 3 && rand() % 450 == 0 && gamecounter == 0)
  || (dead_bullet == 0 && rand() % 1000 == 0 && gamecounter == 0) || esat::IsKeyDown('U')){
    if(ufo->stage == 0){
      ufo->stage = (rand() % 2) + 1;
      ufo->speed.x = (((rand() % 2) * 2) - 1) * ufo->stage * 1.5;
      ufo->speed.y = ((rand() % 3) - 1) * 0.4;
      if(ufo->speed.x > 0){
        ufo->center.x = -50;
      }else{
        ufo->center.x = kWindowWidth + 50;
      }
      ufo->center.y = rand() % (kWindowWidth - 400) + 200;
      
    }
  }
}

void DrawUFO(UFO* ufo){
  if(ufo->stage > 0){
    esat::DrawSolidPath(&ufo->points->x, 8);
    esat::DrawLine((ufo->points + 0)->x,(ufo->points + 0)->y,(ufo->points + 3)->x,(ufo->points + 3)->y);
    esat::DrawLine((ufo->points + 7)->x,(ufo->points + 7)->y,(ufo->points + 4)->x,(ufo->points + 4)->y);
  }
}

void UpdateUFO(UFO* ufo){
  if(ufo->stage > 0){
    //UFO MOVEMENT
    if(rand() % 420 == 0){
      ufo->speed.y = ((rand() % 3) - 1) * 0.3;
    }
    if(rand() % 600 == 0 && ufo->center.x < 800 && ufo->center.x > 300){
      ufo->speed.x = -ufo->speed.x;
    }
    ufo->center.x += ufo->speed.x;
    ufo->center.y += ufo->speed.y;
    esat::Mat3 m = esat::Mat3Identity();
    switch (ufo->stage) {
      case 1: {
        m = esat::Mat3Multiply(esat::Mat3Scale(2.5f, 2.5f), m);
        break;
      }
      case 2: {
        m = esat::Mat3Multiply(esat::Mat3Scale(1.5f, 1.5f), m);
        break;
      }
    }
    m = esat::Mat3Multiply(esat::Mat3Rotate(0.0f), m);
    m = esat::Mat3Multiply(esat::Mat3Translate(ufo->center.x, ufo->center.y), m);
    for (int e = 0; e < 8; e++){
      esat::Vec3 tmp = esat::Mat3TransformVec3(m, *(ufo->basepoints + e));
      *(ufo->points + e) = { tmp.x, tmp.y};
    }
  }
}
void UFOAttacks(UFO* ufo, Bullet* ufo_bullet, Player* player){
  if(ufo->stage > 0){
    ufo->counter++;
    if(rand() % 120 == 0 && ufo->counter >= 100){
      ufo->counter = 0;
      int ArrowCounter = 0;
      while((ufo_bullet + ArrowCounter)->active != false && ArrowCounter < kNBullets -1){
        ArrowCounter++;
      }
      if((ufo_bullet + ArrowCounter)->active == false){
        switch(ufo->stage){
          case 1:{
            (ufo_bullet + ArrowCounter)->center.x = ufo->center.x;
            (ufo_bullet + ArrowCounter)->center.y = ufo->center.y;
    
            
            (ufo_bullet + ArrowCounter)->speed.x = cosf(rand() % 3600) * 15;
            (ufo_bullet + ArrowCounter)->speed.y = sinf(rand() % 3600) * 15;
            (ufo_bullet + ArrowCounter)->counter = 0;
            (ufo_bullet + ArrowCounter)->active = true;
            break;
          }
          case 2:{
            float vec1 = player->center.x - ufo->center.x;
            float vec2 = player->center.y - ufo->center.y;
            float normalice = sqrtf(vec1 * vec1 + vec2 * vec2);
    
            (ufo_bullet + ArrowCounter)->center.x = ufo->center.x;
            (ufo_bullet + ArrowCounter)->center.y = ufo->center.y;
    
            (ufo_bullet + ArrowCounter)->speed.x = vec1 / normalice * 15 + rand() % 1000 * 0.001;
            (ufo_bullet + ArrowCounter)->speed.y = vec2 / normalice * 15 + rand() % 1000 * 0.001;
            (ufo_bullet + ArrowCounter)->counter = 0;
            (ufo_bullet + ArrowCounter)->active = true;
            break;
          }
        }
      }
    }
  }
}
void UFOColides(UFO* ufo, Bullet* ufo_bullet, Player* player, Bullet* bullet){
  if(ufo->stage > 0){
    for(int e = 0; e < kNBullets; e++){
      if(ufo->stage > 0 && (bullet + e)->active != false){
        float distance = sqrtf((ufo->center.x - (bullet + e)->center.x) * (ufo->center.x - (bullet + e)->center.x) 
                    + (ufo->center.y - (bullet + e)->center.y) * (ufo->center.y - (bullet + e)->center.y));
        bool near = false;
        switch(ufo->stage){
          case 1:{
            if(distance <= 50){
              near = true;
            }
            break;
          }
          case 2:{
            if(distance <= 25){
              near = true;
            }
            break;
          }
        }
        if(near == true){
          if( CheckPerimeter((ufo->points + 0)->x, (ufo->points + 0)->y, (ufo->points + 1)->x, (ufo->points + 1)->y, (bullet + e)->center.x, (bullet + e)->center.y) + 
              CheckPerimeter((ufo->points + 1)->x, (ufo->points + 1)->y, (ufo->points + 2)->x, (ufo->points + 2)->y, (bullet + e)->center.x, (bullet + e)->center.y) +
              CheckPerimeter((ufo->points + 2)->x, (ufo->points + 2)->y, (ufo->points + 3)->x, (ufo->points + 3)->y, (bullet + e)->center.x, (bullet + e)->center.y) +
              CheckPerimeter((ufo->points + 3)->x, (ufo->points + 3)->y, (ufo->points + 4)->x, (ufo->points + 4)->y, (bullet + e)->center.x, (bullet + e)->center.y) +
              CheckPerimeter((ufo->points + 4)->x, (ufo->points + 4)->y, (ufo->points + 7)->x, (ufo->points + 7)->y, (bullet + e)->center.x, (bullet + e)->center.y) +
              CheckPerimeter((ufo->points + 7)->x, (ufo->points + 7)->y, (ufo->points + 0)->x, (ufo->points + 0)->y, (bullet + e)->center.x, (bullet + e)->center.y)
              == 6 ||
              CheckPerimeter((ufo->points + 7)->x, (ufo->points + 7)->y, (ufo->points + 4)->x, (ufo->points + 4)->y, (bullet + e)->center.x, (bullet + e)->center.y) + 
              CheckPerimeter((ufo->points + 4)->x, (ufo->points + 4)->y, (ufo->points + 5)->x, (ufo->points + 5)->y, (bullet + e)->center.x, (bullet + e)->center.y) +
              CheckPerimeter((ufo->points + 5)->x, (ufo->points + 5)->y, (ufo->points + 6)->x, (ufo->points + 6)->y, (bullet + e)->center.x, (bullet + e)->center.y) +
              CheckPerimeter((ufo->points + 6)->x, (ufo->points + 6)->y, (ufo->points + 7)->x, (ufo->points + 7)->y, (bullet + e)->center.x, (bullet + e)->center.y)
              == 4)
          {
            switch(ufo->stage){
              case 1:{
                score.number += 200;
                ufo->stage = 0;
                (bullet + e)->active = false;
              }
              case 2:{
                score.number += 200;
                ufo->stage = 0;
                (bullet + e)->active = false;
              }
            }
          }
        }
      }
    }
  }
  if(ufo->stage > 0){
    for(int e = 0; e < kNBullets; e++){
      if((ufo_bullet + e)->active == true){
        float distance = sqrtf((player->center.x - (ufo_bullet + e)->center.x) * (player->center.x - (ufo_bullet + e)->center.x) 
                    + (player->center.y - (ufo_bullet + e)->center.y) * (player->center.y - (ufo_bullet + e)->center.y));
        if(distance <= 50){
          if( CheckPerimeter(*(player->mainpoints + 2), *(player->mainpoints + 3), *(player->mainpoints + 0), *(player->mainpoints + 1), (ufo_bullet + e)->center.x, (ufo_bullet + e)->center.y) + 
              CheckPerimeter(*(player->mainpoints + 8), *(player->mainpoints + 9), *(player->mainpoints + 2), *(player->mainpoints + 3), (ufo_bullet + e)->center.x, (ufo_bullet + e)->center.y) +
              CheckPerimeter(*(player->mainpoints + 0), *(player->mainpoints + 1), *(player->mainpoints + 8), *(player->mainpoints + 9), (ufo_bullet + e)->center.x, (ufo_bullet + e)->center.y)
              == 3){ 
                player->lives--;
                player->stage = 2;
                (ufo_bullet + e)->active = false;
          }
        }
      }
    }
  }
}
 /* .______     ______   .______       _______   _______ .______      
    |   _  \   /  __  \  |   _  \     |       \ |   ____||   _  \     
    |  |_)  | |  |  |  | |  |_)  |    |  .--.  ||  |__   |  |_)  |    
    |   _  <  |  |  |  | |      /     |  |  |  ||   __|  |      /     
    |  |_)  | |  `--'  | |  |\  \----.|  '--'  ||  |____ |  |\  \----.
    |______/   \______/  | _| `._____||_______/ |_______|| _| `._____|
*/
void BorderColide(Player* player, Bullet* bullet, Asteroid* asteroid, UFO* ufo){
  if(player->center.x < 30){
    //UpdatePlayer(player, player->center.x + kWindowWidth, player->center.y);
    //DrawPlayer(player, player->center.x + kWindowWidth, player->center.y);
    if(player->center.x < 0){
      player->center.x = kWindowWidth;
    }
  }
  if(player->center.x > kWindowWidth - 30){
    //UpdatePlayer(player, player->center.x - kWindowWidth, player->center.y);
    //DrawPlayer(player, player->center.x - kWindowWidth, player->center.y);
    if(player->center.x > kWindowWidth){
      player->center.x = 0;
    }
  }
  if(player->center.y < 30){
    //UpdatePlayer(player, player->center.x, player->center.y + kWindowHeight);
    //DrawPlayer(player, player->center.x, player->center.y + kWindowHeight);
    if(player->center.y < 0){
      player->center.y = kWindowHeight;
    }
  }
  if(player->center.y > kWindowHeight - 30){
    //UpdatePlayer(player, player->center.x, player->center.y - kWindowHeight);
    //DrawPlayer(player, player->center.x, player->center.y - kWindowHeight);
    if(player->center.y > kWindowHeight){
      player->center.y = 0;
    }
  }
  for(int i = 0; i < kNBullets; i++){
    if((bullet + i)->center.x > kWindowWidth){
      (bullet + i)->center.x = 0;
    }
    if((bullet + i)->center.x < 0){
      (bullet + i)->center.x = kWindowWidth;
    }
    if((bullet + i)->center.y > kWindowHeight){
      (bullet + i)->center.y = 0;
    }
    if((bullet + i)->center.y < 0){
      (bullet + i)->center.y = kWindowHeight;
    }
  }
  
  for(int i = 0; i < kNAsteroids; i++){
    if((asteroid + i)->stage > 0){
      if((asteroid + i)->center.x > kWindowWidth + 60){
        (asteroid + i)->center.x = (asteroid + i)->center.x - kWindowWidth - 30;
      }else if((asteroid + i)->center.x < -60){
        (asteroid + i)->center.x = (asteroid + i)->center.x + kWindowWidth + 30;
      }else if((asteroid + i)->center.y > kWindowHeight + 60){
        (asteroid + i)->center.y = (asteroid + i)->center.y - kWindowHeight - 30;
      }else if((asteroid + i)->center.y < - 60){
        (asteroid + i)->center.y = (asteroid + i)->center.y + kWindowHeight + 30;
      }
    }
  }
  if(ufo->center.x < -50){
    ufo->stage = 0;
  }
  if(ufo->center.x > kWindowWidth + 50){
    ufo->stage = 0;
  }
  if(ufo->center.y < -50){
    ufo->stage = 0;
  }
  if(ufo->center.y > kWindowHeight + 50){
    ufo->stage = 0;
  }
}
bool AsteroidTriangulate(Asteroid* asteroid, int type, float pointx, float pointy){
  switch (type){
    case 0:{
      if((CheckPerimeter(((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, ((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, ((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, ((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, ((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, ((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, ((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, ((asteroid)->points + 11)->x,((asteroid)->points + 11)->y,pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 11)->x,((asteroid)->points + 11)->y,((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, pointx, pointy)
        ) == 8
        ||
        ( CheckPerimeter(((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, ((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, ((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, ((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, ((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, pointx, pointy)
        ) == 4
        || 
        ( CheckPerimeter(((asteroid)->points + 11)->x,((asteroid)->points + 11)->y,((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, ((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, ((asteroid)->points + 10)->x,((asteroid)->points + 10)->y,pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 10)->x,((asteroid)->points + 10)->y,((asteroid)->points + 11)->x,((asteroid)->points + 11)->y,pointx, pointy)
        ) == 4){
        return true;
      }else{
        return false;
      }
      break;
    }
    case 1:{
      if((CheckPerimeter(((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, ((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, ((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, ((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, ((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, ((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, pointx, pointy)
        ) == 5
        ||
        ( CheckPerimeter(((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, ((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, ((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, ((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, ((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, ((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, pointx, pointy)
        ) == 5
        ||
        ( CheckPerimeter(((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, ((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, ((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, ((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, ((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, pointx, pointy)
        ) == 4){
        return true;
      }else{
        return false;
      }
      break;
    }
    case 2:{
      if((CheckPerimeter(((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, ((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, ((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, ((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, ((asteroid)->points + 11)->x, ((asteroid)->points + 11)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 11)->x, ((asteroid)->points + 11)->y, ((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, pointx, pointy)
        ) == 5
        ||
        ( CheckPerimeter(((asteroid)->points + 11)->x, ((asteroid)->points + 11)->y, ((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, ((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, ((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, ((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, ((asteroid)->points + 11)->x, ((asteroid)->points + 11)->y, pointx, pointy)
        ) == 5
        ||
        ( CheckPerimeter(((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, ((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, ((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, ((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, ((asteroid)->points + 10)->x, ((asteroid)->points + 10)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 10)->x, ((asteroid)->points + 10)->y, ((asteroid)->points + 11)->x, ((asteroid)->points + 11)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 11)->x, ((asteroid)->points + 11)->y, ((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, pointx, pointy)
        ) == 6){
        return true;
      }else{
        return false;
      }
      break;
    }
    case 3:{
      if((CheckPerimeter(((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, ((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, ((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, ((asteroid)->points + 10)->x, ((asteroid)->points + 10)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 10)->x, ((asteroid)->points + 10)->y, ((asteroid)->points + 0)->x, ((asteroid)->points + 0)->y, pointx, pointy)
        ) == 4
        ||
        ( CheckPerimeter(((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, ((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 2)->x, ((asteroid)->points + 2)->y, ((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 3)->x, ((asteroid)->points + 3)->y, ((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 4)->x, ((asteroid)->points + 4)->y, ((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 5)->x, ((asteroid)->points + 5)->y, ((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, ((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, pointx, pointy)
        ) == 6
        ||
        ( CheckPerimeter(((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, ((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 6)->x, ((asteroid)->points + 6)->y, ((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 7)->x, ((asteroid)->points + 7)->y, ((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 8)->x, ((asteroid)->points + 8)->y, ((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, pointx, pointy)
        + CheckPerimeter(((asteroid)->points + 9)->x, ((asteroid)->points + 9)->y, ((asteroid)->points + 1)->x, ((asteroid)->points + 1)->y, pointx, pointy)
        ) == 5){
        return true;
      }else{
        return false;
      }
      break;
    }
    default:{
      return false;
      break;
    }
  }
}
/*   _______.  ______   ______   .______       _______ 
    /       | /      | /  __  \  |   _  \     |   ____|
   |   (----`|  ,----'|  |  |  | |  |_)  |    |  |__   
    \   \    |  |     |  |  |  | |      /     |   __|  
.----)   |   |  `----.|  `--'  | |  |\  \----.|  |____ 
|_______/     \______| \______/  | _| `._____||_______|
 */
void UpdateDrawScore(Player* player){
  snprintf(score.string, sizeof(score.string) * 5, "%04d", score.number);
  esat::DrawSetFillColor(255,255,255);
  esat::DrawText(20,50 ,score.string);
  if(esat::IsKeyPressed('O')){
    score.number += 1000;
  }

  if(gamecounter != 0){
    snprintf(actualround.string,sizeof(actualround.string), "%02d", actualround.number + 1);
    esat::DrawSetFillColor(gamecounter << 1,gamecounter << 1,gamecounter << 1);
    esat::DrawText(350,300 ,"Round");
    esat::DrawText(550,300 ,actualround.string);
    esat::DrawSetFillColor(255,255,255);
  }
  if(player->lives == 0){
    esat::DrawSetFillColor(player->counter << 1,player->counter << 1,player->counter << 1);
    esat::DrawText(350,300 ,"Game Over");
    esat::DrawSetFillColor(255,255,255);
  }

  char frametime[10];
  snprintf(frametime ,sizeof(frametime), "%.02f", deltaTime);
  esat::DrawText(170,50 ,frametime);

  char numasteroid[10];
  snprintf(numasteroid ,sizeof(numasteroid), "%d", AstInGame);
  esat::DrawText(370,50 ,numasteroid);
}
/* _______ .______       _______  _______    .___  ___.  _______ .___  ___.   ______   .______     ____    ____ 
  |   ____||   _  \     |   ____||   ____|   |   \/   | |   ____||   \/   |  /  __  \  |   _  \    \   \  /   / 
  |  |__   |  |_)  |    |  |__   |  |__      |  \  /  | |  |__   |  \  /  | |  |  |  | |  |_)  |    \   \/   /  
  |   __|  |      /     |   __|  |   __|     |  |\/|  | |   __|  |  |\/|  | |  |  |  | |      /      \_    _/   
  |  |     |  |\  \----.|  |____ |  |____    |  |  |  | |  |____ |  |  |  | |  `--'  | |  |\  \----.   |  |     
  |__|     | _| `._____||_______||_______|   |__|  |__| |_______||__|  |__|  \______/  | _| `._____|   |__|     
*/
void FreeMemory(Player* player, Bullet* bullet, Asteroid* asteroid, Faketeroid* faketeroid, Fakeplayer* fakeplayer, UFO* ufo, Bullet* ufo_bullet){
  // PLAYER
  if (player != NULL) {
    if (player->mainpoints != NULL) free(player->mainpoints);
    if (player->fire != NULL) free(player->fire);
    free(player);
  }

  // FAKEPLAYER
  if (fakeplayer != NULL) {
    for (int i = 0; i < 3; i++) {
      if ((fakeplayer + i)->points != NULL)
        free((fakeplayer + i)->points);
    }
    free(fakeplayer);
  }

  // BULLET
  if (bullet != NULL) free(bullet);
  if (bullet_points != NULL) free(bullet_points);

  // ASTEROID
  if (asteroid != NULL) {
    for (int i = 0; i < kNAsteroids; i++) {
      if ((asteroid + i)->points != NULL)
        free((asteroid + i)->points);
    }
    free(asteroid);
  }

  // FAKETEROID
  if (faketeroid != NULL) {
    for (int i = 0; i < 4; i++) {
      if ((faketeroid + i)->points != NULL)
        free((faketeroid + i)->points);
    }
    free(faketeroid);
  }

  // UFO
  if (ufo != NULL) {
    if (ufo->points != NULL) free(ufo->points);
    if (ufo->basepoints != NULL) free(ufo->basepoints);
    free(ufo);
  }
  if (ufo_bullet != NULL) free(ufo_bullet);

  // STRINGS
  if (score.string != NULL) free(score.string);
  if (actualround.string != NULL) free(actualround.string);

  if (username != NULL) free(username);
  if (user_password != NULL) free(user_password);
  if (auxstring != NULL) free(auxstring);
  if (auxstring2 != NULL) free(auxstring2);
}


void KYS(void* punterito) {
  if(punterito == NULL) {
    // 1. Registrar el error de manera más informativa
    fprintf(stderr, "\nERROR CRÍTICO: Puntero nulo en %s, línea %d", __FILE__, __LINE__);
    gamestate = GameState::error;
    
  
  }
}

int CheckPerimeter(float lado1x, float lado1y, float lado2x, float lado2y, float puntox, float puntoy){
  if(((puntox - lado1x) * -(lado1y - lado2y) + (puntoy - lado1y) * (lado1x - lado2x)) < 0){
    return 0;
  }else{
    return 1;
  }
}

/*_______      ___      .___  ___.  _______    .___  ___.      ___      .__   __.      ___       _______  _______ .______      
 /  _____|    /   \     |   \/   | |   ____|   |   \/   |     /   \     |  \ |  |     /   \     /  _____||   ____||   _  \     
|  |  __     /  ^  \    |  \  /  | |  |__      |  \  /  |    /  ^  \    |   \|  |    /  ^  \   |  |  __  |  |__   |  |_)  |    
|  | |_ |   /  /_\  \   |  |\/|  | |   __|     |  |\/|  |   /  /_\  \   |  . `  |   /  /_\  \  |  | |_ | |   __|  |      /     
|  |__| |  /  _____  \  |  |  |  | |  |____    |  |  |  |  /  _____  \  |  |\   |  /  _____  \ |  |__| | |  |____ |  |\  \----.
 \______| /__/     \__\ |__|  |__| |_______|   |__|  |__| /__/     \__\ |__| \__| /__/     \__\ \______| |_______|| _| `._____|
*/
 void Gameplaygamestate(Player* player, Asteroid* asteroid, Faketeroid* faketeroid, Fakeplayer* fakeplayer){
  if(AstInGame == 0 && gamecounter >= 120){
      actualround.number++;
      int fakeround = actualround.number;
      if(actualround.number > 6){
        fakeround = 6;
      }
      int generation = 0;
      FILE *pfile = fopen("rounds.txt", "r"); // Abre el archivo
      if (pfile != NULL) {
        int roundd = 0;
        while(roundd != fakeround){
          fscanf(pfile, "%d %d", &roundd, &generation);
        }
        fclose(pfile);
      }
      
      for(int i = 0; i < generation; i++){
        AsteroidGeneration(asteroid, faketeroid);
      }
    gamecounter = 0;
  }else{
    if(AstInGame == 0){
      gamecounter++;
    }
  }
  if(esat::IsKeyDown('P')){
    for(int i = 0; i < kNAsteroids; i++){
      (asteroid + i)->stage = 0;
      AstInGame = 0;
    }
  }
  if(esat::IsKeyDown('L')){
    player->lives--;
  }
  if(esat::IsKeyDown('M')){
    gamestate = GameState::credits;
  }
}
/*.___  ___.      ___       __  .__   __.    .___  ___.  _______ .__   __.  __    __  
|   \/   |     /   \     |  | |  \ |  |    |   \/   | |   ____||  \ |  | |  |  |  | 
|  \  /  |    /  ^  \    |  | |   \|  |    |  \  /  | |  |__   |   \|  | |  |  |  | 
|  |\/|  |   /  /_\  \   |  | |  . `  |    |  |\/|  | |   __|  |  . `  | |  |  |  | 
|  |  |  |  /  _____  \  |  | |  |\   |    |  |  |  | |  |____ |  |\   | |  `--'  | 
|__|  |__| /__/     \__\ |__| |__| \__|    |__|  |__| |_______||__| \__|  \______/  
*/
void MenuAsteroids(Asteroid* asteroid, Faketeroid* faketeroid){
  if(AstInGame == 0){
    for(int i = 0; i < 4; i++){
      AsteroidGeneration(asteroid, faketeroid);
      AsteroidOblivion(asteroid, faketeroid, 1, rand() % 200 * i, rand() % 150 * i);
      AsteroidOblivion(asteroid, faketeroid, 2, rand() % 200 * i, rand() % 150 * i);
    }
  }
}
void DrawCalcMenu(Asteroid* asteroid, Faketeroid* faketeroid){
  UpdateAsteroid(asteroid, faketeroid);

  esat::DrawBegin();
  esat::DrawClear(0,0,0);
  esat::DrawSetStrokeColor(255,255,255);
  esat::DrawSetFillColor(255,255,255,0);
  DrawAsteroid(asteroid);

  
  esat::DrawSetFillColor(255,255,255,255);
  esat::DrawSetTextSize(90);
  esat::DrawText(270, 120, "ASTEROIDS");  

  esat::DrawSetTextSize(50);
  esat::DrawText(290, 750, "1 COIN 1     1");
  if((esat::MousePositionX() > 545 && esat::MousePositionX() < 660 && esat::MousePositionY() > 705 && esat::MousePositionY() < 758) || esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
    esat::DrawSetTextSize(55);
    if(esat::MouseButtonDown(0) || esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
      EndMenu(asteroid);
    }
  }else{
    esat::DrawSetTextSize(50);
  }

  esat::DrawText(550, 750, "PLAY");
  esat::DrawSetTextSize(20);
  esat::DrawText(385, 870, "ASTEROIDS BY PAUU");  
  esat::DrawSetTextSize(50);
  esat::DrawEnd();
}

void EndMenu(Asteroid* asteroid){
  for(int i = 0; i < kNAsteroids; i++){
    if((asteroid + i)->stage > 0){
      (asteroid + i)->stage = 0;
    }
  }
  gamestate = GameState::fullgame;
  AstInGame = 0;
}

void ScoreBoard(Asteroid* asteroid, Faketeroid* faketeroid){

  UpdateAsteroid(asteroid, faketeroid);

  esat::DrawBegin();
  esat::DrawClear(0,0,0);
  esat::DrawSetFillColor(255,255,255,0);
  DrawAsteroid(asteroid);
  
  esat::DrawSetStrokeColor(255,255,255);
  esat::DrawSetFillColor(255,255,255,255);
  esat::DrawSetTextSize(90);
  esat::DrawText(270, 120, "ASTEROIDS"); 

  esat::DrawSetTextSize(50);
  esat::DrawText(450, 430, username);
  esat::DrawText(240, 500, "Your Score Is: ");
  esat::DrawText(680, 500,score.string);

  esat::DrawSetTextSize(20);
  esat::DrawText(385, 870, "ASTEROIDS BY PAUU");
  if(esat::IsKeyDown('M')){
    score.number = 10000;
    snprintf(score.string,sizeof(score.string), "%04d", score.number);
  }
  
  esat::DrawEnd();
}
void UnLoggedMenu(){
  esat::DrawBegin();
  esat::DrawClear(0,0,0);
  esat::DrawSetStrokeColor(255,255,255);
  esat::DrawSetFillColor(255,255,255,0);

  
  esat::DrawSetFillColor(255,255,255,255);
  esat::DrawSetTextSize(90);
  esat::DrawText(270, 120, "ASTEROIDS");  
  //LOGIN BUTTON
  if(( esat::MousePositionX() > 400 && esat::MousePositionX() < 550 
    && esat::MousePositionY() > 350 && esat::MousePositionY() < 400) || esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
    if(esat::MouseButtonDown(0) || esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
      gamestate = GameState::login;
    }
    esat::DrawSetTextSize(55);
    esat::DrawText(400, 400, "Login");
  }else{
    esat::DrawSetTextSize(50);
    esat::DrawText(400, 400, "Login");
  }
  //REGIStrTER BUTTON
  if(( esat::MousePositionX() > 370 && esat::MousePositionX() < 700 
    && esat::MousePositionY() > 450 && esat::MousePositionY() < 500)){

    if(esat::MouseButtonDown(0)){
      gamestate = GameState::registr;
    }
    esat::DrawSetTextSize(55);
    esat::DrawText(370, 500, "Registrter");
  }else{
    esat::DrawSetTextSize(50);
    esat::DrawText(370, 500, "Registrter");
  }
  //SCOREBOARD BUTTON
  if(( esat::MousePositionX() > 370 && esat::MousePositionX() < 700 
    && esat::MousePositionY() > 550 && esat::MousePositionY() < 600)){

    if(esat::MouseButtonDown(0)){
      gamestate = GameState::scoreboard;
    }
    esat::DrawSetTextSize(55);
    esat::DrawText(370, 600, "ScoreBoard");
  }else{
    esat::DrawSetTextSize(50);
    esat::DrawText(370, 600, "ScoreBoard");
  }
  

  esat::DrawSetTextSize(20);
  esat::DrawText(385, 870, "ASTEROIDS BY PAUU");  
  esat::DrawSetTextSize(50);
  esat::DrawEnd();
}
void RegistrterMenu(){
  static int charcounter = 0;
  static int infocounter = 0;
  esat::DrawBegin();
  esat::DrawClear(0,0,0);
  esat::DrawSetStrokeColor(255,255,255);
  esat::DrawSetFillColor(255,255,255,0);

  esat::DrawSetFillColor(255,255,255,255);
  esat::DrawSetTextSize(90);
  esat::DrawText(270, 120, "ASTEROIDS");  
  //LOGIN BUTTON
  esat::DrawSetTextSize(50);
  switch(infocounter){
    case 0:{
      esat::DrawText(220, 400, "Enter Your Username");
      esat::DrawText(300, 470, username);

      char auxkey = esat::GetNextPressedKey();
      if(auxkey != '\0' && charcounter < 12 && auxkey != ' '){
        *(username + charcounter) = auxkey;
        charcounter++; 
      }
      if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && charcounter > 0){
        charcounter--;
        *(username + charcounter) = '\0';
      }
      esat::ResetBufferdKeyInput();
      break;
    }
    case 1:{
      esat::DrawText(220, 400, "Enter Your Password");
      esat::DrawText(300, 470, user_password);

      char auxkey = esat::GetNextPressedKey();
      if(auxkey != '\0' && charcounter < 12 && auxkey != ' '){
        *(user_password + charcounter) = auxkey;
        charcounter++; 
      }else if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && charcounter > 0){
        charcounter--; 
        *(user_password + charcounter) = '\0';
      }
      esat::ResetBufferdKeyInput();
      break;
    }
  }
  
  esat::DrawSetTextSize(20);
  esat::DrawText(385, 870, "ASTEROIDS BY PAUU");  
  esat::DrawSetTextSize(50);
  esat::DrawEnd();
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
    infocounter++;
    charcounter = 0;
    if(infocounter == 2){
      gamestate = GameState::mainmenu;
      FILE *pfile = fopen("users.txt", "a+"); // Abre el archivo
      if (pfile != NULL) {
        fseek(pfile, 0, SEEK_END);
        fprintf(pfile, "%s %s\n", username, user_password);
        fclose(pfile);
      }
    }
  }

}
void LoginMenu(){
  static int charcounter = 0;
  static int infocounter = 0;
  static int correct = 0;
  esat::DrawBegin();
  esat::DrawClear(0,0,0);
  esat::DrawSetStrokeColor(255,255,255);
  esat::DrawSetFillColor(255,255,255,0);

  esat::DrawSetFillColor(255,255,255,255);
  esat::DrawSetTextSize(90);
  esat::DrawText(270, 120, "ASTEROIDS");  
  //LOGIN BUTTON
  esat::DrawSetTextSize(50);
  switch(infocounter){
    case 0:{
      esat::DrawText(220, 400, "Enter Your Username");
      esat::DrawText(300, 470, username);
      if(correct == 1){
        esat::DrawText(220, 340, "Unvalid Username");
      }
      char auxkey = esat::GetNextPressedKey();
      if(auxkey != '\0' && charcounter < 12 && auxkey != ' '){
        *(username + charcounter) = auxkey;
        charcounter++; 
      }
      if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && charcounter > 0){
        charcounter--;
        *(username + charcounter) = '\0';
      }
      esat::ResetBufferdKeyInput();
      break;
    }
    case 1:{
      esat::DrawText(220, 400, "Enter Your Password");
      esat::DrawText(300, 470, user_password);
      if(correct == 1){
        esat::DrawText(220, 340, "Unvalid Password");
      }
      char auxkey = esat::GetNextPressedKey();
      if(auxkey != '\0' && charcounter < 12 && auxkey != ' '){
        *(user_password + charcounter) = auxkey;
        charcounter++; 
      }else if(esat::IsSpecialKeyDown(esat::kSpecialKey_Backspace) && charcounter > 0){
        charcounter--; 
        *(user_password + charcounter) = '\0';
      }
      esat::ResetBufferdKeyInput();
      break;
    }
    case 2:{
      gamestate = GameState::mainmenu;
      break;
    }
  }
  
  esat::DrawSetTextSize(20);
  esat::DrawText(385, 870, "ASTEROIDS BY PAUU");  
  esat::DrawSetTextSize(50);
  esat::DrawEnd();
  if(esat::IsSpecialKeyDown(esat::kSpecialKey_Enter)){
    switch(infocounter){
      case 0: {
        FILE *pfile = fopen("users.txt", "r"); // Solo lectura
        if(pfile != NULL){
          int found = 0;
          while(fscanf(pfile, "%s %s", auxstring,auxstring2) == 2){
            if(strcmp(auxstring, username) == 0){
                found = 1;
                break;
            }
          }
          fclose(pfile);

          if(found == 1){
            infocounter++;
            correct = 0;
            charcounter = 0;
          }else{
            correct = 1;
          }
        }
        break;
      }
      case 1: {
        FILE *pfile = fopen("users.txt", "r"); // Solo lectura
          int found = 0;
          if(strcmp(auxstring2, user_password) == 0){
            found = 1;
          }

          if(found == 1){
            infocounter++;
            correct = 0;
            charcounter = 0;
          }else{
            correct = 1;
          }
          break;
      }
    }
  }
}