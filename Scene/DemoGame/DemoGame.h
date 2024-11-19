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
#include "Scene/DemoGame/Player.h"
#include "Scene/DemoGame/Colliders.h"
#include "Scene/DemoGame/Sound.h"
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

class BulletSetupSystem : public SetupSystem {
public:
  void run() override {
    // Define a template for the bullet, but do not spawn it immediately.
    Entity* bulletPrefab = scene->createEntity("BULLET_TEMPLATE", 0, 0);
    bulletPrefab->addComponent<BulletComponent>(); // Add Bullet Component
    bulletPrefab->addComponent<TextureComponent>("../Assets/Sprites/Bullet.bmp");
    bulletPrefab->addComponent<SpriteComponent>("../Assets/Sprites/Bullet.bmp", 16, 16, 1, 1, 1000);
    bulletPrefab->addComponent<BoxColliderComponent>(SDL_Rect{0, 0, 16, 16}, SDL_Color{255, 255, 0});
    bulletPrefab->addComponent<VelocityComponent>(0, 0);  // Initial velocity (will be set later)
    bulletPrefab->addComponent<LayerComponent>(1);


  }
};

class BulletRendererSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) {
  }
};


class PlayerMovement : public EventSystem {
 void run(SDL_Event e) {
   const Uint8* ks = SDL_GetKeyboardState(NULL);
   auto view = scene->r.view<PlayerComponent, PositionComponent, SpriteComponent, VelocityComponent,GravityComponent>();
   for (auto e1 : view) {
     auto& pos = view.get<PositionComponent>(e1);
     auto& vel = view.get<VelocityComponent>(e1);
     auto& grav = view.get<GravityComponent>(e1);
     vel.x = 0;
     if (ks[SDL_SCANCODE_A]) {
       vel.x += -350;  // Moving left
     }
     if (ks[SDL_SCANCODE_D]) {
       vel.x += 350;   // Moving right
     }
     if (ks[SDL_SCANCODE_W]) {
       vel.y = -75;   // Moving right
       grav.gravity = 9.81f * 5.0;
       auto audioView = scene->r.view<AudioComponent>();
       for (auto audioEntity : audioView) {
         auto& audio = audioView.get<AudioComponent>(audioEntity);
         audio.triggerPlay = true; // Set the trigger to play audio
         SDL_Log("Hi!");
       }
     }
     if (ks[SDL_SCANCODE_SPACE]) {
       createBullet(pos.x, pos.y); // Create bullet at the player's position
     }
   }
 }

private:
  void createBullet(int playerX, int playerY) {
    // Get the bullet template
    auto bulletTemplate = scene->r.view<BulletComponent>().front(); // Assuming you have only one bullet prefab
    auto& templatePos = scene->r.get<PositionComponent>(bulletTemplate);

    // Create the new bullet entity
    Entity* bullet = scene->createEntity("BULLET", playerX, playerY);

    // Copy components from the template to the new bullet
    bullet->addComponent<BulletComponent>(); // Add Bullet Component
    bullet->addComponent<TextureComponent>("../Assets/Sprites/Bullet.bmp");
    bullet->addComponent<SpriteComponent>("../Assets/Sprites/Bullet.bmp", 16, 16, 1, 1, 1000);
    bullet->addComponent<BoxColliderComponent>(SDL_Rect{0, 0, 16, 16}, SDL_Color{255, 255, 0});
    bullet->addComponent<VelocityComponent>(500, 0);  // Set velocity to the right
    bullet->addComponent<LayerComponent>(1);
  }


};



