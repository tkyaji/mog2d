#include <memory>
#include <string>
#include <unordered_map>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "mog/core/AudioPlayer.h"

namespace mog {

    class AudioData {
    public:
        std::string filename;
        bool loaded = false;
        SLObjectItf playerObject;
        SLEngineItf slEngine;
        SLObjectItf outputMixObj;
        SLPlayItf playerPlay;
        SLSeekItf playerSeek;
        SLVolumeItf playerVolume;

        AudioData(SLEngineItf &slEngine, SLObjectItf &outputMixObj, const char *filename);
        ~AudioData();

        SLuint32 getState();
        void setState(SLuint32 state);
        void seek(float offset);
        float getPosition();
        void setLoop(bool loop);
        bool getLoop();
        void setVolume(float volume);
        float getVolume();

        static std::shared_ptr<AudioData> loadAudioData(SLEngineItf &slEngine, SLObjectItf &outputMixObj, const char *filename, bool cache = true);
        static bool preLoadAssetFile(std::string filename);
        static void clearCache();

    private:
        struct AudioAsset {
            int fd;
            off_t start;
            off_t length;
        };

        static std::unordered_map<std::string, AudioAsset> cachedAudioData;
        static bool loadAssetFile(std::string filename, AudioAsset *audioAsset);
        SLuint32 state = SL_PLAYSTATE_STOPPED;

        void load();
        void close();
    };



    class AudioChannelNative {
    public:
        AudioChannelNative(AudioPlayerNative *audioPlayerNative);
        ~AudioChannelNative();

        void load(const char *filename, bool cache = true);
        void play();
        void playWithFade(float duration, std::function<void()> onComplate = nullptr);
        void pause();
        void resume();
        void stop();
        void stopWithFade(float duration, std::function<void()> onComplate = nullptr);
        void close();
        void seek(float offset);
        void setLoop(bool isLoop);
        bool isLoop();
        void setVolume(float volume);
        float getVolume();
        bool isLoaded();
        AudioChannel::State getState();

    private:
        SLEngineItf slEngine;
        SLObjectItf outputMixObj;
        std::shared_ptr<AudioData> audioData;
        bool loop = false;
        float volume = 1.0f;
        float offset = 0;
        float pausePosition = 0;
        bool pausing = false;
        SLuint32 backupState = SL_PLAYSTATE_STOPPED;

        void createOutputMix();
    };


    class AudioPlayerNative {
    public:
        SLObjectItf slObject;
        SLEngineItf slEngine;

        AudioPlayerNative();
        ~AudioPlayerNative();

        void preload(const char *filename1, const char *filename2 = nullptr, const char *filename3 = nullptr,
                     const char *filename4 = nullptr, const char *filename5 = nullptr, const char *filename6 = nullptr);

        void createOutputMix(SLObjectItf &outputMixObj);
    };
}
