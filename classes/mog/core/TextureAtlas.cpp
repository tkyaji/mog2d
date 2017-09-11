#include "mog/Constants.h"
#include "mog/core/TextureAtlas.h"
#include <stdlib.h>
#include <algorithm>

using namespace mog;

#pragma - TextureAtlasCell

TextureAtlasCell::TextureAtlasCell(const shared_ptr<Texture2D> &texture) {
    this->texture = texture;
    this->width = texture->width;
    this->height = texture->height;
}


#pragma - TextureAtlas

void TextureAtlas::addTexture(const shared_ptr<Texture2D> &tex2d) {
    if (this->cellMap.count(tex2d) > 0) return;
    auto cell = make_shared<TextureAtlasCell>(tex2d);
    this->cells.emplace_back(cell);
    this->cellMap[tex2d] = cell;
}

void TextureAtlas::mapTextureCells() {
    vector<shared_ptr<TextureAtlasCell>> tmpCells = this->cells;
    sort(tmpCells.begin(), tmpCells.end(), [](shared_ptr<TextureAtlasCell> cell1, shared_ptr<TextureAtlasCell> cell2) {
        return cell1->texture->height > cell2->texture->height;
    });
    
    int x = 0;
    int y = 0;
    int mh = 0;
    int mw = 0;

    for (const auto &cell : tmpCells) {
        auto tex2d = cell->texture;
        if (x > 0) x += TEXTURE_MARGIN * 2;
        if (x + tex2d->width > MAX_TEXTURE_SIZE) {
            y += mh + TEXTURE_MARGIN * 2;
            if (y > MAX_TEXTURE_SIZE) break;
            x = 0;
            mh = 0;
        }
        
        cell->x = x;
        cell->y = y;
        
        x += tex2d->width;
        mh = max(mh, tex2d->height);
        mw = max(mw, x);
    }
    
    this->width = mw;
    this->height = y + mh;
}

shared_ptr<Texture2D> TextureAtlas::createTexture() {
    this->mapTextureCells();
    
    int bitsPerPixel = 4;
    auto textureType = TextureType::RGBA;
    
    this->texture = make_shared<Texture2D>();
    this->texture->textureType = textureType;
    this->texture->width = this->width;
    this->texture->height = this->height;
    this->texture->bitsPerPixel = bitsPerPixel;
    this->texture->dataLength = this->texture->width * this->texture->height * bitsPerPixel;
    
    return this->texture;
}

void TextureAtlas::bindTexture() {
    this->texture->bindTexture();
    
    for (const auto &cell : this->cells) {
        this->bindTextureSub(cell);
    }
}

void TextureAtlas::bindTextureSub(const shared_ptr<TextureAtlasCell> &cell) {
    this->texture->bindTextureSub(cell->texture->data, cell->x, cell->y, cell->width, cell->height);
    
    if (cell->x > 0) {
        int marginL = TEXTURE_MARGIN;
        unsigned char *dt = new unsigned char[cell->height * 4];
        this->readTexturePixels(dt, cell->texture, 0, 0, 1, cell->height);
        for (int i = 1; i <= marginL; i++) {
            this->texture->bindTextureSub(dt, cell->x - i, cell->y, 1, cell->height);
        }
        safe_delete_arr(dt);
    }
    
    int marginR = min(this->width - (cell->x + cell->width), TEXTURE_MARGIN);
    if (marginR > 0) {
        unsigned char *dt = new unsigned char[cell->height * 4];
        this->readTexturePixels(dt, cell->texture, cell->width - 1, 0, 1, cell->height);
        for (int i = 0; i < marginR; i++) {
            this->texture->bindTextureSub(dt, cell->x + cell->width + i, cell->y, 1, cell->height);
        }
        safe_delete_arr(dt);
    }
    
    if (cell->y > 0) {
        int marginT = TEXTURE_MARGIN;
        unsigned char *dt = new unsigned char[cell->width * 4];
        this->readTexturePixels(dt, cell->texture, 0, 0, cell->width, 1);
        for (int i = 1; i <= marginT; i++) {
            this->texture->bindTextureSub(dt, cell->x, cell->y - i, cell->width, 1);
        }
        safe_delete_arr(dt);
    }
    
    int marginB = min(this->height - (cell->y + cell->height), TEXTURE_MARGIN);
    if (marginB > 0) {
        unsigned char *dt = new unsigned char[cell->width * 4];
        this->readTexturePixels(dt, cell->texture, 0, cell->height - 1, cell->width, 1);
        for (int i = 0; i < marginB; i++) {
            this->texture->bindTextureSub(dt, cell->x, cell->y + cell->height + i, cell->width, 1);
        }
        safe_delete_arr(dt);
    }
}

void TextureAtlas::readTexturePixels(unsigned char *dst, const shared_ptr<Texture2D> &tex2d, int x, int y, int width, int height) {
    for (int _y = 0; _y < height; _y++) {
        int di = _y * width;
        int si = (y + _y) * tex2d->width + x;
        memcpy(&dst[di * 4], &tex2d->data[si * 4], sizeof(unsigned char) * width * 4);
    }
}

shared_ptr<TextureAtlasCell> TextureAtlas::getCell(const shared_ptr<Texture2D> &tex2d) {
    return this->cellMap[tex2d];
}

