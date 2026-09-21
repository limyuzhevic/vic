// NLM Performance Benchmark
// Enhanced version with improved metrics, extensibility, and error handling

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
#include <numeric>
#include <memory>
#include <mutex>
#include <thread>
#include <map>

using namespace nlm;

// Enhanced performance measurement structure
struct BenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;              // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double peakMemUsage;           // MB
    double avgTimePerStep;         // microseconds
    double efficiencyScore;        // normalized performance metric
    std::string timestamp;
    
    // Constructor with defaults for better extensibility
    BenchmarkResult()
        : neurons(0), synapses(0), steps(0), totalTime(0.0), stepsPerSecond(0.0),
          spikesPerStep(0.0), synapsesPerNeuron(0.0), peakMemUsage(0.0),
          avgTimePerStep(0.0), efficiencyScore(0.0), timestamp("") {}
    
    BenchmarkResult(size_t n, size_t s, size_t st, double time, double spm)
        : neurons(n), synapses(s), steps(st), totalTime(time), stepsPerSecond(spm),
          spikesPerStep(spm / steps), synapsesPerNeuron(static_cast<double>(s) / n),
          peakMemUsage(0.0), avgTimePerStep(0.0), efficiencyScore(0.0), timestamp("") {}
    
    // Comparison operators for easy sorting and analysis
    bool operator<(const BenchmarkResult& other) const {
        return neurons < other.neurons;
    }
    
    bool operator>(const BenchmarkResult& other) const {
        return neurons > other.neurons;
    }
    
    bool operator<=(const BenchmarkResult& other) const {
        return neurons <= other.neurons;
    }
    
    bool operator>=(const BenchmarkResult& other) const {
        return neurons >= other.neurons;
    }
};

// Thread-safe benchmark manager
class BenchmarkManager {
private:
    std::vector<BenchmarkResult> results;
    mutable std::mutex mutex;
    std::map<std::string, double> customMetrics;
    
public:
    void addResult(const BenchmarkResult& result) {
        std::lock_guard<std::mutex> lock(mutex);
        results.push_back(result);
    }
    
    std::vector<BenchmarkResult> getResults() const {
        std::lock_guard<std::mutex> lock(mutex);
        return results;
    }
    
    void addCustomMetric(const std::string& name, double value) {
        std::lock_guard<std::mutex> lock(mutex);
        customMetrics[name] = value;
    }
    
    double getCustomMetric(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = customMetrics.find(name);
        return (it != customMetrics.end()) ? it->second : 0.0;
    }
    
    size_t getResultCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        return results.size();
    }
    
    void clearResults() {
        std::lock_guard<std::mutex> lock(mutex);
        results.clear();
        customMetrics.clear();
    }
};

