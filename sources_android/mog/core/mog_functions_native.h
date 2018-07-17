//
//  UtilsNative.h
//  Emo-x
//
//  Created by tkyaji on 2017/02/11.
//  Copyright © 2017年 tkyaji. All rights reserved.
//

#include <stdarg.h>

void _log(const char* format, va_list args);

#include <sstream>
namespace std {
    template <typename T>
    string to_string(T value) {
        std::ostringstream os;
        os << value ;
        return os.str() ;
    }
}
