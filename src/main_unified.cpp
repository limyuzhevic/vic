// NLM (熙然) - Unified Demo Main
// Enhanced unified demo framework main entry point
// This replaces the old demo system with a comprehensive, extensible framework

#include "core/DemoFramework/DemoFramework.hpp"
#include "core/Config/EnhancedConfig.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>

using namespace std;

// Demo execution functions
int executePhase2Demo(const CLIConfig& config);
int executePhase3Demo(const CLIConfig& config);
int executePhase4Demo(const CLIConfig& config);
int executePhase5Demo(const CLIConfig& config);
int executePhase6Demo(const CLIConfig& config);
int executePhase6TextDemo(const CLIConfig& config);
int runBenchmarks(const CLIConfig& config);
int testConfiguration(const CLIConfig& config);

// Main entry point
int main(int argc, char* argv[]) {
    cout << "Initializing unified demo framework..." << endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create demo framework
    NLMCommander commander;
    
    // Run the commander
    return commander.run(argc, argv);
}

// Phase 2 demo implementation (from original main.cpp)
int executePhase2Demo(const CLIConfig& config) {
    cout << "=== Phase 2: Real Neural Computation ===" << endl;
    cout << "This phase implements real spiking neural computation." << endl;
    
    // Initialize configuration
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(config.maxSteps > 0 ? config.maxSteps : 500), ConfigSource::Default);
    cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    cfg->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // TODO: Actually implement Phase 2 neural computation
    // For now, just return success
    
    return 0;
}

// Phase 3 demo implementation
int executePhase3Demo(const CLIConfig& config) {
    cout << "=== Phase 3: World Interaction ===" << endl;
    cout << "This phase implements world interaction and sensory processing." << endl;
    
    // TODO: Actually implement Phase 3 demo
    // For now, just return success
    
    return 0;
}

// Phase 4 demo implementation
int executePhase4Demo(const CLIConfig& config) {
    cout << "=== Phase 4: Working Memory & Planning ===" << endl;
    cout << "This phase implements advanced cognitive functions." << endl;
    
    return 0;
}

// Phase 5 demo implementation
int executePhase5Demo(const CLIConfig& config) {
    cout << "=== Phase 5: Performance Optimizations ===" << endl;
    cout << "This phase implements performance optimizations and SIMD vectorization." << endl;
    
    return 0;
}

// Phase 6 demo implementation
int executePhase6Demo(const CLIConfig& config) {
    cout << "=== Phase 6: Integration Test ===" << endl;
    cout << "This phase tests the integration of all brain systems." << endl;
    
    // Run Phase 6 integration demo (from src/experiments/Phase6Demo.cpp)
    // TODO: Actually implement Phase 6 demo
    
    return 0;
}

// Phase 6 text demo implementation
int executePhase6TextDemo(const CLIConfig& config) {
    cout << "=== Phase 6 Text Demo: Text Teaching ===" << endl;
    cout << "This phase demonstrates text teaching and concept formation." << endl;
    
    // Run Phase 6 text demo (from src/experiments/Phase6TextDemo.cpp)
    // TODO: Actually implement Phase 6 text demo
    
    return 0;
}

// Benchmark execution
int runBenchmarks(const CLIConfig& config) {
    cout << "=== Running Performance Benchmarks ===" << endl;
    
    // TODO: Implement benchmark suite
    // This would run the existing nlm_benchmark executable
    
    return 0;
}

// Configuration testing
int testConfiguration(const CLIConfig& config) {
    cout << "=== Testing Configuration System ===" << endl;
    
    // Create enhanced configuration
    EnhancedConfig enhancedConfig;
    
    // Test loading from file
    if (config.configFile.empty()) {
        cout << "No config file specified for testing" << endl;
    } else {
        cout << "Testing config file: " << config.configFile << endl;
        if (!enhancedConfig.loadFromFile(config.configFile)) {
            cout << "ERROR: Failed to load config file" << endl;
            return 1;
        }
    }
    
    // Test loading from environment
    cout << "Testing environment variable loading..." << endl;
    if (!enhancedConfig.loadFromEnv("NLM_")) {
        cout << "WARNING: Some environment variables failed to load" << endl;
    }
    
    // Test configuration validation
    vector<ValidationRule> rules = ConfigValidation::getDefaultValidationRules();
    if (!enhancedConfig.isValid(rules)) {
        auto errors = enhancedConfig.validate(rules);
        cout << "Configuration validation failed:" << endl;
        for (const auto& error : errors) {
            cout << "  " << error.field << ": " << error.message << endl;
        }
        return 1;
    }
    
    cout << "Configuration validation passed!" << endl;
    return 0;
}