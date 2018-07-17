#import "PreferenceNative.h"
#import <Foundation/Foundation.h>

using namespace mog;

void PreferenceNative::setInt(const char *key, int intVal) {
    [[NSUserDefaults standardUserDefaults] setInteger:(NSInteger)intVal
                                               forKey:[NSString stringWithUTF8String:key]];
}

int PreferenceNative::getInt(const char *key, int defaultVal) {
    NSString *keyStr = [NSString stringWithUTF8String:key];
    NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
    if ([ud objectForKey:keyStr]) {
        return (int)[ud integerForKey:keyStr];
    } else {
        return defaultVal;
    }
}

void PreferenceNative::setDouble(const char *key, double doubleVal) {
    [[NSUserDefaults standardUserDefaults] setDouble:doubleVal
                                              forKey:[NSString stringWithUTF8String:key]];
}

double PreferenceNative::getDouble(const char *key, double defaultVal) {
    NSString *keyStr = [NSString stringWithUTF8String:key];
    NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
    if ([ud objectForKey:keyStr]) {
        return (double)[ud doubleForKey:keyStr];
    } else {
        return defaultVal;
    }
}

void PreferenceNative::setString(const char *key, string strVal) {
    [[NSUserDefaults standardUserDefaults] setValue:[NSString stringWithUTF8String:strVal.c_str()]
                                             forKey:[NSString stringWithUTF8String:key]];
}

string PreferenceNative::getString(const char *key, string defaultVal) {
    NSString *keyStr = [NSString stringWithUTF8String:key];
    NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
    if ([ud objectForKey:keyStr]) {
        return [[ud stringForKey:keyStr] UTF8String];
    } else {
        return defaultVal;
    }
}

void PreferenceNative::setBool(const char *key, bool boolVal) {
    [[NSUserDefaults standardUserDefaults] setBool:(BOOL)boolVal
                                            forKey:[NSString stringWithUTF8String:key]];
}

bool PreferenceNative::getBool(const char *key, bool defaultVal) {
    NSString *keyStr = [NSString stringWithUTF8String:key];
    NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
    if ([ud objectForKey:keyStr]) {
        return (bool)[ud boolForKey:keyStr];
    } else {
        return defaultVal;
    }
}
