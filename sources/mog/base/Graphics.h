#ifndef Graphics_h
#define Graphics_h

#include <memory>
#include <vector>
#include "mog/base/Drawable.h"
#include "mog/core/plain_objects.h"

namespace mog {
    class Graphics : public Drawable {
    public:
        static std::shared_ptr<Graphics> create(DrawType drawType, const std::vector<Point> &vertexPositions);
        static std::shared_ptr<Graphics> create(DrawType drawType, const std::vector<Point> &vertexPositions, const std::vector<unsigned int> &vertexIndices);

        void setLineWidth(float width);
        void setPointSize(float size);

        void setVertexPositions(const std::vector<Point> &vertexPositions);
        void setVertexPositions(const std::vector<Point> &vertexPositions, const std::vector<unsigned int> &vertexIndices);
        void setVertexColor(const std::vector<Color> &vertexColors);

    protected:
        Graphics() {}
        
        void init(DrawType drawType, const std::vector<Point> &vertexPositions);
        void init(DrawType drawType, const std::vector<Point> &vertexPositions, const std::vector<unsigned int> &vertexIndices);
        virtual void bindVertex() override;
        void bindVertexColors();

        std::vector<Point> vertexPositions;
        std::vector<unsigned int> vertexIndices;
        std::vector<Color> vertexColors;
    };
}

#endif /* Graphics_h */
