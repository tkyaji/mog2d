#include "mog/core/MogStats.h"
#include "mog/core/Engine.h"
#include "mog/base/AppBase.h"
#include <math.h>

using namespace mog;

#define FPS 0
#define DELTA 1
#define DRAW_CALL 2
#define INSTANTS 3
#define ALPHA 150
#define INTERVAL 0.2f

int MogStats::drawCallCount = 0;
int MogStats::instanceCount = 0;

shared_ptr<MogStats> MogStats::create(bool enable) {
    auto stats = shared_ptr<MogStats>(new MogStats());
    stats->enable = enable;
    stats->setAlignment(Alignment::BottomLeft);
    return stats;
}

void MogStats::drawFrame(const shared_ptr<Engine> &engine, float delta) {
    if (!this->enable) return;
    
    this->screenScale = engine->getScreenScale();
    this->screenSize = engine->getScreenSize();
    if (!this->initialized) {
        this->init();
    }
    if (this->dirtyPosition) {
        this->updatePosition();
    }

    this->tmpDelta += delta;
    if (this->tmpDelta >= INTERVAL) {
        this->updateValues(delta);
        this->texture->bindTexture();
        this->tmpDelta = 0;
    }
    this->renderer->drawFrame(this->transform, this->screenScale);
}

bool MogStats::isEnabled() {
    return this->enable;
}

void MogStats::setEnable(bool enable) {
    this->enable = enable;
}

void MogStats::setAlignment(Alignment alignment) {
    this->alignment = alignment;
    this->dirtyPosition = true;
}

void MogStats::updatePosition() {
    if (!this->enable) return;
    
    auto width = this->width / this->screenScale;
    auto height = this->height / this->screenScale;
    
    switch (this->alignment) {
        case Alignment::TopLeft:
        case Alignment::MiddleLeft:
        case Alignment::BottomLeft:
            this->transform->position.x = 0;
            break;
        case Alignment::TopCenter:
        case Alignment::MiddleCenter:
        case Alignment::BottomCenter:
            this->transform->position.x = this->screenSize.width * 0.5f - width * 0.5f;
            break;
        case Alignment::TopRight:
        case Alignment::MiddleRight:
        case Alignment::BottomRight:
            this->transform->position.x = this->screenSize.width - width;
            break;
    }
    switch (alignment) {
        case Alignment::TopLeft:
        case Alignment::TopCenter:
        case Alignment::TopRight:
            this->transform->position.y = 0;
            break;
        case Alignment::MiddleLeft:
        case Alignment::MiddleCenter:
        case Alignment::MiddleRight:
            this->transform->position.y = this->screenSize.height * 0.5f - height * 0.5f;
            break;
        case Alignment::BottomLeft:
        case Alignment::BottomCenter:
        case Alignment::BottomRight:
            this->transform->position.y = this->screenSize.height - height;
            break;
    }
    
    this->dirtyPosition = false;
}

void MogStats::bindVertex() {
    auto indices = new short[4];
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;

    auto vertices = new float[4 * 2];
    vertices[0] = 0;    vertices[1] = 0;
    vertices[2] = 0;    vertices[3] = this->texture->height;
    vertices[4] = this->texture->width;    vertices[5] = 0;
    vertices[6] = this->texture->width;    vertices[7] = this->texture->height;

    auto vertexTexCoords = new float[4 * 2];
    vertexTexCoords[0] = 0;    vertexTexCoords[1] = 0;
    vertexTexCoords[2] = 0;    vertexTexCoords[3] = 1.0;
    vertexTexCoords[4] = 1.0f;    vertexTexCoords[5] = 0;
    vertexTexCoords[6] = 1.0f;    vertexTexCoords[7] = 1.0f;

    this->renderer->bindVertex(vertices, 4 * 2, indices, 4, true);
    this->texture->bindTexture();
    this->renderer->bindTextureVertex(this->texture->textureId, vertexTexCoords, 4 * 2, true);
}

