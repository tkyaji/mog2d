#ifndef FileUtilsNative_h
#define FileUtilsNative_h

#include <string>

using namespace std;

namespace mog {
    class FileUtilsNative {
    public:
        static string readTextAsset(string filename);
        static bool readBytesAsset(string filename, unsigned char **data, int *len);
        
        static string getDocumentsDirectory();
        static string getCachesDirectory();
    };
}

#endif
