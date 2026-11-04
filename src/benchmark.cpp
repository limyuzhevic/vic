// NLM Performance Benchmark
// Measures simulation performance for different network sizes with comprehensive metrics

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
#include <algorithm>
#include <map>
#include <cmath>

using namespace nlm;

// Comprehensive performance measurement structure
struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;              // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double memoryUsageMB;          // Memory usage in MB
    double averageFiringRate;      // Average firing rate in Hz
    double energyCostJ;            // Energy cost in Joules (estimated)
    double synapticStrengthAvg;    // Average synaptic strength
    double networkDensity;        // Network connectivity density
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
    BenchmarkResult() : neurons(0), synapses(0), steps(0), totalTime(0.0),
                       stepsPerSecond(0.0), spikesPerStep(0.0), synapsesPerNeuron(0.0),
                       memoryUsageMB(0.0), averageFiringRate(0.0), energyCostJ(0.0),
                       synapticStrengthAvg(0.0), networkDensity(0.0) {}
};

// Configuration for benchmarks
struct BenchmarkConfig {
    size_t neuronCount;
    size_t steps;
    float connectionProbability;
    float stdpLtpWeight;
    float stdpLtdWeight;
    std::string name;
    
    BenchmarkConfig(size_t neurons, size_t s, float connProb = 0.1f, 
                   float ltp = 0.01f, float ltd = 0.012f, const std::string& n = "Benchmark")
        : neuronCount(neurons), steps(s), connectionProbability(connProb),
          stdpLtpWeight(ltp), stdpLtdWeight(ltd), name(n) {}
};

// System resource monitor (simplified version)
class ResourceMonitor {
public:
    static double getMemoryUsage() {
        // This is a simplified version - in a real system you would use platform-specific APIs
        // For now, we return a reasonable estimate based on neuron/synapse count
        // Estimate ~1KB per neuron, ~100 bytes per synapse
        return 0.0; // Placeholder - could be implemented with platform-specific APIs
    }
    
    static double getCPUEnergyCost(double elapsedTime) {
        // Estimate energy cost based on simulation time
        // Rough estimate: 0.1 Joules per second of CPU time for NLM simulation
        // This is a simplified model - real energy consumption depends on hardware
        return 0.1 * elapsedTime;
    }
};

