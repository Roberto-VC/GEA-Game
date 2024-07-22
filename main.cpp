#define SDL_MAIN_HANDLED
#include <algorithm>
#include <SDL_keyboard.h>
#include <SDL_render.h>
#include <SDL_scancode.h>
#include <string>
#include <iostream>
#include <SDL.h>
#include <vector>
#include <cstdio>


using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int MAX_FPS = 60;
const float BALL_SPEED = 200;
const int BALL_SIZE = 20;
const int BLOCK_WIDTH = 100;
const int BLOCK_HEIGHT = 30;
const int NUM_ROWS = 6;
const int NUM_COLS = 5;

struct Rect {
  SDL_Rect rect = {0, 0, 100, 100};
  float vx = 0;
  float vy = 0;
  SDL_Color color = {0xFF, 0xFF, 0xFF, 0xFF};
  bool isActive = true;
};

SDL_Color red = {0xFF, 0x00, 0x00, 0xFF};
SDL_Color blue = {0x00, 0x00, 0xFF, 0xFF};
SDL_Color white = {0xFF, 0xFF, 0xFF, 0xFF};

Rect ball1 = {{250, 400, 150, 30}, 0, 0, red};
Rect ball2 = {{SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 10, 10}, -BALL_SPEED, BALL_SPEED, blue};

// Array to hold blocks
std::vector<std::vector<Rect>> blocks;
bool GameOver;
bool GameFin;

SDL_Color rainbowColors[] = {
  {255, 0, 0, 255},     // Red
  {255, 165, 0, 255},   // Orange
  {255, 255, 0, 255},   // Yellow
  {0, 255, 0, 255},     // Green
  {0, 0, 255, 255},     // Blue
  {148, 0, 211, 255}    // Violet
};

// Initialize blocks array
void initializeBlocks() {
  for (int row = 0; row < NUM_ROWS; row++) {
    blocks.push_back(std::vector<Rect>());
    for (int col = 0; col < NUM_COLS; col++) {
      
        Rect block = {{row * (BLOCK_WIDTH + 10),col * (BLOCK_HEIGHT + 5), BLOCK_WIDTH, BLOCK_HEIGHT}, 0, 0,
        rainbowColors[col],  true}; // Adjust spacing as needed
        blocks[row].push_back(block);


    }
  }
}

void deactivateRect(Rect& rectObject) {
  rectObject.isActive = false; // Correct: isActive is mutable
}

// Function to render blocks
void renderBlocks(SDL_Renderer* renderer) {
  SDL_Rect blockRect;

  for (const auto& row : blocks) {
    for (const auto& block : row) {
      if (block.isActive) {
        blockRect = { block.rect.x, block.rect.y, block.rect.w, block.rect.h };
        SDL_SetRenderDrawColor(renderer, block.color.r, block.color.g, block.color.b, block.color.a);
        SDL_RenderFillRect(renderer, &blockRect);
      }
    }
  }
}


void renderRect(SDL_Renderer* renderer, Rect& ball) {
  SDL_SetRenderDrawColor(renderer, ball.color.r, ball.color.g, ball.color.b, ball.color.a);
  SDL_RenderFillRect(renderer, &ball.rect);
}

bool checkColission(const SDL_Rect& a, const SDL_Rect& b) {
  return (
    a.x < b.x + b.w &&
    a.x + a.w > b.x &&
    a.y < b.y + b.h &&
    a.y + a.h > b.y
  );
}

void handleInput(SDL_Event& e) {
  // resolve
  const Uint8* ks = SDL_GetKeyboardState(NULL);

  ball1.vx = 0;
  ball1.vy = 0;


  if (ks[SDL_SCANCODE_D]) {
    ball1.vx = BALL_SPEED;
  }
  if (ks[SDL_SCANCODE_A]) {
    ball1.vx = -BALL_SPEED;
  }
}

void removeEmptyRows(std::vector<std::vector<Rect>>& blocks) {
  blocks.erase(std::remove_if(blocks.begin(), blocks.end(),
                              [](const std::vector<Rect>& row) {
                                  return row.empty();
                              }),
               blocks.end());
}

