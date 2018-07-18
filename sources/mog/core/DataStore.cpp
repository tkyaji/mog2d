#include "mog/core/DataStore.h"
#include <dirent.h>

using namespace mog;

#pragma - DataStore

unordered_map<string, shared_ptr<Data>> DataStore::caches;
unordered_map<string, bool> DataStore::unsaved;
mutex DataStore::mtx;

void DataStore::setData(string key, const std::shared_ptr<Data> &value, bool immediatelySave) {
    std::lock_guard<std::mutex> lock(mtx);
    
    DataStore::caches[key] = value;
    if (immediatelySave) {
        _save(key);
    } else {
        DataStore::unsaved[key] = true;
    }
}

void DataStore::serialize(string filepath, const std::shared_ptr<Data> &data) {
    string tmp = filepath + ".tmp";
    ofstream fout;
    fout.exceptions(ios::failbit|ios::badbit);
    fout.open(tmp, ios::out|ios::binary);
    data->write(fout);
    fout.flush();
    fout.close();
    const char *filec = filepath.c_str();
    const char *tmpc = tmp.c_str();
    if (std::rename(tmpc, filec) != 0) {
        std::remove(filec);
        if (std::rename(tmpc, filec) != 0) {
            throw std::ios_base::failure("failed to write file.");
        }
    }
}

void DataStore::serialize(unsigned char **byteData, int *len, const std::shared_ptr<Data> &data) {
    ostringstream sout(ios::binary);
    sout.exceptions(ios::failbit|ios::badbit);
    data->write(sout);
    const char *sdata = sout.str().data();
    size_t size = sout.str().size();
    *len = (int)size;
    *byteData = (unsigned char *)malloc(size);
    memcpy(*byteData, sdata, size);
}

void DataStore::_serialize(string key, const std::shared_ptr<Data> &data) {
    string dirStr = getStoreDirectory();
    const char *dir = dirStr.c_str();
    struct stat st;
    if (stat(dir, &st) == -1) {
        mkdir(dir, S_IRWXU|S_IRWXG);
    }
    string file = getStoreFilePath(key);
    serialize(file, data);
}

bool DataStore::hasKey(string key) {
    std::lock_guard<std::mutex> lock(mtx);
    return _hasKey(key);
}

bool DataStore::_hasKey(string key) {
    if (DataStore::caches.count(key) > 0) return true;
    
    string file = getStoreFilePath(key);
    struct stat st;
    return (stat(file.c_str(), &st) == 0);
}

void DataStore::remove(string key) {
    std::lock_guard<std::mutex> lock(mtx);
    _remove(key);
}

void DataStore::_remove(string key) {
    string file = getStoreFilePath(key);
    std::remove(file.c_str());
}

void DataStore::removeAll() {
    std::lock_guard<std::mutex> lock(mtx);
    _removeAll();
}

void DataStore::_removeAll() {
    DIR* dp = opendir(getStoreDirectory().c_str());
    if (dp == NULL) return;
    
    struct dirent* dt;
    while ((dt = readdir(dp)) != NULL) {
        if (dt->d_type == DT_REG) {
            string file = getStoreDirectory() + dt->d_name;
            std::remove(file.c_str());
        }
    }
    std::remove(getStoreDirectory().c_str());
    DataStore::caches.clear();
}

void DataStore::save() {
    std::lock_guard<std::mutex> lock(mtx);
    _save();
}

void DataStore::_save() {
    for (auto &kv : DataStore::unsaved) {
        if (kv.second) {
            auto data = DataStore::caches[kv.first];
            DataStore::_serialize(kv.first, data);
        }
    }
    DataStore::unsaved.clear();
}

void DataStore::save(string key) {
    std::lock_guard<std::mutex> lock(mtx);
    _save(key);
}

void DataStore::_save(string key) {
    auto data = DataStore::caches[key];
    if (data) {
        DataStore::_serialize(key, data);
        DataStore::unsaved.erase(key);
    }
}

void DataStore::clearCache() {
    DataStore::caches.clear();
}