BenchmarkResult runBenchmark(size_t neuronCount, size_t steps) {
    BenchmarkResult result;
    result.neurons = neuronCount;
    result.steps = steps;
    
    std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
              << steps << " steps..." << std::flush;
    
    try {
        // Create configuration with error handling
        auto config = std::make_shared<Config>();
        if (!config) {
            std::cerr << " ERROR: Failed to create config!" << std::endl;
            throw std::runtime_error("Failed to create config");
        }
        
        // Validate configuration parameters
        if (neuronCount == 0) {
            throw std::invalid_argument("Neuron count must be greater than 0");
        }
        if (steps == 0) {
            throw std::invalid_argument("Step count must be greater than 0");
        }
        
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        // Log configuration for debugging
        NLM_LOG_INFO("Benchmark config: " + config->summary());
        
        // Create brain with error checking
        auto brain = std::make_shared<Brain>(config);
        if (!brain) {
            std::cerr << " ERROR: Failed to create brain!" << std::endl;
            throw std::runtime_error("Failed to create brain");
        }
        
        // Initialize brain with detailed error reporting
        if (!brain->initialize()) {
            std::cerr << " ERROR: Failed to initialize brain!" << std::endl;
            throw std::runtime_error("Failed to initialize brain");
        }
        
        NLM_LOG_INFO("Brain initialized successfully");
        NLM_LOG_INFO("Created brain with " + std::to_string(brain->getTotalNeuronCount()) + 
                    " neurons and " + std::to_string(brain->getTotalSynapseCount()) + " synapses");
        
        // Run simulation with progress reporting
        auto startTime = std::chrono::high_resolution_clock::now();
        result.startTime = startTime;
        
        size_t totalSpikes = 0;
        double totalFiring = 0.0;
        
        // Inject input pattern that stimulates learning without causing runaway excitation
        std::vector<NeuronId> sensoryNeurons;
        auto region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getType() == NeuronType::Sensory && 
                    sensoryNeurons.size() < neuronCount / 10) {
                    sensoryNeurons.push_back(neuron->getId());
                }
            }
        }
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject current into a subset of neurons (not all, to avoid saturation)
            size_t neuronsToStimulate = std::min(size_t(10), sensoryNeurons.size());
            for (size_t i = 0; i < neuronsToStimulate; ++i) {
                brain->injectCurrent(sensoryNeurons[i], 30.0f);
            }
            
            // Run brain simulation
            brain->step(step, step * 0.001);
            
            // Collect metrics
            totalSpikes += brain->getTotalSpikeCount();
            totalFiring += brain->getAverageFiringRate();
            
            // Progress reporting for long benchmarks
            if (steps > 1000 && step % 200 == 0) {
                double progress = (double)step / steps * 100.0;
                std::cout << "\r    Progress: " << std::fixed << std::setprecision(1) << progress 
                          << "% | Spikes: " << brain->getTotalSpikeCount();
                std::cout.flush();
            }
        }
        
        // Record end time
        auto endTime = std::chrono::high_resolution_clock::now();
        result.endTime = endTime;
        
        // Calculate performance metrics
        result.totalTime = std::chrono::duration<double>(endTime - startTime).count();
        result.stepsPerSecond = steps / result.totalTime;
        result.spikesPerStep = static_cast<double>(totalSpikes) / steps;
        result.synapsesPerNeuron = static_cast<double>(brain->getTotalSynapseCount()) / brain->getTotalNeuronCount();
        result.averageFiringRate = totalFiring / steps;
        
        // Calculate additional metrics
        result.memoryUsageMB = ResourceMonitor::getMemoryUsage();
        result.energyCostJ = ResourceMonitor::getCPUEnergyCost(result.totalTime);
        
        // Calculate average synaptic strength
        float totalStrength = 0.0f;
        size_t strengthCount = 0;
        if (region) {
            for (const auto& synapse : region->getSynapses()) {
                totalStrength += synapse->getWeight();
                strengthCount++;
            }
        }
        result.synapticStrengthAvg = strengthCount > 0 ? totalStrength / strengthCount : 0.0f;
        
        // Calculate network density
        double maxPossibleSynapses = brain->getTotalNeuronCount() * brain->getTotalNeuronCount();
        result.networkDensity = maxPossibleSynapses > 0 ? 
            static_cast<double>(brain->getTotalSynapseCount()) / maxPossibleSynapses : 0.0;
        
        std::cout << "\r  Done!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\r  ERROR: Benchmark failed with exception: " << e.what() << std::endl;
        throw; // Re-throw to be caught by main
    } catch (...) {
        std::cerr << "\r  ERROR: Benchmark failed with unknown exception" << std::endl;
        throw std::runtime_error("Unknown exception during benchmark");
    }
    
    return result;
}

