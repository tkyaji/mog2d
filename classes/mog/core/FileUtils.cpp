#include "mog/core/FileUtils.h"
#include "mog/core/FileUtilsNative.h"
#include "mog/core/mog_functions.h"
#include <fstream>
#include <stdlib.h>

using namespace mog;

bool FileUtils::existAsset(string filename) {
    return FileUtilsNative::existAsset(filename);
}

string FileUtils::readTextAsset(string filename) {
    return FileUtilsNative::readTextAsset(filename);
}

bool FileUtils::readBytesAsset(string filename, unsigned char **data, int *len) {
    return FileUtilsNative::readBytesAsset(filename, data, len);
}

bool FileUtils::readFile(string filename, unsigned char **data, int *len, Directory dir) {
    string fileDir = "";
    if (dir == Directory::Documents) {
        fileDir = getDocumentsDirectory();
    } else if (dir == Directory::Caches) {
        fileDir = getCachesDirectory();
    }
    
    string filepath = fileDir + "/" + filename;
    return FileUtils::readDataFromFile(filepath, data, len);
}

bool FileUtils::writeFile(string filename, unsigned char *data, int len, Directory dir) {
    string fileDir = "";
    if (dir == Directory::Documents) {
        fileDir = getDocumentsDirectory();
    } else if (dir == Directory::Caches) {
        fileDir = getCachesDirectory();
    }
    
    string filepath = fileDir + "/" + filename;
    return FileUtils::writeDataToFile(filepath, data, len);
}

bool FileUtils::readDataFromFile(string filepath, unsigned char **data, int *len) {
    std::ifstream ifs;
    ifs.open(filepath, std::ios::in | std::ios_base::binary);
    if (ifs.fail()) {
        LOGE("file open failed: %s", filepath.c_str());
        return false;
    }
    ifs.seekg(0, std::ios::end);
    size_t size = ifs.tellg();
    ifs.seekg(0, ios_base::beg);
    
    bool ret = true;
    char *dt = (char *)malloc(size);
    ifs.read(dt, size);
    if (ifs.bad()) {
        ret = false;
        safe_free(dt);
        LOGE("file read failed: %s", filepath.c_str());
    } else {
        *len = (int)size;
        *data = (unsigned char *)dt;
    }
    ifs.close();
    
    return ret;
}

bool FileUtils::writeDataToFile(string filepath, unsigned char *data, int len) {
    std::ofstream ofs;
    ofs.open(filepath, std::ios::out | std::ios_base::binary);
    if (ofs.fail()) {
        LOGE("file open failed: %s", filepath.c_str());
        return false;
    }
    
    bool ret = true;
    ofs.write((const char *)data, len);
    if (ofs.bad()) {
        ret = false;
        LOGE("file write failed: %s", filepath.c_str());
    }
    ofs.close();
    
    return ret;
}

string FileUtils::getDocumentsDirectory() {
    return FileUtilsNative::getDocumentsDirectory();
}

string FileUtils::getCachesDirectory() {
    return FileUtilsNative::getCachesDirectory();
}
