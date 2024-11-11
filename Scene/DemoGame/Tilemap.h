#pragma once
#include <string>
#include "Sprites.h"
#include "Scene/Engine/Entity.h"
#include "Scene/Engine/Systems.h"
#include <iostream>
#include <time.h>
#include <random>
#include <chrono>

enum class TileType {
  NONE,
  WALL,
  TRIGGER,
};

struct Tile {
  int index;
  int tilemapIndex;
  TileType type;
};

struct TileComponent {
  Tile tile;
};

struct TilemapComponent {
  std::string filename;
  std::vector<Tile> tiles;
  int tileSize;
  int scale;
  int width;
  int height;
};

class TilemapSetupSystem : public SetupSystem {
public:
  void run() override {
    std::vector<int> initialMap = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    std::string filename = "../Assets/Tilesets/tilered.bmp";
    std::string filename2 = "../Assets/Tilesets/bluetile.bmp";
    int tileSize = 8;
    int tileScale = 8;
    std::vector<Tile> tiles;
    for (int i = 0; i < initialMap.size(); i++) {
      TileType type = TileType::NONE;
      switch(initialMap[i]) {
        case 1:
          type = TileType::WALL;
          break;
        case 0:
          break;
      }
      Tile tile = Tile{i, 0, type};
      tiles.push_back(tile);
    }

    Entity* tilemapEntity = scene->createEntity("TILEMAP");
    tilemapEntity->addComponent<TilemapComponent>(
      filename,
      tiles,
      tileSize,
      tileScale,
      12,
      12
    );
    tilemapEntity->addComponent<TextureComponent>("../Assets/Tilesets/tilered.bmp");

    Entity* tilemapEntity2 = scene->createEntity("TILEMAP2");
    tilemapEntity2->addComponent<TilemapComponent>(
  filename2,
  tiles,
  tileSize,
  tileScale,
  12,
  12
);

    tilemapEntity2->addComponent<TextureComponent>("../Assets/Tilesets/bluetile.bmp");
    Entity* background = scene->createEntity("BACKGROUND");

    Entity* background2 = scene->createEntity("BACKGROUND");
    const std::string& bgfile2 = "../Assets/Backgrounds/Red.bmp";
    background2->addComponent<TextureComponent>(bgfile2);
    background2->addComponent<BackgroundComponent>(bgfile2);
    background2->addComponent<LayerComponent>(0);
    const std::string& bgfile = "../Assets/Backgrounds/Blue.bmp";
    background->addComponent<TextureComponent>(bgfile);
    background->addComponent<BackgroundComponent>(bgfile);
    background->addComponent<LayerComponent>(0);
  }

};


class AutoTilingSetupSystem : public SetupSystem {
private:
  bool isTile(const std::vector<Tile>& map, int x, int y, int w, int h) {
    return (x >= 0 && x < w && y >= 0 && y < h && map[y * w + x].type == TileType::WALL);
  }

public:
  void run() override {
    auto view = scene->r.view<TilemapComponent>();

    for (auto e : view) {
      auto& tmap = view.get<TilemapComponent>(e);
      const int mapWidth = tmap.width;
      const int mapHeight = tmap.height;

      std::vector<Tile> newMap = tmap.tiles;

      for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
          if (tmap.tiles[y * mapWidth + x].type == TileType::WALL) {
            int mask = 0;

            // Check neighboring tiles
            bool north = isTile(tmap.tiles, x, y - 1, mapWidth, mapHeight);
            bool south = isTile(tmap.tiles, x, y + 1, mapWidth, mapHeight);
            bool west = isTile(tmap.tiles, x - 1, y, mapWidth, mapHeight);
            bool east = isTile(tmap.tiles, x + 1, y, mapWidth, mapHeight);

            // Set the bits for north, south, east, west
            if (north) mask |= 1;   // North
            if (west)  mask |= 2;   // West
            if (east)  mask |= 4;   // East
            if (south) mask |= 8;   // South

            // Debugging output to see mask value
            std::cout << "Mask for (" << x << ", " << y << "): " << mask << std::endl;

            // Map the mask to the tile index
            newMap[y * mapWidth + x].tilemapIndex = mask;  // Adjust this based on your own mapping logic
          } else {
            newMap[y * mapWidth + x].tilemapIndex = -1;  // Or some default tile index for non-wall tiles
          }
        }
      }

      tmap.tiles = newMap;
    }
  }
};


class AdvancedAutoTilingSetupSystem : public SetupSystem {
private:
  std::unordered_map<int, int> maskToTileIndex = {
    {2, 1}, {8, 2}, {10, 3}, {11, 4}, {16, 5}, {18, 6}, {22, 7}, {24, 8},
    {26, 9}, {27, 10}, {30, 11}, {31, 12}, {64, 13}, {66, 14}, {72, 15},
    {74, 16}, {75, 17}, {80, 18}, {82, 19}, {86, 20}, {88, 21}, {90, 22},
    {91, 23}, {94, 24}, {95, 25}, {104, 26}, {106, 27}, {107, 28}, {120, 29},
    {122, 30}, {123, 31}, {126, 32}, {127, 33}, {208, 34}, {210, 35},
    {214, 36}, {216, 37}, {218, 38}, {219, 39}, {222, 40}, {223, 41},
    {248, 42}, {250, 43}, {251, 44}, {254, 45}, {255, 46}, {0, 47}
  };


