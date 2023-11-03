#pragma once

#include "beryll/billingSystem/BillingSystem.h"

#include "jni.h"

namespace Beryll
{
    class AndroidGLESBillingSystem : public BillingSystem
    {
    public:
        ~AndroidGLESBillingSystem() override;

        void makeInAppPurchaseGooglePlay(std::string googlePlayProductID,
                                         std::function<void()> successCall,
                                         std::function<void()> errorCall) override;

        static std::function<void()> successCallback;
        static std::function<void()> errorCallback;

    private:
        friend class BillingSystem;
        AndroidGLESBillingSystem();

        JNIEnv* m_jniEnv = nullptr;
        jclass m_javaBillingManagerClassID = nullptr;
        jmethodID m_makeInAppPurchaseMethodID = nullptr;
    };
}
