#ifndef VerticalGroup_h
#define VerticalGroup_h

#include "mog/base/AlignmentGroup.h"

namespace mog {
    
    class VerticalGroup : public AlignmentGroup {
        friend class EntityCreator;
    public:
        static std::shared_ptr<VerticalGroup> create(float padding = 0.0f, bool enableBatching = false);

        std::shared_ptr<VerticalGroup> clone();
        virtual std::shared_ptr<Dictionary> serialize() override;
        
    protected:
        VerticalGroup();
    };
    
}

#endif /* VerticalGroup_h */
