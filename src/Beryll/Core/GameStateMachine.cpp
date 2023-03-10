#include "GameStateMachine.h"
#include "Beryll/Core/Log.h"

namespace Beryll
{
    std::vector<std::shared_ptr<GameState>> GameStateMachine::m_gameStates = std::vector<std::shared_ptr<GameState>>();
    std::vector<std::shared_ptr<GameState>> GameStateMachine::m_gameStatesForDelete = std::vector<std::shared_ptr<GameState>>();

    void GameStateMachine::updateBeforePhysics()
    {
        if(!m_gameStates.empty())
        {
            m_gameStates.back()->updateBeforePhysics();
        }

        // only after gameState->update() finished work !!!!
        if(!m_gameStatesForDelete.empty())
        {
            m_gameStatesForDelete.clear();
        }
    }

    void GameStateMachine::updateAfterPhysics()
    {
        if(!m_gameStates.empty())
        {
            m_gameStates.back()->updateAfterPhysics();
        }
    }

    void GameStateMachine::draw()
    {
        if(!m_gameStates.empty())
        {
            m_gameStates.back()->draw();
        }
    }

    void GameStateMachine::playSound()
    {
        if(!m_gameStates.empty())
        {
            m_gameStates.back()->playSound();
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
        if(m_gameStates.empty())
        {
            pushState(state);
        }
        else
        {
            popState();
            pushState(state);
        }
    }

    void GameStateMachine::popState()
    {
        if(!m_gameStates.empty())
        {
            m_gameStatesForDelete.push_back(m_gameStates.back());
            m_gameStates.pop_back();
        }
    }
}