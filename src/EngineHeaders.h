#pragma once

// include this file only outside of engine
// inside engine include files directly (beryll/core/....)
// add Beryll_engine/src to includes path to Cmake

#include "beryll/core/GameLoop.h"
#include "beryll/core/Layer.h"
#include "beryll/core/GameState.h"
#include "beryll/core/GameStateMachine.h"
#include "beryll/core/Log.h"
#include "beryll/core/EventHandler.h"
#include "beryll/core/SoundsManager.h"
#include "beryll/core/Window.h"
#include "beryll/core/RandomGenerator.h"
#include "beryll/core/TimeStep.h"

#include "beryll/renderer/Camera.h"
#include "beryll/renderer/SkyBox.h"
#include "beryll/renderer/Renderer.h"

#include "beryll/gameObjects/BaseSimpleObject.h"
#include "beryll/gameObjects/BaseAnimatedObject.h"
#include "beryll/gameObjects/SimpleObject.h"
#include "beryll/gameObjects/SimpleCollidingObject.h"
#include "beryll/gameObjects/AnimatedObject.h"
#include "beryll/gameObjects/AnimatedCollidingObject.h"

#include "beryll/gameObjects/characters/SimpleCollidingCharacter.h"
#include "beryll/gameObjects/characters/AnimatedCollidingCharacter.h"

#include "beryll/gameObjects/banners/BannerProgressTwoColors.h"

#include "beryll/GUI/MainImGUI.h"
#include "beryll/GUI/DemoImGUI.h"
#include "beryll/GUI/ButtonWithText.h"
#include "beryll/GUI/ButtonWithTexture.h"
#include "beryll/GUI/GUITexture.h"
#include "beryll/GUI/Joystick.h"
#include "beryll/GUI/CheckBox.h"
#include "beryll/GUI/Text.h"
#include "beryll/GUI/Slider.h"

#include "beryll/physics/Physics.h"

#include "beryll/async/AsyncRun.h"

#include "beryll/particleSystem/ParticleSystem.h"

#include "beryll/loadingScreen/LoadingScreen.h"