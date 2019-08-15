#ifndef Json_h
#define Json_h

#include <string>
#include "mog/core/Data.h"

namespace mog {
    class Json {
    public:
        static std::shared_ptr<Dictionary> parse(std::string jsonText);
        static std::string toJson(const std::shared_ptr<Dictionary> &dict);
    };
}

#endif /* Json_h */
