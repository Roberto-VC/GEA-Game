#include "Scene/Engine/Game.h"
#include "Scene/Engine/Scene.h"
#include "Scene/Engine/Entity.h"
#include "Scene/Engine/Components.h"
#include "Scene/Engine/Systems.h"
#include <entt/src/entt/entt.hpp>
#include "Collision.h"
#include "Scene/Engine/Graphics/TextureManager.h"
#include "Scene/Engine/Graphics/Texture.h"
#include <SDL_render.h>
#include <time.h>
#include <random>
#include <chrono>



struct SpriteComponent {
  int width;
  int height;
  SDL_Color color;
  bool active = true;
};

struct ParallaxComponent {
  float parallaxFactor; // How much this layer should move relative to the camera
  float xOffset;        // Current horizontal offset
  float yOffset;        // Current vertical offset
};

struct Moving {

};

bool checkCollision(const PositionComponent pos1, const SpriteComponent spr1,
                 const PositionComponent pos2, const SpriteComponent spr2) {

  return !(pos1.x + spr1.width < pos2.x ||
           pos2.x + spr2.width < pos1.x ||
           pos1.y + spr1.height < pos2.y ||
           pos2.y + spr2.height < pos1.y);
}
void handleCollision(const PositionComponent& ballpos, const SpriteComponent& ballsprt,
                   VelocityComponent& ballvel, const PositionComponent& brickpos,
                   const SpriteComponent& bricksprt) {


  float overlapX = std::min(ballpos.x + ballsprt.width - brickpos.x, brickpos.x + bricksprt.width - ballpos.x);
  float overlapY = std::min(ballpos.y + ballsprt.height - brickpos.y, brickpos.y + bricksprt.height - ballpos.y);

  if (overlapX < overlapY) {
    // Collision on vertical edges
    ballvel.x *= -1; // Reverse horizontal velocity
  } else {
    // Collision on horizontal edges
    ballvel.y *= -1; // Reverse vertical velocity
  }
}


class PlayerMovement : public EventSystem {
 void run(SDL_Event e) {
   const Uint8* ks = SDL_GetKeyboardState(NULL);
   auto view = scene->r.view<PlayerComponent, PositionComponent, SpriteComponent, VelocityComponent>();
   for (auto e1 : view) {
     auto& vel = view.get<VelocityComponent>(e1);
     vel.x = 0;
     vel.y = 0;
     if (ks[SDL_SCANCODE_A]) {
       vel.x += -350;  // Moving left
     }
     if (ks[SDL_SCANCODE_D]) {
       vel.x += 350;   // Moving right
     }
   }
 }

};

class BallCollision : public UpdateSystem {
  void run(float dt) {

    auto view = scene->r.view<BallComponent, PositionComponent, SpriteComponent, VelocityComponent>();
    for (auto e1: view) {
      auto ballpos = view.get<PositionComponent>(e1);
      auto ballsprt = view.get<SpriteComponent>(e1);
      auto &ballvel = view.get<VelocityComponent>(e1);
     // auto ball = view.get<BallComponent>(e1);
      auto viewBricks = scene->r.view<BrickComponent, PositionComponent, SpriteComponent>();
      for (auto e2 : viewBricks) {
        auto brickpos = view.get<PositionComponent>(e2);
        auto& bricksprt = view.get<SpriteComponent>(e2);
        //auto& brick = view.get<BrickComponent>(e1);

        if (checkCollision(brickpos, bricksprt, ballpos, ballsprt)) {
          handleCollision(ballpos, ballsprt, ballvel, brickpos, bricksprt);
          bricksprt.active = false;
        }
      }

      auto viewPlayer = scene->r.view<PlayerComponent, PositionComponent, SpriteComponent>();
      for (auto e2 : viewPlayer) {
        auto brickpos = view.get<PositionComponent>(e2);
        auto& bricksprt = view.get<SpriteComponent>(e2);
        //auto& brick = view.get<BrickComponent>(e1);

        if (checkCollision(brickpos, bricksprt, ballpos, ballsprt)) {
          handleCollision(ballpos, ballsprt, ballvel, brickpos, bricksprt);
        }
      }
    }
  }
};

