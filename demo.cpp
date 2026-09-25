// Enhanced Error Handling and Diagnostics Demo for NLM
// Demonstrates the comprehensive error handling and diagnostics system

#include "core/EnhancedErrorHandling.hpp"
#include "core/Diagnostics.hpp"
#include "core/PerformanceAnalysis.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <memory>

using namespace nlm;

void demoEnhancedErrorHandling() {
    std::cout << "=== Enhanced Error Handling Demo ===" << std::endl;
    
    // Clear all existing errors
    getEnhancedErrorManager().clearErrors();
    
    // Demo 1: Basic error logging with different severity levels
    std::cout << "\n1. Demonstrating error severity levels:" << std::endl;
    
    ErrorContext ctx1("ConfigParser", "loadFromFile", "config.cfg", 42, "loadConfig");
    NLM_LOG_DEBUG("Debug message: Entering configuration parser", ctx1);
    NLM_LOG_INFO("Info message: Configuration file opened successfully", ctx1);
    NLM_LOG_WARNING("Warning message: Deprecated configuration option detected", ctx1);
    NLM_LOG_ERROR("Error message: Failed to parse configuration key 'timeout'", ctx1);
    NLM_LOG_CRITICAL("Critical message: System configuration corrupted", ctx1);
    NLM_LOG_FATAL("Fatal message: Unable to continue without critical configuration", ctx1);
    
    // Demo 2: Error categorization
    std::cout << "\n2. Demonstrating error categorization:" << std::endl;
    
    ErrorContext ctx2("Brain", "initializeNeurons", "brain.cpp", 123, "Brain::initialize");
    NLM_ERROR(NLMErrors::SystemNotInitialized, "Neural system components not properly initialized", ctx2);
    
    ErrorContext ctx3("Memory", "allocateNeurons", "memory.cpp", 456, "Memory::allocate");
    NLM_ERROR(NLMErrors::MemoryAllocation, "Failed to allocate memory for neural population", ctx3);
    
    ErrorContext ctx4("Config", "parseFile", "config.json", 789, "Config::parseFile");
    NLM_ERROR(NLMErrors::ConfigParsing, "Invalid JSON syntax in configuration file", ctx4);
    
    // Demo 3: Error recovery scenarios
    std::cout << "\n3. Demonstrating error recovery mechanisms:" << std::endl;
    
    // Add a recovery callback that attempts to recover from certain errors
    getEnhancedErrorManager().addRecoveryCallback(
        [](const EnhancedErrorInfo& error) -> bool {
            std::cout << "    Recovery callback attempting to handle error: " 
                     << errorTypeToString(error.type) << std::endl;
            
            if (error.type == NLMErrors::SystemNotInitialized) {
                std::cout << "    -> Attempting system reinitialization..." << std::endl;
                return true; // Recovery successful
            }
            return false; // Cannot recover
        }
    );
    
    // Try to trigger recovery by adding an error that can be recovered
    ErrorContext ctx5("System", "reinitialize", "system.cpp", 999, "System::reinitialize");
    NLM_ERROR(NLMErrors::SystemNotInitialized, "System not initialized, attempting recovery", ctx5);
    NLM_ERROR(NLMErrors::SystemNotInitialized, "System not initialized, attempting recovery again", ctx5);
    
    // Demo 4: Error statistics and analysis
    std::cout << "\n4. Demonstrating error statistics and analysis:" << std::endl;
    
    auto errors = getEnhancedErrorManager().getErrors();
    std::cout << "   Total errors collected: " << getEnhancedErrorManager().getErrorCount() << std::endl;
    std::cout << "   Error rate: " << getEnhancedErrorManager().getErrorRatePerMinute() << " errors/minute" << std::endl;
    
    // Show error distribution by type
    std::cout << "   Errors by type:" << std::endl;
    for (int i = 0; i < 13; ++i) {
        NLMErrors type = static_cast<NLMErrors>(i);
        auto count = getEnhancedErrorManager().getErrorCount(type);
        if (count > 0) {
            std::cout << "     " << errorTypeToString(type) << ": " << count << std::endl;
        }
    }
    
    // Demo 5: Error reporting
    std::cout << "\n5. Demonstrating comprehensive error reporting:" << std::endl;
    std::cout << getEnhancedErrorManager().generateErrorReport();
    
    // Clear errors for next demo
    getEnhancedErrorManager().clearErrors();
}

