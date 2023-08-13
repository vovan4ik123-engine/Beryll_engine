#include "GameStateMachine.h"
#include "beryll/core/Log.h"

namespace Beryll
{
    std::vector<std::shared_ptr<GameState>> GameStateMachine::m_gameStates;
    std::shared_ptr<GameState> GameStateMachine::m_currentState = nullptr;

    void GameStateMachine::updateBeforePhysics()
    {
        m_currentState = nullptr;

        if(!m_gameStates.empty())
        {
            // Because if changeState() / popState() will called inside ->updateBeforePhysics();
            // m_gameStates vector will be changed.
            m_currentState = m_gameStates.back();
            m_currentState->updateBeforePhysics();
        }
    }

    void GameStateMachine::updateAfterPhysics()
    {
        if(m_currentState)
        {
            m_currentState->updateAfterPhysics();
        }
    }

    void GameStateMachine::draw()
    {
        if(m_currentState)
        {
            m_currentState->draw();
        }
    }

    void GameStateMachine::pushState(std::shared_ptr<GameState> state)
    {
        auto it = std::find_if(m_gameStates.begin(), m_gameStates.end(),
                               [&state](const std::shared_ptr<GameState>& st)
                               { return st->ID == state->ID; });
        if(it == m_gameStates.end())
        {
            m_gameStates.push_back(state);
        }
        else
        {
            BR_ASSERT(false, "Dont push game state with same ID again: %d", static_cast<int>(state->ID));
        }
    }

    void GameStateMachine::changeState(std::shared_ptr<GameState> state)
    {
        popState();
        pushState(std::move(state));
    }

    void GameStateMachine::popState()
    {
        if(!m_gameStates.empty())
            m_gameStates.pop_back();
    }
}
