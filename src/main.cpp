// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#pragma once

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
#include <functional>

namespace nlm {

// Forward declarations
class Config;
class Brain;
class Logger;

// Namespace for main-related utilities
namespace main {

// Logging level for simulation output
enum class SimulationVerbosity {
    Quiet = 0,
    Summary = 1,
    Detailed = 2,
    Debug = 3
};

// Configuration presets for different simulation scenarios
struct SimulationPresets {
    static Config createBasicConfig() {
        auto config = std::make_shared<Config>();
        
        // Basic parameters
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("simulation_timestep", 0.001);
        config->set("random_seed", static_cast<int64_t>(42));
        
        // Connectivity
        config->set("connection_probability", 0.15f);
        
        // Plasticity parameters
        config->set("stdp_ltp_weight", 0.02f);
        config->set("stdp_ltd_weight", 0.015f);
        config->set("stdp_tau", 20.0f);
        
        // Structural plasticity
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        return config;
    }
    
    static Config createLearningConfig() {
        auto config = createBasicConfig();
        
        // Enhanced learning parameters
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("stdp_ltp_weight", 0.03f);
        config->set("stdp_ltd_weight", 0.02f);
        config->set("stdp_tau", 25.0f);
        config->set("synaptogenesis_rate", 0.0002f);
        
        return config;
    }
    
    static Config createFastConfig() {
        auto config = createBasicConfig();
        
        // Faster, smaller simulation
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("stdp_tau", 10.0f);
        config->set("simulation_timestep", 0.002);
        
        return config;
    }
    
    static Config loadFromFile(const std::string& filename) {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(filename)) {
            return config;
        }
        return createBasicConfig();
    }
};

// Callback function type for simulation events
using SimulationCallback = std::function<void(SimulationStep, double, const Brain&)>;

// Simulation runner class
class SimulationRunner {
public:
    SimulationRunner(std::shared_ptr<Brain> brain, SimulationVerbosity verbosity = SimulationVerbosity::Summary)
        : brain_(brain), verbosity_(verbosity) {}
    
    // Run simulation with logging
    void run(SimulationStep steps, bool logProgress = false) {
        NLM_LOG_INFO("Starting simulation for " + std::to_string(steps) + " steps...");
        
        for (SimulationStep step = 0; step < steps; ++step) {
            Timestamp time = step * brain_->getConfig()->getOr<double>("simulation_timestep", 0.001);
            brain_->step(step, time);
            
            if (logProgress && (step % 100 == 0 || step == steps - 1)) {
                logProgress(step, time);
            }
        }
        
        NLM_LOG_INFO("Simulation completed.");
    }
    
    // Log progress to console
    void logProgress(SimulationStep step, Timestamp time) const {
        if (verbosity_ >= SimulationVerbosity::Summary) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Time: " + std::to_string(time) + "s" +
                        " | Spikes: " + std::to_string(brain_->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain_->getFiringNeuronCount()));
        }
        
