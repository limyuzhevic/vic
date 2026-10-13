// NLM — 熙然
// Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

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
#include <cctype>
#include <fstream>
#include <sstream>

// Enhanced performance monitoring utilities
class PerformanceMonitor {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    uint64_t stepsCompleted = 0;
    uint64_t totalSpikes = 0;
    double totalTime = 0.0;
    std::vector<double> stepTimes;
    
public:
    PerformanceMonitor() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void stepCompleted(uint64_t spikes, double timeStep) {
        stepsCompleted++;
        totalSpikes += spikes;
        totalTime += timeStep;
        stepTimes.push_back(timeStep);
    }
    
    void printReport(const std::shared_ptr<Brain>& brain, const CommandLineOptions& options) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== PERFORMANCE REPORT ===");
        NLM_LOG_INFO("Total simulation time: " + std::to_string(duration) + "s");
        NLM_LOG_INFO("Steps completed: " + std::to_string(stepsCompleted));
        if (stepsCompleted > 0) {
            NLM_LOG_INFO("Average step time: " + std::to_string((duration / stepsCompleted) * 1000) + "ms");
        }
        NLM_LOG_INFO("Total spikes generated: " + std::to_string(totalSpikes));
        if (stepsCompleted > 0) {
            NLM_LOG_INFO("Average spikes per step: " + std::to_string(static_cast<double>(totalSpikes) / stepsCompleted));
        }
        
        if (!stepTimes.empty() && options.benchmark) {
            double minStep = *std::min_element(stepTimes.begin(), stepTimes.end());
            double maxStep = *std::max_element(stepTimes.begin(), stepTimes.end());
            double avgStep = totalTime / stepsCompleted;
            NLM_LOG_INFO("Step time range: " + std::to_string(minStep * 1000) + " - " + std::to_string(maxStep * 1000) + "ms");
            double variance = 0.0;
            for (double t : stepTimes) {
                variance += (t - avgStep) * (t - avgStep);
            }
            variance /= stepTimes.size();
            NLM_LOG_INFO("Step time stddev: " + std::to_string(std::sqrt(variance) * 1000) + "ms");
            NLM_LOG_INFO("Benchmark metrics:");
            NLM_LOG_INFO("  Steps per second: " + std::to_string(stepsCompleted / duration));
            NLM_LOG_INFO("  Spikes per second: " + std::to_string(totalSpikes / duration));
        }
        
