#include <cstddef>
#include <string>
#include <qlogging.h>
#include "mog/core/mog_functions_native.h"

void _log(const char* format, va_list args) {
    size_t slen = strlen(format);
    char *logStr = new char[slen + 4096];
    vsprintf(logStr, format, args);
    qDebug("%s", logStr);
    delete[] logStr;
}
