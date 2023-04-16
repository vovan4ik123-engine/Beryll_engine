#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

namespace Beryll
{
    enum class EventID
    {
        QUIT,
        APP_LOWMEMORY,
        APP_WILLENTERBACKGROUND,
        APP_DIDENTERBACKGROUND,
        APP_WILLENTERFOREGROUND,
        APP_DIDENTERFOREGROUND,
        KEY_AC_BACK_DOWN,
        KEY_AC_BACK_UP,
        DISPLAY_ORIENTATION_CHANGE,


        ALL_EVENTS,
        COUNT // MUST be always last and more than number of elements
    };

    struct Finger
    {
        // range 0...1
        glm::vec2 normalizedPos;
        // ImGUI recognize screen size different than SDL2 window size
        // use it for positioning ImGUI elements
        glm::vec2 ImGuiScreenPos;
        // use it for 3D/2D scene actions
        glm::vec2 SDL2ScreenPos;
        // for disable event from handling any more
        bool handled;
        // true if finger was down at this game loop iteration.
        // will false in next game loop iteration (reset in loadEvents())
        bool downEvent;
        int ID;
    };

    class EventHandler
    {
    public:
        EventHandler() = delete;
        ~EventHandler() = delete;

        static void loadEvents();
        static void resetEvents(EventID id); // reset before load new

        static bool checkEvent(EventID id) { return m_events[static_cast<int>(id)]; }

        static std::vector<Finger>& getFingers() { return m_fingers; }

    private:
        static std::vector<bool> m_events;
        static std::vector<Finger> m_fingers;
    };
}
