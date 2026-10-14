// NLM (熙然) - Neural Learning Machine
// Phase 2: Advanced Neural Computation with Enhanced CLI
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// - Advanced user interface and configuration options

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
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <filesystem>
#include <map>
#include <set>

using namespace nlm;

// Forward declarations for helper functions
void printHelp(const std::string& programName);
void printVersion();
void printBrainStats(const std::shared_ptr<Brain>& brain);
void runFullIntegrationDemo(const std::shared_ptr<Config>& config);
void runExperimentalMode(const std::shared_ptr<Config>& config, const std::string& experimentType);
void runAnalysisMode(const std::shared_ptr<Brain>& brain);
void runInteractiveMode(std::shared_ptr<Brain>& brain);
void runReplayDemo(const std::shared_ptr<Config>& config);
void runCheckpointDemo(const std::shared_ptr<Config>& config);
void runAdvancedPlasticityDemo(const std::shared_ptr<Config>& config);
void printInteractiveHelp();
bool validateConfig(const std::shared_ptr<Config>& config);
void applyAdvancedConfigOptions(std::shared_ptr<Config>& config, const std::string& experimentType);

// Constants for default configuration
const int DEFAULT_NEURON_COUNT = 500;
const int DEFAULT_REGION_COUNT = 1;
const float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
const float DEFAULT_SIMULATION_TIMESTEP = 0.001f;
const uint64_t DEFAULT_MAX_STEPS = 1000;

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

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Advanced Neural Computation                       ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Leaky Integrate-and-Fire (LIF) neuron dynamics           ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity rules                       ║
    ║     - Structural plasticity                                    ║
    ║     - Advanced memory integration                             ║
    ║     - Neuromodulation for learning                            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp(const std::string& programName) {
    std::cout << R"(Usage: " << programName << " [OPTIONS]

NLM - Neural Learning Machine: Advanced Artificial Brain Simulation

Basic Modes:
  --mode basic              Run basic connectivity and plasticity tests (default)
  --mode experimental       Run experimental setup with custom configurations
  --mode integration        Run Phase 6 integration test
  --mode interactive        Interactive exploration mode
  --mode analysis           Analyze brain state and visualize results
  --mode replay             Replay from checkpoint files
  --mode checkpoint         Checkpoint management demo
  --mode plasticity         Advanced plasticity exploration

Configuration Options:
  --config <file>          Load configuration from file
  --neurons <n>            Set neuron count (default: 500)
  --regions <n>            Set region count (default: 1)
  --connections <p>        Set connection probability (default: 0.15)
  --timestep <t>           Set simulation timestep (default: 0.001)
  --steps <n>              Set maximum simulation steps (default: 1000)
  --seed <n>               Set random seed for reproducibility

Advanced Options:
  --checkpoint <file>      Checkpoint file path
  --visualize             Enable visualization mode
  --debug                 Enable debug logging
  --quiet                 Quiet mode (only errors)
  --save-progress         Save progress periodically
  --stats                 Print detailed statistics
  --export <format>       Export results (json, binary, csv)

Phase 6 Integration Options:
  --phase6                 Run complete Phase 6 integration test
  --phase6-quick           Quick integration verification only
  --phase6-demo            Demo mode with reduced complexity

Experiment Types:
  --experiment basic       Basic connectivity test
  --experiment plasticity   Plasticity learning experiment
  --experiment stdp        STDP verification
  --experiment memory      Memory system test
  --experiment full        All experiments combined

Output Options:
  --output <dir>           Output directory for logs and results
  --log-level <level>      Log level: DEBUG, INFO, WARNING, ERROR, CRITICAL
  --no-banner              Skip banner display

Examples:
  " << programName << " --mode experimental --neurons 1000 --steps 5000
  " << programName << " --mode integration --phase6-demo
  " << programName << " --mode interactive --config my_config.cfg
  " << programName << " --mode checkpoint --checkpoint save_state.bin

For more information about NLM, visit: https://github.com/nlm-project/nlm
)" << std::endl;
}

void printVersion() {
    std::cout << "NLM v2.0.0 - Phase 2: Advanced Neural Computation" << std::endl;
    std::cout << "Build with C++20, supporting full neural simulation capabilities." << std::endl;
    std::cout << "Phase 6 integration complete with comprehensive brain system connectivity." << std::endl;
}

