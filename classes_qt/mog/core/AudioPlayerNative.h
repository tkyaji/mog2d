#include <memory>
#include <string>
#include <unordered_map>
#include "mog/core/AudioPlayer.h"

using namespace std;

namespace mog {

    class AudioChannelNative {
    public:
        AudioChannelNative(AudioPlayerNative *audioPlayerNative);
        ~AudioChannelNative();

        void load(const char *filename, bool cache = true);
        void play();
        void playWithFade(float duration, function<void()> onComplate = nullptr);
        void pause();
        void resume();
        void stop();
        void stopWithFade(float duration, function<void()> onComplate = nullptr);
        void close();
        void seek(float offset);
        void setLoop(bool isLoop);
        bool isLoop();
        void setVolume(float volume);
        float getVolume();
        bool isLoaded();
        AudioChannel::State getState();
    };


    class AudioPlayerNative {
    public:
        AudioPlayerNative();
        ~AudioPlayerNative();

        void preload(const char *filename1, const char *filename2 = nullptr, const char *filename3 = nullptr,
                     const char *filename4 = nullptr, const char *filename5 = nullptr, const char *filename6 = nullptr);
    };
}
