#ifndef AlignmentGroup_h
#define AlignmentGroup_h

#include "mog/base/Group.h"

namespace mog {
    
    class AlignmentGroup : public Group {
    public:
        virtual void updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, float *parentRendererMatrix, unsigned char parentDirtyFlag) override;
        virtual void updateFrameForChild(const std::shared_ptr<Engine> &engine, float delta, const std::shared_ptr<Entity> &entity, float *parentMatrix, float *parentRendererMatrix, unsigned char parentDirtyFlag) override;
        virtual void drawFrame(float delta, const std::map<unsigned int, TouchInput> &touches) override;
        virtual void updateMatrix(float *parentMatrix, unsigned char parentDirtyFlag) override;
        
        float getPadding();
        void setPadding(float padding);
        virtual std::shared_ptr<Dictionary> serialize() override;

    protected:
        AlignmentGroup();
        
//        virtual void multiplyChildEntityMatrix(const std::shared_ptr<Entity> &entity, float *parentMatrix) override;
        virtual void deserializeData(const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) override;

        float tmpMatrix[60];
        unsigned char tmpDirtyFlag = 0;
        Point tmpOffset = Point::zero;
        bool alignmentVertical = false;
        bool alignmentHorizontal = false;
        float padding = 0.0f;
    };
    
}

#endif /* AlignmentGroup_h */
