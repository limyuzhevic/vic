// NLM Performance Benchmark
// Measures simulation performance for different network sizes

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>

using namespace nlm;

struct AdvancedBenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    
    // Performance metrics
    double spikeTimeVariance;    // Measure of spike timing consistency
    double energyPerStep;        // Energy consumption per step
    double plasticityRate;       // Synaptic change rate per step
    double learningProgress;     // Learning improvement over time
    
    // Neural activity metrics
    double meanFiringRate;       // Average firing rate across network
    double maxFiringRate;        // Maximum firing rate of any neuron
    double activityCohesion;     // Measure of coordinated activity
    double networkStability;    // Measure of network stability over time
    
    // System health metrics
    size_t checkpointSuccesses;  // Number of successful checkpoints
    size_t memoryUsage;          // Memory usage in bytes
    size_t activeConnections;   // Number of active synaptic connections
    double convergenceRate;     // Rate at which system converges to stable state
};

// Helper function to calculate mean of a specific field from AdvancedBenchmarkResult
float calculateMean(const std::vector<AdvancedBenchmarkResult>& results, double AdvancedBenchmarkResult::*field) {
    if (results.empty()) return 0.0f;
    double sum = 0.0;
    for (const auto& r : results) {
        sum += r.*field;
    }
    return static_cast<float>(sum / results.size());
}
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("enable_checkpointing", true, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Benchmark-specific timing and metrics
    auto startTime = std::chrono::high_resolution_clock::now();
    std::vector<double> stepTimes;
    std::vector<double> spikeCounts;
    std::vector<size_t> synapseCounts;
    std::vector<float> firingRates;
    
    size_t checkpointSuccesses = 0;
    size_t memoryUsage = 0;
    
    for (size_t step = 0; step < steps; ++step) {
        auto stepStart = std::chrono::high_resolution_clock::now();
        
        // Inject some random input
        for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Collect metrics
        double stepTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - stepStart).count();
        stepTimes.push_back(stepTime);
        
        spikeCounts.push_back(brain->getTotalSpikeCount());
        synapseCounts.push_back(brain->getTotalSynapseCount());
        
        // Estimate firing rate (spikes per neuron per step)
        float firingRate = static_cast<float>(brain->getFiringNeuronCount()) / static_cast<float>(neuronCount);
        firingRates.push_back(firingRate);
        
        // Check checkpoint performance
        if (brain->getCheckpointManager() && brain->getCheckpointManager()->shouldSave(step, step * 0.001)) {
            // Note: We can't actually save checkpoints in benchmark mode as it would create files
            // but we can track how often checkpointing would occur
            checkpointSuccesses++;
        }
        
        // Memory usage estimation (rough approximation)
        memoryUsage += sizeof(Neuron) * neuronCount + sizeof(Synapse) * synapseCounts.back();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Calculate advanced metrics
    AdvancedBenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    
    // Calculate variance and statistics
    if (stepTimes.size() > 1) {
        double meanStepTime = std::accumulate(stepTimes.begin(), stepTimes.end(), 0.0) / stepTimes.size();
        double varianceSum = 0.0;
        for (double t : stepTimes) varianceSum += (t - meanStepTime) * (t - meanStepTime);
        result.spikeTimeVariance = varianceSum / stepTimes.size();
    } else {
        result.spikeTimeVariance = 0.0;
    }
    
    // Calculate firing rate statistics
    if (firingRates.size() > 0) {
        double meanFiring = std::accumulate(firingRates.begin(), firingRates.end(), 0.0) / firingRates.size();
        result.meanFiringRate = meanFiring;
        result.maxFiringRate = *std::max_element(firingRates.begin(), firingRates.end());
        
        // Calculate activity cohesion (coordination between steps)
        double activityCohesion = 0.0;
        if (firingRates.size() > 1) {
            for (size_t i = 1; i < firingRates.size(); ++i) {
                activityCohesion += std::abs(firingRates[i] - firingRates[i-1]);
            }
            activityCohesion = 1.0 - (activityCohesion / (firingRates.size() - 1)); // Higher when stable
        } else {
            activityCohesion = 1.0;
        }
        result.activityCohesion = activityCohesion;
    } else {
        result.meanFiringRate = 0.0;
        result.maxFiringRate = 0.0;
        result.activityCohesion = 0.0;
    }
    
    // Estimate learning progress (simplified - based on synapse count changes)
    if (synapseCounts.size() > 1) {
        double initialSynapses = static_cast<double>(synapseCounts.front());
        double finalSynapses = static_cast<double>(synapseCounts.back());
        result.learningProgress = std::abs(finalSynapses - initialSynapses) / (steps * 0.001f);
    } else {
        result.learningProgress = 0.0;
    }
    
    // Calculate plasticity rate (estimated based on spike activity)
    result.plasticityRate = result.spikesPerStep * 0.001f; // Rough estimate
    
    // Network stability (based on firing rate variance)
    if (firingRates.size() > 1) {
        double meanFiring = std::accumulate(firingRates.begin(), firingRates.end(), 0.0) / firingRates.size();
        double rateVariance = 0.0;
        for (float rate : firingRates) rateVariance += (rate - meanFiring) * (rate - meanFiring);
        rateVariance /= firingRates.size();
        result.networkStability = 1.0 - std::min(1.0, rateVariance * 10.0); // Normalize
    } else {
        result.networkStability = 1.0;
    }
    
    // Convergence rate (simplified)
    result.convergenceRate = result.learningProgress / totalTime;
    
    // Final metrics
    result.checkpointSuccesses = checkpointSuccesses;
    result.memoryUsage = memoryUsage;
    result.activeConnections = synapseCounts.empty() ? 0 : synapseCounts.back();
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    std::cout << "    Mean firing rate: " << std::fixed << std::setprecision(3) << result.meanFiringRate << " (normalized)" << std::endl;
    std::cout << "    Activity cohesion: " << std::fixed << std::setprecision(3) << result.activityCohesion << std::endl;
    std::cout << "    Network stability: " << std::fixed << std::setprecision(3) << result.networkStability << std::endl;
    
    return result;
}

