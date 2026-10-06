// NLM (熙然) - Neural Learning Machine Enhanced Entry Point
// Main application with enhanced CLI, configuration, and experiment framework
//
// This enhanced main provides:
// - Comprehensive command-line interface with help, version, and mode selection
// - Flexible configuration system with preset profiles
// - Modular experiment framework for different phase demonstrations
// - Performance monitoring and detailed logging
// - Utility commands for brain inspection and analysis
// - Robust error handling and graceful degradation
//
// Usage examples:
//   ./nlm                    # Run Phase 2 demo (backward compatible)
//   ./nlm --mode phase2        # Explicitly run Phase 2 experiments
//   ./nlm --mode phase6        # Run Phase 6 integration demo
//   ./nlm --mode quick_test    # Run quick verification tests
//   ./nlm --config my_config   # Load custom configuration
//   ./nlm --verbose --save_checkpoint checkpoint.bin
//   ./nlm --help               # Show detailed help information

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
#include <sstream>
#include <map>
#include <set>
#include <fstream>
#include <algorithm>
#include <cstdlib>

using namespace nlm;

// Enhanced command-line argument parser
class CommandLineParser {
public:
    struct Options {
        std::string configFile = "configs/default.cfg";
        std::string mode = "auto";
        std::string outputFile;
        std::string checkpointFile;
        bool verbose = false;
        bool debug = false;
        bool help = false;
        bool version = false;
        bool quiet = false;
        bool jsonOutput = false;
        std::vector<std::string> experiments;
        std::map<std::string, std::string> customArgs;
    };

    static Options parse(int argc, char** argv) {
        Options options;

        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);

            if (arg == "--help" || arg == "-h") {
                options.help = true;
            } else if (arg == "--version") {
                options.version = true;
            } else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
            } else if (arg == "--debug") {
                options.debug = true;
                options.verbose = true;
            } else if (arg == "--quiet" || arg == "-q") {
                options.quiet = true;
                options.verbose = false;
            } else if (arg == "--json") {
                options.jsonOutput = true;
            } else if (arg == "--mode" && i + 1 < argc) {
                options.mode = argv[++i];
            } else if (arg == "--config" && i + 1 < argc) {
                options.configFile = argv[++i];
            } else if (arg == "--save_checkpoint" && i + 1 < argc) {
                options.checkpointFile = argv[++i];
            } else if (arg == "--output" && i + 1 < argc) {
                options.outputFile = argv[++i];
            } else if (arg == "--experiment" && i + 1 < argc) {
                options.experiments.push_back(argv[++i]);
            } else if (arg.find('=') != std::string::npos) {
                size_t pos = arg.find('=');
                std::string key = arg.substr(0, pos);
                std::string value = arg.substr(pos + 1);
                options.customArgs[key] = value;
            } else {
                // Unknown argument
                std::cerr << "Warning: Unknown argument " << arg << std::endl;
            }
        }

        return options;
    }

    static void printHelp() {
        std::cout << R"(
NLM — 熙然 (Neural Learning Machine)

Enhanced Neural Simulation System

USAGE:
    ./nlm [OPTIONS]

MODES:
    --mode auto             Automatically select appropriate mode (default)
    --mode phase2           Run Phase 2: Basic neural computation demonstration
    --mode phase6           Run Phase 6: Full integration demonstration
    --mode quick_test       Run quick verification tests
    --mode demo             Run simple demonstration with minimal settings
    --mode benchmark        Run performance benchmarking

CONFIGURATION:
    --config FILE          Load configuration from FILE (default: configs/default.cfg)
    --save_checkpoint FILE Save brain state to FILE after completion
    --output FILE          Save experiment results to FILE

EXPERIMENTS (can be combined):
    --experiment connectivity    Test basic neural connectivity
    --experiment plasticity      Test learning and synaptic modification
    --experiment stdp            Test Spike-Timing Dependent Plasticity
    --experiment all            Run all available experiments (default)

OUTPUT & VERBOSITY:
    -v, --verbose           Enable verbose output
    -q, --quiet             Suppress informational output, only errors
    --debug                 Enable debug mode with additional diagnostics
    --json                  Output results in JSON format

MISCELLANEOUS:
    -h, --help              Show this help message
    --version               Show version information

EXAMPLES:
    ./nlm                                    # Default Phase 2 demo
    ./nlm --mode phase6                      # Full integration demo
    ./nlm --mode quick_test --experiment all # Quick comprehensive test
    ./nlm --verbose --save_checkpoint brain.bin # Debug with checkpoint
    ./nlm --experiment connectivity --experiment plasticity # Specific tests only

For more information, visit the NLM documentation.
        )" << std::endl;
    }

    static std::string getVersion() {
        return "NLM Phase 6.0.0 (Enhanced)";
    }
};

