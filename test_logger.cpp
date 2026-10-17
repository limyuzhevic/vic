#include <iostream>
#include "src/core/Logger/Logger.hpp"

int main() {
    // Initialize the global logger
    auto& logger = nlm::Logger::getGlobal();
    
    // Test logging with all log levels
    logger.trace("This is a trace message");
    logger.verbose("This is a verbose message");
    logger.debug("This is a debug message");
    logger.info("This is an info message");
    logger.notice("This is a notice message");
    logger.warning("This is a warning message");
    logger.error("This is an error message");
    logger.critical("This is a critical message");
    logger.alert("This is an alert message");
    logger.emergency("This is an emergency message");
    logger.fatal("This is a fatal message");
    
    // Test structured logging
    std::map<std::string, std::string> fields;
    fields["key1"] = "value1";
    fields["key2"] = "value2";
    
    logger.info("This is a structured log message", "", 0, "", fields);
    
    // Test macro logging
    NLM_LOG_TRACE("Using NLM_LOG_TRACE macro");
    NLM_LOG_VERBOSE("Using NLM_LOG_VERBOSE macro");
    NLM_LOG_DEBUG("Using NLM_LOG_DEBUG macro");
    NLM_LOG_INFO("Using NLM_LOG_INFO macro");
    NLM_LOG_NOTICE("Using NLM_LOG_NOTICE macro");
    NLM_LOG_WARNING("Using NLM_LOG_WARNING macro");
    NLM_LOG_ERROR("Using NLM_LOG_ERROR macro");
    NLM_LOG_CRITICAL("Using NLM_LOG_CRITICAL macro");
    NLM_LOG_ALERT("Using NLM_LOG_ALERT macro");
    NLM_LOG_EMERGENCY("Using NLM_LOG_EMERGENCY macro");
    NLM_LOG_FATAL("Using NLM_LOG_FATAL macro");
    
    std::cout << "All logging tests completed successfully!" << std::endl;
    return 0;
}
