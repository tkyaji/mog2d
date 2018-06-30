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
        static Bytes readBytesAsset(string filename);

        static Bytes readFile(string filename, Directory dir = Directory::Documents);
        static bool writeFile(string filename, const Bytes &bytes, Directory dir = Directory::Documents);
        static Bytes readDataFromFile(string filepath);
        static bool writeDataToFile(string filepath, const Bytes &bytes);

        static string getDocumentsDirectory();
        static string getCachesDirectory();
    };
}

#endif /* FileUtils_h */
