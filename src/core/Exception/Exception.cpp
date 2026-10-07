#include "Exception.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <ctime>

namespace nlm {

// Initialize static members for ExceptionHandler
std::vector<ExceptionHandler::HandlerInfo> ExceptionHandler::handlers_;
std::mutex ExceptionHandler::mutex_;

// Implementation of static methods that need to be in .cpp
void ExceptionHandler::registerHandler(const HandlerInfo& handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_.push_back(handler);
}

bool ExceptionHandler::handleException(const NLMException& ex, const std::string& operation) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find matching handler
    for (const auto& handler : handlers_) {
        bool componentMatch = std::find(handler.components.begin(), handler.components.end(), ex.getComponent()) != handler.components.end();
        bool codeMatch = std::find(handler.errorCodes.begin(), handler.errorCodes.end(), ex.getCode()) != handler.errorCodes.end();
        
        if (componentMatch && codeMatch) {
            return executeHandler(ex, handler, operation);
        }
    }
    
    // No matching handler found
    return false;
}

bool ExceptionHandler::executeHandler(const NLMException& ex, const HandlerInfo& handler, const std::string& operation) {
    switch (handler.action) {
        case Action::THROW:
            return false; // Let the exception propagate
        
        case Action::LOG_AND_THROW:
            if (handler.enableLogging) {
                logException(ex, operation);
            }
            return false;
        
        case Action::LOG_AND_CONTINUE:
            if (handler.enableLogging) {
                logException(ex, operation);
            }
            return true;
        
        case Action::IGNORE:
            return true;
        
        case Action::RETRY:
            return executeWithRetry(ex, handler, operation);
        
        case Action::FALLBACK:
            if (handler.enableLogging) {
                logException(ex, operation);
            }
            return true;
        
        default:
            return false;
    }
}

bool ExceptionHandler::executeWithRetry(const NLMException& ex, const HandlerInfo& handler, const std::string& operation) {
    for (int attempt = 0; attempt < handler.maxRetries; ++attempt) {
        try {
            // In a real implementation, we would retry the operation here
            // For now, we'll just log the retry attempt
            if (handler.enableLogging) {
                std::cout << "Retrying operation after failure (attempt " 
                          << (attempt + 1) << "/" << handler.maxRetries << "): "
                          << ex.what() << std::endl;
            }
            
            if (attempt < handler.maxRetries - 1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(
                    static_cast<int>(handler.retryDelay * 1000)));
            }
            
            return false; // Continue waiting for retry
        } catch (const std::exception& e) {
            if (attempt == handler.maxRetries - 1) {
                if (handler.enableLogging) {
                    std::cout << "Retry failed after " << handler.maxRetries 
                              << " attempts: " << e.what() << std::endl;
                }
                return false; // Let the exception propagate
            }
        }
    }
    
    return false;
}

void ExceptionHandler::logException(const NLMException& ex, const std::string& operation) {
    std::ostringstream oss;
    oss << "Exception in " << ex.getComponent() << "::" << operation << ": "
        << ex.what()
        << " (Code: " << static_cast<int>(ex.getCode()) << ")";
    
    if (!ex.getContext("file").empty()) {
        oss << " at " << ex.getContext("file") << ":" << ex.getContext("line");
    }
    
    std::cout << oss.str() << std::endl;
    
    // Log context if available
    const auto& context = ex.getAllContext();
    if (!context.empty()) {
        oss.str("");
        oss << "  Context: ";
        bool first = true;
        for (const auto& pair : context) {
            if (!first) oss << ", ";
            oss << pair.first << "=" << pair.second;
            first = false;
        }
        std::cout << oss.str() << std::endl;
    }
}

// Implementation of LogPerformanceMonitor
struct LogPerformanceMonitor::Impl {
    std::string name_;
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point endTime_;
    size_t messageCount_ = 0;
    double totalTime_ = 0.0;
    bool isRunning_ = false;
    std::mutex mutex_;
};

LogPerformanceMonitor::LogPerformanceMonitor(const std::string& name) : pImpl(std::make_unique<Impl>()) {
    pImpl->name_ = name;
    pImpl->isRunning_ = true;
    pImpl->startTime_ = std::chrono::steady_clock::now();
}

LogPerformanceMonitor::~LogPerformanceMonitor() {
    if (pImpl->isRunning_) {
        stop();
    }
}

void LogPerformanceMonitor::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    
    auto now = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(now - pImpl->startTime_).count();
    
    pImpl->messageCount_++;
    pImpl->totalTime_ += elapsed;
    
    // Log the message with performance information
    std::cout << "[PERF] " << pImpl->name_ << ": " << message
              << " (message " << pImpl->messageCount_ << ", avg time: "
              << (pImpl->messageCount_ > 0 ? pImpl->totalTime_ / pImpl->messageCount_ : 0.0)
              << "s)" << std::endl;
}

double LogPerformanceMonitor::getAverageTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->messageCount_ > 0 ? pImpl->totalTime_ / pImpl->messageCount_ : 0.0;
}

size_t LogPerformanceMonitor::getMessageCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    return pImpl->messageCount_;
}

void LogPerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->messageCount_ = 0;
    pImpl->totalTime_ = 0.0;
    pImpl->startTime_ = std::chrono::steady_clock::now();
}

void LogPerformanceMonitor::stop() {
    if (!pImpl->isRunning_) return;
    
    auto now = std::chrono::steady_clock::now();
    double totalElapsed = std::chrono::duration<double>(now - pImpl->startTime_).count();
    
    std::lock_guard<std::mutex> lock(pImpl->mutex_);
    pImpl->totalTime_ = totalElapsed;
    pImpl->isRunning_ = false;
    
    // Log final statistics
    std::cout << "[PERF] " << pImpl->name_ << " stopped after "
              << totalElapsed << "s, " << pImpl->messageCount_ << " messages"
              << " (avg: " << getAverageTime() << "s)" << std::endl;
}

// Implementation of LogEvent
LogEvent::LogEvent(Type type, const std::string& message,
                   const std::string& source,
                   const std::unordered_map<std::string, std::string>& metadata)
    : type_(type), message_(message), source_(source), metadata_(metadata),
      timestamp_(std::chrono::system_clock::now()) {}

// Implementation of LogPerformanceMonitor methods that need to be in .cpp
void LogPerformanceMonitor::stop() {
    if (pImpl->isRunning_) {
        auto now = std::chrono::steady_clock::now();
        double totalElapsed = std::chrono::duration<double>(now - pImpl->startTime_).count();
        
        std::lock_guard<std::mutex> lock(pImpl->mutex_);
        pImpl->totalTime_ = totalElapsed;
        pImpl->isRunning_ = false;
        
        // Log final statistics
        std::cout << "[PERF] " << pImpl->name_ << " stopped after "
                  << totalElapsed << "s, " << pImpl->messageCount_ << " messages"
                  << " (avg: " << getAverageTime() << "s)" << std::endl;
    }
}

} // namespace nlm