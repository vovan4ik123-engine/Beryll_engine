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

    uint32_t GameLoop::m_maxFPS = 300;

    Platform GameLoop::m_platform = Platform::UNKNOWN;

    uint32_t GameLoop::calcStart = 0;
    uint32_t GameLoop::calcTime = 0;
    uint32_t GameLoop::drawStart = 0;
    uint32_t GameLoop::drawTime = 0;

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
        BR_INFO("%s", "GameLoop started");

        m_isRun = true;
        m_loopTime = 1000 / m_maxFPS; // millisec
        Physics::enableSimulation(); // also reset timer inside Physics

        while(m_isRun)
        {
            TimeStep::fixateTime(); // fixate time of last finished game loop
            m_frameStart = TimeStep::getMillisecFromStart();
            calcStart = TimeStep::getMillisecFromStart();

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

            calcTime = TimeStep::getMillisecFromStart() - calcStart;
        // Draw start    DONT CALL ANY DRAW COMMANDS before this point !!!!!!!!
            // First finish draw previous frame
            //Window::getInstance()->finishDraw(); // very slow
            //Window::getInstance()->flushDraw(); // potentially can be called but not necessary
            Window::getInstance()->swapWindow();
            drawTime = TimeStep::getMillisecFromStart() - drawStart;
            // Next start draw new frame
            drawStart = TimeStep::getMillisecFromStart();
            Window::getInstance()->clear();
            MainImGUI::getInstance()->beginFrame();

            GameStateMachine::draw();

            MainImGUI::getInstance()->endFrame();

        // Draw finish

        // PlaySound start
            GameStateMachine::playSound();
        // PlaySound finish

            // sleep if we finished work faster than m_loopTime
            m_frameTime = TimeStep::getMillisecFromStart() - m_frameStart;
            //BR_INFO("m_frameTime %d", m_frameTime);
            if(m_frameTime < m_loopTime)
            {
                TimeStep::sleep(m_loopTime - m_frameTime);
            }
        }

        BR_INFO("%s", "GameLoop stopped");
    }
}