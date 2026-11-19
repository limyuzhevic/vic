// NLM Demo System - Modular Demo Framework
// This file provides a modular demo system for NLM with different modes

#pragma once

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <map>

namespace nlm {

// Demo modes
enum class DemoMode {
    BASIC_CONNECTIVITY,
    PLASTICITY_LEARNING,
    STDP_VERIFICATION,
    INTEGRATION_TEST,
    CUSTOM
};

// Demo result structure
struct DemoResult {
    bool success;
    std::string message;
    size_t durationSteps;
    std::map<std::string, std::string> metrics;
};

// Enhanced demo framework
class DemoFramework {
public:
    DemoFramework();
    ~DemoFramework();
    
    // Initialize demo with configuration
    bool initialize(std::shared_ptr<Config> config);
    
    // Run specific demo
    DemoResult runDemo(DemoMode mode);
    
    // Print demo help and options
    void printHelp() const;
    
    // Get available demo modes
    const std::map<DemoMode, std::string>& getAvailableModes() const;
    
    // Enhanced logging setup
    void setupEnhancedLogging();
    
    // Configuration validation
    bool validateConfig(std::shared_ptr<Config> config);
    
    // Progress reporting
    void reportProgress(const std::string& message, size_t current, size_t total);
    
    // Resource management
    void cleanup();
    
    // Test demo implementations
    DemoResult runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    DemoResult runPlasticityLearningTest(std::shared_ptr<Brain> brain);
    DemoResult runSTDPVerificationTest(std::shared_ptr<Brain> brain);
    DemoResult runIntegrationTest(std::shared_ptr<Brain> brain);
    DemoResult runCustomTest(std::shared_ptr<Brain> brain);

private:
    std::shared_ptr<Config> config;
    std::map<DemoMode, std::string> availableModes;
};