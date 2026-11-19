// Improved main demo system with better modularity
// This file provides enhanced main functionality with better separation of concerns

#pragma once

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <map>
#include <functional>

namespace nlm {

// Demo mode enumeration
enum class DemoMode {
    BASIC_CONNECTIVITY = 0,
    PLASTICITY_LEARNING = 1,
    STDP_VERIFICATION = 2,
    INTEGRATION_TEST = 3,
    CUSTOM = 4
};

// Demo result structure
struct DemoResult {
    bool success;
    std::string message;
    size_t durationSteps;
    std::map<std::string, std::string> metrics;
    double executionTimeMs;
};

// Enhanced main demo class
class NLMMainDemo {
public:
    NLMMainDemo();
    ~NLMMainDemo();
    
    // Initialize demo with configuration
    bool initialize(int argc, char** argv);
    
    // Run the demo
    int run();
    
    // Parse command line arguments
    bool parseArguments(int argc, char** argv);
    
    // Print help message
    void printHelp() const;
    
    // Print banner
    void printBanner() const;
    
    // Log configuration summary
    void logConfigurationSummary() const;
    
    // Run specific demo mode
    DemoResult runDemo(DemoMode mode);
    
    // Enhanced logging setup
    void setupLogging();
    
    // Progress reporting
    void reportProgress(const std::string& message, size_t current, size_t total);
    
    // Configuration management
    std::shared_ptr<Config> getConfig() const;
    
    // Brain initialization
    bool initializeBrain();
    
    // Cleanup resources
    void cleanup();
    
    // Get execution time
    double getExecutionTime() const;

// Helper methods
    bool parseDemoMode(const std::string& arg);
    bool parseSteps(const std::string& arg);
    bool parseVerbose(const std::string& arg);
    bool parseConfigFile(const std::string& arg);
    void setupDefaultConfiguration();
    void applyCommandLineOverrides();
    DemoResult runBasicConnectivityDemo();
    DemoResult runPlasticityLearningDemo();
    DemoResult runStdpVerificationDemo();
    DemoResult runIntegrationDemo();
    DemoResult runCustomDemo();
    void setConfigKey(const std::string& key, const std::string& value);
};
    
    // Helper methods
    bool parseDemoMode(const std::string& arg);
    bool parseSteps(const std::string& arg);
    bool parseVerbose(const std::string& arg);
    bool parseConfigFile(const std::string& arg);
    void setupDefaultConfiguration();
    void applyCommandLineOverrides();
    DemoResult runBasicConnectivityDemo();
    DemoResult runPlasticityLearningDemo();
    DemoResult runStdpVerificationDemo();
    DemoResult runIntegrationDemo();
    DemoResult runCustomDemo();
};