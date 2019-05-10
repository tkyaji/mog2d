#include "mog/core/FileUtilsNative.h"
#include "mog/core/Engine.h"
#include "mog/os/AndroidHelper.h"
#include "mog/Constants.h"
#include <jni.h>
#include <stdlib.h>

using namespace mog;

bool FileUtilsNative::existAsset(string filename) {
    AAssetManager *aAssetManager = AndroidHelper::getAssetManager();
    AAsset *aAsset = AAssetManager_open(aAssetManager, filename.c_str(), AASSET_MODE_BUFFER);
    bool isExist = (aAsset != nullptr);
    AAsset_close(aAsset);
    return isExist;
}

string FileUtilsNative::readTextAsset(string filename) {
    unsigned char *bytes = nullptr;
    unsigned int len = 0;
    FileUtilsNative::readBytesAsset(filename, &bytes, &len);
    auto str = string((char *)bytes, len);
    mogfree(bytes);
    return str;
}

bool FileUtilsNative::readBytesAsset(string filename, unsigned char **data, unsigned int *len) {
    AAssetManager *aAssetManager = AndroidHelper::getAssetManager();
    AAsset *aAsset = AAssetManager_open(aAssetManager, filename.c_str(), AASSET_MODE_BUFFER);
    if (aAsset == nullptr) {
        *data = nullptr;
        if (len) *len = 0;
        return false;
    }
    unsigned int size = (unsigned int)AAsset_getLength(aAsset);
    *data = (unsigned char *)mogmalloc(size);
    AAsset_read(aAsset, *data, size);
    if (len) *len = size;

    AAsset_close(aAsset);
    return true;
}

string FileUtilsNative::getDocumentsDirectory() {
    auto fileRet = AndroidHelper::mogActivity->execute<NativeObject>("getFilesDir");
    auto pathRet = fileRet->execute<String>("getCanonicalPath");
    return pathRet->getValue();
}

string FileUtilsNative::getCachesDirectory() {
    auto fileRet = AndroidHelper::mogActivity->execute<NativeObject>("getCacheDir");
    auto pathRet = fileRet->execute<String>("getCanonicalPath");
    return pathRet->getValue();
}

