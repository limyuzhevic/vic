// Initialize exceptions logger
Logger::getGlobal().addLogger(std::make_shared<ConsoleLogger>(LogLevel::Warning));

void setupExceptionHandling() {
    // Configure exception handling options
    Logger::getGlobal().setLevel(LogLevel::Warning);
    
    // Enable colors if possible
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    consoleLogger->setUseColors(true);
    Logger::getGlobal().addLogger(consoleLogger);
    
    NLM_LOG_INFO("Exception handling system initialized");
}