void MogStats::init() {
    this->renderer = make_shared<Renderer>();
    this->transform = make_shared<Transform>();

    for (int i = 0; i < 10; i++) {
        auto tex = this->createLabelTexture(to_string(i));
        this->numberTexture2ds[i] = tex;
    }
    this->numberTexture2ds['.'] = this->createLabelTexture(".");

    const int padding = 25;
    const int xMargin = 15;
    const int yMargin = 4;
    const int startX = padding;
    const int startY = padding;
    int x = startX;
    int y = startY;

    auto fps = this->createLabelTexture("000.00");
    auto separator = this->createLabelTexture("/");
    auto delta = this->createLabelTexture("00.0000");
    auto drawCallLabel = this->createLabelTexture("DRAW CALL :");
    auto drawCall = this->createLabelTexture("0");
    auto instantsLabel = this->createLabelTexture("INSTANTS  :");
    auto instants = this->createLabelTexture("0");

    this->width = fps->width + separator->width + delta->width + xMargin * 2 + padding * 2;
    this->height = max(fps->height, delta->height) +
        max(drawCallLabel->height, drawCall->height) +
        max(instantsLabel->height, instants->height) + padding * 2;
    this->data = (unsigned char *)calloc(this->width * this->height * 4, sizeof(unsigned char));
    for (int i = 0; i < this->width * this->height; i++) {
        this->data[i * 4 + 3] = ALPHA;
    }
    this->texture = Texture2D::createWithRGBA(this->data, this->width, this->height);

    this->setTextToData(fps, x, y);
    this->positions[FPS] = pair<int, int>(x, y);
    x += fps->width + xMargin;
    this->setTextToData(separator, x, y);
    x += separator->width + xMargin;
    this->setTextToData(delta, x, y);
    this->positions[DELTA] = pair<int, int>(x, y);

    x = startX;
    y += fps->height + yMargin;
    this->setTextToData(drawCallLabel, x, y);
    x += drawCallLabel->width + xMargin;
    this->setTextToData(drawCall, x, y);
    this->positions[DRAW_CALL] = pair<int, int>(x, y);

    x = startX;
    y += drawCallLabel->height + yMargin;
    this->setTextToData(instantsLabel, x, y);
    x += instantsLabel->width + xMargin;
    this->setTextToData(instants, x, y);
    this->positions[INSTANTS] = pair<int, int>(x, y);

    this->bindVertex();
    this->initialized = true;
    this->setAlignment(this->alignment);
    
    this->initialized = true;
}

void MogStats::setTextToData(shared_ptr<Texture2D> text, int x, int y) {
    for (int yi = 0; yi < text->height; yi++) {
        memcpy(&this->data[((yi + y) * this->width + x) * 4],
               &text->data[(yi * text->width) * 4],
               text->width * 4 * sizeof(unsigned char));
    }
}

void MogStats::setNumberToData(float value, int intLength, int decimalLength, int x, int y) {
    intLength = max(intLength, (int)log10(value) + 1);
    int intVal = (int)(value * pow(10, decimalLength));
    int length = intLength + decimalLength;
    vector<shared_ptr<Texture2D>> vec;
    int width = 0;
    int height = 0;
    for (int i = 0; i < length; i++) {
        int v = intVal % 10;
        auto tex = this->numberTexture2ds[v];
        vec.emplace_back(tex);
        width += tex->width;
        height = max(height, tex->height);
        if (i + 1 == decimalLength) {
            tex = this->numberTexture2ds['.'];
            vec.emplace_back(tex);
            width += tex->width;
            height = max(height, tex->height);
        }
        intVal /= 10;
    }

    int _x = 0;
    for (int i = (int)vec.size() - 1; i >= 0; i--) {
        auto tex = vec[i];
        this->setTextToData(tex, x + _x, y);
        _x += tex->width;
    }
}

#ifdef MOG_QT
#include <QFontDatabase>
#endif

shared_ptr<Texture2D> MogStats::createLabelTexture(string text) {
#if defined(MOG_IOS) || defined(MOG_OSX)
    const char *fontFace = "Courier";
#elif defined(MOG_ANDROID)
    const char *fontFace = "monospace";
#elif defined(MOG_QT)
    const char *fontFace = QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont).family().toStdString().c_str();
#else
    const char *fontFace = "";
#endif
    auto tex2d = Texture2D::createWithText(text, 13.0f, fontFace);
    unsigned char *tmpData = (unsigned char *)malloc(tex2d->dataLength * sizeof(unsigned char));
    memcpy(tmpData, tex2d->data, tex2d->dataLength * sizeof(unsigned char));

    for (int yi = 0; yi < tex2d->height; yi++) {
        int yr = yi;
        if (tex2d->isFlip) {
            yr = tex2d->height - yi - 1;
        }
        for (int xi = 0; xi < tex2d->width; xi++) {
            int i = yi * tex2d->width + xi;
            int ii = yr * tex2d->width + xi;
            unsigned char alpha = tmpData[i * 4 + 3];
            unsigned char data[4] = {alpha, alpha, alpha, ALPHA};
            memcpy(&tex2d->data[ii * 4], data, 4 * sizeof(unsigned char));
        }
    }

    safe_free(tmpData);
    return tex2d;
}

void MogStats::updateValues(float delta) {
    float fps = 1.0f / delta;
    if (fps >= 1000) {
        fps = 999.99f;
    }
    if (delta >= 100) {
        delta = 99.9999f;
    }
    this->setNumberToData(fps, 3, 2, this->positions[FPS].first, this->positions[FPS].second);
    this->setNumberToData(delta, 2, 4, this->positions[DELTA].first, this->positions[DELTA].second);
    this->setNumberToData(drawCallCount, 0, 0, this->positions[DRAW_CALL].first, this->positions[DRAW_CALL].second);
    this->setNumberToData(instanceCount, 0, 0, this->positions[INSTANTS].first, this->positions[INSTANTS].second);
}
