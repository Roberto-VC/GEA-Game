
#pragma once
#include "Scene.h"
#include <SDL.h>

class System {
protected:
    Scene* scene;

public:
    void setScene(Scene* s) { scene = s; };
};

class SetupSystem : public System {
public:
    virtual void run() = 0;
};

class UpdateSystem : public System {
public:
    virtual void run(float dt) = 0;
};

class EventSystem : public System {
public:
    virtual void run(SDL_Event e) = 0;
};

class RenderSystem : public System {
public:
    virtual void run(SDL_Renderer* renderer) = 0;
};