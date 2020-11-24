#pragma once

#include <string>

namespace VcdParser {
    class Tokenizer {
        // std::string::iterator it;
        // std::string::iterator end;

        const char *data;
        const char *end;
        const char *p;

        size_t line = 0;
        size_t column = 0;
    public:
        explicit Tokenizer(const char *data, size_t len);

        std::string getNextToken();

        [[nodiscard]]
        inline size_t getLine() const {
            return line;
        }

        [[nodiscard]]
        inline size_t getColumn() const {
            return column;
        }
    };
}
