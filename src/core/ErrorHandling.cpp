# Error handling implementation file for NLM
// This file provides comprehensive error handling infrastructure for the NLM project

#include "ErrorHandling.hpp"
#include <chrono>
#include <sstream>

namespace nlm {

// Implementation of timestamp generation
std::string ErrorHandling::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time), "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << ms.count() << "Z";
    return oss.str();
}

// Implementation of error ID generation
std::string ErrorHandling::generateErrorId() {
    static std::atomic<size_t> counter{0};
    return "NLM-" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + 
           "-" + std::to_string(counter++);
}

// Helper functions for error message construction
std::string ErrorHandling::errorCategoryToString(ErrorCategory category) {
    static const std::unordered_map<ErrorCategory, std::string> categoryStrings {
        {ErrorCategory::None, "None"},
        {ErrorCategory::Memory, "Memory"},
        {ErrorCategory::Validation, "Validation"},
        {ErrorCategory::Configuration, "Configuration"},
        {ErrorCategory::Plasticity, "Plasticity"},
        {ErrorCategory::Neural, "Neural"},
        {ErrorCategory::System, "System"},
        {ErrorCategory::Resource, "Resource"},
        {ErrorCategory::Thread, "Thread"},
        {ErrorCategory::IO, "IO"},
        {ErrorCategory::Network, "Network"},
        {ErrorCategory::Algorithm, "Algorithm"},
        {ErrorCategory::Integration, "Integration"},
        {ErrorCategory::Performance, "Performance"}
    };
    
    auto it = categoryStrings.find(category);
    return (it != categoryStrings.end()) ? it->second : "Unknown";
}

std::string ErrorHandling::errorSeverityToString(ErrorSeverity severity) {
    static const std::unordered_map<ErrorSeverity, std::string> severityStrings {
        {ErrorSeverity::Debug, "Debug"},
        {ErrorSeverity::Info, "Info"},
        {ErrorSeverity::Warning, "Warning"},
        {ErrorSeverity::Error, "Error"},
        {ErrorSeverity::Critical, "Critical"},
        {ErrorSeverity::Fatal, "Fatal"}
    };
    
    auto it = severityStrings.find(severity);
    return (it != severityStrings.end()) ? it->second : "Unknown";
}

std::string ErrorHandling::resourceStateToString(ResourceState state) {
    static const std::unordered_map<ResourceState, std::string> stateStrings {
        {ResourceState::None, "None"},
        {ResourceState::Initializing, "Initializing"},
        {ResourceState::Available, "Available"},
        {ResourceState::InUse, "InUse"},
        {ResourceState::Released, "Released"},
        {ResourceState::Corrupted, "Corrupted"}
    };
    
    auto it = stateStrings.find(state);
    return (it != stateStrings.end()) ? it->second : "Unknown";
}

} // namespace nlm
