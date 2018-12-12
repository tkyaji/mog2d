#ifndef Transform_h
#define Transform_h

#include <memory>
#include <math.h>
#include "mog/core/plain_objects.h"

namespace mog {
    
    class Transform : public std::enable_shared_from_this<Transform> {
    public:
        static void multiplyMatrix(float *matrix1, float *matrix2, float *dstMatrix);
        static void multiplyColor(float *matrix1, float *matrix2, float *dstMatrix);
        static bool inverseMatrix(float *matrix, float *dstMatrix);

        Point anchor = Point::zero;
        Point position = Point::zero;
        Size size = Size::zero;
        Point scale = Point::one;
        float rotation = 0;
        Color color = Color::white;
        float matrix[20] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
            1, 1, 1, 1,
        };
        float tmpMatrix[48] = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };
        void updateMatrix(float *parentMatrix = nullptr);
        void updateColor(float *parentMatrix = nullptr);
        void setMatrix(float *matrix);
    };
}


#endif /* Transform_h */
