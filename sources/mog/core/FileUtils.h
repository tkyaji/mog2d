#ifndef FileUtils_h
#define FileUtils_h

#include <string>
#include <vector>
#include "mog/core/Data.h"

namespace mog {
    class FileUtils {
    public:
        enum class Directory {
            Documents,
            Caches,
        };
        
        static bool existAsset(std::string filename);
        static std::string readTextAsset(std::string filename);
        static std::shared_ptr<ByteArray> readBytesAsset(std::string filename);

        static std::shared_ptr<ByteArray> readFile(std::string filename, Directory dir = Directory::Documents);
        static bool writeFile(std::string filename, const std::shared_ptr<ByteArray> &bytes, Directory dir = Directory::Documents);
        static std::shared_ptr<ByteArray> readBytesFromFile(std::string filepath);
        static bool writeBytesToFile(std::string filepath, const std::shared_ptr<ByteArray> &bytes);

        static std::string getDocumentsDirectory();
        static std::string getCachesDirectory();
    };
}

#endif /* FileUtils_h */
