#include "ErrorHandling.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <stdexcept>

namespace nlm {

struct ErrorHandling::Impl {
    std::vector<std::pair<std::string, std::string>> errors;
    std::map<std::string, std::vector<std::string>> errorDetails;
    std::chrono::system_clock::time_point lastErrorTime;
    bool criticalErrorOccurred;
};

ErrorHandling::ErrorHandling() : pImpl(std::make_unique<Impl>()) {}

ErrorHandling::~ErrorHandling() = default;

void ErrorHandling::initialize() {
    auto logger = Logger::getGlobal();
    logger->setLevel(LogLevel::Warning);
    NLM_LOG_INFO("Error handling system initialized");
}

void ErrorHandling::logError(const std::string& component, const std::string& message,
                            const std::vector<std::string>& details) {
    auto logger = Logger::getGlobal();
    
    // Format error message
    std::string fullMessage = "[" + component + "] " + message;
    
    // Add details if present
    if (!details.empty()) {
        fullMessage += "\n  Details:";
        for (const auto& detail : details) {
            fullMessage += "\n    - " + detail;
        }
    }
    
    // Log as error
    NLM_LOG_ERROR(fullMessage);
    
    // Store in internal error log
    pImpl->errors.push_back({component, message});
    
    // Store details if present
    if (!details.empty()) {
        pImpl->errorDetails[component] = details;
    }
    
    // Update last error time
    pImpl->lastErrorTime = std::chrono::system_clock::now();
}

bool ErrorHandling::validateParameter(const std::string& name, const std::string& value,
                                      const std::string& type,
                                      const std::vector<std::string>& validValues) {
    auto logger = Logger::getGlobal();
    
    // Check for empty or invalid values
    if (name.empty()) {
        NLM_LOG_ERROR("Parameter validation failed: Parameter name cannot be empty");
        return false;
    }
    
    if (value.empty()) {
        // Empty values might be valid (e.g., for optional parameters)
        NLM_LOG_WARNING("Parameter validation: Parameter '" + name + "' has empty value");
    }
    
    // Type validation (simplified)
    if (type == "int") {
        try {
            std::stoi(value);
        } catch (const std::invalid_argument& e) {
            NLM_LOG_ERROR("Parameter validation failed: Invalid integer value for parameter '" + name + "': " + e.what());
            return false;
        }
    } else if (type == "double") {
        try {
            std::stod(value);
        } catch (const std::invalid_argument& e) {
            NLM_LOG_ERROR("Parameter validation failed: Invalid double value for parameter '" + name + "': " + e.what());
            return false;
        }
    }
    
    // Valid values validation
    if (!validValues.empty()) {
        bool found = false;
        for (const auto& validValue : validValues) {
            if (value == validValue) {
                found = true;
                break;
            }
        }
        if (!found) {
            std::string errorMsg = "Parameter validation failed: Invalid value '" + value + "' for parameter '" + name + "'. Valid values: ";
            for (size_t i = 0; i < validValues.size(); ++i) {
                if (i > 0) errorMsg += ", ";
                errorMsg += validValues[i];
            }
            NLM_LOG_ERROR(errorMsg);
            return false;
        }
    }
    
    return true;
}

std::string ErrorHandling::createErrorReport(const std::string& component, const std::string& context) {
    std::ostringstream report;
    
    report << "=== Error Report ===\n";
    report << "Generated at: " << std::chrono::system_clock::now() << "\n";
    report << "Component: " + component << "\n";
    if (!context.empty()) {
        report << "Context: " + context << "\n";
    }
    report << "\nErrors:\n";
    
    if (pImpl->errors.empty()) {
        report << "  No errors found.\n";
    } else {
        for (const auto& error : pImpl->errors) {
            report << "  - [" + error.first + "] " + error.second << "\n";
            
            // Add details if available
            auto detailIt = pImpl->errorDetails.find(error.first);
            if (detailIt != pImpl->errorDetails.end()) {
                report << "    Details:\n";
                for (const auto& detail : detailIt->second) {
                    report << "      * " + detail << "\n";
                }
            }
        }
    }
    
    report << "\n=== End Report ===";
    return report.str();
}

bool ErrorHandling::hasCriticalErrors() const {
    return pImpl->criticalErrorOccurred || !pImpl->errors.empty();
}

void ErrorHandling::reset() {
    pImpl->errors.clear();
    pImpl->errorDetails.clear();
    pImpl->lastErrorTime = std::chrono::system_clock::now();
    pImpl->criticalErrorOccurred = false;
}
}