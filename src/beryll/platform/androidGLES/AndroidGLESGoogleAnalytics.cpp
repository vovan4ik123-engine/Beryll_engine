#include "AndroidGLESGoogleAnalytics.h"

namespace Beryll
{
    AndroidGLESGoogleAnalytics::AndroidGLESGoogleAnalytics()
    {
        m_jniEnv = static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
        if (!m_jniEnv)
        {
            BR_ASSERT(false, "%s", "SDL_AndroidGetJNIEnv() returned nullptr.")
        }

        m_javaGoogleAnalyticsManagerClassID = m_jniEnv->FindClass("managers/GoogleAnalyticsManager"); // app/src/main/java/managers/GoogleAnalyticsManager.java
        if (!m_javaGoogleAnalyticsManagerClassID)
        {
            BR_ASSERT(false, "%s", "m_javaGoogleAnalyticsManagerClassID is nullptr.")
        }

        m_sendEventEmptyMethodID = m_jniEnv->GetStaticMethodID(m_javaGoogleAnalyticsManagerClassID, "sendEventEmpty", "(Ljava/lang/String;)V");
        if (!m_sendEventEmptyMethodID)
        {
            BR_ASSERT(false, "%s", "m_sendEventEmptyMethodID is nullptr.")
        }

        m_sendEventStringParamMethodID = m_jniEnv->GetStaticMethodID(m_javaGoogleAnalyticsManagerClassID, "sendEventStringParam", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        if (!m_sendEventStringParamMethodID)
        {
            BR_ASSERT(false, "%s", "m_sendEventStringParamMethodID is nullptr.")
        }

        m_sendEventIntParamMethodID = m_jniEnv->GetStaticMethodID(m_javaGoogleAnalyticsManagerClassID, "sendEventIntParam", "(Ljava/lang/String;Ljava/lang/String;I)V");
        if (!m_sendEventIntParamMethodID)
        {
            BR_ASSERT(false, "%s", "m_sendEventIntParamMethodID is nullptr.")
        }
    }

    AndroidGLESGoogleAnalytics::~AndroidGLESGoogleAnalytics()
    {

    }

    void AndroidGLESGoogleAnalytics::sendEventEmpty(std::string eventType)
    {
        jstring eventT = m_jniEnv->NewStringUTF(eventType.c_str());

        m_jniEnv->CallStaticVoidMethod(m_javaGoogleAnalyticsManagerClassID, m_sendEventEmptyMethodID, eventT);

        m_jniEnv->DeleteLocalRef(eventT);
    }

    void AndroidGLESGoogleAnalytics::sendEventStringParam(std::string eventType, std::string paramName, std::string paramValue)
    {
        jstring eventT = m_jniEnv->NewStringUTF(eventType.c_str());
        jstring paramN = m_jniEnv->NewStringUTF(paramName.c_str());
        jstring paramV = m_jniEnv->NewStringUTF(paramValue.c_str());

        m_jniEnv->CallStaticVoidMethod(m_javaGoogleAnalyticsManagerClassID, m_sendEventStringParamMethodID, eventT, paramN, paramV);

        m_jniEnv->DeleteLocalRef(eventT);
        m_jniEnv->DeleteLocalRef(paramN);
        m_jniEnv->DeleteLocalRef(paramV);
    }

    void AndroidGLESGoogleAnalytics::sendEventIntParam(std::string eventType, std::string paramName, int paramValue)
    {
        jstring eventT = m_jniEnv->NewStringUTF(eventType.c_str());
        jstring paramN = m_jniEnv->NewStringUTF(paramName.c_str());

        m_jniEnv->CallStaticVoidMethod(m_javaGoogleAnalyticsManagerClassID, m_sendEventIntParamMethodID, eventT, paramN, paramValue);

        m_jniEnv->DeleteLocalRef(eventT);
        m_jniEnv->DeleteLocalRef(paramN);
    }
}
