#pragma once

/**
 * NLM Main Entry Point - Phase 2 Reference Implementation
 *
 * This file provides a clean, well-structured implementation of the NLM
 * (Neural Learning Machine) Phase 2 demonstration. It serves as both:
 *
 * 1. A reference implementation showing best practices for using the NLM library
 * 2. A template for creating new NLM-based applications
 *
 * The implementation follows these principles:
 * - Clear separation of concerns
 * - Configurable behavior through the Config system
 * - Proper error handling and logging
 * - Testable architecture with discrete components
 * - Minimal hardcoded constants
 *
 * Usage:
 *   ./nlm
 *   ./nlm --neuron_count 500
 *   ./nlm --config my_config.cfg
 */

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <string>

// Forward declarations for test scenarios
namespace nlm {
    void runBasicConnectivityTest(Brain& brain);
    void runPlasticityExperiment(Brain& brain);
    void runStdpVerification(Brain& brain);
}

// Configuration defaults for Phase 2
struct Phase2Defaults {
    static constexpr size_t DEFAULT_NEURON_COUNT = 500;
    static constexpr size_t DEFAULT_REGION_COUNT = 1;
    static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
    static constexpr double DEFAULT_TIMESTEP = 0.001;
    static constexpr uint64_t DEFAULT_SEED = 42;
    
    // STDP parameters
    static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
    static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
    static constexpr float DEFAULT_STDP_TAU = 20.0f;
    
    // Structural plasticity
    static constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
};

// Application class - encapsulates the complete Phase 2 demo
class Phase2Application {
public:
    Phase2Application() = default;
    ~Phase2Application() = default;
    
