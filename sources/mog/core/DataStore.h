#ifndef DataStore_h
#define DataStore_h

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <mutex>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "mog/Constants.h"
#include "mog/core/FileUtils.h"
#include "mog/core/Data.h"
#include "mog/core/DataStore.h"
#include "mog/libs/sha256.h"

namespace mog {
    
    class DataStore {
    public:
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        static std::shared_ptr<T> getData(std::string key, const std::shared_ptr<T> &defaultValue) {
            std::lock_guard<std::mutex> lock(mtx);
            
            if (!_hasKey(key)) {
                return defaultValue;
            }
            
            auto data = DataStore::caches[key];
            if (data) {
                return std::static_pointer_cast<T>(data);
            } else {
                return _deserialize<T>(key);
            }
        }
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        static std::shared_ptr<T> getData(std::string key) {
            auto defaultValue = std::shared_ptr<T>(new T());
            return getData<T>(key, defaultValue);
        }
        
        static void setData(std::string key, const std::shared_ptr<Data> &value, bool immediatelySave = false);
        static void serialize(std::string filepath, const std::shared_ptr<Data> &data);
        static void serialize(unsigned char **byteData, int *len, const std::shared_ptr<Data> &data);

        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        static std::shared_ptr<T> deserialize(std::string filepath) {
            auto data = std::shared_ptr<T>(new T());
            std::ifstream fin;
            fin.exceptions(std::ios::failbit|std::ios::badbit);
            fin.open(filepath, std::ios::in|std::ios::binary);
            data->read(fin);
            fin.close();
            return data;
        }
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        static std::shared_ptr<T> deserialize(const std::shared_ptr<ByteArray> &bytes) {
            auto data = std::shared_ptr<T>(new T());
            unsigned char *byteData = nullptr;
            unsigned int len = 0;
            bytes->getValue(&byteData, &len);
            auto str = std::string(byteData, byteData + len);
            std::istringstream sin(str, std::ios::binary);
            sin.exceptions(std::ios::failbit|std::ios::badbit);
            data->read(sin);
            return data;
        }
        
        static std::string getStoreDirectory() {
            return mog::FileUtils::getDocumentsDirectory() + "/data_store/";
        }
        static std::string getStoreFilePath(std::string key) {
            return getStoreDirectory() + sha256(key);
        }
        
        static bool hasKey(std::string key);
        static void remove(std::string key);
        static void removeAll();
        static void save();
        static void save(std::string key);
        static void clearCache();
        
    private:
        static std::unordered_map<std::string, std::shared_ptr<Data>> caches;
        static std::unordered_map<std::string, bool> unsaved;
        static std::mutex mtx;
        
        static bool _hasKey(std::string key);
        static void _remove(std::string key);
        static void _removeAll();
        static void _save();
        static void _save(std::string key);
        
        static void _serialize(std::string key, const std::shared_ptr<Data> &data);
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        static std::shared_ptr<T> _deserialize(std::string key) {
            std::shared_ptr<T> d = nullptr;
            std::string file = getStoreFilePath(key);
            struct stat st;
            if (stat(file.c_str(), &st) != 0) return d;
            
            d = deserialize<T>(file);
            DataStore::caches[key] = d;
            return d;
        }
        
        
    };
}

#endif /* DataStore_h */

