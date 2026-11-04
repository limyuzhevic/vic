#include "CommandLineParsing.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>

namespace nlm {

// CommandLineParser implementation - moving helper functions here

bool CommandLineParser::isFlag(const std::string& key) const {
    auto it = std::find_if(options.begin(), options.end(), 
        [&key](const OptionDef& opt) {
            return opt.longOpt == key || opt.shortOpt == key;
        });
    
    return (it != options.end() && it->type == "flag");
}

void CommandLineParser::printError(const std::string& error) const {
    if (verbose || debug) {
        std::cerr << "Error: " << error << std::endl;
    } else {
        std::cerr << error << std::endl;
    }
}

void CommandLineParser::printHelp() const {
    std::cout << getHelpText();
}

void CommandLineParser::printVersion() const {
    std::cout << "NLM (Neural Learning Machine) v1.0.0" << std::endl;
    std::cout << "Enhanced Command Line Interface" << std::endl;
    std::cout << "Build with enhanced configuration and logging support" << std::endl;
}

// Helper function implementations for boolean, integer, double, and log level parsing

bool parseBool(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        return false;
    }
    
    throw std::invalid_argument("Invalid boolean string: " + str);
}

int parseInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (const std::exception&) {
        throw std::invalid_argument("Invalid integer string: " + str);
    }
}

double parseDouble(const std::string& str) {
    try {
        return std::stod(str);
    } catch (const std::exception&) {
        throw std::invalid_argument("Invalid double string: " + str);
    }
}

LogLevel parseLogLevel(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "trace") return LogLevel::Trace;
    if (lower == "debug") return LogLevel::Debug;
    if (lower == "info") return LogLevel::Info;
    if (lower == "notice") return LogLevel::Notice;
    if (lower == "warning") return LogLevel::Warning;
    if (lower == "error") return LogLevel::Error;
    if (lower == "critical") return LogLevel::Critical;
    if (lower == "alert") return LogLevel::Alert;
    if (lower == "emergency") return LogLevel::Emergency;
    
    throw std::invalid_argument("Invalid log level: " + str);
}

} // namespace nlm