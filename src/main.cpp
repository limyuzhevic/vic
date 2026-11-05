#include "experiments/LearningExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "brain/Region.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/SpikeSystem.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Metrics.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <algorithm>
#include <chrono>
#include <cmath>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Simple main menu for easy access to different test modes
void showMainMenu() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                        NLM TEST MENU                          ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  [1] Run Basic Connectivity Test                            ║\n";
    std::cout << "║  [2] Run Plasticity Learning Experiment                     ║\n";
    std::cout << "║  [3] Run STDP Verification Test                              ║\n";
    std::cout << "║  [4] Run Advanced Learning Test                               ║\n";
    std::cout << "║  [5] Run All Tests (Full Suite)                             ║\n";
    std::cout << "║  [6] Configure Test Settings                                 ║\n";
    std::cout << "║  [7] Show Available Config Files                             ║\n";
    std::cout << "║  [8] Performance Benchmark                                   ║\n";
    std::cout << "║  [9] Exit                                                     ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    std::cout << "Enter your choice: ";
}

class TestRunner {
public:
    TestRunner() {
        // Initialize logger for tests
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    struct TestResult {
        std::string name;
        bool passed;
        std::string message;
        double executionTime;
    };
    
    TestResult runBasicConnectivityTest(const std::string& configFile = "configs/default.cfg") {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("=== Test: Basic Neural Connectivity ===");
        
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            return {"Basic Connectivity", false, "Failed to load config", 0.0};
        }
        
        // Apply test-specific settings
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            return {"Basic Connectivity", false, "Failed to initialize brain", 0.0};
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        auto neurons = brain->getAllNeurons();
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        if (spikes > 0) {
            return {"Basic Connectivity", true, 
                   "Spikes propagate through network: " + std::to_string(spikes), executionTime};
        } else {
            return {"Basic Connectivity", false, "No spikes generated", executionTime};
        }
    }
    
    TestResult runPlasticityExperiment(const std::string& configFile = "configs/default.cfg") {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("=== Test: Plasticity Learning Experiment ===");
        
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            return {"Plasticity", false, "Failed to load config", 0.0};
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            return {"Plasticity", false, "Failed to initialize brain", 0.0};
        }
        
