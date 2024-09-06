#pragma once
#include <string>
#include "Scene/Engine/Systems.h"
#include "Scene/Engine/Components.h"
#include "Scene/Engine/Graphics/TextureManager.h"



struct TextureComponent {
  std::string filename;
};

struct LayerComponent {
  int layer; // Lower value means further back, higher value means in front
};


struct SpriteComponent {
  std::string filename;
  int width;
  int height;
  int scale = 1;
  int animationFrames = 0;
  int animationDuration = 0;
  Uint32 lastUpdate = 0;
  int xIndex = 0;
  int yIndex = 0;
  bool active = true;
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

class SpriteMovementSystem : public UpdateSystem {
  void run(float dT) {
    auto view = scene->r.view<SpriteComponent, VelocityComponent>();

    for (auto e : view) {
      auto& spr = view.get<SpriteComponent>(e);
      auto vel = view.get<VelocityComponent>(e);

      if (vel.x == 0) {
        if (spr.yIndex == 1) {
          spr.yIndex = 0;
        }
      }
      if (vel.x < 0 || vel.y < 0) {
        spr.yIndex = 1;
      }
      if (vel.x > 0 || vel.y > 0) {
        spr.yIndex = 1;
      }
   }
  }
};

class SpriteAnimationSystem : public UpdateSystem {
  void run(float dT) override {
    auto view = scene->r.view<SpriteComponent>();
    Uint32 now = SDL_GetTicks();

    for (auto e : view) {
      auto& spr = view.get<SpriteComponent>(e);

      if (spr.animationFrames > 0) {
        if (spr.lastUpdate == 0) {
          spr.lastUpdate = now;
          continue;
        }
        float timeSinceLastUpdate = now - spr.lastUpdate;

        int lastFrame = spr.animationFrames - 1;

        int framesToUpdate = timeSinceLastUpdate / spr.animationDuration * spr.animationFrames; 

        if (framesToUpdate > 0) {
          spr.xIndex += framesToUpdate;
          spr.xIndex %= spr.animationFrames;
          spr.lastUpdate = now;
        }

      }
    }
  }
};

class SpriteRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) {
    // Create a view that includes PositionComponent, SpriteComponent, and LayerComponent
    auto view = scene->r.view<PositionComponent, SpriteComponent, LayerComponent>();

    // Create a vector of entities
    std::vector<entt::entity> sortedEntities;
    sortedEntities.reserve(std::distance(view.begin(), view.end()));

    // Populate the vector with entities from the view
    for (auto e : view) {
      sortedEntities.push_back(e);
    }

    // Sort entities by their layer component
    std::sort(sortedEntities.begin(), sortedEntities.end(), [this](auto a, auto b) {
      auto& layerA = scene->r.get<LayerComponent>(a);
      auto& layerB = scene->r.get<LayerComponent>(b);
      return layerA.layer < layerB.layer;
    });

    // Render sorted entities
    for (auto e : sortedEntities) {
      auto pos = scene->r.get<PositionComponent>(e);
      auto spr = scene->r.get<SpriteComponent>(e);

      Texture* texture = TextureManager::GetTexture(spr.filename);
      SDL_Rect clip = {
        spr.xIndex * spr.width,
        spr.yIndex * spr.height,
        spr.width,
        spr.height,
      };

      texture->render(scene->renderer, pos.x, pos.y, spr.width * spr.scale, spr.height * spr.scale, &clip);
    }
  }
};