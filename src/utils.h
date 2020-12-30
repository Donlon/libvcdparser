// SPDX-License-Identifier: MIT

#pragma once

#include <string>

namespace VcdParser {
    namespace Utils {
        std::string formatString(const char *fmt, va_list va);
    }
}
