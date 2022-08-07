#include "SoundsManager.h"
#include "Log.h"

namespace Beryll
{
    std::map<std::string, std::unique_ptr<Mix_Chunk, decltype(&Mix_FreeChunk)>> SoundsManager::m_WAVs;

    std::map<std::string, std::unique_ptr<Mix_Music, decltype(&Mix_FreeMusic)>> SoundsManager::m_MP3s;

    void SoundsManager::create()
    {
        // MIX_DEFAULT_FREQUENCY = sample rate = frequency = speed playing				// size (speed playing )
        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_CHANNELS, 1024) == -1)
        {
            BR_ASSERT(false, "SDL mixer init error");
        }
    }

    void SoundsManager::loadWAV(const char* path)
    {
        const std::string strPath = path;
        BR_ASSERT((strPath.find_last_of('.') != std::string::npos), "Sound does not have extension:{0}", strPath);

        std::string extension = strPath.substr(strPath.find_last_of('.'));
        BR_ASSERT((extension == ".wav"), "loadWAV() loads only .wav sounds");

        auto result =  m_WAVs.find(strPath);
        if(result != m_WAVs.end()) { return; }

        Mix_Chunk* wavSound = Mix_LoadWAV(path);
        BR_ASSERT((wavSound != nullptr), "Create Mix_Chunk failed:{0}", strPath);

        Mix_VolumeChunk(wavSound, MIX_MAX_VOLUME / 2);

        m_WAVs.insert(std::make_pair(strPath, std::unique_ptr<Mix_Chunk, decltype(&Mix_FreeChunk)>(wavSound, Mix_FreeChunk)));
    }

    void SoundsManager::playWAV(const std::string& path, uint32_t timesRepeat)
    {
        auto result =  m_WAVs.find(path);
        if(result != m_WAVs.end())
        {
            Mix_PlayChannel(-1, result->second.get(), timesRepeat);
        }
    }

    void SoundsManager::loadBackgroundMP3(const char* path)
    {
        const std::string strPath = path;
        BR_ASSERT((strPath.find_last_of('.') != std::string::npos), "Sound does not have extension:{0}", strPath);

        std::string extension = strPath.substr(strPath.find_last_of('.'));
        BR_ASSERT((extension == ".mp3"), "loadBackgroundMP3() loads only .mp3 sounds");

        auto result =  m_MP3s.find(strPath);
        if(result != m_MP3s.end()) { return; }

        Mix_Music* music = Mix_LoadMUS(path);
        BR_ASSERT((music != nullptr), "Create Mix_Music failed:{0}", strPath);

        Mix_VolumeMusic(64); // volume from 0 to 128

        m_MP3s.insert(std::make_pair(strPath, std::unique_ptr<Mix_Music, decltype(&Mix_FreeMusic)>(music, Mix_FreeMusic)));
    }

    void SoundsManager::startBackgroundMP3(const std::string& path, bool repeat)
    {
        auto result = m_MP3s.find(path);
        if(result == m_MP3s.end()) { return; }

        stopBackgroundMP3();

        if(repeat) { Mix_PlayMusic(result->second.get(), -1); }
        else { Mix_PlayMusic(result->second.get(), 0); }
    }

    void SoundsManager::setBackgroundVolume(uint32_t volume)
    {
        Mix_VolumeMusic(volume);
    }

    void SoundsManager::pauseBackgroundMP3()
    {
        if ( !Mix_PausedMusic()) { Mix_PauseMusic(); }
    }

    void SoundsManager::resumeBackgroundMP3()
    {
        if (Mix_PausedMusic()) { Mix_ResumeMusic(); }
    }

    void SoundsManager::stopBackgroundMP3()
    {
        Mix_HaltMusic();
    }

    bool SoundsManager::getIsBackgroundMP3Playing()
    {
        return getIsBackgroundMP3Started() && !getIsBackgroundMP3Paused();
    }

    bool SoundsManager::getIsBackgroundMP3Paused()
    {
        return Mix_PausedMusic();
    }

    bool SoundsManager::getIsBackgroundMP3Started()
    {
        return Mix_PlayingMusic(); // does not check paused or not. true is fas called Mix_PlayMusic()
    }
}