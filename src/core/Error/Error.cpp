#include "Error.hpp"

namespace nlm {

// Error handler implementation
ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

void ErrorHandler::addErrorHandler(std::function<void(const NLMException&)>) {
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_.push_back(std::move(handler));
}

void ErrorHandler::logError(const NLMException& error) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Log to all registered handlers
    for (auto& handler : handlers_) {
        try {
            handler(error);
        } catch (const std::exception& e) {
            // If handler throws, log to stderr as fallback
            std::cerr << "Error handler threw exception: " << e.what() << std::endl;
        }
    }
    
    // Default fallback logging
    std::cerr << "NLM Error: " << error.what() << std::endl;
}

void ErrorHandler::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    logLevel_ = level;
}

LogLevel ErrorHandler::getLogLevel() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return logLevel_;
}

// Exception-safe wrapper implementation
template<typename T, typename Func>
T ExceptionSafe::executeSafely(Func&& func, const std::string& operationName) {
    try {
        return func();
    } catch (const ConfigError& e) {
        // Re-throw configuration errors as-is
        throw;
    } catch (const BrainError& e) {
        // Re-throw brain errors as-is
        throw;
    } catch (const std::exception& e) {
        // Wrap other exceptions in a BrainError
        throw BrainError(std::format("Operation '{}' failed: {}", operationName, e.what()));
    }
}

// Explicit instantiation of template
// (If needed, we can add it here)

} // namespace nlm
