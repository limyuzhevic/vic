// StatusReporter implementation

#include "HelpSystem.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {
namespace cli {

GlobalOptions StatusReporter::globalOpts;
bool StatusReporter::coloredOutput = true;

void StatusReporter::info(const std::string& message) {
    if (globalOpts.quiet) return;
    
    std::cout << "[INFO] " << message << "\n";
}

void StatusReporter::warning(const std::string& message) {
    if (globalOpts.quiet) return;
    
    std::cerr << "[WARNING] " << message << "\n";
}

void StatusReporter::error(const std::string& message) {
    std::cerr << "[ERROR] " << message << "\n";
}

void StatusReporter::success(const std::string& message) {
    if (globalOpts.quiet) return;
    
    std::cout << "[SUCCESS] " << message << "\n";
}

void StatusReporter::progress(const std::string& message) {
    if (globalOpts.quiet) return;
    
    std::cout << "\r" << message << std::flush;
}

void StatusReporter::setGlobalOptions(const GlobalOptions& opts) {
    globalOpts = opts;
}

void StatusReporter::enableColoredOutput(bool enable) {
    coloredOutput = enable;
}

// Environment implementation

std::string Environment::getVar(const std::string& name, const std::string& defaultValue) {
    const char* value = std::getenv(name.c_str());
    return value ? value : defaultValue;
}

int Environment::getVar(const std::string& name, int defaultValue) {
    const char* value = std::getenv(name.c_str());
    if (!value) return defaultValue;
    
    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
}

double Environment::getVar(const std::string& name, double defaultValue) {
    const char* value = std::getenv(name.c_str());
    if (!value) return defaultValue;
    
    try {
        return std::stod(value);
    } catch (...) {
        return defaultValue;
    }
}

bool Environment::getVar(const std::string& name, bool defaultValue) {
    const char* value = std::getenv(name.c_str());
    if (!value) return defaultValue;
    
    std::string val = value;
    std::transform(val.begin(), val.end(), val.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return val == "true" || val == "1" || val == "yes";
}

std::vector<std::string> Environment::getVarList(const std::string& name, char separator) {
    std::vector<std::string> result;
    const char* value = std::getenv(name.c_str());
    if (!value) return result;
    
    std::string str(value);
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, separator)) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

bool Environment::setVar(const std::string& name, const std::string& value) {
    // Note: This is environment-specific and may not work on all platforms
    // For portability, this is mainly for demonstration
    return std::setenv(name.c_str(), value.c_str(), 1) == 0;
}

bool Environment::isSet(const std::string& name) {
    return std::getenv(name.c_str()) != nullptr;
}

} // namespace cli
} // namespace nlm