class BulletUpdateSystem : public UpdateSystem {
public:
  void run(float dt) override {
    auto view = scene->r.view<PositionComponent, VelocityComponent>();

    for (auto e : view) {
      auto& pos = view.get<PositionComponent>(e);
      auto& vel = view.get<VelocityComponent>(e);

      // Update position based on velocity
      pos.x += vel.x * dt;
      pos.y += vel.y * dt;

      // Handle out of bounds or collision logic (optional)
      if (pos.x < 0 || pos.x > 1024 || pos.y < 0 || pos.y > 720) {
        scene->r.remove<PositionComponent>(e); // Remove the bullet entity if out of bounds
        scene->r.remove<VelocityComponent>(e);
        scene->r.remove<SpriteComponent>(e); // Remove associated components
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

class GravitySystem : public UpdateSystem {
public:
  void run(float dt) override {
    auto view = scene->r.view<PositionComponent, VelocityComponent, GravityComponent>();

    for (auto e : view) {
      auto& pos = view.get<PositionComponent>(e);
      auto& vel = view.get<VelocityComponent>(e);
      auto& gravity = view.get<GravityComponent>(e);

      // Apply gravity
      vel.y += gravity.gravity * dt; // Update vertical velocity
      pos.y += vel.y * dt; // Update position based on velocity
    }
  }
};


class SquareSpawnSetupSystem : public SetupSystem {
  void run() {
Entity* square = scene->createEntity("SQUARE", 10, 400);
    square->addComponent<PlayerComponent>();
    square->addComponent<VelocityComponent>(300);
    square->addComponent<GravityComponent>(9.81f);
    square->addComponent<TextureComponent>("../Assets/Sprites/Human.bmp");
    square->addComponent<SpriteComponent>("../Assets/Sprites/Human.bmp", 16, 16, 7, 8, 1000);
    square->addComponent<BoxColliderComponent>(SDL_Rect{25, 0, 60, 100}, SDL_Color{255, 0, 0});
    square->addComponent<LayerComponent>(1);

    Entity* square2 = scene->createEntity("SQUARE", 200, 300);
    square2->addComponent<PowerUpComponent>();
    square2->addComponent<TextureComponent>("../Assets/Sprites/Alien.bmp");
    square2->addComponent<SpriteComponent>("../Assets/Sprites/Alien.bmp", 16, 16, 5, 8, 1000);
    square2->addComponent<BoxColliderComponent>(SDL_Rect{0, 0, 80, 80}, SDL_Color{0, 255, 0});
    square2->addComponent<EnemyComponent>();
    square2->addComponent<LayerComponent>(1);

    Entity* square3 = scene->createEntity("SQUARE", 100, 400);
    square3->addComponent<PowerUpComponent>();
    square3->addComponent<VelocityComponent>(0, 0);
    square3->addComponent<TextureComponent>("../Assets/Sprites/Alien.bmp");
    square3->addComponent<SpriteComponent>("../Assets/Sprites/Alien.bmp", 16, 16, 5, 8, 1000);
    square3->addComponent<BoxColliderComponent>(SDL_Rect{0, 0, 80, 80}, SDL_Color{0, 255, 0});
    square3->addComponent<EnemyComponent>();
    square3->addComponent<LayerComponent>(1);

    Entity* win = scene->createEntity("SQUARE", 400, 400, false);
    win->addComponent<WinComponent>();
    win->addComponent<TextureComponent>("../Assets/Sprites/Win.bmp");
    win->addComponent<SpriteComponent>("../Assets/Sprites/Win.bmp", 16, 16, 10, 8, 1000);
    win->addComponent<LayerComponent>(1);

    Entity* lose = scene->createEntity("SQUARE", 400, 400, false);
    lose->addComponent<LoseComponent>();
    lose->addComponent<TextureComponent>("../Assets/Sprites/Lose.bmp");
    lose->addComponent<SpriteComponent>("../Assets/Sprites/Lose.bmp", 16, 16, 10, 8, 1000);
    lose->addComponent<LayerComponent>(1);


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
class EnemyMovementSystem : public UpdateSystem {
public:
  void run(float dt) override {
    auto view = scene->r.view<EnemyComponent, PositionComponent, VelocityComponent>();

    for (auto e : view) {
      auto& pos = view.get<PositionComponent>(e);
      auto& vel = view.get<VelocityComponent>(e);

      // Move the enemy to the right at a constant speed
      pos.x += 1;
      if (pos.x > 768) { // Assuming screen width of 800
        pos.x = 0; // Reset to left side of the screen
      }

      // Optional: Add boundary checking to reset position or reverse direction
    }
  }
};

class BulletMovementSystem : public UpdateSystem {
public:
  void run(float dt) override {
    auto view = scene->r.view<BulletComponent, PositionComponent, VelocityComponent>();

    for (auto e : view) {
      auto& pos = view.get<PositionComponent>(e);
      auto& vel = view.get<VelocityComponent>(e);

      // Move the enemy to the right at a constant speed
      pos.x += 10;

      // Optional: Add boundary checking to reset position or reverse direction
    }
  }
};







class DemoGame : public Game {
public:
  Scene* sampleScene;
  entt::registry r;

public:
  DemoGame()
  : Game("SAMPLE", 800, 720)
  { }

  void setup() {
    sampleScene = new Scene("SAMPLE SCENE", r, renderer);
    addSetupSystem<BackgroundSetupSystem>(sampleScene);
    addSetupSystem<AudioSetupSystem>(sampleScene);
    addSetupSystem<SquareSpawnSetupSystem>(sampleScene);
    addSetupSystem<BulletSetupSystem>(sampleScene);
    addSetupSystem<TilemapSetupSystem>(sampleScene);
    addSetupSystem<TextureSetupSystem>(sampleScene);
    addSetupSystem<AutoTilingSetupSystem>(sampleScene);
    addSetupSystem<TilemapEntitySetupSystem>(sampleScene);





    addRenderSystem<BackgroundRenderSystem>(sampleScene);
    addRenderSystem<TilemapRenderSystem>(sampleScene);
    addRenderSystem<SpriteRenderSystem>(sampleScene);
    addRenderSystem<ColliderRenderSystem>(sampleScene);
    addUpdateSystem<AudioSystem>(sampleScene);
    addUpdateSystem<ColliderResetSystem>(sampleScene);
    addUpdateSystem<PlayerPowerUpCollisionDetectionSystem>(sampleScene);
    addUpdateSystem<PlayerPowerUpCollisionSystem>(sampleScene);
    addUpdateSystem<PlayerTileCollisionDetectionSystem>(sampleScene);
    addUpdateSystem<PlayerWallCollisionSystem>(sampleScene);
    addUpdateSystem<GravitySystem>(sampleScene);
    addUpdateSystem<MovementSystem>(sampleScene);
    addUpdateSystem<SpriteMovementSystem>(sampleScene);
    addUpdateSystem<SpriteAnimationSystem>(sampleScene);
    addUpdateSystem<BulletMovementSystem>(sampleScene);
    addUpdateSystem<EnemyMovementSystem>(sampleScene);

    addEventSystem<PlayerMovement>(sampleScene);





    //addUpdateSystem<WallHitSystem>(sampleScene);
    //addUpdateSystem<PositionUpdateSystem>(sampleScene);
    //addUpdateSystem<PhysicsSimulationSytem>(sampleScene);
    //addEventSystem<PlayerInputSystem>(sampleScene);
    //addUpdateSystem<RemoveSystem>(sampleScene);
    //addUpdateSystem<BallCollision>(sampleScene);


    //addRenderSystem<SquareRenderSystem>(sampleScene);


    setScene(sampleScene);
  }
};