#ifndef FileUtils_h
#define FileUtils_h

#include <string>

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
        static bool readBytesAsset(string filename, unsigned char **data, int *len);
        
        static bool readFile(string filename, unsigned char **data, int *len, Directory dir = Directory::Documents);
        static bool writeFile(string filename, unsigned char *data, int len, Directory dir = Directory::Documents);
        static bool readDataFromFile(string filepath, unsigned char **data, int *len);
        static bool writeDataToFile(string filepath, unsigned char *data, int len);

        static string getDocumentsDirectory();
        static string getCachesDirectory();
    };
}

#endif /* FileUtils_h */
