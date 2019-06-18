#include "mog/Constants.h"
#include "mog/core/AudioPlayerNative.h"
#include "mog/core/FileUtils.h"
#include "mog/libs/stb_vorbis.h"

using namespace mog;

#pragma - AudioData

std::unordered_map<std::string, std::shared_ptr<AudioData>> AudioData::cachedAudioData;

AudioData::AudioData(ALuint buffer, std::string filename) {
    this->buffer = buffer;
    this->filename = filename;
}

AudioData::~AudioData() {
    alDeleteBuffers(1, &this->buffer);
}

std::shared_ptr<AudioData> AudioData::loadAudioData(const char *filename, bool cache) {
    if (AudioData::cachedAudioData.count(filename) > 0) {
        return AudioData::cachedAudioData[filename];
    }
    
    ALuint buffer;
    alGenBuffers(1, &buffer);
    if (AudioData::loadFromAssetNative(buffer, filename)) {
        auto audioData = std::shared_ptr<AudioData>(new AudioData(buffer, filename));
        if (cache) {
            AudioData::cachedAudioData[filename] = audioData;
        }
        return audioData;
    } else {
        alDeleteBuffers(1, &buffer);
    }
    
    return nullptr;
}

void AudioData::clearCache() {
    AudioData::cachedAudioData.clear();
}

bool AudioData::loadFromAssetNative(ALuint buffer, const char *filename) {
    alGetError();
    
    char filepath[256];
    sprintf(filepath, "assets/%s", filename);

    bool audioLoaded = false;
    void* audioData = nullptr;
    ALsizei dataSize = 0;
    ALenum dataFormat = 0;
    ALsizei sampleRate = 0;

    const char *suffix = ".ogg";
    int pos = strlen(filepath) - strlen(suffix);

    if (pos >= 0 && strcmp(&filepath[pos], suffix) == 0) {
        auto data = FileUtils::readBytesAsset(filename);
        
        int _channels = 0;
        int _sampleRate = 0;
        short *_output;
        
        unsigned char *value = nullptr;
        unsigned int length = 0;
        data->getValue(&value, &length);
        int dataLen = stb_vorbis_decode_memory((const uint8 *)value, length, &_channels, &_sampleRate, &_output);
        if (dataLen < 0) {
            LOGE("Audio resource load failed.\n");
            LOGE(filename);
            return false;
            
        } else {
            dataSize = dataLen * _channels * sizeof(short);
            audioData = (void *)_output;
            dataFormat = (_channels > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
            sampleRate = _sampleRate;
            audioLoaded = true;
        }
    } else {
        // Not supported
    }

    if (!audioLoaded) return false;
    
    alBufferData(buffer, dataFormat, audioData, dataSize, sampleRate);
    
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        mogfree(audioData);
        LOGE("alBufferData failed. error=%d\n", (int)error);
        return false;
    }
    mogfree(audioData);
    
    return true;
}



#pragma - AudioChannelNative

std::shared_ptr<AudioChannelNative> AudioChannelNative::create(const std::shared_ptr<AudioPlayerNative> &audioPlayerNative) {
    auto audioChannelNative = std::shared_ptr<AudioChannelNative>(new AudioChannelNative());
    audioChannelNative->audioPlayerNative = audioPlayerNative;
    return audioChannelNative;
}

void AudioChannelNative::initialize() {
    if (this->initialized) return;
    
    alGenSources(1, &this->source);

    for (auto &pair : this->commandQueue) {
        switch (pair.first) {
            case Command::Load: {
                auto dict = std::static_pointer_cast<Dictionary>(pair.second);
                auto filename = dict->get<String>("filename")->getValue();
                bool cache = dict->get<Bool>("cache")->getValue();
                this->load(filename.c_str(), cache);
                break;
            }
            case Command::Play: {
                this->play();
                break;
            }
            case Command::Pause: {
                this->pause();
                break;
            }
            case Command::Resume: {
                this->resume();
                break;
            }
            case Command::Stop: {
                this->stop();
                break;
            }
            case Command::Close: {
                this->close();
                break;
            }
            case Command::Seek: {
                float s = std::static_pointer_cast<Float>(pair.second)->getValue();
                this->seek(s);
                break;
            }
            case Command::SetLoop: {
                bool b = std::static_pointer_cast<Bool>(pair.second)->getValue();
                this->setLoop(b);
                break;
            }
            case Command::SetVolume: {
                float v = std::static_pointer_cast<Float>(pair.second)->getValue();
                this->setVolume(v);
                break;
            }
        }
    }

    this->initialized = true;
}

void AudioChannelNative::load(const char *filename, bool cache) {
    if (!this->initialized) {
        auto dict = Dictionary::create();
        dict->put("filename", String::create(filename));
        dict->put("cache", Bool::create(cache));
        this->commandQueue.emplace_back(std::pair<Command, std::shared_ptr<Data>>(Command::Load, dict));
        return;
    }

    if (this->audioData) {
        this->stop();
    }
    
    this->audioData = AudioData::loadAudioData(filename, cache);
    if (this->audioData) {
        alSourcei(this->source, AL_BUFFER, this->audioData->buffer);
        
        ALenum error = alGetError();
        if (error != AL_NO_ERROR) {
            LOGE("alSourcei failed. error=%d\n", (int)error);
            return;
        }
        
        this->loaded = true;
        alSourceRewind(this->source);
    }
}