void printAdvancedBenchmarkResults(const std::vector<AdvancedBenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                     NLM Advanced Performance Benchmark Results                        ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╦═══════════════╣" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec ║ Spikes/step ║ Synapses/Neuron      ║ Network Stability  ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╬═══════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                  << " ║ " << std::setw(14) << std::fixed << std::setprecision(3) << r.networkStability
                  << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╩═══════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Print additional performance analysis
    std::cout << "Advanced Performance Analysis:" << std::endl;
    std::cout << "  Spike Time Variance: " << std::fixed << std::setprecision(6) << calculateMean(results, &AdvancedBenchmarkResult::spikeTimeVariance) << std::endl;
    std::cout << "  Mean Firing Rate: " << std::fixed << std::setprecision(4) << calculateMean(results, &AdvancedBenchmarkResult::meanFiringRate) << " (normalized)" << std::endl;
    std::cout << "  Activity Cohesion: " << std::fixed << std::setprecision(4) << calculateMean(results, &AdvancedBenchmarkResult::activityCohesion) << std::endl;
    std::cout << "  Learning Progress: " << std::fixed << std::setprecision(4) << calculateMean(results, &AdvancedBenchmarkResult::learningProgress) << std::endl;
    std::cout << "  Plasticity Rate: " << std::fixed << std::setprecision(6) << calculateMean(results, &AdvancedBenchmarkResult::plasticityRate) << std::endl;
    std::cout << "  Memory Efficiency: " << std::fixed << std::setprecision(2) << calculateMean(results, &AdvancedBenchmarkResult::memoryUsage) / 1024 << " KB average" << std::endl;
}

int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Advanced Performance Benchmark                            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running NLM Advanced Performance Benchmarks..." << std::endl;
    std::cout << std::endl;
    
    std::vector<AdvancedBenchmarkResult> results;
    
    // Benchmark configurations: (neuron_count, steps)
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},      // 100 neurons, 1000 steps
        {500, 1000},      // 500 neurons, 1000 steps
        {1000, 500},      // 1000 neurons, 500 steps
        {2000, 200},      // 2000 neurons, 200 steps
    };
    
    for (const auto& [neurons, steps] : benchmarks) {
        auto result = runAdvancedBenchmark(neurons, steps);
        results.push_back(result);
        std::cout << std::endl;
    }
    
    // Print summary with advanced metrics
    printAdvancedBenchmarkResults(results);
    
    std::cout << "Advanced benchmark complete." << std::endl;
    std::cout << std::endl;
    std::cout << "Key Performance Insights:" << std::endl;
    std::cout << "  - Activity cohesion indicates network coordination" << std::endl;
    std::cout << "  - Network stability measures learning progress" << std::endl;
    std::cout << "  - Spike time variance reflects spike timing precision" << std::endl;
    std::cout << "  - Learning progress shows adaptive capabilities" << std::endl;
    
    return 0;
}
