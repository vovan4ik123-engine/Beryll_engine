#pragma once

// include this file only outside of engine
// inside engine include files directly (Beryll/Core/....)
// add Beryll_engine/src to includes path to Cmake

#include "Beryll/Core/GameLoop.h"
#include "Beryll/Core/Layer.h"
#include "Beryll/Core/GameState.h"
#include "Beryll/Core/GameStateMachine.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Core/EventHandler.h"
#include "Beryll/Core/SoundsManager.h"
#include "Beryll/Core/Window.h"
#include "Beryll/Core/RandomGenerator.h"
#include "Beryll/Core/TimeStep.h"

#include "Beryll/Renderer/Camera.h"
#include "Beryll/Renderer/SkyBox.h"
#include "Beryll/Renderer/Renderer.h"

#include "Beryll/GameObjects/BaseSimpleObject.h"
#include "Beryll/GameObjects/BaseAnimatedObject.h"
#include "Beryll/GameObjects/SimpleObject.h"
#include "Beryll/GameObjects/CollidingSimpleObject.h"
#include "Beryll/GameObjects/AnimatedObject.h"
#include "Beryll/GameObjects/CollidingAnimatedObject.h"

#include "Beryll/GameObjects/Player/CollidingSimplePlayer.h"

#include "Beryll/GUI/MainImGUI.h"
#include "Beryll/GUI/DemoImGUI.h"
#include "Beryll/GUI/Button.h"
#include "Beryll/GUI/CheckBox.h"
#include "Beryll/GUI/Text.h"
#include "Beryll/GUI/DrawAnyFunction.h"
#include "Beryll/GUI/Slider.h"

#include "Beryll/Physics/Physics.h"

#include "Beryll/Async/AsyncRun.h"

#include "Beryll/ParticleSystem/ParticleSystem.h"

#include "Beryll/LoadingScreen/LoadingScreen.h"