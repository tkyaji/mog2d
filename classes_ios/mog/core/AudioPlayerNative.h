#include <memory>
#include <string>
#include <unordered_map>
#include <OpenAL/OpenAL.h>
#include <AudioToolbox/AudioToolbox.h>
#include "AudioPlayer.h"

using namespace std;

namespace mog {
    
    class AudioPlayerNative;
    
    class AudioData {
    public:
        AudioData(ALuint buffer, string filename);
        ~AudioData();
        
        static shared_ptr<AudioData> loadAudioData(const char *filename, bool cache = true);
        static void clearCache();
        
        ALuint buffer;
        string filename;
        
    private:
        static unordered_map<string, shared_ptr<AudioData>> cachedAudioData;
        
        static bool loadFromAssetNative(ALuint buffer, const char *filename);
        static void *getOpenALAudioData(CFURLRef fileURL, ALsizei* dataSize,
                                        ALenum* dataFormat, ALsizei *sampleRate, bool *loaded);

    };

    
    class AudioChannelNative {
    public:
        AudioChannelNative(AudioPlayerNative *audioPlayerNative);
        
        void load(const char *filename, bool cache = true);
        void play();
        void pause();
        void resume();
        void stop();
        void close();
        void seek(float offset);
        void setLoop(bool isLoop);
        bool isLoop();
        void setVolume(float volume);
        float getVolume();
        bool isLoaded();
        AudioChannel::State getState();
        
    private:
        AudioPlayerNative *audioPlayerNative;
        bool enabled;
        bool loaded;
        ALuint  source;
        shared_ptr<AudioData> audioData;
    };
    
    
    class AudioPlayerNative {
    public:
        AudioPlayerNative();
        ~AudioPlayerNative();
        
        void preload(const char *filename1, const char *filename2 = nullptr, const char *filename3 = nullptr,
                     const char *filename4 = nullptr, const char *filename5 = nullptr, const char *filename6 = nullptr);
        
    private:
        ALCdevice*  device;
        ALCcontext* context;
    };
}
