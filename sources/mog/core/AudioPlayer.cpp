#include "mog/Constants.h"
#include "mog/core/AudioPlayer.h"
#include "mog/core/AudioPlayerNative.h"
#include <future>

using namespace mog;

#pragma - AudioChannel

std::shared_ptr<AudioChannel> AudioChannel::create(const std::shared_ptr<AudioPlayer> &audioPlayer) {
    auto audioChannel = std::shared_ptr<AudioChannel>(new AudioChannel());
    audioChannel->audioChannelNative = AudioChannelNative::create(audioPlayer->audioPlayerNative);
    return audioChannel;
}

void AudioChannel::play(std::string filename, bool cache) {
    if (this->mute) return;
    this->audioChannelNative->load(filename.c_str(), cache);
    this->audioChannelNative->play();
}

void AudioChannel::pause() {
    if (this->mute) return;
    this->audioChannelNative->pause();
}

void AudioChannel::resume() {
    if (this->mute) return;
    this->audioChannelNative->resume();
}

void AudioChannel::stop() {
    this->audioChannelNative->stop();
}

void AudioChannel::close() {
    this->audioChannelNative->close();
}

void AudioChannel::seek(float offset) {
    this->audioChannelNative->seek(offset);
}

void AudioChannel::setLoop(bool isLoop) {
    this->audioChannelNative->setLoop(isLoop);
}

bool AudioChannel::isLoop() {
    return this->audioChannelNative->isLoop();
}

void AudioChannel::setVolume(float volume) {
    this->audioChannelNative->setVolume(volume);
}

float AudioChannel::getVolume() {
    return this->audioChannelNative->getVolume();
}

void AudioChannel::setMute(bool mute) {
    this->mute = mute;
    if (mute) {
        this->stop();
    }
}

bool AudioChannel::isMute() {
    return this->mute;
}

bool AudioChannel::isLoaded() {
    return this->audioChannelNative->isLoaded();
}

AudioChannel::State AudioChannel::getState() {
    return this->audioChannelNative->getState();
}

void AudioChannel::execute() {
}

#pragma - AudioPlayer

std::weak_ptr<AudioPlayer> AudioPlayer::instance;

std::shared_ptr<AudioPlayer> AudioPlayer::create() {
    auto audioPlayer = std::shared_ptr<AudioPlayer>(new AudioPlayer());
    audioPlayer->audioPlayerNative = AudioPlayerNative::create();
    auto channel = AudioChannel::create(audioPlayer);
    audioPlayer->poolOneShotChannels.emplace_back(channel);
    instance = audioPlayer;
    return audioPlayer;
}

std::shared_ptr<AudioChannel> AudioPlayer::createChannel(std::string key) {
    if (auto audioPlayer = instance.lock()) {
        if (audioPlayer->channels[key]) {
            return audioPlayer->channels[key];
        }
        auto channel = AudioChannel::create(audioPlayer);
        audioPlayer->channels[key] = channel;
        return channel;
    }
    return nullptr;
}

std::shared_ptr<AudioChannel> AudioPlayer::getChannel(std::string key) {
    if (auto audioPlayer = instance.lock()) {
        audioPlayer->channels[key];
    }
    return nullptr;
}

std::unordered_map<std::string, std::shared_ptr<AudioChannel>> AudioPlayer::getAllChannels() {
    if (auto audioPlayer = instance.lock()) {
        return audioPlayer->channels;
    }
    return std::unordered_map<std::string, std::shared_ptr<AudioChannel>>();
}

void AudioPlayer::removeChannel(std::string key) {
    if (auto audioPlayer = instance.lock()) {
        auto channel = audioPlayer->channels[key];
        if (channel) {
            channel->close();
            audioPlayer->channels.erase(key);
        }
    }
}

void AudioPlayer::preloadOne(std::string filename) {
    if (auto audioPlayer = instance.lock()) {
        audioPlayer->audioPlayerNative->preload(filename.c_str());
    }
}

void AudioPlayer::playOneShot(std::string filename) {
    if (auto audioPlayer = instance.lock()) {
        std::shared_ptr<AudioChannel> channel = nullptr;
        for (int i = (int)audioPlayer->poolOneShotChannels.size() - 1; i >= 0; i--) {
            const std::shared_ptr<AudioChannel> &c = audioPlayer->poolOneShotChannels[i];
            if (c->getState() == AudioChannel::State::Playing) {
                continue;
            }
            
            if (c->getState() == AudioChannel::State::Paused) {
                c->stop();
            }
            if (channel) {
                c->close();
                audioPlayer->poolOneShotChannels.erase(audioPlayer->poolOneShotChannels.begin() + i);
            } else {
                channel = c;
            }
        }
        if (!channel) {
            channel = AudioChannel::create(audioPlayer);
            audioPlayer->poolOneShotChannels.emplace_back(channel);
        }
        channel->play(filename);
    }
}

void AudioPlayer::onPause() {
    if (auto audioPlayer = instance.lock()) {
        for (const auto &pair : audioPlayer->channels) {
            if (pair.second->getState() == AudioChannel::State::Playing) {
                audioPlayer->resumeChannels.emplace_back(pair.second);
                pair.second->pause();
            }
        }
        for (const auto &c : audioPlayer->poolOneShotChannels) {
            if (c->getState() == AudioChannel::State::Playing) {
                audioPlayer->resumeChannels.emplace_back(c);
                c->pause();
            }
        }
    }
}

void AudioPlayer::onResume() {
    if (auto audioPlayer = instance.lock()) {
        for (const auto &c : audioPlayer->resumeChannels) {
            c->resume();
        }
        audioPlayer->resumeChannels.clear();
    }
}

void AudioPlayer::execute() {
    if (auto audioPlayer = instance.lock()) {
        for (const auto &pair : audioPlayer->channels) {
            pair.second->execute();
        }
    }
}

