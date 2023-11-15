#include "EventHandler.h"
#include "beryll/core/TimeStep.h"
#include "beryll/core/Log.h"
#include "beryll/core/Window.h"
#include "beryll/GUI/MainImGUI.h"

namespace Beryll
{
    std::vector<bool> EventHandler::m_events = std::vector<bool>(static_cast<int>(EventID::COUNT), false);
    std::vector<Finger> EventHandler::m_fingers = std::vector<Finger>();

    void EventHandler::loadEvents()
    {
        for(Finger& f : m_fingers)
        {
            f.downEvent = false;
        }

        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type)
            {
//MOBILE APP LIFECYCLE
                // Active(100% visible) -> onPause()-> Visible(partially hidden) -> onStop() -> Background(100% hidden)
                // Active(100% visible) <- onResume() <- Visible(partially hidden) <- onStart() <- Background(100% hidden)

                //The application is low on memory, free memory if possible.
                // Called on iOS in applicationDidReceiveMemoryWarning()
                //Called on Android in onLowMemory()
                case SDL_APP_LOWMEMORY:
                    m_events[static_cast<int>(EventID::APP_LOWMEMORY)] = true;
                    break;

                    // Prepare for go to background. Can terminated without event.
                    // Prepare for potential terminating here !!!!
                    // Called on iOS in applicationWillResignActive()
                    // Called on Android in onPause()
                case SDL_APP_WILLENTERBACKGROUND:
                    m_events[static_cast<int>(EventID::APP_WILLENTERBACKGROUND)] = true;
                    break;

                    //already in background
                    //Called on iOS in applicationDidEnterBackground()
                    //Called on Android in onPause()
                case SDL_APP_DIDENTERBACKGROUND:
                    m_events[static_cast<int>(EventID::APP_DIDENTERBACKGROUND)] = true;
                    break;

                    //prepare to appear in foreground
                    //Called on iOS in applicationWillEnterForeground()
                    //Called on Android in onResume()
                case SDL_APP_WILLENTERFOREGROUND:
                    m_events[static_cast<int>(EventID::APP_WILLENTERFOREGROUND)] = true;
                    break;

                    //already in foreground
                    //Called on iOS in applicationDidBecomeActive()
                    //Called on Android in onResume()
                case SDL_APP_DIDENTERFOREGROUND:
                    m_events[static_cast<int>(EventID::APP_DIDENTERFOREGROUND)] = true;
                    break;
// ENDMOBILE APP LIFECYCLE

                    //Android OS can decide to terminate your application by calling onDestroy()
                    //Your application will receive a SDL_QUIT event
                    //Save game before Android will close it
                case SDL_QUIT:
                    m_events[static_cast<int>(EventID::QUIT)] = true;
                    break;

//KEYS
                case SDL_KEYDOWN:
                    if (event.key.keysym.scancode == SDL_SCANCODE_AC_BACK)
                    {
                        m_events[static_cast<int>(EventID::KEY_AC_BACK_DOWN)] = true;
                    }
                    break;

                case SDL_KEYUP:
                    if (event.key.keysym.scancode == SDL_SCANCODE_AC_BACK)
                    {
                        m_events[static_cast<int>(EventID::KEY_AC_BACK_UP)] = true;
                    }
                    break;
//END KEYS

//TOUCH EVENT
                case SDL_FINGERDOWN:
                    m_fingers.emplace_back(Finger{glm::vec2(event.tfinger.x, event.tfinger.y),
                                                 glm::vec2(event.tfinger.x * MainImGUI::getInstance()->getGUIWidth(), event.tfinger.y * MainImGUI::getInstance()->getGUIHeight()),
                                                 glm::vec2(event.tfinger.x * Window::getInstance()->getScreenWidth(), event.tfinger.y * Window::getInstance()->getScreenHeight()),
                                                  false,
                                                  true,
                                                  static_cast<int>(event.tfinger.fingerId)});
                    break;

                case SDL_FINGERUP:
                    {
                        auto it = std::find_if(m_fingers.begin(), m_fingers.end(), [&event](const Finger& f){ return f.ID == static_cast<int>(event.tfinger.fingerId); });
                        if(it != m_fingers.end())
                        {
                            m_fingers.erase(it);
                        }
                        break;
                    }

                case SDL_FINGERMOTION:
                    {
                        auto it = std::find_if(m_fingers.begin(), m_fingers.end(), [&event](const Finger& f){ return f.ID == static_cast<int>(event.tfinger.fingerId); });
                        if(it != m_fingers.end())
                        {
                            (*it).normalizedPos.x = event.tfinger.x;
                            (*it).normalizedPos.y = event.tfinger.y;

                            (*it).ImGuiScreenPos.x = event.tfinger.x * MainImGUI::getInstance()->getGUIWidth();
                            (*it).ImGuiScreenPos.y = event.tfinger.y * MainImGUI::getInstance()->getGUIHeight();

                            (*it).SDL2ScreenPos.x = event.tfinger.x * Window::getInstance()->getScreenWidth();
                            (*it).SDL2ScreenPos.y = event.tfinger.y * Window::getInstance()->getScreenHeight();
                        }
                        break;
                    }

                case SDL_MULTIGESTURE:
                    // multi touch pinch
                    // if( glm::abs(event.mgesture.dDist) > 0.00001f)

                    // multi touch rotation
                    // if( glm::abs(event.mgesture.dTheta) > min angle)

                    break;
//END TOUCH EVENT

//SCREEN
                case SDL_WINDOWEVENT :
                    if (SDL_GetDisplayOrientation(0) != Window::getInstance()->currentOrientation)
                    {
                        m_events[static_cast<int>(EventID::DISPLAY_ORIENTATION_CHANGE)] = true;
                        Window::getInstance()->currentOrientation = SDL_GetDisplayOrientation(0);
                    }
                    break;
//END SCREEN
                default:
                    break;
            }
        }
    }

    void EventHandler::resetEvents(EventID id)
    {
        if(id == EventID::ALL_EVENTS)
        {
            std::fill(m_events.begin(), m_events.end(), false);
        }
        else if(id == EventID::DISPLAY_ORIENTATION_CHANGE)
        {
            m_events[static_cast<int>(EventID::DISPLAY_ORIENTATION_CHANGE)] = false;
        }
        else
        {
            BR_ASSERT(false, "Reset for this event not implemented: %d", static_cast<int>(id));
        }
    }
}
