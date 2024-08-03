#include "GameLoop.h"
#include "beryll/core/TimeStep.h"
#include "beryll/core/GameStateMachine.h"
#include "beryll/core/EventHandler.h"
#include "beryll/core/SoundsManager.h"
#include "beryll/GUI/MainImGUI.h"
#include "beryll/physics/Physics.h"
#include "beryll/renderer/Camera.h"
#include "beryll/particleSystem/ParticleSystem.h"
#include "beryll/loadingScreen/LoadingScreen.h"
#include "beryll/billingSystem/BillingSystem.h"
#include "beryll/googleAnalytics/GoogleAnalytics.h"
#include "beryll/network/Network.h"
#include "beryll/ads/Ads.cpp"

namespace Beryll
{
    bool GameLoop::m_isRun = true;
    bool GameLoop::needDraw = true;

    float GameLoop::m_frameStart = 0.0f;
    float GameLoop::m_frameTime = 0.0f;
    float GameLoop::m_frameTimeIncludeSleep = 0.0f;
    float GameLoop::m_CPUTime = 0.0f;
    float GameLoop::m_GPUTimeStart = 0.0f;
    float GameLoop::m_GPUTime = 0.0f;
    Timer GameLoop::m_timer;

    float GameLoop::m_FPS = 500.0f;
    float GameLoop::m_loopTimeMicroSec = 1000000.0f / 500.0f;
    long long int GameLoop::m_sleepTimeMicroSec = 0;

    void GameLoop::create(ScreenOrientation orientation)
    {
        Window::setScreenOrientation(orientation);
        Window::create();
        Window::getInstance()->setClearColor(0.0667f, 0.6667f, 0.0f, 1.0f);
        Window::getInstance()->clear();
        Window::getInstance()->swapWindow();
        Window::getInstance()->reCreate();

        MainImGUI::create();

        SoundsManager::create();

        Physics::create();

        ParticleSystem::create();

        LoadingScreen::create();

        BillingSystem::create();

        GoogleAnalytics::create();

        Network::create();

        Ads::create();
    }

    void GameLoop::run()
    {
        BR_INFO("%s", "GameLoop started.");
        Physics::resetTimer();

        while(m_isRun)
        {
            TimeStep::fixateTime(); // Fixate time of last finished frame.
            m_timer.reset();
            m_frameStart = m_timer.getElapsedMicroSec();

        // Check user input.
            EventHandler::resetEvents(EventID::ALL_EVENTS);
            EventHandler::loadEvents();

        // Update layers start.
            // First react to user input, set positions of objects, move objects: player->move().
            // Then update objects (let themselves prepare to simulation): GameObject->updateBeforePhysics();.
            GameStateMachine::updateBeforePhysics();

            Physics::simulate();

            // Read positions of objects after simulation, resolve collisions here.
            // Prefer update camera properties here.
            GameStateMachine::updateAfterPhysics();
        // Update layers finish.

        // Update camera (immediately before draw).
            // Don't set any camera attributes after this call (set in updateAfterPhysics()).
            Camera::update3DCamera();

            m_CPUTime = m_timer.getElapsedMicroSec() - m_frameStart;
            m_GPUTimeStart = m_timer.getElapsedMicroSec();

        // Draw start.
            if(GameLoop::needDraw)
            {
                Window::getInstance()->clear();
                MainImGUI::getInstance()->beginFrame();

                GameStateMachine::draw();

                MainImGUI::getInstance()->endFrame();
                //Window::getInstance()->finishDraw(); // Very slow. Call only if you have rendering artefacts.
                //Window::getInstance()->flushDraw(); // Potentially can be called but not necessary.
                Window::getInstance()->swapWindow();
            }
        // Draw finish.

            m_GPUTime = m_timer.getElapsedMicroSec() - m_GPUTimeStart;

            m_frameTime = m_timer.getElapsedMicroSec() - m_frameStart;
            //BR_INFO("m_frameTime: %f", m_frameTime);

            m_sleepTimeMicroSec = static_cast<long long int>(m_loopTimeMicroSec - m_frameTime);
            if(m_sleepTimeMicroSec < 1500) { m_sleepTimeMicroSec = 1500; } // In any way sleep 1.5 ms.
            //BR_INFO("m_sleepTimeMicroSec: %d", int(m_sleepTimeMicroSec));
            std::this_thread::sleep_for(std::chrono::microseconds(m_sleepTimeMicroSec));

            m_frameTimeIncludeSleep = m_timer.getElapsedMicroSec() - m_frameStart;
        }

        BR_INFO("%s", "GameLoop stopped.");
    }
}
