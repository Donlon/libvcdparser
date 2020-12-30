// SPDX-License-Identifier: MIT

#include <iostream>
#include <fstream>

#include <libvcdparser.h>

const char *getTimeUnitString(VcdFormat::TimeUnit unit) {
    switch (unit) {
        case VcdFormat::TimeUnit::unit_s:
            return "s";
        case VcdFormat::TimeUnit::unit_ms:
            return "ms";
        case VcdFormat::TimeUnit::unit_us:
            return "us";
        case VcdFormat::TimeUnit::unit_ns:
            return "ns";
        case VcdFormat::TimeUnit::unit_ps:
            return "ps";
        case VcdFormat::TimeUnit::unit_fs:
            return "fs";
        default:
            return "(Unknown unit)";
    }
}

std::string readFile(const char *path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cout << "Can't read " << path << std::endl;
        exit(1);
    }
    std::string buffer;
    f.seekg(0, std::ios::end);
    size_t fileSize = f.tellg();
    buffer.resize(fileSize);
    f.seekg(0, std::ios::beg);
    f.read(&buffer[0], fileSize);
    return buffer;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Usage: " << argv[0] << " vcd_file_path" << std::endl;
        return 1;
    }

    std::string buffer = readFile(argv[1]);

    VcdParser::VcdParser parser(buffer);
    try {
        parser.parse();
    } catch (const VcdParser::VcdException &exception) {
        std::cout << "Parser error: (" << exception.line << ":" << exception.column << "): "
                  << exception.msg << std::endl;
        return 1;
    }

    std::cout << argv[1] << " has been parsed." << std::endl;

    VcdFormat::VcdFile &result = parser.getResult();

    std::cout << "Date: " << result.date << std::endl;
    std::cout << "Version: " << result.version << std::endl;
    std::cout << "Timescale: " << result.timescale.timeNumber
              << getTimeUnitString(result.timescale.timeUnit) << std::endl;
    std::cout << "Last variable change time: " << result.lastVariableChangeTime << std::endl;

    for (auto it : result.variableList) {
        std::cout << "Variable:" << std::endl;
        std::cout << "  name: " << it->name << std::endl;
        std::cout << "  identifier: " << it->identifier << std::endl;
        std::cout << "  bus width: " << it->signalLists.size() << std::endl;
        for (const auto &it2 : it->signalLists) {
            std::cout << "    signal[" << it2.index << "]: data size=" << it2.values.size() << std::endl;
        }
    }
}
