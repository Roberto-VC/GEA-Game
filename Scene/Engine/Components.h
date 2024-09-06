#pragma once
#include <string>

struct NameComponent {
  const std::string name;
};

struct PositionComponent {
  int x;
  int y;
};

struct VelocityComponent {
  int speed;
  int x = 0;
  int y = 0;
};



