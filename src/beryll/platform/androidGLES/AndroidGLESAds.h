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
                            std::function<void()> errorCall,
                            bool callbackAtCloseWindow) override;

        void showInterstitialAd(std::function<void()> successCall,
                                std::function<void()> errorCall) override;

        // One methods for all ads. That means only one type of ad can be run at one time.
        static std::function<void()> successCallback;
        static std::function<void()> errorCallback;

    private:
        friend class Ads;
        AndroidGLESAds();

        JNIEnv* m_jniEnv = nullptr;
        jclass m_javaAdsManagerClassID = nullptr;
        jmethodID m_showRewardedAdMethodID = nullptr;
        jmethodID m_showInterstitialAdMethodID = nullptr;
    };
}
