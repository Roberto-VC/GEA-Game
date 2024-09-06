#pragma once
#include <string>
#include "Scene/DemoGame/Sprites.h"
#include "Scene/Engine/Entity.h"
#include "Scene/Engine/Systems.h"
#include "Scene/Engine/Components.h"
#include <iostream>

struct TilemapComponent {
  std::string filename;
  std::vector<std::vector<int>> map;
  int tileSize;
  int scale;
};

class TilemapSetupSystem : public SetupSystem {
public:
  void run() override {
    std::vector<std::vector<int>> map = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
      {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
      {0, 0, 0, 0, 0, 1, 1, 1, 0, 0},
      {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
      {0, 0, 0, 1, 0, 1, 0, 0, 1, 0},
      {0, 0, 0, 1, 1, 1, 0, 0, 1, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    std::string filename = "D:/Temp/Game/Assets/Tilesets/large.bmp";
    Entity* tilemap = scene->createEntity("TILEMAP");
    tilemap->addComponent<TilemapComponent>(
      "D:/Temp/Game/Assets/Tilesets/large.bmp",
      map,
      8,
      8
    );
    tilemap->addComponent<TextureComponent>("D:/Temp/Game/Assets/Tilesets/large.bmp");
  }
};

class AutoTilingSetupSystem : public SetupSystem {
private:
  bool isTile(std::vector<std::vector<int>>& map, int x, int y) {
    return x >= 0 && x < map[0].size() && y >= 0 && y < map.size() && map[y][x] == 1;
  }

public:
  void run() override {
    auto view = scene->r.view<TilemapComponent>();
    for (auto e : view) {
      auto& tmap = view.get<TilemapComponent>(e);
      int tilemapHeight = tmap.map.size();
      int tilemapWidth = tmap.map[0].size();
      std::vector<std::vector<int>> map = tmap.map;

      for (int y = 0; y < tilemapHeight; y++) {
        for (int x = 0; x < tilemapWidth; x++) {
          if (tmap.map[y][x] == 1) {
            std::cout << "tmap.map[y][x]: " << tmap.map[y][x] << std::endl;
            std::cout << "x: " << x << std::endl;
            std::cout << "y: " << y << std::endl;
            bool north = isTile(tmap.map, x, y - 1);
            bool south = isTile(tmap.map, x, y + 1);
            bool west = isTile(tmap.map, x - 1, y);
            bool east = isTile(tmap.map, x + 1, y);

            int mask = 0;
            if (north) {
              mask |= 1;
            }
            if (west) {
              mask |= 2;
            }
            if (east) {
              mask |= 4;
            }
            if (south) {
              mask |= 8;
            }
            std::cout << "mask: " << mask << std::endl << std::endl;
            map[y][x] = mask;

          } else {
            map[y][x] = -1;
          }
        }
      }
      tmap.map = map;
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


  bool isTile(const std::vector<std::vector<int>>& map, int x, int y) {
    return (x >= 0 && x < map[0].size() && y >= 0 && y < map.size() && map[y][x] == 1);
  }

public:
  void run() override {
    auto view = scene->r.view<TilemapComponent>();

    for (auto entity : view) {
      auto& tilemap = view.get<TilemapComponent>(entity);

      const size_t mapHeight = tilemap.map.size();
      const size_t mapWidth = tilemap.map[0].size();

      std::vector<std::vector<int>> newMap = tilemap.map;

      for (size_t y = 0; y < mapHeight; ++y) {
        for (size_t x = 0; x < mapWidth; ++x) {
          if (tilemap.map[y][x] == 1) {
            int mask = 0;

            bool north = isTile(tilemap.map, x, y-1);
            bool south = isTile(tilemap.map, x, y+1);
            bool west = isTile(tilemap.map, x-1, y);
            bool east = isTile(tilemap.map, x+1, y);

            // Cardinal directions
            if (north) mask |= 2;
            if (west) mask |= 8;
            if (east) mask |= 16;
            if (south) mask |= 64;

            // Corners (with redundancy check)
            if (north && west && isTile(tilemap.map, x-1, y-1)) mask |= 1;
            if (north && east && isTile(tilemap.map, x+1, y-1)) mask |= 4;
            if (south && west && isTile(tilemap.map, x-1, y+1)) mask |= 32;
            if (south && east && isTile(tilemap.map, x+1, y+1)) mask |= 128;

            // Map the mask to the tile index
            auto it = maskToTileIndex.find(mask);
            if (it != maskToTileIndex.end()) {
              newMap[y][x] = it->second;
            } else {
              // If the mask doesn't have a mapping, use a default tile
              newMap[y][x] = 47;  // Assuming 47 is your default tile
            }
          } else {
            newMap[y][x] = -1;  // Empty space
          }
        }
      }

      tilemap.map = newMap;
    }
  }
};

class TilemapRenderSystem : public RenderSystem {
  void run(SDL_Renderer* renderer) {
    auto view = scene->r.view<TilemapComponent, TextureComponent>();
    for (auto e : view) {

      auto tmap = view.get<TilemapComponent>(e);
      SDL_Log("No Error");
      auto tex = view.get<TextureComponent>(e);
      SDL_Log("Error");

      Texture* texture = TextureManager::GetTexture(tex.filename);
      SDL_Log("Error textura");
      SDL_Log(tex.filename.c_str());

      int tileSize = tmap.tileSize * tmap.scale;
      int tilemapHeight = tmap.map.size();
      int tilemapWidth = tmap.map[0].size();

      for (int y = 0; y < tilemapHeight; y++) {
        for (int x = 0; x < tilemapWidth; x++) {
          int tileIndex = tmap.map[y][x];
          SDL_Log("1");

          if (tileIndex >= 0) {
            int tileIndexX = tileIndex % 8;
            int tileIndexY = tileIndex / 8;

            SDL_Rect clip = {
              tileIndexX * tmap.tileSize,
              tileIndexY * tmap.tileSize,
              tmap.tileSize,
              tmap.tileSize,
            };
            SDL_Log("2");

            texture->render(
              scene->renderer,
              x * tileSize,
              y * tileSize,
              tileSize,
              tileSize,
              &clip
            );
            SDL_Log("3");
          }
        }
      }
    }
  }
}; 