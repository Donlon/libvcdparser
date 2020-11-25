#include "tokenizer.h"

VcdParser::Tokenizer::Tokenizer(const char *data, size_t len)
        : data(data),
          p(data),
          end(data + len) {
}

std::string VcdParser::Tokenizer::getNextToken() {
    while (p < end) { // skip spaces
        switch (*p) {
            case '\n':
                p++;
                line += 1;
                column = 0;
                continue;
            case '\r':
            case '\0':
            case ' ':
            case '\t':
                p++;
                column += 1;
                continue;
            default:
                break;
        }
        break;
    }
    const char *tokenStart = p;
    while (p < end) {
        switch (*p) {
            case '\n':
                line += 1;
                column = 0;
            case '\r':
            case ' ':
            case '\t':
            case '\0': {
                const char *tokenEnd = p;
                if (*p != '\n') {
                    column++;
                }
                p++;
                return std::string(tokenStart, tokenEnd);
            }
            default:
                p++;
                column += 1;
                continue;
        }
    }
    return "";
}
