cmake_minimum_required(VERSION 3.23)
project(cppGameDev)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

file(GLOB_RECURSE CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/*.cpp")

# Add directory containing print.h
include_directories(${PROJECT_SOURCE_DIR}/path/to/print/header)

# Or if print.h is in the same directory as your source files
include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

set(SDL2_PATH "SDL2-2.30.5\\x86_64-w64-mingw32")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

set(SDL2_MIXER_PATH "SDL2_mixer-2.8.0\\x86_64-w64-mingw32")
find_package(SDL2_mixer REQUIRED)
include_directories(${SDL2_MIXER_INCLUDE_DIRS})

include_directories(box2d-3.0.0/include)  # Include the Box2D headers
add_subdirectory(box2d-3.0.0)


add_executable(Game main.cpp
        Scene/Engine/Scene.cpp
        Scene/Engine/Scene.h
        Scene/Engine/Entity.h
        Scene/Engine/Systems.h
        Scene/Engine/Components.h
        Scene/Engine/Game.h
        Scene/Engine/Game.cpp
        Scene/DemoGame/DemoGame.h
        Scene/Engine/Components.h
        Scene/DemoGame/Collision.h
        Scene/Engine/Graphics/Texture.h
        Scene/Engine/Graphics/Texture.cpp
        Scene/Engine/Graphics/TextureManager.cpp
        Scene/DemoGame/Tilemap.h
        Scene/DemoGame/Tilemap.h
        Scene/DemoGame/Sprites.h
        Scene/DemoGame/Sprites.h
        Scene/DemoGame/Backgrounds.h
        Scene/DemoGame/Backgrounds.h
        Scene/DemoGame/Player.h
        Scene/DemoGame/Colliders.h
        Scene/DemoGame/Sound.h

)
target_link_libraries(Game ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARIES} box2d -lmingw32)
