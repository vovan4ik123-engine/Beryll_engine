#pragma once

#include "beryll/ads/Ads.h"

#include "jni.h"

namespace Beryll
{
    class AndroidGLESAds : public Ads
    {
    public:
        ~AndroidGLESAds() override;

        void showRewardedAd(std::function<void()> successCall,
                            std::function<void()> errorCall) override;

        static std::function<void()> successCallback;
        static std::function<void()> errorCallback;

    private:
        friend class Ads;
        AndroidGLESAds();

        JNIEnv* m_jniEnv = nullptr;
        jclass m_javaAdsManagerClassID = nullptr;
        jmethodID m_showRewardedAdMethodID = nullptr;
    };
}
