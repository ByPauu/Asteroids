#include <esat/window.h>
#include <esat/draw.h>
#include <esat/input.h>
#include <esat/sprite.h>
#include <esat/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#define PI 3.14159265


//PLAYER
struct Player{
  esat::Vec2 center;
  esat::Vec2 speed;
  float mainpoints[12];  //All player points that conform the figure
  float fire[6];        //Player points of the fire booster
  float angle;        //Player global angle
  int stage;          //Stage: 1 -> Normal, Alive | 2 -> Dead, Respawning
  int lives;          //Number of lives player has
  int counter;        //Timer to various functions    
  float mass;
};
Player* player;

const int kWindowHeight = 1000;
const int kWindowWidth = 1000;
unsigned char fps = 60; //Control de frames por segundo
int frameCounter = 0;
double deltaTime = 0.0, current_time = 0.0, last_time = 0.0;
double bullet_time = 0.0, current_bullet_time = 0.0;

int tra = 1;
esat::Vec2* trayec = (esat::Vec2*) malloc(sizeof(esat::Vec2) * tra);
bool start = false;

struct BlackHole{
  esat::Vec2 center;
  esat::Vec3* base;
  esat::Vec2* points;
  int stage;
  float mass;
};
BlackHole* blackhole =  (BlackHole*) malloc(sizeof(BlackHole) * 1);

void DrawPlayer();
void InputPlayer();
void UpdatePlayer();
void BlackHoleUpdate();
void DrawBlackHole();
void InitBlackHole();

int esat::main(int argc, char **argv) {
  player = (Player*) malloc(sizeof(Player) * 1);
  (player)->center.x = kWindowWidth / 1.5;
  (player)->center.y = kWindowHeight / 2;
  (player)->speed.x = 0;
  (player)->speed.y = 0;
  (player)->angle = -90 * PI / 180;
  (player)->lives = 3;
  (player)->stage = 1;
  (player)->counter = 0;

	esat::WindowInit(kWindowWidth,kWindowHeight);
	esat::WindowSetMouseVisibility(true);
  InitBlackHole();
  while(esat::WindowIsOpened() && !esat::IsSpecialKeyDown(esat::kSpecialKey_Escape)) {
    //Control time & fps
    do{
      current_time = esat::Time();
      deltaTime = current_time - last_time;
    } while((deltaTime) <= 1000.0 / fps);
    frameCounter = (frameCounter>=fps)?0:frameCounter +1;
    last_time = esat::Time();
    for(int i = 0; i < 1; i++){
      BlackHoleUpdate();
      InputPlayer();

      esat::DrawBegin();
      esat::DrawClear(0,0,0);
      DrawBlackHole();
      DrawPlayer();
      esat::DrawEnd();

      esat::WindowFrame();
    }
  }
  esat::WindowDestroy();
  return 0;
}


void InitBlackHole(){
  float angle = 2 * PI / 5;
  blackhole->base = (esat::Vec3*) malloc(sizeof(esat::Vec3) * 5);
  blackhole->points = (esat::Vec2*) malloc(sizeof(esat::Vec2) * 5);
  blackhole->stage = 0;

  blackhole->center.x = 500;
  blackhole->center.y = 500;

  for(int i = 0; i < 5; i++){
    (blackhole->base + i)->x = cosf(i * angle) * 50;
    (blackhole->base + i)->y = sinf(i * angle) * 50;
    (blackhole->base + i)->z = 1.0f;
  }
}
void DrawBlackHole(){
  if(blackhole->stage > 0){
    for(int i = 4; i >= 0; i--){
      esat::Mat3 m = esat::Mat3Identity();
      m = esat::Mat3Multiply(esat::Mat3Translate(0.0f, 0.0f),m);
      m = esat::Mat3Multiply(esat::Mat3Scale(i * 0.25 + 1, i * 0.25 + 1), m);
      m = esat::Mat3Multiply(esat::Mat3Rotate((i * 33 * PI / 180 + esat::Time() * 0.001) * (((i % 2) * 2) - 1)), m);
      m = esat::Mat3Multiply(esat::Mat3Translate(blackhole->center.x	, blackhole->center.y), m);
  
      for (int e = 0; e < 5; e++) {
        esat::Vec3 tmp = esat::Mat3TransformVec3(m, *(blackhole->base + e));
        *(blackhole->points + e) = { tmp.x, tmp.y };
      }
      esat::DrawSetStrokeColor(5 + i * 50, 5 + i * 50, 5 + i * 50, 255);
      esat::DrawSetFillColor(i * 10, i * 10, i * 10, 255);
      esat::DrawSolidPath(&blackhole->points->x, 5, true);
    }
  }
  if(start){
    trayec = (esat::Vec2*) realloc( trayec, tra * sizeof(esat::Vec2));
    (trayec + tra - 1)->x = player->center.x;
    (trayec + tra - 1)->y = player->center.y;
    esat::DrawSetStrokeColor(255, 255, 255, 255);
    esat::DrawSetFillColor(0, 0, 0, 255);
    esat::DrawPath(&trayec->x, tra);
    tra++;
  }
  

}

