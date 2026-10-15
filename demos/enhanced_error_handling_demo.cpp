// Example: Enhanced Error Handling Integration
// Demonstrates how to use the new error handling and logging system

#include "ErrorHandling.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <iostream>
#include <fstream>

void demonstrateErrorHandling() {
    std::cout << "=== Enhanced Error Handling and Logging Demo ===" << std::endl;
    
    // Initialize error handling system
    std::cout << "\n1. Initializing error handling system..." << std::endl;
    nlm::ErrorHandling::initialize("./logs");
    
    // Configure log rotation
    nlm::LogRotationConfig rotationConfig;
    rotationConfig.maxFileSize = 5 * 1024 * 1024; // 5MB
    rotationConfig.maxFiles = 5;
    rotationConfig.rotationTime = std::chrono::hours(24);
    
    // Start log aggregator
    nlm::getLogAggregator().start();
    std::cout << "   Error handling system initialized successfully" << std::endl;
    
    // Demonstrate structured error logging
    std::cout << "\n2. Demonstrating structured error logging..." << std::endl;
    
    try {
        // Simulate a configuration error
        nlm::ErrorHandling::throwError(
            nlm::ErrorType::ConfigurationError,
            nlm::ErrorSeverity::Warning,
            "Config",
            "loadFromFile",
            "Configuration file not found: config.txt"
        );
    } catch (const nlm::Error& e) {
        std::cout << "   Configuration error logged:" << std::endl;
        std::cout << "     " << e.what() << std::endl;
        nlm::ErrorHandling::handleError(e);
    }
    
    try {
        // Simulate a memory error
        nlm::ErrorHandling::throwError(
            nlm::ErrorType::MemoryError,
            nlm::ErrorSeverity::Critical,
            "Memory",
            "allocate",
            "Failed to allocate 1GB of memory"
        );
    } catch (const nlm::Error& e) {
        std::cout << "   Memory error logged:" << std::endl;
        std::cout << "     " << e.what() << std::endl;
        nlm::ErrorHandling::handleError(e);
    }
    
    // Demonstrate performance logging
    std::cout << "\n3. Demonstrating performance logging..." << std::endl;
    nlm::ErrorHandling::trackPerformance(
        std::chrono::milliseconds(1500),
        512 * 1024 * 1024, // 512MB
        "brain_simulation",
        "Brain"
    );
    
    // Demonstrate event logging
    std::cout << "\n4. Demonstrating event logging..." << std::endl;
    nlm::Json::Value data;
    data["simulation_step"] = 100;
    data["neuron_count"] = 1000;
    data["spike_count"] = 5000;
    
    nlm::ErrorHandling::logEvent(
        nlm::EventType::PerformanceMetric,
        "Simulation",
        "brain_engine",
        data
    );
    
    // Demonstrate system health check
    std::cout << "\n5. Demonstrating system health check..." << std::endl;
    bool isHealthy = nlm::ErrorHandling::checkSystemHealth();
    std::cout << "   System health: " << (isHealthy ? "Healthy" : "Unhealthy") << std::endl;
    
    // Demonstrate circuit breaker
    std::cout << "\n6. Demonstrating circuit breaker..." << std::endl;
    bool canExecute = nlm::ErrorHandling::checkCircuitBreaker("memory_allocator");
    std::cout << "   Memory allocator circuit: " << (canExecute ? "Ready" : "Open") << std::endl;
    
    // Record success
    nlm::ErrorHandling::recordCircuitBreakerSuccess("memory_allocator");
    
    // Demonstrate audit logging
    std::cout << "\n7. Demonstrating audit logging..." << stdendl;
    nlm::AuditLogEntry auditEntry;
    auditEntry.userId = "user123";
    auditEntry.sessionId = "session456";
    auditEntry.action = "load_simulation";
    auditEntry.resource = "simulation.json";
    auditEntry.outcome = "success";
    auditEntry.ipAddress = "192.168.1.100";
    auditEntry.userAgent = "NLM-Client/1.0";
    
    nlm::getAdvancedLogger().logAudit(auditEntry);
    
    // Demonstrate graceful degradation
    std::cout << "\n8. Demonstrating graceful degradation..." << std::endl;
    bool degraded = nlm::ErrorHandling::degradeGracefully(
        "Configuration not found, using defaults",
        "Config",
        {{ "mode", "limited" }, { "neuron_count", "100" }}
    );
    std::cout << "   Graceful degradation: " << (degraded ? "Successful" : "Failed") << std::endl;
    
    // Demonstrate log analysis
    std::cout << "\n9. Demonstrating log analysis..." << std::endl;
    std::string analysis = nlm::ErrorHandling::getLogAnalysis("Brain");
    std::cout << "   Log analysis:\n" << analysis << std::endl;
    
    // Demonstrate log export
    std::cout << "\n10. Demonstrating log export..." << std::endl;
    std::string jsonLogs = nlm::ErrorHandling::exportLogs("json");
    std::cout << "   Exported " << jsonLogs.size() << " bytes of logs in JSON format" << std::endl;
    
    // Demonstrate error recovery manager
    std::cout << "\n11. Demonstrating error recovery manager..." << std::endl;
    std::string stats = nlm::ErrorHandling::getErrorStatistics();
    std::cout << "   Error statistics:\n" << stats << std::endl;
    
    // Demonstrate safe execution
    std::cout << "\n12. Demonstrating safe execution..." << std::endl;
    nlm::ErrorHandling::safeExecute(
        "Brain",
        "initialize",
        []() {
            std::cout << "   [Safe execution] Simulating brain initialization..." << std::endl;
            // Simulate some work
            for (int i = 0; i < 1000; ++i) {
                // Do nothing
            }
        }
    );
    
    // Demonstrate component-specific errors
    std::cout << "\n13. Demonstrating component-specific errors..." << std::endl;
    
    try {
        nlm::ErrorHandling::throwBrainError(
            "initialize",
            "Brain initialization failed: insufficient neurons"
        );
    } catch (const nlm::Error& e) {
        std::cout << "   Brain error: " << e.what() << std::endl;
    }
    
    try {
        nlm::ErrorHandling::throwNeuronError(
            "step",
            "Neuron computation failed: invalid membrane potential"
        );
    } catch (const nlm::Error& e) {
        std::cout << "   Neuron error: " << e.what() << std::endl;
    }
    
    try {
        nlm::ErrorHandling::throwMemoryError(
            "allocate",
            "Memory allocation failed: out of memory"
        );
    } catch (const nlm::Error& e) {
        std::cout << "   Memory error: " << e.what() << std::endl;
    }
    
    // Demonstrate error translation
    std::cout << "\n14. Demonstrating error translation..." << std::endl;
    try {
        // Simulate a standard exception
        throw std::runtime_error("Standard C++ exception occurred");
    } catch (const std::exception& e) {
        auto translatedError = nlm::ErrorHandling::translateException(e, "Brain", "compute");
        std::cout << "   Translated error: " << translatedError.what() << std::endl;
    }
    
    // Shutdown error handling system
    std::cout << "\n15. Shutting down error handling system..." << std::endl;
    nlm::ErrorHandling::shutdown();
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
    std::cout << "All error handling and logging features demonstrated successfully!" << std::endl;
}

