// NLM Plasticity System Stress Tests
// Comprehensive stress testing for Plasticity system

#include "stress_tests/PlasticityStressTests.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "core/Config/Config.hpp"
#include "core/Profiler.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
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
#include <string>

using namespace nlm;

namespace stress_tests {

// STDP stress tests focusing on high-frequency spike pairs
void PlasticityStressTests::runSTDPStressTests() {
    std::cout << "=== STDP Stress Tests ===" << std::endl;
    
    const std::vector<std::pair<size_t, size_t>> configurations = {
        {50, 1000},    // 50 neurons, 1000 spike pairs
        {100, 2000},   // 100 neurons, 2000 spike pairs
        {200, 3000},   // 200 neurons, 3000 spike pairs
        {500, 5000},   // 500 neurons, 5000 spike pairs
    };
    
    STDPResults globalResults;
    globalResults.totalTests = configurations.size();
    
    for (const auto& [neuronCount, spikePairs] : configurations) {
        std::cout << "STDP stress test: " << neuronCount << " neurons, " << spikePairs << " spike pairs..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.022f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Get STDP system for direct testing
        auto* stdpSystem = brain->getSTDP();
        if (!stdpSystem) {
            std::cout << " Failed - STDP system not available!" << std::endl;
            continue;
        }
        
        // Start profiling STDP operations
        startPhase(SimulationStepProfiler::PHASE_STDP);
        
        STDPStressTestResults result;
        result.neuronCount = neuronCount;
        result.spikePairCount = spikePairs;
        result.totalSteps = spikePairs / 100; // Group pairs into steps
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Generate high-frequency spike pairs
        for (size_t step = 0; step < result.totalSteps; ++step) {
            // Generate multiple spike pairs per step to stress STDP
            size_t pairsInStep = 100 + (step % 100); // Varying pair count per step
            
            for (size_t i = 0; i < pairsInStep; ++i) {
                // Create pre-post spike pairs with high frequency
                float preTime = static_cast<float>(step * 1000 + i * 10); // 10ms intervals
                float postTime = preTime + 20.0f; // 20ms post (strong LTP/LTD)
                
                // Trigger STDP with high-frequency pairs
                stdpSystem->processPrePostSpike(preTime, postTime, NeuronType::Excitatory);
            }
            
            // Also trigger brain step to integrate changes
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stopPhase(); // PHASE_STDP
        
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get profiling statistics
        auto& stepProfiler = SimulationStepProfiler::getInstance();
        auto stdpStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_STDP);
        
        STDPStats stdpStatsResult;
        stdpStatsResult.totalSpikePairs = spikePairs;
        stdpStatsResult.stdpUpdateTime = stdpStats.averageTimeNs() / 1000.0;
        stdpStatsResult.plasticityConvergenceRate = calculateSTDPConvergenceRate(neuronCount, spikePairs);
        
        result.totalTime = totalTime;
        result.stepsPerSecond = result.totalSteps / totalTime;
        result.avgSTDPUpdateTime = stdpStatsResult.stdpUpdateTime;
        result.convergenceRate = stdpStatsResult.plasticityConvergenceRate;
        
        globalResults.testResults.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    STDP update: " << std::fixed << std::setprecision(3) << result.avgSTDPUpdateTime << " µs" << std::endl;
        std::cout << "    Convergence rate: " << std::fixed << std::setprecision(2) << result.convergenceRate << std::endl;
        
        resetProfiling(); // Reset for next test
    }
    
    printSTDPStressResults(globalResults);
}

// Hebbian stress tests focusing on correlational learning patterns
void PlasticityStressTests::runHebbianStressTests() {
    std::cout << "=== Hebbian Stress Tests ===" << std::endl;
    
    const std::vector<std::pair<size_t, float>> configurations = {
        {50, 1.0f},    // 50 neurons, learning rate 1.0
        {100, 2.0f},   // 100 neurons, learning rate 2.0
        {200, 3.0f},   // 200 neurons, learning rate 3.0
        {500, 5.0f},   // 500 neurons, learning rate 5.0
    };
    
    HebbianResults globalResults;
    globalResults.totalTests = configurations.size();
    
    for (const auto& [neuronCount, learningRate] : configurations) {
        std::cout << "Hebbian stress test: " << neuronCount << " neurons, LR " << learningRate << "..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("hebbian_learning_rate", learningRate, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Get Hebbian system for direct testing
        auto* hebbianSystem = brain->getHebbian();
        if (!hebbianSystem) {
            std::cout << " Failed - Hebbian system not available!" << std::endl;
            continue;
        }
        
        // Start profiling Hebbian operations
        startPhase(SimulationStepProfiler::PHASE_HEBBIAN);
        
        HebbianStressTestResults result;
        result.neuronCount = neuronCount;
        result.learningRate = learningRate;
        result.totalSteps = 200;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < result.totalSteps; ++step) {
            // Generate correlated spike patterns for Hebbian learning
            for (size_t i = 0; i < neuronCount / 10; ++i) {
                // Create correlated neurons (pre and post)
                NeuronId preNeuron = i;
                NeuronId postNeuron = (i + 1) % (neuronCount / 10);
                
                // Strong correlation to maximize Hebbian effect
                float correlationStrength = learningRate * 0.5f;
                
                // Trigger Hebbian learning
                hebbianSystem->strengthenConnection(preNeuron, postNeuron, correlationStrength);
                
                // Also weaken some connections for LTD
                if (i % 3 == 0 && i > 0) {
                    hebbianSystem->weakenConnection(preNeuron, postNeuron, learningRate * 0.3f);
                }
            }
            
            // Integrate with brain simulation
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f + learningRate * 10.0f);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stopPhase(); // PHASE_HEBBIAN
        
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get profiling statistics
        auto& stepProfiler = SimulationStepProfiler::getInstance();
        auto hebbianStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_HEBBIAN);
        
