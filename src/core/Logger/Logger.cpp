#include "Logger.hpp"
#include "AdvancedLogger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace nlm {

// Global initialization for the new advanced logging system
void initializeAdvancedLogging(const std::string& logFile = "nlm.log") {
    // Use the advanced logger manager instead of the old system
    auto logger = nlm::LoggerFactory::createRotatingFileLogger(logFile);
    nlm::AdvancedLoggerManager::getInstance().initialize(logger);
}

// Backwards compatibility functions - these use the advanced logger
void logGlobal(LogLevel level, const std::string& message) {
    AdvancedLoggerManager::log(static_cast<AdvancedLogLevel>(level), message);
}

void debugGlobal(const std::string& message) {
    AdvancedLoggerManager::debug(message);
}

void infoGlobal(const std::string& message) {
    AdvancedLoggerManager::info(message);
}

void warningGlobal(const std::string& message) {
    AdvancedLoggerManager::warning(message);
}

void errorGlobal(const std::string& message) {
    AdvancedLoggerManager::error(message);
}

void criticalGlobal(const std::string& message) {
    AdvancedLoggerManager::critical(message);
}

// Setup functions for different environments
void setupDevelopmentLogging(const std::string& logFile = "nlm_development.log") {
    LoggerFactory::setupDevelopmentLogger(logFile);
}

void setupProductionLogging(const std::string& logFile = "nlm_production.log") {
    LoggerFactory::setupProductionLogger(logFile);
}

void setupPerformanceLogging(const std::string& logFile = "nlm_performance.log") {
    LoggerFactory::setupPerformanceLogger(logFile);
}

void setupIntegrationLogging(const std::string& logFile = "nlm_integration.log") {
    LoggerFactory::setupIntegrationLogger(logFile);
}

} // namespace nlm

