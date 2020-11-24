#include "libvcdparser.h"

#include <iostream>
#include <map>

namespace VcdParser {
    enum ParserStates {
        InDefinitionCmds,
        InSimulationCmds,

        InComment,
        InDate,
        InEndDefinitions,
        InScope,
        InTimescale,
        InUpscope,
        InVar,
        InVersion,

        InVectorValueChange,
    };

    enum class VarParseState {
        WaitVarType,
        WaitSize,
        WaitIdentifierCode,
        WaitName
    };

    struct Var {
        VarType type = Unknown; // var_type
        int size = 0;
        std::string identifier; // identifier_code
        std::string name; // reference
        VarParseState state = VarParseState::WaitVarType;

        bool setVarType(const std::string &typeName) {
            if (typeName == "reg") {
                type = Reg;
            } else if (typeName == "wire") {
                type = Wire;
            } else if (typeName == "event") {
                type = Event;
            } else if (typeName == "integer") {
                type = Integer;
            } else if (typeName == "parameter") {
                type = Parameter;
            } else if (typeName == "real") {
                type = Real;
            } else if (typeName == "realtime") {
                type = Realtime;
            } else if (typeName == "supply0") {
                type = Supply0;
            } else if (typeName == "supply1") {
                type = Supply1;
            } else if (typeName == "time") {
                type = Time;
            } else if (typeName == "tri") {
                type = Tri;
            } else if (typeName == "triand") {
                type = Triand;
            } else if (typeName == "trior") {
                type = Trior;
            } else if (typeName == "trireg") {
                type = Trireg;
            } else if (typeName == "tri0") {
                type = Tri0;
            } else if (typeName == "tri1") {
                type = Tri1;
            } else if (typeName == "wand") {
                type = Wand;
            } else if (typeName == "wor") {
                type = Wor;
            } else {
                return false;
            }
            return true;
        }

        bool setSize(const std::string &sizeName) {
            int s = std::stoi(sizeName);
            if (s <= 0) {
                return false;
            } else {
                size = s;
                return true;
            }
        }

        bool setIdentifier(const std::string &str) {
            identifier = str;
            return true;
        }

        bool setName(const std::string &str) {
            name = str;
            return true;
        }
    };

    enum class TimescaleParseState {
        WaitTimeNumber,
        WaitTimeUnit,
    };

    struct Timescale {
        int timeNumber = 0;
        VcdFormat::TimeUnit timeUnit = VcdFormat::TimeUnit::Unknown;
        TimescaleParseState state = TimescaleParseState::WaitTimeNumber;

        bool setTimeNumber(const std::string &str) {
            int s = std::stoi(str);
            if (s <= 0) {
                return false;
            } else {
                timeNumber = s;
                return true;
            }
        }

        bool setTimeUnit(const std::string &str) {
            if (str == "s") {
                timeUnit = VcdFormat::TimeUnit::s;
            } else if (str == "ms") {
                timeUnit = VcdFormat::TimeUnit::ms;
            } else if (str == "us") {
                timeUnit = VcdFormat::TimeUnit::us;
            } else if (str == "ns") {
                timeUnit = VcdFormat::TimeUnit::ns;
            } else if (str == "ps") {
                timeUnit = VcdFormat::TimeUnit::ps;
            } else if (str == "fs") {
                timeUnit = VcdFormat::TimeUnit::fs;
            } else {
                return false;
            }
            return true;
        }
    };

    static inline bool checkVariableValue(char ch) {
        return ch == '0' || ch == '1'
               || ch == 'x' || ch == 'X'
               || ch == 'z' || ch == 'Z';
    }
}

VcdParser::VcdParse::VcdParse(const char *data, size_t len)
        : tokenizer(data, len) {
}

