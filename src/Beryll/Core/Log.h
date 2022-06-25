#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

namespace Beryll
{
    class Log
    {
    public:
        static Log* inst()
        {
            static Log instance;
            return &instance;
        }

#if defined(ANDROID)
        std::shared_ptr<spdlog::logger> getAndroidLogger()
        {
            return m_androidLogger;
        }
#endif

    private:
        Log() {};
        ~Log() {};

#if defined(ANDROID)
        std::shared_ptr<spdlog::logger> m_androidLogger = spdlog::android_logger_mt("android", "spdlog-android");
#endif
    };
}

#if defined(BR_DEBUG)

    // pass __VA_ARGS__ here. return first argument from __VA_ARGS__
    #define FIND_FIRST_ARG(FIRST_ARG, ...) FIRST_ARG
    // pass __VA_ARGS__ here. returned __VA_ARGS__ = passed __VA_ARGS__ -1 first argument
    #define FIND_AFTER_FIRST_ARG(FIRST_ARG, ...) __VA_ARGS__
    // call CHECK_SECOND_ARG(__VA_ARGS__, 0) will return second argument or 0
    #define CHECK_SECOND_ARG(FIRST_ARG,N,...) N

    // Ignore first ARG because it included in strForm
    #define ARG1(A)                 );
    #define ARG2(A, B)              , B);
    #define ARG3(A, B, C)           ,B, C);
    #define ARG4(A, B, C, D)        ,B, C, D);
    #define ARG5(A, B, C, D, E)     ,B, C, D, E);
    #define ARG6(A, B, C, D, E, F)  ,B, C, D, E, F);

    #define GET_MACRO(_1,_2,_3,_4,_5,_6,NAME,...) NAME
    #define GET_ARGS(...) GET_MACRO(__VA_ARGS__, ARG6, ARG5, ARG4, ARG3, ARG2, ARG1)(__VA_ARGS__)

    #if defined(ANDROID)

        #define BR_INFO(...) \
        { \
            std::string strForm; strForm.reserve(50); \
            strForm = __FILE__; \
            strForm = strForm.substr(strForm.find_last_of('/') + 1); \
            strForm += " : "; \
            strForm +=  std::to_string(__LINE__); \
            strForm +=  " | "; \
            strForm += FIND_FIRST_ARG(__VA_ARGS__); \
            Beryll::Log::inst()->getAndroidLogger()->info(strForm GET_ARGS(__VA_ARGS__) \
        }
        #define BR_WARN(...) \
        { \
            std::string strForm; strForm.reserve(50); \
            strForm = __FILE__; \
            strForm = strForm.substr(strForm.find_last_of('/') + 1); \
            strForm += " : "; \
            strForm +=  std::to_string(__LINE__); \
            strForm +=  " | "; \
            strForm += FIND_FIRST_ARG(__VA_ARGS__); \
            Beryll::Log::inst()->getAndroidLogger()->warn(strForm GET_ARGS(__VA_ARGS__) \
        }
        #define BR_ERROR(...) \
        { \
            std::string strForm; strForm.reserve(50); \
            strForm = __FILE__; \
            strForm = strForm.substr(strForm.find_last_of('/') + 1); \
            strForm += " : "; \
            strForm +=  std::to_string(__LINE__); \
            strForm +=  " | "; \
            strForm += FIND_FIRST_ARG(__VA_ARGS__); \
            Beryll::Log::inst()->getAndroidLogger()->error(strForm GET_ARGS(__VA_ARGS__) \
        }

        #define BR_ASSERT(condition, ...) { if(condition == false) { BR_ERROR(__VA_ARGS__); assert(condition); } }

    #else // if defined(BR_DEBUG) but unknown platform

        #define BR_INFO(...)
        #define BR_WARN(...)
        #define BR_ERROR(...)
        #define BR_ASSERT(condition, ...)

    #endif //ANDROID

#else // not BR_DEBUG

    #define BR_INFO(...)
    #define BR_WARN(...)
    #define BR_ERROR(...)
    #define BR_ASSERT(condition, ...)

#endif // BR_DEBUG

// Exapmles
// ... = formatted string + parameters or usual string
// BR_INFO("Info with param:{0} second:{1} {2} {3} -- == -{4}", "info1", 434.344f, 3, 4, 5);
// BR_WARN("Warn with param {0}", "warn1");
// BR_ERROR("Error with param {0}", "error1");
// BR_ASSERT(false, "message before assert {0} {1}", "some info", 123);
