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

namespace Beryll
{
    bool GameLoop::m_isRun = true;

    float GameLoop::m_loopTime = 0.0f;
    float GameLoop::m_frameStart = 0.0f;
    float GameLoop::m_frameTime = 0.0f;
    float GameLoop::m_frameTimeIncludeSleep = 0.0f;
    float GameLoop::m_CPUTime = 0.0f;
    float GameLoop::m_GPUTimeStart = 0.0f;
    float GameLoop::m_GPUTime = 0.0f;
    Timer GameLoop::m_timer;

    float GameLoop::m_maxFPS = 250.0f;
    bool GameLoop::m_regulateFPS = true;
    int GameLoop::m_regulateFPSFramesCount = 0;
    float GameLoop::m_regulateFPSFramesSum = 0.0f;

    void GameLoop::create(ScreenOrientation orientation)
    {
        Window::setScreenOrientation(orientation);
        Window::create();
        Window::getInstance()->setClearColor(0.8f, 0.0f, 0.8f, 1.0f);
        Window::getInstance()->clear();
        Window::getInstance()->swapWindow();

        MainImGUI::create();

        SoundsManager::create();

        Physics::create();

        ParticleSystem::create();

        LoadingScreen::create();
    }

    void GameLoop::run()
    {
        BR_INFO("%s", "GameLoop started");

        m_loopTime = 1000000.0f / m_maxFPS; // microSec
        m_timer.reset();
        Physics::enableSimulation(); // also reset timer inside Physics

        Window::getInstance()->reCreate();
        MainImGUI::getInstance()->reCreate();

        while(m_isRun)
        {
            TimeStep::fixateTime(); // fixate time of last finished game loop
            m_timer.reset();
            m_frameStart = m_timer.getElapsedMicroSec();

            if(m_regulateFPS) { regulateFPS(); }

        // Check user input
            EventHandler::resetEvents(EventID::ALL_EVENTS);
            EventHandler::loadEvents();

        // Update layers start
            // first react to user input, set positions of objects, move objects: player->move()
            // then update objects (let themselves prepare to simulation): GameObject->updateBeforePhysics();
            GameStateMachine::updateBeforePhysics();

            Physics::simulate();

            // read positions of objects after simulation, resolve collisions here
            // prefer update camera properties here
            GameStateMachine::updateAfterPhysics();
        // Update layers finish

        // Update camera (immediately before draw)
            // Dont set any camera attributes after this call (set in updateAfterPhysics())
            Camera::update3DCamera();

            m_CPUTime = m_timer.getElapsedMicroSec() - m_frameStart;
            m_GPUTimeStart = m_timer.getElapsedMicroSec();

        // Draw start DONT CALL ANY DRAW COMMANDS before this point !!!!!!!!
            // First finish draw previous frame
            MainImGUI::getInstance()->endFrame();
            //Window::getInstance()->finishDraw(); // very slow
            //Window::getInstance()->flushDraw(); // potentially can be called but not necessary
            Window::getInstance()->swapWindow();

            // Next start draw new frame
            Window::getInstance()->clear();
            MainImGUI::getInstance()->beginFrame();

            GameStateMachine::draw();
        // Draw finish

            m_GPUTime = m_timer.getElapsedMicroSec() - m_GPUTimeStart;

            m_frameTime = m_timer.getElapsedMicroSec() - m_frameStart;
            //BR_INFO("m_frameTime: %f", m_frameTime);

            if(m_frameTime < m_loopTime)
            {
                // sleep if we finished work faster than m_loopTime
                //BR_INFO("sleepTime: %f", (m_loopTime - m_frameTime));
                std::chrono::duration<long long, std::nano> sleepTime(static_cast<long long>((m_loopTime - m_frameTime) * 1000.0f));
                std::this_thread::sleep_for(sleepTime);
            }

            m_frameTimeIncludeSleep = m_timer.getElapsedMicroSec() - m_frameStart;
        }

        BR_INFO("%s", "GameLoop stopped");
    }

    void GameLoop::regulateFPS()
    {
        if(m_regulateFPSFramesCount >= 50) // regulate once per 50 frames
        {
            float averageFrameTime = m_regulateFPSFramesSum / 50.0f; // in microSec

            setMaxFPS((1000000.0f / averageFrameTime) * 0.98f, true);

            m_regulateFPSFramesCount = 0;
            m_regulateFPSFramesSum = 0.0f;
        }
        else
        {
            m_regulateFPSFramesSum += m_frameTime;
            ++m_regulateFPSFramesCount;
        }
    }
}