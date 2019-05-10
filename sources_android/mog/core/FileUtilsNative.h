#ifndef FileUtilsNative_h
#define FileUtilsNative_h

#include <string>

namespace mog {
    class FileUtilsNative {
    public:
        static bool existAsset(std::string filename);
        static std::string readTextAsset(std::string filename);
        static bool readBytesAsset(std::string filename, unsigned char **data, unsigned int *len);

        static std::string getDocumentsDirectory();
        static std::string getCachesDirectory();
    };
}

#endif
