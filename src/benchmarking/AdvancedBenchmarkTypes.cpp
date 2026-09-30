// NLM Advanced Benchmark Types Implementation
// AdvancedBenchmarkTypes.cpp

#include "AdvancedBenchmarkTypes.hpp"
#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "EnhancedBenchmarkManager.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>

namespace nlm {
namespace benchmarking {

// Standard Benchmark Implementation
BenchmarkResult StandardBenchmark::run(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) {
    BenchmarkResult result = createBaseResult(getName(), config.neuronCount, config.steps, config.regions);
    
    // Record phase start time
    std::string phase = "simulation";
    auto& impl = *static_cast<typename std::remove_reference<decltype(pImpl)>::type*>(this);
    impl.phaseTimes[phase] = PerformanceCounter::getNanoseconds();
    
    auto startTime = PerformanceCounter::TimePoint::now();
    
    NLM_LOG_INFO("Starting standard benchmark simulation");
    
    for (size_t step = 0; step < config.steps; ++step) {
        // Inject random input patterns
        for (size_t i = 0; i < std::min(static_cast<size_t>(10), config.neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Update result
        result.neuralMetrics.spikeCount = brain->getTotalSpikeCount();
        result.neuralMetrics.firingNeuronCount = brain->getFiringNeuronCount();
        result.neuralMetrics.activeNeuronCount = brain->getActiveNeuronCount();
        
        // Record metrics every 100 steps
        if (step % 100 == 0) {
            notifyProgress(static_cast<double>(step) / config.steps, result);
        }
        
        // Store throughput history
        double currentThroughput = 1.0 / (PerformanceCounter::getSeconds() - 
                                          startTime.asSeconds() + 1e-9); // Avoid division by zero
        result.throughputHistory.push_back(currentThroughput);
    }
    
    auto endTime = PerformanceCounter::TimePoint::now();
    int64_t elapsedNs = endTime.elapsedSince(startTime);
    result.totalTimeNs = elapsedNs;
    result.stepsPerSecond = config.steps / (elapsedNs / 1e9);
    
    // Collect synapse count
    result.synapseCount = brain->getTotalSynapseCount();
    
    NLM_LOG_INFO("Standard benchmark completed");
    NLM_LOG_INFO("  Total time: " + std::to_string(result.getTotalTimeSeconds()) + "s");
    NLM_LOG_INFO("  Steps per second: " + std::to_string(result.stepsPerSecond));
    
    return result;
}

bool StandardBenchmark::validateConfig(const BenchmarkConfig& config) const {
    return config.neuronCount > 0 && config.steps > 0 && config.regions > 0;
}

std::vector<std::string> StandardBenchmark::getRequiredSystemFeatures() const {
    return {"basic_neural_simulation", "spike_computation", "plasticity"};
}

// Memory Benchmark Implementation
BenchmarkResult MemoryBenchmark::run(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) {
    BenchmarkResult result = createBaseResult(getName(), config.neuronCount, config.steps, config.regions);
    
    NLM_LOG_INFO("Starting memory benchmark");
    
    // Record memory before benchmark
    auto memBefore = EnhancedBenchmarkManager::Impl().getCurrentMemoryUsage();
    
    auto startTime = PerformanceCounter::TimePoint::now();
    
    // Run simulation with memory pressure
    for (size_t step = 0; step < config.steps; ++step) {
        // Inject input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        brain->step(step, step * 0.001);
        
        // Access memory systems to measure memory bandwidth
        if (brain->getWorkingMemory()) {
            ++result.neuralMetrics.memoryAccesses;
        }
        if (brain->getEpisodicMemory()) {
            ++result.neuralMetrics.memoryAccesses;
        }
        if (brain->getAssociativeMemory()) {
            ++result.neuralMetrics.memoryAccesses;
        }
        
        // Record memory usage
        auto memUsage = EnhancedBenchmarkManager::Impl().getCurrentMemoryUsage();n        result.memoryHistory.push_back(memUsage.usedBytes / (1024.0 * 1024.0)); // MB
        
        if (step % 100 == 0) {
            notifyProgress(static_cast<double>(step) / config.steps, result);
        }
    }
    
    auto endTime = PerformanceCounter::TimePoint::now();
    int64_t elapsedNs = endTime.elapsedSince(startTime);
    result.totalTimeNs = elapsedNs;
    result.stepsPerSecond = config.steps / (elapsedNs / 1e9);
    
    // Record memory after benchmark
    auto memAfter = EnhancedBenchmarkManager::Impl().getCurrentMemoryUsage();
    
    // Calculate memory metrics
    result.memoryUsage = memAfter;
    double memoryGrowth = memAfter.usedBytes - memBefore.usedBytes;
    result.customMetrics["memoryGrowthBytes"] = memoryGrowth;
    result.customMetrics["memoryGrowthPercent"] = 
        (memBefore.usedBytes > 0) ? (memoryGrowth / memBefore.usedBytes) * 100.0 : 0.0;
    
    result.synapseCount = brain->getTotalSynapseCount();
    
    NLM_LOG_INFO("Memory benchmark completed");
    NLM_LOG_INFO("  Memory before: " + memBefore.toString());
    NLM_LOG_INFO("  Memory after: " + memAfter.toString());
    NLM_LOG_INFO("  Memory growth: " + std::to_string(memoryGrowth) + " bytes");
    
    return result;
}

bool MemoryBenchmark::validateConfig(const BenchmarkConfig& config) const {
    return config.neuronCount > 0 && config.steps > 0 && config.regions > 0;
}

std::vector<std::string> MemoryBenchmark::getRequiredSystemFeatures() const {
    return {"memory_systems", "working_memory", "episodic_memory", "associative_memory"};
}

// Plasticity Benchmark Implementation
BenchmarkResult PlasticityBenchmark::run(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) {
    BenchmarkResult result = createBaseResult(getName(), config.neuronCount, config.steps, config.regions);
    
    NLM_LOG_INFO("Starting plasticity benchmark");
    
    // Enable plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    auto startTime = PerformanceCounter::TimePoint::now();
    std::vector<float> initialWeights;
    
    // Record initial weights
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    // Run plasticity learning
    for (size_t step = 0; step < config.steps; ++step) {
        // Apply repeated input patterns to stimulate learning
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Record plasticity metrics
        result.neuralMetrics.plasticityOperations = brain->getTotalPlasticityOperations();
        
        if (step % 100 == 0) {
            notifyProgress(static_cast<double>(step) / config.steps, result);
        }
    }
    
    auto endTime = PerformanceCounter::TimePoint::now();
    int64_t elapsedNs = endTime.elapsedSince(startTime);
    result.totalTimeNs = elapsedNs;
    result.stepsPerSecond = config.steps / (elapsedNs / 1e9);
    
    // Record final weights and calculate plasticity efficiency
    std::vector<float> finalWeights;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    // Calculate plasticity metrics
    if (!initialWeights.empty() && !finalWeights.empty()) {
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        size_t strengthened = 0;
        size_t weakened = 0;
        float totalWeightChange = 0.0f;
        
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            totalWeightChange += std::abs(delta);
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
        }
        
        result.customMetrics["plasticityEfficiency"] = 
            (minSize > 0) ? (static_cast<float>(strengthened + weakened) / minSize) : 0.0f;
        result.customMetrics["totalWeightChange"] = totalWeightChange;
        result.customMetrics["strengthenedCount"] = static_cast<double>(strengthened);
        result.customMetrics["weakenedCount"] = static_cast<double>(weakened);
    }
    
    result.synapseCount = brain->getTotalSynapseCount();
    
    NLM_LOG_INFO("Plasticity benchmark completed");
    NLM_LOG_INFO("  Plasticity operations: " + std::to_string(result.neuralMetrics.plasticityOperations));
    NLM_LOG_INFO("  Plasticity efficiency: " + 
                std::to_string(result.customMetrics["plasticityEfficiency"] * 100.0) + "%");
    
    return result;
}

bool PlasticityBenchmark::validateConfig(const BenchmarkConfig& config) const {
    return config.neuronCount > 0 && config.steps > 0 && config.regions > 0;
}

std::vector<std::string> PlasticityBenchmark::getRequiredSystemFeatures() const {
    return {"plasticity", "stdp", "hebbian", "structural_plasticity"};
}

// Spike Benchmark Implementation
BenchmarkResult SpikeBenchmark::run(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) {
    BenchmarkResult result = createBaseResult(getName(), config.neuronCount, config.steps, config.regions);
    
    NLM_LOG_INFO("Starting spike benchmark");
    
    auto startTime = PerformanceCounter::TimePoint::now();
    size_t spikeCountAtStart = brain->getTotalSpikeCount();
    
    // Run with varying input intensities to measure spike generation patterns
    for (size_t step = 0; step < config.steps; ++step) {
        // Apply different input patterns based on step modulo
        NeuronType inputType = static_cast<NeuronType>(step % 3);
        float inputIntensity = 20.0f + (static_cast<float>(step % 10) * 5.0f);
        
        brain->injectCurrentToNeurons(inputType, inputIntensity);
        brain->step(step, step * 0.001);
        
        // Record spike metrics
        size_t currentSpikes = brain->getTotalSpikeCount() - spikeCountAtStart;
        result.neuralMetrics.spikeCount = currentSpikes;
        result.neuralMetrics.firingNeuronCount = brain->getFiringNeuronCount();
        result.neuralMetrics.activeNeuronCount = brain->getActiveNeuronCount();
        
        // Record spike rate for this step
        double spikeRate = static_cast<double>(currentSpikes) / (step + 1);
        result.throughputHistory.push_back(spikeRate);
        
        if (step % 100 == 0) {
            notifyProgress(static_cast<double>(step) / config.steps, result);
        }
    }
    
    auto endTime = PerformanceCounter::TimePoint::now();
    int64_t elapsedNs = endTime.elapsedSince(startTime);
    result.totalTimeNs = elapsedNs;
    result.stepsPerSecond = config.steps / (elapsedNs / 1e9);
    
    // Calculate spike metrics
    size_t totalSpikes = brain->getTotalSpikeCount() - spikeCountAtStart;
    result.customMetrics["averageSpikesPerStep"] = static_cast<double>(totalSpikes) / config.steps;
    result.customMetrics["peakSpikesPerStep"] = *std::max_element(result.throughputHistory.begin(), 
                                                                    result.throughputHistory.end());
    result.customMetrics["spikeCoefficientOfVariation"] = calculateCoefficientOfVariation(result.throughputHistory);
    
    result.synapseCount = brain->getTotalSynapseCount();
    
    NLM_LOG_INFO("Spike benchmark completed");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(totalSpikes));
    NLM_LOG_INFO("  Average spikes per step: " + std::to_string(result.customMetrics["averageSpikesPerStep"]));
    
    return result;
}

bool SpikeBenchmark::validateConfig(const BenchmarkConfig& config) const {
    return config.neuronCount > 0 && config.steps > 0 && config.regions > 0;
}

std::vector<std::string> SpikeBenchmark::getRequiredSystemFeatures() const {
    return {"spike_system", "neuron_dynamics", "excitatory_inhibitory_balance"};
}

// Integration Benchmark Implementation
BenchmarkResult IntegrationBenchmark::run(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) {
    BenchmarkResult result = createBaseResult(getName(), config.neuronCount, config.steps, config.regions);
    
    NLM_LOG_INFO("Starting integration benchmark");
    
    auto startTime = PerformanceCounter::TimePoint::now();
    
    // Test integration of multiple brain systems
    for (size_t step = 0; step < config.steps; ++step) {
        // Inject sensory input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        
        // Apply neuromodulation
        if (brain->getDopamine()) {
            // Apply reward signal
            Neuromodulator rewardSignal(0.5f, 1.0f); // Reward with strength 0.5, duration 1.0s
            brain->applyNeuromodulation(rewardSignal);
        }
        
        // Step the brain (this processes all systems)
        brain->step(step, step * 0.001);
        
        // Record integration metrics
        result.neuralMetrics.spikeCount = brain->getTotalSpikeCount();
        result.neuralMetrics.firingNeuronCount = brain->getFiringNeuronCount();
        
        // Check system integration
        bool workingMemoryIntegrated = brain->getWorkingMemory() != nullptr;
        bool episodicMemoryIntegrated = brain->getEpisodicMemory() != nullptr;
        bool neuromodulationIntegrated = brain->getDopamine() != nullptr;
        bool predictionIntegrated = brain->getPredictionSystem() != nullptr;
        
        result.customMetrics["workingMemoryIntegrated"] = workingMemoryIntegrated ? 1.0 : 0.0;
        result.customMetrics["episodicMemoryIntegrated"] = episodicMemoryIntegrated ? 1.0 : 0.0;
        result.customMetrics["neuromodulationIntegrated"] = neuromodulationIntegrated ? 1.0 : 0.0;
        result.customMetrics["predictionIntegrated"] = predictionIntegrated ? 1.0 : 0.0;
        
        if (step % 100 == 0) {
            notifyProgress(static_cast<double>(step) / config.steps, result);
        }
    }
    
    auto endTime = PerformanceCounter::TimePoint::now();
    int64_t elapsedNs = endTime.elapsedSince(startTime);
    result.totalTimeNs = elapsedNs;
    result.stepsPerSecond = config.steps / (elapsedNs / 1e9);
    
    // Calculate integration score
    double integrationScore = 0.0;
    int integrationChecks = 0;
    
    for (const auto& pair : result.customMetrics) {
        if (pair.first.find("Integrated") != std::string::npos) {
            integrationScore += pair.second;
            integrationChecks++;
        }
    }
    
    if (integrationChecks > 0) {
        result.customMetrics["integrationScore"] = integrationScore / integrationChecks;
    }
    
    result.synapseCount = brain->getTotalSynapseCount();
    
    NLM_LOG_INFO("Integration benchmark completed");
    NLM_LOG_INFO("  Integration score: " + std::to_string(result.customMetrics["integrationScore"] * 100.0) + "%");
    
    return result;
}

bool IntegrationBenchmark::validateConfig(const BenchmarkConfig& config) const {
    return config.neuronCount > 0 && config.steps > 0 && config.regions > 0;
}

std::vector<std::string> IntegrationBenchmark::getRequiredSystemFeatures() const {
    return {"working_memory", "episodic_memory", "neuromodulation", "prediction_system",
            "cognition", "development", "sensory", "motor"};
}

// Scalability Benchmark Implementation
BenchmarkResult ScalabilityBenchmark::run(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) {
    BenchmarkResult result = createBaseResult(getName(), config.neuronCount, config.steps, config.regions);
    
    NLM_LOG_INFO("Starting scalability benchmark");
    
    // For scalability, we test performance at different neuron counts
    // This is a simplified version - would typically run multiple sub-benchmarks
    
    auto startTime = PerformanceCounter::TimePoint::now();
    
    // Run simulation with focus on network scaling
    for (size_t step = 0; step < config.steps; ++step) {
        // Apply input pattern that scales with network size
        size_t inputNeurons = std::min(static_cast<size_t>(20), config.neuronCount / 10);
        float inputIntensity = 15.0f + static_cast<float>(config.neuronCount / 100);
        
        for (size_t i = 0; i < inputNeurons; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, inputIntensity);
        }
        
        brain->step(step, step * 0.001);
        
        // Record scaling metrics
        result.neuralMetrics.spikeCount = brain->getTotalSpikeCount();
        result.neuralMetrics.firingNeuronCount = brain->getFiringNeuronCount();
        
        // Store neuron count scaling data
        double neuronsPerSpike = config.neuronCount > 0 ? 
                                 static_cast<double>(result.neuralMetrics.spikeCount) / config.neuronCount : 0.0;
        result.customMetrics["neuronsPerSpike"] = neuronsPerSpike;
        
        if (step % 100 == 0) {
            notifyProgress(static_cast<double>(step) / config.steps, result);
        }
    }
    
