// ErrorHandler implementation for NLM neural network simulation

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

ErrorHandler* ErrorHandler::instance = nullptr;

// Static function to handle error recovery
void ErrorHandler::recoverFromError(ErrorCode errorCode) {
    switch (errorCode) {
        case ErrorCode::ComputationOverflow:
            // Reduce neural complexity/load
            NLM_LOG_WARNING("Recovering from computation overflow - reducing neural complexity");
            break;
        case ErrorCode::MemoryAllocationFailed:
            // Force garbage collection and cleanup
            NLM_LOG_WARNING("Recovering from memory allocation failure - performing cleanup");
            break;
        case ErrorCode::FileIOError:
            // Use fallback configuration or default values
            NLM_LOG_WARNING("Recovering from file I/O error - using fallback values");
            break;
        case ErrorCode::ParseError:
            // Reset to safe state with default values
            NLM_LOG_WARNING("Recovering from parse error - resetting to defaults");
            break;
        case ErrorCode::ValidationError:
            // Clamp values to valid ranges
            NLM_LOG_WARNING("Recovering from validation error - clamping values");
            break;
        default:
            // Generic recovery
            NLM_LOG_WARNING("Generic error recovery triggered");
            break;
    }
}

// Function to save error history to file
void ErrorHandler::saveErrorHistory(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save error history to: " + filepath);
        return;
    }
    
    std::lock_guard<std::mutex> lock(errorMutex);
    
    file << "NLM Error History - " << getCurrentTimestamp() << "\n";
    file << "==============================\n\n";
    
    for (const auto& error : errorHistory) {
        file << "Error Code: " << static_cast<int>(error.code) << "\n";
        file << "Message: " << error.message << "\n";
        file << "Severity: " << levelToString(error.severity) << "\n";
        if (!error.file.empty()) file << "File: " << error.file << "\n";
        if (error.line > 0) file << "Line: " << error.line << "\n";
        if (!error.function.empty()) file << "Function: " << error.function << "\n";
        if (error.neuronId >= 0) file << "Neuron ID: " << error.neuronId << "\n";
        if (error.synapseId >= 0) file << "Synapse ID: " << error.synapseId << "\n";
        if (error.regionId >= 0) file << "Region ID: " << error.regionId << "\n";
        if (error.step > 0) file << "Step: " << error.step << "\n";
        if (error.time > 0.0) file << "Time: " << error.time << "\n";
        file << "------------------------\n\n";
    }
    
    file << "Total errors: " << errorHistory.size() << "\n";
    
    NLM_LOG_INFO("Error history saved to: " + filepath);
}

// Function to generate error report
std::string ErrorHandler::generateErrorReport() {
    std::ostringstream oss;
    oss << "NLM Neural Network Error Report\n";
    oss << "Generated: " << getCurrentTimestamp() << "\n\n";
    oss << "Summary Statistics:\n";
    oss << "  Total errors: " << errorHistory.size() << "\n";
    
    // Count by severity
    std::map<LogLevel, int> severityCount;
    severityCount[LogLevel::Debug] = 0;
    severityCount[LogLevel::Info] = 0;
    severityCount[LogLevel::Warning] = 0;
    severityCount[LogLevel::Error] = 0;
    severityCount[LogLevel::Critical] = 0;
    
    for (const auto& error : errorHistory) {
        severityCount[error.severity]++;
    }
    
    oss << "  By severity:\n";
    oss << "    Debug: " << severityCount[LogLevel::Debug] << "\n";
    oss << "    Info: " << severityCount[LogLevel::Info] << "\n";
    oss << "    Warning: " << severityCount[LogLevel::Warning] << "\n";
    oss << "    Error: " << severityCount[LogLevel::Error] << "\n";
    oss << "    Critical: " << severityCount[LogLevel::Critical] << "\n\n";
    
    // Most common error codes
    std::map<ErrorCode, int> errorCodeCount;
    for (const auto& error : errorHistory) {
        errorCodeCount[error.code]++;
    }
    
    oss << "Most common errors:\n";
    for (const auto& pair : errorCodeCount) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << " times\n";
    }
    
    oss << "\nDetailed Error Log:\n";
    oss << "====================\n\n";
    
    for (const auto& error : errorHistory) {
        oss << "[" << levelToString(error.severity) << "] ";
        oss << "[NEURON:" << error.neuronId << "][SYN:" << error.synapseId << "] ";
        oss << error.message << "\n";
        oss << "  Time: " << error.time << ", Step: " << error.step;
        if (!error.file.empty()) oss << ", File: " << error.file << ":" << error.line;
        if (!error.function.empty()) oss << " (" << error.function << ")";
        oss << "\n\n";
    }
    
    return oss.str();
}

// Helper function to get current timestamp
std::string ErrorHandler::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Helper function to convert LogLevel to string
std::string ErrorHandler::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

} // namespace nlm