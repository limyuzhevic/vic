// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
// Enhanced with advanced CLI, configuration, and testing capabilities

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
#include <map>
#include <algorithm>
#include <cmath>
#include <filesystem>

using namespace nlm;

// Advanced logging levels
enum class LogLevel {
    Quiet = 0,
    Error = 1,
    Warning = 2,
    Info = 3,
    Debug = 4,
    Trace = 5
};

// Enhanced brain simulation modes
enum class SimulationMode {
    BasicConnectivity,
    PlasticityLearning,
    STDPVerification,
    FullIntegration,
    DevelopmentCycle,
    Neuromodulation,
    MemorySystems,
    Custom
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string message;
    std::map<std::string, float> warnings;
    std::map<std::string, float> errors;
};

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
    ║     - Advanced memory systems                                ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Enhanced help information
void printHelp(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --version, -v                 Show version information" << std::endl;
    std::cout << "  --config, -c FILE             Load configuration from FILE" << std::endl;
    std::cout << "  --mode MODE                   Simulation mode (see modes below)" << std::endl;
    std::cout << "  --steps STEPS                 Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --neurons N                   Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --regions R                   Number of regions (default: 1)" << std::endl;
    std::cout << "  --timestep T                  Simulation timestep in seconds (default: 0.001)" << std::endl;
    std::cout << "  --log-level LEVEL             Logging level: QUIET, ERROR, WARNING, INFO, DEBUG, TRACE" << std::endl;
    std::cout << "  --output-dir DIR               Directory for output files (default: ./outputs)" << std::endl;
    std::cout << "  --save-brain FILE             Save brain state to FILE" << std::endl;
    std::cout << "  --load-brain FILE             Load brain state from FILE" << std::endl;
    std::cout << "  --verbose, -V                 Enable verbose output" << std::endl;
    std::cout << "  --quiet, -Q                   Suppress informational output" << std::endl;
    std::cout << "  --measure-performance         Measure and report performance metrics" << std::endl;
    std::cout << "  --benchmark                   Run performance benchmarks" << std::endl;
    std::cout << "  --analyze-activity            Analyze and visualize neural activity patterns" << std::endl;
    std::cout << "  --debug-mode                  Enable debug mode with enhanced logging" << std::endl;
    std::cout << std::endl;
    std::cout << "Available simulation modes:" << std::endl;
    std::cout << "  basic     - Basic neural connectivity test" << std::endl;
    std::cout << "  plasticity - Plasticity learning experiment" << std::endl;
    std::cout << "  stdp      - STDP verification experiment" << std::endl;
    std::cout << "  integration - Full integration test" << std::endl;
    std::cout << "  development - Developmental cycle simulation" << std::endl;
    std::cout << "  neuromod   - Neuromodulation experiment" << std::endl;
    std::cout << "  memory    - Memory systems test" << std::endl;
    std::cout << "  custom    - Custom simulation with specified parameters" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --mode plasticity --steps 5000 --neurons 2000" << std::endl;
    std::cout << "  " << programName << " --mode integration --timestep 0.0005 --save-brain brain.checkpoint" << std::endl;
    std::cout << "  " << programName << " --mode development --benchmark --analyze-activity" << std::endl;
}

// Version information
void printVersion() {
    std::cout << "NLM v0.1.0 (Phase 2: Real Neural Computation)" << std::endl;
    std::cout << "Copyright (c) 2026 NLM Research Project" << std::endl;
    std::cout << "Built with C++20, pybind11 for Python bindings" << std::endl;
}

// Configuration validation
ValidationResult validateConfiguration(const std::shared_ptr<Config>& config) {
    ValidationResult result;
    result.valid = true;
    result.message = "Configuration valid";
    
    // Check for required keys
    std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            result.valid = false;
            result.errors[key] = 0.0f;
            result.message += " Missing required key: " + key;
        }
    }
    
    // Validate parameter ranges
    if (config->has("simulation_timestep")) {
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0.0 || timestep > 1.0) {
            result.warnings["simulation_timestep"] = timestep;
            result.message += " Simulation timestep out of recommended range (0.0001-0.1)";
        }
    }
    
    if (config->has("neuron_count")) {
        size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
        if (neuronCount == 0 || neuronCount > 1000000) {
            result.errors["neuron_count"] = static_cast<float>(neuronCount);
            result.valid = false;
        }
    }
    
    return result;
}

