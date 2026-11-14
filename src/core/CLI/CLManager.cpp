// Implementation of CLI utilities for NLM

#include "CLI/CLManager.hpp"
#include <algorithm>
#include <iomanip>

namespace nlm {

void CLIParser::showHelp(const std::string& programName) {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "===============================" << std::endl << std::endl;
    
    std::cout << "USAGE:" << std::endl;
    std::cout << "  " << programName << " [OPTIONS] [ARGUMENTS]" << std::endl << std::endl;
    
    std::cout << "OPTIONS:" << std::endl;
    for (const auto& option : options) {
        std::string optName = "--" + option.name;
        if (!option.shortName.empty()) {
            optName += ", -" + option.shortName;
        }
        
        std::cout << "  " << std::left << std::setw(20) << optName;
        
        switch (option.type) {
            case ArgumentType::Flag:
                std::cout << "[flag]";
                break;
            case ArgumentType::Value:
                std::cout << "<value>";
                break;
            case ArgumentType::String:
                std::cout << "<string>";
                break;
            case ArgumentType::Integer:
                std::cout << "<int>";
                break;
            case ArgumentType::Float:
                std::cout << "<float>";
                break;
            case ArgumentType::Choice:
                std::cout << "<choice>";
                break;
        }
        
        std::cout << " " << option.description;
        
        if (!option.defaultValue.empty()) {
            std::cout << " (default: " << option.defaultValue << ")";
        }
        
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "GLOBAL OPTIONS:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -v, --version           Show version information" << std::endl;
    std::cout << "  -V, --verbose           Enable verbose output" << std::endl;
    std::cout << "  -d, --debug             Enable debug mode" << std::endl;
    std::cout << "  -p, --phase <phase>     Select phase (1, 2, 3, 4, 5, 6)" << std::endl;
    std::cout << "  -c, --config <file>     Load configuration from file" << std::endl;
    std::cout << "  -l, --log-level <level> Set log level (debug, info, warn, error)" << std::endl;
    
    std::cout << std::endl;
    std::cout << "ARGUMENTS:" << std::endl;
    std::cout << "  <episodes>             Number of episodes (for Phase 3)" << std::endl;
    std::cout << "  <steps>                Number of steps per episode (for Phase 3)" << std::endl;
    std::cout << "  <trials>                Number of trials (for Phase 4)" << std::endl;
}

} // namespace nlm
