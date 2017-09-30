#include "mog/core/mog_functions.h"
#include <math.h>
#include <stdio.h>
#include <time.h>

void LOGD(const char *format, ...) {
#if LOG_LEVEL <= LOG_DEBUG
    va_list args;
    va_start(args, format);
    _log(format, args);
    va_end(args);
#endif
}

void LOGI(const char *format, ...) {
#if LOG_LEVEL <= LOG_INFO
    va_list args;
    va_start(args, format);
    _log(format, args);
    va_end(args);
#endif
}

void LOGW(const char *format, ...) {
#if LOG_LEVEL <= LOG_WARN
    va_list args;
    va_start(args, format);
    _log(format, args);
    va_end(args);
#endif
}

void LOGE(const char *format, ...) {
#if LOG_LEVEL <= LOG_ERROR
    va_list args;
    va_start(args, format);
    _log(format, args);
    va_end(args);
#endif
}

bool isPowerOfTwo(int x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}

int nextPowerOfTwo(int minimum) {
    if (isPowerOfTwo(minimum)) {
        return minimum;
    }
    int v = 1;
    while ((v *= 2) < minimum) {}
    return v;
}

void printMatrix(float *matrix, const char *label) {
    if (label) {
        printf("-- matrix [%s] --\n", label);
    } else {
        printf("-- matrix --\n");
    }
    printf("[ 0] %f,  [ 4] %f,  [ 8] %f,  [12] %f\n", matrix[0], matrix[4], matrix[8], matrix[12]);
    printf("[ 1] %f,  [ 5] %f,  [ 9] %f,  [13] %f\n", matrix[1], matrix[5], matrix[9], matrix[13]);
    printf("[ 2] %f,  [ 6] %f,  [10] %f,  [14] %f\n", matrix[2], matrix[6], matrix[10], matrix[14]);
    printf("[ 3] %f,  [ 7] %f,  [11] %f,  [15] %f\n", matrix[3], matrix[7], matrix[11], matrix[15]);
}

long long getTimestamp() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    // micro sec
    return (long)ts.tv_sec * 1000000 + (long)ts.tv_nsec * 0.001;
}

float smoothstep(float edge0, float edge1, float x) {
    float t = fmax(fmin((x - edge0) / (edge1 - edge0), 1.0f), 0);
    return t * t * (3 - 2 * t );
}

bool approximately(float a, float b) {
    if (a > b) {
        return a - b < 0.00000000000001f;
    } else if (a < b) {
        return b - a < 0.00000000000001f;
    } else {
        return true;
    }
}