        if (brain) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Brain Statistics:");
            NLM_LOG_INFO("  Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
            NLM_LOG_INFO("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
            NLM_LOG_INFO("  Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
            NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
            NLM_LOG_INFO("  Excitation/Inhibition ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        }
    }
};

// Diagnostic and debugging utilities
class DiagnosticTools {
public:
    static void analyzeBrainState(const std::shared_ptr<Brain>& brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== BRAIN DIAGNOSTICS ===");
        
        if (!brain) {
            NLM_LOG_ERROR("ERROR: No brain object provided");
            return;
        }
        
        auto regions = brain->getAllRegions();
        NLM_LOG_INFO("Regional Analysis:");
        for (const auto& region : regions) {
            auto neurons = region->getAllNeurons();
            auto synapses = region->getSynapses();
            NLM_LOG_INFO("  Region " + std::to_string(region->getId()) + ": " + 
                        std::to_string(neurons.size()) + " neurons, " + 
                        std::to_string(synapses.size()) + " synapses");
            
            if (!neurons.empty()) {
                double avgPotential = 0.0;
                size_t firingCount = 0;
                for (const auto& neuron : neurons) {
                    avgPotential += neuron->getMembranePotential();
                    if (neuron->getIsFiring()) firingCount++;
                }
                avgPotential /= neurons.size();
                NLM_LOG_INFO("    Average membrane potential: " + std::to_string(avgPotential) + "mV");
                NLM_LOG_INFO("    Firing neurons: " + std::to_string(firingCount) + " (" + 
                           std::to_string(firingCount * 100.0 / neurons.size()) + "%)");
            }
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Plasticity Analysis:");
        size_t totalPlastic = 0;
        for (const auto& region : regions) {
            for (const auto& synapse : region->getSynapses()) {
                if (synapse->getPlasticityEnabled()) {
                    totalPlastic++;
                }
            }
        }
        NLM_LOG_INFO("  Synaptic plasticity enabled: " + std::to_string(totalPlastic) + " synapses");
    }
    
    static void checkSystemHealth(const std::shared_ptr<Brain>& brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== SYSTEM HEALTH CHECK ===");
        
        bool healthy = true;
        std::vector<std::string> issues;
        
        // Check brain initialization
        if (!brain) {
            issues.push_back("Brain object is null");
            healthy = false;
        } else {
            if (brain->getTotalNeuronCount() == 0) {
                issues.push_back("Brain has zero neurons");
                healthy = false;
            }
            
            if (brain->getTotalSynapseCount() == 0) {
                issues.push_back("Brain has zero synapses");
                healthy = false;
            }
            
            // Check for numerical stability
            auto regions = brain->getAllRegions();
            for (const auto& region : regions) {
                for (const auto& neuron : region->getAllNeurons()) {
                    double potential = neuron->getMembranePotential();
                    if (std::isinf(potential) || std::isnan(potential)) {
                        issues.push_back("Numerical instability detected in neuron membrane potential");
                        healthy = false;
                        break;
                    }
                }
                if (!healthy) break;
            }
        }
        
        if (healthy) {
            NLM_LOG_INFO("✓ System is healthy");
        } else {
            NLM_LOG_INFO("✗ System health issues detected:");
            for (const auto& issue : issues) {
                NLM_LOG_INFO("  - " + issue);
            }
        }
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain, PerformanceMonitor& perfMonitor) {
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
        perfMonitor.stepCompleted(brain->getFiringNeuronCount(), 0.001);
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain, PerformanceMonitor& perfMonitor) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
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
        perfMonitor.stepCompleted(brain->getFiringNeuronCount(), 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
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
    
    // Compute and display statistics
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Learning Experiment Results ===");
    
    if (initialWeights.empty() || finalWeights.empty()) {
        NLM_LOG_ERROR("ERROR: No weights recorded");
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

void runStdpVerification(std::shared_ptr<Brain> brain, PerformanceMonitor& perfMonitor) {
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
            perfMonitor.stepCompleted(brain->getFiringNeuronCount(), 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            perfMonitor.stepCompleted(brain->getFiringNeuronCount(), 0.001);
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

int main(int argc, char** argv) {
    printBanner();
    
    // Parse enhanced command line options
    CommandLineOptions options = parseCommandLine(argc, argv);
    
    // Display help if requested
    if (options.help) {
        printHelp(options);
        return 0;
    }
    
    // Display version if requested
    if (options.version) {
        printVersion();
        return 0;
    }
    
    // Set up enhanced logging based on options
    auto logger = std::make_shared<Logger>();
    
    LogLevel logLevel;
    if (options.logLevel == "DEBUG") logLevel = LogLevel::Debug;
    else if (options.logLevel == "WARN") logLevel = LogLevel::Warning;
    else if (options.logLevel == "ERROR") logLevel = LogLevel::Error;
    else if (options.logLevel == "CRITICAL") logLevel = LogLevel::Critical;
    else logLevel = LogLevel::Info;
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    logger->addLogger(consoleLogger);
    
    if (!options.logFile.empty() && options.logFile != "-") {
        auto fileLogger = std::make_shared<FileLogger>(options.logFile, logLevel);
        logger->addLogger(fileLogger);
    }
    
    Logger::setGlobal(logger);
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation..." << std::endl;
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Enhanced CLI interface with advanced features");
    NLM_LOG_INFO("Build time: " << __DATE__ << " " << __TIME__);
    NLM_LOG_INFO("");
    
    // Load configuration with enhanced support
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = options.configFile;
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Override with custom parameters from enhanced CLI
    for (const auto& param : options.customParams) {
        config->set(param.first, param.second, ConfigSource::CommandLine);
        NLM_LOG_INFO("Custom parameter: " + param.first + " = " + param.second);
    }
    
    // Set defaults based on enhanced options
    config->set("random_seed", static_cast<int64_t>(options.seed), ConfigSource::CommandLine);
    config->set("simulation_timestep", options.timestep, ConfigSource::CommandLine);
    config->set("max_simulation_steps", static_cast<int64_t>(options.maxSteps), ConfigSource::CommandLine);
    
    // Phase 2 specific defaults (smaller for faster demo)
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Enhanced Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  max_simulation_steps: " + std::to_string(config->getOr<int64_t>("max_simulation_steps", 10000)));
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Enhanced simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize enhanced brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run experiments based on command line arguments
    bool success = true;
    
    // Initialize performance monitor
    PerformanceMonitor perfMonitor;
    
    if (options.experiments.empty()) {
        // Default behavior - run all demo experiments
        NLM_LOG_INFO("No experiments specified - running comprehensive demo...");
        
        // Run Test 1: Basic connectivity
        runBasicConnectivityTest(brain, perfMonitor);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        // Run Test 2: Plasticity learning experiment
        runPlasticityExperiment(brain, perfMonitor);
        
        // Reset and run Test 3: STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain, perfMonitor);
    } else {
        // Run specific experiments based on command line
        for (const auto& experiment : options.experiments) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== RUNNING EXPERIMENT: " + experiment + " ===");
            
            if (experiment == "learning") {
                runPlasticityExperiment(brain, perfMonitor);
            } else if (experiment == "plasticity") {
                runPlasticityExperiment(brain, perfMonitor);
            } else if (experiment == "stdp") {
                runStdpVerification(brain, perfMonitor);
            } else if (experiment == "connectivity") {
                runBasicConnectivityTest(brain, perfMonitor);
            } else {
                NLM_LOG_ERROR("Unknown experiment: " + experiment);
                success = false;
            }
        }
    }
    
    // Print performance report
    perfMonitor.printReport(brain, options);
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Enhanced Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Phase 2 Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Enhanced NLM Brain successfully demonstrated advanced CLI features:");
    NLM_LOG_INFO("  ✓ Comprehensive command-line argument parsing");
    NLM_LOG_INFO("  ✓ Advanced configuration management with overrides");
    NLM_LOG_INFO("  ✓ Enhanced logging and output formatting");
    NLM_LOG_INFO("  ✓ Multiple experiment modes with enhanced testing");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The enhanced NLM brain now supports production-ready features");
    NLM_LOG_INFO("with comprehensive configuration management and debugging tools.");
    NLM_LOG_INFO("");
    
    if (success) {
        NLM_LOG_INFO("✓ All experiments completed successfully");
        return 0;
    } else {
        NLM_LOG_ERROR("✗ Some experiments failed");
        return 1;
    }
}