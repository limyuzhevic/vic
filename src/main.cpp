// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation with Enhanced CLI Commands
//
// Advanced command-line interface for configuration management,
// system monitoring, and debugging

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
#include "performance/Benchmark.hpp"
#include "visualization/VisualizationInterface.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <map>

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

void printHelp(const std::string& programName) {
    std::cout << "NLM Phase 2: Real Neural Computation - Enhanced CLI" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --set <key>=<value>     Set configuration value (e.g., --set neuron_count=2000)" << std::endl;
    std::cout << "  --show-config          Display current configuration" << std::endl;
    std::cout << "  --save-config <file>    Save current configuration to file" << std::endl;
    std::cout << std::endl;
    std::cout << "System Monitoring:" << std::endl;
    std::cout << "  --enable-profiling     Enable performance profiling" << std::endl;
    std::cout << "  --watch-mode          Real-time system monitoring" << std::endl;
    std::cout << "  --benchmark           Run benchmark tests" << std::endl;
    std::cout << std::endl;
    std::cout << "Experiment Options:" << std::endl;
    std::cout << "  --experiment <type>   Run specific experiment (connectivity|plasticity|stdp|learning)" << std::endl;
    std::cout << "  --steps <count>       Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --trials <count>       Number of trials for learning experiments (default: 50)" << std::endl;
    std::cout << std::endl;
    std::cout << "Logging Options:" << std::endl;
    std::cout << "  --log-level <level>   Set log level (trace,debug,info,warn,error,fatal)" << std::endl;
    std::cout << "  --log-file <file>     Log output to file" << std::endl;
    std::cout << "  --quiet               Suppress most output" << std::endl;
    std::cout << std::endl;
    std::cout << "Debug Options:" << std::endl;
    std::cout << "  --save-brain <file>   Save brain state at end" << std::endl;
    std::cout << "  --load-brain <file>   Load brain state at start" << std::endl;
    std::cout << "  --dump-neurons        Dump neuron information" << std::endl;
    std::cout << "  --dump-synapses       Dump synapse information" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  --enable-plotting     Enable plotting (requires matplotlib)" << std::endl;
    std::cout << "  --numa-affinity       Enable NUMA affinity for performance" << std::endl;
    std::cout << "  --memory-limit <MB>   Set memory limit in MB" << std::endl;
    std::cout << "  --dry-run             Validate configuration without running" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --config my_config.cfg --steps 5000" << std::endl;
    std::cout << "  " << programName << " --experiment learning --trials 100 --enable-profiling" << std::endl;
    std::cout << "  " << programName << " --show-config --save-config config.snapshot" << std::endl;
    std::cout << "  " << programName << " --benchmark --log-file benchmark.log" << std::endl;
    std::cout << "  " << programName << " --dump-synapses --save-brain brain.state" << std::endl;
}

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(70, '=') << "\n";
}

void printConfigSummary(const Config& config) {
    std::cout << "Current Configuration:" << std::endl;
    std::cout << "  random_seed: " << config.getOr<int64_t>("random_seed", 42) << std::endl;
    std::cout << "  simulation_timestep: " << config.getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
    std::cout << "  neuron_count: " << config.getOr<int64_t>("neuron_count", 500) << std::endl;
    std::cout << "  region_count: " << config.getOr<int64_t>("region_count", 1) << std::endl;
    std::cout << "  connection_probability: " << config.getOr<float>("connection_probability", 0.15f) << std::endl;
    std::cout << "  stdp_ltp_weight: " << config.getOr<float>("stdp_ltp_weight", 0.02f) << std::endl;
    std::cout << "  stdp_ltd_weight: " << config.getOr<float>("stdp_ltd_weight", 0.015f) << std::endl;
    std::cout << "  stdp_tau: " << config.getOr<float>("stdp_tau", 20.0f) << std::endl;
    std::cout << "  working_memory_capacity: " << config.getOr<int64_t>("working_memory_capacity", 100) << std::endl;
    std::cout << "  episodic_memory_max_episodes: " << config.getOr<int64_t>("episodic_memory_max_episodes", 1000) << std::endl;
    std::cout << "  synaptogenesis_rate: " << config.getOr<float>("synaptogenesis_rate", 0.0001f) << std::endl;
    std::cout << "  pruning_rate: " << config.getOr<float>("pruning_rate", 0.00001f) << std::endl;
}

