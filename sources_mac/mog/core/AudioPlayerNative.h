#include <memory>
#include <string>
#include <unordered_map>
#include <OpenAL/OpenAL.h>
#include <AudioToolbox/AudioToolbox.h>
#include "AudioPlayer.h"

namespace mog {
    
    class AudioPlayerNative;
    
    class AudioData {
    public:
        AudioData(ALuint buffer, std::string filename);
        ~AudioData();
        
        static std::shared_ptr<AudioData> loadAudioData(const char *filename, bool cache = true);
        static void clearCache();
        
        ALuint buffer;
        std::string filename;
        
    private:
        static std::unordered_map<std::string, std::shared_ptr<AudioData>> cachedAudioData;
        
        static bool loadFromAssetNative(ALuint buffer, const char *filename);
        static void *getOpenALAudioData(CFURLRef fileURL, ALsizei* dataSize,
                                        ALenum* dataFormat, ALsizei *sampleRate, bool *loaded);

    };

    
    class AudioChannelNative {
    public:
        static std::shared_ptr<AudioChannelNative> create(const std::shared_ptr<AudioPlayerNative> &audioPlayerNative);
        
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
        AudioChannelNative();
        
        std::weak_ptr<AudioPlayerNative> audioPlayerNative;
        bool enabled;
        bool loaded;
        ALuint  source;
        std::shared_ptr<AudioData> audioData;
    };
    
    
    class AudioPlayerNative {
    public:
        static std::shared_ptr<AudioPlayerNative> create();
        ~AudioPlayerNative();
        
        void preload(const char *filename);
        
    private:
        AudioPlayerNative();
        ALCdevice*  device;
        ALCcontext* context;
    };
}
