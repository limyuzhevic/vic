// NLM Brain Stress Tests
// Comprehensive stress testing for Brain system

#include "stress_tests/BrainStressTests.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include "core/Profiler.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <algorithm>
#include <random>
#include <map>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <set>

using namespace nlm;

namespace stress_tests {

// Memory stress tests focusing on allocation/deallocation patterns
void BrainStressTests::runMemoryStressTests() {
    std::cout << "=== Memory Stress Tests ===" << std::endl;
    
    const std::vector<size_t> neuronCounts = {100, 500, 1000, 2000, 5000};
    const size_t steps = 300;
    const std::vector<float> stressLevels = {0.5f, 1.0f, 2.0f, 5.0f};
    
    MemoryStressResults globalResults;
    globalResults.totalTests = neuronCounts.size() * stressLevels.size();
    
    for (size_t neuronCount : neuronCounts) {
        for (float stressLevel : stressLevels) {
            std::cout << "Memory stress test: " << neuronCount << " neurons, stress level " << stressLevel << "..." << std::flush;
            
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.1f / stressLevel, ConfigSource::Default);
            config->set("stdp_ltp_weight", 0.01f * stressLevel, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.012f * stressLevel, ConfigSource::Default);
            
            // Start memory tracking
            auto& memProfiler = MemoryAllocationProfiler::getInstance();
            memProfiler.getStats(); // Clear
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            MemoryStressTestResults result;
            result.neuronCount = neuronCount;
            result.stressLevel = stressLevel;
            
            // Run intensive stress test
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (size_t step = 0; step < steps; ++step) {
                // Vary input intensity based on stress level
                float baseInput = 20.0f + (80.0f * (stressLevel / 10.0f));
                float input = baseInput * (1.0f + 0.5f * std::sin(static_cast<double>(step) * 0.1));
                
                brain->injectCurrentToNeurons(NeuronType::Sensory, input);
                brain->step(step, step * 0.001);
                
                // Force intensive processing
                if (brain->getSTDP()) {
                    auto* spikeSystem = brain->getSpikeSystem();
                    if (spikeSystem) {
                        // Trigger spike processing multiple times
                        for (int i = 0; i < static_cast<int>(stressLevel); ++i) {
                            auto spikes = spikeSystem->getSpikes();
                            spikeSystem->processSpikes();
                        }
                    }
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double totalTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Get memory statistics
            MemoryAllocationProfiler::MemoryStats memStats = memProfiler.getStats();
            
            result.totalTime = totalTime;
            result.stepsPerSecond = steps / totalTime;
            result.peakMemoryKB = memStats.peakAllocated / 1024.0;
            result.totalMemoryKB = memStats.totalAllocated / 1024.0;
            result.allocationCount = memStats.allocationCount;
            result.stressImpactScore = calculateStressImpactScore(memStats, neuronCount, steps);
            
            globalResults.testResults.push_back(result);
            
            std::cout << " Done!" << std::endl;
            std::cout << "    Time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
            std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
            std::cout << "    Stress Impact: " << std::fixed << std::setprecision(2) << result.stressImpactScore << std::endl;
        }
    }
    
    printMemoryStressResults(globalResults);
}

// CPU stress tests for maximum computation load
void BrainStressTests::runCPUSTressTests() {
    std::cout << "=== CPU Stress Tests ===" << std::endl;
    
    const std::vector<std::pair<size_t, size_t>> configurations = {
        {100, 1000},   // 100 neurons, 1000 steps
        {500, 500},    // 500 neurons, 500 steps
        {1000, 200},   // 1000 neurons, 200 steps
        {2000, 100},   // 2000 neurons, 100 steps
    };
    
    CPUMemoryResults globalResults;
    globalResults.totalTests = configurations.size();
    
    for (const auto& [neuronCount, steps] : configurations) {
        std::cout << "CPU stress test: " << neuronCount << " neurons, " << steps << " steps..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.025f, ConfigSource::Default);
        
        // Start profiling
        startPhase(SimulationStepProfiler::PHASE_SYNAPSE_INIT);
        startPhase(SimulationStepProfiler::PHASE_NEURON_INIT);
        startPhase(SimulationStepProfiler::PHASE_SYNAPSE_CONNECT);
        startPhase(SimulationStepProfiler::PHASE_NEURON_CONNECT);
        startPhase(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
        startPhase(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
        startPhase(SimulationStepProfiler::PHASE_STDP);
        startPhase(SimulationStepProfiler::PHASE_HEBBIAN);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        CPUMemoryTestResults result;
        result.neuronCount = neuronCount;
        result.steps = steps;
        
        for (size_t step = 0; step < steps; ++step) {
            // Maximum intensity input to stress CPU
            brain->injectCurrentToNeurons(NeuronType::Excitatory, 100.0f);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Stop all phases
        stopPhase(); // PHASE_HEBBIAN
        stopPhase(); // PHASE_STDP
        stopPhase(); // PHASE_NEURON_ACTIVATION
        stopPhase(); // PHASE_SYNAPSE_ACTIVATION
        stopPhase(); // PHASE_NEURON_CONNECT
        stopPhase(); // PHASE_SYNAPSE_CONNECT
        stopPhase(); // PHASE_NEURON_INIT
        stopPhase(); // PHASE_SYNAPSE_INIT
        
        // Get profiling statistics
        auto& stepProfiler = SimulationStepProfiler::getInstance();
        
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        
        // Collect phase statistics
        auto synapseInitStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_INIT);
        auto neuronInitStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_NEURON_INIT);
        auto synapseConnectStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_CONNECT);
        auto neuronConnectStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_NEURON_CONNECT);
        auto synapseActivationStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
        auto neuronActivationStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
        auto stdpStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_STDP);
        auto hebbianStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_HEBBIAN);
        
