#pragma once
#include <string>

struct NameComponent {
  const std::string name;
};

struct PositionComponent {
  int x;
  int y;
};

struct ActiveComponent {
  bool active;
};

struct VelocityComponent {
  int speed;
  float x = 0;
  float y = 0;
};



