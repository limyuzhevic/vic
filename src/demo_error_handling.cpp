#include "EnhancedLogger.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// Example: Demonstrating enhanced error handling and logging
void demonstrateErrorHandling() {
    std::cout << "=== Enhanced Error Handling and Logging Demo ===" << std::endl << std::endl;
    
    // Initialize logging system
    auto logger = std::make_shared<nlm::Logger>();
    auto enhancedLogger = std::make_shared<nlm::EnhancedLogger>();
    auto consoleLogger = std::make_shared<nlm::EnhancedConsoleLogger>(nlm::LogLevel::Info);
    
    // Configure enhanced console logger
    consoleLogger->setUseColors(true);
    consoleLogger->setOutputFile("demo.log");
    consoleLogger->setMaxFileSize(1024 * 1024); // 1MB
    consoleLogger->setMaxFiles(3);
    
    // Add loggers to enhanced logger
    enhancedLogger->addLogger(consoleLogger);
    enhancedLogger->addLogger(logger);
    
    // Set global enhanced logger
    nlm::Logger::setGlobal(enhancedLogger);
    
    std::cout << "1. Basic Logging Demo:" << std::endl;
    
    // Basic logging
    NLM_LOG_INFO("System initialization started");
    
    // Structured logging with context
    enhancedLogger->logStructured(nlm::LogLevel::Info, 
                                "User authentication attempt",
                                {"user_id", "session_id", "ip_address"});
    
    NLM_LOG_WARNING("Configuration file not found, using defaults");
    NLM_LOG_ERROR("Failed to connect to database server");
    NLM_LOG_CRITICAL("System shutdown - unrecoverable error");
    
    std::cout << "\n2. Error Handling Demo:" << std::endl;
    
    // Generate correlation ID for error tracking
    uint64_t correlationId = nlm::ErrorHandler::generateCorrelationId();
    nlm::ErrorHandler::setGlobalCorrelationId(correlationId);
    
    std::cout << "Generated correlation ID: " << correlationId << std::endl;
    
    // Demonstrate different types of errors
    try {
        NLM_THROW_CONFIGURATION_ERROR("Invalid configuration parameter 'max_neurons'", "CFG_001");
        NLM_THROW_VALIDATION_ERROR("Validation failed: neuron count must be > 0", "VAL_002");
        NLM_THROW_MEMORY_ERROR("Out of memory: failed to allocate neural network", "MEM_003");
    } catch (...) {
        // Errors are caught and logged automatically
        std::cout << "Errors caught and handled" << std::endl;
    }
    
    std::cout << "\n3. Performance Monitoring Demo:" << std::endl;
    
    // Performance monitoring
    enhancedLogger->startOperation("brain_initialization");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    enhancedLogger->endOperation("brain_initialization");
    
    enhancedLogger->startOperation("neuron_synaptic_setup");
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    enhancedLogger->endOperation("neuron_synaptic_setup");
    
    // Log custom performance metrics
    enhancedLogger->logPerformance("total_simulation_time", 150.5, "ms");
    enhancedLogger->logPerformance("neurons_per_second", 50000.0, "neurons/sec");
    
    // Log memory usage
    nlm::EnhancedLogger::getMetrics().recordMemoryUsage(512 * 1024 * 1024); // 512MB
    nlm::EnhancedLogger::getMetrics().recordCpuUsage(25.5); // 25.5%
    
    std::cout << "\n4. Exception Safety Demo:" << std::endl;
    
    // Try-catch with error logging
    try {
        // This would be a real error scenario
        NLM_THROW_CONFIGURATION_ERROR("Test error scenario", "TEST_001");
        
        // Nested exception handling
        try {
            NLM_THROW_VALIDATION_ERROR("Nested error scenario", "TEST_002");
        } catch (...) {
            std::cout << "Inner exception caught" << std::endl;
        }
        
        std::cout << "Should not reach here due to error" << std::endl;
    } catch (...) {
        std::cout << "Exception caught at top level" << std::endl;
    }
    
    std::cout << "\n5. Log File Operations Demo:" << std::endl;
    
    // Log to file with rotation
    NLM_LOG_INFO("This message will be written to demo.log");
    NLM_LOG_WARNING("This is another log message");
    
    // Log some structured data
    enhancedLogger->logStructured(nlm::LogLevel::Info, "Experiment started",
                                {"experiment_id", "sample_size", "duration"});
    
    // Flush logs to file
    consoleLogger->closeOutputFile();
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
    std::cout << "Check demo.log for detailed log output" << std::endl;
}

void demonstrateRobustErrorRecovery() {
    std::cout << "\n=== Robust Error Recovery Demo ===" << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<nlm::Logger>();
    auto enhancedLogger = std::make_shared<nlm::EnhancedLogger>();
    auto consoleLogger = std::make_shared<nlm::EnhancedConsoleLogger>(nlm::LogLevel::Warning);
    
    enhancedLogger->addLogger(consoleLogger);
    enhancedLogger->addLogger(logger);
    nlm::Logger::setGlobal(enhancedLogger);
    
    std::cout << "Testing error recovery mechanisms:" << std::endl;
    
    // Simulate operations that may fail
    for (int i = 0; i < 3; ++i) {
        std::cout << "Attempt " << i + 1 << ": ";
        
        try {
            // Simulate a potential failure
            if (i < 2) {
                NLM_THROW_CONFIGURATION_ERROR("Simulated configuration failure", "SIM_001");
            } else {
                NLM_LOG_INFO("Operation succeeded after recovery");
            }
        } catch (...) {
            std::cout << "Recovery successful" << std::endl;
            
            // Log recovery attempt
            enhancedLogger->logStructured(nlm::LogLevel::Warning,
                                        "Error recovery initiated",
                                        {"attempt", "recovery_action"});
        }
    }
    
    std::cout << "\nError recovery demo complete" << std::endl;
}