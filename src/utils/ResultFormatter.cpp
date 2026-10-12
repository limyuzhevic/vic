#include "ResultFormatter.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <string>

namespace nlm {

ResultFormatter::ResultFormatter() {
}

ResultFormatter::~ResultFormatter() {
}

void ResultFormatter::format(const std::map<std::string, std::string>& data, const std::string& format) {
    if (format == "json") {
        formatAsJson(data);
    } else if (format == "yaml") {
        formatAsYaml(data);
    } else {
        formatAsText(data);
    }
}

void ResultFormatter::formatAsJson(const std::map<std::string, std::string>& data) {
    // Simple JSON formatting without external libraries
    std::cout << "{\n";
    bool first = true;
    for (const auto& pair : data) {
        if (!first) std::cout << ",\n";
        std::cout << "  \"" << pair.first << "\": \"" << pair.second << "\"";
        first = false;
    }
    std::cout << "\n}" << std::endl;
}

void ResultFormatter::formatAsYaml(const std::map<std::string, std::string>& data) {
    // Simple YAML formatting without external libraries
    for (const auto& pair : data) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
}

void ResultFormatter::formatAsText(const std::map<std::string, std::string>& data) {
    for (const auto& pair : data) {
        std::cout << std::left << std::setw(20) << pair.first << ": " << pair.second << std::endl;
    }
}

} // namespace nlm