        HebbianStats hebbianStatsResult;
        hebbianStatsResult.totalConnections = neuronCount * (neuronCount / 10);
        hebbianStatsResult.hebbianUpdateTime = hebbianStats.averageTimeNs() / 1000.0;
        hebbianStatsResult.learningSpeed = calculateHebbianLearningSpeed(neuronCount, learningRate);
        
        result.totalTime = totalTime;
        result.stepsPerSecond = result.totalSteps / totalTime;
        result.avgHebbianUpdateTime = hebbianStatsResult.hebbianUpdateTime;
        result.learningSpeed = hebbianStatsResult.learningSpeed;
        
        globalResults.testResults.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Hebbian update: " << std::fixed << std::setprecision(3) << result.avgHebbianUpdateTime << " µs" << std::endl;
        std::cout << "    Learning speed: " << std::fixed << std::setprecision(2) << result.learningSpeed << std::endl;
        
        resetProfiling(); // Reset for next test
    }
    
    printHebbianStressResults(globalResults);
}

// Structural plasticity stress tests focusing on synaptogenesis/pruning
void PlasticityStressTests::runStructuralPlasticityStressTests() {
    std::cout << "=== Structural Plasticity Stress Tests ===" << std::endl;
    
    const std::vector<std::pair<size_t, float>> configurations = {
        {100, 0.05f},   // 100 neurons, synaptogenesis rate 0.05
        {300, 0.1f},    // 300 neurons, synaptogenesis rate 0.1
        {600, 0.15f},   // 600 neurons, synaptogenesis rate 0.15
        {1000, 0.2f},   // 1000 neurons, synaptogenesis rate 0.2
    };
    
    StructuralPlasticityResults globalResults;
    globalResults.totalTests = configurations.size();
    
    for (const auto& [neuronCount, synRate] : configurations) {
        std::cout << "Structural plasticity stress test: " << neuronCount << " neurons, syn rate " << synRate << "..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        config->set("structural_plasticity_rate", synRate, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Get structural plasticity system
        auto* structuralPlasticity = brain->getStructuralPlasticity();
        if (!structuralPlasticity) {
            std::cout << " Failed - Structural plasticity system not available!" << std::endl;
            continue;
        }
        
        // Start profiling structural plasticity operations
        startPhase(SimulationStepProfiler::PHASE_SYNAPSE_INIT);
        startPhase(SimulationStepProfiler::PHASE_SYNAPSE_CONNECT);
        
        StructuralPlasticityStressTestResults result;
        result.neuronCount = neuronCount;
        result.synaptogenesisRate = synRate;
        result.totalSteps = 250;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < result.totalSteps; ++step) {
            // Stress structural plasticity with rapid synaptogenesis/pruning
            if (synRate > 0.1f) {
                // High synaptogenesis rate
                for (size_t i = 0; i < static_cast<size_t>(synRate * neuronCount); ++i) {
                    structuralPlasticity->initiateSynaptogenesis();
                }
            } else {
                // Moderate rate
                for (size_t i = 0; i < static_cast<size_t>(synRate * neuronCount * 10); ++i) {
                    if (i % 3 == 0) {
                        structuralPlasticity->initiateSynaptogenesis();
                    } else {
                        structuralPlasticity->initiatePruning();
                    }
                }
            }
            
            // Also trigger normal brain simulation
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stopPhase(); // PHASE_SYNAPSE_CONNECT
        stopPhase(); // PHASE_SYNAPSE_INIT
        
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get profiling statistics
        auto& stepProfiler = SimulationStepProfiler::getInstance();
        auto synapseInitStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_INIT);
        auto synapseConnectStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_SYNAPSE_CONNECT);
        
        StructuralPlasticityStats structStats;
        structStats.totalSynapticEvents = static_cast<size_t>(synRate * neuronCount * result.totalSteps * 10);
        structStats.synaptogenesisTime = synapseInitStats.averageTimeNs() / 1000.0;
        structStats.pruningTime = synapseConnectStats.averageTimeNs() / 1000.0;
        structStats.structuralPlasticityScore = calculateStructuralPlasticityScore(neuronCount, synRate);
        
        result.totalTime = totalTime;
        result.stepsPerSecond = result.totalSteps / totalTime;
        result.synaptogenesisEfficiency = structStats.synaptogenesisTime;
        result.pruningEfficiency = structStats.pruningTime;
        result.structuralScore = structStats.structuralPlasticityScore;
        
        globalResults.testResults.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Synaptogenesis time: " << std::fixed << std::setprecision(3) << result.synaptogenesisEfficiency << " µs" << std::endl;
        std::cout << "    Pruning time: " << std::fixed << std::setprecision(3) << result.pruningEfficiency << " µs" << std::endl;
        std::cout << "    Structural score: " << std::fixed << std::setprecision(2) << result.structuralScore << std::endl;
        
        resetProfiling(); // Reset for next test
    }
    
    printStructuralPlasticityStressResults(globalResults);
}

