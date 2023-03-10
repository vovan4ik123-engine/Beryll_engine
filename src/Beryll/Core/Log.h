#pragma once

#if defined(BR_DEBUG)

#include "LibsHeaders.h"
#include "CppHeaders.h"

    static std::mutex globalLogMutex;

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
            strForm += FIND_FIRST_ARG(__VA_ARGS__);   \
            {                \
                std::scoped_lock<std::mutex> logLock(globalLogMutex);                 \
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, strForm.c_str() GET_ARGS(__VA_ARGS__) \
            }                 \
        }
        #define BR_WARN(...) \
        { \
            std::string strForm; strForm.reserve(50); \
            strForm = __FILE__; \
            strForm = strForm.substr(strForm.find_last_of('/') + 1); \
            strForm += " : "; \
            strForm +=  std::to_string(__LINE__); \
            strForm +=  " | "; \
            strForm += FIND_FIRST_ARG(__VA_ARGS__);   \
            {                \
                std::scoped_lock<std::mutex> logLock(globalLogMutex);                 \
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, strForm.c_str() GET_ARGS(__VA_ARGS__) \
            }                  \
        }
        #define BR_ERROR(...) \
        { \
            std::string strForm; strForm.reserve(50); \
            strForm = __FILE__; \
            strForm = strForm.substr(strForm.find_last_of('/') + 1); \
            strForm += " : "; \
            strForm +=  std::to_string(__LINE__); \
            strForm +=  " | "; \
            strForm += FIND_FIRST_ARG(__VA_ARGS__);   \
            {                \
                std::scoped_lock<std::mutex> logLock(globalLogMutex);                 \
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, strForm.c_str() GET_ARGS(__VA_ARGS__) \
            }                   \
        }

        #define BR_ASSERT(condition, ...) { if(condition == false) { BR_ERROR(__VA_ARGS__); assert(false); } }

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

// max number of argument in format string = 5
// example of string with 3 arguments
// format string: "str: %s int: %d float: %f, "qwe", 12, 4.1234

// %s = const char*
// %f = float
// %d = int

