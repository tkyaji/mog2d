//
//  PreferenceNative.h
//  Flick10
//
//  Created by tkyaji on 2017/03/23.
//  Copyright © 2017年 MagicAnt. All rights reserved.
//

#ifndef PreferenceNative_h
#define PreferenceNative_h

#include <string>

using namespace std;

namespace mog {
    class PreferenceNative {
    public:
        static void setInt(const char *key, int intVal);
        static int getInt(const char *key, int defaultVal = 0);

        static void setDouble(const char *key, double doubleVal);
        static double getDouble(const char *key, double defaultVal = 0);

        static void setString(const char *key, string strVal);
        static string getString(const char *key, string defaultVal = nullptr);
        
        static void setBool(const char *key, bool boolVal);
        static bool getBool(const char *key, bool defaultVal = false);
    };
}

#endif /* PreferenceNative_h */