void demoDiagnosticsSystem() {
    std::cout << "\n\n=== Diagnostics System Demo ===" << std::endl;
    
    // Start system monitor
    getSystemMonitor().initialize();
    
    // Demo 1: Health checking
    std::cout << "\n1. Demonstrating health checking system:" << std::endl;
    
    // Add a simple health check
    auto healthCheck = []() -> bool {
        return true; // Always healthy for demo
    };
    
    getSystemMonitor().addHealthCheck(healthCheck, "Basic Health Check");
    
    // Perform health check
    HealthStatus status = getSystemMonitor().performHealthCheck();
    std::cout << "   Health Status: " << status.getHealthSummary() << std::endl;
    
    if (!status.warnings.empty()) {
        std::cout << "   Warnings:" << std::endl;
        for (const auto& warning : status.warnings) {
            std::cout << "     - " << warning << std::endl;
        }
    }
    
    if (!status.issues.empty()) {
        std::cout << "   Issues:" << std::endl;
        for (const auto& issue : status.issues) {
            std::cout << "     - " << issue << std::endl;
        }
    }
    
    // Demo 2: Component monitoring
    std::cout << "\n2. Demonstrating component monitoring:" << std::endl;
    
    ComponentHealth configHealth("Config", true, "Configuration system operational");
    configHealth.metadata["lastLoad"] = "2025-01-25 10:30:00";
    configHealth.metadata["filesLoaded"] = "5";
    getSystemMonitor().updateComponentHealth("Config", configHealth);
    
    ComponentHealth brainHealth("Brain", true, "Neural computation system operational");
    brainHealth.metadata["neurons"] = "1000";
    brainHealth.metadata["synapses"] = "10000";
    getSystemMonitor().updateComponentHealth("Brain", brainHealth);
    
    ComponentHealth memoryHealth("Memory", false, "Memory system degraded");
    memoryHealth.metadata["freeMemory"] = "512MB";
    memoryHealth.metadata["usage"] = "85%";
    getSystemMonitor().updateComponentHealth("Memory", memoryHealth);
    
    std::cout << "   Component Health:" << std::endl;
    for (const auto& pair : getSystemMonitor().getDiagnosticInfo().componentHealth) {
        const auto& health = pair.second;
        std::cout << "     - " << health.name << ": " 
                 << (health.isOperational ? "✓ OPERATIONAL" : "✗ DEGRADED")
                 << " (" << health.statusMessage << ")" << std::endl;
    }
    
    // Demo 3: Performance metrics
    std::cout << "\n3. Demonstrating performance metrics collection:" << std::endl;
    
    // Simulate some performance metrics
    PerformanceMetrics metrics;
    metrics.cpuUsage = 45.2;
    metrics.memoryUsage = 2048.0;
    metrics.memoryPercent = 68.5;
    metrics.diskUsage = 512.0;
    metrics.diskPercent = 23.1;
    metrics.activeThreads = 8;
    metrics.totalThreads = 12;
    metrics.garbageCollections = 42;
    
    getSystemMonitor().recordPerformanceMetrics(metrics);
    
    std::cout << "   Performance Metrics: " << metrics.getFormattedMetrics() << std::endl;
    
    // Show performance statistics
    auto history = getSystemMonitor().getPerformanceHistory(10);
    if (!history.empty()) {
        std::cout << "   Average CPU Usage: " << getSystemMonitor().getAverageCPUUsage() << "%" << std::endl;
        std::cout << "   Average Memory Usage: " << getSystemMonitor().getAverageMemoryUsage() << "MB" << std::endl;
    }
    
    // Demo 4: Diagnostic information
    std::cout << "\n4. Demonstrating diagnostic information:" << std::endl;
    DiagnosticInfo diag = getSystemMonitor().getDiagnosticInfo();
    std::cout << "   Diagnostic Summary:" << std::endl;
    std::cout << diag.getDiagnosticSummary();
    
    // Shutdown system monitor
    getSystemMonitor().shutdown();
}

void demoPerformanceAnalysis() {
    std::cout << "\n\n=== Performance Analysis Demo ===" << std::endl;
    
    // Demo 1: Basic profiling
    std::cout << "\n1. Demonstrating basic profiling:" << std::endl;
    
    auto [result, profile1] = getPerformanceProfiler().profileFunction<int>([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return 42;
    }, "Addition Function");
    
    std::cout << "   Profile Result: " << result << std::endl;
    std::cout << "   Profile Details:" << std::endl;
    std::cout << "     " << profile1.getFormattedProfile();
    
    // Demo 2: Scoped profiling
    std::cout << "\n2. Demonstrating scoped profiling:" << std::endl;
    
    // Create a scoped profile manually
    ProfileInfo profile("ComplexCalculation");
    profile.start();
    
    // Simulate some work
    for (int i = 0; i < 1000; ++i) {
        double x = std::sqrt(static_cast<double>(i));
        double y = x * x;
        if (i % 100 == 0) {
            getPerformanceProfiler().sampleCurrentProfile();
        }
    }
    
    profile.end();
    profile.cpuTime = 0.5;  // Simulate CPU time
    profile.memoryUsed = 1024 * 1024;  // 1MB
    profile.peakMemory = 2 * 1024 * 1024;  // 2MB
    profile.allocations = 50;
    
    std::cout << "   Complex Calculation Profile:" << std::endl;
    std::cout << "     " << profile.getFormattedProfile();
    
    // Demo 3: Performance analysis
    std::cout << "\n3. Demonstrating performance analysis:" << std::endl;
    
    PerformanceAnalysis analysis = getPerformanceProfiler().analyzeProfile(profile);
    std::cout << "   Performance Analysis:" << std::endl;
    std::cout << "     " << analysis.getFormattedAnalysis();
    
    // Demo 4: Performance reporting
    std::cout << "\n4. Demonstrating performance reporting:" << std::endl;
    std::cout << getPerformanceReport();
}

