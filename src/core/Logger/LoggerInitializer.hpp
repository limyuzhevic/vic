#ifndef NLM_LOGGER_INITIALIZER_H
#define NLM_LOGGER_INITIALIZER_H

#include "LoggingSystem.hpp"
#include <memory>

namespace nlm {

class LoggerInitializer {
public:
    LoggerInitializer() {
        // Initialize spdlog
        spdlog::set_async_mode(8192);  // Buffer 8192 messages
        spdlog::init_mutex();
        
        // Initialize global logger
        LogManager::Logger::initializeDefaultLogger();
        
        // Enable default console logging
        auto globalLogger = LogManager::getGlobalLogger();
        if (globalLogger) {
            auto nlmLogger = globalLogger->getLogger("NLM");
            if (nlmLogger) {
                nlmLogger->setLevel(LogManager::Logger::Level::Info);
            }
        }
        
        // Initialize error handler
        LogManager::initializeErrorHandler(LogManager::ErrorHandler::ErrorAction::LogAndContinue);
        
        // Initialize performance logger
        getPerformanceLogger("Default");
        
        // Log initialization complete
        NLM_LOG_INFO("Logger system initialized successfully");
    }
    
    ~LoggerInitializer() {
        NLM_LOG_INFO("Shutting down logger system");
        LogManager::Logger::shutdown();
    }
};

// Global logger initializer
static LoggerInitializer loggerInitializer;

} // namespace nlm

#endif // NLM_LOGGER_INITIALIZER_H
