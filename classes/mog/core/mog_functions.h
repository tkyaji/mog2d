#ifndef mog_functions_h
#define mog_functions_h

#include "mog/core/mog_functions_native.h"
#include <stdlib.h>

void LOGD(const char *format, ...);
void LOGI(const char *format, ...);
void LOGE(const char *format, ...);
void LOGW(const char *format, ...);

bool isPowerOfTwo(int x);
int nextPowerOfTwo(int minimum);

void printMatrix(float *matrix, const char *label = nullptr);
long long getTimestamp();
float smoothstep(float edge0, float edge1, float x);
bool approximately(float a, float b);

template <typename T>
inline void safe_delete(T*& p) {
    if (p == nullptr) return;
    delete p;
    (p) = nullptr;
}

template <typename T>
inline void safe_delete_arr(T*& p) {
    if (p == nullptr) return;
    delete[] p;
    (p) = nullptr;
}

template <typename T>
inline void safe_free(T*& p) {
    if (p == nullptr) return;
    free(p);
    (p) = nullptr;
}

#endif /* mog_functions_h */