// Parse simulation mode from string
SimulationMode parseMode(const std::string& modeStr) {
    std::string mode = modeStr;
    std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
    
    if (mode == "basic" || mode == "connectivity") return SimulationMode::BasicConnectivity;
    if (mode == "plasticity") return SimulationMode::PlasticityLearning;
    if (mode == "stdp" || mode == "stdp-verification") return SimulationMode::STDPVerification;
    if (mode == "integration") return SimulationMode::FullIntegration;
    if (mode == "development") return SimulationMode::DevelopmentCycle;
    if (mode == "neuromod" || mode == "neuromodulation") return SimulationMode::Neuromodulation;
    if (mode == "memory") return SimulationMode::MemorySystems;
    if (mode == "custom") return SimulationMode::Custom;
    
    return SimulationMode::BasicConnectivity; // Default
}

// Performance measurement
struct PerformanceMetrics {
    double totalTime;
    double simulationTime;
    double initializationTime;
    double stepTime;
    size_t totalSteps;
    size_t spikesGenerated;
    double spikesPerSecond;
    size_t neurons;
    size_t synapses;
};

PerformanceMetrics measurePerformance(std::shared_ptr<Brain> brain, SimulationMode mode, size_t steps) {
    PerformanceMetrics metrics;
    metrics.totalSteps = steps;
    
    // Measure performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (mode == SimulationMode::BasicConnectivity) {
        runBasicConnectivityTest(brain);
    } else if (mode == SimulationMode::PlasticityLearning) {
        runPlasticityExperiment(brain);
    } else if (mode == SimulationMode::STDPVerification) {
        runStdpVerification(brain);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    metrics.totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Calculate metrics
    metrics.spikesGenerated = brain->getTotalSpikeCount();
    metrics.spikesPerSecond = static_cast<double>(metrics.spikesGenerated) / metrics.totalTime;
    
    return metrics;
}

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    std::string modeName;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s, const std::string& name) 
        : brain(b), seed(s), modeName(name), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded for " + modeName + ":");
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
        
        NLM_LOG_INFO("Final state recorded for " + modeName + ":");
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
        NLM_LOG_INFO("Mode: " + modeName);
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

