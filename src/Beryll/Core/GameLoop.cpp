#include "GameLoop.h"
#include "Beryll/Core/Log.h"
#include "Beryll/Core/TimeStep.h"
#include "Beryll/Core/Window.h"
#include "Beryll/Core/GameStateMachine.h"
#include "Beryll/Core/EventHandler.h"
#include "Beryll/Core/SoundsManager.h"
#include "Beryll/GUI/MainImGUI.h"
#include "Beryll/Physics/Physics.h"
#include "Beryll/Renderer/Camera.h"
#include "Beryll/ParticleSystem/ParticleSystem.h"
#include "Beryll/LoadingScreen/LoadingScreen.h"

namespace Beryll
{
    bool GameLoop::m_isRun = false;

    float GameLoop::m_loopTime = 0.0f;
    float GameLoop::m_frameStart = 0.0f;
    float GameLoop::m_frameTime = 0.0f;
    float GameLoop::m_CPUTimeStart = 0.0f;
    float GameLoop::m_CPUTime = 0.0f;
    float GameLoop::m_GPUTimeStart = 0.0f;
    float GameLoop::m_GPUTime = 0.0f;
    Timer GameLoop::m_timer;

    float GameLoop::m_maxFPS = 250.0f;

    void GameLoop::create()
    {
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

    float GameLoop::getFPS()
    {
        return 1000.0f / TimeStep::getTimeStepMilliSec();
    }

    void GameLoop::run()
    {
        BR_INFO("%s", "GameLoop started");

        m_isRun = true;
        m_loopTime = 1000000.0f / m_maxFPS; // microSec
        m_timer.reset();
        Physics::enableSimulation(); // also reset timer inside Physics

        while(m_isRun)
        {
            TimeStep::fixateTime(); // fixate time of last finished game loop
            m_timer.reset();
            m_frameStart = m_timer.getElapsedMicroSec();
            m_CPUTimeStart = m_timer.getElapsedMicroSec();

        // Check user input
            EventHandler::resetEvents(EventID::ALL_EVENTS);
            EventHandler::loadEvents();

        // Update layers start
            Window::getInstance()->checkOrientationChange();
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

            m_CPUTime = m_timer.getElapsedMicroSec() - m_CPUTimeStart;
            m_GPUTimeStart = m_timer.getElapsedMicroSec();

        // Draw start DONT CALL ANY DRAW COMMANDS before this point !!!!!!!!
            // First finish draw previous frame
            //Window::getInstance()->finishDraw(); // very slow
            //Window::getInstance()->flushDraw(); // potentially can be called but not necessary
            Window::getInstance()->swapWindow();

            // Next start draw new frame
            Window::getInstance()->clear();
            MainImGUI::getInstance()->beginFrame();

            GameStateMachine::draw();

            MainImGUI::getInstance()->endFrame();
        // Draw finish

            m_GPUTime = m_timer.getElapsedMicroSec() - m_GPUTimeStart;

            // sleep if we finished work faster than m_loopTime
            m_frameTime = m_timer.getElapsedMicroSec() - m_frameStart;
            //BR_INFO("m_frameTime: %d", m_frameTime);
            if(m_frameTime < m_loopTime)
            {
                BR_INFO("sleepTime: %f", (m_loopTime - m_frameTime));
                std::chrono::duration<long long, std::nano> sleepTime(static_cast<long long>((m_loopTime - m_frameTime) * 1000.0f));
                std::this_thread::sleep_for(sleepTime);
            }
        }

        BR_INFO("%s", "GameLoop stopped");
    }
}