        result.synapseInitTime = synapseInitStats.averageTimeNs() / 1000.0;
        result.neuronInitTime = neuronInitStats.averageTimeNs() / 1000.0;
        result.synapseConnectTime = synapseConnectStats.averageTimeNs() / 1000.0;
        result.neuronConnectTime = neuronConnectStats.averageTimeNs() / 1000.0;
        result.synapseActivationTime = synapseActivationStats.averageTimeNs() / 1000.0;
        result.neuronActivationTime = neuronActivationStats.averageTimeNs() / 1000.0;
        result.stdpTime = stdpStats.averageTimeNs() / 1000.0;
        result.hebbianTime = hebbianStats.averageTimeNs() / 1000.0;
        
        result.totalSpikeCount = brain->getTotalSpikeCount();
        
        globalResults.testResults.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Total time: " << std::fixed << std::setprecision(3) << totalTime << "s" << std::endl;
        
        resetProfiling(); // Reset for next test
    }
    
    printCPUMemoryStressResults(globalResults);
}

// Network stress tests for large-scale neural networks
void BrainStressTests::runNetworkStressTests() {
    std::cout << "=== Network Stress Tests ===" << std::endl;
    
    const std::vector<size_t> neuronCounts = {200, 500, 1000, 2000};
    const size_t steps = 150;
    
    NetworkStressResults globalResults;
    globalResults.totalTests = neuronCounts.size();
    
    for (size_t neuronCount : neuronCounts) {
        std::cout << "Network stress test: " << neuronCount << " neurons, " << steps << " steps..." << std::flush;
        
        // Create multi-region configuration to stress network connectivity
        size_t regionCount = std::min<size_t>(neuronCount / 100, 4);
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        NetworkStressTestResults result;
        result.neuronCount = neuronCount;
        result.regionCount = regionCount;
        result.totalSteps = steps;
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject input to multiple neuron types to stress network
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->injectCurrentToNeurons(NeuronType::Excitatory, 25.0f);
            brain->injectCurrentToNeurons(NeuronType::Inhibitory, 15.0f);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        NetworkStats brainStats;
        brainStats.neuronCount = brain->getTotalNeuronCount();
        brainStats.synapseCount = brain->getTotalSynapseCount();
        brainStats.spikeCount = brain->getTotalSpikeCount();
        
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.networkUtilization = calculateNetworkUtilization(brainStats, neuronCount);
        
        globalResults.testResults.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Network utilization: " << std::fixed << std::setprecision(2) << result.networkUtilization << std::endl;
    }
    
    printNetworkStressResults(globalResults);
}

