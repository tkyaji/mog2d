#include "mog/base/Graphics.h"
#include "mog/core/Engine.h"

using namespace mog;

Graphics::Graphics() {
}

Graphics::~Graphics() {
}

std::shared_ptr<Graphics> Graphics::create(DrawType drawType, const vector<Point> &vertexPositions) {
    auto graphics = std::shared_ptr<Graphics>(new Graphics());
    graphics->init(drawType, vertexPositions);
    return graphics;
}

std::shared_ptr<Graphics> Graphics::create(DrawType drawType, const std::vector<Point> &vertexPositions, const std::vector<unsigned int> &vertexIndices) {
    auto graphics = std::shared_ptr<Graphics>(new Graphics());
    graphics->init(drawType, vertexPositions, vertexIndices);
    return graphics;
}

void Graphics::init(DrawType drawType, const std::vector<Point> &vertexPositions) {
    this->renderer->setDrawType(drawType);
    this->setVertexPositions(vertexPositions);
    
    int num = (int)vertexPositions.size();
    this->renderer->setVerticesNum(num);
    this->renderer->setIndicesNum(num);
    this->renderer->newVerticesArr();
    this->renderer->newIndicesArr();
}

void Graphics::init(DrawType drawType, const std::vector<Point> &vertexPositions, const std::vector<unsigned int> &vertexIndices) {
    this->renderer->setDrawType(drawType);
    this->setVertexPositions(vertexPositions, vertexIndices);
    
    this->renderer->setVerticesNum((int)vertexPositions.size());
    this->renderer->setIndicesNum((int)vertexIndices.size());
    this->renderer->newVerticesArr();
    this->renderer->newIndicesArr();
}

void Graphics::setLineWidth(float width) {
    float scale = Engine::getInstance()->getScreenScale();
    this->renderer->setLineWidth(width * scale);
}

void Graphics::setPointSize(float size) {
    float scale = Engine::getInstance()->getScreenScale();
    this->renderer->setUniformPointSize(size * scale);
}

void Graphics::setVertexPositions(const std::vector<Point> &vertexPositions) {
    this->vertexPositions = vertexPositions;
    std::vector<unsigned int> vertexIndices;
    vertexIndices.reserve(vertexPositions.size());
    for (int i = 0; i < vertexPositions.size(); i++) {
        vertexIndices.emplace_back(i);
    }
    this->setVertexPositions(vertexPositions, vertexIndices);
}

void Graphics::setVertexPositions(const std::vector<Point> &vertexPositions, const std::vector<unsigned int> &vertexIndices) {
    this->vertexPositions = vertexPositions;
    this->vertexIndices = vertexIndices;
    
    if (vertexPositions.size() > 0) {
        Point minPos = vertexPositions[0];
        Point maxPos = vertexPositions[0];
        for (auto pos : vertexPositions) {
            minPos.x = fmin(minPos.x, pos.x);
            minPos.y = fmin(minPos.y, pos.y);
            maxPos.x = fmax(maxPos.x, pos.x);
            maxPos.y = fmax(maxPos.y, pos.y);
        }
        this->setSize(maxPos - minPos);
    }

    this->reRenderFlag |= (RERENDER_VERTEX | RERENDER_COLOR);
}

void Graphics::setVertexColor(const std::vector<Color> &vertexColors) {
    this->vertexColors = vertexColors;
    this->reRenderFlag |= RERENDER_COLOR;
}

void Graphics::bindVertex() {
    if (this->reRenderFlag == 0) return;
    
    if ((this->reRenderFlag & RERENDER_VERTEX) == RERENDER_VERTEX) {
        for (int i = 0; i < this->vertexPositions.size(); i++) {
            auto pos = this->vertexPositions[i];
            this->renderer->indices[i] = i;
            this->renderer->vertices[i * 2 + 0] = pos.x;
            this->renderer->vertices[i * 2 + 1] = pos.y;
        }
        for (int i = 0; i < this->vertexIndices.size(); i++) {
            this->renderer->indices[i] = vertexIndices[i];
        }
        this->renderer->bindVertex(true);
    }
    
    this->bindVertexColors();
    
    this->reRenderFlag = 0;
}

void Graphics::bindVertexColors() {
    if (this->vertexColors.size() == 0) return;
    
    if ((this->reRenderFlag & RERENDER_COLOR) == RERENDER_COLOR) {
        this->renderer->newVertexColorsArr();
        for (int i = 0; i < this->vertexPositions.size(); i++) {
            Color color = this->transform->color;
            if (i < this->vertexColors.size()) {
                color = this->vertexColors[i];
            }
            this->renderer->vertexColors[i * 4 + 0] = color.r;
            this->renderer->vertexColors[i * 4 + 1] = color.g;
            this->renderer->vertexColors[i * 4 + 2] = color.b;
            this->renderer->vertexColors[i * 4 + 3] = color.a;
        }
        this->renderer->bindVertexColors();
    }
}
