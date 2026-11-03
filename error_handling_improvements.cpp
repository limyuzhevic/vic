#error_handling_improvements.cpp
// Implementation of error handling improvements for NLM

#include "error_handling_improvements.hpp"
#include <sstream>

namespace nlm {

std::string NLMError::getErrorCodeString(ErrorCode code) {
    switch (code) {
        case ErrorCode::SUCCESS:
            return "SUCCESS";
        case ErrorCode::NULL_POINTER:
            return "NULL_POINTER";
        case ErrorCode::INVALID_PARAMETER:
            return "INVALID_PARAMETER";
        case ErrorCode::FILE_NOT_FOUND:
            return "FILE_NOT_FOUND";
        case ErrorCode::MEMORY_ALLOCATION_FAILED:
            return "MEMORY_ALLOCATION_FAILED";
        case ErrorCode::INITIALIZATION_FAILED:
            return "INITIALIZATION_FAILED";
        case ErrorCode::CONFIGURATION_ERROR:
            return "CONFIGURATION_ERROR";
        case ErrorCode::NETWORK_ERROR:
            return "NETWORK_ERROR";
        case ErrorCode::TIMEOUT:
            return "TIMEOUT";
        case ErrorCode::UNKNOWN_ERROR:
            return "UNKNOWN_ERROR";
        default:
            return "UNKNOWN_ERROR";
    }
}

void NLMError::logError(ErrorCode code, const std::string& context, const std::string& details) {
    std::ostringstream oss;
    oss << "[NLM Error " << getErrorCodeString(code) << "] " << context;
    if (!details.empty()) {
        oss << " - " << details;
    }
    
    // Use different log levels based on error severity
    switch (code) {
        case ErrorCode::NULL_POINTER:
        case ErrorCode::INVALID_PARAMETER:
            NLM_LOG_WARNING(oss.str());
            break;
        case ErrorCode::FILE_NOT_FOUND:
        case ErrorCode::CONFIGURATION_ERROR:
            NLM_LOG_ERROR(oss.str());
            break;
        case ErrorCode::MEMORY_ALLOCATION_FAILED:
        case ErrorCode::INITIALIZATION_FAILED:
            NLM_LOG_CRITICAL(oss.str());
            break;
        case ErrorCode::NETWORK_ERROR:
        case ErrorCode::TIMEOUT:
            NLM_LOG_ERROR(oss.str());
            break;
        default:
            NLM_LOG_ERROR(oss.str());
            break;
    }
}

} // namespace nlm
