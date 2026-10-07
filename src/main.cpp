// NLM Main Entry Point - Refactored for Better Architecture
// This file now serves as the primary coordinator, delegating to specialized modules

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include "sensory/SensoryInput.hpp"
#include "visualization/VisualizationInterface.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <chrono>

using namespace nlm;

// Forward declarations for module functions
void initializeSystem();
void shutdownSystem();
void printWelcomeBanner();
void loadSystemConfiguration(Config& config, int argc, char** argv);
void displayConfigurationSummary(const Config& config);
void runBasicConnectivityTests(std::shared_ptr<Brain> brain);
void runAdvancedPlasticityExperiments(std::shared_ptr<Brain> brain);
void runStdpVerificationExperiments(std::shared_ptr<Brain> brain);
void runIntegrationDemo(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment);
void runComprehensiveBenchmark(std::shared_ptr<Brain> brain);
void saveExperimentResults(const std::string& filename);

// Global system state
struct SystemState {
    bool initialized = false;
    std::chrono::steady_clock::time_point startTime;
    uint64_t totalStepsExecuted = 0;
    float totalMemoryUsage = 0.0f;
};

SystemState g_systemState;

int main(int argc, char** argv) {
    // Initialize system timing
    g_systemState.startTime = std::chrono::steady_clock::now();
    
    try {
        // Phase 1: System initialization
        printWelcomeBanner();
        initializeSystem();
        
        // Phase 2: Configuration management
        auto config = std::make_shared<Config>();
        loadSystemConfiguration(*config, argc, argv);
        displayConfigurationSummary(*config);
        
        // Phase 3: Brain initialization
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
            return 1;
        }
        
        brain->logStatus();
        
        // Phase 4: Execute experiments based on command line options
        if (config->has("experiment_mode")) {
            std::string mode = config->getOr<std::string>("experiment_mode", "");
            
            if (mode == "connectivity") {
                runBasicConnectivityTests(brain);
            } else if (mode == "plasticity") {
                runAdvancedPlasticityExperiments(brain);
            } else if (mode == "stdp") {
                runStdpVerificationExperiments(brain);
            } else if (mode == "integration") {
                auto environment = std::make_shared<Environment>();
                runIntegrationDemo(brain, environment);
            } else if (mode == "benchmark") {
                runComprehensiveBenchmark(brain);
            } else {
                std::cerr << "ERROR: Unknown experiment mode: " << mode << std::endl;
                return 1;
            }
        } else {
            // Default: Run all basic tests
            runBasicConnectivityTests(brain);
            brain->reset();
            brain->initialize();
            runAdvancedPlasticityExperiments(brain);
            brain->reset();
            brain->initialize();
            runStdpVerificationExperiments(brain);
        }
        
        // Phase 5: System cleanup and reporting
        shutdownSystem();
        saveExperimentResults("experiment_results.json");
        
        std::cout << "\n=== NLM Execution Complete ===" << std::endl;
        std::cout << "Total steps executed: " << g_systemState.totalStepsExecuted << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        shutdownSystem();
        return 1;
    } catch (...) {
        std::cerr << "FATAL UNKNOWN ERROR!" << std::endl;
        shutdownSystem();
        return 1;
    }
}