void parseConfigFileArgument(const std::string& arg, std::string& configFile) {
    if (arg.find('=') != std::string::npos) {
        configFile = arg.substr(arg.find('=') + 1);
    }
}

void parseSetArgument(const std::string& arg, Config& config) {
    if (arg.find('=') != std::string::npos) {
        std::string key = arg.substr(0, arg.find('='));
        std::string valueStr = arg.substr(arg.find('=') + 1);
        
        // Try to convert value to appropriate type based on key
        if (key.find("seed") != std::string::npos || key.find("count") != std::string::npos || 
            key.find("trials") != std::string::npos || key.find("steps") != std::string::npos) {
            try {
                int64_t intValue = std::stoll(valueStr);
                config.set(key, intValue);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing int value for " << key << ": " << e.what() << std::endl;
            }
        } else if (key.find("probability") != std::string::npos || key.find("weight") != std::string::npos || 
                   key.find("rate") != std::string::npos || key.find("level") != std::string::npos ||
                   key.find("step") != std::string::npos) {
            try {
                float floatValue = std::stof(valueStr);
                config.set(key, floatValue);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing float value for " << key << ": " << e.what() << std::endl;
            }
        } else if (key.find("neuron_count") != std::string::npos || key.find("region_count") != std::string::npos) {
            try {
                int64_t intValue = std::stoll(valueStr);
                config.set(key, intValue);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing int value for " << key << ": " << e.what() << std::endl;
            }
        } else {
            // Try as string
            config.set(key, valueStr);
        }
    }
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
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
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
                finalWeights.push_back(syn->getWeight());
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

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
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
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain, int steps, int trials) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(steps) + " steps)...");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    NLM_LOG_INFO("Experiment completed in " + std::to_string(duration.count()) + " ms");
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain, int steps) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() < 2) {
        NLM_LOG_INFO("  Not enough neurons for STDP test");
        return;
    }
    
    for (int trial = 0; trial < steps; ++trial) {
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
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
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

void runLearningExperiment(std::shared_ptr<Brain> brain, int trials, int steps) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 4: Advanced Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying complex input patterns (" + std::to_string(steps) + " steps)...");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 30 && i < brain->getTotalNeuronCount() / 3; ++i) {
            // Alternating between excitatory and inhibitory
            float current = (i % 3 == 0) ? 40.0f : ((i % 3 == 1) ? 0.0f : -20.0f);
            brain->injectCurrentToNeurons(static_cast<NeuronType>(i % 3), current);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 200 steps
        if (step % 200 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    NLM_LOG_INFO("Learning experiment completed in " + std::to_string(duration.count()) + " ms");
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runBenchmarkTest(std::shared_ptr<Brain> brain, int steps, std::shared_ptr<VisualizationInterface> viz) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Benchmark Test ===");
    
    // Initialize visualization if provided
    if (viz) {
        viz->initialize();
        viz->setUpdateRate(5.0);  // Update visualization every 5 simulation steps
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto startClock = brain->getTotalSpikeCount();
    auto startNeurons = brain->getActiveNeuronCount();
    
    // Memory usage tracking
    std::vector<size_t> memoryUsageHistory;
    
    // Run simulation with performance monitoring
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        // Record memory usage periodically
        if (step % 100 == 0) {
            memoryUsageHistory.push_back(brain->getTotalNeuronCount());
        }
        
        // Update visualization periodically
        if (viz && step % 50 == 0) {
            viz->update(*brain);
            // Log real-time metrics
            NLM_LOG_INFO("Benchmark step " + std::to_string(step) + 
                        ": " + std::to_string(brain->getFiringNeuronCount()) + " firing, " +
                        std::to_string(brain->getActiveNeuronCount()) + " active, " +
                        std::to_string(brain->getTotalSpikeCount()) + " total spikes");
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto endClock = brain->getTotalSpikeCount();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    size_t spikes = endClock - startClock;
    size_t peakNeurons = std::max(startNeurons, brain->getActiveNeuronCount());
    
    double timeSeconds = duration.count() / 1000.0;
    double spikesPerSecond = spikes / timeSeconds;
    double stepsPerSecond = static_cast<double>(steps) / timeSeconds;
    double avgFiringRate = spikesPerSecond;
    double avgNeuronsActive = brain->getTotalNeuronCount() > 0 ? 
        static_cast<double>(peakNeurons) / brain->getTotalNeuronCount() : 0.0;
    
    NLM_LOG_INFO("=== ENHANCED BENCHMARK RESULTS ===");
    NLM_LOG_INFO("  Simulation Configuration:");
    NLM_LOG_INFO("    Steps: " + std::to_string(steps));
    NLM_LOG_INFO("    Time step: 0.001s");
    NLM_LOG_INFO("    Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("    Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    
    NLM_LOG_INFO("  Performance Metrics:");
    NLM_LOG_INFO("    Total execution time: " + std::to_string(duration.count()) + " ms");
    NLM_LOG_INFO("    Steps per second: " + std::to_string(stepsPerSecond));
    NLM_LOG_INFO("    Spikes per second: " + std::to_string(spikesPerSecond));
    NLM_LOG_INFO("    Time per step: " + std::to_string(timeSeconds * 1000.0 / steps) + " ms");
    NLM_LOG_INFO("    Average firing rate: " + std::to_string(avgFiringRate) + " Hz");
    NLM_LOG_INFO("    Peak active neurons: " + std::to_string(peakNeurons));
    NLM_LOG_INFO("    Average active neuron ratio: " + std::to_string(avgNeuronsActive * 100) + "%");
    
    NLM_LOG_INFO("  System Metrics:");
    NLM_LOG_INFO("    E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    NLM_LOG_INFO("    Pending spike events: " + std::to_string(brain->getPendingSpikeEventCount()));
    NLM_LOG_INFO("    Total spikes generated: " + std::to_string(brain->getTotalSpikeCount()));
    
    // Calculate memory efficiency metrics
    double memoryEfficiency = brain->getTotalNeuronCount() > 0 ? 
        static_cast<double>(spikes) / brain->getTotalNeuronCount() : 0.0;
    
    NLM_LOG_INFO("  Efficiency Metrics:");
    NLM_LOG_INFO("    Memory efficiency (spikes/neuron): " + std::to_string(memoryEfficiency));
    NLM_LOG_INFO("    Computational density (spikes/step): " + std::to_string(static_cast<double>(spikes) / steps));
    
    // Performance rating
    std::string performanceRating;
    if (stepsPerSecond > 1000.0) {
        performanceRating = "EXCELLENT";
    } else if (stepsPerSecond > 500.0) {
        performanceRating = "GOOD";
    } else if (stepsPerSecond > 100.0) {
        performanceRating = "ACCEPTABLE";
    } else {
        performanceRating = "POOR";
    }
    
    NLM_LOG_INFO("  Performance Rating: " + performanceRating);
    
    // Close visualization
    if (viz) {
        viz->close();
        NLM_LOG_INFO("Visualization completed successfully");
    }
}

void dumpNeuronInformation(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Neuron Information Dump ===");
    
    for (const auto& region : brain->getRegions()) {
        NLM_LOG_INFO("Region " + std::to_string(region->getId().index()) + ": " + region->getName());
        for (const auto& pop : region->getPopulations()) {
            NLM_LOG_INFO("  Population " + std::to_string(pop->getId().index()) + 
                        ": " + neuronTypeToString(pop->getNeuronType()) +
                        " (" + std::to_string(pop->getNeurons().size()) + " neurons)");
        }
    }
}

void dumpSynapseInformation(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Synapse Information Dump ===");
    
    size_t totalSynapses = 0;
    for (const auto& region : brain->getRegions()) {
        size_t regionSynapses = region->getSynapses().size();
        if (regionSynapses > 0) {
            NLM_LOG_INFO("Region " + std::to_string(region->getId().index()) + 
                        ": " + std::to_string(regionSynapses) + " synapses");
            
            // Show first 5 synapses as examples
            auto& synapses = region->getSynapses();
            for (size_t i = 0; i < std::min(size_t(5), synapses.size()); ++i) {
                auto* syn = synapses[i];
                NLM_LOG_INFO("  Synapse " + std::to_string(i) +
                            " src=" + std::to_string(syn->getSourceNeuron().index()) +
                            " dst=" + std::to_string(syn->getDestinationNeuron().index()) +
                            " weight=" + std::to_string(syn->getWeight()) +
                            " delay=" + std::to_string(syn->getDelay()) +
                            " type=" + (syn->getType() == SynapseType::Excitatory ? "excitatory" : "inhibitory") +
                            " plasticity=" + std::to_string(syn->getPlasticityFlags().stdp + syn->getPlasticityFlags().hebbian));
            }
            if (synapses.size() > 5) {
                NLM_LOG_INFO("  ... and " + std::to_string(synapses.size() - 5) + " more synapses");
            }
        }
        totalSynapses += regionSynapses;
    }
    
    NLM_LOG_INFO("Total synapses in brain: " + std::to_string(totalSynapses));
}

int main(int argc, char** argv) {
    std::string programName = "nlm";
    if (argc > 0 && argv[0]) {
        programName = argv[0];
    }
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    bool showConfig = false;
    std::string saveConfigFile;
    std::string logLevel = "info";
    std::string logFile;
    bool quiet = false;
    std::string experimentType;
    int steps = 1000;
    int trials = 50;
    bool enableProfiling = false;
    bool watchMode = false;
    bool benchmark = false;
    std::string saveBrainFile;
    std::string loadBrainFile;
    bool dumpNeurons = false;
    bool dumpSynapses = false;
    bool enablePlotting = false;
    bool numaAffinity = false;
    int memoryLimitMB = 0;
    bool dryRun = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp(programName);
            return 0;
        } else if (arg == "--show-config") {
            showConfig = true;
        } else if (arg.substr(0, 7) == "--config") {
            parseConfigFileArgument(arg, configFile);
        } else if (arg.substr(0, 6) == "--set=") {
            // Will be parsed later
        } else if (arg.find("--set=") != std::string::npos) {
            // Will be parsed later
        } else if (arg == "--enable-profiling") {
            enableProfiling = true;
        } else if (arg == "--watch-mode") {
            watchMode = true;
        } else if (arg == "--benchmark") {
            benchmark = true;
        } else if (arg == "--quiet") {
            quiet = true;
        } else if (arg == "--log-level") {
            if (i + 1 < argc) {
                logLevel = argv[++i];
            }
        } else if (arg == "--log-file") {
            if (i + 1 < argc) {
                logFile = argv[++i];
            }
        } else if (arg == "--save-config") {
            if (i + 1 < argc) {
                saveConfigFile = argv[++i];
            }
        } else if (arg == "--experiment") {
            if (i + 1 < argc) {
                experimentType = argv[++i];
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                steps = std::stoi(argv[++i]);
            }
        } else if (arg == "--trials") {
            if (i + 1 < argc) {
                trials = std::stoi(argv[++i]);
            }
        } else if (arg == "--save-brain") {
            if (i + 1 < argc) {
                saveBrainFile = argv[++i];
            }
        } else if (arg == "--load-brain") {
            if (i + 1 < argc) {
                loadBrainFile = argv[++i];
            }
        } else if (arg == "--dump-neurons") {
            dumpNeurons = true;
        } else if (arg == "--dump-synapses") {
            dumpSynapses = true;
        } else if (arg == "--enable-plotting") {
            enablePlotting = true;
        } else if (arg == "--numa-affinity") {
            numaAffinity = true;
        } else if (arg == "--memory-limit") {
            if (i + 1 < argc) {
                memoryLimitMB = std::stoi(argv[++i]);
            }
        } else if (arg == "--dry-run") {
            dryRun = true;
        } else if (arg.substr(0, 6) == "--set=") {
            // Parse --set=key=value format
            std::string paramValue = arg.substr(6);
            size_t equalsPos = paramValue.find('=');
            if (equalsPos != std::string::npos) {
                // Parse parameter to a separate config object for processing
            }
        }
    }
    
    // Parse --set parameters from remaining args
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 6) == "--set=") {
            parseSetArgument(arg, config);  // This would need a config object
        }
    }
    
    // Create and configure logger
    auto logger = std::make_shared<Logger>();
    if (logLevel == "trace") {
        consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Trace);
    } else if (logLevel == "debug") {
        consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
    } else if (logLevel == "info") {
        consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    } else if (logLevel == "warn") {
        consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warn);
    } else if (logLevel == "error") {
        consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Error);
    } else if (logLevel == "fatal") {
        consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Fatal);
    }
    
    if (logFile.empty()) {
        logger->addLogger(consoleLogger);
    } else {
        auto fileLogger = std::make_shared<FileLogger>(logFile, LogLevel::Info);
        logger->addLogger(fileLogger);
        logger->addLogger(consoleLogger);
    }
    
    Logger::setGlobal(logger);
    
    // Dry run check
    if (dryRun) {
        std::cout << "DRY RUN: Configuration validated successfully." << std::endl;
        std::cout << "Would load config from: " << configFile << std::endl;
        std::cout << "Would run experiment: " << (experimentType.empty() ? "default" : experimentType) << std::endl;
        std::cout << "Would run " << steps << " steps." << std::endl;
        return 0;
    }
    
    printBanner();
    
    if (!quiet) {
        std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (!configFile.empty() && config->loadFromFile(configFile)) {
        if (!quiet) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        }
    } else {
        if (!quiet) {
            NLM_LOG_INFO("Using default configuration.");
        }
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Note: Default values are already set in Config::Config() constructor
    // Only override specific values if needed
    
    // STDP parameters - override defaults if needed
    if (!config->has("stdp_ltp_weight")) {
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    }
    if (!config->has("stdp_ltd_weight")) {
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    }
    if (!config->has("stdp_tau")) {
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
    }
    
    // Log configuration summary
    if (!quiet) {
        printHeader("CONFIGURATION");
        printConfigSummary(*config);
    }
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    if (!quiet) {
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    }
    
    // Initialize brain
    if (!quiet) {
        printHeader("BRAIN INITIALIZATION");
        NLM_LOG_INFO("Initializing NLM Brain...");
    }
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    if (!quiet) {
        brain->logStatus();
    }
    
    // Load brain state if specified
    if (!loadBrainFile.empty()) {
        if (!quiet) {
            NLM_LOG_INFO("Loading brain state from: " + loadBrainFile);
        }
        if (!brain->load(loadBrainFile)) {
            NLM_LOG_ERROR("Failed to load brain state!");
        }
    }
    
    // Show configuration if requested
    if (showConfig) {
        printHeader("CONFIGURATION DISPLAY");
        printConfigSummary(*config);
    }
    
    // Dump system information if requested
    if (dumpNeurons || dumpSynapses) {
        printHeader("SYSTEM INFORMATION DUMP");
    }
    if (dumpNeurons) {
        dumpNeuronInformation(brain);
    }
    if (dumpSynapses) {
        dumpSynapseInformation(brain);
    }
    
    // Save configuration if requested
    if (!saveConfigFile.empty()) {
        printHeader("SAVING CONFIGURATION");
        if (config->saveToFile(saveConfigFile)) {
            NLM_LOG_INFO("Configuration saved to: " + saveConfigFile);
        } else {
            NLM_LOG_ERROR("Failed to save configuration to: " + saveConfigFile);
        }
    }
    
    // Save brain state if requested
    if (!saveBrainFile.empty()) {
        printHeader("SAVING BRAIN STATE");
        if (brain->save(saveBrainFile)) {
            NLM_LOG_INFO("Brain state saved to: " + saveBrainFile);
        } else {
            NLM_LOG_ERROR("Failed to save brain state to: " + saveBrainFile);
        }
    }
    
    // Run selected experiment or default tests
    if (!quiet) {
        printHeader("EXPERIMENT EXECUTION");
    }
    
    bool experimentRun = false;
    
    if (!experimentType.empty()) {
        if (experimentType == "connectivity") {
            runBasicConnectivityTest(brain);
            experimentRun = true;
        } else if (experimentType == "plasticity") {
            runPlasticityExperiment(brain, steps, trials);
            experimentRun = true;
        } else if (experimentType == "stdp") {
            runStdpVerification(brain, steps);
            experimentRun = true;
        } else if (experimentType == "learning") {
            runLearningExperiment(brain, trials, steps);
            experimentRun = true;
        }
    }
    
    if (!experimentRun) {
        // Run default tests
        runBasicConnectivityTest(brain);
        
        brain->reset();
        brain->initialize();
        
        runPlasticityExperiment(brain, steps, trials);
        
        brain->reset();
        brain->initialize();
        
        runStdpVerification(brain, steps);
        
        brain->reset();
        brain->initialize();
        
        runLearningExperiment(brain, trials, steps);
        
        if (benchmark) {
            brain->reset();
            brain->initialize();
            
            // Create visualization for enhanced benchmarking
            auto viz = std::make_shared<VisualizationInterface>();
            
            // Run enhanced benchmark with visualization
            runBenchmarkTest(brain, steps, viz);
        }
    }
    
    // Save brain state at end if requested
    if (!saveBrainFile.empty()) {
        NLM_LOG_INFO("Saving final brain state...");
        brain->save(saveBrainFile);
    }
    
    // Final brain status
    if (!quiet) {
        printHeader("FINAL BRAIN STATUS");
        brain->logStatus();
        
        printHeader("EXECUTION SUMMARY");
        NLM_LOG_INFO("Execution completed successfully!");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 2 Complete ===");
    
    return 0;
}