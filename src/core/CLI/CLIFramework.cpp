// NLM CLI Framework Implementation
// Provides comprehensive argument parsing, help system, and command management

#include "core/CLI/CLIFramework.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace nlm {

bool CLIParser::setArgument(const std::string& name, const std::string& value) {
    for (auto& arg : arguments) {
        if (arg.name == name) {
            parsedArgs[name] = value;
            return true;
        }
    }
    return false;
}

std::string CLIParser::getArgument(const std::string& name, const std::string& defaultValue) {
    auto it = parsedArgs.find(name);
    return it != parsedArgs.end() ? it->second : defaultValue;
}

bool CLIParser::hasArgument(const std::string& name) {
    return parsedArgs.find(name) != parsedArgs.end();
}

void CLIParser::showHelp() {
    std::cout << helpText << std::endl;
    
    std::cout << "\nArguments:" << std::endl;
    for (const auto& arg : arguments) {
        if (arg.name == "help" || arg.name == "version" || arg.name == "verbose" || arg.name == "quiet") {
            continue; // Skip already displayed options
        }
        std::cout << "  -" << arg.shortOpt << ", --" << arg.longOpt;
        if (!arg.defaultValue.empty()) {
            std::cout << "=" << arg.defaultValue;
        }
        std::cout << std::endl;
        std::cout << "    " << arg.description << std::endl;
    }
    
    std::cout << std::endl;
}

void CLIParser::showVersion() {
    std::cout << "NLM (熙然) Neural Learning Machine v0.1.0" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
}

void CLIParser::log(const std::string& message) {
    if (!quiet) {
        if (verbose) {
            std::cout << "[VERBOSE] " << message << std::endl;
        } else {
            std::cout << message << std::endl;
        }
    }
}

} // namespace nlm
