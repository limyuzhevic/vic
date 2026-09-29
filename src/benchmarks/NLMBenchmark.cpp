#include "benchmarks/NLMBenchmark.hpp"
#include "benchmarks/NeuralBenchmark.hpp"
#include "benchmarks/DevelopmentBenchmark.hpp"
#include "benchmarks/ResourceBenchmark.hpp"
#include "benchmarks/ScalabilityBenchmark.hpp"
#include "core/Logger/Logger.hpp"
#include <chrono>
#include <memory>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace nlm {

// NLM Benchmark Suite Implementation
NLMBenchmark::NLMBenchmark() {
    NLM_LOG_INFO("Initializing NLM Benchmark Suite");
}

NLMBenchmark::~NLMBenchmark() {
    NLM_LOG_INFO("NLM Benchmark Suite shutdown complete");
}

void NLMBenchmark::initialize() {
    NLM_LOG_INFO("NLM Benchmark Suite initialized");
    startWallClock = std::chrono::high_resolution_clock::now();
    startCpuClock = std::chrono::steady_clock::now();
}

void NLMBenchmark::shutdown() {
    auto endWallClock = std::chrono::high_resolution_clock::now();
    auto endCpuClock = std::chrono::steady_clock::now();
    
    wallClockDuration = std::chrono::duration<double>(endWallClock - startWallClock).count();
    cpuDuration = std::chrono::duration<double>(endCpuClock - startCpuClock).count();
    
    NLM_LOG_INFO("NLM Benchmark Suite completed in " + std::to_string(wallClockDuration) + "s (" + std::to_string(cpuDuration) + "s CPU)");
}

BrainBenchmarkResult NLMBenchmark::benchmarkBrainPerformance(std::shared_ptr<Brain> brain, uint64_t steps) {
    BrainBenchmarkResult result;
    result.steps = steps;
    
    NLM_LOG_INFO("Benchmarking brain performance: " + std::to_string(steps) + " steps");
    
    // Record initial state
    result.initialNeuronCount = brain->getTotalNeuronCount();
    result.initialSynapseCount = brain->getTotalSynapseCount();
    result.initialSpikeCount = brain->getTotalSpikeCount();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run simulation
    for (uint64_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        // Collect metrics every 1000 steps
        if (step % 1000 == 0) {
            result.spikesGenerated += brain->getTotalSpikeCount() - result.initialSpikeCount;
            result.firingNeurons += brain->getFiringNeuronCount();
            result.memoryTraces += brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0;
            result.memoryEpisodes += brain->getEpisodicMemory() ? brain->getEpisodicMemory()->getEpisodeCount() : 0;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Calculate metrics
    result.wallClockTime = elapsedTime;
    result.stepsPerSecond = steps / elapsedTime;
    result.finalNeuronCount = brain->getTotalNeuronCount();
    result.finalSynapseCount = brain->getTotalSynapseCount();
    result.finalSpikeCount = brain->getTotalSpikeCount();
    result.spikesGenerated = result.finalSpikeCount - result.initialSpikeCount;
    result.averageFiringRate = static_cast<double>(result.firingNeurons) / steps;
    result.averageMemoryTraces = static_cast<double>(result.memoryTraces) / steps;
    result.averageMemoryEpisodes = static_cast<double>(result.memoryEpisodes) / steps;
    
    NLM_LOG_INFO("Brain benchmark completed: " + std::to_string(steps) + " steps in " + std::to_string(elapsedTime) + "s at " + std::to_string(result.stepsPerSecond) + " steps/s");
    
    return result;
}

NeuralBenchmarkResult NLMBenchmark::benchmarkNeuralPerformance(std::shared_ptr<Brain> brain, uint64_t steps) {
    NeuralBenchmarkResult result;
    result.steps = steps;
    
    NLM_LOG_INFO("Benchmarking neural performance: " + std::to_string(steps) + " steps");
    
    // Enable plasticity for benchmarking
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Record initial weights
    result.initialWeights.clear();
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            result.initialWeights.push_back(syn->getWeight());
        }
    }
    
    // Run simulation with stimuli to drive plasticity
    for (uint64_t step = 0; step < steps; ++step) {
        // Inject current to stimulate neural activity
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0);
        
        // Apply plasticity updates
        brain->step(step, step * 0.001);
        
        // Record spike metrics
        result.totalSpikes += brain->getTotalSpikeCount();
        result.firingNeurons += brain->getFiringNeuronCount();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Record final weights
    result.finalWeights.clear();
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            result.finalWeights.push_back(syn->getWeight());
        }
    }
    
    // Calculate neural performance metrics
    result.wallClockTime = elapsedTime;
    result.stepsPerSecond = steps / elapsedTime;
    result.spikesPerSecond = static_cast<double>(result.totalSpikes) / elapsedTime;
    result.averageFiringRate = static_cast<double>(result.firingNeurons) / steps;
    result.plasticityRate = calculatePlasticityRate(result.initialWeights, result.finalWeights, elapsedTime);
    
    NLM_LOG_INFO("Neural benchmark completed: " + std::to_string(result.plasticityRate) + " plasticity events/s, " + std::to_string(result.spikesPerSecond) + " spikes/s");
    
    return result;
}

