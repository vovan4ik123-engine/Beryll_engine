#include "AndroidGLESBillingSystem.h"

namespace Beryll
{
    std::function<void()> AndroidGLESBillingSystem::successCallback;
    std::function<void()> AndroidGLESBillingSystem::errorCallback;

    AndroidGLESBillingSystem::AndroidGLESBillingSystem()
    {
        m_jniEnv = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
        if (!m_jniEnv)
        {
            BR_ASSERT(false, "%s", "SDL_AndroidGetJNIEnv() returned nullptr.")
        }

        m_javaBillingManagerClassID = m_jniEnv->FindClass("managers/BillingManager"); // app/src/main/java/managers/BillingManager.java
        if (!m_javaBillingManagerClassID)
        {
            BR_ASSERT(false, "%s", "m_javaBillingManagerClassID is nullptr.")
        }

        m_makeInAppPurchaseMethodID = m_jniEnv->GetStaticMethodID(m_javaBillingManagerClassID, "makeInAppPurchase", "(Ljava/lang/String;)V");
        if (!m_makeInAppPurchaseMethodID)
        {
            BR_ASSERT(false, "%s", "m_inAppMethodID is nullptr.")
        }
    }

    AndroidGLESBillingSystem::~AndroidGLESBillingSystem()
    {

    }

    void AndroidGLESBillingSystem::makeInAppPurchaseGooglePlay(std::string googlePlayProductID,
                                                               std::function<void()> successCall,
                                                               std::function<void()> errorCall)
    {
        BR_INFO("testBillingLogs makeInAppPurchaseGooglePlay(std::string googlePlayProductID) %s", googlePlayProductID.c_str());

        AndroidGLESBillingSystem::successCallback = std::move(successCall);
        AndroidGLESBillingSystem::errorCallback = std::move(errorCall);

        jstring productID = m_jniEnv->NewStringUTF(googlePlayProductID.c_str());

        m_jniEnv->CallStaticVoidMethod(m_javaBillingManagerClassID, m_makeInAppPurchaseMethodID, productID);

        m_jniEnv->DeleteLocalRef(productID);
    }
}

// Callbacks from java code on android.
extern "C" JNIEXPORT void JNICALL
Java_com_controlledbattles_bigbattlesimulator_app_BillingManager_billingSystemSuccessCallback(JNIEnv *env, jclass clazz)
{
    BR_INFO("%s", "testBillingLogs Java_com_controlledbattles_bigbattlesimulator_app_BillingManager_billingSystemSuccessCallback");

    if(Beryll::AndroidGLESBillingSystem::successCallback)
    {
        Beryll::AndroidGLESBillingSystem::successCallback();
    }

    Beryll::AndroidGLESBillingSystem::successCallback = nullptr;
    Beryll::AndroidGLESBillingSystem::errorCallback = nullptr;
}
extern "C" JNIEXPORT void JNICALL
Java_com_controlledbattles_bigbattlesimulator_app_BillingManager_billingSystemErrorCallback(JNIEnv *env, jclass clazz)
{
    BR_INFO("%s", "testBillingLogs Java_com_controlledbattles_bigbattlesimulator_app_BillingManager_billingSystemErrorCallback");

    if(Beryll::AndroidGLESBillingSystem::errorCallback)
    {
        Beryll::AndroidGLESBillingSystem::errorCallback();
    }

    Beryll::AndroidGLESBillingSystem::successCallback = nullptr;
    Beryll::AndroidGLESBillingSystem::errorCallback = nullptr;
}
