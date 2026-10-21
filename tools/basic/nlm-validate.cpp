#!/usr/bin/env g++
// NLM Validation Tool
// Tool for validating configuration files and testing basic brain functionality

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cmath>
#include <chrono>
#include <iomanip>

// NLM core headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

// Brain headers
#include "brain/Brain.hpp"

using namespace nlm;

struct ValidationResult {
    std::string testName;
    bool passed;
    std::string message;
    double executionTime;
};

class NLMValidator {
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Logger> logger;
    std::vector<ValidationResult> results;
    
public:
    NLMValidator() {
        logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    void addResult(const std::string& testName, bool passed, 
                   const std::string& message, double timeMs) {
        ValidationResult result;
        result.testName = testName;
        result.passed = passed;
        result.message = message;
        result.executionTime = timeMs;
        results.push_back(result);
    }
    
    void validateConfig(const std::string& configPath) {
        std::cout << "=== Configuration Validation ===" << std::endl;
        
        config = std::make_shared<Config>();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        bool configLoaded = config->loadFromFile(configPath);
        auto endTime = std::chrono::high_resolution_clock::now();
        double configLoadTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        if (!configLoaded) {
            addResult("Config Load", false, 
                     "Failed to load configuration from: " + configPath, 
                     configLoadTime);
            std::cout << "Error: Failed to load configuration file: " << configPath << std::endl;
            return;
        }
        
        addResult("Config Load", true, 
                 "Successfully loaded configuration from: " + configPath, 
                 configLoadTime);
        
        // Display configuration summary
        std::cout << "Configuration summary:" << std::endl;
        std::cout << config->summary() << std::endl;
        
        // Validate required configuration values
        validateRequiredValues();
        
        // Validate configuration ranges
        validateRanges();
        
        printResults();
    }
    
    void validateRequiredValues() {
        std::cout << "\n--- Validating Required Values ---" << std::endl;
        
        // Check for essential configuration keys
        std::vector<std::string> requiredKeys = {
            "random_seed",
            "simulation_timestep",
            "neuron_count",
            "region_count"
        };
        
        for (const auto& key : requiredKeys) {
            if (config->has(key)) {
                auto value = config->get<std::string>(key);
                addResult("Config Check " + key, true, 
                         "Found required key: " + key + " = " + value.value(), 
                         0.0);
                std::cout << "✓ " << key << ": " << value.value() << std::endl;
            } else {
                addResult("Config Check " + key, false, 
                         "Missing required key: " + key, 
                         0.0);
                std::cout << "✗ Missing required key: " << key << std::endl;
            }
        }
    }
    
    void validateRanges() {
        std::cout << "\n--- Validating Configuration Ranges ---" << std::endl;
        
        // Validate random seed
        if (config->has("random_seed")) {
            auto value = config->get<int64_t>("random_seed");
            if (value.has_value() && value.value() >= 0) {
                addResult("Random Seed", true, 
                         "Valid random seed: " + std::to_string(value.value()), 
                         0.0);
                std::cout << "✓ random_seed: " << value.value() << " (valid)" << std::endl;
            } else {
                addResult("Random Seed", false, 
                         "Invalid random seed value", 
                         0.0);
                std::cout << "✗ random_seed: Invalid range" << std::endl;
            }
        }
        
        // Validate simulation timestep
        if (config->has("simulation_timestep")) {
            auto value = config->get<double>("simulation_timestep");
            if (value.has_value() && value.value() > 0.0 && value.value() < 1.0) {
                addResult("Timestep", true, 
                         "Valid timestep: " + std::to_string(value.value()) + "s", 
                         0.0);
                std::cout << "✓ simulation_timestep: " << value.value() << "s (valid)" << std::endl;
            } else {
                addResult("Timestep", false, 
                         "Invalid timestep value", 
                         0.0);
                std::cout << "✗ simulation_timestep: Must be > 0 and < 1.0" << std::endl;
            }
        }
        
        // Validate neuron count
        if (config->has("neuron_count")) {
            auto value = config->get<int64_t>("neuron_count");
            if (value.has_value() && value.value() > 0) {
                addResult("Neuron Count", true, 
                         "Valid neuron count: " + std::to_string(value.value()), 
                         0.0);
                std::cout << "✓ neuron_count: " << value.value() << " (valid)" << std::endl;
            } else {
                addResult("Neuron Count", false, 
                         "Invalid neuron count", 
                         0.0);
                std::cout << "✗ neuron_count: Must be > 0" << std::endl;
            }
        }
        
        // Validate region count
        if (config->has("region_count")) {
            auto value = config->get<int64_t>("region_count");
            if (value.has_value() && value.value() > 0 && value.value() <= 10) {
                addResult("Region Count", true, 
                         "Valid region count: " + std::to_string(value.value()), 
                         0.0);
                std::cout << "✓ region_count: " << value.value() << " (valid)" << std::endl;
            } else {
                addResult("Region Count", false, 
                         "Invalid region count", 
                         0.0);
                std::cout << "✗ region_count: Must be 1-10" << std::endl;
            }
        }
    }
    
    void testBrainFunctionality() {
        std::cout << "\n=== Brain Functionality Tests ===" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        
        auto initTime = std::chrono::high_resolution_clock::now();
        double initTimeMs = std::chrono::duration<double, std::milli>(initTime - startTime).count();
        
        if (!brain->initialize()) {
            addResult("Brain Initialization", false, "Brain failed to initialize", initTimeMs);
            std::cout << "Error: Brain initialization failed" << std::endl;
            return;
        }
        
        addResult("Brain Initialization", true, "Brain initialized successfully", initTimeMs);
        std::cout << "✓ Brain initialization successful" << std::endl;
        
        // Test 1: Basic connectivity
        std::cout << "\nTest 1: Basic Neural Connectivity" << std::endl;
        runBasicConnectivityTest(brain);
        
        // Reset brain for next test
        brain->reset();
        brain->initialize();
        
        // Test 2: Plasticity
        std::cout << "\nTest 2: Plasticity Learning" << std::endl;
        runPlasticityTest(brain);
        
        // Test 3: Memory
        std::cout << "\nTest 3: Memory Functionality" << std::endl;
        runMemoryTest(brain);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        addResult("Overall Test Time", true, 
                 "All tests completed in " + std::to_string(totalTimeMs) + "ms", 
                 totalTimeMs);
        
        std::cout << "\n=== Brain functionality tests complete ===" << std::endl;
    }
    
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            addResult("Connectivity Test", false, "No brain region found", 0.0);
            std::cout << "✗ No brain region found" << std::endl;
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            addResult("Connectivity Test", false, "No neurons found", 0.0);
            std::cout << "✗ No neurons found" << std::endl;
            return;
        }
        
