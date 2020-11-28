#pragma once

#include <string>

namespace VcdParser {
    class Tokenizer {
        // std::string::iterator it;
        // std::string::iterator end;

        const char *data;
        const char *end;
        const char *p;

        size_t line = 1;
        size_t column = 0;
        size_t lastLine = 1;
        size_t lastColumn = 0;
    public:
        explicit Tokenizer(const char *data, size_t len);

        std::string getNextToken();

        inline size_t getLine() const {
            return line;
        }

        inline size_t getColumn() const {
            return column;
        }

        inline size_t getLastLine() const {
            return lastLine;
        }

        inline size_t getLastColumn() const {
            return lastColumn;
        }
    };
}
