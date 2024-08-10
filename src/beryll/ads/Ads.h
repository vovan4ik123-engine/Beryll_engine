#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/core/Log.h"

namespace Beryll
{
    class Ads
    {
    public:
        virtual ~Ads() {}

        static std::unique_ptr<Ads>& getInstance()
        {
            BR_ASSERT((m_ads != nullptr), "%s", "m_ads is not created. Use Ads::create() first.");
            return m_ads;
        }

        // True = success call back called when window with add is closing.
        // False = success call back called when reward earned(last second of ad).
        virtual void showRewardedAd(std::function<void()> successCall,
                                    std::function<void()> errorCall,
                                    bool callbackAtCloseWindow) = 0;

    protected:
        Ads() {}

    private:
        friend class GameLoop;
        static void create(); // Should be called only in GameLoop::create().

        static std::unique_ptr<Ads> m_ads;
    };
}
