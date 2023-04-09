#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

namespace Beryll
{
    class SoundsManager
    {
    public:
        SoundsManager() = delete;
        ~SoundsManager() = delete;

        static void create();

        // short WAV sound effect. can be playing 8 effects at same time
        static void loadWAV(const char* path);
        static void playWAV(const std::string& path, uint32_t timesRepeat = 0);

        // long MP3 music. can be played only 1 music at time
        static void loadBackgroundMP3(const char* path);
        static void startBackgroundMP3(const std::string& path, bool repeat = false);
        static void setBackgroundVolume(uint32_t volume); // 0 - 128
        static void pauseBackgroundMP3();
        static void resumeBackgroundMP3();
        static void stopBackgroundMP3();
        static bool getIsBackgroundMP3Playing();
        static bool getIsBackgroundMP3Paused();
        static bool getIsBackgroundMP3Started();

    private:
        static std::map<std::string, std::unique_ptr<Mix_Chunk, decltype(&Mix_FreeChunk)>> m_WAVs; // with custom deleter

        static std::map<std::string, std::unique_ptr<Mix_Music, decltype(&Mix_FreeMusic)>> m_MP3s;

        static bool m_created;
    };
}
