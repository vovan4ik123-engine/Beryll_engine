#include "AndroidGLESGoogleAnalytics.h"

namespace Beryll
{
    AndroidGLESGoogleAnalytics::AndroidGLESGoogleAnalytics()
    {
        m_jniEnv = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
        if (!m_jniEnv)
        {
            BR_ASSERT(false, "%s", "SDL_AndroidGetJNIEnv() returned nullptr.")
        }

        m_javaGoogleAnalyticsManagerClassID = m_jniEnv->FindClass("com/controlledbattles/bigbattlesimulator/app/GoogleAnalyticsManager");
        if (!m_javaGoogleAnalyticsManagerClassID)
        {
            BR_ASSERT(false, "%s", "m_javaGoogleAnalyticsManagerClassID is nullptr.")
        }

        m_sendEventMethodID = m_jniEnv->GetStaticMethodID(m_javaGoogleAnalyticsManagerClassID, "sendEvent", "(Ljava/lang/String;Ljava/lang/String;)V");
        if (!m_sendEventMethodID)
        {
            BR_ASSERT(false, "%s", "m_sendEventMethodID is nullptr.")
        }
    }

    AndroidGLESGoogleAnalytics::~AndroidGLESGoogleAnalytics()
    {

    }

    void AndroidGLESGoogleAnalytics::sendEvent(std::string eventType, std::string eventName)
    {
        jstring eventT = m_jniEnv->NewStringUTF(eventType.c_str());
        jstring eventN = m_jniEnv->NewStringUTF(eventName.c_str());

        m_jniEnv->CallStaticVoidMethod(m_javaGoogleAnalyticsManagerClassID, m_sendEventMethodID, eventT, eventN);

        m_jniEnv->DeleteLocalRef(eventT);
        m_jniEnv->DeleteLocalRef(eventN);
    }
}
