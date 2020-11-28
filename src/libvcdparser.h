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


    struct ValueChange {
        uint64_t time;
        char data; // 'U', 'Z', '0', '1';
    };

    struct SignalRecord {
        unsigned int index = 0;//represent the order in the bus signal
        std::vector<ValueChange> values;
    };

    struct Variable {
        std::string name;
        std::string identifier;
        std::vector<SignalRecord> signals;
    };

    struct VcdFile {
        std::string date;
        std::string version;
        VcdFormat::Timescale timescale;
        uint64_t lastVariableChangeTime = 0;
        //can add more filed if necessary

        std::vector<Variable *> variableList;
        // std::vector<SignalRecord *> signalList;

        ~VcdFile();

        Variable *createVariable(std::string name, std::string identifier);

        // SignalRecord *createSignal();
    };
}

namespace VcdParser {
    struct VcdException : public std::exception {
        std::string msg;
        size_t line = 0;
        size_t column = 0;

        VcdException(std::string msg, size_t line, size_t column)
                : msg(std::move(msg)), line(line), column(column) {
        };
    };

    class VcdParser {
        Tokenizer tokenizer;
        VcdFormat::VcdFile vcdFile;

        uint64_t currentTime = 0;
        std::map<std::string, VcdFormat::Variable *> varIdentifierMap;
    public:
        VcdParser(const char *data, size_t len);

        explicit VcdParser(const std::string &buffer)
                : VcdParser(buffer.data(), buffer.size()) {
        }

        void parse();

        VcdFormat::VcdFile &getResult() {
            return vcdFile;
        };

    private:
        void parseScalarValueChange(const std::string &definition);

        void parseVectorValueChange(const std::string &identifier,
                                    const std::string &value);

        void throwException(const char *fmt, ...);

        void throwException(const std::string &msg);
    };
}
