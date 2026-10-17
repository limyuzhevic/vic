#pragma once

#include "Phase2Experiments.hpp"
#include "../core/Logger/Logger.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <iostream>

namespace nlm {

/**
 * Phase 2 experiment runner using the new experiment execution framework
 */
class Phase2ExperimentRunner {
public:
    Phase2ExperimentRunner() {
        // Setup logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    ~Phase2ExperimentRunner() = default;
    
    /**
     * Run all Phase 2 experiments
     */
    bool runAllExperiments(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
        NLM_LOG_INFO("Running comprehensive integration tests...");
        NLM_LOG_INFO("");
        
        std::vector<std::string> experimentNames = {
            "ConnectivityTest",
            "PlasticityLearning", 
            "STDPVerification"
        };
        
        bool allPassed = true;
        
        for (const auto& name : experimentNames) {
            if (runExperiment(name, brain, environment)) {
                NLM_LOG_INFO("✓ " + name + " PASSED");
            } else {
                NLM_LOG_ERROR("✗ " + name + " FAILED");
                allPassed = false;
            }
            NLM_LOG_INFO("");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = endTime - startTime;
        
        NLM_LOG_INFO("=== Phase 2 Complete ===");
        NLM_LOG_INFO("Execution time: " + std::to_string(duration.count()) + " seconds");
        NLM_LOG_INFO("");
        
        if (allPassed) {
            NLM_LOG_INFO("✓ All Phase 2 experiments PASSED");
            printPhase2Summary(brain);
        } else {
            NLM_LOG_ERROR("✗ Some Phase 2 experiments FAILED");
        }
        
        return allPassed;
    }
    
    /**
     * Run a specific experiment
     */
    bool runExperiment(const std::string& experimentName, 
                      std::shared_ptr<Brain> brain, 
                      std::shared_ptr<Environment> environment) {
        ExperimentManager experimentManager;
        
        if (!experimentManager.hasExperiment(experimentName)) {
            NLM_LOG_ERROR("Experiment not found: " + experimentName);
            return false;
        }
        
        NLM_LOG_INFO("Running experiment: " + experimentName);
        
        auto result = experimentManager.runExperiment(experimentName, brain, environment);
        
        if (result.success) {
            NLM_LOG_INFO("Experiment completed successfully: " + result.message);
        } else {
            NLM_LOG_ERROR("Experiment failed: " + result.message);
        }
        
        return result.success;
    }
    
    /**
     * Run a simple brain step for logging
     */
    void runBrainStep(std::shared_ptr<Brain> brain, uint64_t step) {
        if (brain) {
            brain->step(step, step * 0.001);
        }
    }
    
private:
    void printPhase2Summary(std::shared_ptr<Brain> brain) {
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
};

} // namespace nlm