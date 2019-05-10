#import "mog/core/mog_functions_native.h"
#include <stdio.h>

void _log(const char* format, va_list args) {
    vprintf(format, args);
}
