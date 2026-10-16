#include "NLMApplication.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include "core/TestFramework/TestFramework.hpp"
#include "core/AdvancedCommands/AdvancedCommands.hpp"
#include "core/PerformanceOptimization/PerformanceOptimization.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

using namespace nlm;

NLMApplication::NLMApplication(const CommandLineOptions::Options& options)
    : options_(options), config_(std::make_shared<Config>()) {
}

bool NLMApplication::initialize() {
    printBanner();
    
    setupLogging();
    setupConfiguration();
    setupBrain();
    
    stats_.startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    
    NLM_LOG_INFO("Application initialized successfully");
    return true;
}

void NLMApplication::setupLogging() {
    logger_ = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(options_.verbose ? LogLevel::Debug : LogLevel::Info);
    logger_->addLogger(consoleLogger);
    Logger::setGlobal(logger_);
    
    NLM_LOG_INFO("Logging initialized (level: " + std::string(options_.verbose ? "DEBUG" : "INFO") + ")");
}

void NLMApplication::setupConfiguration() {
    ErrorContext context("Configuration Setup");
    
    NLM_LOG_INFO("Loading configuration from: " + options_.configFile);
    
    if (!config_->loadFromFile(options_.configFile)) {
        NLM_LOG_INFO("Using default configuration (file not found: " + options_.configFile + ")");
    }
    
    // Validate configuration using error handling
    try {
        ErrorManager::ConfigValidator::validate(*config_);
    } catch (const NLMConfigurationError& e) {
        NLM_LOG_ERROR("Configuration validation failed: " + std::string(e.what()));
        throw;
    }
    
    // Override with command line options
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", options_.timestep, ConfigSource::Default);
    config_->set("neuron_count", static_cast<int64_t>(options_.neuronCount), ConfigSource::Default);
    config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config_->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config_->getOr<double>("simulation_timestep", options_.timestep)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config_->getOr<int64_t>("neuron_count", options_.neuronCount)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config_->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config_->getOr<float>("connection_probability", 0.15f)));
}

void NLMApplication::setupBrain() {
    ErrorContext context("Brain Setup");
    
    NLM_LOG_INFO("Initializing NLM Brain...");
    brain_ = std::make_shared<Brain>(config_);
    
    if (!brain_->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        throw NLMPermissionError("Brain initialization failed");
    }
    
    brain_->logStatus();
}

void NLMApplication::run() {
    switch (options_.mode) {
        case CommandLineOptions::Mode::SIMULATION:
            runSimulationMode();
            break;
        case CommandLineOptions::Mode::CONNECTIVITY_TEST:
            runConnectivityTest();
            break;
        case CommandLineOptions::Mode::PLASTICITY_TEST:
            runPlasticityTest();
            break;
        case CommandLineOptions::Mode::STDP_TEST:
            runSTDPTest();
            break;
        case CommandLineOptions::Mode::BENCHMARK:
            runBenchmark();
            break;
        case CommandLineOptions::Mode::CONFIG_EXPORT:
            exportConfiguration();
            break;
        case CommandLineOptions::Mode::HELP:
            CommandLineOptions::printHelp();
            break;
    }
}

void NLMApplication::cleanup() {
    ErrorContext context("Application Cleanup");
    
    stats_.endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    
    if (options_.saveOnExit && brain_) {
        try {
            NLM_LOG_INFO("Saving brain state to: " + options_.outputFile);
            if (brain_->save(options_.outputFile)) {
                NLM_LOG_INFO("Brain state saved successfully");
            } else {
                NLM_LOG_ERROR("Failed to save brain state");
            }
        } catch (const NLMError& e) {
            NLM_LOG_ERROR("Error during cleanup save: " + std::string(e.what()));
        }
    }
    
    NLM_LOG_INFO("Application cleaned up");
    printPerformanceStats();
}

void NLMApplication::runSimulationMode() {
    ErrorContext context("Simulation Mode");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Simulation Mode ===");
    NLM_LOG_INFO("Running standard neural simulation with " + std::to_string(options_.neuronCount) + " neurons");
    NLM_LOG_INFO("Duration: " + std::to_string(options_.duration) + " steps");
    NLM_LOG_INFO("Timestep: " + std::to_string(options_.timestep) + "s");
    
    runStandardTests();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Simulation Complete ===");
    brain_->logStatus();
}

void NLMApplication::runConnectivityTest() {
    ErrorContext context("Connectivity Test");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Basic Neural Connectivity Test ===");
    
    runBasicConnectivityTest(brain_);
}