void update(float dT) {
  if (ball1.rect.x < 0) {
    ball1.vx *= -1;
  }
  if (ball2.rect.x < 0) {
    ball2.vx *= -1;
  }
  if (ball1.rect.y < 0) {
    ball1.vy *= -1;
  }
  if (ball2.rect.y < 0) {
    ball2.vy *= -1;

  }
  if (ball1.rect.x + ball1.rect.w > SCREEN_WIDTH) {
    ball1.vx *= -1;
  }
  if (ball2.rect.x + ball2.rect.w > SCREEN_WIDTH) {
    ball2.vx *= -1;
  }
  if (ball1.rect.y + ball1.rect.h > SCREEN_HEIGHT) {
    ball1.vy *= -1;
  }
  if (ball2.rect.y > SCREEN_HEIGHT) {

    GameOver = true;

  }

  if (checkColission(ball1.rect, ball2.rect)) {

    ball2.vy *= -1.1;
    //ball2.vx *= ball1.vx;

  }

  ball1.rect.x += ball1.vx * dT;
  ball1.rect.y += ball1.vy * dT;
  ball2.rect.x += ball2.vx * dT;
  ball2.rect.y += ball2.vy * dT;

  for (auto& row : blocks) {
    for (auto& block : row) {

      if (checkColission(ball2.rect, block.rect)) {
        deactivateRect(block);
        if (ball2.vy < 5000){
        ball2.vy *= -1;
          }

      }
    }
  }
  for (auto& row : blocks) {
    row.erase(std::remove_if(row.begin(), row.end(), [](const Rect& block) {
        return !block.isActive;
    }), row.end());
  }

  removeEmptyRows(blocks);

  std::string s = std::to_string(blocks.size());
  const char* cstr = s.c_str();

  //SDL_Log(cstr);
  if (blocks.empty()) {
    // 'blocks' is empty
    std::cout << "blocks vector is now empty\n";
    GameFin = true;
  }




}

int main() {
  SDL_Init(SDL_INIT_EVERYTHING);

  SDL_Window* window = SDL_CreateWindow("", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  bool quit = false;
  SDL_Event e;

  initializeBlocks();
  printf("GameOver");

  Uint32 frameCount = 1;
  Uint32 frameStartTimestamp;
  Uint32 frameEndTimestamp;
  Uint32 lastFrameTime = SDL_GetTicks();
  Uint32 lastUpdateTime = 0;
  float frameDuration = (1.0/MAX_FPS) * 1000.0;
  float actualFrameDuration;
  int FPS = MAX_FPS;


  while (!quit) {
    frameStartTimestamp = SDL_GetTicks();

    // delta time
    Uint32 currentFrameTime = SDL_GetTicks();
    float dT = (currentFrameTime - lastFrameTime) / 1000.0;
    lastFrameTime = currentFrameTime;

    // poll events
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT || GameOver || GameFin) {
        quit = true;
        if (GameOver) {
          SDL_Log("Game Over");
        }
        else if (GameFin) {
          SDL_Log("Level Complete!");
        }
      }
      handleInput(e);
    }
    // update
    update(dT);
    // render
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    renderRect(renderer, ball1);
    renderRect(renderer, ball2);
    renderBlocks(renderer);
    SDL_RenderPresent(renderer);

    frameEndTimestamp = SDL_GetTicks();
    actualFrameDuration = frameEndTimestamp - frameStartTimestamp;

    if (actualFrameDuration < frameDuration) {
      SDL_Delay(frameDuration - actualFrameDuration);
    }

    // fps calculation
    frameCount++;
    Uint32 currentTime = SDL_GetTicks();
    Uint32 elapsedTime = currentTime - lastUpdateTime;
    if (elapsedTime > 1000) {
      FPS = (float)frameCount / (elapsedTime / 1000.0);
      lastUpdateTime = currentTime;
      frameCount = 0;
    }
    SDL_SetWindowTitle(window, ("FPS: " + std::to_string(FPS)).c_str());
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}