void demonstrateConfigurationIntegration() {
    std::cout << "\n=== Configuration Integration Demo ===" << std::endl;
    
    // Initialize error handling
    nlm::ErrorHandling::initialize("./logs");
    
    nlm::Config config;
    
    // Demonstrate configuration loading with error handling
    std::cout << "\n1. Loading configuration with error handling..." << std::endl;
    
    bool loadSuccess = config.loadFromFile("config.txt");
    if (!loadSuccess) {
        std::cout << "   Configuration file not found, using defaults..." << std::endl;
        
        // Demonstrate configuration error with context
        nlm::ErrorHandling::throwError(
            nlm::ErrorType::ConfigurationError,
            nlm::ErrorSeverity::Warning,
            "Config",
            "loadFromFile",
            "Configuration file not found: using default configuration"
        );
        
        // Set default configuration
        config.set("neuron_count", 1000);
        config.set("simulation_timestep", 0.001);
        config.set("random_seed", 42);
        
        std::cout << "   Default configuration applied" << std::endl;
    }
    
    // Demonstrate configuration validation
    std::cout << "\n2. Validating configuration..." << std::endl;
    
    if (!config.has("neuron_count")) {
        nlm::ErrorHandling::throwError(
            nlm::ErrorType::ValidationError,
            nlm::ErrorSeverity::Error,
            "Config",
            "validate",
            "Required configuration parameter 'neuron_count' is missing"
        );
    }
    
    if (config.get<int>("neuron_count") <= 0) {
        nlm::ErrorHandling::throwError(
            nlm::ErrorType::ValidationError,
            nlm::ErrorSeverity::Error,
            "Config",
            "validate",
            "Neuron count must be positive"
        );
    }
    
    std::cout << "   Configuration validation passed" << std::endl;
    
    // Demonstrate configuration access with error handling
    std::cout << "\n3. Accessing configuration with error handling..." << std::endl;
    
    auto neuronCount = config.get<int>("neuron_count");
    if (neuronCount.has_value()) {
        std::cout << "   Neuron count: " << neuronCount.value() << std::endl;
        
        // Log performance metric
        nlm::ErrorHandling::trackPerformance(
            std::chrono::milliseconds(100),
            neuronCount.value() * 100,
            "config_access",
            "Config"
        );
    } else {
        nlm::ErrorHandling::throwError(
            nlm::ErrorType::ConfigurationError,
            nlm::ErrorSeverity::Error,
            "Config",
            "get",
            "Failed to retrieve neuron count from configuration"
        );
    }
    
    // Shutdown
    nlm::ErrorHandling::shutdown();
    
    std::cout << "\n=== Configuration Integration Demo Complete ===" << std::endl;
}