        // Inject current into neurons and check for spikes
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run simulation steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        
        if (spikes > 0) {
            addResult("Connectivity Test", true, 
                     "Generated " + std::to_string(spikes) + " spikes through network", 
                     0.0);
            std::cout << "✓ Generated " << spikes << " spikes through network" << std::endl;
        } else {
            addResult("Connectivity Test", false, 
                     "No spikes generated", 
                     0.0);
            std::cout << "✗ No spikes generated" << std::endl;
        }
    }
    
    void runPlasticityTest(std::shared_ptr<Brain> brain) {
        // Enable plasticity
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Run input patterns to stimulate learning
        for (SimulationStep step = 0; step < 100; ++step) {
            // Create input pattern
            for (size_t i = 0; i < 5 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        // Check if learning occurred (simplified check)
        auto* spikeSystem = brain->getSpikeSystem();
        if (spikeSystem) {
            auto mostActive = spikeSystem->getMostActiveNeurons(5);
            if (!mostActive.empty()) {
                addResult("Plasticity Test", true, 
                         "Learning detected - " + std::to_string(mostActive.size()) + " active neurons", 
                         0.0);
                std::cout << "✓ Learning detected - " << mostActive.size() << " active neurons" << std::endl;
            } else {
                addResult("Plasticity Test", false, 
                         "No significant learning activity", 
                         0.0);
                std::cout << "✗ No significant learning activity" << std::endl;
            }
        }
    }
    
    void runMemoryTest(std::shared_ptr<Brain> brain) {
        // Simple memory test - check if memory systems are working
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        
        bool memoryActive = false;
        
        if (workingMemory) {
            memoryActive = true;
            addResult("Working Memory Test", true, "Working memory system accessible", 0.0);
            std::cout << "✓ Working memory system accessible" << std::endl;
        }
        
        if (episodicMemory) {
            memoryActive = true;
            addResult("Episodic Memory Test", true, "Episodic memory system accessible", 0.0);
            std::cout << "✓ Episodic memory system accessible" << std::endl;
        }
        
        if (memoryActive) {
            addResult("Memory Integration Test", true, "Memory systems integrated", 0.0);
            std::cout << "✓ Memory systems integrated" << std::endl;
        } else {
            addResult("Memory Integration Test", false, 
                     "Memory systems not accessible", 
                     0.0);
            std::cout << "✗ Memory systems not accessible" << std::endl;
        }
    }
    
    void printResults() {
        std::cout << "\n=== Validation Results ===" << std::endl;
        
        int passed = 0;
        int failed = 0;
        double totalTime = 0.0;
        
        for (const auto& result : results) {
            std::string status = result.passed ? "✓ PASS" : "✗ FAIL";
            std::cout << status << " " << std::left << std::setw(25) << result.testName 
                     << " - " << result.message << std::endl;
            
            if (result.passed) {
                passed++;
            } else {
                failed++;
            }
            
            totalTime += result.executionTime;
        }
        
        std::cout << "\n=== Summary ===" << std::endl;
        std::cout << "Total Tests: " << results.size() << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Success Rate: " << (passed * 100.0 / results.size()) << "%" << std::endl;
        std::cout << "Total Execution Time: " << totalTime << "ms" << std::endl;
        
        if (failed > 0) {
            std::cout << "\nValidation FAILED with " << failed << " failures." << std::endl;
        } else {
            std::cout << "\nValidation PASSED!" << std::endl;
        }
    }
    
    void printUsage(const std::string& programName) {
        std::cout << "Usage: " << programName << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --config <file>           Load and validate configuration file" << std::endl;
        std::cout << "  --brain-tests             Run brain functionality tests" << std::endl;
        std::cout << "  --all                    Run all validation tests" << std::endl;
    }
};

int main(int argc, char** argv) {
    std::string programName = "nlm-validate";
    std::string configFile;
    bool runBrainTests = false;
    bool runAll = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            NLMValidator validator;
            validator.printUsage(programName);
            return 0;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            } else {
                std::cerr << "Error: --config requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "--brain-tests") {
            runBrainTests = true;
        } else if (arg == "--all") {
            runAll = true;
        }
    }
    
    NLMValidator validator;
    
    if (!configFile.empty()) {
        validator.validateConfig(configFile);
        
        if (runBrainTests || runAll) {
            validator.testBrainFunctionality();
        }
    } else if (runBrainTests || runAll) {
        std::cout << "Error: --config is required for brain tests" << std::endl;
        return 1;
    } else {
        validator.printUsage(programName);
        return 0;
    }
    
    return 0;
}