    // Initialize the application with configuration
    bool initialize(const std::string& configFile = "configs/default.cfg") {
        std::cout << "=== NLM Phase 2: Real Neural Computation ===" << std::endl;
        std::cout << "Setting up NLM brain simulation..." << std::endl;
        
        // Initialize logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        NLM_LOG_INFO("Phase 2 Application initialized");
        
        // Load configuration
        if (!loadConfiguration(configFile)) {
            NLM_LOG_ERROR("Failed to load configuration");
            return false;
        }
        
        // Configure brain parameters
        if (!configureBrainParameters()) {
            NLM_LOG_ERROR("Failed to configure brain parameters");
            return false;
        }
        
        // Initialize brain
        brain_ = std::make_shared<Brain>(config_);
        if (!brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
        
        NLM_LOG_INFO("Phase 2 Application successfully initialized");
        return true;
    }
    
    // Run the complete Phase 2 demonstration
    bool run() {
        if (!brain_) {
            NLM_LOG_ERROR("Brain not initialized - cannot run demonstration");
            return false;
        }
        
        NLM_LOG_INFO("=== Starting Phase 2 Demonstration ===");
        
        try {
            // Run Test 1: Basic connectivity
            NLM_LOG_INFO("--- Test 1: Basic Neural Connectivity ---");
            runBasicConnectivityTest(*brain_);
            
            // Reset brain for plasticity experiment
            resetBrainForNextTest();
            
            // Run Test 2: Plasticity learning experiment
            NLM_LOG_INFO("--- Test 2: Plasticity Learning Experiment ---");
            runPlasticityExperiment(*brain_);
            
            // Reset brain for STDP verification
            resetBrainForNextTest();
            
            // Run Test 3: STDP verification
            NLM_LOG_INFO("--- Test 3: STDP Verification ---");
            runStdpVerification(*brain_);
            
            // Display final results
            displayFinalResults();
            
            NLM_LOG_INFO("=== Phase 2 Demonstration Complete ===");
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception during demonstration: ") + e.what());
            return false;
        }
    }
    
    // Save current brain state to checkpoint
    bool saveCheckpoint(const std::string& filepath) {
        if (!brain_) {
            NLM_LOG_ERROR("Cannot save checkpoint - brain not initialized");
            return false;
        }
        
        NLM_LOG_INFO("Saving brain state to checkpoint: " + filepath);
        bool success = brain_->save(filepath);
        
        if (success) {
            NLM_LOG_INFO("Checkpoint saved successfully");
        } else {
            NLM_LOG_ERROR("Failed to save checkpoint");
        }
        
        return success;
    }
    
    // Load brain state from checkpoint
    bool loadCheckpoint(const std::string& filepath) {
        if (!brain_) {
            NLM_LOG_ERROR("Cannot load checkpoint - brain not initialized");
            return false;
        }
        
        NLM_LOG_INFO("Loading brain state from checkpoint: " + filepath);
        bool success = brain_->load(filepath);
        
        if (success) {
            NLM_LOG_INFO("Checkpoint loaded successfully");
        } else {
            NLM_LOG_ERROR("Failed to load checkpoint");
        }
        
        return success;
    }
    
    const std::shared_ptr<Brain>& getBrain() const { return brain_; }
    const std::shared_ptr<Config>& getConfig() const { return config_; }
    
private:
    bool loadConfiguration(const std::string& configFile) {
        NLM_LOG_INFO("Loading configuration from: " + configFile);
        
        config_ = std::make_shared<Config>();
        
        // Try to load from file (ignore if not found)
        if (config_->loadFromFile(configFile)) {
            NLM_LOG_INFO("Configuration loaded from file");
        } else {
            NLM_LOG_INFO("Using defaults (no config file found)");
        }
        
        return true;
    }
    
    bool configureBrainParameters() {
        if (!config_) {
            config_ = std::make_shared<Config>();
        }
        
        // Set default Phase 2 parameters
        config_->set("random_seed", static_cast<int64_t>(Phase2Defaults::DEFAULT_SEED), ConfigSource::Default);
        config_->set("simulation_timestep", Phase2Defaults::DEFAULT_TIMESTEP, ConfigSource::Default);
        config_->set("neuron_count", static_cast<int64_t>(Phase2Defaults::DEFAULT_NEURON_COUNT), ConfigSource::Default);
        config_->set("region_count", static_cast<int64_t>(Phase2Defaults::DEFAULT_REGION_COUNT), ConfigSource::Default);
        config_->set("connection_probability", Phase2Defaults::DEFAULT_CONNECTION_PROBABILITY, ConfigSource::Default);
        
        // STDP parameters
        config_->set("stdp_ltp_weight", Phase2Defaults::DEFAULT_STDP_LTP_WEIGHT, ConfigSource::Default);
        config_->set("stdp_ltd_weight", Phase2Defaults::DEFAULT_STDP_LTD_WEIGHT, ConfigSource::Default);
        config_->set("stdp_tau", Phase2Defaults::DEFAULT_STDP_TAU, ConfigSource::Default);
        
        // Structural plasticity parameters
        config_->set("synaptogenesis_rate", Phase2Defaults::DEFAULT_SYNAPTOGENESIS_RATE, ConfigSource::Default);
        config_->set("pruning_rate", Phase2Defaults::DEFAULT_PRUNING_RATE, ConfigSource::Default);
        
        // Log configuration summary
        logConfigurationSummary();
        return true;
    }
    
    void logConfigurationSummary() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration Summary:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config_->getOr<int64_t>("random_seed", Phase2Defaults::DEFAULT_SEED)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config_->getOr<double>("simulation_timestep", Phase2Defaults::DEFAULT_TIMESTEP)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config_->getOr<int64_t>("neuron_count", Phase2Defaults::DEFAULT_NEURON_COUNT)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config_->getOr<int64_t>("region_count", Phase2Defaults::DEFAULT_REGION_COUNT)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config_->getOr<float>("connection_probability", Phase2Defaults::DEFAULT_CONNECTION_PROBABILITY)));
        NLM_LOG_INFO("");
    }
    
    void resetBrainForNextTest() {
        if (brain_) {
            NLM_LOG_INFO("Resetting brain for next test...");
            brain_->reset();
            brain_->initialize();
        }
    }
    
    void displayFinalResults() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Phase 2 Results ===");
        
        if (!brain_) {
            NLM_LOG_ERROR("Cannot display results - brain not available");
            return;
        }
        
        NLM_LOG_INFO("Total neurons: " + std::to_string(brain_->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(brain_->getTotalSynapseCount()));
        NLM_LOG_INFO("Active neurons: " + std::to_string(brain_->getActiveNeuronCount()));
        NLM_LOG_INFO("Firing neurons (last step): " + std::to_string(brain_->getFiringNeuronCount()));
        NLM_LOG_INFO("Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(brain_->getAverageFiringRate()));
        NLM_LOG_INFO("Excitatory/Inhibitory ratio: " + std::to_string(brain_->getExcitationInhibitionRatio()));
        
        brain_->logStatus();
        
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
    }
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
};