void demonstrateBrainIntegration() {
    std::cout << "\n=== Brain Integration Demo ===" << std::endl;
    
    // Initialize error handling
    nlm::ErrorHandling::initialize("./logs");
    
    // Create configuration
    nlm::Config config;
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    config.set("random_seed", 42);
    
    // Create brain with error handling
    std::cout << "\n1. Creating brain with error handling..." << std::endl;
    
    try {
        auto brain = std::make_unique<nlm::Brain>(std::make_shared<nlm::Config>(config));
        
        std::cout << "   Brain object created successfully" << std::endl;
        
        // Demonstrate brain initialization with error handling
        std::cout << "\n2. Initializing brain..." << std::endl;
        
        bool initSuccess = brain->initialize();
        if (!initSuccess) {
            nlm::ErrorHandling::throwError(
                nlm::ErrorType::InitializationError,
                nlm::ErrorSeverity::Critical,
                "Brain",
                "initialize",
                "Brain initialization failed"
            );
        }
        
        std::cout << "   Brain initialized successfully" << std::endl;
        
        // Demonstrate brain operation with error handling
        std::cout << "\n3. Operating brain..." << std::endl;
        
        for (int step = 0; step < 10; ++step) {
            brain->step(static_cast<size_t>(step));
            
            // Log progress
            nlm::Json::Value stepData;
            stepData["step"] = step;
            stepData["spike_count"] = brain->getTotalSpikeCount();
            
            nlm::ErrorHandling::logEvent(
                nlm::EventType::PerformanceMetric,
                "Brain",
                "simulation",
                stepData
            );
            
            // Check for errors
            if (brain->getTotalSpikeCount() > 10000) {
                nlm::ErrorHandling::throwError(
                    nlm::ErrorType::SimulationError,
                    nlm::ErrorSeverity::Error,
                    "Brain",
                    "step",
                    "Excessive spike count detected"
                );
            }
        }
        
        std::cout << "   Brain operation completed successfully" << std::endl;
        
        // Demonstrate brain logging
        std::cout << "\n4. Brain logging..." << std::endl;
        
        brain->logStatus();
        
        std::cout << "   Brain status logged" << std::endl;
        
        // Demonstrate brain error handling
        std::cout << "\n5. Demonstrating brain error handling..." << std::endl;
        
        try {
            brain->receiveSensoryInput(nlm::SensoryInput({0.5f, 0.7f, 0.9f}));
            std::cout << "   Sensory input received successfully" << std::endl;
        } catch (const nlm::Error& e) {
            std::cout << "   Error handling sensory input: " << e.what() << std::endl;
        }
        
    } catch (const nlm::Error& e) {
        std::cout << "   Error creating/initializing brain: " << e.what() << std::endl;
        nlm::ErrorHandling::handleError(e);
    }
    
    // Shutdown
    nlm::ErrorHandling::shutdown();
    
    std::cout << "\n=== Brain Integration Demo Complete ===" << std::endl;
}

int main() {
    // Run demonstrations
    demonstrateErrorHandling();
    demonstrateConfigurationIntegration();
    demonstrateBrainIntegration();
    
    return 0;
}
