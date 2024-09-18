#include "SoundsManager.h"
#include "Log.h"

namespace Beryll
{
    bool SoundsManager::m_created = false;
    std::map<std::string, std::unique_ptr<Mix_Chunk, decltype(&Mix_FreeChunk)>> SoundsManager::m_WAVs;
    std::map<std::string, std::unique_ptr<Mix_Music, decltype(&Mix_FreeMusic)>> SoundsManager::m_MP3s;


    void SoundsManager::create()
    {
        if(m_created) { return; }

        // MIX_DEFAULT_FREQUENCY = sample rate = frequency = speed playing				// size (speed playing )
        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_CHANNELS, 1024) == -1)
        {
            BR_ASSERT(false, "%s", "SDL mixer init error");
        }

        m_created = true;
    }

    void SoundsManager::loadWAV(const std::string& path, int volume)
    {
        BR_ASSERT((path.find_last_of('.') != std::string::npos), "Sound does not have extension: %s", path.c_str());
        BR_ASSERT((path.substr(path.find_last_of('.')) == ".wav"), "%s", "loadWAV() loads only .wav sounds.");

        auto result =  m_WAVs.find(path);
        if(result != m_WAVs.end()) { return; }

        Mix_Chunk* wavSound = Mix_LoadWAV(path.c_str());
        BR_ASSERT((wavSound != nullptr), "Mix_LoadWAV() failed: %s", path.c_str());

        Mix_VolumeChunk(wavSound, volume);

        m_WAVs.insert(std::make_pair(path, std::unique_ptr<Mix_Chunk, decltype(&Mix_FreeChunk)>(wavSound, Mix_FreeChunk)));
    }

    void SoundsManager::playWAV(const std::string& path, int timesRepeat)
    {
        auto result =  m_WAVs.find(path);
        if(result != m_WAVs.end())
        {
            Mix_PlayChannel(-1, result->second.get(), timesRepeat);
        }
    }

    int SoundsManager::getNumberOfWAVCurrentlyPlaying()
    {
        return Mix_Playing(-1);
    }

    void SoundsManager::loadBackgroundMP3(const std::string& path, int volume)
    {
        BR_ASSERT((path.find_last_of('.') != std::string::npos), "Sound does not have extension: %s", path.c_str());
        BR_ASSERT((path.substr(path.find_last_of('.')) == ".mp3"), "%s", "loadBackgroundMP3() loads only .mp3 sounds.");

        auto result =  m_MP3s.find(path);
        if(result != m_MP3s.end()) { return; }

        Mix_Music* music = Mix_LoadMUS(path.c_str());
        BR_ASSERT((music != nullptr), "Mix_LoadMUS() failed: %s", path.c_str());

        Mix_VolumeMusic(volume);

        m_MP3s.insert(std::make_pair(path, std::unique_ptr<Mix_Music, decltype(&Mix_FreeMusic)>(music, Mix_FreeMusic)));
    }

    void SoundsManager::startBackgroundMP3(const std::string& path, bool repeat)
    {
        auto result = m_MP3s.find(path);
        if(result == m_MP3s.end()) { return; }

        stopBackgroundMP3();

        if(repeat) { Mix_PlayMusic(result->second.get(), -1); }
        else { Mix_PlayMusic(result->second.get(), 0); }
    }

    void SoundsManager::setBackgroundVolume(int volume)
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
        return Mix_PlayingMusic(); // Does not check paused or not. True if was called Mix_PlayMusic().
    }
}
