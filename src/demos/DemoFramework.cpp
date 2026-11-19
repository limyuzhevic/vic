// DemoFramework.cpp - Implementation of the modular demo system

#include "DemoFramework.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>

namespace nlm {

DemoFramework::DemoFramework() {
    // Initialize available demo modes
    availableModes = {
        {DemoMode::BASIC_CONNECTIVITY, "Basic neural connectivity test"},
        {DemoMode::PLASTICITY_LEARNING, "Plasticity learning experiment"},
        {DemoMode::STDP_VERIFICATION, "STDP spike-timing dependent plasticity test"},
        {DemoMode::INTEGRATION_TEST, "Full integration test with all systems"},
        {DemoMode::CUSTOM, "Custom test configuration"}
    };
}

DemoFramework::~DemoFramework() {
    cleanup();
}

bool DemoFramework::initialize(std::shared_ptr<Config> config) {
    this->config = config;  // Store reference for later use
    if (!validateConfig(config)) {
        NLM_LOG_ERROR("Configuration validation failed");
        return false;
    }
    
    setupEnhancedLogging();
    NLM_LOG_INFO("DemoFramework initialized successfully");
    return true;
}

DemoResult DemoFramework::runDemo(DemoMode mode) {
    auto startTime = std::chrono::high_resolution_clock::now();
    DemoResult result;
    result.success = false;
    result.durationSteps = 0;
    
    try {
        // Initialize brain with current config
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            result.message = "Failed to initialize brain";
            return result;
        }
        
        // Run specific demo based on mode
        switch (mode) {
            case DemoMode::BASIC_CONNECTIVITY:
                result = runBasicConnectivityTest(brain);
                break;
            case DemoMode::PLASTICITY_LEARNING:
                result = runPlasticityLearningTest(brain);
                break;
            case DemoMode::STDP_VERIFICATION:
                result = runSTDPVerificationTest(brain);
                break;
            case DemoMode::INTEGRATION_TEST:
                result = runIntegrationTest(brain);
                break;
            case DemoMode::CUSTOM:
                result = runCustomTest(brain);
                break;
        }
        
    } catch (const std::exception& e) {
        result.message = std::string("Exception in demo: ") + e.what();
        NLM_LOG_ERROR(result.message);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    NLM_LOG_INFO("Demo completed in " + std::to_string(duration.count()) + "ms");
    NLM_LOG_INFO("Result: " + result.message);
    
    return result;
}

void DemoFramework::printHelp() const {
    std::cout << "NLM Demo System - Available Options:" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Demo Modes:" << std::endl;
    for (const auto& pair : availableModes) {
        std::cout << "  --mode " << static_cast<int>(pair.first) << " : " << pair.second << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "Common Options:" << std::endl;
    std::cout << "  --config <file>     : Configuration file" << std::endl;
    std::cout << "  --steps <n>         : Number of simulation steps" << std::endl;
    std::cout << "  --verbose           : Enable verbose output" << std::endl;
    std::cout << "  --help              : Show this help message" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm_demo --mode 0 --steps 1000" << std::endl;
    std::cout << "  ./nlm_demo --mode 1 --config custom.cfg" << std::endl;
    std::cout << std::endl;
}

const std::map<DemoMode, std::string>& DemoFramework::getAvailableModes() const {
    return availableModes;
}

void DemoFramework::setupEnhancedLogging() {
    // Create enhanced logger with progress tracking
    auto logger = std::make_shared<Logger>();
    
    // Console logger with enhanced formatting
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setFormat("[{timestamp}] [{level}] {message}");
    logger->addLogger(consoleLogger);
    
    // Performance logger for timing
    auto perfLogger = std::make_shared<FileLogger>("nlm_perf.log");
    perfLogger->setLevel(LogLevel::Debug);
    logger->addLogger(perfLogger);
    
    Logger::setGlobal(logger);
}

bool DemoFramework::validateConfig(std::shared_ptr<Config> config) {
    // Validate required configuration values
    std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep", 
        "neuron_count",
        "region_count",
        "connection_probability"
    };
    
    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            NLM_LOG_WARNING("Missing required config key: " + key);
            return false;
        }
    }
    
    // Validate value ranges
    auto neuronCount = config->getOr<int64_t>("neuron_count", 0);
    if (neuronCount <= 0) {
        NLM_LOG_ERROR("neuron_count must be positive");
        return false;
    }
    
    auto timestep = config->getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0) {
        NLM_LOG_ERROR("simulation_timestep must be positive");
        return false;
    }
    
    auto connProb = config->getOr<float>("connection_probability", 0.0f);
    if (connProb < 0.0f || connProb > 1.0f) {
        NLM_LOG_ERROR("connection_probability must be between 0.0 and 1.0");
        return false;
    }
    
    return true;
}

void DemoFramework::reportProgress(const std::string& message, size_t current, size_t total) {
    if (total == 0) return;
    
    float progress = (static_cast<float>(current) / static_cast<float>(total)) * 100.0f;
    
    // Clear line and show progress
    std::cout << "\r[" << std::string(static_cast<size_t>(progress / 10), '=')
              << ">" << std::string(10 - static_cast<size_t>(progress / 10), ' ')
              << std::fixed << std::setprecision(1) << progress << "%] "
              << message << std::flush;
    
    if (current >= total) {
        std::cout << std::endl;
    }
}

void DemoFramework::cleanup() {
    NLM_LOG_INFO("Cleaning up demo resources");
    // Reset global logger
    Logger::setGlobal(nullptr);
}