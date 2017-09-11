#ifndef Polygon_h
#define Polygon_h

#include <vector>
#include "mog/base/DrawEntity.h"
#include "mog/core/plain_objects.h"

using namespace std;

namespace mog {
    class Polygon : public DrawEntity {
    public:
        virtual void getVerticesNum(int *num) override;
        virtual void getIndiciesNum(int *num) override;
        virtual void bindVertices(float *vertices, int *idx, bool bakeTransform = false) override;
        virtual void bindIndices(short *indices, int *idx, int start) override;
        virtual void bindVertexTexCoords(float *vertexTexCoords, int *idx, float x, float y, float w, float h) override;
        
    protected:
        Polygon();
        void init(const vector<Point> &vertexPoints);
        
        static shared_ptr<Texture2D> sharedTexture;
        vector<Point> vertexPoints;
        
        Point minPosition = Point::zero;
        Point maxPosition = Point::zero;
        
        virtual void copyFrom(const shared_ptr<Entity> &src) override;
    };
}


#endif /* Polygon_h */
