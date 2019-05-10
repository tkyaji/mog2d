#include "mog/core/FileUtilsNative.h"
#include "mog/Constants.h"
#include "mog/core/Engine.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <sys/stat.h>

using namespace mog;

static bool _existAsset(const char *filepath) {
    struct stat st;
    if (stat(filepath, &st) != 0) {
        return 0;
    }
    return (st.st_mode & S_IFMT) == S_IFREG;
}

static std::string _readTextAsset(const char *filepath) {
    auto fin = std::ifstream(filepath);
    if (!fin) return "";

    return std::string((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
}

static bool _readBytesAsset(const char *filepath, unsigned char **data, unsigned int *len) {
    ifstream ifs;
    ifs.exceptions(ios::failbit|ios::badbit);
    ifs.open(filepath, ios::in|ios::binary);
    ifs.seekg(0, std::ios::end);
    unsigned int size = (unsigned int)ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);
    char *dt = (char *)mogmalloc(sizeof(char) * size);
    ifs.read(dt, size);
    bool ret = false;
    if (ifs.bad()) {
        mogfree(dt);
        *data = nullptr;
        *len = 0;
    } else {
        *data = (unsigned char *)dt;
        *len = size;
        ret = true;
    }
    ifs.close();
    return ret;
}

bool FileUtilsNative::existAsset(string filename) {
    char filepath[256];
    sprintf(filepath, "assets_emscripten/%s", filename.c_str());
    if (_existAsset(filepath)) return true;

    memset(filepath, 0, 256);
    sprintf(filepath, "assets/%s", filename.c_str());
    return _existAsset(filepath);
}

std::string FileUtilsNative::readTextAsset(string filename) {
    char filepath[256];
    sprintf(filepath, "assets_emscripten/%s", filename.c_str());
    if (_existAsset(filepath)) {
        return _readTextAsset(filepath);
    }

    memset(filepath, 0, 256);
    sprintf(filepath, "assets/%s", filename.c_str());
    if (_existAsset(filepath)) {
        return _readTextAsset(filepath);
    }
    return "";
}

bool FileUtilsNative::readBytesAsset(string filename, unsigned char **data, unsigned int *len) {
    char filepath[256];
    sprintf(filepath, "assets_emscripten/%s", filename.c_str());
    if (_existAsset(filepath)) {
        return _readBytesAsset(filepath, data, len);
    }

    memset(filepath, 0, 256);
    sprintf(filepath, "assets/%s", filename.c_str());
    if (_existAsset(filepath)) {
        return _readBytesAsset(filepath, data, len);
    }

    *data = nullptr;
    *len = 0;
    return false;
}

string FileUtilsNative::getDocumentsDirectory() {
    return "/";
}

string FileUtilsNative::getCachesDirectory() {
    return "/";
}
