#include "mog/os/AndroidHelper.h"
#include "mog/core/AudioPlayerNative.h"
#include "mog/core/Engine.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

using namespace mog;

std::unordered_map<std::string, AudioData::AudioAsset> AudioData::cachedAudioData;

AudioData::AudioData(SLEngineItf &slEngine, SLObjectItf &outputMixObj, const char *filename) {
    this->slEngine = slEngine;
    this->outputMixObj = outputMixObj;
    this->filename = filename;
    this->load();
}

AudioData::~AudioData() {
    this->close();
}

std::shared_ptr<AudioData> AudioData::loadAudioData(SLEngineItf &slEngine, SLObjectItf &outputMixObj, const char *filename, bool cache) {
    return std::make_shared<AudioData>(slEngine, outputMixObj, filename);
}

void AudioData::clearCache() {
    AudioData::cachedAudioData.clear();
}

bool AudioData::preLoadAssetFile(std::string filename) {
    AudioAsset audioAsset;
    return AudioData::loadAssetFile(filename, &audioAsset);
}

bool AudioData::loadAssetFile(std::string filename, AudioAsset *audioAsset) {
    if (AudioData::cachedAudioData.count(filename) > 0) {
        auto cachedAudioAsset = AudioData::cachedAudioData[filename];
        audioAsset->fd = cachedAudioAsset.fd;
        audioAsset->start = cachedAudioAsset.start;
        audioAsset->length = cachedAudioAsset.length;
        return true;
    }

    AAssetManager *aAssetManager = AndroidHelper::getAssetManager();

    AAsset* asset = AAssetManager_open(aAssetManager, filename.c_str(), AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
        LOGE("[AudioPlayerNative] failed to open an audio file (%s)", filename.c_str());
        return false;
    }

    off_t start;
    off_t length;
    int fd = AAsset_openFileDescriptor(asset, &start, &length);
    if (fd < 0) {
        LOGE("[AudioPlayerNative] failed to open an audio file (%s)", filename.c_str());
        LOGE(filename.c_str());
    }

    AAsset_close(asset);

    if (fd >= 0) {
        audioAsset->fd = fd;
        audioAsset->start = start;
        audioAsset->length = length;
        AudioData::cachedAudioData[filename] = *audioAsset;
        return true;

    } else {
        return false;
    }
}

void AudioData::load() {
    AudioAsset audioAsset;
    if (!AudioData::loadAssetFile(this->filename, &audioAsset)) {
        return;
    }

    SLDataLocator_AndroidFD loc_fd = {SL_DATALOCATOR_ANDROIDFD, audioAsset.fd, audioAsset.start, audioAsset.length};
    SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
    SLDataSource audioSrc = {&loc_fd, &format_mime};

    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, this->outputMixObj};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    const SLInterfaceID player_ids[3] = {SL_IID_PLAY, SL_IID_VOLUME, SL_IID_SEEK};
    const SLboolean player_req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    SLresult result = (*this->slEngine)->CreateAudioPlayer(this->slEngine,
                                                           &this->playerObject,
                                                           &audioSrc, &audioSnk,
                                                           3, player_ids, player_req);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AudioPlayerNative] CreateAudioPlayer failed.");
        this->close();
        return;
    }

    result = (*this->playerObject)->Realize(this->playerObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AudioPlayerNative] Realize player failed.");
        this->close();
        return;
    }

    result = (*this->playerObject)->GetInterface(this->playerObject, SL_IID_PLAY, &this->playerPlay);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AudioPlayerNative] failed to get an audio player interface.");
        this->close();
        return;
    }

    result = (*this->playerObject)->GetInterface(this->playerObject, SL_IID_SEEK, &this->playerSeek);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AudioPlayerNative] failed to get an audio seek interface.");
        this->close();
        return;
    }

    result = (*this->playerObject)->GetInterface(this->playerObject, SL_IID_VOLUME, &this->playerVolume);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AudioPlayerNative] failed to create an audio volume interface.");
        this->close();
        return;
    }

    this->loaded = true;
}

void AudioData::close() {
    if (this->playerObject) {
        (*this->playerObject)->Destroy(this->playerObject);
    }
    this->playerObject = nullptr;
    this->playerPlay = nullptr;
    this->playerSeek = nullptr;
    this->playerVolume = nullptr;
    this->loaded = false;
}

