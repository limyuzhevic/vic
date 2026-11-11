#include "ErrorHandlingUtils.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <fstream>
#include <filesystem>
#include <ctime>

namespace nlm {

void ErrorHandlingUtils::logError(const std::string& component, const std::string& message,
                                const std::string& file,
                                int line,
                                const std::string& function,
                                const std::vector<std::string>& details) {
    auto logger = Logger::getGlobal();
    
    // Format error message with location information
    std::ostringstream oss;
    oss << "ERROR in component " << component << ": " << message;
    oss << "\n  Location: " << file << ":" << line << " in function " << function;
    
    if (!details.empty()) {
        oss << "\n  Details:";
        for (const auto& detail : details) {
            oss << "\n    - " << detail;
        }
    }
    
    // Log error with appropriate level
    NLM_LOG_ERROR(oss.str());
}

bool ErrorHandlingUtils::validate(const std::string& name, const std::string& value,
                                 const std::string& type,
                                 const std::vector<std::string>& validValues) {
    auto logger = Logger::getGlobal();
    
    // Check name validity
    if (name.empty()) {
        NLM_LOG_ERROR("Parameter validation failed: Parameter name cannot be empty");
        return false;
    }
    
    if (value.empty() && type != "string") {
        // Some types should not be empty
        NLM_LOG_WARNING("Parameter validation: Parameter '" + name + "' has empty value");
    }
    
    // Type validation
    if (type == "int") {
        try {
            std::stoi(value);
        } catch (const std::invalid_argument& e) {
            NLM_LOG_ERROR("Invalid integer value for parameter '" + name + "': " + e.what());
            return false;
        } catch (const std::out_of_range& e) {
            NLM_LOG_ERROR("Integer value out of range for parameter '" + name + "': " + e.what());
            return false;
        }
    } else if (type == "double") {
        try {
            std::stod(value);
        } catch (const std::invalid_argument& e) {
            NLM_LOG_ERROR("Invalid double value for parameter '" + name + "': " + e.what());
            return false;
        } catch (const std::out_of_range& e) {
            NLM_LOG_ERROR("Double value out of range for parameter '" + name + "': " + e.what());
            return false;
        }
    } else if (type == "bool") {
        // Simple boolean validation: "true", "false", "1", "0"
        if (value != "true" && value != "false" && value != "1" && value != "0") {
            NLM_LOG_ERROR("Invalid boolean value for parameter '" + name + "': must be true/false or 1/0");
            return false;
        }
    }
    
    // Valid values check
    if (!validValues.empty()) {
        bool found = false;
        for (const auto& valid : validValues) {
            if (value == valid) {
                found = true;
                break;
            }
        }
        if (!found) {
            NLM_LOG_ERROR("Parameter '" + name + "': value '" + value + "' is not a valid option");
            return false;
        }
    }
    
    return true;
}

int ErrorHandlingUtils::toInt(const std::string& str, int defaultValue) {
    try {
        return std::stoi(str);
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("Failed to convert string to int: " + str + ", using default: " + std::to_string(defaultValue));
        return defaultValue;
    }
}

double ErrorHandlingUtils::toDouble(const std::string& str, double defaultValue) {
    try {
        return std::stod(str);
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("Failed to convert string to double: " + str + ", using default: " + std::to_string(defaultValue));
        return defaultValue;
    }
}

bool ErrorHandlingUtils::toBool(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "true" || lower == "1") return true;
    if (lower == "false" || lower == "0") return false;
    
    NLM_LOG_ERROR("Invalid boolean string: " + str + ", defaulting to false");
    return false;
}

bool ErrorHandlingUtils::fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

bool ErrorHandlingUtils::createBackupDir(const std::string& basePath) {
    std::string timestamp = std::to_string(std::time(nullptr));
    std::string backupPath = basePath + "_backup_" + timestamp;
    
    try {
        if (std::filesystem::create_directory(backupPath)) {
            NLM_LOG_INFO("Created backup directory: " + backupPath);
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create backup directory: " + std::string(e.what()));
    }
    
    return false;
}
}