// Enhanced benchmark runner with better error handling
BenchmarkResult runBenchmarkEnhanced(size_t neuronCount, size_t steps, 
                                     BenchmarkManager* manager = nullptr) {
    std::cout << "  Running benchmark: " << neuronCount << " neurons, " 
              << steps << " steps..." << std::flush;
    
    // Input validation with clear error messages
    if (neuronCount == 0) {
        std::cerr << " ERROR: neuronCount cannot be zero!" << std::endl;
        return BenchmarkResult{}; // Return empty result
    }
    
    if (steps == 0) {
        std::cerr << " ERROR: steps cannot be zero!" << std::endl;
        return BenchmarkResult{}; // Return empty result
    }
    
    try {
        // Create configuration using the improved Config system
        auto config = std::make_shared<Config>();
        
        // Set configuration with validation
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        // Enable validation for production runs
        if (manager) {
            config->enableValidation(true);
        }
        
        // Create brain with error handling
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << " ERROR: Failed to initialize brain!" << std::endl;
            return BenchmarkResult{};
        }
        
        // Run simulation with enhanced metrics collection
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Pre-calculate input count to avoid repeated division
        size_t inputCount = std::min<size_t>(10, neuronCount / std::max<size_t>(1, neuronCount / 10));
        
        size_t totalSpikes = 0;
        size_t peakStep = 0;
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject random input with better distribution
            for (size_t i = 0; i < inputCount; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            totalSpikes += brain->getTotalSpikeCount();
            
            if (manager) {
                manager->addCustomMetric("spike_count_step_" + std::to_string(step), 
                                        brain->getTotalSpikeCount());
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Collect comprehensive results
        BenchmarkResult result;
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.spikesPerStep = static_cast<double>(totalSpikes) / steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        // Calculate additional metrics
        result.avgTimePerStep = (totalTime * 1000000.0) / steps; // microseconds
        result.efficiencyScore = result.stepsPerSecond / std::max<double>(1.0, result.neurons * 0.01);
        
        // Add timestamp for benchmarking
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        result.timestamp = ss.str();
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
        std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
        std::cout << "    Avg time/step: " << std::fixed << std::setprecision(2) << result.avgTimePerStep << "μs" << std::endl;
        
        // Add to manager if provided
        if (manager) {
            manager->addResult(result);
        }
        
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << " ERROR: Benchmark failed with exception: " << e.what() << std::endl;
        return BenchmarkResult{};
    } catch (...) {
        std::cerr << " ERROR: Benchmark failed with unknown exception!" << std::endl;
        return BenchmarkResult{};
    }
}

// Enhanced result printer with more detailed formatting
void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                       ENHANCED NLM Performance Benchmark Results                        ║" << std::endl;
    std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╦════════════╣" << std::endl;
    std::cout << "║   Neurons     ║   Synapses    ║ Steps/sec  ║ Spikes/step ║ Synapses/Neuron     ║   Avg μs/step   ║" << std::endl;
    std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╬════════════╣" << std::endl;
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        if (r.neurons == 0) continue; // Skip failed benchmarks
        
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(9) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(2) << r.synapsesPerNeuron
                  << " ║ " << std::setw(13) << std::fixed << std::setprecision(2) << r.avgTimePerStep
                  << " μs ║" << std::endl;
        
        // Add timestamp for first result
        if (i == 0 && !r.timestamp.empty()) {
            std::cout << "║ (Run at: " << std::left << std::setw(43) << r.timestamp << ") ║" << std::endl;
        }
    }
    
    std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╩════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Calculate and print statistics
    if (!results.empty()) {
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                             BENCHMARK STATISTICS                                  ║" << std::endl;
        std::cout << "╠═══════════════╦════════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        // Calculate averages (only for valid results)
        double totalNeurons = 0.0, totalStepsPerSec = 0.0, totalAvgTimePerStep = 0.0;
        size_t validCount = 0;
        
        for (const auto& r : results) {
            if (r.neurons > 0) {
                totalNeurons += r.neurons;
                totalStepsPerSec += r.stepsPerSecond;
                totalAvgTimePerStep += r.avgTimePerStep;
                validCount++;
            }
        }
        
        if (validCount > 0) {
            std::cout << "║ Average Neurons    : " << std::left << std::setw(50) 
                      << std::fixed << std::setprecision(0) << (totalNeurons / validCount) << "║" << std::endl;
            std::cout << "║ Average Steps/sec  : " << std::left << std::setw(50) 
                      << std::fixed << std::setprecision(0) << (totalStepsPerSec / validCount) << "║" << std::endl;
            std::cout << "║ Average Time/step  : " << std::left << std::setw(50) 
                      << std::fixed << std::setprecision(2) << (totalAvgTimePerStep / validCount) << "μs║" << std::endl;
            std::cout << "║ Total benchmarks   : " << std::left << std::setw(50) << validCount << "║" << std::endl;
        }
        
        std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
    }
}