SLuint32 AudioData::getState() {
    if (!this->loaded) {
        return SL_PLAYSTATE_STOPPED;
    }
    if (this->getLoop()) {
        return this->state;
    }
    SLuint32 state;
    SLresult result = (*this->playerPlay)->GetPlayState(this->playerPlay, &state);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AudioPlayerNative] GetPlayState failed");
    }
    return state;
}

void AudioData::setState(SLuint32 state) {
    if (!this->loaded) {
        return;
    }
    SLresult result = (*this->playerPlay)->SetPlayState(this->playerPlay, state);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AudioPlayerNative] SetPlayState failed");
    } else {
        this->state = state;
    }
}

void AudioData::seek(float offset) {
    if (!this->loaded) {
        return;
    }
    SLmillisecond pos = (SLmillisecond)(offset * 1000.0f);
    SLresult result = (*this->playerSeek)->SetPosition(this->playerSeek, pos, SL_SEEKMODE_FAST);

    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AudioPlayerNative] SetPosition failed");
    }
}

float AudioData::getPosition() {
    SLmillisecond pos;
    (*this->playerPlay)->GetPosition(this->playerPlay, &pos);
    return (pos / 1000.0f);
}

void AudioData::setLoop(bool loop) {
    if (!this->loaded) {
        return;
    }
    SLboolean b = (loop) ? SL_BOOLEAN_TRUE : SL_BOOLEAN_FALSE;
    SLresult result = (*this->playerSeek)->SetLoop(this->playerSeek, b, 0, SL_TIME_UNKNOWN);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AudioPlayerNative] SetLoop failed");
    }
}

bool AudioData::getLoop() {
    if (!this->loaded) {
        return false;
    }
    SLboolean b = SL_BOOLEAN_FALSE;
    SLmillisecond start;
    SLmillisecond end;
    SLresult result = (*this->playerSeek)->GetLoop(this->playerSeek, &b, &start, &end);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AudioPlayerNative] GetLoop failed");
    }
    return (b == SL_BOOLEAN_TRUE);
}

void AudioData::setVolume(float volume) {
    if (!this->loaded) {
        return;
    }
    if (volume <= 0) {
        SLresult result = (*this->playerVolume)->SetMute(this->playerVolume, SL_BOOLEAN_TRUE);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("[AudioPlayerNative] SetMute failed");
        }
    } else {
        SLresult result = (*this->playerVolume)->SetMute(this->playerVolume, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("[AudioPlayerNative] SetMute failed");
        }
        SLmillibel level = (SLmillibel)((1.0f - volume) * SL_MILLIBEL_MIN);
        result = (*this->playerVolume)->SetVolumeLevel(this->playerVolume, level);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("[AudioPlayerNative] SetLoop failed");
        }
    }
}

float AudioData::getVolume() {
    if (!this->loaded) {
        return 0;
    }
    SLboolean b;
    SLresult result = (*this->playerVolume)->GetMute(this->playerVolume, &b);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AudioPlayerNative] GetMute failed");
    }
    if (b == SL_BOOLEAN_TRUE) {
        return 0;
    }

    SLmillibel v;
    result = (*this->playerVolume)->GetVolumeLevel(this->playerVolume, &v);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("[AudioPlayerNative] GetVolumeLevel failed");
        return 0;
    }

    return 1.0f - ((float)v / SL_MILLIBEL_MIN);
}


#pragma - AudioChannelNative

AudioChannelNative::AudioChannelNative(AudioPlayerNative *audioPlayerNative) {
    this->slEngine = audioPlayerNative->slEngine;
    this->createOutputMix();
}

AudioChannelNative::~AudioChannelNative() {
    (*this->outputMixObj)->Destroy(this->outputMixObj);
}

void AudioChannelNative::createOutputMix() {
    const SLInterfaceID ids[1] = {SL_IID_VOLUME};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    SLresult result = (*this->slEngine)->CreateOutputMix(this->slEngine, &this->outputMixObj, 0, ids, req);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("[AudioPlayerNative] Failed to CreateOutputMix");
        return;
    }
    result = (*this->outputMixObj)->Realize(this->outputMixObj, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("[AudioPlayerNative] Failed to Realize");
    }
}

