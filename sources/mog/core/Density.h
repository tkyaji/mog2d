#ifndef Density_h
#define Density_h

#include <vector>
#include <string>

namespace mog {
    class Density {
        enum class Type {
            x1_0 = 0,
            x1_5,
            x2_0,
            x3_0,
            x4_0,
        };
        
    public:
        static Density getCurrent();
        
        Density(Type type);
        operator int() const {
            return (int)this->type;
        }
        
        static const Density x1_0;
        static const Density x1_5;
        static const Density x2_0;
        static const Density x3_0;
        static const Density x4_0;
        static std::vector<Density> allDensities;
        
        Type type;
        float value;
        std::string directory;
    };
}

#endif /* Density_h */
