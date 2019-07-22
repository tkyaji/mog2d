#ifndef MogUILoader_h
#define MogUILoader_h

#include <memory>
#include <string>
#include "mog/core/Data.h"
#include "mog/base/Group.h"

extern void *enabler;

namespace mog {
    class MogUILoader {
    public:
        class Param {
        public:
            std::string name;
            std::string propertyKey;
            std::shared_ptr<Data> propertyValue;
        };
        
        static std::shared_ptr<Group> load(std::string filename);
        static std::shared_ptr<Group> load(std::string filename, const std::vector<Param> &params);

        template<class First, class... Rest, typename std::enable_if<std::is_base_of<Param, First>::value>::type*& = enabler>
        static std::shared_ptr<Group> load(std::string filename, const First &first, const Rest&... rest) {
            std::vector<Param> params;
            addParam(params, first, rest...);
            load(filename, params);
        }

        static std::string toJsonString(const std::shared_ptr<Dictionary> &dict);
        
    private:
        static void dictToGroupJsonString(const std::shared_ptr<Dictionary> &dict, std::string &jsonStr);
        
        template<class First, class... Rest, typename std::enable_if<std::is_base_of<Param, First>::value>::type*& = enabler>
        static void addParam(const std::vector<Param> &params, const First &first, const Rest&... rest) {
            params.emplace_back(first);
            addParam(params, rest...);
        }
        static void addParam(const std::vector<Param> &params) { }
    };
}

#endif /* MogUILoader_h */
