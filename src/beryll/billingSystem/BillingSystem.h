#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/core/Log.h"

namespace Beryll
{
    class BillingSystem
    {
    public:
        virtual ~BillingSystem() {}

        static std::unique_ptr<BillingSystem>& getInstance()
        {
            BR_ASSERT((m_billingSystem != nullptr), "%s", "BillingSystem is not created. Use BillingSystem::create() first.");
            return m_billingSystem;
        }

        virtual void makeInAppPurchaseGooglePlay(std::string googlePlayProductID,
                                                 std::function<void()> successCall,
                                                 std::function<void()> errorCall) = 0;

    protected:
        BillingSystem() {}

    private:
        friend class GameLoop;
        static void create(); // Should be called only in GameLoop::create().

        static std::unique_ptr<BillingSystem> m_billingSystem;
    };
}
