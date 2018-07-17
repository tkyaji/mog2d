#import "mog/core/mog_functions_native.h"
#import <Foundation/Foundation.h>

void _log(const char* format, va_list args) {
    size_t slen = strlen(format);
    char *logStr = new char[slen + 4096];
    vsprintf(logStr, format, args);
    NSLog(@"%s", logStr);
    delete[] logStr;
}
