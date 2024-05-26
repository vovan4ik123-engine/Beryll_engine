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

        virtual void sendEventEmpty(std::string eventType) = 0;
        virtual void sendEventStringParam(std::string eventType, std::string paramName, std::string paramValue) = 0;
        virtual void sendEventIntParam(std::string eventType, std::string paramName, int paramValue) = 0;

    protected:
        GoogleAnalytics() {}

    private:
        friend class GameLoop;
        static void create(); // Should be called only in GameLoop.

        static std::unique_ptr<GoogleAnalytics> m_googleAnalytics;
    };
}
