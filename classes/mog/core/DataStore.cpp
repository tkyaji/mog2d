#include "mog/core/DataStore.h"
#include <dirent.h>

using namespace mog;

#pragma - DataStore

unordered_map<string, shared_ptr<Data>> DataStore::caches;
unordered_map<string, bool> DataStore::unsaved;

bool DataStore::hasKey(string key) {
    if (DataStore::caches.count(key) > 0) return true;
    
    string file = _getStoreFilePath(key);
    struct stat st;
    return (stat(file.c_str(), &st) == 0);
}

void DataStore::remove(string key) {
    string file = _getStoreFilePath(key);
    std::remove(file.c_str());
}

void DataStore::removeAll() {
    DIR* dp = opendir(_getStoreDirectory().c_str());
    if (dp == NULL) return;
    
    struct dirent* dt;
    while ((dt = readdir(dp)) != NULL) {
        if (dt->d_type == DT_REG) {
            string file = _getStoreDirectory() + dt->d_name;
            std::remove(file.c_str());
        }
    }
    std::remove(_getStoreDirectory().c_str());
}

void DataStore::save() {
    for (auto &kv : DataStore::unsaved) {
        if (kv.second) {
            auto data = DataStore::caches[kv.first];
            DataStore::serialize(kv.first, *data.get());
        }
    }
    DataStore::unsaved.clear();
}

void DataStore::save(string key) {
    auto data = DataStore::caches[key];
    if (data) {
        DataStore::serialize(key, *data.get());
        DataStore::unsaved.erase(key);
    }
}

void DataStore::enableEncryption(string key) {
    
}
