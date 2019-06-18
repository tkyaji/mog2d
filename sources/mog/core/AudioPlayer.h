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
        friend class AudioPlayer;
    public:
        enum class State {
            Playing,
            Stopped,
            Paused,
        };
        
        static std::shared_ptr<AudioChannel> create(const std::shared_ptr<AudioPlayer> &audioPlayer);
        
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
        
        std::shared_ptr<AudioChannelNative> audioChannelNative;

    private:
        AudioChannel() {}
        void execute();
        
        bool mute = false;
    };
    
    
    class AudioPlayer {
    public:
        static std::weak_ptr<AudioPlayer> instance;
        std::shared_ptr<AudioPlayerNative> audioPlayerNative;
        
        static std::shared_ptr<AudioPlayer> create();
        static std::shared_ptr<AudioChannel> createChannel(std::string key);
        static std::shared_ptr<AudioChannel> getChannel(std::string key);
        static std::unordered_map<std::string, std::shared_ptr<AudioChannel>> getAllChannels();
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
        static void preload() { }
        static void preloadOne(std::string filename);
        static void execute();

        AudioPlayer() {}

        std::unordered_map<std::string, std::shared_ptr<AudioChannel>> channels;
        std::vector<std::shared_ptr<AudioChannel>> poolOneShotChannels;
        std::vector<std::shared_ptr<AudioChannel>> resumeChannels;
    };
}

#endif /* MogAudio_h */