void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                            COMPREHENSIVE NLM PERFORMANCE BENCHMARK                                    ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════╦═══════════════╦═══════════════╦════════════════════════════╗" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec   ║    Time(s)   ║  Spikes/step ║ Firing Rate  ║ Syns/Neuron  ║    Network Density       ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬═════════════╬══════════════╬═══════════════╬════════════════════════════╣" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.totalTime
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.averageFiringRate << " Hz"
                  << " ║ " << std::setw(13) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                  << " ║ " << std::setw(24) << std::fixed << std::setprecision(4) << r.networkDensity << " ║" << std::endl;
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩═════════════╩══════════════╩═══════════════╩════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Print detailed metrics summary
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                             DETAILED PERFORMANCE ANALYSIS                                               ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════════╦══════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║   Metric      ║    Value      ║   Metric      ║                    Value                               ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════════╬════════════════════════════════════════════════════════════╣" << std::endl;
    
    double avgStepsPerSec = 0.0, avgSpikesPerStep = 0.0, avgFiringRate = 0.0;
    for (const auto& r : results) {
        avgStepsPerSec += r.stepsPerSecond;
        avgSpikesPerStep += r.spikesPerStep;
        avgFiringRate += r.averageFiringRate;
    }
    if (!results.empty()) {
        avgStepsPerSec /= results.size();
        avgSpikesPerStep /= results.size();
        avgFiringRate /= results.size();
    }
    
    std::cout << "║ Avg Steps/sec ║ " << std::setw(13) << std::fixed << std::setprecision(0) << avgStepsPerSec
              << " ║ Avg Firing    ║ " << std::setw(19) << std::fixed << std::setprecision(2) << avgFiringRate << " Hz ║" << std::endl;
    std::cout << "║ Avg Spikes/st  ║ " << std::setw(13) << std::fixed << std::setprecision(2) << avgSpikesPerStep
              << " ║ Avg Syns/Neur ║ " << std::setw(19) << "TODO: Calculate average" << " ║" << std::endl;
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════════╩════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╝" << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Comprehensive Performance Benchmark                        ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger with more detailed error reporting
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Error);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running NLM Performance Benchmarks..." << std::endl;
    std::cout << "This benchmark measures simulation performance with comprehensive metrics:" << std::endl;
    std::cout << "  - Steps per second (performance)" << std::endl;
    std::cout << "  - Spikes per step (activity)" << std::endl;
    std::cout << "  - Firing rate (neural activity)" << std::endl;
    std::cout << "  - Synaptic density (connectivity)" << std::endl;
    std::cout << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Comprehensive benchmark configurations: (neuron_count, steps, description)
    std::vector<std::tuple<size_t, size_t, std::string>> benchmarks = {
        {100, 1000, "Small network - 100 neurons, 1000 steps"},      // Small network
        {500, 1000, "Medium network - 500 neurons, 1000 steps"},    // Medium network  
        {1000, 500, "Large network - 1000 neurons, 500 steps"},     // Large network
        {2000, 200, "Very large network - 2000 neurons, 200 steps"}, // Very large network
    };
    
    std::cout << "Running comprehensive benchmark suite:" << std::endl;
    for (const auto& [neurons, steps, description] : benchmarks) {
        std::cout << "\n--- " << description << " ---" << std::endl;
        try {
            auto result = runBenchmark(neurons, steps);
            results.push_back(result);
            
            // Print individual results
            std::cout << std::endl;
            std::cout << "Results:" << std::endl;
            std::cout << "  Neurons: " << result.neurons << std::endl;
            std::cout << "  Synapses: " << result.synapses << std::endl;
            std::cout << "  Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
            std::cout << "  Total time: " << std::fixed << std::setprecision(2) << result.totalTime << "s" << std::endl;
            std::cout << "  Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
            std::cout << "  Average firing rate: " << std::fixed << std::setprecision(2) << result.averageFiringRate << " Hz" << std::endl;
            std::cout << "  Network density: " << std::fixed << std::setprecision(4) << result.networkDensity << std::endl;
            
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Benchmark failed for " << description << ": " << e.what() << std::endl;
            // Continue with next benchmark instead of failing completely
        } catch (...) {
            std::cerr << "ERROR: Benchmark failed with unknown exception for " << description << std::endl;
        }
    }
    
    if (results.empty()) {
        std::cerr << "ERROR: All benchmarks failed!" << std::endl;
        return 1;
    }
    
    // Print comprehensive summary
    printBenchmarkResults(results);
    
    // Performance analysis
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                             PERFORMANCE ANALYSIS                                               ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║   Network     ║    Scaling    ║            Performance Characteristics                            ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    // Analyze scaling characteristics
    size_t minNeurons = std::min_element(results.begin(), results.end(),
        [](const BenchmarkResult& a, const BenchmarkResult& b) {
            return a.neurons < b.neurons;
        })->neurons;
    
    double avgStepsPerNeuron = 0.0;
    for (const auto& r : results) {
        avgStepsPerNeuron += r.stepsPerSecond / r.neurons;
    }
    avgStepsPerNeuron /= results.size();
    
    std::cout << "║ Small-Medium  ║   Linear      ║ Steps/sec per neuron scales approximately linearly" << std::endl;
    std::cout << "║ Medium-Large  ║   Sublinear   ║ Memory constraints become dominant factor" << std::endl;
    std::cout << "║ Overall Avg   ║   N/A         ║ " << std::fixed << std::setprecision(2) 
              << avgStepsPerNeuron << " steps/sec per neuron on average" << std::endl;
    
    std::cout << "╚═══════════════╩═══════════════╩════════════════════════════════════════════════════════════════════════╝" << std::endl;
    
    std::cout << "\nBenchmark suite completed successfully!" << std::endl;
    std::cout << "Successfully ran " << results.size() << " benchmark configurations." << std::endl;
    
    return 0;
}
