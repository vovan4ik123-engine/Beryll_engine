#pragma once

#include "beryll/googleAnalytics/GoogleAnalytics.h"

#include "jni.h"

namespace Beryll
{
    class AndroidGLESGoogleAnalytics : public GoogleAnalytics
    {
    public:
        ~AndroidGLESGoogleAnalytics() override;

        void sendEventEmpty(std::string eventType) override;
        void sendEventStringParam(std::string eventType, std::string paramName, std::string paramValue) override;
        void sendEventFloatParam(std::string eventType, std::string paramName, float paramValue) override;

    private:
        friend class GoogleAnalytics;
        AndroidGLESGoogleAnalytics();

        JNIEnv* m_jniEnv = nullptr;
        jclass m_javaGoogleAnalyticsManagerClassID = nullptr;
        jmethodID m_sendEventEmptyMethodID = nullptr;
        jmethodID m_sendEventStringParamMethodID = nullptr;
        jmethodID m_sendEventFloatParamMethodID = nullptr;
    };
}