void AudioChannelNative::load(const char *filename, bool cache) {
    if (this->audioData && this->audioData->filename == filename) return;
    this->audioData = AudioData::loadAudioData(this->slEngine, this->outputMixObj, filename, cache);
    if (!this->audioData) {
        return;
    }
    this->setLoop(this->loop);
    this->setVolume(this->volume);
}

void AudioChannelNative::play() {
    if (!this->isLoaded()) {
        LOGW("[AudioPlayerNative] Audio file is not loaded.");
        return;
    }

    if (this->audioData->getState() != SL_PLAYSTATE_STOPPED) {
        this->stop();
    }
    this->audioData->setState(SL_PLAYSTATE_PLAYING);
}

void AudioChannelNative::pause() {
    if (this->pausing) return;
    if (!this->isLoaded()) {
        LOGW("[AudioPlayerNative] Audio file is not loaded.");
        return;
    }
    this->pausePosition = this->audioData->getPosition();
    this->backupState = this->audioData->getState();
    this->audioData->setState(SL_PLAYSTATE_STOPPED);
    this->pausing = true;
}

void AudioChannelNative::resume() {
    if (!this->pausing) return;
    if (!this->isLoaded()) {
        LOGW("[AudioPlayerNative] Audio file is not loaded.");
        return;
    }
    this->seek(this->pausePosition);
    this->pausePosition = 0;
    this->audioData->setState(this->backupState);
    this->pausing = false;
}

void AudioChannelNative::stop() {
    if (!this->isLoaded()) {
        LOGW("[AudioPlayerNative] Audio file is not loaded.");
        return;
    }
    this->audioData->setState(SL_PLAYSTATE_STOPPED);
    this->audioData->seek(0);
}

void AudioChannelNative::close() {
    this->audioData = nullptr;
}

void AudioChannelNative::seek(float offset) {
    if (this->audioData) {
        this->audioData->seek(offset);
    }
    this->offset = offset;
}

void AudioChannelNative::setLoop(bool isLoop) {
    if (this->audioData) {
        this->audioData->setLoop(isLoop);
    }
    this->loop = isLoop;
}

bool AudioChannelNative::isLoop() {
    return this->loop;
}

void AudioChannelNative::setVolume(float volume) {
    if (this->audioData) {
        this->audioData->setVolume(volume);
    }
    this->volume = volume;
}

float AudioChannelNative::getVolume() {
    return this->volume;
}

bool AudioChannelNative::isLoaded() {
    return this->audioData && this->audioData->loaded;
}

AudioChannel::State AudioChannelNative::getState() {
    if (!this->audioData) {
        return AudioChannel::State::Stopped;
    }

    auto state = this->audioData->getState();
    switch (state) {
        case SL_PLAYSTATE_PLAYING:
            return AudioChannel::State::Playing;
        case SL_PLAYSTATE_PAUSED:
            return AudioChannel::State::Paused;
        default:
            return AudioChannel::State::Stopped;
    }
}


#pragma - AudioPlayerNative

AudioPlayerNative::AudioPlayerNative() {
    // create engine
    const SLInterfaceID ids[] = {SL_IID_ENGINE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    SLresult result = slCreateEngine(&this->slObject, 0, NULL, 1, ids, req);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("failed to create an audio player");
    }

    result = (*this->slObject)->Realize(this->slObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("failed to create an audio player");
    }

    result = (*this->slObject)->GetInterface(this->slObject, SL_IID_ENGINE, &this->slEngine);
    if (SL_RESULT_SUCCESS != result) {
        LOGE("failed to create an audio player");
    }
}

AudioPlayerNative::~AudioPlayerNative() {
    (*this->slObject)->Destroy(this->slObject);
}

void AudioPlayerNative::preload(const char *filename1, const char *filename2, const char *filename3,
                                 const char *filename4, const char *filename5, const char *filename6) {
    if (filename1 != nullptr) AudioData::preLoadAssetFile(filename1);
    if (filename2 != nullptr) AudioData::preLoadAssetFile(filename2);
    if (filename3 != nullptr) AudioData::preLoadAssetFile(filename3);
    if (filename4 != nullptr) AudioData::preLoadAssetFile(filename4);
    if (filename5 != nullptr) AudioData::preLoadAssetFile(filename5);
    if (filename6 != nullptr) AudioData::preLoadAssetFile(filename6);
}
