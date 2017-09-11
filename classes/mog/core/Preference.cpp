#include "mog/core/Preference.h"
#include "mog/core/PreferenceNative.h"

using namespace mog;

void Preference::setInt(const char *key, int intVal) {
    PreferenceNative::setInt(key, intVal);
}

int Preference::getInt(const char *key, int defaultVal) {
    return PreferenceNative::getInt(key, defaultVal);
}

void Preference::setDouble(const char *key, double doubleVal) {
    PreferenceNative::setDouble(key, doubleVal);
}

double Preference::getDouble(const char *key, double defaultVal) {
    return PreferenceNative::getDouble(key, defaultVal);
}

void Preference::setString(const char *key, string strVal) {
    PreferenceNative::setString(key, strVal);
}

string Preference::getString(const char *key, string defaultVal) {
    return PreferenceNative::getString(key, defaultVal);
}

void Preference::setBool(const char *key, bool boolVal) {
    PreferenceNative::setBool(key, boolVal);
}

bool Preference::getBool(const char *key, bool defaultVal) {
    return PreferenceNative::getBool(key, defaultVal);
}
