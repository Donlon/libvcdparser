// SPDX-License-Identifier: MIT

#include "utils.h"

#include <cstdarg>
#include <stdexcept>

std::string VcdParser::Utils::formatString(const char *fmt, va_list va) {
    va_list va2;
    va_copy(va2, va);
    size_t size = vsnprintf(nullptr, 0, fmt, va) + 1;
    if (size <= 0) {
        throw std::runtime_error("Error during formatting.");
    }
    std::string str;
    str.resize(size);
    vsnprintf(const_cast<char *>(str.data()), size, fmt, va2);
    return str;
}