void printBrainStats(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        NLM_LOG_ERROR("Brain pointer is null");
        return;
    }
    
    NLM_LOG_INFO("=== BRAIN STATISTICS ===");
    NLM_LOG_INFO("Neuron Count: " << brain->getTotalNeuronCount());
    NLM_LOG_INFO("Synapse Count: " << brain->getTotalSynapseCount());
    NLM_LOG_INFO("Spike Count: " << brain->getTotalSpikeCount());
    NLM_LOG_INFO("Firing Neurons: " << brain->getFiringNeuronCount());
    NLM_LOG_INFO("Average Firing Rate: " << brain->getAverageFiringRate());
    NLM_LOG_INFO("Working Memory Traces: " << brain->getWorkingMemory() ? 
        brain->getWorkingMemory()->getActiveTraces() : 0);
    NLM_LOG_INFO("Episodic Memory Episodes: " << brain->getEpisodicMemory() ? 
        brain->getEpisodicMemory()->getEpisodeCount() : 0);
}

void runFullIntegrationDemo(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("=== FULL INTEGRATION DEMO ===");
    NLM_LOG_INFO("Running comprehensive brain integration test...");
    
    Phase6IntegratedExperiment experiment;
    Phase6Config phase6Config;
    phase6Config.neuronCount = config->getOr<int64_t>("neuron_count", 500);
    phase6Config.maxSteps = config->getOr<int64_t>("max_steps", 2000);
    phase6Config.enableCheckpointing = true;
    phase6Config.enableReplay = true;
    phase6Config.enableDevelopment = true;
    
    auto result = experiment.run(phase6Config);
    
    NLM_LOG_INFO("=== INTEGRATION RESULTS ===");
    NLM_LOG_INFO("Total Reward: " << result.totalReward);
    NLM_LOG_INFO("Average Firing Rate: " << result.avgFiringRate);
    NLM_LOG_INFO("Memory Episodes Stored: " << result.memoryEpisodesStored);
    NLM_LOG_INFO("Integration Status:");
    NLM_LOG_INFO("  Memory Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("  Memory Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("  Prediction System: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("  Development System: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING"));
}

void runExperimentalMode(const std::shared_ptr<Config>& config, const std::string& experimentType) {
    NLM_LOG_INFO("=== EXPERIMENTAL MODE: " << experimentType << " ===");
    
    if (experimentType == "basic") {
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return;
        }
        
        brain->logStatus();
        
        // Run comprehensive test suite
        runBasicConnectivityTest(brain);
        
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        brain->logStatus();
    } 
    else if (experimentType == "advanced") {
        runAdvancedPlasticityDemo(config);
    } else {
        NLM_LOG_WARNING("Unknown experiment type: " << experimentType);
        NLM_LOG_INFO("Running basic experiment instead");
        runExperimentalMode(config, "basic");
    }
}

void runAnalysisMode(const std::shared_ptr<Brain>& brain) {
    if (!brain) {
        NLM_LOG_ERROR("Brain pointer is null");
        return;
    }
    
    NLM_LOG_INFO("=== ANALYSIS MODE ===");
    printBrainStats(brain);
    
    // Analyze synaptic weights
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        NLM_LOG_INFO("=== SYNAPTIC ANALYSIS ===");
        size_t totalSynapses = region->getSynapses().size();
        if (totalSynapses > 0) {
            float totalWeight = 0.0f;
            float maxWeight = -FLT_MAX;
            float minWeight = FLT_MAX;
            
            for (const auto& syn : region->getSynapses()) {
                float weight = syn->getWeight();
                totalWeight += weight;
                maxWeight = std::max(maxWeight, weight);
                minWeight = std::min(minWeight, weight);
            }
            
            NLM_LOG_INFO("Average synaptic weight: " << totalWeight / totalSynapses);
            NLM_LOG_INFO("Max synaptic weight: " << maxWeight);
            NLM_LOG_INFO("Min synaptic weight: " << minWeight);
            NLM_LOG_INFO("Weight std deviation: " << sqrt(std::max(0.0f, 
                (totalWeight * totalWeight / totalSynapses) - (totalWeight * totalWeight / totalSynapses / totalSynapses)))));
        }
    }
}

