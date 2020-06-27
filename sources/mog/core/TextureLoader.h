#pragma once

namespace mog {
    class TextureLoader {
    public:
        static bool load(unsigned char *buffer, int len, unsigned char **imageData, int *imageWidth, int *imageHeight, int *imageBitsPerPixel);
    };
}