// Enhanced main with command-line argument support
int main(int argc, char** argv) {
    std::cout << R"(
    ╔════════════════════════════════════════════════════════════════════════════════════════╗
    ║                                                                                ║
    ║     NLM — 熙然                                                                ║
    ║     Neural Learning Machine                                                     ║
    ║                                                                                ║
    ║     Enhanced Performance Benchmark                                             ║
    ║                                                                                ║
    ║     Now with improved metrics, error handling, and extensibility              ║
    ║                                                                                ║
    ╚════════════════════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running Enhanced NLM Performance Benchmarks..." << std::endl;
    std::cout << std::endl;
    
    // Initialize benchmark manager
    BenchmarkManager manager;
    
    // Parse command line arguments
    std::vector<std::pair<size_t, size_t>> benchmarks;
    
    // Default benchmarks (same as before but enhanced)
    std::vector<std::pair<size_t, size_t>> defaultBenchmarks = {
        {100, 1000},      // 100 neurons, 1000 steps
        {500, 1000},      // 500 neurons, 1000 steps
        {1000, 500},      // 1000 neurons, 500 steps
        {2000, 200},      // 2000 neurons, 200 steps
    };
    
    // Check for custom benchmarks from command line
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "Enhanced Benchmark Usage:" << std::endl;
            std::cout << "  ./benchmark                    Run default benchmarks" << std::endl;
            std::cout << "  ./benchmark --neurons N --steps S  Run custom benchmark" << std::endl;
            std::cout << "  ./benchmark --list              List all default benchmarks" << std::endl;
            std::cout << std::endl;
            return 0;
        }
        
        if (arg == "--list") {
            std::cout << "Default Benchmarks:" << std::endl;
            for (size_t i = 0; i < defaultBenchmarks.size(); ++i) {
                std::cout << "  " << (i + 1) << ". " << defaultBenchmarks[i].first 
                          << " neurons, " << defaultBenchmarks[i].second << " steps" << std::endl;
            }
            return 0;
        }
        
        if (arg == "--neurons" && i + 1 < argc) {
            try {
                size_t neurons = std::stoul(argv[++i]);
                if (i + 1 < argc && std::string(argv[i + 1]) == "--steps") {
                    size_t steps = std::stoul(argv[++i + 1]);
                    benchmarks.emplace_back(neurons, steps);
                }
            } catch (const std::exception& e) {
                std::cerr << "Error parsing neurons/steps: " << e.what() << std::endl;
            }
        }
    }
    
    // Use custom benchmarks if provided, otherwise use defaults
    if (benchmarks.empty()) {
        benchmarks = defaultBenchmarks;
    }
    
    std::cout << "Running " << benchmarks.size() << " benchmark(s)..." << std::endl;
    std::cout << std::endl;
    
    // Run all benchmarks
    std::vector<BenchmarkResult> results;
    
    for (size_t i = 0; i < benchmarks.size(); ++i) {
        const auto& [neurons, steps] = benchmarks[i];
        
        std::cout << "Benchmark " << (i + 1) << "/" << benchmarks.size() << ":" << std::endl;
        
        BenchmarkResult result = runBenchmarkEnhanced(neurons, steps, &manager);
        
        if (result.neurons > 0) {  // Only add valid results
            results.push_back(result);
        } else {
            std::cout << "  Benchmark " << (i + 1) << " FAILED!" << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    // Print summary with enhanced statistics
    printBenchmarkResults(results);
    
    // Print custom metrics if any
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                           CUSTOM METRICS                                        ║" << std::endl;
    std::cout << "╠═══════════════╦════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    bool hasCustomMetrics = false;
    for (const auto& [name, value] : manager.getCustomMetrics()) {
        if (name.find("spike_count_step_") == 0) {
            hasCustomMetrics = true;
            size_t stepNum = std::stoull(name.substr(16));
            std::cout << "║ Step " << std::left << std::setw(3) << stepNum 
                      << ": " << std::left << std::setw(48) << (value > 0 ? "✓ Active" : "✗ Inactive")
                      << "║" << std::endl;
        }
    }
    
    if (!hasCustomMetrics) {
        std::cout << "║ No custom metrics collected                                               ║" << std::endl;
    }
    
    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Enhanced Benchmark Complete." << std::endl;
    
    // Return success if at least one benchmark succeeded
    return (results.empty() ? 1 : 0);
}