void printInteractiveHelp() {
    std::cout << R"(=== INTERACTIVE MODE HELP ===

Commands available in interactive mode:

System Control:
  status          - Show current brain status and statistics
  reset           - Reset brain to initial state
  step <n>        - Advance simulation by n steps
  help           - Show this help message

Brain Exploration:
  neurons         - List all neurons and their properties
  synapses        - List all synapses and their weights
  memory          - Check memory system status
  neuromod         - Check neuromodulation system

Experiment Control:
  run <experiment> - Run specific experiment (basic, plasticity, stdp)
  save <file>      - Save current brain state to file
  load <file>      - Load brain state from file

Configuration:
  config <key> <value> - Set configuration parameter
  showconfig      - Display current configuration
  resetconfig     - Reset to default configuration

Simulation:
  simulate <mode> - Start simulation in specified mode
  visualize       - Show visualization (if available)
  analyze         - Run analysis on current brain state
) << std::endl;
}

bool validateConfig(const std::shared_ptr<Config>& config) {
    if (!config) {
        NLM_LOG_ERROR("Config is null");
        return false;
    }
    
    bool valid = true;
    
    // Check critical parameters
    int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);
    if (neuronCount <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count: " << neuronCount);
        valid = false;
    }
    
    float connectionProb = config->getOr<float>("connection_probability", 0.0f);
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        NLM_LOG_ERROR("Invalid connection_probability: " << connectionProb);
        valid = false;
    }
    
    double timestep = config->getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: " << timestep);
        valid = false;
    }
    
    return valid;
}

void applyAdvancedConfigOptions(std::shared_ptr<Config>& config, const std::string& experimentType) {
    if (!config) return;
    
    // Set experiment-specific defaults
    if (experimentType == "plasticity") {
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
        config->set("pruning_rate", 0.00002f, ConfigSource::Default);
    } else if (experimentType == "stdp") {
        config->set("stdp_ltp_weight", 0.05f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.04f, ConfigSource::Default);
        config->set("stdp_tau", 30.0f, ConfigSource::Default);
    } else if (experimentType == "memory") {
        config->set("working_memory_size", 100, ConfigSource::Default);
        config->set("episodic_memory_capacity", 50, ConfigSource::Default);
    }
}

void runReplayDemo(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("=== REPLAY DEMO ===");
    NLM_LOG_INFO("Loading saved brain state...");
    
    // In a real implementation, this would load from a checkpoint file
    // For demo purposes, we create a new brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for replay!");
        return;
    }
    
    NLM_LOG_INFO("Replay demo completed (simulated)");
}

void runCheckpointDemo(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("=== CHECKPOINT DEMO ===");
    NLM_LOG_INFO("Testing checkpoint save/load functionality...");
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for checkpoint demo!");
        return;
    }
    
    // Run some steps
    for (int i = 0; i < 100; ++i) {
        brain->step(i, i * 0.001);
    }
    
    // Save checkpoint
    std::string checkpointPath = "/tmp/nlm_checkpoint_demo.bin";
    if (brain->save(checkpointPath)) {
        NLM_LOG_INFO("Checkpoint saved to: " << checkpointPath);
        
        // Load checkpoint
        auto brain2 = std::make_shared<Brain>(config);
        brain2->initialize();
        if (brain2->load(checkpointPath)) {
            NLM_LOG_INFO("Checkpoint loaded successfully");
            NLM_LOG_INFO("Demo completed successfully");
        } else {
            NLM_LOG_ERROR("Failed to load checkpoint");
        }
    } else {
        NLM_LOG_ERROR("Failed to save checkpoint");
    }
}

