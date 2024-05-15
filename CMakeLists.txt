cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_language(C CXX ASM)

add_compile_options(-fexceptions -frtti -fPIC) # -fopenmp -pthread -fexceptions -frtti -fPIC

# for info/warning/assert messages
    add_definitions(-DBR_DEBUG)

# add flags for specific compiler
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
            src/beryll/platform/androidGLES/AndroidGLESWindow.cpp
            src/beryll/platform/androidGLES/AndroidGLESImGUI.cpp
            src/beryll/platform/androidGLES/AndroidGLESBuffer.cpp
            src/beryll/platform/androidGLES/AndroidGLESVertexArray.cpp
            src/beryll/platform/androidGLES/AndroidGLESShader.cpp
            src/beryll/platform/androidGLES/AndroidGLESTexture.cpp
            src/beryll/platform/androidGLES/AndroidGLESSkyBox.cpp
            src/beryll/platform/androidGLES/AndroidGLESShadowMapTexture.cpp
            src/beryll/platform/androidGLES/AndroidGLESBillingSystem.cpp
            src/beryll/platform/androidGLES/AndroidGLESGoogleAnalytics.cpp
            )
elseif(APPLE)
    set(IOSMetal_SRC
            )
endif()

set(Common_SRC
        src/beryll/core/GameStateMachine.cpp
        src/beryll/core/LayerStack.cpp
        src/beryll/core/GameLoop.cpp
        src/beryll/core/Window.cpp
        src/beryll/core/EventHandler.cpp
        src/beryll/core/SoundsManager.cpp
        src/beryll/core/TimeStep.cpp
        src/beryll/core/RandomGenerator.cpp

        src/beryll/utils/CommonUtils.cpp

        src/beryll/gameObjects/SceneObject.cpp
        src/beryll/gameObjects/BaseSimpleObject.cpp
        src/beryll/gameObjects/SimpleObject.cpp
        src/beryll/gameObjects/BaseAnimatedObject.cpp
        src/beryll/gameObjects/AnimatedObject.cpp
        src/beryll/gameObjects/SimpleCollidingObject.cpp
        src/beryll/gameObjects/AnimatedCollidingObject.cpp

        src/beryll/gameObjects/characters/SimpleCollidingCharacter.cpp
        src/beryll/gameObjects/characters/AnimatedCollidingCharacter.cpp
        src/beryll/gameObjects/characters/CharacterController.cpp

        src/beryll/gameObjects/banners/BannerProgressTwoColors.cpp

        src/beryll/GUI/MainImGUI.cpp
        src/beryll/GUI/DemoImGUI.cpp
        src/beryll/GUI/ButtonWithText.cpp
        src/beryll/GUI/ButtonWithTexture.cpp
        src/beryll/GUI/GUITexture.cpp
        src/beryll/GUI/Joystick.cpp
        src/beryll/GUI/CheckBox.cpp
        src/beryll/GUI/Text.cpp
        src/beryll/GUI/SliderHorizontal.cpp
        src/beryll/GUI/SliderVertical.cpp
        src/beryll/GUI/ProgressBar.cpp

        src/beryll/dataBase/DataBase.cpp

        src/beryll/renderer/Camera.cpp
        src/beryll/renderer/Renderer.cpp

        src/beryll/physics/Physics.cpp

        src/beryll/async/AsyncRun.cpp

        src/beryll/particleSystem/ParticleSystem.cpp

        src/beryll/loadingScreen/LoadingScreen.cpp

        src/beryll/billingSystem/BillingSystem.cpp

        src/beryll/googleAnalytics/GoogleAnalytics.cpp

        src/beryll/network/Network.cpp
)

if(ANDROID)
    add_library(Beryll-static STATIC ${AndroidGLES_SRC} ${Common_SRC})
elseif(APPLE)
    add_library(Beryll-static STATIC ${IOSMetal_SRC} ${Common_SRC})
endif()

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
