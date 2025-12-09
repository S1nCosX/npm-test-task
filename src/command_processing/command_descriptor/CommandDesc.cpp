#include "CommandDesc.h"
#include "command_processing/enums/ECommandType.h"
#include <iostream>
#include <regex>

ECommandType CommandDesc::desc(std::string inp) {
    std::regex time(R"(\s*/time\s*)");
    std::regex stats(R"(\s*/stats\s*)");
    std::regex shutdown(R"(\s*/shutdown\s*)");
    std::smatch m;
    std::cout << inp;
    if (std::regex_match(inp, m, time))
        return ECommandType::TIME;
    if (regex_match(inp, m, stats))
        return ECommandType::STATS;
    if (regex_match(inp, m, shutdown))
        return ECommandType::SHUTDOWN;
    return ECommandType::NONE;
}
