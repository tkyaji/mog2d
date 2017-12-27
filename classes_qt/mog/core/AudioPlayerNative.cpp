#include "mog/core/AudioPlayerNative.h"
#include "mog/core/Engine.h"

using namespace mog;

#pragma - AudioChannelNative

AudioChannelNative::AudioChannelNative(AudioPlayerNative *audioPlayerNative) {
}

AudioChannelNative::~AudioChannelNative() {
}

void AudioChannelNative::load(const char *filename, bool cache) {
}

void AudioChannelNative::play() {
}

void AudioChannelNative::pause() {
}

void AudioChannelNative::resume() {
}

void AudioChannelNative::stop() {
}

void AudioChannelNative::close() {
}

void AudioChannelNative::seek(float offset) {
}

void AudioChannelNative::setLoop(bool isLoop) {
}

bool AudioChannelNative::isLoop() {
    return false;
}

void AudioChannelNative::setVolume(float volume) {
}

float AudioChannelNative::getVolume() {
    return 0;
}

bool AudioChannelNative::isLoaded() {
    return false;
}

AudioChannel::State AudioChannelNative::getState() {
    return AudioChannel::State::Stopped;
}


#pragma - AudioPlayerNative

AudioPlayerNative::AudioPlayerNative() {
}

AudioPlayerNative::~AudioPlayerNative() {
}

void AudioPlayerNative::preload(const char *filename1, const char *filename2, const char *filename3,
                                 const char *filename4, const char *filename5, const char *filename6) {
}