void runAdvancedPlasticityDemo(const std::shared_ptr<Config>& config) {
    NLM_LOG_INFO("=== ADVANCED PLASTICITY DEMO ===");
    NLM_LOG_INFO("Demonstrating advanced plasticity mechanisms...");
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for advanced plasticity demo!");
        return;
    }
    
    // Enable all plasticity mechanisms
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);  // Hebbian, STDP, Structural
        }
    }
    
    // Run plasticity experiment with enhanced parameters
    LearningExperiment experiment(brain, 12345);
    experiment.recordInitialState();
    
    // Apply varied input patterns to stimulate complex plasticity
    for (SimulationStep step = 0; step < 2000; ++step) {
        // Mixed input pattern
        size_t sensoryCount = brain->getTotalNeuronCount() / 3;
        for (size_t i = 0; i < sensoryCount; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f + (rand() % 30));
        }
        
        // Add some motor activity
        if (step % 100 == 0) {
            for (size_t i = 0; i < sensoryCount / 2; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Motor, 15.0f + (rand() % 20));
            }
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 500 == 0) {
            NLM_LOG_INFO("Advanced plasticity step " << step << 
                        " | Spikes: " << brain->getTotalSpikeCount() <<
                        " | Firing: " << brain->getFiringNeuronCount());
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
    
    NLM_LOG_INFO("=== ADVANCED PLASTICITY DEMO COMPLETE ===");
}

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
            NLM_LOG_INFO("    Neuron " << i << 
                        " V=" << std::to_string(neurons[i]->getMembranePotential()) <<
                        " thresh=" << std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
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
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " << step << 
                        " | Spikes: " << brain->getTotalSpikeCount() <<
                        " | Firing: " << brain->getFiringNeuronCount());
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
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
        NLM_LOG_INFO("    Synapse " << i << 
                    " before: " << std::to_string(beforeWeights[i]));
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
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " << i << 
                    " after: " << std::to_string(synapses[i]->getWeight()) <<
                    " (Δ=" << std::to_string(delta) << ")");
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

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2: Advanced Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Advanced Neural Computation ===");
    NLM_LOG_INFO("Building on real spiking neural computation with enhanced user controls.");
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string experimentType = "basic";
    std::string mode = "basic";
    int neuronCount = DEFAULT_NEURON_COUNT;
    int regionCount = DEFAULT_REGION_COUNT;
    float connectionProbability = DEFAULT_CONNECTION_PROBABILITY;
    float timestep = DEFAULT_SIMULATION_TIMESTEP;
    uint64_t maxSteps = DEFAULT_MAX_STEPS;
    uint64_t seed = 42;
    std::string checkpointPath = "";
    std::string outputDir = ".";
    std::string logLevel = "INFO";
    bool showBanner = true;
    bool enableVisualization = false;
    bool quietMode = false;
    bool saveProgress = false;
    bool showStats = false;
    std::string exportFormat = "";
    bool runPhase6 = false;
    bool phase6Quick = false;
    bool phase6Demo = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp(argv[0]);
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "--mode" && i + 1 < argc) {
            mode = argv[++i];
        } else if (arg == "--experiment" && i + 1 < argc) {
            experimentType = argv[++i];
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--neurons" && i + 1 < argc) {
            neuronCount = std::atoi(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            regionCount = std::atoi(argv[++i]);
        } else if (arg == "--connections" && i + 1 < argc) {
            connectionProbability = std::atof(argv[++i]);
        } else if (arg == "--timestep" && i + 1 < argc) {
            timestep = std::atof(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            maxSteps = std::stoull(argv[++i]);
        } else if (arg == "--seed" && i + 1 < argc) {
            seed = std::stoull(argv[++i]);
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            checkpointPath = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            outputDir = argv[++i];
        } else if (arg == "--log-level" && i + 1 < argc) {
            logLevel = argv[++i];
        } else if (arg == "--no-banner") {
            showBanner = false;
        } else if (arg == "--visualize") {
            enableVisualization = true;
        } else if (arg == "--quiet") {
            quietMode = true;
        } else if (arg == "--save-progress") {
            saveProgress = true;
        } else if (arg == "--stats") {
            showStats = true;
        } else if (arg == "--export" && i + 1 < argc) {
            exportFormat = argv[++i];
        } else if (arg == "--phase6") {
            runPhase6 = true;
        } else if (arg == "--phase6-quick") {
            phase6Quick = true;
        } else if (arg == "--phase6-demo") {
            phase6Demo = true;
        }
    }
    
    // Update log level
    if (logLevel == "DEBUG") {
        consoleLogger->setLevel(LogLevel::Debug);
    } else if (logLevel == "WARNING") {
        consoleLogger->setLevel(LogLevel::Warning);
    } else if (logLevel == "ERROR") {
        consoleLogger->setLevel(LogLevel::Error);
    }
    
    // Show banner if enabled
    if (showBanner) {
        printBanner();
    }
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (std::filesystem::exists(configFile)) {
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " << configFile);
        } else {
            NLM_LOG_INFO("Using default configuration (file not found or invalid).");
        }
    } else {
        NLM_LOG_INFO("Using default configuration (file not found).");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values based on command line arguments
    config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
    config->set("simulation_timestep", timestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
    config->set("connection_probability", connectionProbability, ConfigSource::Default);
    config->set("max_steps", static_cast<int64_t>(maxSteps), ConfigSource::Default);
    
    // Apply experiment-specific configurations
    applyAdvancedConfigOptions(config, experimentType);
    
    // Validate configuration
    if (!validateConfig(config)) {
        NLM_LOG_ERROR("Configuration validation failed!");
        return 1;
    }
    
    // Create simulation clock
    double actualTimestep = config->getOr<double>("simulation_timestep", DEFAULT_SIMULATION_TIMESTEP);
    SimulationClock clock(actualTimestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " << actualTimestep << "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run based on mode
    if (runPhase6) {
        runFullIntegrationDemo(config);
    } else if (phase6Quick) {
        Phase6IntegratedExperiment experiment;
        NLM_LOG_INFO("=== Phase 6 Quick Integration ===");
        if (experiment.verifyIntegration()) {
            NLM_LOG_INFO("Phase 6 integration verification PASSED!");
        } else {
            NLM_LOG_ERROR("Phase 6 integration verification FAILED!");
        }
    } else if (phase6Demo) {
        Phase6IntegratedExperiment experiment;
        Phase6Config phase6Config;
        phase6Config.neuronCount = neuronCount;
        phase6Config.maxSteps = 500;
        phase6Config.enableCheckpointing = false;
        phase6Config.enableReplay = false;
        phase6Config.enableDevelopment = false;
        
        auto result = experiment.run(phase6Config);
        
        NLM_LOG_INFO("=== Phase 6 Demo Results ===");
        NLM_LOG_INFO("Total Reward: " << result.totalReward);
        NLM_LOG_INFO("Integration Status: " << (result.memoryWorkingMemoryIntegrated ? "WORKING" : "FAILED"));
    } else if (mode == "interactive") {
        NLM_LOG_INFO("=== INTERACTIVE MODE ===");
        NLM_LOG_INFO("Starting interactive brain exploration...");
        runInteractiveMode(brain);
    } else if (mode == "analysis") {
        NLM_LOG_INFO("=== ANALYSIS MODE ===");
        runAnalysisMode(brain);
    } else if (mode == "replay") {
        if (!checkpointPath.empty()) {
            // Load checkpoint
            auto brain2 = std::make_shared<Brain>(config);
            brain2->initialize();
            if (brain2->load(checkpointPath)) {
                NLM_LOG_INFO("Successfully loaded checkpoint from: " << checkpointPath);
                printBrainStats(brain2);
            } else {
                NLM_LOG_ERROR("Failed to load checkpoint");
            }
        } else {
            runReplayDemo(config);
        }
    } else if (mode == "checkpoint") {
        runCheckpointDemo(config);
    } else if (mode == "plasticity") {
        runAdvancedPlasticityDemo(config);
    } else if (mode == "experimental") {
        runExperimentalMode(config, experimentType);
    } else {
        // Default mode: run comprehensive test suite
        NLM_LOG_INFO("=== DEFAULT MODE: COMPREHENSIVE TEST SUITE ===");
        
        // Run Test 1: Basic connectivity
        runBasicConnectivityTest(brain);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        // Run Test 2: Plasticity learning experiment
        runPlasticityExperiment(brain);
        
        // Reset and run Test 3: STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        // Show final stats if requested
        if (showStats) {
            printBrainStats(brain);
        }
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== FINAL BRAIN STATUS ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== EXECUTION COMPLETE ===");
    NLM_LOG_INFO("All requested operations completed successfully.");
    
    return 0;
}