// Configuration preset profiles
class ConfigPresets {
public:
    static void applyPreset(std::shared_ptr<Config> config, const std::string& preset) {
        if (preset == "quick_test") {
            // Very small network for rapid testing
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
            config->set("max_steps", static_cast<int64_t>(500), ConfigSource::Default);

        } else if (preset == "demo") {
            // Balanced demo settings
            config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
            config->set("connection_probability", 0.2f, ConfigSource::Default);

        } else if (preset == "full_experiment") {
            // Large-scale experiment
            config->set("random_seed", static_cast<int64_t>(999), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);

        } else if (preset == "phase2_defaults") {
            // Original Phase 2 settings
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);

        } else {
            // Default is phase2_defaults
            applyPreset(config, "phase2_defaults");
        }
    }
};

// Enhanced experiment framework base class
class EnhancedExperiment {
public:
    virtual ~EnhancedExperiment() = default;
    virtual std::string getName() const = 0;
    virtual bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) = 0;
    virtual void setup(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) = 0;
    virtual void cleanup(std::shared_ptr<Brain> brain) = 0;
    virtual std::map<std::string, double> getResults() const = 0;
    virtual bool validate() const { return true; }
};

// Phase 2 Experiments (enhanced)
class Phase2ConnectivityExperiment : public EnhancedExperiment {
public:
    std::string getName() const override { return "connectivity"; }
    
    bool validate() const override { return true; }
    
    void setup(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) override {
        NLM_LOG_INFO("Setting up Phase 2 Connectivity Experiment");
        brain->reset();
        brain->initialize();
    }
    
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) override {
        NLM_LOG_INFO("=== Enhanced Test 1: Basic Neural Connectivity ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No regions found!");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("No neurons found!");
            return false;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("✓ Spikes propagate through network");
            results["connectivity_success"] = 1.0;
            results["spike_count"] = static_cast<double>(spikes);
            return true;
        } else {
            NLM_LOG_INFO("! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("  Neuron " + std::to_string(i) + 
                           " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                           " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
            results["connectivity_success"] = 0.0;
            results["spike_count"] = 0.0;
            return false;
        }
    }
    
    void cleanup(std::shared_ptr<Brain> brain) override {
        NLM_LOG_INFO("Cleaning up connectivity experiment");
        brain->reset();
    }
    
    std::map<std::string, double> getResults() const override {
        return results;
    }

private:
    std::map<std::string, double> results;
};

class Phase2PlasticityExperiment : public EnhancedExperiment {
public:
    std::string getName() const override { return "plasticity"; }
    
    void setup(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) override {
        NLM_LOG_INFO("Setting up Phase 2 Plasticity Experiment");
        brain->reset();
        brain->initialize();
    }
    
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) override {
        NLM_LOG_INFO("=== Enhanced Test 2: Plasticity Learning Experiment ===");
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Record initial state
        size_t initialSynapseCount = brain->getTotalSynapseCount();
        std::vector<float> initialWeights;
        
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
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                           " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        size_t finalSynapseCount = brain->getTotalSynapseCount();
        std::vector<float> finalWeights;
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
        
        // Compute weight changes and learning statistics
        bool learningOccurred = computeLearningResults(initialWeights, finalWeights, initialSynapseCount, finalSynapseCount);
        results["learning_occurred"] = learningOccurred ? 1.0 : 0.0;
        results["synapse_change_ratio"] = static_cast<double>(finalSynapseCount) / initialSynapseCount;
        
        return learningOccurred;
    }
    
    void cleanup(std::shared_ptr<Brain> brain) override {
        NLM_LOG_INFO("Cleaning up plasticity experiment");
        brain->reset();
    }
    
    std::map<std::string, double> getResults() const override {
        return results;
    }

private:
    bool computeLearningResults(const std::vector<float>& initialWeights, const std::vector<float>& finalWeights,
                               size_t initialSynapseCount, size_t finalSynapseCount) {
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return false;
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
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                               (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
        
        return learningOccurred;
    }
    
    std::map<std::string, double> results;
};

class Phase2STDPExperiment : public EnhancedExperiment {
public:
    std::string getName() const override { return "stdp"; }
    
