#include "LoggingManager.hpp"
#include "Logger.hpp"

namespace nlm {

std::shared_ptr<Logger> LoggingManager::setupLogging(const CLI::Options& cliOptions) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    
    // Set log level
    LogLevel logLevel;
    if (cliOptions.logLevel == "debug") {
        logLevel = LogLevel::Debug;
    } else if (cliOptions.logLevel == "warning") {
        logLevel = LogLevel::Warning;
    } else if (cliOptions.logLevel == "error") {
        logLevel = LogLevel::Error;
    } else if (cliOptions.logLevel == "critical") {
        logLevel = LogLevel::Critical;
    } else {
        logLevel = LogLevel::Info;
    }
    
    consoleLogger->setLevel(logLevel);
    
    // Set up file logging if specified
    if (!cliOptions.logFile.empty()) {
        consoleLogger->setOutputFile(cliOptions.logFile);
    }
    
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Logger initialized with level: " + cliOptions.logLevel);
    if (!cliOptions.logFile.empty()) {
        NLM_LOG_INFO("Log file: " + cliOptions.logFile);
    }
    
    return logger;
}

} // namespace nlm
