#include "AndroidGLESAds.h"

namespace Beryll
{
    std::function<void()> AndroidGLESAds::successCallback;
    std::function<void()> AndroidGLESAds::errorCallback;

    AndroidGLESAds::AndroidGLESAds()
    {
        m_jniEnv = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
        if (!m_jniEnv)
        {
            BR_ASSERT(false, "%s", "SDL_AndroidGetJNIEnv() returned nullptr.")
        }

        m_javaAdsManagerClassID = m_jniEnv->FindClass("managers/AdsManager"); // app/src/main/java/managers/AdsManager.java
        if (!m_javaAdsManagerClassID)
        {
            BR_ASSERT(false, "%s", "m_javaAdsManagerClassID is nullptr.")
        }

        m_showRewardedAdMethodID = m_jniEnv->GetStaticMethodID(m_javaAdsManagerClassID, "showRewardedAd", "()V");
        if (!m_showRewardedAdMethodID)
        {
            BR_ASSERT(false, "%s", "m_showRewardedAdMethodID is nullptr.")
        }
    }

    AndroidGLESAds::~AndroidGLESAds()
    {

    }

    void AndroidGLESAds::showRewardedAd(std::function<void()> successCall,
                                        std::function<void()> errorCall)
    {
        BR_INFO("%s", "adsLogs showRewardedAd()");

        AndroidGLESAds::successCallback = std::move(successCall);
        AndroidGLESAds::errorCallback = std::move(errorCall);

        m_jniEnv->CallStaticVoidMethod(m_javaAdsManagerClassID, m_showRewardedAdMethodID);
    }
}

// Callbacks from java code on android.
extern "C" JNIEXPORT void JNICALL
Java_managers_AdsManager_rewardedAdSuccessCallback(JNIEnv *env, jclass clazz)
{
    BR_INFO("%s", "adsLogs Java_managers_AdsManager_rewardedAdSuccessCallback");

    if(Beryll::AndroidGLESAds::successCallback)
    {
        Beryll::AndroidGLESAds::successCallback();
    }

    Beryll::AndroidGLESAds::successCallback = nullptr;
    Beryll::AndroidGLESAds::errorCallback = nullptr;
}
extern "C" JNIEXPORT void JNICALL
Java_managers_AdsManager_rewardedAdErrorCallback(JNIEnv *env, jclass clazz)
{
    BR_INFO("%s", "adsLogs Java_managers_AdsManager_rewardedAdErrorCallback");

    if(Beryll::AndroidGLESAds::errorCallback)
    {
        Beryll::AndroidGLESAds::errorCallback();
    }

    Beryll::AndroidGLESAds::successCallback = nullptr;
    Beryll::AndroidGLESAds::errorCallback = nullptr;
}