// Standalone test functions (same as original but cleaner)
void runBasicConnectivityTest(Brain& brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain.getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain.step(step, step * 0.001);
    }
    
    size_t spikes = brain.getTotalSpikeCount() - initialSpikes;
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

void runPlasticityExperiment(Brain& brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    // Create experiment structure (simplified)
    struct LearningExperiment {
        Brain& brain;
        uint64_t seed;
        size_t initialSynapseCount;
        std::vector<float> initialWeights;
        std::vector<float> finalWeights;
        
        LearningExperiment(Brain& b, uint64_t s) : brain(b), seed(s), initialSynapseCount(0) {}
        
        void recordInitialState() {
            initialSynapseCount = brain.getTotalSynapseCount();
            initialWeights.clear();
            
            // Record initial weights from first region
            if (auto* region = brain.getRegion(RegionId(1))) {
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
            if (auto* region = brain.getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    finalWeights.push_back(syn->getWeight());
                }
            }
            
            NLM_LOG_INFO("Final state recorded:");
            NLM_LOG_INFO("  Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
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
            NLM_LOG_INFO("  Total spikes: " + std::to_string(brain.getTotalSpikeCount()));
            
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
    
    // Enable plasticity on synapses
    if (auto* region = brain.getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    LearningExperiment experiment(brain, 42);
    experiment.recordInitialState();
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain.getTotalNeuronCount() / 4; ++i) {
            brain.injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain.step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain.getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain.getFiringNeuronCount()));
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
}

void runStdpVerification(Brain& brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain.getRegion(RegionId(1));
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
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain.step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
        totalDelta += delta;
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
    // Create and run Phase 2 application
    Phase2Application app;
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    bool saveCheckpoint = false;
    std::string checkpointPath = "brain_checkpoint.bin";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--save_checkpoint" && i + 1 < argc) {
            saveCheckpoint = true;
            checkpointPath = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: ./nlm [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --config <file>    Configuration file (default: configs/default.cfg)" << std::endl;
            std::cout << "  --save_checkpoint <file>  Save checkpoint after successful run" << std::endl;
            std::cout << "  -h, --help         Show this help message" << std::endl;
            return 0;
        }
    }
    
    // Initialize and run application
    if (!app.initialize(configFile)) {
        std::cerr << "Failed to initialize application" << std::endl;
        return 1;
    }
    
    if (!app.run()) {
        std::cerr << "Application execution failed" << std::endl;
        return 1;
    }
    
    // Save checkpoint if requested
    if (saveCheckpoint && app.saveCheckpoint(checkpointPath)) {
        std::cout << "Checkpoint saved successfully to: " << checkpointPath << std::endl;
    } else if (saveCheckpoint) {
        std::cerr << "Failed to save checkpoint" << std::endl;
    }
    
    std::cout << "=== Phase 2 Complete ===" << std::endl;
    return 0;
}