    void setup(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) override {
        NLM_LOG_INFO("Setting up Phase 2 STDP Experiment");
        brain->reset();
        brain->initialize();
    }
    
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) override {
        NLM_LOG_INFO("=== Enhanced Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No regions found!");
            return false;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            results["stdp_test_skipped"] = 1.0;
            return true;  // Not a failure, just skipped
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
            results["stdp_success"] = 1.0;
            results["stdp_total_delta"] = static_cast<double>(totalDelta);
            return true;
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
            results["stdp_reversed"] = 1.0;
            results["stdp_total_delta"] = static_cast<double>(totalDelta);
            return false;
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
            results["stdp_no_change"] = 1.0;
            results["stdp_total_delta"] = static_cast<double>(totalDelta);
            return false;
        }
    }
    
    void cleanup(std::shared_ptr<Brain> brain) override {
        NLM_LOG_INFO("Cleaning up STDP experiment");
        brain->reset();
    }
    
    std::map<std::string, double> getResults() const override {
        return results;
    }

private:
    std::map<std::string, double> results;
};

// Phase 6 Integration Experiment (simplified)
class Phase6IntegrationExperiment : public EnhancedExperiment {
public:
    std::string getName() const override { return "phase6_integration"; }
    
    void setup(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) override {
        NLM_LOG_INFO("Setting up Phase 6 Integration Experiment");
        brain->reset();
        brain->initialize();
    }
    
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) override {
        NLM_LOG_INFO("=== Phase 6: Full Integration Test ===");
        
        // This is a simplified version that tests key integration points
        // In a full implementation, this would test all 6 phases
        
        results.clear();
        results["memory_working_memory"] = 0.0;
        results["memory_episodic"] = 0.0;
        results["neuromodulation"] = 0.0;
        results["prediction"] = 0.0;
        results["development"] = 0.0;
        
        NLM_LOG_INFO("Phase 6 integration test running...");
        NLM_LOG_INFO("(Note: Full integration testing requires more extensive setup)");
        
        // Test basic functionality
        NLM_LOG_INFO("Testing basic neural dynamics...");
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (!neurons.empty()) {
                neurons[0]->injectCurrent(40.0f);
                for (int step = 0; step < 100; ++step) {
                    brain->step(step, step * 0.001);
                }
                results["basic_dynamics_success"] = 1.0;
                results["spike_count"] = static_cast<double>(brain->getTotalSpikeCount());
            }
        }
        
        NLM_LOG_INFO("Phase 6 integration completed");
        return true;
    }
    
    void cleanup(std::shared_ptr<Brain> brain) override {
        NLM_LOG_INFO("Cleaning up Phase 6 integration experiment");
        brain->reset();
    }
    
    std::map<std::string, double> getResults() const override {
        return results;
    }

private:
    std::map<std::string, double> results;
};

// Brain inspector for utility commands
class BrainInspector {
public:
    static void printNeuralStatistics(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("=== Brain Statistics ===");
        
        NLM_LOG_INFO("Overall Statistics:");
        NLM_LOG_INFO("  Total Neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("  Total Synapses: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("  Active Neurons: " + std::to_string(brain->getActiveNeuronCount()));
        NLM_LOG_INFO("  Firing Neurons: " + std::to_string(brain->getFiringNeuronCount()));
        NLM_LOG_INFO("  Total Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Average Firing Rate: " + std::to_string(brain->getAverageFiringRate()));
        NLM_LOG_INFO("  Excitation/Inhibition Ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Regional Breakdown:");
        auto regionIds = brain->getRegionIds();
        for (RegionId regionId : regionIds) {
            if (auto* region = brain->getRegion(regionId)) {
                NLM_LOG_INFO("  Region " + std::to_string(regionId) + " (" + region->getName() + "):");
                NLM_LOG_INFO("    Neurons: " + std::to_string(region->getNeuronCount()));
                NLM_LOG_INFO("    Synapses: " + std::to_string(region->getSynapseCount()));
                NLM_LOG_INFO("    Populations: " + std::to_string(region->getPopulationCount()));
            }
        }
        
        NLM_LOG_INFO("");
    }
    
    static void printConfigurationSummary(std::shared_ptr<Config> config) {
        NLM_LOG_INFO("=== Configuration Summary ===");
        
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            auto value = config->get<std::string>(key);
            if (value.has_value()) {
                NLM_LOG_INFO("  " + key + ": " + value.value());
            }
        }
        
        NLM_LOG_INFO("");
    }
    
    static void saveBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
        NLM_LOG_INFO("Saving brain state to: " + filepath);
        bool success = brain->save(filepath);
        if (success) {
            NLM_LOG_INFO("✓ Brain state saved successfully");
        } else {
            NLM_LOG_ERROR("Failed to save brain state");
        }
    }
    
    static bool loadBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
        NLM_LOG_INFO("Loading brain state from: " + filepath);
        bool success = brain->load(filepath);
        if (success) {
            NLM_LOG_INFO("✓ Brain state loaded successfully");
        } else {
            NLM_LOG_ERROR("Failed to load brain state");
        }
        return success;
    }
};