void demoErrorValidation() {
    std::cout << "\n\n=== Error Validation Demo ===" << std::endl;
    
    // Clear errors
    getEnhancedErrorManager().clearErrors();
    
    // Demo 1: Input validation
    std::cout << "\n1. Demonstrating input validation:" << std::endl;
    
    int validValue = 42;
    int invalidValue = -1;
    
    // Valid value passes
    std::cout << "   Testing valid value " << validValue << ": ";
    try {
        NLM_VALIDATE(validValue, validValue >= 0, NLMErrors::InvalidParameter, 
                    "Value must be non-negative");
        std::cout << "✓ PASSED" << std::endl;
    } catch (const NLMErrorManager::Exception& e) {
        std::cout << "✗ FAILED: " << e.what() << std::endl;
    }
    
    // Invalid value throws exception
    std::cout << "   Testing invalid value " << invalidValue << ": ";
    try {
        NLM_VALIDATE(invalidValue, invalidValue >= 0, NLMErrors::InvalidParameter,
                    "Value must be non-negative");
        std::cout << "✗ UNEXPECTED PASS" << std::endl;
    } catch (const NLMErrorManager::Exception& e) {
        std::cout << "✓ EXPECTED FAIL: " << e.getFullMessage() << std::endl;
    }
    
    // Demo 2: Parameter validation with context
    std::cout << "\n2. Demonstrating parameter validation with context:" << std::endl;
    
    double temperature = -10.0;
    ErrorContext tempContext("Sensor", "readTemperature", "sensor.cpp", 123, "readTemperature");
    tempContext.metadata["sensor_id"] = "temp_001";
    tempContext.metadata["unit"] = "Celsius";
    
    try {
        NLM_VALIDATE(temperature, temperature >= -50.0 && temperature <= 150.0,
                    NLMErrors::InvalidParameter,
                    "Temperature out of valid range for sensor " + tempContext.metadata["sensor_id"]);
        std::cout << "   Temperature " << temperature << " is valid" << std::endl;
    } catch (const NLMErrorManager::Exception& e) {
        std::cout << "   Temperature " << temperature << " is invalid: " << e.getFullMessage() << std::endl;
    }
    
    // Demo 3: Configuration validation
    std::cout << "\n3. Demonstrating configuration validation:" << std::endl;
    
    Config config;
    config.set("neuron_count", 1000);
    config.set("region_count", 4);
    config.set("connection_probability", 0.1);
    
    // Validate configuration
    int neuronCount = config.get<int>("neuron_count").value_or(0);
    int regionCount = config.get<int>("region_count").value_or(0);
    double connectionProb = config.get<double>("connection_probability").value_or(0.0);
    
    NLM_VALIDATE(neuronCount > 0, neuronCount > 0, NLMErrors::InvalidParameter,
                "Neuron count must be positive");
    NLM_VALIDATE(regionCount > 0, regionCount > 0, NLMErrors::InvalidParameter,
                "Region count must be positive");
    NLM_VALIDATE(connectionProb >= 0.0 && connectionProb <= 1.0, 
                connectionProb >= 0.0 && connectionProb <= 1.0, NLMErrors::InvalidParameter,
                "Connection probability must be between 0 and 1");
    
    std::cout << "   Configuration validation passed:" << std::endl;
    std::cout << "     Neuron count: " << neuronCount << std::endl;
    std::cout << "     Region count: " << regionCount << std::endl;
    std::cout << "     Connection probability: " << connectionProb << std::endl;
    
    // Demo 4: Error statistics from validation
    std::cout << "\n4. Error statistics from validation tests:" << std::endl;
    std::cout << "   Total errors: " << getEnhancedErrorManager().getErrorCount() << std::endl;
    std::cout << "   Errors by severity:" << std::endl;
    for (int i = 0; i < 6; ++i) {
        ErrorSeverity severity = static_cast<ErrorSeverity>(i);
        auto count = getEnhancedErrorManager().getErrorCount(severity);
        if (count > 0) {
            std::cout << "     " << errorSeverityToString(severity) << ": " << count << std::endl;
        }
    }
}

int main() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Error Handling & Diagnostics Demo                ║
    ║                                                               ║
    ║     Demonstrating comprehensive error handling and             ║
    ║     diagnostics capabilities                                  ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    try {
        demoEnhancedErrorHandling();
        demoDiagnosticsSystem();
        demoPerformanceAnalysis();
        demoErrorValidation();
        
        std::cout << "\n\n" << std::string(80, '=') << std::endl;
        std::cout << "✓ DEMONSTRATION COMPLETE" << std::endl;
        std::cout << "✓ All enhanced error handling features demonstrated successfully" << std::endl;
        std::cout << "✓ Comprehensive error recovery and fallback mechanisms working" << std::endl;
        std::cout << "✓ Runtime diagnostics and monitoring operational" << std::endl;
        std::cout << "✓ Performance analysis and profiling functional" << std::endl;
        std::cout << "✓ Input validation and error recovery integrated" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ DEMONSTRATION FAILED: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}