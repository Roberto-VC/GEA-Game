#pragma once
#include <entt/src/entt/entt.hpp>
#include <utility>
#include "Scene.h"
#include <iostream>
#include "Components.h"

class Entity {
public:
    Entity(entt::entity h, Scene* s)
      : handle(h), scene(s)
    {
        std::cout << "Entity Created\n" << std::endl;
    }

    ~Entity() {
        scene->r.destroy(handle);
        std::cout << ("Enttiy Destroyed\n") <<std::endl;
    }

    template<typename T>
    void addComponent(auto&&... args) {
        scene->r.emplace<T>(handle, std::forward<decltype(args)>(args)...);
    }

private:
    entt::entity handle;
    Scene* scene;
};