// Plasticity stress tests with high learning rates
void BrainStressTests::runPlasticityStressTests() {
    std::cout << "=== Plasticity Stress Tests ===" << std::endl;
    
    const std::vector<std::pair<size_t, float>> configurations = {
        {100, 0.1f},   // 100 neurons, high learning rate
        {500, 0.2f},   // 500 neurons, very high learning rate
        {1000, 0.3f},  // 1000 neurons, extreme learning rate
        {2000, 0.5f},  // 2000 neurons, maximum learning rate
    };
    
    PlasticityResults globalResults;
    globalResults.totalTests = configurations.size();
    
    for (const auto& [neuronCount, learningRate] : configurations) {
        std::cout << "Plasticity stress test: " << neuronCount << " neurons, LR " << learningRate << "..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f * learningRate, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f * learningRate, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Start profiling plasticity phases
        startPhase(SimulationStepProfiler::PHASE_STDP);
        startPhase(SimulationStepProfiler::PHASE_HEBBIAN);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        PlasticityStressTestResults result;
        result.neuronCount = neuronCount;
        result.learningRate = learningRate;
        result.totalSteps = 200;
        
        for (size_t step = 0; step < result.totalSteps; ++step) {
            // Inject input with high variance to stress plasticity
            float input = 20.0f + (60.0f * ((static_cast<float>(std::rand()) / RAND_MAX) - 0.5f) * learningRate);
            brain->injectCurrentToNeurons(NeuronType::Sensory, std::max(0.0f, input));
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Stop plasticity phases
        stopPhase(); // PHASE_HEBBIAN
        stopPhase(); // PHASE_STDP
        
        // Get profiling statistics
        auto& stepProfiler = SimulationStepProfiler::getInstance();
        auto stdpStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_STDP);
        auto hebbianStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_HEBBIAN);
        
        PlasticityStats plasticityStats;
        plasticityStats.totalSpikeCount = brain->getTotalSpikeCount();
        plasticityStats.plasticityConvergenceRate = calculatePlasticityConvergenceRate(neuronCount, learningRate);
        
        result.totalTime = totalTime;
        result.stepsPerSecond = result.totalSteps / totalTime;
        result.stdpUpdateTime = stdpStats.averageTimeNs() / 1000.0;
        result.hebbianUpdateTime = hebbianStats.averageTimeNs() / 1000.0;
        
        globalResults.testResults.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    STDP time: " << std::fixed << std::setprecision(3) << result.stdpUpdateTime << " µs" << std::endl;
        std::cout << "    Hebbian time: " << std::fixed << std::setprecision(3) << result.hebbianUpdateTime << " µs" << std::endl;
        
        resetProfiling(); // Reset for next test
    }
    
    printPlasticityStressResults(globalResults);
}