void VcdParser::VcdParse::parse() {
    std::string token;
    ParserStates state = InDefinitionCmds;

    Var var;
    Timescale timescale;

    // vectorValueChangeType --binary/real
    std::string vectorValueChangeValue;

    while (!token.empty()) {
        token = tokenizer.getNextToken();
        switch (state) {
            case InDefinitionCmds:
                if (token == "$comment") {
                    state = InComment;
                } else if (token == "$date") {
                    state = InDate;
                } else if (token == "$scope") {
                    state = InScope;
                } else if (token == "$timescale") {
                    state = InTimescale;
                    timescale.state = TimescaleParseState::WaitTimeNumber;
                } else if (token == "$var") {
                    state = InVar;
                    var.state = VarParseState::WaitVarType;
                } else if (token == "$version") {
                    state = InVersion;
                } else {
                    throwException(std::string("Unknown token ") + token);
                }
                break;

            case InComment:// Not implemented
            case InScope:
            case InUpscope:
                if (token == "$end") {
                    state = InDefinitionCmds;
                } else {
                    // pass
                }
                break;

            case InDate:
                if (token == "$end") {
                    state = InDefinitionCmds;
                } else {
                    std::string &v = vcdFile.date;
                    if (!v.empty()) {
                        v += " ";
                    }
                    v += token;
                }
                break;

            case InTimescale:
                if (token == "$end") {
                    vcdFile.timescale.timeNumber = timescale.timeNumber;
                    vcdFile.timescale.timeUnit = timescale.timeUnit;
                    state = InDefinitionCmds;
                } else {
                    switch (timescale.state) {
                        case TimescaleParseState::WaitTimeNumber:
                            if (!timescale.setTimeNumber(token)) {
                                throwException("time_number of variable is invalid");
                            }
                            break;
                        case TimescaleParseState::WaitTimeUnit:
                            if (!timescale.setTimeUnit(token)) {
                                throwException("time_unit of variable is invalid");
                            }
                            break;
                    }
                }
                break;

            case InVar:
                if (token == "$end") {
                    state = InDefinitionCmds;
                    vcdFile.fileSignals.push_back({
                                                          var.name,  // std::string signalName;
                                                          var.identifier,  // std::string signalSymbol;
                                                          {} // std::vector<SubVcdSignal> signals;
                                                  });
                    std::vector<SubVcdSignal> &signals = vcdFile.fileSignals.end()->signals;
                    signals.reserve(var.size);
                    int i = 0;
                    for (auto it : signals) {
                        it.index = i;
                        i++;
                    }
                    varIdentifierMap[var.identifier] = &*vcdFile.fileSignals.end();
                } else {
                    switch (var.state) {
                        case VarParseState::WaitVarType:
                            if (!var.setVarType(token)) {
                                throwException("type of variable is invalid");
                            }
                            break;
                        case VarParseState::WaitSize:
                            if (!var.setSize(token)) {
                                throwException("size of variable is invalid");
                            }
                            break;
                        case VarParseState::WaitIdentifierCode:
                            if (!var.setIdentifier(token)) {
                                throwException("identifier of variable is invalid");
                            }
                            break;
                        case VarParseState::WaitName:
                            if (!var.setName(token)) {
                                throwException("name of variable is invalid");
                            }
                            break;
                    }
                }
                break;

            case InVersion:
                if (token == "$end") {
                    state = InDefinitionCmds;
                } else {
                    std::string &v = vcdFile.version;
                    if (!v.empty()) {
                        v += " ";
                    }
                    v += token;
                }
                break;

            case InEndDefinitions:
                if (token == "$end") {
                    // eNd dEfInItIoNs
                    state = InSimulationCmds;
                }
                break;

            case InSimulationCmds: {
                char c = token[0];
                if (c == 'b' || c == 'B') {
                    // vector_value_change
                    vectorValueChangeValue = token.substr(1);
                    state = InVectorValueChange;
                } else if (c == '#') {
                    int s = std::stoi(token.substr(1));
                    if (s <= 0) {
                        throwException("invalid simulation time");
                    } else {
                        currentTime = s;
                    }

                } else if (c == 'r' || c == 'R') {
                    throwException("real_number is not supported in vector_value_change");
                } else {
                    // scalar_value_change
                    bool i = parseScalarValueChange(token);
                    if (!i) {
                        throwException("invalid scalar value change definition");
                    }
                }
                break;
            }

            case InVectorValueChange: {
                bool i = parseVectorValueChange(token, vectorValueChangeValue);
                if (!i) {
                    throwException("invalid vector value change definition");
                }
                state = InSimulationCmds;
                break;
            }
            default:
                return;
        }
    }
}

bool VcdParser::VcdParse::parseScalarValueChange(const std::string &definition) {
    if (definition.length() <= 1) {
        return false;
    }
    VcdSignal *var = varIdentifierMap[definition.substr(1)];
    if (!var) {
        return false;
    }
    char value = definition[0];
    if (!checkVariableValue(value)) {
        return false;
    }
    var->signals[0].data.push_back({currentTime, value});

    return false;
}

bool VcdParser::VcdParse::parseVectorValueChange(const std::string &identifier, const std::string &value) {
    VcdSignal *var = varIdentifierMap[identifier];
    if (!var) {
        return false;
    }
    uint64_t varSize = var->signals.size();
    if (value.length() != varSize) {
        return false;
    }
    auto valueIt = value.begin();
    for (auto it : var->signals) {
        char v = *valueIt;
        if (!checkVariableValue(v)) {
            return false;
        }
        it.data.push_back({currentTime, v});
    }
    return false;
}

void VcdParser::VcdParse::throwException(const std::string &msg) {
    throw VcdException(msg, tokenizer.getLine(), tokenizer.getColumn());
}
