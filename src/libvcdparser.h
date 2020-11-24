#pragma once

#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include <map>

#include "tokenizer.h"

namespace VcdFormat {
    enum class TimeUnit {
        Unknown,
        s,
        ms,
        us,
        ns,
        ps,
        fs
    };

    struct Timescale {
        int timeNumber;
        TimeUnit timeUnit;
    };

}

namespace VcdParser {
    // enum Value {
    //     High, Low,
    //     U, Z
    // };

    enum VarType {
        Unknown,

        Event,
        Integer,
        Parameter,
        Real,
        Realtime,
        Reg,
        Supply0,
        Supply1,
        Time,
        Tri,
        Triand,
        Trior,
        Trireg,
        Tri0,
        Tri1,
        Wand,
        Wire,
        Wor,
    };


    struct dataUnit {
        uint64_t timestamp;
        char data; // 'U', 'Z', '0', '1';
    };

    struct SubVcdSignal {
        unsigned int index = 0;//represent the order in the bus signal
        std::vector<dataUnit> data;
    };

    struct VcdSignal {
        std::string signalName;
        std::string signalSymbol;
        std::vector<SubVcdSignal> signals;
    };

    struct VcdFile {
        std::string comment;
        std::string date;
        std::string version;
        VcdFormat::Timescale timescale;
        //can add more filed if necessary
        // unsigned int signalNums;
        std::vector<VcdSignal> fileSignals;
    };

    struct VcdException : public std::exception {
        std::string msg;
        size_t line = 0;
        size_t column = 0;

        VcdException(std::string msg, int line, int column)
                : msg(std::move(msg)), line(line), column(column) {
        };
    };

    class VcdParse {
        VcdParser::Tokenizer tokenizer;
        VcdFile vcdFile;

        uint64_t currentTime = 0;
        std::map<std::string, VcdSignal *> varIdentifierMap;
    public:
        VcdParse(const char *data, size_t len);

        void parse();

    private:
        bool parseScalarValueChange(const std::string &definition);

        bool parseVectorValueChange(const std::string &identifier,
                                    const std::string &value);

        void throwException(const std::string &msg);
    };
}