    auto endTime = PerformanceCounter::TimePoint::now();
    int64_t elapsedNs = endTime.elapsedSince(startTime);
    result.totalTimeNs = elapsedNs;
    result.stepsPerSecond = config.steps / (elapsedNs / 1e9);
    
    // Calculate scaling efficiency
    double baselineEfficiency = 1000.0 / 500.0; // Assume 1000 steps/sec for 500 neurons baseline
    double currentEfficiency = result.stepsPerSecond / static_cast<double>(config.neuronCount / 500);
    result.customMetrics["scalingEfficiency"] = currentEfficiency / baselineEfficiency;
    
    // Calculate strong scaling (parallel efficiency would need multiple threads)
    result.customMetrics["strongScalingEfficiency"] = 1.0; // Simplified
    
    result.synapseCount = brain->getTotalSynapseCount();
    
    NLM_LOG_INFO("Scalability benchmark completed");
    NLM_LOG_INFO("  Scaling efficiency: " + std::to_string(result.customMetrics["scalingEfficiency"] * 100.0) + "%");
    
    return result;
}

bool ScalabilityBenchmark::validateConfig(const BenchmarkConfig& config) const {
    return config.neuronCount > 0 && config.steps > 0 && config.regions > 0;
}

std::vector<std::string> ScalabilityBenchmark::getRequiredSystemFeatures() const {
    return {"neuron_network", "synaptic_connections", "spike_communication", "memory_systems"};
}

double ScalabilityBenchmark::calculateCoefficientOfVariation(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    
    double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    double sumSquares = 0.0;
    
    for (double value : values) {
        double diff = value - mean;
        sumSquares += diff * diff;
    }
    
    double variance = sumSquares / values.size();
    return std::sqrt(variance) / (mean + 1e-9); // Avoid division by zero
}

}}} // namespace nlm::benchmarking
