cmake_minimum_required(VERSION 3.10)

enable_language(C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# for info/warning/assert messages
add_definitions(-DBR_DEBUG)

# add flags for compiler
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    message(WARNING "Compiler = Clang")
    add_compile_options(-fexceptions -frtti -pthread -fPIC)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    message(WARNING "Compiler = GCC")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    # using Intel C++
    message(WARNING "Compiler = Intel")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    message(WARNING "Compiler = Visual Studio")
endif()

add_subdirectory(libs/SDL2)
add_subdirectory(libs/SDL2_image)
add_subdirectory(libs/SDL2_mixer)
add_subdirectory(libs/SDL2_net)
add_subdirectory(libs/imgui)
add_subdirectory(libs/assimp)
add_subdirectory(libs/sqlite)
add_subdirectory(libs/bullet)

set(AndroidGLES_SRC
        src/Beryll/Platform/AndroidGLES/AndroidGLESWindow.cpp
        src/Beryll/Platform/AndroidGLES/AndroidGLESImGUI.cpp
        src/Beryll/Platform/AndroidGLES/AndroidGLESBuffer.cpp
        src/Beryll/Platform/AndroidGLES/AndroidGLESVertexArray.cpp
        src/Beryll/Platform/AndroidGLES/AndroidGLESShader.cpp
        src/Beryll/Platform/AndroidGLES/AndroidGLESTexture.cpp
)

set(Common_SRC
        src/Beryll/Core/GameStateMachine.cpp
        src/Beryll/Core/LayerStack.cpp
        src/Beryll/Core/GameLoop.cpp
        src/Beryll/Core/Window.cpp
        src/Beryll/Core/EventHandler.cpp
        src/Beryll/Core/SoundsManager.cpp
        src/Beryll/Core/TimeStep.cpp
        src/Beryll/Core/RandomGenerator.cpp

        src/Beryll/GameObjects/SimpleObject.cpp
        src/Beryll/GameObjects/AnimatedObject.cpp
        src/Beryll/GameObjects/CollidingSimpleObject.cpp
        src/Beryll/GameObjects/CollidingAnimatedObject.cpp

        src/Beryll/GUI/MainImGUI.cpp
        src/Beryll/GUI/DemoImGUI.cpp
        src/Beryll/GUI/Button.cpp
        src/Beryll/GUI/CheckBox.cpp
        src/Beryll/GUI/Text.cpp
        src/Beryll/GUI/DrawAnyFunction.cpp
        src/Beryll/GUI/Slider.cpp

        src/Beryll/Renderer/Camera.cpp
        src/Beryll/Renderer/Renderer.cpp

        src/Beryll/Physics/Physics.cpp
)

add_library(Beryll-static STATIC ${AndroidGLES_SRC} ${Common_SRC})

# add these lines to main CmakeLists.txt
#include_directories(Beryll_engine/libs)
#include_directories(Beryll_engine/src)

target_link_libraries(Beryll-static #target

                      SDL2-static # libs
                      SDL2_image-static
                      SDL2_mixer-static
                      SDL2_net-static
                      ImGUI-static
                      assimp-static
                      sqlite3-static
                      bullet-static)