// Common CLI help system for NLM demo executables
#include "Help.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

namespace nlm {
namespace demo {

void printUsage(const std::string& programName, const std::string& description,
                const std::vector<std::string>& arguments) {
    std::cout << "Usage: " << programName;
    if (!arguments.empty()) {
        std::cout << " [";
        for (size_t i = 0; i < arguments.size(); ++i) {
            if (i > 0) std::cout << " | ";
            std::cout << arguments[i];
        }
        std::cout << "]";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << description << std::endl;
    std::cout << std::endl;
    printSeparator();
}

void printHelp(const std::string& programName, const std::string& description,
               const std::vector<std::string>& arguments) {
    printUsage(programName, description, arguments);
    
    if (!arguments.empty()) {
        std::cout << "Arguments:" << std::endl;
        for (size_t i = 0; i < arguments.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << arguments[i] << std::endl;
        }
        std::cout << std::endl;
    }
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << "                    Run with default settings" << std::endl;
    if (!arguments.empty()) {
        std::cout << "  " << programName << " <value>           Specify " << arguments[0] << std::endl;
    }
}

int parseIntArg(int argc, char* argv[], int argIndex, int defaultValue,
                const std::string& argName) {
    if (argIndex < argc) {
        try {
            return std::stoi(argv[argIndex]);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Invalid value for " << argName
                      << ". Using default: " << defaultValue << std::endl;
        }
    }
    return defaultValue;
}

void printSeparator(char ch, int length) {
    std::cout << std::string(length, ch) << std::endl;
}

void printHeader(const std::string& title) {
    std::cout << std::endl << std::string(60, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void printSection(const std::string& title) {
    std::cout << std::endl << "--- " << title << " ---" << std::endl;
}

} // namespace demo
} // namespace nlm