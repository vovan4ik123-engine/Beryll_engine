#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/core/Log.h"

namespace Beryll
{
    class GoogleAnalytics
    {
    public:
        virtual ~GoogleAnalytics() {}

        static std::unique_ptr<GoogleAnalytics>& getInstance()
        {
            BR_ASSERT((m_googleAnalytics != nullptr), "%s", "GoogleAnalytics is not created. Use GoogleAnalytics::create() first.");
            return m_googleAnalytics;
        }

        virtual void sendEvent(std::string eventType, std::string eventName) = 0;

    protected:
        GoogleAnalytics() {}

    private:
        friend class GameLoop;
        static void create(); // Should be called only in GameLoop.

        static std::unique_ptr<GoogleAnalytics> m_googleAnalytics;
    };
}
