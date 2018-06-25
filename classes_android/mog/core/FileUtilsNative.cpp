#include "mog/core/FileUtilsNative.h"
#include "mog/core/Engine.h"
#include "mog/os/AndroidHelper.h"
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
    int len = 0;
    FileUtilsNative::readBytesAsset(filename, &bytes, &len);
    auto str = string((char *)bytes, len);
    free(bytes);
    return str;
}

bool FileUtilsNative::readBytesAsset(string filename, unsigned char **data, int *len) {
    AAssetManager *aAssetManager = AndroidHelper::getAssetManager();
    AAsset *aAsset = AAssetManager_open(aAssetManager, filename.c_str(), AASSET_MODE_BUFFER);
    if (aAsset == nullptr) {
        *data = nullptr;
        if (len) *len = 0;
        return false;
    }
    size_t size = (int)AAsset_getLength(aAsset);
    *data = (unsigned char *)malloc(size);
    AAsset_read(aAsset, *data, size);
    if (len) *len = (int)size;

    AAsset_close(aAsset);
    return true;
}

string FileUtilsNative::getDocumentsDirectory() {
    auto fileRet = AndroidHelper::getActivity()->execute("getFilesDir", NRet::Type::Object);
    auto pathRet = fileRet.o->execute("getCanonicalPath", NRet::Type::Object);
    return NUtils::toString(pathRet);
}

string FileUtilsNative::getCachesDirectory() {
    auto fileRet = AndroidHelper::getActivity()->execute("getCacheDir", NRet::Type::Object);
    auto pathRet = fileRet.o->execute("getCanonicalPath", NRet::Type::Object);
    return NUtils::toString(pathRet);
}

