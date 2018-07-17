#include "mog/core/DataStore.h"
#include <dirent.h>

using namespace mog;

#pragma - DataStore

unordered_map<string, shared_ptr<Data>> DataStore::caches;
unordered_map<string, bool> DataStore::unsaved;
mutex DataStore::mtx;

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
            DataStore::_serialize(kv.first, *data.get());
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
        DataStore::_serialize(key, *data.get());
        DataStore::unsaved.erase(key);
    }
}

void DataStore::clearCache() {
    DataStore::caches.clear();
}