void BlackHoleUpdate(){
  if(esat::IsKeyDown('K')){
    blackhole->stage = 1;
    start = true;
  }
  if(blackhole->stage > 0){
    double vecnuevo_x = blackhole->center.x - player->center.x;
    double vecnuevo_y = blackhole->center.y - player->center.y;
    double norm = sqrtf(vecnuevo_x * vecnuevo_x + vecnuevo_y * vecnuevo_y);
    vecnuevo_x = vecnuevo_x / norm;
    vecnuevo_y = vecnuevo_y / norm;

    float kGravity = 0.1;
    float kForce = 1;

    player->speed.x +=  vecnuevo_x * kGravity;
    player->speed.y +=  vecnuevo_y * kGravity;

    printf("\n\n%.10f\n%.10f",vecnuevo_x,vecnuevo_y);
  }
  
}






void DrawPlayer(){
  if(player->stage == 1){
    player->center.x += player->speed.x;
    player->center.y += player->speed.y;

    //HEAD
    *(player->mainpoints + 0) = cosf(((0 * PI) / 180) + player->angle) * 18 + player->center.x;
    *(player->mainpoints + 1) = sinf(((0 * PI) / 180) + player->angle) * 18 + player->center.y;

    //SIDES
    *(player->mainpoints + 2) = cosf((( 153 * PI) / 180) +  player->angle) * 25 + player->center.x;
    *(player->mainpoints + 3) = sinf((( 153 * PI) / 180) + player->angle) * 25 + player->center.y;
    *(player->mainpoints + 8) = cosf(((-153 * PI) / 180) + player->angle) * 25 + player->center.x;
    *(player->mainpoints + 9) = sinf(((-153 * PI) / 180) + player->angle) * 25 + player->center.y;

    //BACK
    *(player->mainpoints + 4) = cosf((( 160 * PI) / 180) + player->angle) * 18 + player->center.x;
    *(player->mainpoints + 5) = sinf((( 160 * PI) / 180) + player->angle) * 18 + player->center.y;
    *(player->mainpoints + 6) = cosf(((-160 * PI) / 180) + player->angle) * 18 + player->center.x;
    *(player->mainpoints + 7) = sinf(((-160 * PI) / 180) + player->angle) * 18 + player->center.y;

    *(player->mainpoints + 10) = player->center.x;
    *(player->mainpoints + 11) = player->center.y;

    if(esat::IsKeyPressed('W')){
      //FIRE
      *(player->fire + 0) = cosf((( 164 * PI) / 180) + player->angle) * 18 + player->center.x;
      *(player->fire + 1) = sinf((( 164 * PI) / 180) + player->angle) * 18 + player->center.y;
      *(player->fire + 2) = cosf((( 180 * PI) / 180) + player->angle) * 25 + player->center.x;
      *(player->fire + 3) = sinf((( 180 * PI) / 180) + player->angle) * 25 + player->center.y;
      *(player->fire + 4) = cosf(((-164 * PI) / 180) + player->angle) * 18 + player->center.x;
      *(player->fire + 5) = sinf(((-164 * PI) / 180) + player->angle) * 18 + player->center.y;
    }
  }
  

  esat::DrawSetStrokeColor(255,255,255);
  esat::DrawSetFillColor(255,255,255,0);
  esat::DrawSolidPath(player->mainpoints, 5);
  
  //DRAW FIRE
  if(esat::IsKeyPressed('W') && (frameCounter % 2 == 0)){
    esat::DrawSolidPath(player->fire, 3);
  }

  esat::DrawEnd();
}

void InputPlayer(){
  if(player->stage == 1){
    if(player->speed.x != 0 && !esat::IsKeyPressed('W')){
      //player->speed.x *= 0.965;
    }
    if(player->speed.y != 0 && !esat::IsKeyPressed('W')){
      //player->speed.y *= 0.965;
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
  }

  if(player->center.x < 0){
    player->center.x = 1000;
  }
  if(player->center.x > 1000){
    player->center.x = 0;
  }
  if(player->center.y < 0){
    player->center.y = 1000;
  }
  if(player->center.y > 1000){
    player->center.y = 0;
  }
}
