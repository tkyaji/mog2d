#ifndef HorizontalGroup_h
#define HorizontalGroup_h

#include "mog/base/AlignmentGroup.h"

namespace mog {
    
    class HorizontalGroup : public AlignmentGroup {
        friend class EntityCreator;
    public:
        static std::shared_ptr<HorizontalGroup> create(float padding = 0.0f, bool enableBatching = false);
        
        std::shared_ptr<HorizontalGroup> clone();
        virtual std::shared_ptr<Dictionary> serialize() override;
        
    protected:
        HorizontalGroup();
    };
    
}

#endif /* HorizontalGroup_h */
