#include "mog/Constants.h"
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

std::shared_ptr<ByteArray> FileUtils::readBytesAsset(string filename) {
    unsigned char *data = nullptr;
    unsigned int len = 0;
    FileUtilsNative::readBytesAsset(filename, &data, &len);
    return ByteArray::create(data, len);
}

std::shared_ptr<ByteArray> FileUtils::readFile(string filename, Directory dir) {
    string fileDir = "";
    if (dir == Directory::Documents) {
        fileDir = getDocumentsDirectory();
    } else if (dir == Directory::Caches) {
        fileDir = getCachesDirectory();
    }
    
    string filepath = fileDir + "/" + filename;
    return FileUtils::readDataFromFile(filepath);
}

bool FileUtils::writeFile(string filename, const std::shared_ptr<ByteArray> &bytes, Directory dir) {
    string fileDir = "";
    if (dir == Directory::Documents) {
        fileDir = getDocumentsDirectory();
    } else if (dir == Directory::Caches) {
        fileDir = getCachesDirectory();
    }
    
    string filepath = fileDir + "/" + filename;
    return FileUtils::writeDataToFile(filepath, bytes);
}

std::shared_ptr<ByteArray> FileUtils::readDataFromFile(string filepath) {
    std::ifstream ifs;
    ifs.open(filepath, std::ios::in | std::ios_base::binary);
    if (ifs.fail()) {
        LOGE("file open failed: %s", filepath.c_str());
        return nullptr;
    }
    ifs.seekg(0, std::ios::end);
    unsigned int size = (unsigned int)ifs.tellg();
    ifs.seekg(0, ios_base::beg);
    
    bool ret = true;
    char *dt = (char *)rpmalloc(sizeof(char) * size);
    ifs.read(dt, size);
    std::shared_ptr<ByteArray> data = nullptr;
    if (ifs.bad()) {
        ret = false;
        rpfree(dt);
        LOGE("file read failed: %s", filepath.c_str());
    } else {
        data = ByteArray::create((unsigned char *)dt, (unsigned int)size);
    }
    ifs.close();
    
    return data;
}

bool FileUtils::writeDataToFile(string filepath, const std::shared_ptr<ByteArray> &bytes) {
    std::ofstream ofs;
    ofs.open(filepath, std::ios::out | std::ios_base::binary);
    if (ofs.fail()) {
        LOGE("file open failed: %s", filepath.c_str());
        return false;
    }
    
    bool ret = true;
    unsigned char *value;
    unsigned int length;
    bytes->getValue(&value, &length);
    ofs.write((const char *)value, length);
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

