#pragma once

#include <string>

namespace VcdParser::Utils {
    std::string formatString(const char *fmt, va_list va);
}