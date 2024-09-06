#include "Scene/Engine/Game.h"
#include "Scene/Engine/Scene.h"
#include "Scene/Engine/Entity.h"
#include "Scene/Engine/Components.h"
#include "Scene/Engine/Systems.h"
#include <entt/src/entt/entt.hpp>
#include "Collision.h"
#include "Scene/Engine/Graphics/TextureManager.h"
#include "Scene/Engine/Graphics/Texture.h"
#include "Scene/DemoGame/Backgrounds.h"
//#include "Scene/DemoGame/Sprites.h"
#include "Scene/DemoGame/Tilemap.h"
#include <SDL_render.h>





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
     if (ks[SDL_SCANCODE_S]) {
       vel.y += 350;   // Moving right
     }
     if (ks[SDL_SCANCODE_W]) {
       vel.y += -350;  // Moving left
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
Entity* square = scene->createEntity("SQUARE", 10, 10);
    square->addComponent<PlayerComponent>();
    square->addComponent<VelocityComponent>(300);
    square->addComponent<TextureComponent>("D:/Temp/Game/Assets/Sprites/Human.bmp");
    square->addComponent<SpriteComponent>("D:/Temp/Game/Assets/Sprites/Human.bmp", 16, 16, 7, 8, 1000);
    square->addComponent<LayerComponent>(1);

    Entity* square2 = scene->createEntity("SQUARE", 10, 10);
    square2->addComponent<TextureComponent>("D:/Temp/Game/Assets/Sprites/Alien.bmp");
    square2->addComponent<SpriteComponent>("D:/Temp/Game/Assets/Sprites/Alien.bmp", 16, 16, 5, 8, 1000);
    square2->addComponent<LayerComponent>(1);

    Entity* square3 = scene->createEntity("SQUARE", 60, 10);
    square3->addComponent<TextureComponent>("D:/Temp/Game/Assets/Sprites/Alien.bmp");
    square3->addComponent<SpriteComponent>("D:/Temp/Game/Assets/Sprites/Alien.bmp", 16, 16, 5, 8, 1000);
    square3->addComponent<LayerComponent>(1);
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





/*class SquareRenderSystem : public RenderSystem {
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
};*/

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
    addSetupSystem<BackgroundSetupSystem>(sampleScene);
    addSetupSystem<SquareSpawnSetupSystem>(sampleScene);
    addSetupSystem<TilemapSetupSystem>(sampleScene);
    addSetupSystem<TextureSetupSystem>(sampleScene);
    addSetupSystem<AdvancedAutoTilingSetupSystem>(sampleScene);
    addRenderSystem<BackgroundRenderSystem>(sampleScene);
    addRenderSystem<SpriteRenderSystem>(sampleScene);
    //addRenderSystem<TilemapRenderSystem>(sampleScene);
    addUpdateSystem<MovementSystem>(sampleScene);
    addUpdateSystem<SpriteMovementSystem>(sampleScene);
    addUpdateSystem<SpriteAnimationSystem>(sampleScene);
    //addUpdateSystem<WallHitSystem>(sampleScene);
    addEventSystem<PlayerMovement>(sampleScene);
    //addUpdateSystem<RemoveSystem>(sampleScene);
    //addUpdateSystem<BallCollision>(sampleScene);


    //addRenderSystem<SquareRenderSystem>(sampleScene);


    setScene(sampleScene);
  }
};