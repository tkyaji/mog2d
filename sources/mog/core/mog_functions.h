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
double getTimestampSec();
float smoothstep(float edge0, float edge1, float x);
bool approximately(float a, float b);

#endif /* mog_functions_h */
