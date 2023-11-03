#pragma once

#include "beryll/googleAnalytics/GoogleAnalytics.h"

#include "jni.h"

namespace Beryll
{
    class AndroidGLESGoogleAnalytics : public GoogleAnalytics
    {
    public:
        ~AndroidGLESGoogleAnalytics() override;

        void sendEvent(std::string eventType, std::string eventName) override;

    private:
        friend class GoogleAnalytics;
        AndroidGLESGoogleAnalytics();

        JNIEnv* m_jniEnv = nullptr;
        jclass m_javaGoogleAnalyticsManagerClassID = nullptr;
        jmethodID m_sendEventMethodID = nullptr;
    };
}
