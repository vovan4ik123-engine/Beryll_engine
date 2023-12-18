#include "GameLoop.h"
#include "beryll/core/Log.h"
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

namespace Beryll
{
    bool GameLoop::m_isRun = true;
    bool GameLoop::needDraw = true;

    float GameLoop::m_loopTime = 0.0f;
    float GameLoop::m_frameStart = 0.0f;
    float GameLoop::m_frameTime = 0.0f;
    float GameLoop::m_frameTimeIncludeSleep = 0.0f;
    float GameLoop::m_CPUTime = 0.0f;
    float GameLoop::m_GPUTimeStart = 0.0f;
    float GameLoop::m_GPUTime = 0.0f;
    Timer GameLoop::m_timer;

    float GameLoop::m_FPS = 500.0f;
    float GameLoop::m_FPSMaxLimit = 500.0f;

    int GameLoop::m_regulateFPSFramesCount = 0;
    float GameLoop::m_regulateFPSFramesSum = 0.0f;

    void GameLoop::create(ScreenOrientation orientation)
    {
        Window::setScreenOrientation(orientation);
        Window::create();
        Window::getInstance()->setClearColor(0.5f, 0.0f, 0.0f, 1.0f);
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
    }

    void GameLoop::run()
    {
        BR_INFO("%s", "GameLoop started.");

        m_loopTime = 1000000.0f / m_FPS; // MicroSec.
        Physics::resetTimer();
        m_isRun = true; // Can be false before this method call.

        while(m_isRun)
        {
            TimeStep::fixateTime(); // Fixate time of last finished frame.
            m_timer.reset();
            m_frameStart = m_timer.getElapsedMicroSec();

            regulateFPS();

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

            if(m_frameTime < m_loopTime)
            {
                // Sleep if we finished work faster than m_loopTime.
                //BR_INFO("sleepTime: %f", (m_loopTime - m_frameTime) / 1000.0f);
                std::chrono::duration<long long, std::nano> sleepTime(static_cast<long long>((m_loopTime - m_frameTime) * 1000.0f));
                std::this_thread::sleep_for(sleepTime);
            }

            m_frameTimeIncludeSleep = m_timer.getElapsedMicroSec() - m_frameStart;
        }

        BR_INFO("%s", "GameLoop stopped.");
    }

    void GameLoop::regulateFPS()
    {
        if(m_regulateFPSFramesCount >= 4) // Regulate once per 4 frames.
        {
            float averageFrameTime = m_regulateFPSFramesSum / float(m_regulateFPSFramesCount); // In microSec.

            float newFPS = (1000000.0f / averageFrameTime) * 0.95f;

            if(newFPS >= m_FPSMaxLimit)
            {
                m_FPS = m_FPSMaxLimit;
                m_loopTime = 1000000.0f / m_FPS; // In microSec.
            }
            else if(newFPS >= m_FPSMinLimit && newFPS <= m_FPSMaxLimit)
            {
                m_FPS = newFPS;
                m_loopTime = 1000000.0f / m_FPS; // In microSec.
            }

            m_regulateFPSFramesSum = 0.0f;
            m_regulateFPSFramesCount = 0;
        }
        else
        {
            // Some very long operation can happen in frame (like change game state with game loading).
            // Avoid frame if its time > 0.5 sec to avoid drop FPS.
            if(m_frameTime > 500000.0f)
                return;

            m_regulateFPSFramesSum += m_frameTime;
            ++m_regulateFPSFramesCount;
        }
    }
}
