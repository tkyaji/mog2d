#ifndef MogAudio_h
#define MogAudio_h

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>

using namespace std;

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
        
        void play(string filename, bool cache = true);
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
        State getState();
        
    private:
        unique_ptr<AudioChannelNative> audioChannelNative;
    };
    
    
    class AudioPlayer {
    public:
        unique_ptr<AudioPlayerNative> audioPlayerNative;
        
        static void initialize();
        static shared_ptr<AudioChannel> createChannel(string key);
        static shared_ptr<AudioChannel> getChannel(string key);
        static void removeChannel(string key);
        static void preload(string filename1, string filename2 = nullptr, string filename3 = nullptr,
                            string filename4 = nullptr, string filename5 = nullptr, string filename6 = nullptr);
        static void playOneShot(string filename);
        
        static void onPause();
        static void onResume();

    private:
        static AudioPlayer *instance;
        
        unordered_map<string, shared_ptr<AudioChannel>> channels;
        vector<shared_ptr<AudioChannel>> poolOneShotChannels;
        vector<shared_ptr<AudioChannel>> resumeChannels;
        
        AudioPlayer();
    };
}

#endif /* MogAudio_h */
