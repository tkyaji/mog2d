#ifndef Density_h
#define Density_h

#include <vector>
#include <string>

namespace mog {
    class Density {
    public:
        static Density getCurrent();
        
        Density(int idx);
        operator int() const {
            return this->idx;
        }
        
        static const Density x1_0;
        static const Density x1_5;
        static const Density x2_0;
        static const Density x3_0;
        static const Density x4_0;
        static std::vector<Density> allDensities;
        
        int idx;
        float value;
        std::string directory;
    };
}

#endif /* Density_h */
