#include "core/ExperimentController.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <iostream>
#include <iomanip>

namespace nlm {

ExperimentController::ExperimentController() 
    : configManager(std::make_unique<ConfigManager>()),
      testRunner(std::make_unique<TestRunner>()) {}

bool ExperimentController::runExperiment(const std::string& configFile, int argc, char** argv) {
    try {
        statusMessage = "Initializing experiment...";
        NLM_LOG_INFO("Starting NLM Phase 2 Experiment");
        
        // Setup phase
        if (!setupPhase(configFile, argc, argv)) {
            statusMessage = "Failed to setup experiment";
            return false;
        }
        
        // Get configuration
        auto config = configManager->getConfig();
        if (!config) {
            statusMessage = "Failed to get configuration";
            return false;
        }
        
        // Initialize simulation clock
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
        
        // Initialize brain
        statusMessage = "Initializing NLM Brain...";
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            statusMessage = "Brain initialization failed";
            return false;
        }
        
        brain->logStatus();
        
        // Log startup information
        logStartupInfo();
        
        // Run phase
        statusMessage = "Running experiments...";
        if (!runPhase(brain)) {
            statusMessage = "Experiment execution failed";
            return false;
        }
        
        // Log completion information
        statusMessage = "Experiment completed successfully";
        logCompletionInfo(brain);
        
        // Cleanup phase
        cleanupPhase();
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception in experiment: ") + e.what());
        statusMessage = "Experiment exception: " + std::string(e.what());
        cleanupPhase();
        return false;
    }
}

bool ExperimentController::setupPhase(const std::string& configFile, int argc, char** argv) {
    statusMessage = "Loading configuration...";
    
    if (!configManager->loadConfig(configFile, argc, argv)) {
        NLM_LOG_ERROR("Failed to load configuration");
        return false;
    }
    
    configManager->logConfigSummary();
    return true;
}

bool ExperimentController::runPhase(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("CRITICAL: Brain is null - cannot run experiment phase");
        return false;
    }
    
    // Run all tests
    bool testsPassed = testRunner->runAllTests(brain);
    
    if (!testsPassed) {
        NLM_LOG_ERROR("Some tests failed");
        return false;
    }
    
    // Get test results
    experimentResults = testRunner->getTestResults();
    
    return true;
}

void ExperimentController::cleanupPhase() {
    NLM_LOG_INFO("Cleaning up experiment...");
    // Additional cleanup can be added here
}

void ExperimentController::logStartupInfo() const {
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
}

void ExperimentController::logCompletionInfo(std::shared_ptr<Brain> brain) const {
    if (!brain) {
        NLM_LOG_ERROR("CRITICAL: Brain is null - cannot log completion info");
        return;
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 2 Objectives Completed:");
    NLM_LOG_INFO("  ✓ Real LIF neuron dynamics implemented");
    NLM_LOG_INFO("  ✓ Event-driven spike propagation with delays");
    NLM_LOG_INFO("  ✓ STDP plasticity rule");
    NLM_LOG_INFO("  ✓ Hebbian plasticity rule");
    NLM_LOG_INFO("  ✓ Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
    NLM_LOG_INFO("");
}

} // namespace nlm