// Neuromodulation stress tests with rapid signal changes
void BrainStressTests::runNeuromodulationStressTests() {
    std::cout << "=== Neuromodulation Stress Tests ===" << std::endl;
    
    const std::vector<size_t> neuronCounts = {100, 300, 600, 1200};
    const size_t steps = 100;
    
    NeuromodulationResults globalResults;
    globalResults.totalTests = neuronCounts.size();
    
    for (size_t neuronCount : neuronCounts) {
        std::cout << "Neuromodulation stress test: " << neuronCount << " neurons..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.08f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Start profiling neuromodulation-related phases
        startPhase(SimulationStepProfiler::PHASE_SYNAPSE_ACTIVATION);
        startPhase(SimulationStepProfiler::PHASE_NEURON_ACTIVATION);
        
        NeuromodulationStressTestResults result;
        result.neuronCount = neuronCount;
        result.totalSteps = steps;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation with rapidly changing neuromodulatory signals
        for (size_t step = 0; step < steps; ++step) {
            // Rapidly changing input to simulate neuromodulatory effects
            float timeBasedInput = 20.0f + (40.0f * std::sin(static_cast<double>(step) * 0.1));
            float randomInput = 10.0f + (30.0f * (static_cast<float>(std::rand()) / RAND_MAX));
            float combinedInput = (timeBasedInput + randomInput) / 2.0f;
            
            brain->injectCurrentToNeurons(NeuronType::Sensory, combinedInput);
            brain->step(step, step * 0.001);
            
            // Check for neuromodulatory system activation
            if (brain->getDopamine()) {
                result.dopamineActivations++;
            }
            if (brain->getCuriosity()) {
                result.curiosityActivations++;
            }
            if (brain->getNovelty()) {
                result.noveltyActivations++;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Stop profiling phases
        stopPhase(); // PHASE_NEURON_ACTIVATION
        stopPhase(); // PHASE_SYNAPSE_ACTIVATION
        
        NeuromodulationStats neuromodStats;
        neuromodStats.totalSpikeCount = brain->getTotalSpikeCount();
        neuromodStats.activationsPerStep = static_cast<double>(result.dopamineActivations + result.curiosityActivations + result.noveltyActivations) / steps;
        
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.neuromodulationStressScore = calculateNeuromodulationStressScore(neuromStats, neuronCount);
        
        globalResults.testResults.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Neuromodulation stress score: " << std::fixed << std::setprecision(2) << result.neuromodulationStressScore << std::endl;
        
        resetProfiling(); // Reset for next test
    }
    
    printNeuromodulationStressResults(globalResults);
}

// Development stress tests with accelerated development
void BrainStressTests::runDevelopmentStressTests() {
    std::cout << "=== Development Stress Tests ===" << std::endl;
    
    const std::vector<std::pair<size_t, float>> configurations = {
        {100, 2.0f},   // 100 neurons, 2x development acceleration
        {300, 3.0f},   // 300 neurons, 3x development acceleration
        {600, 4.0f},   // 600 neurons, 4x development acceleration
        {1000, 5.0f},  // 1000 neurons, 5x development acceleration
    };
    
    DevelopmentResults globalResults;
    globalResults.totalTests = configurations.size();
    
    for (const auto& [neuronCount, accelFactor] : configurations) {
        std::cout << "Development stress test: " << neuronCount << " neurons, accel " << accelFactor << "..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f * accelFactor, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f * accelFactor, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Run simulation with accelerated development parameters
        DevelopmentStressTestResults result;
        result.neuronCount = neuronCount;
        result.accelerationFactor = accelFactor;
        result.totalSteps = 300;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < result.totalSteps; ++step) {
            // Inject input with varying intensity to trigger development
            float baseInput = 20.0f;
            float accelerationInput = baseInput * accelFactor;
            float developmentInput = accelerationInput * (1.0f + 0.3f * std::sin(static_cast<double>(step) * 0.05));
            
            brain->injectCurrentToNeurons(NeuronType::Sensory, developmentInput);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        DevelopmentStats devStats;
        devStats.totalSpikeCount = brain->getTotalSpikeCount();
        devStats.developmentRate = calculateDevelopmentRate(neuronCount, accelFactor);
        
        result.totalTime = totalTime;
        result.stepsPerSecond = result.totalSteps / totalTime;
        result.developmentStability = calculateDevelopmentStability(devStats, neuronCount);
        
        globalResults.testResults.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Development stability: " << std::fixed << std::setprecision(2) << result.developmentStability << std::endl;
    }
    
    printDevelopmentStressResults(globalResults);
}

// Helper functions for stress test analysis

double BrainStressTests::calculateStressImpactScore(const MemoryAllocationProfiler::MemoryStats& memStats, 
                                                   size_t neuronCount, size_t steps) {
    double memoryPerNeuron = static_cast<double>(memStats.peakAllocated) / neuronCount;
    double allocPerStep = static_cast<double>(memStats.allocationCount) / steps;
    
    // Score that penalizes high memory usage and excessive allocations
    double memoryScore = std::min(1.0, 100000.0 / memStats.peakAllocated); // 1 KB gives 100% score
    double allocScore = std::min(1.0, 10000.0 / memStats.allocationCount); // 10K allocs gives 100% score
    
    return (memoryScore + allocScore) / 2.0;
}

double BrainStressTests::calculateNetworkUtilization(const NetworkStats& brainStats, size_t neuronCount) {
    double synapseDensity = static_cast<double>(brainStats.synapseCount) / neuronCount;
    double activityLevel = static_cast<double>(brainStats.spikeCount) / (brainStats.spikeCount + 1); // Normalize
    
    return (synapseDensity / 10.0) * activityLevel; // Normalize synapse density
}

double BrainStressTests::calculatePlasticityConvergenceRate(size_t neuronCount, float learningRate) {
    double baseRate = 100.0 / neuronCount; // Base convergence rate
    double rateBoost = learningRate / 5.0f; // Learning rate boost
    
    return std::min(1.0, baseRate + rateBoost); // Cap at 1.0
}

double BrainStressTests::calculateNeuromodulationStressScore(const NeuromodulationStats& neuromodStats, size_t neuronCount) {
    double activationPerNeuron = neuromodStats.activationsPerStep * neuronCount;
    double normalizedActivations = std::min(1.0, activationPerNeuron / 100.0); // 100 activations max for 100% score
    
    return normalizedActivations;
}

double BrainStressTests::calculateDevelopmentRate(size_t neuronCount, float accelerationFactor) {
    double baseRate = 1.0 / neuronCount; // Base rate
    double accelRate = (accelerationFactor - 1.0f) / 5.0f; // Acceleration rate
    
    return std::min(1.0, baseRate + accelRate);
}

double BrainStressTests::calculateDevelopmentStability(const DevelopmentStats& devStats, size_t neuronCount) {
    double spikeRate = static_cast<double>(devStats.totalSpikeCount) / (devStats.totalSpikeCount + 100); // Normalize
    double developmentRate = devStats.developmentRate;
    
    return (spikeRate + developmentRate) / 2.0;
}

// Result printing functions

void BrainStressTests::printMemoryStressResults(const MemoryStressResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                 Memory Stress Test Results                              ║" << std::endl;
    std::cout << "╠════════════╦════════╦══════════╦════════════╦══════════╦════════╗" << std::endl;
    std::cout << "║  Neurons   ║ Stress ║ Steps/s  ║ Peak(KB)  ║ Allocs   ║ Impact  ║" << std::endl;
    std::cout << "╠════════════╬════════╬══════════╬════════════╬══════════╬════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(6) << std::fixed << std::setprecision(1) << r.stressLevel
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(1) << r.peakMemoryKB
                  << " ║ " << std::setw(8) << r.allocationCount
                  << " ║ " << std::setw(6) << std::fixed << std::setprecision(2) << r.stressImpactScore
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩══════════╩════════════╩══════════╩════════╝" << std::endl;
    std::cout << std::endl;
}

void BrainStressTests::printCPUMemoryStressResults(const CPUMemoryResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║               CPU Memory Stress Test Results                            ║" << std::endl;
    std::cout << "╠════════════╦════════╦══════════════╦══════════╦════════════╦════════╗" << std::endl;
    std::cout << "║  Neurons   ║ Steps/s ║ SynapseInit  ║ NeuronInit ║ STDP(µs)  ║ Hebbian(µs) ║" << std::endl;
    std::cout << "╠════════════╬════════╬══════════════╬══════════╬════════════╬════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(14) << std::fixed << std::setprecision(3) << r.synapseInitTime
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(3) << r.neuronInitTime
                  << " ║ " << std::setw(10) << std::fixed << std::setprecision(3) << r.stdpTime
                  << " ║ " << std::setw(10) << std::fixed << std::setprecision(3) << r.hebbianTime
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩══════════════╩══════════╩════════════╩════════╝" << std::endl;
    std::cout << std::endl;
}

void BrainStressTests::printNetworkStressResults(const NetworkStressResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║               Network Stress Test Results                              ║" << std::endl;
    std::cout << "╠════════════╦══════════╦════════╦══════════╦════════╗" << std::endl;
    std::cout << "║  Neurons   ║ Regions ║ Steps/s ║ Util     ║ Spikes  ║" << std::endl;
    std::cout << "╠════════════╬══════════╬════════╬══════════╬════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(10) << r.regionCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(2) << r.networkUtilization
                  << " ║ " << std::setw(7) << r.stats.spikeCount
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩══════════╩════════╩══════════╩════════╝" << std::endl;
    std::cout << std::endl;
}

void BrainStressTests::printPlasticityStressResults(const PlasticityResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║               Plasticity Stress Test Results                            ║" << std::endl;
    std::cout << "╠════════════╦════════╦══════════════╦══════════╦════════════╗" << std::endl;
    std::cout << "║  Neurons   ║ Stress ║ Steps/s     ║ STDP(µs)  ║ Hebbian(µs) ║" << std::endl;
    std::cout << "╠════════════╬════════╬══════════════╬══════════╬════════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(6) << std::fixed << std::setprecision(1) << r.learningRate
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(3) << r.stdpUpdateTime
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(3) << r.hebbianUpdateTime
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩══════════════╩══════════╩════════════╝" << std::endl;
    std::cout << std::endl;
}

void BrainStressTests::printNeuromodulationStressResults(const NeuromodulationResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║           Neuromodulation Stress Test Results                          ║" << std::endl;
    std::cout << "╠════════════╦════════╦══════════╦════════╦══════════╦════════╗" << std::endl;
    std::cout << "║  Neurons   ║ Steps/s ║ Stress   ║ DopAM   ║ Curious  ║ Novel   ║" << std::endl;
    std::cout << "╠════════════╬════════╬══════════╬════════╬══════════╬════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(10) << std::fixed << std::setprecision(2) << r.neuromodulationStressScore
                  << " ║ " << std::setw(7) << r.dopamineActivations
                  << " ║ " << std::setw(7) << r.curiosityActivations
                  << " ║ " << std::setw(7) << r.noveltyActivations
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩══════════╩════════╩══════════╩════════╝" << std::endl;
    std::cout << std::endl;
}

void BrainStressTests::printDevelopmentStressResults(const DevelopmentResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║               Development Stress Test Results                           ║" << std::endl;
    std::cout << "╠════════════╦═════════╦════════╦════════════╗" << std::endl;
    std::cout << "║  Neurons   ║ Accel   ║ Steps/s ║ Stability ║" << std::endl;
    std::cout << "╠════════════╬═════════╬════════╬════════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(8) << std::fixed << std::setprecision(1) << r.accelerationFactor
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(2) << r.developmentStability
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩═════════╩════════╩════════════╝" << std::endl;
    std::cout << std::endl;
}

} // namespace stress_tests

// Main stress test runner
int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Brain Stress Tests                                  ║
    ║                                                               ║
    ║     Comprehensive Brain System Stress Testing                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Initialize random number generator
    auto rng = std::make_shared<RandomGenerator>();
    RandomGenerator::setGlobal(rng);
    
    std::cout << "Running Brain Stress Tests..." << std::endl;
    std::cout << std::endl;
    
    // Run all stress test suites
    stress_tests::BrainStressTests::runMemoryStressTests();
    std::cout << std::endl;
    
    stress_tests::BrainStressTests::runCPUSTressTests();
    std::cout << std::endl;
    
    stress_tests::BrainStressTests::runNetworkStressTests();
    std::cout << std::endl;
    
    stress_tests::BrainStressTests::runPlasticityStressTests();
    std::cout << std::endl;
    
    stress_tests::BrainStressTests::runNeuromodulationStressTests();
    std::cout << std::endl;
    
    stress_tests::BrainStressTests::runDevelopmentStressTests();
    std::cout << std::endl;
    
    // Generate final profiling report
    std::cout << "=== FINAL STRESS TEST PROFILING REPORT ===" << std::endl;
    std::cout << generateProfilingReport() << std::endl;
    
    std::cout << "Brain Stress Tests complete." << std::endl;
    
    return 0;
}
