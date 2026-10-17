// CLI framework macros and helper functions
#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace nlm {

// CLI help text
extern const std::string CLI_HELP;

// CLI error prefix
extern const std::string CLI_ERROR_PREFIX;

// CLI version
extern const std::string CLI_VERSION;

// Check if string starts with prefix
inline bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

// Extract value from --key=value format
inline std::string extractValue(const std::string& arg) {
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        return arg.substr(pos + 1);
    }
    return "";
}

// Convert string to lowercase
inline std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

// Parse benchmark configuration string
inline void parseBenchmarkConfig(const std::string& value, std::vector<std::pair<size_t, size_t>>& configs) {
    std::stringstream ss(value);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        std::stringstream itemStream(item);
        size_t neurons, steps;
        char comma;
        if (itemStream >> neurons >> comma >> steps) {
            configs.push_back({neurons, steps});
        }
    }
}

} // namespace nlm