        if (verbosity_ >= SimulationVerbosity::Debug) {
            brain_->logStatus();
        }
    }
    
    // Inject current pattern to a specific neuron type
    void injectCurrentPattern(NeuronType type, const std::vector<float>& pattern) {
        NLM_LOG_INFO("Injecting " + std::string(typeToString(type)) + " current pattern...");
        
        float currentStep = 0.0f;
        for (float current : pattern) {
            brain_->injectCurrentToNeurons(type, current);
            brain_->step(currentStep, currentStep * 0.001);
            currentStep++;
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
    SimulationVerbosity verbosity_;
    
    std::string typeToString(NeuronType type) const {
        switch (type) {
            case NeuronType::Excitatory: return "excitatory";
            case NeuronType::Inhibitory: return "inhibitory";
            case NeuronType::Modulatory: return "modulatory";
            case NeuronType::Sensory: return "sensory";
            case NeuronType::Motor: return "motor";
            case NeuronType::Internal: return "internal";
            default: return "unknown";
        }
    }
};

// Brain state recorder for experiment tracking
class BrainStateRecorder {
public:
    struct RecordedState {
        size_t step;
        size_t neuronCount;
        size_t synapseCount;
        size_t spikeCount;
        size_t firingCount;
        float averageWeight;
        std::vector<float> weights;
        double time;
    };
    
    BrainStateRecorder(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    void recordState(SimulationStep step) {
        RecordedState state;
        state.step = step;
        state.neuronCount = brain_->getTotalNeuronCount();
        state.synapseCount = brain_->getTotalSynapseCount();
        state.spikeCount = brain_->getTotalSpikeCount();
        state.firingCount = brain_->getFiringNeuronCount();
        
        // Calculate average weight
        state.averageWeight = 0.0f;
        state.weights.clear();
        
        if (auto* region = brain_->getRegion(RegionId(1))) {
            size_t synapseCount = 0;
            float totalWeight = 0.0f;
            for (const auto& syn : region->getSynapses()) {
                state.weights.push_back(syn->getWeight());
                totalWeight += syn->getWeight();
                synapseCount++;
            }
            state.averageWeight = synapseCount > 0 ? totalWeight / synapseCount : 0.0f;
        }
        
        state.time = step * brain_->getConfig()->getOr<double>("simulation_timestep", 0.001);
        
        states_.push_back(state);
        
        NLM_LOG_DEBUG("Recorded state at step " + std::to_string(step) +
                     ": neurons=" + std::to_string(neuronCount) +
                     ", synapses=" + std::to_string(synapseCount) +
                     ", spikes=" + std::to_string(spikeCount));
    }
    
    const std::vector<RecordedState>& getStates() const { return states_; }
    void clear() { states_.clear(); }
    
    // Calculate learning metrics
    struct LearningMetrics {
        float weightChange;
        size_t strengthened;
        size_t weakened;
        size_t unchanged;
        bool learningOccurred;
    };
    
    LearningMetrics calculateLearning() const {
        if (states_.size() < 2) {
            return {0.0f, 0, 0, 0, false};
        }
        
        const auto& initial = states_.front();
        const auto& final = states_.back();
        
        LearningMetrics metrics;
        
        if (!initial.weights.empty() && !final.weights.empty()) {
            float initialSum = std::accumulate(initial.weights.begin(), initial.weights.end(), 0.0f);
            float finalSum = std::accumulate(final.weights.begin(), final.weights.end(), 0.0f);
            metrics.weightChange = final.averageWeight - initial.averageWeight;
            
            size_t minSize = std::min(initial.weights.size(), final.weights.size());
            for (size_t i = 0; i < minSize; ++i) {
                float delta = final.weights[i] - initial.weights[i];
                if (delta > 0.01f) metrics.strengthened++;
                else if (delta < -0.01f) metrics.weakened++;
                else metrics.unchanged++;
            }
        }
        
        metrics.learningOccurred = (std::abs(metrics.weightChange) > 0.001f) ||
                                  (metrics.strengthened > 0 || metrics.weakened > 0);
        
        return metrics;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::vector<RecordedState> states_;
};

// Command-line argument parser
class ArgumentParser {
public:
    struct ParsedArgs {
        std::string configFile;
        SimulationVerbosity verbosity;
        size_t steps;
        bool enableLearning;
        bool enableStdp;
        bool enableStructuralPlasticity;
        std::string experimentType;
    };
    
    static ParsedArgs parse(int argc, char** argv) {
        ParsedArgs args;
        args.verbosity = SimulationVerbosity::Summary;
        args.steps = 1000;
        args.enableLearning = true;
        args.enableStdp = true;
        args.enableStructuralPlasticity = true;
        args.experimentType = "comprehensive";
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--quiet") {
                args.verbosity = SimulationVerbosity::Quiet;
            } else if (arg == "--debug") {
                args.verbosity = SimulationVerbosity::Debug;
            } else if (arg == "--steps" && i + 1 < argc) {
                args.steps = std::stoull(argv[++i]);
            } else if (arg == "--no-learning") {
                args.enableLearning = false;
            } else if (arg == "--no-stdp") {
                args.enableStdp = false;
            } else if (arg == "--no-structural") {
                args.enableStructuralPlasticity = false;
            } else if (arg.substr(0, 9) == "--config=") {
                args.configFile = arg.substr(9);
            } else if (arg == "--experiment" && i + 1 < argc) {
                args.experimentType = argv[++i];
            } else if (arg == "--help" || arg == "-h") {
                printHelp();
                exit(0);
            }
        }
        
        return args;
    }
    
private:
    static void printHelp() {
        std::cout << "NLM Simulation Options:" << std::endl;
        std::cout << "  --config=file.cfg     Load configuration from file" << std::endl;
        std::cout << "  --steps=N            Number of simulation steps (default: 1000)" << std::endl;
        std::cout << "  --quiet              Minimal output" << std::endl;
        std::cout << "  --debug              Verbose output" << std::endl;
        std::cout << "  --no-learning        Disable plasticity" << std::endl;
        std::cout << "  --no-stdp            Disable STDP" << std::endl;
        std::cout << "  --no-structural      Disable structural plasticity" << std::endl;
        std::cout << "  --experiment=type    Experiment type (basic|learning|stdp|comprehensive)" << std::endl;
        std::cout << "  -h, --help           Show this help message" << std::endl;
    }
};

// Print formatted simulation summary
void printSimulationSummary(const Brain& brain, const std::string& title) {
    std::cout << "\n" << title << std::endl;
    std::cout << std::string(title.length(), '=') << std::endl;
    std::cout << "Network Statistics:" << std::endl;
    std::cout << "  Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "  Active Neurons: " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "  Firing Neurons: " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Excitation/Inhibition Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    
    auto config = brain.getConfig();
    if (config) {
        std::cout << "\nConfiguration:" << std::endl;
        std::cout << "  Time step: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
        std::cout << "  Random seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
    }
}

// Run basic connectivity test
void runBasicConnectivityTest(std::shared_ptr<Brain> brain, SimulationVerbosity verbosity = SimulationVerbosity::Summary) {
    if (verbosity >= SimulationVerbosity::Summary) {
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    }
    
    auto region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("No regions found!");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    if (verbosity >= SimulationVerbosity::Summary) {
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
    }
    
    size_t neuronCount = std::min(size_t(10), neurons.size());
    for (size_t i = 0; i < neuronCount; ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    
    if (verbosity >= SimulationVerbosity::Summary) {
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
}

// Run comprehensive learning experiment
void runLearningExperiment(std::shared_ptr<Brain> brain, SimulationVerbosity verbosity = SimulationVerbosity::Summary) {
    if (verbosity >= SimulationVerbosity::Summary) {
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    }
    
    // Enable plasticity on synapses
    if (auto region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern
    if (verbosity >= SimulationVerbosity::Summary) {
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    }
    
    BrainStateRecorder recorder(brain);
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0 && verbosity >= SimulationVerbosity::Summary) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
        
        recorder.recordState(step);
    }
    
    // Analyze learning results
    auto metrics = recorder.calculateLearning();
    
    if (verbosity >= SimulationVerbosity::Summary) {
        NLM_LOG_INFO("\n=== Learning Experiment Results ===");
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Weight change: " + std::to_string(metrics.weightChange));
        NLM_LOG_INFO("  Strengthened: " + std::to_string(metrics.strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(metrics.weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(metrics.unchanged));
        
        if (metrics.learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
}

// Run STDP verification test
void runStdpVerification(std::shared_ptr<Brain> brain, SimulationVerbosity verbosity = SimulationVerbosity::Summary) {
    if (verbosity >= SimulationVerbosity::Summary) {
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    }
    
    auto region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    if (verbosity >= SimulationVerbosity::Summary) {
        NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    }
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        if (verbosity >= SimulationVerbosity::Debug) {
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
        }
    }
    
    // Create correlated activity
    if (verbosity >= SimulationVerbosity::Summary) {
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        for (int trial = 0; trial < 50; ++trial) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    if (verbosity >= SimulationVerbosity::Summary) {
        NLM_LOG_INFO("  After correlated activity:");
    }
    
    // Check weight changes
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        
        if (verbosity >= SimulationVerbosity::Summary) {
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
        }
    }
    
    if (verbosity >= SimulationVerbosity::Summary) {
        if (totalDelta > 0.001f) {
            NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        } else if (totalDelta < -0.001f) {
            NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        } else {
            NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        }
    }
}

// Run comprehensive experiment suite
void runComprehensiveExperiment(std::shared_ptr<Brain> brain, SimulationVerbosity verbosity = SimulationVerbosity::Summary) {
    NLM_LOG_INFO("=== COMPREHENSIVE NLM EXPERIMENT SUITE ===");
    NLM_LOG_INFO("Testing all major neural computation capabilities...");
    
    // Test 1: Basic connectivity
    runBasicConnectivityTest(brain, verbosity);
    
    // Reset for plasticity test
    brain->reset();
    brain->initialize();
    
    // Test 2: Plasticity and learning
    runLearningExperiment(brain, verbosity);
    
    // Reset for STDP test
    brain->reset();
    brain->initialize();
    
    // Test 3: STDP verification
    runStdpVerification(brain, verbosity);
    
    // Print final summary
    if (verbosity >= SimulationVerbosity::Summary) {
        printSimulationSummary(*brain, "=== FINAL SIMULATION SUMMARY ===");
    }
}

} // namespace main

int main(int argc, char** argv) {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line arguments
    auto args = main::ArgumentParser::parse(argc, argv);
    
    // Set up logging level based on verbosity
    LogLevel logLevel = LogLevel::Info;
    switch (args.verbosity) {
        case main::SimulationVerbosity::Debug: logLevel = LogLevel::Debug; break;
        case main::SimulationVerbosity::Summary: logLevel = LogLevel::Info; break;
        case main::SimulationVerbosity::Quiet: logLevel = LogLevel::Warning; break;
        case main::SimulationVerbosity::Debug: logLevel = LogLevel::Debug; break;
    }
    
    consoleLogger->setLevel(logLevel);
    
    // Create configuration
    auto config = main::SimulationPresets::loadFromFile(args.configFile);
    
    // Apply experiment-specific overrides
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Initialize brain
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run experiment based on type
    switch (args.experimentType.empty() ? "comprehensive" : args.experimentType) {
        case "basic":
            main::runBasicConnectivityTest(brain, args.verbosity);
            break;
        case "learning":
            main::runLearningExperiment(brain, args.verbosity);
            break;
        case "stdp":
            main::runStdpVerification(brain, args.verbosity);
            break;
        case "comprehensive":
        default:
            main::runComprehensiveExperiment(brain, args.verbosity);
            break;
    }
    
    // Final brain status
    printSimulationSummary(*brain, "=== FINAL BRAIN STATUS ===");
    
    return 0;
}

} // namespace nlm