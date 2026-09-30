#include "ExceptionIntegration.hpp"
#include "Logger/Logger.hpp"
#include <unordered_map>

namespace nlm {

// Global exception handler settings
namespace {
    bool exceptionLoggingEnabled_ = true;
    bool exceptionRecoveryEnabled_ = true;
}

void logNLMException(const std::exception& e, const std::string& context) {
    if (!exceptionLoggingEnabled_) {
        return;
    }
    
    std::string message = e.what();
    
    // Try to extract NLMException specific info
    try {
        const NLMException* nlmEx = dynamic_cast<const NLMException*>(&e);
        if (nlmEx) {
            // Log with appropriate severity based on exception severity
            switch (nlmEx->severity()) {
                case NLMException::Severity::DEBUG:
                    Logger::getGlobal().debug(message, "", 0, context);
                    break;
                case NLMException::Severity::INFO:
                    Logger::getGlobal().info(message, "", 0, context);
                    break;
                case NLMException::Severity::WARNING:
                    Logger::getGlobal().warning(message, "", 0, context);
                    break;
                case NLMException::Severity::ERROR:
                    Logger::getGlobal().error(message, "", 0, context);
                    break;
                case NLMException::Severity::CRITICAL:
                    Logger::getGlobal().critical(message, "", 0, context);
                    break;
            }
            return;
        }
    } catch (...) {
        // If dynamic_cast fails, fall back to standard error logging
    }
    
    // Standard exception logging for non-NLM exceptions
    Logger::getGlobal().error(std::string("Unhandled exception: ") + message, "", 0, context);
}

void setExceptionLoggingEnabled(bool enabled) {
    exceptionLoggingEnabled_ = enabled;
}

bool isExceptionLoggingEnabled() {
    return exceptionLoggingEnabled_;
}

} // namespace nlm