  bool isTile(const std::vector<Tile>& map, int x, int y, int w, int h) {
    return (x >= 0 && x < w && y >= 0 && y < h && map[y * w + x].type == TileType::WALL);
  }

public:
  void run() override {
    auto view = scene->r.view<TilemapComponent>();

    for (auto entity : view) {
      auto& tilemap = view.get<TilemapComponent>(entity);

      const int mapHeight = tilemap.width;
      const int mapWidth = tilemap.height;

      std::vector<Tile> newMap = tilemap.tiles;

      for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
          if (tilemap.tiles[y * mapWidth + x].type == TileType::WALL) {
            int mask = 0;

            bool north = isTile(tilemap.tiles, x, y-1, tilemap.width, tilemap.height);
            bool south = isTile(tilemap.tiles, x, y+1, tilemap.width, tilemap.height);
            bool west = isTile(tilemap.tiles, x-1, y, tilemap.width, tilemap.height);
            bool east = isTile(tilemap.tiles, x+1, y, tilemap.width, tilemap.height);

            // Cardinal directions
            if (north) mask |= 2;
            if (west) mask |= 8;
            if (east) mask |= 16;
            if (south) mask |= 64;

            // Corners (with redundancy check)
            if (north && west && isTile(tilemap.tiles, x-1, y-1, tilemap.width, tilemap.height)) mask |= 1;
            if (north && east && isTile(tilemap.tiles, x+1, y-1, tilemap.width, tilemap.height)) mask |= 4;
            if (south && west && isTile(tilemap.tiles, x-1, y+1, tilemap.width, tilemap.height)) mask |= 32;
            if (south && east && isTile(tilemap.tiles, x+1, y+1, tilemap.width, tilemap.height)) mask |= 128;

            // Map the mask to the tile index
            auto it = maskToTileIndex.find(mask);
            if (it != maskToTileIndex.end()) {
              newMap[y * mapWidth + x].tilemapIndex = it->second;
            } else {
              // If the mask doesn't have a mapping, use a default tile
              newMap[y * mapWidth + x].tilemapIndex = 47;
            }
          } else {
            newMap[y * mapWidth + x].tilemapIndex = -1;
          }
        }
      }

      tilemap.tiles = newMap;
    }
  }
};

class TilemapRenderSystem : public RenderSystem {
private:
  std::chrono::steady_clock::time_point lastSwitchTime;
  std::chrono::seconds switchInterval;
  std::chrono::milliseconds blinkInterval;
  std::mt19937 rng;
  std::uniform_int_distribution<size_t> dist;
  TilemapComponent* selectedComponent = nullptr;
  BackgroundComponent* bselectedComponent = nullptr;
  bool isBlinking;
  std::chrono::steady_clock::time_point blinkStartTime;

public:
  TilemapRenderSystem()
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

    if (now - lastSwitchTime > switchInterval) {
      lastSwitchTime = now;
      auto view = scene->r.view<TilemapComponent>();
      std::vector<TilemapComponent*> components;
      std::vector<TilemapComponent*> components1;

      for (auto e : view) {
        components.push_back(&view.get<TilemapComponent>(e));
      }

      auto view2 = scene->r.view<BackgroundComponent>();

      std::vector<BackgroundComponent*> bcomponents;
      for (auto e : view2) {
        bcomponents.push_back(&view2.get<BackgroundComponent>(e));
      }

      unsigned long long indexx;
      if (!components.empty()) {
        // Set up the random number generator to select an index
        dist.param(std::uniform_int_distribution<size_t>::param_type(0, components.size() - 1));
        auto selectedIndex = dist(rng);
        indexx = selectedIndex;
        selectedComponent = components[selectedIndex];

        // Start blinking effect
        isBlinking = true;
        blinkStartTime = now;
      }

      if (!bcomponents.empty()) {
        // Set up the random number generator to select an index

        bselectedComponent = bcomponents[indexx];

        // Start blinking effect
        isBlinking = true;
        blinkStartTime = now;
      }
    }


    auto elapsedBlinkTime = now - blinkStartTime;
    bool shouldBlink = isBlinking && (elapsedBlinkTime < blinkInterval);

    if (shouldBlink) {
      // Render a blank screen or a color to indicate blinking
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White or any color
      SDL_RenderClear(renderer);
    } else {
      // Render the selected background component
      if(bselectedComponent) {
        auto texture = TextureManager::GetTexture(bselectedComponent->filename);
        texture->render(renderer, 0, 0);  // Adjust position as needed
      }

      if (selectedComponent) {

        auto texture = TextureManager::GetTexture(selectedComponent->filename);
        auto tmap = selectedComponent;




          int tileSize = tmap->tileSize * tmap->scale;
          int tilemapHeight = tmap->height;
          int tilemapWidth = tmap->width;


          for (int y = 0; y < tilemapHeight; y++) {
            for (int x = 0; x < tilemapWidth; x++) {
              int tileIndex = tmap->tiles[y * tilemapWidth + x].tilemapIndex;

              if (tileIndex >= 0) {
                int tileIndexX = tileIndex % 4;
                int tileIndexY = tileIndex / 4;

                SDL_Rect clip = {
                  tileIndexX * tmap->tileSize,
                  tileIndexY * tmap->tileSize,
                  tmap->tileSize,
                  tmap->tileSize,
                };

                texture->render(
                  scene->renderer,
                  x * tileSize,
                  y * tileSize,
                  tileSize,
                  tileSize,
                  &clip
                );
              }
            }
          }

      }
      }

      // Reset blinking state after rendering the new background
      if (isBlinking && elapsedBlinkTime >= blinkInterval) {
        isBlinking = false;
      }


  }
};