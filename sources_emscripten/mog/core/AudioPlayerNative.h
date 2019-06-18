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
        static std::shared_ptr<AudioChannelNative> create(const std::shared_ptr<AudioPlayerNative> &audioPlayerNative);
        
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

        AudioChannelNative() {}
        
        std::weak_ptr<AudioPlayerNative> audioPlayerNative;
        bool loaded = false;
        ALuint source = 0;
        std::shared_ptr<AudioData> audioData;
        std::vector<std::pair<Command, std::shared_ptr<mog::Data>>> commandQueue;
        bool initialized = false;
    };
    
    
    class AudioPlayerNative {
    public:
        static std::shared_ptr<AudioPlayerNative> create();
        void initialize();
        bool isInitialized();
        ~AudioPlayerNative();
        
        void preload(const char *filename);
        
    private:
        AudioPlayerNative();
        ALCdevice*  device;
        ALCcontext* context;
        bool initialized = false;
        std::vector<std::string> preloadFileQueue;
    };
}