DevelopmentBenchmarkResult NLMBenchmark::benchmarkDevelopmentPerformance(std::shared_ptr<Brain> brain, uint64_t steps) {
    DevelopmentBenchmarkResult result;
    result.steps = steps;
    
    NLM_LOG_INFO("Benchmarking development performance: " + std::to_string(steps) + " steps");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Record initial development state
    result.initialStage = brain->getDevelopmentalStage();
    
    // Run simulation
    for (uint64_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        // Track development stage changes
        DevelopmentalStage currentStage = brain->getDevelopmentalStage();
        if (currentStage != result.initialStage) {
            result.stageChanges++;
            result.initialStage = currentStage;
        }
        
        // Track structural plasticity
        if (brain->getStructuralPlasticity()) {
            // Note: This is a simplified tracking - actual structural plasticity
            // would require more detailed integration with the development system
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Calculate development performance metrics
    result.wallClockTime = elapsedTime;
    result.stepsPerSecond = steps / elapsedTime;
    result.finalStage = brain->getDevelopmentalStage();
    result.developmentEfficiency = static_cast<double>(result.stageChanges) / elapsedTime;
    
    NLM_LOG_INFO("Development benchmark completed: " + std::to_string(result.developmentEfficiency) + " stage changes/s, final stage: " + std::to_string(static_cast<int>(result.finalStage)));
    
    return result;
}

ResourceBenchmarkResult NLMBenchmark::benchmarkResourceEfficiency(std::shared_ptr<Brain> brain, uint64_t steps) {
    ResourceBenchmarkResult result;
    result.steps = steps;
    
    NLM_LOG_INFO("Benchmarking resource efficiency: " + std::to_string(steps) + " steps");
    
    // Record initial memory usage (simplified - would use system APIs in real implementation)
    result.initialMemoryUsage = brain->getTotalNeuronCount() * sizeof(Neuron) + 
                               brain->getTotalSynapseCount() * sizeof(Synapse);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run simulation
    for (uint64_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Record final memory usage
    result.finalMemoryUsage = brain->getTotalNeuronCount() * sizeof(Neuron) + 
                             brain->getTotalSynapseCount() * sizeof(Synapse);
    
    // Calculate resource efficiency metrics
    result.wallClockTime = elapsedTime;
    result.stepsPerSecond = steps / elapsedTime;
    result.memoryUsagePerNeuron = static_cast<double>(result.finalMemoryUsage) / brain->getTotalNeuronCount();
    result.memoryUsagePerSynapse = static_cast<double>(result.finalMemoryUsage) / brain->getTotalSynapseCount();
    result.memoryEfficiency = static_cast<double>(brain->getTotalNeuronCount() * sizeof(Neuron)) / result.finalMemoryUsage;
    
    NLM_LOG_INFO("Resource efficiency benchmark completed: " + std::to_string(result.memoryUsagePerNeuron) + " bytes/neuron, " + std::to_string(result.memoryUsagePerSynapse) + " bytes/synapse");
    
    return result;
}

ScalabilityMetrics NLMBenchmark::benchmarkScalability(const ScalabilityBenchmark::ScaleTestConfig& config) {
    ScalabilityMetrics metrics;
    
    NLM_LOG_INFO("Benchmarking scalability with " + std::to_string(config.neuronCounts.size()) + " neuron scales");
    
    // Neuron scaling
    metrics.neuronScaling = benchmarkNeuronScaling(config);
    
    // Region scaling  
    metrics.regionScaling = benchmarkRegionScaling(config);
    
    // Connection scaling
    metrics.connectionScaling = benchmarkConnectionScaling(config);
    
    // Memory scaling
    metrics.memoryScaling = benchmarkMemoryScaling(config);
    
    NLM_LOG_INFO("Scalability benchmark completed with " + std::to_string(metrics.neuronScaling.size()) + " data points per category");
    
    return metrics;
}

double NLMBenchmark::calculatePlasticityRate(const std::vector<float>& initialWeights, 
                                            const std::vector<float>& finalWeights, 
                                            double elapsedTime) {
    if (initialWeights.empty() || finalWeights.empty() || elapsedTime <= 0.0) {
        return 0.0;
    }
    
    // Calculate weight changes
    double totalWeightChange = 0.0;
    size_t changedSynapses = 0;
    
    size_t minSize = std::min(initialWeights.size(), finalWeights.size());
    for (size_t i = 0; i < minSize; ++i) {
        double delta = finalWeights[i] - initialWeights[i];
        totalWeightChange += std::abs(delta);
        if (std::abs(delta) > 0.001f) {
            changedSynapses++;
        }
    }
    
    // Calculate plasticity rate
    double plasticityRate = changedSynapses / elapsedTime;
    
    return plasticityRate;
}

std::vector<double> NLMBenchmark::benchmarkNeuronScaling(const ScalabilityBenchmark::ScaleTestConfig& config) {
    std::vector<double> results;
    results.reserve(config.neuronCounts.size());
    
    for (size_t neuronCount : config.neuronCounts) {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(neuronCount));
        cfg->set("region_count", static_cast<int64_t>(1));
        cfg->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for scaling test with " + std::to_string(neuronCount) + " neurons");
            results.push_back(0.0);
            continue;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation
        for (uint64_t step = 0; step < config.simulationSteps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
        double stepsPerSecond = config.simulationSteps / elapsedTime;
        
        results.push_back(stepsPerSecond);
        NLM_LOG_INFO("Neuron scaling test with " + std::to_string(neuronCount) + " neurons: " + std::to_string(stepsPerSecond) + " steps/s");
    }
    
    return results;
}

std::vector<double> NLMBenchmark::benchmarkRegionScaling(const ScalabilityBenchmark::ScaleTestConfig& config) {
    std::vector<double> results;
    results.reserve(config.regionCounts.size());
    
    for (size_t regionCount : config.regionCounts) {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(1000)); // Fixed neuron count
        cfg->set("region_count", static_cast<int64_t>(regionCount));
        cfg->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for region scaling test with " + std::to_string(regionCount) + " regions");
            results.push_back(0.0);
            continue;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation
        for (uint64_t step = 0; step < config.simulationSteps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
        double stepsPerSecond = config.simulationSteps / elapsedTime;
        
        results.push_back(stepsPerSecond);
        NLM_LOG_INFO("Region scaling test with " + std::to_string(regionCount) + " regions: " + std::to_string(stepsPerSecond) + " steps/s");
    }
    
    return results;
}

std::vector<double> NLMBenchmark::benchmarkConnectionScaling(const ScalabilityBenchmark::ScaleTestConfig& config) {
    std::vector<double> results;
    results.reserve(config.neuronCounts.size());
    
    for (size_t neuronCount : config.neuronCounts) {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(neuronCount));
        cfg->set("region_count", static_cast<int64_t>(1));
        // Vary connection probability
        float connectionProb = std::min(0.5f, static_cast<float>(neuronCount) / 1000.0f);
        cfg->set("connection_probability", connectionProb);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for connection scaling test");
            results.push_back(0.0);
            continue;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation
        for (uint64_t step = 0; step < config.simulationSteps; ++step) {
            brain->step(step, step * 0.001);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
        double stepsPerSecond = config.simulationSteps / elapsedTime;
        
        results.push_back(stepsPerSecond);
        NLM_LOG_INFO("Connection scaling test with " + std::to_string(neuronCount) + " neurons, " + std::to_string(connectionProb) + " connection prob: " + std::to_string(stepsPerSecond) + " steps/s");
    }
    
    return results;
}

std::vector<double> NLMBenchmark::benchmarkMemoryScaling(const ScalabilityBenchmark::ScaleTestConfig& config) {
    std::vector<double> results;
    results.reserve(config.neuronCounts.size());
    
    for (size_t neuronCount : config.neuronCounts) {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(neuronCount));
        cfg->set("region_count", static_cast<int64_t>(1));
        cfg->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for memory scaling test");
            results.push_back(0.0);
            continue;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation
        for (uint64_t step = 0; step < config.simulationSteps; ++step) {
            brain->step(step, step * 0.001);
            
            // Record memory metrics
            if (brain->getWorkingMemory()) {
                result.workingMemoryTraces += brain->getWorkingMemory()->getActiveTraces();
            }
            if (brain->getEpisodicMemory()) {
                result.memoryEpisodes += brain->getEpisodicMemory()->getEpisodeCount();
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
        double stepsPerSecond = config.simulationSteps / elapsedTime;
        
        results.push_back(stepsPerSecond);
        NLM_LOG_INFO("Memory scaling test with " + std::to_string(neuronCount) + " neurons: " + std::to_string(stepsPerSecond) + " steps/s");
    }
    
    return results;
}

void NLMBenchmark::runAllBenchmarks() {
    NLM_LOG_INFO("Starting comprehensive NLM benchmark suite");
    
    // Initialize benchmark suite
    initialize();
    
    try {
        // Create test brain
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 1000);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize test brain for benchmarks");
            return;
        }
        
        uint64_t steps = 5000;
        
        // Run neural benchmarks
        brainBenchmark = benchmarkBrainPerformance(brain, steps);
        
        // Run neural performance benchmarks
        neuralBenchmark = benchmarkNeuralPerformance(brain, steps);
        
        // Run development benchmarks
        developmentBenchmark = benchmarkDevelopmentPerformance(brain, steps);
        
        // Run resource efficiency benchmarks
        resourceBenchmark = benchmarkResourceEfficiency(brain, steps);
        
        // Prepare scalability test config
        ScalabilityBenchmark::ScaleTestConfig scaleConfig;
        scaleConfig.neuronCounts = {100, 500, 1000, 5000};
        scaleConfig.regionCounts = {1, 2, 5};
        scaleConfig.simulationSteps = 1000;
        
        // Run scalability benchmarks
        scalabilityBenchmark = benchmarkScalability(scaleConfig);
        
        NLM_LOG_INFO("All benchmarks completed successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during benchmarking: " + std::string(e.what()));
    }
    
    // Shutdown benchmark suite
    shutdown();
    
    // Print summary
    printBenchmarkSummary();
}

void NLMBenchmark::printBenchmarkSummary() {
    NLM_LOG_INFO("\n" + std::string(60, '='));
    NLM_LOG_INFO("NLM Benchmark Suite Summary");
    NLM_LOG_INFO(std::string(60, '='));
    
    if (brainBenchmark.steps > 0) {
        NLM_LOG_INFO("Brain Performance:");
        NLM_LOG_INFO("  Steps executed: " + std::to_string(brainBenchmark.steps));
        NLM_LOG_INFO("  Wall clock time: " + std::to_string(brainBenchmark.wallClockTime) + "s");
        NLM_LOG_INFO("  Steps per second: " + std::to_string(brainBenchmark.stepsPerSecond));
        NLM_LOG_INFO("  Final neurons: " + std::to_string(brainBenchmark.finalNeuronCount));
        NLM_LOG_INFO("  Final synapses: " + std::to_string(brainBenchmark.finalSynapseCount));
        NLM_LOG_INFO("  Total spikes generated: " + std::to_string(brainBenchmark.spikesGenerated));
        NLM_LOG_INFO("  Average firing rate: " + std::to_string(brainBenchmark.averageFiringRate));
    }
    
    if (neuralBenchmark.steps > 0) {
        NLM_LOG_INFO("Neural Performance:");
        NLM_LOG_INFO("  Wall clock time: " + std::to_string(neuralBenchmark.wallClockTime) + "s");
        NLM_LOG_INFO("  Steps per second: " + std::to_string(neuralBenchmark.stepsPerSecond));
        NLM_LOG_INFO("  Spikes per second: " + std::to_string(neuralBenchmark.spikesPerSecond));
        NLM_LOG_INFO("  Average firing rate: " + std::to_string(neuralBenchmark.averageFiringRate));
        NLM_LOG_INFO("  Plasticity rate: " + std::to_string(neuralBenchmark.plasticityRate));
    }
    
    if (developmentBenchmark.steps > 0) {
        NLM_LOG_INFO("Development Performance:");
        NLM_LOG_INFO("  Wall clock time: " + std::to_string(developmentBenchmark.wallClockTime) + "s");
        NLM_LOG_INFO("  Steps per second: " + std::to_string(developmentBenchmark.stepsPerSecond));
        NLM_LOG_INFO("  Development efficiency: " + std::to_string(developmentBenchmark.developmentEfficiency));
        NLM_LOG_INFO("  Stage changes: " + std::to_string(developmentBenchmark.stageChanges));
    }
    
    if (resourceBenchmark.steps > 0) {
        NLM_LOG_INFO("Resource Efficiency:");
        NLM_LOG_INFO("  Wall clock time: " + std::to_string(resourceBenchmark.wallClockTime) + "s");
        NLM_LOG_INFO("  Steps per second: " + std::to_string(resourceBenchmark.stepsPerSecond));
        NLM_LOG_INFO("  Memory per neuron: " + std::to_string(resourceBenchmark.memoryUsagePerNeuron) + " bytes");
        NLM_LOG_INFO("  Memory per synapse: " + std::to_string(resourceBenchmark.memoryUsagePerSynapse) + " bytes");
        NLM_LOG_INFO("  Memory efficiency: " + std::to_string(resourceBenchmark.memoryEfficiency));
    }
    
    if (!scalabilityBenchmark.neuronScaling.empty()) {
        NLM_LOG_INFO("Scalability (Neuron Scaling):");
        NLM_LOG_INFO("  Data points: " + std::to_string(scalabilityBenchmark.neuronScaling.size()));
        double avgPerformance = std::accumulate(scalabilityBenchmark.neuronScaling.begin(), 
                                               scalabilityBenchmark.neuronScaling.end(), 0.0) / 
                                scalabilityBenchmark.neuronScaling.size();
        NLM_LOG_INFO("  Average performance: " + std::to_string(avgPerformance) + " steps/s");
    }
    
    NLM_LOG_INFO(std::string(60, '='));
    NLM_LOG_INFO("Benchmark Suite Complete");
    NLM_LOG_INFO(std::string(60, '='));
}

void NLMBenchmark::printDetailedResults() {
    NLM_LOG_INFO("Detailed benchmark results would be written to file in production environment.");
    NLM_LOG_INFO("For detailed analysis, use the Python interface or export to JSON/CSV format.");
}

// Standalone benchmark function
void runNLMBenchmarks() {
    NLM_LOG_INFO("Starting NLM Benchmark Suite");
    
    NLMBenchmark benchmark;
    benchmark.runAllBenchmarks();
}

} // namespace nlm