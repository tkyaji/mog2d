#include "mog/core/PreferenceNative.h"

using namespace mog;

void PreferenceNative::setInt(const char *key, int intVal) {
}

int PreferenceNative::getInt(const char *key, int defaultVal) {
    return defaultVal;
}

void PreferenceNative::setDouble(const char *key, double doubleVal) {
}

double PreferenceNative::getDouble(const char *key, double defaultVal) {
    return defaultVal;
}

void PreferenceNative::setString(const char *key, std::string strVal) {
}

std::string PreferenceNative::getString(const char *key, std::string defaultVal) {
    return defaultVal;
}

void PreferenceNative::setBool(const char *key, bool boolVal) {
}

bool PreferenceNative::getBool(const char *key, bool defaultVal) {
    return defaultVal;
}
