cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_language(C CXX ASM)

add_compile_options(-fexceptions -frtti -fPIC) # -fopenmp -pthread -fexceptions -frtti -fPIC

# for info/warning/assert messages
add_definitions(-DBR_DEBUG)

# add flags for specyfic compiler
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(WARNING "Compiler = Clang")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(WARNING "Compiler = GCC")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    message(WARNING "Compiler = Intel")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
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

if(ANDROID)
    set(AndroidGLES_SRC
            src/Beryll/Platform/AndroidGLES/AndroidGLESWindow.cpp
            src/Beryll/Platform/AndroidGLES/AndroidGLESImGUI.cpp
            src/Beryll/Platform/AndroidGLES/AndroidGLESBuffer.cpp
            src/Beryll/Platform/AndroidGLES/AndroidGLESVertexArray.cpp
            src/Beryll/Platform/AndroidGLES/AndroidGLESShader.cpp
            src/Beryll/Platform/AndroidGLES/AndroidGLESTexture.cpp
            src/Beryll/Platform/AndroidGLES/AndroidGLESSkyBox.cpp
            src/Beryll/Platform/AndroidGLES/AndroidGLESShadowMapTexture.cpp
            src/Beryll/Platform/AndroidGLES/AndroidGLESGlobal.cpp
            )
elseif(APPLE)
    #set(IOSMetal_SRC src/Beryll/Platform/...)
endif()

set(Common_SRC
        src/Beryll/Core/GameStateMachine.cpp
        src/Beryll/Core/LayerStack.cpp
        src/Beryll/Core/GameLoop.cpp
        src/Beryll/Core/Window.cpp
        src/Beryll/Core/EventHandler.cpp
        src/Beryll/Core/SoundsManager.cpp
        src/Beryll/Core/TimeStep.cpp
        src/Beryll/Core/RandomGenerator.cpp

        src/Beryll/Utils/CommonUtils.cpp

        src/Beryll/GameObjects/SimpleObject.cpp
        src/Beryll/GameObjects/AnimatedObject.cpp
        src/Beryll/GameObjects/CollidingSimpleObject.cpp
        src/Beryll/GameObjects/CollidingAnimatedObject.cpp

        src/Beryll/GameObjects/Player/CollidingSimplePlayer.cpp

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

        src/Beryll/Async/AsyncRun.cpp
)

add_library(Beryll-static STATIC ${AndroidGLES_SRC} ${IOSMetal_SRC} ${Common_SRC})

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