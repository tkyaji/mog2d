#include "mog/core/TextureLoader.h"
#include "mog/Constants.h"

#define STBI_MALLOC(sz)           mogmalloc(sz)
#define STBI_REALLOC(p,newsz)     mogrealloc(p,newsz)
#define STBI_FREE(p)              mogfree(p)

#define STB_IMAGE_IMPLEMENTATION
#include "mog/libs/stb_image.h"

using namespace mog;

bool TextureLoader::load(unsigned char *buffer, int len, unsigned char **imageData, int *imageWidth, int *imageHeight, int *imageBitsPerPixel) {
    int x = 0;
    int y = 0;
    int _n = 0;
    int n = 4;  // fixed RGBA
    unsigned char *data = stbi_load_from_memory(buffer, len, &x, &y, &_n, n);
    if (data == nullptr) {
        return false;
    };
    
    *imageData = data;
    *imageWidth = x;
    *imageHeight = y;
    *imageBitsPerPixel = n;
    
    return true;
}
