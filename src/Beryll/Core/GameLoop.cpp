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

namespace Beryll
{
    bool GameLoop::m_isRun = false;

    uint32_t GameLoop::m_loopTime = 0;
    uint32_t GameLoop::m_frameStart = 0;
    uint32_t GameLoop::m_frameTime = 0;

    uint32_t GameLoop::m_maxFPS = 30;

    Platform GameLoop::m_platform = Platform::UNKNOWN;

    void GameLoop::create(Platform p)
    {
        m_platform = p;

        Window::create();
        Window::getInstance()->setClearColor(0.8f, 0.0f, 0.8f, 1.0f);

        MainImGUI::create();

        SoundsManager::create();

        Physics::create();
    }

    void GameLoop::run()
    {
        BR_INFO("GameLoop started");

        m_isRun = true;
        m_loopTime = 1000 / m_maxFPS; // millisec
        Physics::enableSimulation(); // also reset timer inside Physics

        while(m_isRun)
        {
            TimeStep::fixateTime(); // fixate time of last finished game loop
            m_frameStart = TimeStep::getMillisecFromStart();

        // Check user input
            EventHandler::resetEvents(EventID::ALL_EVENTS);
            EventHandler::loadEvents();

        // Update layers start
            Window::getInstance()->checkOrientationChange();
            // set positions of objects here based on user input
            GameStateMachine::updateBeforePhysics();

            Physics::simulate();

            // read positions of objects after simulation here
            // prefer update camera properties here
            GameStateMachine::updateAfterPhysics();
        // Update layers finish

        // Update camera (immediately before draw)
            // Dont set any camera attributes after this call (set in updateAfterPhysics())
            Camera::update3DCamera(Window::getInstance()->getScreenWidth(), Window::getInstance()->getScreenHeight());

        // Draw start
            Window::getInstance()->clear();
            MainImGUI::getInstance()->beginFrame();

            GameStateMachine::draw();

            Window::getInstance()->flushDraw();
            MainImGUI::getInstance()->endFrame();
            Window::getInstance()->swapWindow();
        // Draw finish

        // PlaySound start
            GameStateMachine::playSound();
        // PlaySound finish

            // sleep if we finished work faster than m_loopTime
            m_frameTime = TimeStep::getMillisecFromStart() - m_frameStart;
            if(m_frameTime < m_loopTime)
            {
                TimeStep::sleep(m_loopTime - m_frameTime);
            }
        }

        BR_INFO("GameLoop stopped");
    }
}