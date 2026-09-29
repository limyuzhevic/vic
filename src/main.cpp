#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <ctime>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Comprehensive Neural Simulation Framework                  ║
    ║                                                               ║
    ║     Multiple phases and advanced controls                       ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp(const std::string& programName) {
    std::cout << "=== NLM Usage ===" << std::endl;
    std::cout << "Program: " << programName << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "Basic Usage:" << std::endl;
    std::cout << "  " << programName << " [options]" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "Phase Options:" << std::endl;
    std::cout << "  --phase X        Run specific phase (1-6, 0 for all phases)" << std::endl;
    std::cout << "  --run [name]     Run named experiment (basic, plasticity, stdp, connectivity, integration)" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --config FILE    Load configuration from FILE (default: configs/default.cfg)" << std::endl;
    std::cout << "  --set KEY=VAL    Set configuration key-value pair" << std::endl;
    std::cout << "  --list-config     List all configuration keys and values" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "Execution Options:" << std::endl;
    std::cout << "  --steps N        Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --duration T     Simulation duration in seconds (default: 1.0)" << std::endl;
    std::cout << "  --threads N      Number of threads for parallel processing" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "Logging Options:" << std::endl;
    std::cout << "  --log-level L    Set log level (trace, debug, info, warning, error, critical)" << std::endl;
    std::cout << "  --log-file F      Write logs to file F" << std::endl;
    std::cout << "  --quiet          Suppress all output except errors" << std::endl;
    std::cout << "  --verbose        Enable verbose output" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "Output Options:" << std::endl;
    std::cout << "  --output-format F    Output format (text, json, csv)" << std::endl;
    std::cout << "  --output-file F       Write output to file F" << std::endl;
    std::cout << "  --metrics             Output detailed metrics" << std::endl;
    std::cout << "  --performance         Measure and report performance metrics" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "Debug Options:" << std::endl;
    std::cout << "  --debug              Enable debug mode with extended validation" << std::endl;
    std::cout << "  --validate           Validate configuration before running" << std::endl;
    std::cout << "  --checkpoint F       Save checkpoint to F" << std::endl;
    std::cout << "  --load-checkpoint F   Load checkpoint from F" << std::endl;
    std::cout << "" << std::endl;
    
    std::cout << "Expert Options:" << std::endl;
    std::cout << "  --neurogenesis        Enable neurogenesis (advanced feature)" << std::endl;
    std::cout << "  --custom-plasticity   Enable custom plasticity rules" << std::endl;
    std::cout << "  --memory-tracing     Enable detailed memory tracing" << std::endl;
    std::cout << "  --performance-profile Run performance profiling" << std::endl;
    std::cout << "" << std::endl;
}

void printVersion() {
    std::cout << "NLM Version 0.1.0" << std::endl;
    std::cout << "Phase 6 Integration Complete" << std::endl;
    std::cout << "Build: Comprehensive neural simulation framework" << std::endl;
    std::cout << "" << std::endl;
}

void logConfigurationSummary(const std::shared_ptr<Config>& config) {
    if (!config) {
        NLM_LOG_WARNING("Configuration is null - cannot log summary");
        return;
    }
    
    NLM_LOG_INFO("=== Configuration Summary ===");
    
    // Get important configuration keys
    std::vector<std::string> keys = config->getKeys();
    std::vector<std::string> importantKeys = {
        "random_seed", "simulation_timestep", "neuron_count", "region_count",
        "connection_probability", "stdp_ltp_weight", "stdp_ltd_weight",
        "plasticity_learning_rate", "development_synaptogenesis_rate",
        "development_pruning_rate", "dopamine_baseline", "reward_discount_factor",
        "environment_name", "environment_width", "environment_height"
    };
    
    for (const auto& key : importantKeys) {
        if (config->has(key)) {
            if (key.find("seed") != std::string::npos) {
                NLM_LOG_INFO("  " + key + ": " + std::to_string(config->getOr<int64_t>(key, 0)));
            } else if (key.find("timestep") != std::string::npos || key.find("rate") != std::string::npos) {
                NLM_LOG_INFO("  " + key + ": " + std::to_string(config->getOr<double>(key, 0.0)));
            } else if (key.find("count") != std::string::npos) {
                NLM_LOG_INFO("  " + key + ": " + std::to_string(config->getOr<int64_t>(key, 0)));
            } else {
                NLM_LOG_INFO("  " + key + ": " + std::to_string(config->getOr<float>(key, 0.0f)));
            }
        }
    }
}

void printMetrics(const std::shared_ptr<Brain>& brain, double wallClockTime,
                  size_t totalSteps, const std::string& phaseName) {
    if (!brain) {
        NLM_LOG_WARNING("Cannot print metrics: brain is null");
        return;
    }
    
    NLM_LOG_INFO("=== Performance Metrics ===");
    NLM_LOG_INFO("Phase: " + phaseName);
    NLM_LOG_INFO("Wall clock time: " + std::to_string(wallClockTime) + "s");
    NLM_LOG_INFO("Steps per second: " + std::to_string(totalSteps / wallClockTime) + " steps/s");
    
    if (brain->getTotalNeuronCount() > 0) {
        NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
        NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
        NLM_LOG_INFO("Excitation/inhibition ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        NLM_LOG_INFO("Total spike count: " + std::to_string(brain->getTotalSpikeCount()));
    }
    
    NLM_LOG_INFO("=== End of Metrics ===");
}

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        if (!brain) return;
        
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                if (syn) {
                    initialWeights.push_back(syn->getWeight());
                }
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                if (syn) {
                    finalWeights.push_back(syn->getWeight());
                }
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain, bool verbose = false) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot run connectivity test: brain is null");
        return;
    }
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("  No region found for connectivity test");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        Neuron* neuron = neurons[i];
        if (neuron) {
            neuron->injectCurrent(50.0f);  // Strong excitatory input
        }
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));\n    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        if (verbose && !neurons.empty()) {
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                Neuron* neuron = neurons[i];
                if (neuron) {
                    NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                                 " V=" + std::to_string(neuron->getMembranePotential()) +
                                 " thresh=" + std::to_string(neuron->getThreshold()));
                }
            }
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain, bool verbose = false) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot run plasticity experiment: brain is null");
        return;
    }
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            if (syn) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (verbose && step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                         " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                         " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain, bool verbose = false) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot run STDP verification: brain is null");
        return;
    }
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("  No region found for STDP test");
        return;
    }
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
        Synapse* syn = synapses[i];
        if (syn) {
            beforeWeights.push_back(syn->getWeight());
            syn->enablePlasticity(false, true, false);  // Enable only STDP
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                         " before: " + std::to_string(beforeWeights[i]));
        }
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    float totalDelta = 0.0f;
    for (size_t i = 0; i < beforeWeights.size(); ++i) {
        Synapse* syn = synapses[i];
        if (syn) {
            float delta = syn->getWeight() - beforeWeights[i];
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                         " after: " + std::to_string(syn->getWeight()) +
                         " (Δ=" + std::to_string(delta) + ")");
            totalDelta += delta;
        }
    }
    
    // Check if weights increased (LTP)
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}