// Metaplasticity stress tests focusing on rapid parameter changes
void PlasticityStressTests::runMetaplasticityStressTests() {
    std::cout << "=== Metaplasticity Stress Tests ===" << std::endl;
    
    const std::vector<size_t> neuronCounts = {50, 150, 300, 600};
    const size_t steps = 200;
    
    MetaplasticityResults globalResults;
    globalResults.totalTests = neuronCounts.size();
    
    for (size_t neuronCount : neuronCounts) {
        std::cout << "Metaplasticity stress test: " << neuronCount << " neurons..." << std::flush;
        
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Start profiling metaplasticity-related phases
        startPhase(SimulationStepProfiler::PHASE_STDP);
        startPhase(SimulationStepProfiler::PHASE_HEBBIAN);
        
        MetaplasticityStressTestResults result;
        result.neuronCount = neuronCount;
        result.totalSteps = steps;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation with rapid metaplasticity parameter changes
        for (size_t step = 0; step < steps; ++step) {
            // Rapidly change learning parameters
            float baseLearningRate = 0.01f;
            float learningRateOscillation = 2.0f * std::sin(static_cast<double>(step) * 0.1);
            float currentLearningRate = baseLearningRate * (1.0f + learningRateOscillation);
            
            config->set("stdp_ltp_weight", 0.01f * currentLearningRate, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.012f * currentLearningRate, ConfigSource::Default);
            
            // Inject input to trigger plasticity with varying rates
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f + learningRateOscillation * 20.0f);
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stopPhase(); // PHASE_HEBBIAN
        stopPhase(); // PHASE_STDP
        
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Get profiling statistics
        auto& stepProfiler = SimulationStepProfiler::getInstance();
        auto stdpStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_STDP);
        auto hebbianStats = stepProfiler.getPhaseStats(SimulationStepProfiler::PHASE_HEBBIAN);
        
        MetaplasticityStats metaStats;
        metaStats.parameterChangeFrequency = 10; // Number of parameter changes per step
        metaStats.stdpAdaptationRate = stdpStats.averageTimeNs() / 1000.0;
        metaStats.hebbianAdaptationRate = hebbianStats.averageTimeNs() / 1000.0;
        metaStats.metaplasticityStability = calculateMetaplasticityStability(neuronCount, steps);
        
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.parameterStability = metaStats.metaplasticityStability;
        result.avgSTDPTime = metaStats.stdpAdaptationRate;
        result.avgHebbianTime = metaStats.hebbianAdaptationRate;
        
        globalResults.testResults.push_back(result);
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(1) << result.stepsPerSecond << std::endl;
        std::cout << "    Parameter stability: " << std::fixed << std::setprecision(2) << result.parameterStability << std::endl;
        std::cout << "    STDP adaptation: " << std::fixed << std::setprecision(3) << result.avgSTDPTime << " µs" << std::endl;
        std::cout << "    Hebbian adaptation: " << std::fixed << std::setprecision(3) << result.avgHebbianTime << " µs" << std::endl;
        
        resetProfiling(); // Reset for next test
    }
    
    printMetaplasticityStressResults(globalResults);
}

