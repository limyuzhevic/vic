#include "ErrorHandling.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// ErrorHandling implementation

// Static global error handler instance
namespace {
    std::unique_ptr<ErrorHandler>& globalErrorHandler() {
        static std::unique_ptr<ErrorHandler> handler = std::make_unique<DefaultErrorHandler>();
        return handler;
    }
}

ErrorHandler& ErrorHandler::getGlobal() {
    return *globalErrorHandler();
}

void ErrorHandler::setGlobal(std::unique_ptr<ErrorHandler> handler) {
    if (handler) {
        globalErrorHandler() = std::move(handler);
    }
}

bool DefaultErrorHandler::handleError(const NLMError& error, const ErrorContext& context) {
    // Log the error with full context
    NLM_LOG_ERROR("Error in " + context.component + "::" + context.operation + ": " + error.what());
    
    if (!context.details.empty()) {
        NLM_LOG_ERROR("  Details: " + context.details);
    }
    
    if (!context.suggestion.empty()) {
        NLM_LOG_ERROR("  Suggestion: " + context.suggestion);
    }
    
    if (!error.getFile().empty()) {
        NLM_LOG_ERROR("  Location: " + error.getFile() + ":" + std::to_string(error.getLine()));
    }
    
    NLM_LOG_ERROR("  Error code: " + std::to_string(static_cast<int>(error.getErrorCode())));
    
    // For now, always rethrow the error
    // This can be customized by derived error handlers
    return false;
}

// NLMError implementation

std::string NLMError::formatError(ErrorCode code, const std::string& message) {
    return "NLM Error " + std::to_string(static_cast<int>(code)) + ": " + message;
}

std::string NLMError::formatError(ErrorCode code, const std::string& message,
                                   const std::string& file, int line, const std::string& function) {
    std::stringstream ss;
    ss << "NLM Error " << static_cast<int>(code) << ": " << message;
    if (!file.empty()) {
        ss << " (in " << file;
        if (line > 0) {
            ss << ":" << line;
        }
        if (!function.empty()) {
            ss << " in " << function;
        }
        ss << ")";
    }
    return ss.str();
}

NLMError NLMError::configFileError(const std::string& filepath, const std::string& reason) {
    return NLMError(ErrorCode::ConfigFileError, "Failed to load config file '" + filepath + "': " + reason);
}

NLMError NLMError::invalidRange(const std::string& operation, const std::string& param1, const std::string& param2) {
    return NLMError(ErrorCode::InvalidArgument, "Invalid range in " + operation + ": " + param1 + " to " + param2);
}

NLMError NLMError::notImplemented(const std::string& feature) {
    return NLMError(ErrorCode::NotImplemented, "Feature not implemented: " + feature);
}

NLMError NLMError::systemNotIntegrated(const std::string& system) {
    return NLMError(ErrorCode::SystemNotIntegrated, "System not integrated: " + system);
}

} // namespace nlm