class SquareSpawnSetupSystem : public SetupSystem {
  void run() {
    Entity* square2 = scene->createEntity("SQUARE2", 400, 400);
    square2->addComponent<VelocityComponent>(-400, -400);
    square2->addComponent<SpriteComponent>(20, 20, SDL_Color{0, 0, 255}, true);
    square2->addComponent<BallComponent>();
    //square2->addComponent<NameComponent>("SQUARE2");

    Entity* square = scene->createEntity("SQUARE", 10, 10);
    square->addComponent<SpriteComponent>(200, 100, SDL_Color{255, 0, 0}, true);
    square->addComponent<BrickComponent>();

    Entity* square4 = scene->createEntity("SQUARE4", 220, 10);
    square4->addComponent<SpriteComponent>(200, 100, SDL_Color{255, 0, 0}, true);
    square4->addComponent<BrickComponent>();

    Entity* square5 = scene->createEntity("SQUAR5", 430, 10);
    square5->addComponent<SpriteComponent>(200, 100, SDL_Color{255, 0, 0}, true);
    square5->addComponent<BrickComponent>();

    Entity* square6 = scene->createEntity("SQUAR5", 640, 10);
    square6->addComponent<SpriteComponent>(200, 100, SDL_Color{255, 0, 0}, true);
    square6->addComponent<BrickComponent>();

    Entity* square7 = scene->createEntity("SQUAR5", 850, 10);
    square7->addComponent<SpriteComponent>(200, 100, SDL_Color{255, 0, 0}, true);
    square7->addComponent<BrickComponent>();
    //square2->addComponent<NameComponent>("SQUARE");

    Entity* square3 = scene->createEntity("SQUARE", 400, 650);
    square3->addComponent<SpriteComponent>(250, 50, SDL_Color{0, 255, 0}, true);
    square3->addComponent<PlayerComponent>();
    square3->addComponent<VelocityComponent>(0,0);
    //square2->addComponent<NameComponent>("SQUARE");
  }
};

class MovementSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<PositionComponent, VelocityComponent>();

    for (auto e : view) {
      auto& pos = view.get<PositionComponent>(e);
      auto vel = view.get<VelocityComponent>(e);

      pos.x += vel.x * dT;
      pos.y += vel.y * dT;
    }
  }
};

class WallHitSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<PositionComponent, VelocityComponent, SpriteComponent>();

    for (auto e : view) {
      auto pos = view.get<PositionComponent>(e);
      auto spr = view.get<SpriteComponent>(e);
      auto& vel = view.get<VelocityComponent>(e);


      int newPosX = pos.x + vel.x * dT;
      int newPosY = pos.y + vel.y * dT;

      if (newPosX < 0 || newPosX + spr.width > 1024) {
        vel.x *= -1;
      }

      if (newPosY < 0) {
        vel.y *= -1;
      }
      if (newPosY + spr.height > 768) {
        setScene(NULL);
      }

    }
  }
};





class SquareRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) {
    auto view = scene->r.view<PositionComponent, SpriteComponent>();
    for (auto e : view) {

      auto pos = view.get<PositionComponent>(e);
      auto spr = view.get<SpriteComponent>(e);

      if (spr.active) {
        SDL_SetRenderDrawColor(renderer, spr.color.r, spr.color.g, spr.color.b, spr.color.a);
        SDL_Rect r = { pos.x, pos.y, spr.width, spr.height };
        SDL_RenderFillRect(renderer, &r);
      }
    }
    auto view2 = scene->r.view<BrickComponent>();
    if (view2.empty()) {
      setScene(NULL);
    };
  }
};

class RemoveSystem : public UpdateSystem {
  void run(float dt) {
    // Iterate through all entities with a SpriteComponent
    auto view1 = scene->r.view<BrickComponent>();


    auto view = scene->r.view<SpriteComponent, PositionComponent, BrickComponent>();
    for (auto e : view) {
      auto& spr = view.get<SpriteComponent>(e);
      if (!spr.active) {
        // Remove the entity from the scene
        scene->r.remove<PositionComponent>(e); // Remove PositionComponent
        scene->r.remove<SpriteComponent>(e);   // Remove SpriteComponent
        scene->r.remove<BrickComponent>(e);
        // Optionally, remove other components associated with the entity
        // scene->r.remove<OtherComponent>(e);
      }
    }
  }
};

struct TextureComponent {
  std::string filename;
};

struct BackgroundComponent {
  std::string filename;
};

class BackgroundSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* background = scene->createEntity("BACKGROUND");

    Entity* background2 = scene->createEntity("BACKGROUND");
    const std::string& bgfile2 = "D:/Temp/Game/Assets/Backgrounds/Blue.bmp";
    background2->addComponent<TextureComponent>(bgfile2);
    background2->addComponent<BackgroundComponent>(bgfile2);
    const std::string& bgfile = "D:/Temp/Game/Assets/Backgrounds/Red.bmp";
    background->addComponent<TextureComponent>(bgfile);
    background->addComponent<BackgroundComponent>(bgfile);

  }
};

class TextureSetupSystem : public SetupSystem {
  void run() {
    auto view = scene->r.view<TextureComponent>();
    for (auto e : view) {
      auto tex = view.get<TextureComponent>(e);
      TextureManager::LoadTexture(tex.filename, scene->renderer);
    }
  }
};

class BackgroundRenderSystem : public RenderSystem {
public:
  BackgroundRenderSystem()
    : lastSwitchTime(std::chrono::steady_clock::now()),
      switchInterval(std::chrono::seconds(5)),  // Change interval as needed
      blinkInterval(std::chrono::milliseconds(200)), // Interval for blinking
      rng(std::random_device{}()),
      dist(0, 0), // Initialized later with the correct range
      isBlinking(false),
      blinkStartTime(std::chrono::steady_clock::now())
  {}

  void run(SDL_Renderer* renderer) {
    auto now = std::chrono::steady_clock::now();

    // Check if it's time to switch the background
    if (now - lastSwitchTime > switchInterval) {
      lastSwitchTime = now;

      // Get a list of all background components
      auto view = scene->r.view<BackgroundComponent>();
      std::vector<BackgroundComponent*> components;

      for (auto e : view) {
        components.push_back(&view.get<BackgroundComponent>(e));
      }

      if (!components.empty()) {
        // Set up the random number generator to select an index
        dist.param(std::uniform_int_distribution<size_t>::param_type(0, components.size() - 1));
        auto selectedIndex = dist(rng);
        selectedComponent = components[selectedIndex];

        // Start blinking effect
        isBlinking = true;
        blinkStartTime = now;
      }
    }

    // Calculate elapsed time for blinking effect
    auto elapsedBlinkTime = now - blinkStartTime;
    bool shouldBlink = isBlinking && (elapsedBlinkTime < blinkInterval);

    if (shouldBlink) {
      // Render a blank screen or a color to indicate blinking
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White or any color
      SDL_RenderClear(renderer);
    } else {
      // Render the selected background component
      if (selectedComponent) {
        auto texture = TextureManager::GetTexture(selectedComponent->filename);
        texture->render(renderer, 100, 0);  // Adjust position as needed
      }

      // Reset blinking state after rendering the new background
      if (isBlinking && elapsedBlinkTime >= blinkInterval) {
        isBlinking = false;
      }
    }
  }

private:
  std::chrono::steady_clock::time_point lastSwitchTime;
  std::chrono::seconds switchInterval;
  std::chrono::milliseconds blinkInterval;
  std::mt19937 rng;
  std::uniform_int_distribution<size_t> dist;
  BackgroundComponent* selectedComponent = nullptr;
  bool isBlinking;
  std::chrono::steady_clock::time_point blinkStartTime;
};

class DemoGame : public Game {
public:
  Scene* sampleScene;
  entt::registry r;

public:
  DemoGame()
  : Game("SAMPLE", 1024, 768)
  { }

  void setup() {
    sampleScene = new Scene("SAMPLE SCENE", r, renderer);
    //addSetupSystem<SquareSpawnSetupSystem>(sampleScene);
    addSetupSystem<BackgroundSetupSystem>(sampleScene);
    addSetupSystem<TextureSetupSystem>(sampleScene);

    //addUpdateSystem<MovementSystem>(sampleScene);
    //addUpdateSystem<WallHitSystem>(sampleScene);
    //addEventSystem<PlayerMovement>(sampleScene);
    //addUpdateSystem<RemoveSystem>(sampleScene);
    //addUpdateSystem<BallCollision>(sampleScene);
    addRenderSystem<BackgroundRenderSystem>(sampleScene);
    //addRenderSystem<SquareRenderSystem>(sampleScene);


    setScene(sampleScene);
  }
};