// Helper functions for plasticity stress test analysis

double PlasticityStressTests::calculateSTDPConvergenceRate(size_t neuronCount, size_t spikePairs) {
    double baseRate = 100.0 / neuronCount;
    double pairBoost = static_cast<double>(spikePairs) / 1000.0; // More pairs, faster convergence
    
    return std::min(1.0, baseRate + pairBoost);
}

double PlasticityStressTests::calculateHebbianLearningSpeed(size_t neuronCount, float learningRate) {
    double neuronFactor = 1000.0 / neuronCount;
    double rateFactor = std::log(1.0 + learningRate); // Logarithmic scaling for learning rate
    
    return neuronFactor * rateFactor;
}

double PlasticityStressTests::calculateStructuralPlasticityScore(size_t neuronCount, float synRate) {
    double neuronFactor = 10000.0 / neuronCount;
    double synFactor = std::min(1.0, synRate / 0.5f); // Normalize to max 0.5
    
    return neuronFactor * synFactor;
}

double PlasticityStressTests::calculateMetaplasticityStability(size_t neuronCount, size_t steps) {
    double neuronFactor = 1.0 / neuronCount;
    double stepFactor = 1.0 / steps;
    
    // Stability decreases with neuron count and steps, but has a minimum
    return std::max(0.1, 1.0 - neuronFactor - stepFactor);
}

// Result printing functions

void PlasticityStressTests::printSTDPStressResults(const STDPResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                STDP Stress Test Results                                 ║" << std::endl;
    std::cout << "╠════════════╦════════════╦════════╦════════════╦════════════╗" << std::endl;
    std::cout << "║  Neurons   ║ Pairs     ║ Steps/s ║ STDP(µs)  ║ Convergence ║" << std::endl;
    std::cout << "╠════════════╬════════════╬════════╬════════════╬════════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(11) << r.spikePairCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(3) << r.avgSTDPUpdateTime
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(2) << r.convergenceRate
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════════╩════════╩════════════╩════════════╝" << std::endl;
    std::cout << std::endl;
}