void AudioChannelNative::play() {
    if (!this->initialized) {
        this->commandQueue.emplace_back(std::pair<Command, std::shared_ptr<Data>>(Command::Play, Null::create()));
        return;
    }

    if (!this->isLoaded()) {
        LOGE("Audio file is not loaded.\n");
        return;
    }
    alSourcePlay(this->source);
}

void AudioChannelNative::pause() {
    if (!this->initialized) {
        this->commandQueue.emplace_back(std::pair<Command, std::shared_ptr<Data>>(Command::Pause, Null::create()));
        return;
    }

    if (this->getState() != AudioChannel::State::Playing) return;
    alSourcePause(this->source);
}

void AudioChannelNative::resume() {
    if (!this->initialized) {
        this->commandQueue.emplace_back(std::pair<Command, std::shared_ptr<Data>>(Command::Resume, Null::create()));
        return;
    }

    if (this->getState() != AudioChannel::State::Paused) return;
    alSourcePlay(this->source);
}

void AudioChannelNative::stop() {
    alSourceStop(this->source);
}

void AudioChannelNative::close() {
    if (!this->initialized) {
        this->commandQueue.emplace_back(std::pair<Command, std::shared_ptr<Data>>(Command::Close, Null::create()));
        return;
    }

    alDeleteSources(1, &this->source);
    this->source = 0;
    this->audioData.reset();
}

void AudioChannelNative::seek(float offset) {
    if (!this->initialized) {
        this->commandQueue.emplace_back(std::pair<Command, std::shared_ptr<Data>>(Command::Seek, Float::create(offset)));
        return;
    }

    alSourcef(this->source, AL_SEC_OFFSET, offset);
}

void AudioChannelNative::setLoop(bool isLoop) {
    if (!this->initialized) {
        this->commandQueue.emplace_back(std::pair<Command, std::shared_ptr<Data>>(Command::SetLoop, Bool::create(isLoop)));
        return;
    }

    alSourcei(this->source, AL_LOOPING, (ALuint)isLoop);
}

bool AudioChannelNative::isLoop() {
    if (!this->initialized) {
        float l = 1.0f;
        for (auto &pair : this->commandQueue) {
            if (pair.first == Command::SetLoop) {
                l = std::static_pointer_cast<Bool>(pair.second)->getValue();
            }
        }
        return l;
    }

    ALint loop;
    alGetSourcei(this->source, AL_LOOPING, &loop);
    return loop != 0;
}

void AudioChannelNative::setVolume(float volume) {
    if (!this->initialized) {
        this->commandQueue.emplace_back(std::pair<Command, std::shared_ptr<Data>>(Command::SetVolume, Float::create(volume)));
        return;
    }

    alSourcef(this->source, AL_GAIN, (ALfloat)volume);
}

float AudioChannelNative::getVolume() {
    if (!this->initialized) {
        float v = 1.0f;
        for (auto &pair : this->commandQueue) {
            if (pair.first == Command::SetVolume) {
                v = std::static_pointer_cast<Float>(pair.second)->getValue();
            }
        }
        return v;
    }

    ALfloat volume;
    alGetSourcef(this->source, AL_GAIN, &volume);
    return (float)volume;
}

bool AudioChannelNative::isLoaded() {
    return this->loaded;
}

AudioChannel::State AudioChannelNative::getState() {
    if (!this->initialized) {
        AudioChannel::State state = AudioChannel::State::Stopped;
        for (auto &pair : this->commandQueue) {
            if (pair.first == Command::Play) {
                state = AudioChannel::State::Playing;
            } else if (pair.first == Command::Play) {
                state = AudioChannel::State::Paused;
            } else if (pair.first == Command::Stop || pair.first == Command::Close) {
                state = AudioChannel::State::Stopped;
            }
        }
        return state;
    }

    ALint state;
    alGetSourcei(this->source, AL_SOURCE_STATE, &state);
    switch (state) {
        case AL_PLAYING:
            return AudioChannel::State::Playing;
        case AL_PAUSED:
            return AudioChannel::State::Paused;
        default:
            return AudioChannel::State::Stopped;
    }
}


#pragma - AudioPlayerNative

std::shared_ptr<AudioPlayerNative> AudioPlayerNative::create() {
    auto audioPlayerNative = std::shared_ptr<AudioPlayerNative>(new AudioPlayerNative());
    return audioPlayerNative;
}

AudioPlayerNative::AudioPlayerNative() {
}

AudioPlayerNative::~AudioPlayerNative() {
    if (this->initialized) {
        alcMakeContextCurrent(NULL);
        alcDestroyContext(this->context);
        alcCloseDevice(this->device);
    }
}

void AudioPlayerNative::initialize() {
    if (this->initialized) return;

    this->device = alcOpenDevice(NULL);
    this->context = alcCreateContext(this->device, NULL);
    alcMakeContextCurrent(this->context);
    for (auto &filename : this->preloadFileQueue) {
        AudioData::loadAudioData(filename.c_str());
    }
    this->preloadFileQueue.clear();
    this->initialized = true;
}

bool AudioPlayerNative::isInitialized() {
    return this->initialized;
}

void AudioPlayerNative::preload(const char *filename) {
    if (this->initialized) {
        AudioData::loadAudioData(filename);
    } else {
        this->preloadFileQueue.emplace_back(filename);
    }
}
