#pragma once
#include <string>
#include "Scene/Engine/Systems.h"
#include "Scene/Engine/Components.h"
#include "Scene/Engine/Graphics/TextureManager.h"
#include "Sprites.h"
#include <time.h>
#include <random>
#include <chrono>



struct BackgroundComponent {
  std::string filename;
};



class BackgroundSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* background = scene->createEntity("BACKGROUND");

    Entity* background2 = scene->createEntity("BACKGROUND");
    const std::string& bgfile2 = "../Assets/Backgrounds/Blue.bmp";
    background2->addComponent<TextureComponent>(bgfile2);
    background2->addComponent<BackgroundComponent>(bgfile2);
    background2->addComponent<LayerComponent>(0);
    const std::string& bgfile = "../Assets/Backgrounds/Red.bmp";
    background->addComponent<TextureComponent>(bgfile);
    background->addComponent<BackgroundComponent>(bgfile);
    background->addComponent<LayerComponent>(0);

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