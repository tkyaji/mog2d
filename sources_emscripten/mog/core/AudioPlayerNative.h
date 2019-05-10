#include <memory>
#include <string>
#include <unordered_map>
#include <set>
#include <AL/al.h>
#include <AL/alc.h>
#include "mog/core/AudioPlayer.h"
#include "mog/core/Data.h"

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
    };

    
    class AudioChannelNative {
    public:
        AudioChannelNative(AudioPlayerNative *audioPlayerNative);
        ~AudioChannelNative();
        
        void initialize();
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
        enum class Command {
            Load,
            Play,
            Pause,
            Resume,
            Stop,
            Close,
            Seek,
            SetLoop,
            SetVolume,
        };

        AudioPlayerNative *audioPlayerNative;
        bool initialized = false;
        bool enabled;
        bool loaded;
        ALuint  source;
        std::shared_ptr<AudioData> audioData;
        std::vector<std::pair<Command, std::shared_ptr<mog::Data>>> commandQueue;
    };
    
    
    class AudioPlayerNative {
    public:
        AudioPlayerNative();
        ~AudioPlayerNative();
        
        void initialize();
        bool isInitialized();
        void preload(const char *filename);
        void registerChannel(AudioChannelNative *channel);
        void unRegisterChannel(AudioChannelNative *channel);
        
    private:
        bool initialized = false;
        ALCdevice*  device;
        ALCcontext* context;

        std::set<AudioChannelNative *> channels;
    };
}
