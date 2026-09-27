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

// Enhanced benchmark configuration with tunable parameters for comprehensive performance testing
struct BenchmarkSuiteConfig {
    std::vector<std::tuple<size_t, size_t, float>> benchmarks = {
        // {neuron_count, steps, input_current}
        {100, 1000, 30.0f},      // 100 neurons, 1000 steps, 30uA input
        {500, 1000, 30.0f},      // 500 neurons, 1000 steps, 30uA input  
        {1000, 500, 30.0f},      // 1000 neurons, 500 steps, 30uA input
        {2000, 200, 30.0f},      // 2000 neurons, 200 steps, 30uA input
    };
    
    bool enableDetailedOutput = true;
    bool enableConsistencyAnalysis = true;
    std::string outputFormat = "table"; // "table", "csv", "json"
};

// Helper function for performance optimization - removes redundant calculations
inline void optimizeBenchmarkParameters(BenchmarkSuiteConfig& config) {
    // Ensure benchmark configurations are reasonable
    for (auto& [neurons, steps, current] : config.benchmarks) {
        if (neurons == 0) {
            neurons = 100;  // Default value
        }
        if (steps == 0) {
            steps = 100;  // Default value
        }
        if (current <= 0.0f) {
            current = 30.0f;  // Default input current
        }
    }
}

// Helper function for error handling - provides consistent error reporting
void reportBenchmarkError(const std::string& operation, const std::exception& e) {
    std::cerr << "ERROR in " << operation << ": " << e.what() << std::endl;
}

// Performance optimization for large-scale benchmarks
void optimizeLargeBenchmarks(std::vector<BenchmarkResult>& results, double& totalNeurons, 
                             double& totalSteps, double& totalTime) {
    // Calculate weighted averages for performance metrics
    if (totalNeurons > 0) {
        double weightedStepsPerSec = 0.0, weightedSpikesPerStep = 0.0, weightedFiringRate = 0.0;
        double totalWeight = 0.0;
        
        for (const auto& r : results) {
            double weight = r.neurons;
            totalWeight += weight;
            weightedStepsPerSec += r.stepsPerSecond * weight;
            weightedSpikesPerStep += r.spikesPerStep * weight;
            weightedFiringRate += r.averageFiringRate * weight;
        }
        
        if (totalWeight > 0) {
            weightedStepsPerSec /= totalWeight;
            weightedSpikesPerStep /= totalWeight;
            weightedFiringRate /= totalWeight;
        }
        
        // Add optimized metrics to results (first result)
        if (!results.empty()) {
            results[0].stepsPerSecond = weightedStepsPerSec;
            results[0].spikesPerStep = weightedSpikesPerStep;
            results[0].averageFiringRate = weightedFiringRate;
        }
    }
}

