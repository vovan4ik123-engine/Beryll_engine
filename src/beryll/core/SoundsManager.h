#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

namespace Beryll
{
    class SoundsManager final
    {
    public:
        SoundsManager() = delete;
        ~SoundsManager() = delete;

        static void create();

        // Short WAV sound effect. Can be playing 8 effects at same time.
        static void loadWAV(const std::string& path, int volume); // volume = 0...128.
        static void playWAV(const std::string& path, int timesRepeat = 0);

        static int getNumberOfWAVCurrentlyPlaying();

        // Long MP3 music. Can be played only 1 music at time.
        static void loadBackgroundMP3(const std::string& path, int volume); // volume = 0...128.
        static void startBackgroundMP3(const std::string& path, bool repeat = false);
        static void setBackgroundVolume(int volume); // volume = 0...128.
        static void pauseBackgroundMP3();
        static void resumeBackgroundMP3();
        static void stopBackgroundMP3();
        static bool getIsBackgroundMP3Playing();
        static bool getIsBackgroundMP3Paused();
        static bool getIsBackgroundMP3Started();

    private:
        static std::map<std::string, std::unique_ptr<Mix_Chunk, decltype(&Mix_FreeChunk)>> m_WAVs; // With custom deleter.

        static std::map<std::string, std::unique_ptr<Mix_Music, decltype(&Mix_FreeMusic)>> m_MP3s;

        static bool m_created;
    };
}