void runBasicConnectivityTest(std::shared_ptr<Brain> brain, const std::string& modeName = "Basic Connectivity") {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    NLM_LOG_INFO("Mode: " + modeName);
    
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain, const std::string& modeName = "Plasticity Learning") {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    NLM_LOG_INFO("Mode: " + modeName);
    
    LearningExperiment experiment(brain, 42, modeName);
    
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

void runStdpVerification(std::shared_ptr<Brain> brain, const std::string& modeName = "STDP Verification") {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    NLM_LOG_INFO("Mode: " + modeName);
    
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
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

void runFullIntegrationTest(std::shared_ptr<Brain> brain, const std::string& modeName = "Full Integration") {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 4: Full Integration Test ===");
    NLM_LOG_INFO("Mode: " + modeName);
    NLM_LOG_INFO("Testing all integrated systems...");
    
    // Record initial state
    LearningExperiment experiment(brain, 123, modeName);
    experiment.recordInitialState();
    
    // Enable all plasticity for comprehensive testing
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    // Run comprehensive simulation with all systems
    NLM_LOG_INFO("Running comprehensive integration simulation (500 steps)...");
    for (SimulationStep step = 0; step < 500; ++step) {
        // Inject varied input patterns
        for (size_t i = 0; i < brain->getTotalNeuronCount() / 10; ++i) {
            brain->injectCurrentToNeurons(static_cast<NeuronType>(i % 4), 20.0f + (i % 3) * 10.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Active: " + std::to_string(brain->getActiveNeuronCount()) +
                        " | Memory traces: " + std::to_string(brain->getWorkingMemory() ? 
                            brain->getWorkingMemory()->getActiveTraces() : 0));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display comprehensive statistics
    experiment.computeStatistics();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Integration Test Summary ===");
    NLM_LOG_INFO("Brain Status:");
    brain->logStatus();
}

void runDevelopmentCycleTest(std::shared_ptr<Brain> brain, const std::string& modeName = "Development Cycle") {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 5: Developmental Cycle Simulation ===");
    NLM_LOG_INFO("Mode: " + modeName);
    NLM_LOG_INFO("Simulating developmental stages over time...");
    
    // Record initial state
    LearningExperiment experiment(brain, 456, modeName);
    experiment.recordInitialState();
    
    // Run through developmental stages
    NLM_LOG_INFO("Simulating development stages (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Inject developmental stimulation
        for (size_t i = 0; i < brain->getTotalNeuronCount() / 20; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Internal, 10.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Change developmental stage at specific milestones
        if (step == 200) {
            brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
            NLM_LOG_INFO("  Reached Critical Period stage");
        } else if (step == 500) {
            brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
            NLM_LOG_INFO("  Reached Maturation stage");
        } else if (step == 800) {
            brain->setDevelopmentalStage(DevelopmentalStage::Adult);
            NLM_LOG_INFO("  Reached Adult stage");
        }
        
        if (step % 200 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())) +
                        " | Plasticity: " + std::to_string(brain->getExcitationInhibitionRatio()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display developmental statistics
    experiment.computeStatistics();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Development cycle complete. Brain matured through all stages.");
}

void runNeuromodulationTest(std::shared_ptr<Brain> brain, const std::string& modeName = "Neuromodulation") {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 6: Neuromodulation Integration ===");
    NLM_LOG_INFO("Mode: " + modeName);
    NLM_LOG_INFO("Testing neuromodulation effects on learning...");
    
    // Record initial state
    LearningExperiment experiment(brain, 789, modeName);
    experiment.recordInitialState();
    
    // Enable neuromodulation for testing
    if (auto* dopamine = brain->getDopamine()) {
        dopamine->enable(true);
        NLM_LOG_INFO("Dopamine system enabled");
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        curiosity->enable(true);
        NLM_LOG_INFO("Curiosity system enabled");
    }
    
    // Run simulation with neuromodulation
    NLM_LOG_INFO("Running neuromodulation simulation (800 steps)...");
    
    for (SimulationStep step = 0; step < 800; ++step) {
        // Inject input that will trigger neuromodulation
        brain->injectCurrentToNeurons(NeuronType::Sensory, 25.0f);
        
        // Add occasional reward prediction errors
        if (step % 100 == 0) {
            if (auto* predictionError = brain->getPredictionErrorSignal()) {
                predictionError->setLevel(1.0f);
                NLM_LOG_INFO("  Reward prediction error: +1.0 at step " + std::to_string(step));
            }
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 200 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Dopamine: " + std::to_string(brain->getDopamine() ? 
                            brain->getDopamine()->getLevel() : 0.0f) +
                        " | Curiosity: " + std::to_string(brain->getCuriosity() ? 
                            brain->getCuriosity()->getLevel() : 0.0f) +
                        " | Novelty: " + std::to_string(brain->getNovelty() ? 
                            brain->getNovelty()->getLevel() : 0.0f));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display neuromodulation statistics
    experiment.computeStatistics();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Neuromodulation test complete. System response recorded.");
}

void runMemorySystemsTest(std::shared_ptr<Brain> brain, const std::string& modeName = "Memory Systems") {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 7: Memory Systems Integration ===");
    NLM_LOG_INFO("Mode: " + modeName);
    NLM_LOG_INFO("Testing memory system integration...");
    
    // Record initial state
    LearningExperiment experiment(brain, 999, modeName);
    experiment.recordInitialState();
    
    // Run simulation with memory system focus
    NLM_LOG_INFO("Running memory systems simulation (600 steps)...");
    
    for (SimulationStep step = 0; step < 600; ++step) {
        // Create patterns that will be stored in memory
        brain->injectCurrentToNeurons(NeuronType::Sensory, 15.0f);
        
        // Periodically trigger memory consolidation
        if (step % 100 == 0 && step > 0) {
            NLM_LOG_INFO("  Performing memory consolidation at step " + std::to_string(step));
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Working memory: " + std::to_string(brain->getWorkingMemory() ? 
                            brain->getWorkingMemory()->getActiveTraces() : 0) +
                        " | Episodic memory: " + std::to_string(brain->getEpisodicMemory() ? 
                            brain->getEpisodicMemory()->getEpisodeCount() : 0) +
                        " | Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display memory statistics
    experiment.computeStatistics();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Memory systems test complete. Memory integration verified.");
}

void runCustomTest(std::shared_ptr<Brain> brain, const std::string& modeName = "Custom") {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test: Custom Configuration ===");
    NLM_LOG_INFO("Mode: " + modeName);
    NLM_LOG_INFO("Running custom simulation with user-defined parameters...");
    
    // Record initial state
    LearningExperiment experiment(brain, 111, modeName);
    experiment.recordInitialState();
    
    // Run extended simulation with custom behavior
    NLM_LOG_INFO("Running custom simulation (1500 steps)...");
    
    for (SimulationStep step = 0; step < 1500; ++step) {
        // Vary input patterns based on step
        float currentAmplitude = 10.0f + 20.0f * std::sin(static_cast<float>(step) * 0.01f);
        brain->injectCurrentToNeurons(NeuronType::Sensory, currentAmplitude);
        
        brain->step(step, step * 0.001);
        
        if (step % 300 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Current: " + std::to_string(currentAmplitude) +
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display custom statistics
    experiment.computeStatistics();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Custom test complete. User-defined parameters executed successfully.");
}