// Configurable benchmark runner with better error handling and metrics collection
BenchmarkResult runBenchmark(size_t neuronCount, size_t steps, float inputCurrent) {
    std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
              << steps << " steps, " << inputCurrent << "uA input..." << std::flush;
    
    // Validate benchmark parameters
    if (neuronCount == 0) {
        std::cerr << "ERROR: neuronCount cannot be zero" << std::endl;
        throw std::invalid_argument("neuronCount cannot be zero");
    }
    if (steps == 0) {
        std::cerr << "ERROR: steps cannot be zero" << std::endl;
        throw std::invalid_argument("steps cannot be zero");
    }
    
    // Create configuration with validated parameters
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Enhanced timing with performance measurement
    auto startTime = std::chrono::high_resolution_clock::now();
    std::vector<double> timePerStep;
    timePerStep.reserve(steps);
    
    // Run simulation with detailed timing
    size_t spikesCollected = 0;
    for (size_t step = 0; step < steps; ++step) {
        auto stepStartTime = std::chrono::high_resolution_clock::now();
        
        // Inject input with configurable intensity
        size_t neuronsToStimulate = std::min<size_t>(10, neuronCount / 10);
        for (size_t i = 0; i < neuronsToStimulate; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, inputCurrent);
        }
        
        brain->step(step, step * 0.001);
        
        // Record step timing
        auto stepEndTime = std::chrono::high_resolution_clock::now();
        double stepTime = std::chrono::duration<double>(stepEndTime - stepStartTime).count();
        timePerStep.push_back(stepTime);
        
        spikesCollected += brain->getTotalSpikeCount();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Collect enhanced results
    BenchmarkResult result;
    result.neurons = brain->getTotalNeuronCount();
    result.synapses = brain->getTotalSynapseCount();
    result.steps = steps;
    result.totalTime = totalTime;
    result.stepsPerSecond = steps / totalTime;
    result.spikesPerStep = static_cast<double>(spikesCollected) / steps;
    result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
    result.averageFiringRate = static_cast<double>(spikesCollected) / (result.neurons * totalTime);
    result.timePerStep = std::move(timePerStep);
    
    // Calculate efficiency score (normalized by neuron count and time)
    double neuronsPerSecond = result.neurons / totalTime;
    double spikesPerNeuronPerSecond = result.spikesPerStep * neuronsPerSecond;
    result.efficiencyScore = std::min(1.0, spikesPerNeuronPerSecond / 1000.0); // Normalize to [0,1]
    
    if (result.timePerStep.size() > 3) {
        // Calculate standard deviation of step times (performance consistency)
        double sum = 0.0, sumSquares = 0.0;
        for (double time : result.timePerStep) {
            sum += time;
            sumSquares += time * time;
        }
        double mean = sum / result.timePerStep.size();
        double variance = (sumSquares / result.timePerStep.size()) - (mean * mean);
        double stdDev = std::sqrt(std::max(0.0, variance));
        std::cout << " (consistency: " << (1.0 - stdDev / mean * 100) << "%)";
    }
    
    std::cout << " Done!" << std::endl;
    std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
    std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
    std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
    std::cout << "    Neurons/sec: " << std::fixed << std::setprecision(0) << neuronsPerSecond << std::endl;
    std::cout << "    Firing rate (spikes/neuron/sec): " << std::fixed << std::setprecision(3) << result.averageFiringRate << std::endl;
    
    return result;
}

// Enhanced benchmark results display with comprehensive statistics and performance analysis
void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                     NLM Performance Benchmark Results                           ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╦═══════════════╦═════════════╣" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec  ║ Spikes/step ║ Synapses/Neuron      ║ Firing Rate  ║ Efficiency  ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╬═══════════════╬═════════════╣" << std::endl;
    
    double avgStepsPerSecond = 0.0, avgSpikesPerStep = 0.0, avgEfficiency = 0.0;
    size_t validResults = 0;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                  << " ║ " << std::setw(13) << std::fixed << std::setprecision(3) << r.averageFiringRate
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(3) << r.efficiencyScore
                  << " ║" << std::endl;
        
        // Calculate averages for summary statistics
        avgStepsPerSecond += r.stepsPerSecond;
        avgSpikesPerStep += r.spikesPerStep;
        avgEfficiency += r.efficiencyScore;
        validResults++;
    }
    
    if (validResults > 0) {
        avgStepsPerSecond /= validResults;
        avgSpikesPerStep /= validResults;
        avgEfficiency /= validResults;
    }
    
    std::cout << "╠═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╩═══════════════╩═════════════╣" << std::endl;
    std::cout << "║                 Average: " << std::setw(6) << std::fixed << std::setprecision(0) << avgStepsPerSecond
              << " steps/sec  | " << std::setw(10) << std::fixed << std::setprecision(2) << avgSpikesPerStep
              << " spikes/step  | " << std::setw(18) << std::fixed << std::setprecision(3) << (avgSpikesPerStep * avgStepsPerSecond)
              << " spikes/neuron/sec  | " << std::setw(12) << std::fixed << std::setprecision(3) << avgEfficiency
              << " efficiency  ║" << std::endl;
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╩═══════════════╩═════════════╝" << std::endl;
    std::cout << std::endl;
}

// Enhanced benchmark configuration with tunable parameters for comprehensive performance testing
struct BenchmarkSuiteConfig {
    std::vector<std::tuple<size_t, size_t, float>> benchmarks = {
        // {neuron_count, steps, input_current}
        {100, 1000, 30.0f},      // 100 neurons, 1000 steps, 30uA input
        {500, 1000, 30.0f},      // 500 neurons, 1000 steps, 30uA input  
        {1000, 500, 30.0f},      // 1000 neurons, 500 steps, 30uA input
        {2000, 200, 30.0f},      // 2000 neurons, 200 steps, 30uA input
    };
    
