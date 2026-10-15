#include "ErrorHandler.hpp"
#include <stdexcept>

namespace nlm {

void ExceptionErrorHandler::handleError(const ErrorInfo& error) {
    // Convert to exception and throw
    throwAsException(error);
}

bool ExceptionErrorHandler::canRecover(const ErrorInfo& error) const {
    // All errors can be represented as exceptions, so recovery depends on the exception
    try {
        throwAsException(error);
    } catch (const std::exception& e) {
        // Check if exception has a retry mechanism
        return false; // In this implementation, all exceptions are considered non-recoverable
    }
}

std::string ExceptionErrorHandler::getRecoveryAction(const ErrorInfo& error) const {
    return "exception";
}

std::chrono::milliseconds ExceptionErrorHandler::getRecoveryDelay(const ErrorInfo& error) const {
    return std::chrono::milliseconds(0);
}

void ExceptionErrorHandler::throwAsException(const ErrorInfo& error) {
    std::stringstream ss;
    ss << "Error in test " << error.testName << ": " << error.message;
    
    switch (error.type) {
        case ErrorType::TestFailure:
            throw std::runtime_error("Test failed: " + error.message);
        case ErrorType::ConfigurationError:
            throw std::invalid_argument("Configuration error: " + error.message);
        case ErrorType::PluginError:
            throw std::runtime_error("Plugin error: " + error.message);
        case ErrorType::ResourceError:
            throw std::runtime_error("Resource error: " + error.message);
        case ErrorType::NetworkError:
            throw std::runtime_error("Network error: " + error.message);
        case ErrorType::MemoryError:
            throw std::bad_alloc();
        case ErrorType::TimeoutError:
            throw std::runtime_error("Timeout error: " + error.message);
        case ErrorType::DependencyError:
            throw std::runtime_error("Dependency error: " + error.message);
        case ErrorType::ValidationError:
            throw std::invalid_argument("Validation error: " + error.message);
        case ErrorType::SystemError:
            throw std::runtime_error("System error: " + error.message);
        case ErrorType::CustomError:
            throw std::runtime_error(error.message);
        default:
            throw std::runtime_error("Unknown error: " + error.message);
    }
}

bool ExceptionErrorHandler::catchException(const std::exception& e) {
    // Try to handle specific exceptions
    try {
        throw;
    } catch (const std::bad_alloc&) {
        return true; // Memory allocation error
    } catch (const std::invalid_argument&) {
        return true; // Invalid argument
    } catch (const std::runtime_error&) {
        return true; // Runtime error
    } catch (...) {
        return false; // Unknown exception
    }
}

} // namespace nlm
