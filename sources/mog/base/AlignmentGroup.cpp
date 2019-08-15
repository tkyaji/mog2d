#include "mog/base/AlignmentGroup.h"

using namespace mog;

AlignmentGroup::AlignmentGroup() {
    memcpy(this->tmpMatrix, Renderer::identityMatrix, sizeof(float) * 20);
}

void AlignmentGroup::updateFrame(const std::shared_ptr<Engine> &engine, float delta, float *parentMatrix, float *parentRendererMatrix, unsigned char parentDirtyFlag) {
    this->tmpMatrix[12] = 0;
    this->tmpMatrix[13] = 0;
    this->tmpDirtyFlag = 0;
    Group::updateFrame(engine, delta, parentMatrix, parentRendererMatrix, parentDirtyFlag);
}

void AlignmentGroup::updateFrameForChild(const std::shared_ptr<Engine> &engine, float delta, const std::shared_ptr<Entity> &entity, float *parentMatrix, float *parentRendererMatrix, unsigned char parentDirtyFlag) {
    this->tmpMatrix[12] = this->tmpOffset.x;
    this->tmpMatrix[13] = this->tmpOffset.y;
    Transform::multiplyMatrix(parentMatrix, &this->tmpMatrix[0], &this->tmpMatrix[20]);
    Transform::multiplyColor(parentMatrix, &this->tmpMatrix[0], &this->tmpMatrix[20]);
    Transform::multiplyMatrix(parentRendererMatrix, &this->tmpMatrix[0], &this->tmpMatrix[40]);
    Transform::multiplyColor(parentRendererMatrix, &this->tmpMatrix[0], &this->tmpMatrix[40]);

    Group::updateFrameForChild(engine, delta, entity, &this->tmpMatrix[20], &this->tmpMatrix[40], (parentDirtyFlag | this->tmpDirtyFlag));
    
    auto pos = entity->getPosition(Point::zero, Point::zero);
    auto size = entity->getAbsoluteSize();
    if (this->alignmentHorizontal) {
        this->tmpOffset.x += pos.x + size.width + this->padding;
    }
    if (this->alignmentVertical) {
        this->tmpOffset.y += pos.y + size.height + this->padding;
    }
    this->tmpDirtyFlag |= (entity->getDirtyFlag() & DIRTY_VERTEX);
}

void AlignmentGroup::updateMatrix(float *parentMatrix, unsigned char parentDirtyFlag) {
    Entity::updateMatrix(parentMatrix, parentDirtyFlag);
    unsigned char dirtyFlag = (this->dirtyFlag | parentDirtyFlag);
    float matrix[16];
    Transform::multiplyMatrix(this->matrix, parentMatrix, matrix);
    
    Point offset = Point::zero;
    float tmpMatrix[32];
    memcpy(tmpMatrix, Renderer::identityMatrix, sizeof(float) * 16);
    
    for (const auto &drawable : this->drawableContainer->sortedChildDrawables) {
        auto entity = std::static_pointer_cast<Entity>(drawable);
        tmpMatrix[12] = offset.x;
        tmpMatrix[13] = offset.y;
        Transform::multiplyMatrix(matrix, &tmpMatrix[0], &tmpMatrix[16]);

        drawable->updateMatrix(&tmpMatrix[16], dirtyFlag);

        auto entityTr = entity->getTransform();
        Point basePosition = entityTr->position - entityTr->size * entityTr->scale * entityTr->pivot;
        auto pos = basePosition + entityTr->size * entityTr->scale * entityTr->pivot + entityTr->offset;
        
        auto entityMtx = entity->getMatrix();
        float scaleX = sqrt(entityMtx[0] * entityMtx[0] +
                            entityMtx[1] * entityMtx[1]);
        float scaleY = sqrt(entityMtx[4] * entityMtx[4] +
                            entityMtx[5] * entityMtx[5]);
        auto size = entityTr->size * Point(scaleX, scaleY);
        
        if (this->alignmentHorizontal) {
            offset.x += pos.x + size.width + this->padding;
        }
        if (this->alignmentVertical) {
            offset.y += pos.y + size.height + this->padding;
        }
    }
}

void AlignmentGroup::drawFrame(float delta, const std::map<unsigned int, TouchInput> &touches) {
    this->tmpOffset = Point::zero;
    this->dirtyFlag |= this->dirtyFlagChildren;
    Group::drawFrame(delta, touches);
}

float AlignmentGroup::getPadding() {
    return this->padding;
}

void AlignmentGroup::setPadding(float padding) {
    this->padding = padding;
    this->dirtyFlag |= DIRTY_VERTEX;
}

/*
void AlignmentGroup::multiplyChildEntityMatrix(const std::shared_ptr<Entity> &entity, float *parentMatrix) {
    if (this->dirtyFlag == 0) return;
    
    memcpy(&this->tmpMatrix[0], Renderer::identityMatrix, sizeof(float) * 20);
    this->tmpMatrix[12] = this->tmpOffset.x;
    this->tmpMatrix[13] = this->tmpOffset.y;
    Transform::multiplyMatrix(parentMatrix, &this->tmpMatrix[0], &this->tmpMatrix[20]);
    Transform::multiplyColor(parentMatrix, &this->tmpMatrix[0], &this->tmpMatrix[20]);
    
    Group::multiplyChildEntityMatrix(entity, &this->tmpMatrix[20]);
    
    auto pos = entity->getPosition(Point::zero);
    auto size = entity->getAbsoluteSize();
    if (this->alignmentHorizontal) {
        this->tmpOffset.x += pos.x + size.width + this->padding;
    }
    if (this->alignmentVertical) {
        this->tmpOffset.y += pos.y + size.height + this->padding;
    }
}
*/

std::shared_ptr<Dictionary> AlignmentGroup::serialize() {
    auto dict = Group::serialize();
    dict->put(PROP_KEY_PADDING, Float::create(this->padding));
    return dict;
}

void AlignmentGroup::deserializeData(const std::shared_ptr<Dictionary> &dict, const std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<Data>>> &params) {
    Group::deserializeData(dict, params);
    this->padding = this->getPropertyData<Float>(dict, PROP_KEY_PADDING, params)->getValue();
}
