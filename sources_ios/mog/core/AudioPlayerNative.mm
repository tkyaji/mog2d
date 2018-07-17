#import "mog/Constants.h"
#import "mog/core/AudioPlayerNative.h"
#import "mog/core/FileUtils.h"
#import "mog/libs/stb_vorbis.h"

using namespace mog;

#pragma - AudioData

unordered_map<string, shared_ptr<AudioData>> AudioData::cachedAudioData;

AudioData::AudioData(ALuint buffer, string filename) {
    this->buffer = buffer;
    this->filename = filename;
}

AudioData::~AudioData() {
    alDeleteBuffers(1, &this->buffer);
}

shared_ptr<AudioData> AudioData::loadAudioData(const char *filename, bool cache) {
    if (AudioData::cachedAudioData.count(filename) > 0) {
        return AudioData::cachedAudioData[filename];
    }
    
    ALuint buffer;
    alGenBuffers(1, &buffer);
    if (AudioData::loadFromAssetNative(buffer, filename)) {
        auto audioData = shared_ptr<AudioData>(new AudioData(buffer, filename));
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
    
    NSString *filenameStr = [NSString stringWithFormat:@"assets/%s", filename];
    
    bool audioLoaded = false;
    void* audioData = nullptr;
    ALsizei dataSize = 0;
    ALenum dataFormat = 0;
    ALsizei sampleRate = 0;
    
    if ([[filenameStr lowercaseString] hasSuffix:@".ogg"]) {
        auto data = FileUtils::readBytesAsset(filename);
        
        int _channels = 0;
        int _sampleRate = 0;
        short *_output;
        
        unsigned char *value = nullptr;
        unsigned int length = 0;
        data->getValue(&value, &length);
        int dataLen = stb_vorbis_decode_memory((const uint8 *)value, length, &_channels, &_sampleRate, &_output);
        if (dataLen < 0) {
            LOGE("Audio resource load failed.");
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
        NSString *path = [[NSBundle mainBundle] pathForResource:filenameStr ofType:nil];
        if (!path) {
            LOGE("Audio resource is not found");
            LOGE(filename);
            return false;
        }
        audioData = AudioData::getOpenALAudioData((__bridge CFURLRef)[NSURL fileURLWithPath:path],
                                                  &dataSize, &dataFormat, &sampleRate, &audioLoaded);
    }
    
    if (!audioLoaded) return false;
    
    alBufferData(buffer, dataFormat, audioData, dataSize, sampleRate);
    
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        rpfree(audioData);
        LOGE("alBufferData failed. error=%d", (int)error);
        return false;
    }
    rpfree(audioData);
    
    return true;
}

void *AudioData::getOpenALAudioData(CFURLRef fileURL, ALsizei* dataSize,
                                    ALenum* dataFormat, ALsizei *sampleRate, bool *loaded) {
    OSStatus err;
    UInt32 size;
    void *data = nullptr;
    
    *loaded = false;
    
    ExtAudioFileRef audioFile;
    err = ExtAudioFileOpenURL(fileURL, &audioFile);
    if (err) {
        LOGE("getOpenALAudioData: failed to open audioFile");
        if (audioFile) ExtAudioFileDispose(audioFile);
        return data;
    }
    
    AudioStreamBasicDescription fileFormat;
    size = sizeof(fileFormat);
    err = ExtAudioFileGetProperty(audioFile, kExtAudioFileProperty_FileDataFormat, &size, &fileFormat);
    if (err) {
        LOGE("getOpenALAudioData: failed to get fileFormat");
        if (audioFile) ExtAudioFileDispose(audioFile);
        return data;
    }
    
    AudioStreamBasicDescription outputFormat;
    outputFormat.mSampleRate = fileFormat.mSampleRate;
    outputFormat.mChannelsPerFrame = fileFormat.mChannelsPerFrame;
    outputFormat.mFormatID = kAudioFormatLinearPCM;
    outputFormat.mBytesPerPacket = 2 * outputFormat.mChannelsPerFrame;
    outputFormat.mFramesPerPacket = 1;
    outputFormat.mBytesPerFrame = 2 * outputFormat.mChannelsPerFrame;
    outputFormat.mBitsPerChannel = 16;
    outputFormat.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
    err = ExtAudioFileSetProperty(audioFile, kExtAudioFileProperty_ClientDataFormat, sizeof(outputFormat), &outputFormat);
    if (err) {
        LOGE("getOpenALAudioData: failed to set outputFormat");
        if (audioFile) ExtAudioFileDispose(audioFile);
        return data;
    }
    
    SInt64 fileLengthFrames = 0;
    size = sizeof(fileLengthFrames);
    err = ExtAudioFileGetProperty(audioFile, kExtAudioFileProperty_FileLengthFrames, &size, &fileLengthFrames);
    if (err) {
        LOGE("getOpenALAudioData: failed to get fileLengthFrames");
        if (audioFile) ExtAudioFileDispose(audioFile);
        return data;
    }
    
    UInt32 bufferSize;
    AudioBufferList dataBuffer;
    bufferSize = (UInt32)(fileLengthFrames * outputFormat.mBytesPerFrame);
    data = rpmalloc(bufferSize);
    dataBuffer.mNumberBuffers = 1;
    dataBuffer.mBuffers[0].mDataByteSize = bufferSize;
    dataBuffer.mBuffers[0].mNumberChannels = outputFormat.mChannelsPerFrame;
    dataBuffer.mBuffers[0].mData = data;
    
    err = ExtAudioFileRead(audioFile, (UInt32*)&fileLengthFrames, &dataBuffer);
    if (err) {
        LOGE("getOpenALAudioData: failed to read audioFile");
        rpfree(data);
        if (audioFile) ExtAudioFileDispose(audioFile);
        return data;
    }
    
    *dataSize = (ALsizei)bufferSize;
    *dataFormat = (outputFormat.mChannelsPerFrame > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
    *sampleRate = (ALsizei)outputFormat.mSampleRate;
    
    *loaded = true;
    
    return data;
}



#pragma - AudioChannelNative

AudioChannelNative::AudioChannelNative(AudioPlayerNative *audioPlayerNative) {
    this->audioPlayerNative = audioPlayerNative;
    this->loaded = false;
    alGenSources(1, &this->source);
}

void AudioChannelNative::load(const char *filename, bool cache) {
    if (this->audioData) {
        this->stop();
    }
    
    this->audioData = AudioData::loadAudioData(filename, cache);
    if (this->audioData) {
        alSourcei(this->source, AL_BUFFER, this->audioData->buffer);
        
        ALenum error = alGetError();
        if (error != AL_NO_ERROR) {
            LOGE("alSourcei failed. error=%d", (int)error);
            return;
        }
        
        this->loaded = true;
        alSourceRewind(this->source);
    }
}

void AudioChannelNative::play() {
    if (!this->isLoaded()) {
        LOGE("Audio file is not loaded.");
        return;
    }
    alSourcePlay(this->source);
}

void AudioChannelNative::pause() {
    if (this->getState() != AudioChannel::State::Playing) return;
    alSourcePause(this->source);
}

void AudioChannelNative::resume() {
    if (this->getState() != AudioChannel::State::Paused) return;
    alSourcePlay(this->source);
}

void AudioChannelNative::stop() {
    alSourceStop(this->source);
}

void AudioChannelNative::close() {
    alDeleteSources(1, &this->source);
    this->source = 0;
    this->audioData.reset();
}

void AudioChannelNative::seek(float offset) {
    alSourcef(this->source, AL_SEC_OFFSET, offset);
}

void AudioChannelNative::setLoop(bool isLoop) {
    alSourcei(this->source, AL_LOOPING, (ALuint)isLoop);
}

bool AudioChannelNative::isLoop() {
    ALint loop;
    alGetSourcei(this->source, AL_LOOPING, &loop);
    return loop != 0;
}

void AudioChannelNative::setVolume(float volume) {
    alSourcef(this->source, AL_GAIN, (ALfloat)volume);
}

float AudioChannelNative::getVolume() {
    ALfloat volume;
    alGetSourcef(this->source, AL_GAIN, &volume);
    return (float)volume;
}

bool AudioChannelNative::isLoaded() {
    return this->loaded;
}

AudioChannel::State AudioChannelNative::getState() {
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

AudioPlayerNative::AudioPlayerNative() {
    this->device = alcOpenDevice(NULL);
    this->context = alcCreateContext(this->device, NULL);
    alcMakeContextCurrent(this->context);
}

AudioPlayerNative::~AudioPlayerNative() {
    alcMakeContextCurrent(NULL);
    alcDestroyContext(this->context);
    alcCloseDevice(this->device);
}

void AudioPlayerNative::preload(const char *filename) {
    AudioData::loadAudioData(filename);
}

