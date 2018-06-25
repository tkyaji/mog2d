#ifndef MogAudio_h
#define MogAudio_h

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>

namespace mog {
    
    class AudioPlayer;
    class AudioPlayerNative;
    class AudioChannelNative;
    
    class AudioChannel {
    public:
        enum class State {
            Playing,
            Stopped,
            Paused,
        };
        
        AudioChannel(AudioPlayer *audioPlayer);
        
        void play(std::string filename, bool cache = true);
        void pause();
        void resume();
        void stop();
        void close();
        void seek(float offset);
        void setLoop(bool isLoop);
        bool isLoop();
        void setVolume(float volume);
        float getVolume();
        void setMute(bool mute);
        bool isMute();
        bool isLoaded();
        State getState();
        
    private:
        std::unique_ptr<AudioChannelNative> audioChannelNative;
        bool mute = false;
    };
    
    
    class AudioPlayer {
    public:
        std::unique_ptr<AudioPlayerNative> audioPlayerNative;
        
        static void initialize();
        static std::shared_ptr<AudioChannel> createChannel(std::string key);
        static std::shared_ptr<AudioChannel> getChannel(std::string key);
        static void removeChannel(std::string key);

        template<class First, class... Rest>
        static void preload(const First& first, const Rest&... rest) {
            AudioPlayer::preloadOne(first);
            AudioPlayer::preload(rest...);
        }
        
        static void playOneShot(std::string filename);
        
        static void onPause();
        static void onResume();

    private:
        static AudioPlayer *instance;
        static void preload() {
        }
        static void preloadOne(std::string filename);

        std::unordered_map<std::string, std::shared_ptr<AudioChannel>> channels;
        std::vector<std::shared_ptr<AudioChannel>> poolOneShotChannels;
        std::vector<std::shared_ptr<AudioChannel>> resumeChannels;
        
        AudioPlayer();
    };
}

#endif /* MogAudio_h */