void NLMApplication::runPlasticityTest() {
    ErrorContext context("Plasticity Test");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Plasticity Learning Experiment ===");
    
    // Reset brain for clean experiment
    brain_->reset();
    brain_->initialize();
    
    runPlasticityExperiment(brain_);
}

void NLMApplication::runSTDPTest() {
    ErrorContext context("STDP Test");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== STDP Verification Test ===");
    
    // Reset brain for clean test
    brain_->reset();
    brain_->initialize();
    
    runStdpVerification(brain_);
}

void NLMApplication::runBenchmark() {
    ErrorContext context("Benchmark");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Benchmark Mode ===");
    NLM_LOG_INFO("Running performance benchmarks...");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Run multiple test scenarios
        brain_->reset();
        brain_->initialize();
        
        // Test 1: Basic connectivity
        runBasicConnectivityTest(brain_);
        
        // Test 2: Plasticity
        runPlasticityExperiment(brain_);
        
        // Test 3: STDP verification
        runStdpVerification(brain_);
    } catch (const NLMError& e) {
        NLM_LOG_ERROR("Benchmark failed: " + std::string(e.what()));
        throw;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Benchmark Results ===");
    NLM_LOG_INFO("Total execution time: " + std::to_string(duration.count()) + " ms");
    NLM_LOG_INFO("Average step time: " + std::to_string(duration.count() / (options_.duration * 10.0)) + " ms");
}

void NLMApplication::exportConfiguration() {
    ErrorContext context("Configuration Export");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Configuration Export ===");
    
    // Validate output file path using error handling
    try {
        getErrorManager().validateFilePath(options_.outputFile);
    } catch (const NLMValidationError& e) {
        NLM_LOG_ERROR("Invalid output file path: " + std::string(e.what()));
        throw;
    }
    
    // Create config directory if it doesn't exist
    std::ofstream configFile(options_.outputFile, std::ios::out | std::ios::trunc);
    if (!configFile.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + options_.outputFile);
        throw NLMPermissionError("Cannot write to configuration file");
    }
    
    // Export basic configuration
    configFile << "# NLM Configuration Export" << std::endl;
    configFile << "# Generated by NLM Application" << std::endl;
    configFile << std::endl;
    configFile << "[GENERAL]" << std::endl;
    configFile << "random_seed = 42" << std::endl;
    configFile << "simulation_timestep = " << options_.timestep << std::endl;
    configFile << "neuron_count = " << options_.neuronCount << std::endl;
    configFile << "region_count = 1" << std::endl;
    configFile << std::endl;
    configFile << "[PLASTICITY]" << std::endl;
    configFile << "stdp_ltp_weight = 0.02" << std::endl;
    configFile << "stdp_ltd_weight = 0.015" << std::endl;
    configFile << "stdp_tau = 20.0" << std::endl;
    configFile << std::endl;
    configFile << "[STRUCTURAL]" << std::endl;
    configFile << "synaptogenesis_rate = 0.0001" << std::endl;
    configFile << "pruning_rate = 0.00001" << std::endl;
    configFile << std::endl;
    
    configFile.close();
    
    NLM_LOG_INFO("Configuration exported to: " + options_.outputFile);
}

void NLMApplication::runStandardTests() {
    ErrorContext context("Standard Tests");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running Standard Test Suite ===");
    
    try {
        if (options_.testMode == "all" || options_.testMode == "connectivity") {
            runBasicConnectivityTest(brain_);
        }
        
        if (options_.testMode == "all" || options_.testMode == "plasticity") {
            runPlasticityExperiment(brain_);
        }
        
        if (options_.testMode == "all" || options_.testMode == "stdp") {
            runStdpVerification(brain_);
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test Suite Complete ===");
    } catch (const NLMError& e) {
        NLM_LOG_ERROR("Test suite failed: " + std::string(e.what()));
        throw;
    }
}

void NLMApplication::printPerformanceStats() {
    ErrorContext context("Performance Stats");
    
    double elapsed = stats_.endTime - stats_.startTime;
    
    if (elapsed <= 0.0) {
        NLM_LOG_WARNING("Invalid elapsed time for performance stats");
        return;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Performance Statistics ===");
    NLM_LOG_INFO("Total simulation time: " + std::to_string(elapsed) + " seconds");
    NLM_LOG_INFO("Total steps executed: " + std::to_string(stats_.totalSteps));
    NLM_LOG_INFO("Steps per second: " + std::to_string(stats_.totalSteps / elapsed));
    NLM_LOG_INFO("Total spikes: " + std::to_string(stats_.totalSpikes));
    NLM_LOG_INFO("Average spikes per step: " + std::to_string(static_cast<double>(stats_.totalSpikes) / stats_.totalSteps));
}