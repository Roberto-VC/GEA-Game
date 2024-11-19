#pragma once
#include <string>
#include <entt/src/entt/entt.hpp>
#include <SDL.h>

class Entity;
class SetupSystem;  // loading
class EventSystem;  // for events + keyboard
class RenderSystem;  // render
class UpdateSystem;  // updates variables

class Scene {
public:
    std::vector<SetupSystem*> setupSystems;
    std::vector<EventSystem*> eventSystems;
    std::vector<RenderSystem*> renderSystems;
    std::vector<UpdateSystem*> updateSystems;

    Scene(const std::string&, entt::registry&, SDL_Renderer*);
    ~Scene();

    Entity* createEntity(const std::string&);
    Entity* createEntity(const std::string&,int,int);
    Entity* createEntity(const std::string&,int,int, bool);

    void setup();
    void update(float dT);
    void render(SDL_Renderer* renderer);
    void processEvents(SDL_Event e);

    entt::registry& r;
    std::string name;
    SDL_Renderer* renderer;



};