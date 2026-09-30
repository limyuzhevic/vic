#include "Exceptions.hpp"
#include "Logger/Logger.hpp"
#include <stdexcept>

namespace nlm {

void logException(const std::exception& e, const std::string& context) {
    // Check if it's an NLMException with severity info
    NLMException::Severity severity = NLMException::Severity::ERROR;
    std::string message = e.what();
    
    // Try to extract NLMException specific info
    try {
        const NLMException* nlmEx = dynamic_cast<const NLMException*>(&e);
        if (nlmEx) {
            severity = nlmEx->severity();
            message = nlmEx->what();
        }
    } catch (...) {
        // If dynamic_cast fails, use default error severity
    }
    
    // Log the exception with appropriate severity
    switch (severity) {
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
}

LogLevel exceptionSeverityToLogLevel(NLMException::Severity severity) {
    switch (severity) {
        case NLMException::Severity::DEBUG:
            return LogLevel::Debug;
        case NLMException::Severity::INFO:
            return LogLevel::Info;
        case NLMException::Severity::WARNING:
            return LogLevel::Warning;
        case NLMException::Severity::ERROR:
            return LogLevel::Error;
        case NLMException::Severity::CRITICAL:
            return LogLevel::Critical;
        default:
            return LogLevel::Error;
    }
}

} // namespace nlm