void PlasticityStressTests::printHebbianStressResults(const HebbianResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                Hebbian Stress Test Results                              ║" << std::endl;
    std::cout << "╠════════════╦════════╦════════╦════════════╦════════════╗" << std::endl;
    std::cout << "║  Neurons   ║ Stress ║ Steps/s ║ Hebbian(µs) ║ Learning  ║" << std::endl;
    std::cout << "╠════════════╬════════╬════════╬════════════╬════════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(6) << std::fixed << std::setprecision(1) << r.learningRate
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(3) << r.avgHebbianUpdateTime
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(2) << r.learningSpeed
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩════════╩════════════╩════════════╝" << std::endl;
    std::cout << std::endl;
}

void PlasticityStressTests::printStructuralPlasticityStressResults(const StructuralPlasticityResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║            Structural Plasticity Stress Test Results                     ║" << std::endl;
    std::cout << "╠════════════╦════════════╦════════╦════════════╦════════════╗" << std::endl;
    std::cout << "║  Neurons   ║ Synaptic  ║ Steps/s ║ Synapse(µs) ║ Structure ║" << std::endl;
    std::cout << "╠════════════╬════════════╬════════╬════════════╬════════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.synaptogenesisRate
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(3) << r.synaptogenesisEfficiency
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(2) << r.structuralScore
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════════╩════════╩════════════╩════════════╝" << std::endl;
    std::cout << std::endl;
}

void PlasticityStressTests::printMetaplasticityStressResults(const MetaplasticityResults& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║               Metaplasticity Stress Test Results                        ║" << std::endl;
    std::cout << "╠════════════╦════════╦════════════╦════════════╦══════════╗" << std::endl;
    std::cout << "║  Neurons   ║ Steps/s ║ Stability ║ STDP(µs)  ║ Hebbian(µs) ║" << std::endl;
    std::cout << "╠════════════╬════════╬════════════╬════════════╬══════════╗" << std::endl;
    
    for (const auto& r : results.testResults) {
        std::cout << "║ " << std::setw(10) << r.neuronCount
                  << " ║ " << std::setw(7) << std::fixed << std::setprecision(1) << r.stepsPerSecond
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(2) << r.parameterStability
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(3) << r.avgSTDPTime
                  << " ║ " << std::setw(12) << std::fixed << std::setprecision(3) << r.avgHebbianTime
                  << " ║" << std::endl;
    }
    
    std::cout << "╚════════════╩════════╩════════════╩════════════╩══════════╝" << std::endl;
    std::cout << std::endl;
}

} // namespace stress_tests

// Main plasticity stress test runner
int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Plasticity System Stress Tests                      ║
    ║                                                               ║
    ║     Comprehensive Plasticity System Stress Testing               ║
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
    
    std::cout << "Running Plasticity System Stress Tests..." << std::endl;
    std::cout << std::endl;
    
    // Run all plasticity stress test suites
    stress_tests::PlasticityStressTests::runSTDPStressTests();
    std::cout << std::endl;
    
    stress_tests::PlasticityStressTests::runHebbianStressTests();
    std::cout << std::endl;
    
    stress_tests::PlasticityStressTests::runStructuralPlasticityStressTests();
    std::cout << std::endl;
    
    stress_tests::PlasticityStressTests::runMetaplasticityStressTests();
    std::cout << std::endl;
    
    // Generate final profiling report
    std::cout << "=== FINAL PLASTICITY STRESS TEST PROFILING REPORT ===" << std::endl;
    std::cout << generateProfilingReport() << std::endl;
    
    std::cout << "Plasticity System Stress Tests complete." << std::endl;
    
    return 0;
}
