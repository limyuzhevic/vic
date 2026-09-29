#include "NLMErrors.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

void logNLMError(const std::string& operation, ErrorCode errorCode, const std::string& details = "") {
    std::string userFriendlyMsg = generateUserFriendlyError(operation, errorCode, details);
    std::string technicalMsg = "Operation: " + operation + " | Error: " + errorCodeToString(errorCode);
    
    if (!details.empty()) {
        technicalMsg += " | Details: " + details;
    }
    
    // Log both user-friendly and technical messages
    NLM_LOG_ERROR(userFriendlyMsg);
    
    // For debugging, also log technical details
    if (errorCode == ErrorCode::UnknownError || 
        errorCode == ErrorCode::FileAccessDenied ||
        errorCode == ErrorCode::FileNotFound) {
        NLM_LOG_DEBUG(technicalMsg);
    }
}

void safeExecute(const std::string& operation, std::function<void()> func, const ErrorCode& errorCode) {
    try {
        func();
    } catch (const std::bad_alloc&) {
        logNLMError(operation, ErrorCode::MemoryAllocationFailed, "Out of memory during " + operation);
        throwNLMException(generateUserFriendlyError(operation, ErrorCode::MemoryAllocationFailed), ErrorCode::MemoryAllocationFailed);
    } catch (const std::exception& e) {
        logNLMError(operation, errorCode, std::string("Exception: ") + e.what());
        throwNLMException(generateUserFriendlyError(operation, errorCode, e.what()), errorCode);
    } catch (...) {
        logNLMError(operation, errorCode, "Unknown exception during " + operation);
        throwNLMException(generateUserFriendlyError(operation, errorCode, "Unknown exception"), errorCode);
    }
}

bool safeExecuteOrReturn(const std::string& operation, std::function<bool()> func, const ErrorCode& errorCode) {
    try {
        return func();
    } catch (const std::bad_alloc&) {
        logNLMError(operation, ErrorCode::MemoryAllocationFailed, "Out of memory during " + operation);
        return false;
    } catch (const std::exception& e) {
        logNLMError(operation, errorCode, std::string("Exception: ") + e.what());
        return false;
    } catch (...) {
        logNLMError(operation, errorCode, "Unknown exception during " + operation);
        return false;
    }
}

} // namespace nlm