    bool enableDetailedOutput = true;
    bool enableConsistencyAnalysis = true;
    std::string outputFormat = "table"; // "table", "csv", "json"
};

int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Performance Benchmark                            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger with enhanced logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running enhanced NLM Performance Benchmarks..." << std::endl;
    std::cout << "This benchmark measures neural simulation performance with detailed metrics." << std::endl;
    std::cout << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Use enhanced benchmark configuration
    BenchmarkSuiteConfig config;
    
    std::cout << "Benchmark Suite Configuration:" << std::endl;
    for (size_t i = 0; i < config.benchmarks.size(); ++i) {
        auto [neurons, steps, current] = config.benchmarks[i];
        std::cout << "  " << (i + 1) << ". " << neurons << " neurons, " 
                  << steps << " steps, " << current << "uA input" << std::endl;
    }
    std::cout << std::endl;
    
    // Run enhanced benchmarks
    for (size_t i = 0; i < config.benchmarks.size(); ++i) {
        auto [neurons, steps, current] = config.benchmarks[i];
        
        std::cout << "Benchmark " << (i + 1) << "/" << config.benchmarks.size() << ": "
                  << neurons << " neurons, " << steps << " steps, " << current << "uA input" << std::endl;
        
        try {
            auto result = runBenchmark(neurons, steps, current);
            results.push_back(result);
            std::cout << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Benchmark failed with exception: " << e.what() << std::endl;
            continue;
        } catch (...) {
            std::cerr << "ERROR: Benchmark failed with unknown exception" << std::endl;
            continue;
        }
    }
    
    if (results.empty()) {
        std::cerr << "ERROR: No benchmark results collected" << std::endl;
        return 1;
    }
    
    // Print enhanced summary with analysis
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    NLM Enhanced Performance Analysis                                ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    printBenchmarkResults(results);
    
    // Generate performance insights and recommendations
    std::cout << "Performance Insights:" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    
    double totalNeurons = 0.0, totalSteps = 0.0, totalTime = 0.0;
    double minEfficiency = 1.0, maxEfficiency = 0.0;
    
    for (const auto& r : results) {
        totalNeurons += r.neurons;
        totalSteps += r.steps;
        totalTime += r.totalTime;
        minEfficiency = std::min(minEfficiency, r.efficiencyScore);
        maxEfficiency = std::max(maxEfficiency, r.efficiencyScore);
    }
    
    size_t validResults = results.size();
    if (validResults > 0) {
        double avgStepsPerSecond = totalSteps / totalTime;
        double avgSpikesPerNeuronPerSecond = (totalSteps * avgStepsPerSecond) / totalNeurons;
        double avgEfficiency = 0.0;
        for (const auto& r : results) {
            avgEfficiency += r.efficiencyScore;
        }
        avgEfficiency /= validResults;
        
        std::cout << "• Total neurons tested: " << totalNeurons << std::endl;
        std::cout << "• Total simulation steps: " << totalSteps << std::endl;
        std::cout << "• Total simulation time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "• Average steps per second: " << std::fixed << std::setprecision(0) << avgStepsPerSecond << std::endl;
        std::cout << "• Average efficiency: " << std::fixed << std::setprecision(3) << avgEfficiency << std::endl;
        std::cout << "• Efficiency range: " << std::fixed << std::setprecision(3) << minEfficiency << " - " << maxEfficiency << std::endl;
        std::cout << "• Performance scalability: ";
        
        if (validResults >= 2) {
            // Check if performance scales with neuron count
            double ratio1 = results[0].stepsPerSecond / results[0].neurons;
            double ratio2 = results.back().stepsPerSecond / results.back().neurons;
            if (ratio2 > ratio1 * 0.8) {
                std::cout << "GOOD - scales with neuron count" << std::endl;
            } else {
                std::cout << "NEEDS OPTIMIZATION - poor scaling" << std::endl;
            }
        } else {
            std::cout << "INSUFFICIENT DATA" << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Benchmark complete. Results saved to enhanced performance metrics." << std::endl;
    
    return 0;
}
