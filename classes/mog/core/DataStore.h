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
#include <sys/stat.h>
#include "mog/core/FileUtils.h"
#include "mog/core/Data.h"
#include "mog/libs/sha256.h"

using namespace std;

namespace mog {
    
    class DataStore {
    public:
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        static T getData(string key, const T &defaultValue) {
            std::lock_guard<std::mutex> lock(mtx);
            
            if (!_hasKey(key)) {
                return defaultValue;
            }
            
            auto data = DataStore::caches[key];
            if (data) {
                return *static_pointer_cast<T>(data).get();
            } else {
                return _deserialize<T>(key);
            }
        }
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        static T getData(string key) {
            auto defaultValue = T();
            defaultValue.type = DataType::Null;
            return getData<T>(key, defaultValue);
        }
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        static void setData(string key, const T &value, bool immediatelySave = false) {
            std::lock_guard<std::mutex> lock(mtx);
            
            DataStore::caches[key] = shared_ptr<T>(new T(value));
            if (immediatelySave) {
                _save(key);
            } else {
                DataStore::unsaved[key] = true;
            }
        }
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        static void serialize(string filepath, T &data) {
            string tmp = filepath + ".tmp";
            ofstream fout;
            fout.exceptions(ios::failbit|ios::badbit);
            fout.open(tmp, ios::out|ios::binary);
            data.write(fout);
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

        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        static void serialize(unsigned char **byteData, int *len, T &data) {
            ostringstream sout(ios::binary);
            sout.exceptions(ios::failbit|ios::badbit);
            data.write(sout);
            const char *sdata = sout.str().data();
            size_t size = sout.str().size();
            *len = (int)size;
            *byteData = (unsigned char *)malloc(size);
            memcpy(*byteData, sdata, size);
        }

        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        static T deserialize(string filepath) {
            T data;
            ifstream fin;
            fin.exceptions(ios::failbit|ios::badbit);
            fin.open(filepath, ios::in|ios::binary);
            data.read(fin);
            fin.close();
            return data;
        }
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        static T deserialize(unsigned char *byteData, int len) {
            T data;
            auto str = std::string(byteData, byteData + len);
            istringstream sin(str, ios::binary);
            sin.exceptions(ios::failbit|ios::badbit);
            data.read(sin);
            return data;
        }
        
        static string getStoreDirectory() {
            return mog::FileUtils::getDocumentsDirectory() + "/data_store/";
        }
        static string getStoreFilePath(string key) {
            return getStoreDirectory() + sha256(key);
        }
        
        static bool hasKey(string key);
        static void remove(string key);
        static void removeAll();
        static void save();
        static void save(string key);
        static void clearCache();
        
    private:
        static unordered_map<string, shared_ptr<Data>> caches;
        static unordered_map<string, bool> unsaved;
        static mutex mtx;
        
        static bool _hasKey(string key);
        static void _remove(string key);
        static void _removeAll();
        static void _save();
        static void _save(string key);
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        static void _serialize(string key, T &data) {
            string dirStr = getStoreDirectory();
            const char *dir = dirStr.c_str();
            struct stat st;
            if (stat(dir, &st) == -1) {
                mkdir(dir, S_IRWXU|S_IRWXG);
            }
            string file = getStoreFilePath(key);
            serialize(file, data);
        }
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        static T _deserialize(string key) {
            T d;
            string file = getStoreFilePath(key);
            struct stat st;
            if (stat(file.c_str(), &st) != 0) return d;
            
            d = deserialize<T>(file);
            DataStore::caches[key] = shared_ptr<T>(new T(d));
            return d;
        }
        
        
    };
}

#endif /* DataStore_h */

