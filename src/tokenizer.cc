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
                line += 1;
                column = 0;
            case '\r':
            case '\0':
            case ' ':
            case '\t':
                continue;
            default:
                column += 1;
                p++;
                continue;
        }
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
            case '\0':
                return std::string(tokenStart, p);
            default:
                column += 1;
                p++;
                continue;
        }
    }
    return "";
}