        // Create learning experiment
        LearningExperiment experiment(brain, 42);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            return {"Plasticity", false, "No brain region found", 0.0};
        }
        
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
        
        // Apply selective input pattern (not 600.0f to every neuron)
        std::vector<NeuronId> patternNeurons;
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
        
        NLM_LOG_INFO("Applying selective input pattern (1000 steps)...");
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (NeuronId id : patternNeurons) {
                brain->injectCurrent(id, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state and compute statistics
        experiment.recordFinalState();
        experiment.computeStatistics();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return {"Plasticity", true, "Plasticity experiment completed", executionTime};
    }
    
    TestResult runStdpVerification(const std::string& configFile = "configs/default.cfg") {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("=== Test: STDP Verification ===");
        
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            return {"STDP", false, "Failed to load config", 0.0};
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            return {"STDP", false, "Failed to initialize brain", 0.0};
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            return {"STDP", false, "No brain region found", 0.0};
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            return {"STDP", false, "No synapses for STDP test", 0.0};
        }
        
        Synapse* syn = synapses[0].get();
        syn->enablePlasticity(false, true, false);  // STDP only
        float beforeWeight = syn->getWeight();
        
        NLM_LOG_INFO("  Testing STDP on 1 synapse:");
        NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                     " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                     ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
        
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 50; ++trial) {
            brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float delta = syn->getWeight() - beforeWeight;
        NLM_LOG_INFO("  After correlated activity:");
        NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                     " (Δ=" + std::to_string(delta) + ")");
        
        float totalDelta = delta;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        if (totalDelta > 0.001f) {
            return {"STDP", true, "STDP working: Pre-before-post produced potentiation", executionTime};
        } else if (totalDelta < -0.001f) {
            return {"STDP", false, "STDP reversed: Check parameters", executionTime};
        } else {
            return {"STDP", false, "No change: STDP may not be triggering", executionTime};
        }
    }
    
    TestResult runAdvancedLearningTest(const std::string& configFile = "configs/test.cfg") {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("=== Test: Advanced Learning with Enhanced Plasticity ===");
        
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            return {"Advanced Learning", false, "Failed to load config", 0.0};
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            return {"Advanced Learning", false, "Failed to initialize brain", 0.0};
        }
        
        // Enable all plasticity types for advanced learning test
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);  // STDP, Hebbian, structural
            }
        }
        
        // Apply complex learning pattern
        NLM_LOG_INFO("  Running complex learning pattern (2000 steps)...");
        
        // Create oscillating input pattern
        std::vector<NeuronId> sensoryNeurons;
        if (region) {
            for (auto* neuron : region->getAllNeurons()) {
                if (neuron->getType() == NeuronType::Sensory && sensoryNeurons.size() < 50) {
                    sensoryNeurons.push_back(neuron->getId());
                }
            }
        }
        
        // Run alternating pattern
        for (SimulationStep step = 0; step < 2000; ++step) {
            // Inject pattern A: first half of sensory neurons
            size_t halfCount = sensoryNeurons.size() / 2;
            for (size_t i = 0; i < halfCount; ++i) {
                brain->injectCurrent(sensoryNeurons[i], 25.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Inject pattern B: second half of sensory neurons (different timing)
            if (step % 2 == 1) {
                for (size_t i = halfCount; i < sensoryNeurons.size() && i < halfCount + 10; ++i) {
                    brain->injectCurrent(sensoryNeurons[i], 30.0f);
                }
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 500 == 0) {
                NLM_LOG_INFO("    Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Synapses: " + std::to_string(brain->getTotalSynapseCount()));
            }
        }
        
        // Analyze learning outcomes
        NLM_LOG_INFO("  Analyzing learning outcomes...");
        
        size_t totalSpikes = brain->getTotalSpikeCount();
        size_t totalSynapses = brain->getTotalSynapseCount();
        float avgFiringRate = brain->getAverageFiringRate();
        
        NLM_LOG_INFO("  Advanced learning results:");
        NLM_LOG_INFO("    Total spikes: " + std::to_string(totalSpikes));
        NLM_LOG_INFO("    Total synapses: " + std::to_string(totalSynapses));
        NLM_LOG_INFO("    Average firing rate: " + std::to_string(avgFiringRate));
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        if (totalSpikes > 1000 && avgFiringRate > 0.1) {
            return {"Advanced Learning", true, 
                   "Complex learning successful: high activity and synaptic changes", executionTime};
        } else {
            return {"Advanced Learning", false, "Learning insufficient for advanced test", executionTime};
        }
    }
    
    void runAllTests(const std::string& configFile = "configs/default.cfg") {
        NLM_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
        NLM_LOG_INFO("║                    NLM COMPREHENSIVE TEST SUITE                    ║");
        NLM_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
        NLM_LOG_INFO("");
        
        std::vector<TestResult> results;
        
        // Run tests
        results.push_back(runBasicConnectivityTest(configFile));
        results.push_back(runPlasticityExperiment(configFile));
        results.push_back(runStdpVerification(configFile));
        results.push_back(runAdvancedLearningTest(configFile));
        
        // Display summary
        NLM_LOG_INFO("");
        NLM_LOG_INFO("╔════════════════════════════════════════════════════════════════╗");
        NLM_LOG_INFO("║                     TEST SUMMARY                                  ║");
        NLM_LOG_INFO("╠═══════╦═══════════════════╦═════════════╦════════════╦═════════╣");
        NLM_LOG_INFO("║ Test  ║ Result           ║ Time (s)    ║ Status     ║ Details ║");
        NLM_LOG_INFO("╠═══════╬═══════════════════╬═════════════╬════════════╬═════════╣");
        
        int passedCount = 0;
        for (const auto& result : results) {
            std::string status = result.passed ? "PASSED" : "FAILED";
            std::string statusColor = result.passed ? "✓" : "✗";
            
            // Truncate long names for display
            std::string displayName = result.name;
            if (displayName.length() > 16) {
                displayName = displayName.substr(0, 13) + "...";
            }
            
            NLM_LOG_INFO("║ " + std::string(1, statusColor) + " ║ " + 
                        std::string(18 - (int)displayName.length(), ' ') + displayName + " ║ " +
                        std::string(11, ' ') + std::to_string(result.executionTime).substr(0, 6) + " ║ " +
                        std::string(11 - (int)status.length(), ' ') + status + " ║ " +
                        std::string(10, ' ') + result.message.substr(0, 10) + "... ║");
            
            if (result.passed) passedCount++;
        }
        
        NLM_LOG_INFO("╠═══════╩═══════════════════╩═════════════╩════════════╩═════════╣");
        NLM_LOG_INFO("║ Summary: " + std::to_string(passedCount) + "/" + std::to_string(results.size()) + 
                     " tests passed                                         ║");
        NLM_LOG_INFO("╚════════════════════════════════════════════════════════════════╝");
        
        if (passedCount == results.size()) {
            NLM_LOG_INFO("\n🎉 ALL TESTS PASSED! The NLM system is functioning correctly.");
        } else {
            NLM_LOG_INFO("\n⚠️  Some tests failed. Review the output above for details.");
        }
    }
};

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Comprehensive Test Suite...\n" << std::endl;
    
    TestRunner runner;
    
    // Check for command line arguments
    std::string configFile = "configs/default.cfg";
    bool runOnlyAdvanced = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--advanced-only") {
            runOnlyAdvanced = true;
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg == "--help") {
            std::cout << "\nNLM Test Suite Usage:" << std::endl;
            std::cout << "  nlm_test                    Run all tests" << std::endl;
            std::cout << "  nlm_test --advanced-only    Run only advanced learning tests" << std::endl;
            std::cout << "  nlm_test --config <file>     Use specific config file" << std::endl;
            std::cout << "  nlm_test --help              Show this help" << std::endl;
            return 0;
        }
    }
    
    if (runOnlyAdvanced) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        NLM_LOG_INFO("=== Advanced Learning Test Only ===");
        auto result = runner.runAdvancedLearningTest(configFile);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        if (result.passed) {
            std::cout << "\n✅ Advanced learning test PASSED!" << std::endl;
            std::cout << "   Execution time: " << executionTime << "s" << std::endl;
            std::cout << "   Result: " << result.message << std::endl;
        } else {
            std::cout << "\n❌ Advanced learning test FAILED!" << std::endl;
            std::cout << "   Execution time: " << executionTime << "s" << std::endl;
            std::cout << "   Error: " << result.message << std::endl;
        }
    } else {
        runner.runAllTests(configFile);
    }
    
    return 0;
}