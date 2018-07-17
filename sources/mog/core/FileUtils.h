#ifndef FileUtils_h
#define FileUtils_h

#include <string>
#include <vector>
#include "mog/core/Data.h"

using namespace std;

namespace mog {
    class FileUtils {
    public:
        enum class Directory {
            Documents,
            Caches,
        };
        
        static bool existAsset(string filename);
        static string readTextAsset(string filename);
        static std::shared_ptr<ByteArray> readBytesAsset(string filename);

        static std::shared_ptr<ByteArray> readFile(string filename, Directory dir = Directory::Documents);
        static bool writeFile(string filename, const std::shared_ptr<ByteArray> &bytes, Directory dir = Directory::Documents);
        static std::shared_ptr<ByteArray> readDataFromFile(string filepath);
        static bool writeDataToFile(string filepath, const std::shared_ptr<ByteArray> &bytes);

        static string getDocumentsDirectory();
        static string getCachesDirectory();
    };
}

#endif /* FileUtils_h */
