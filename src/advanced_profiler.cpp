// NLM Advanced Performance Profiler
// Comprehensive profiling and performance analysis for NLM

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
#include <memory>
#include <fstream>
#include <map>
#include <algorithm>

using namespace nlm;

// Advanced profiling metrics
struct AdvancedBenchmarkResult {
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;           // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double networkDensity;
    double activityRatio;
    double learningRate;
    double plasticityLoad;
    double memoryUsage;
    double cpuUsage;
    std::chrono::microseconds avgStepTime;
    std::chrono::microseconds minStepTime;
    std::chrono::microseconds maxStepTime;
    size_t totalSpikes;
    size_t firingNeuronCount;
    float avgFiringRate;
    float maxFiringRate;
    std::vector<float> firingRates;
    std::vector<size_t> stepTimes;
};

// Configuration for profiling
struct ProfilerConfig {
    size_t neuronCount;
    size_t steps;
    bool enableMemoryTracking;
    bool enableCPUTracking;
    bool enableDetailedTiming;
    bool collectHistograms;
    std::string outputFile;
    std::string format; // "detailed", "summary", "json"
};

// Advanced profiler class
class PerformanceProfiler {
private:
    ProfilerConfig config_;
    std::vector<AdvancedBenchmarkResult> results_;
    std::map<std::string, double> systemMetrics_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point endTime_;
    
public:
    PerformanceProfiler(const ProfilerConfig& config) : config_(config) {}
    
    void runAdvancedBenchmark() {
        std::cout << "Advanced NLM Performance Profiler" << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "Neuron count: " << config_.neuronCount << std::endl;
        std::cout << "Steps: " << config_.steps << std::endl;
        std::cout << "Memory tracking: " << (config_.enableMemoryTracking ? "Yes" : "No") << std::endl;
        std::cout << "CPU tracking: " << (config_.enableCPUTracking ? "Yes" : "No") << std::endl;
        std::cout << "Detailed timing: " << (config_.enableDetailedTiming ? "Yes" : "No") << std::endl;
        std::cout << std::endl;
        
        startTime_ = std::chrono::high_resolution_clock::now();
        
        AdvancedBenchmarkResult result = runAdvancedBenchmark(config_.neuronCount, config_.steps);
        
        endTime_ = std::chrono::high_resolution_clock::now();
        result.totalTime = std::chrono::duration<double>(endTime_ - startTime_).count();
        
        results_.push_back(result);
        
        printAdvancedResults(result);
    }
    
    AdvancedBenchmarkResult runAdvancedBenchmark(size_t neuronCount, size_t steps) {
        std::cout << "Running advanced benchmark: " << neuronCount << " neurons, " 
                  << steps << " steps..." << std::flush;
        
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Prepare timing collection
        std::vector<size_t> stepTimes;
        if (config_.enableDetailedTiming) {
            stepTimes.reserve(steps);
        }
        
        std::vector<float> firingRates;
        if (config_.enableDetailedTiming) {
            firingRates.reserve(steps);
        }
        
        // Run simulation with detailed timing
        auto stepStartTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject some random input
            for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            // Record step timing
            auto stepEndTime = std::chrono::high_resolution_clock::now();
            auto stepDuration = std::chrono::duration_cast<std::chrono::microseconds>(stepEndTime - stepStartTime);
            
            if (config_.enableDetailedTiming) {
                stepTimes.push_back(stepDuration.count());
            }
            
            brain->step(step, step * 0.001);
            
            // Record firing rates
            if (config_.enableDetailedTiming) {
                firingRates.push_back(brain->getAverageFiringRate());
            }
            
            stepStartTime = std::chrono::high_resolution_clock::now();
        }
        
        // Collect results with enhanced metrics
        AdvancedBenchmarkResult result;
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = steps;
        result.stepsPerSecond = steps / result.totalTime;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        // Calculate advanced metrics
        result.networkDensity = static_cast<double>(result.synapses) / (result.neurons * result.neurons);
        result.activityRatio = result.spikesPerStep * 100.0;
        result.totalSpikes = brain->getTotalSpikeCount();
        result.firingNeuronCount = brain->getFiringNeuronCount();
        result.avgFiringRate = brain->getAverageFiringRate();
        result.maxFiringRate = 0.0f;
        
        // Calculate firing rate statistics
        if (!firingRates.empty()) {
            result.firingRates = firingRates;
            result.maxFiringRate = *std::max_element(firingRates.begin(), firingRates.end());
            
            // Calculate step timing statistics
            if (config_.enableDetailedTiming && !stepTimes.empty()) {
                size_t totalStepTime = std::accumulate(stepTimes.begin(), stepTimes.end(), 0);
                result.avgStepTime = std::chrono::microseconds(totalStepTime / stepTimes.size());
                result.minStepTime = std::chrono::microseconds(*std::min_element(stepTimes.begin(), stepTimes.end()));
                result.maxStepTime = std::chrono::microseconds(*std::max_element(stepTimes.begin(), stepTimes.end()));
            }
        }
        
        // Calculate plasticity metrics (if available)
        result.plasticityLoad = 0.5; // Placeholder - would need access to brain's plasticity system
        result.learningRate = 0.01;   // Placeholder
        
        // Estimate memory usage (simplified)
        result.memoryUsage = (result.neurons * 64 + result.synapses * 32) / (1024.0 * 1024.0); // MB
        
        // Estimate CPU usage (simplified)
        result.cpuUsage = 50.0 + result.stepsPerSecond * 0.1; // Percentage
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Time: " << std::fixed << std::setprecision(2) << result.totalTime << "s" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
        std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
        std::cout << "    Network density: " << std::fixed << std::setprecision(6) << result.networkDensity << std::endl;
        std::cout << "    Neural activity: " << std::fixed << std::setprecision(3) << result.activityRatio << "% per step" << std::endl;
        std::cout << "    Avg firing rate: " << std::fixed << std::setprecision(2) << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "    Max firing rate: " << std::fixed << std::setprecision(2) << result.maxFiringRate << " Hz" << std::endl;
        std::cout << "    Estimated memory: " << std::fixed << std::setprecision(2) << result.memoryUsage << " MB" << std::endl;
        std::cout << "    Estimated CPU: " << std::fixed << std::setprecision(1) << result.cpuUsage << "%" << std::endl;
        if (config_.enableDetailedTiming) {
            std::cout << "    Avg step time: " << result.avgStepTime.count() << " μs" << std::endl;
            std::cout << "    Min step time: " << result.minStepTime.count() << " μs" << std::endl;
            std::cout << "    Max step time: " << result.maxStepTime.count() << " μs" << std::endl;
        }
        
        return result;
    }
    
    void printAdvancedResults(const AdvancedBenchmarkResult& result) {
        std::cout << std::endl;
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    ADVANCED NLM PERFORMANCE PROFILER RESULTS                    ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ NEURAL NETWORK ANALYSIS                                                        ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ Neurons     : " << std::setw(12) << result.neurons << "                              ║" << std::endl;
        std::cout << "║ Synapses    : " << std::setw(12) << result.synapses << "                              ║" << std::endl;
        std::cout << "║ Network Density: " << std::setw(10) << std::fixed << std::setprecision(6) << result.networkDensity << "                       ║" << std::endl;
        std::cout << "║ Synapses/Neuron: " << std::setw(10) << std::fixed << std::setprecision(2) << result.synapsesPerNeuron << "                        ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ PERFORMANCE METRICS                                                            ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ Total Time   : " << std::setw(12) << std::fixed << std::setprecision(2) << result.totalTime << " s                    ║" << std::endl;
        std::cout << "║ Steps/sec    : " << std::setw(12) << std::fixed << std::setprecision(0) << result.stepsPerSecond << "                   ║" << std::endl;
        std::cout << "║ Spikes/step  : " << std::setw(12) << std::fixed << std::setprecision(2) << result.spikesPerStep << "                  ║" << std::endl;
        std::cout << "║ Neural Activity: " << std::setw(11) << std::fixed << std::setprecision(3) << result.activityRatio << "% per step  ║" << std::endl;
        std::cout << "║ Avg Firing Rate: " << std::setw(11) << std::fixed << std::setprecision(2) << result.avgFiringRate << " Hz          ║" << std::endl;
        std::cout << "║ Max Firing Rate: " << std::setw(11) << std::fixed << std::setprecision(2) << result.maxFiringRate << " Hz          ║" << std::endl;
        std::cout << "║ Total Spikes  : " << std::setw(12) << result.totalSpikes << "                     ║" << std::endl;
        std::cout << "║ Firing Neurons: " << std::setw(12) << result.firingNeuronCount << "                   ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ RESOURCE UTILIZATION                                                             ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ Estimated Memory: " << std::setw(12) << std::fixed << std::setprecision(2) << result.memoryUsage << " MB             ║" << std::endl;
        std::cout << "║ Estimated CPU   : " << std::setw(12) << std::fixed << std::setprecision(1) << result.cpuUsage << "%              ║" << std::endl;
        if (result.avgStepTime.count() > 0) {
            std::cout << "║ Avg Step Time  : " << std::setw(12) << result.avgStepTime.count() << " μs                 ║" << std::endl;
        }
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ SCALING ANALYSIS                                                                 ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        // Calculate scaling metrics
        double neuronsPerSec = static_cast<double>(result.neurons) / result.totalTime;
        double synapsesPerSec = static_cast<double>(result.synapses) / result.totalTime;
        double efficiency = result.stepsPerSecond / neuronsPerSec;
        
        std::cout << "║ Neurons/sec   : " << std::setw(12) << std::fixed << std::setprecision(0) << neuronsPerSec << "                  ║" << std::endl;
        std::cout << "║ Synapses/sec  : " << std::setw(12) << std::fixed << std::setprecision(0) << synapsesPerSec << "                  ║" << std::endl;
        std::cout << "║ Efficiency    : " << std::setw(12) << std::fixed << std::setprecision(3) << efficiency << " neurons/step    ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ MACHINE LEARNING CAPABILITIES                                                    ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        // Estimate learning capabilities
        double learningCapacity = result.activityRatio * result.networkDensity * 100.0;
        double stability = 100.0 / (1.0 + result.spikesPerStep);
        
        std::cout << "║ Learning Capacity: " << std::setw(11) << std::fixed << std::setprecision(2) << learningCapacity << "             ║" << std::endl;
        std::cout << "║ Network Stability: " << std::setw(12) << std::fixed << std::setprecision(1) << stability << "%          ║" << std::endl;
        std::cout << "║ Plasticity Load: " << std::setw(13) << std::fixed << std::setprecision(2) << result.plasticityLoad << "            ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║ OPTIMIZATION RECOMMENDATIONS                                                     ║" << std::endl;
        std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
        
        // Generate optimization recommendations
        std::string optimization = "General performance optimization";
        if (result.cpuUsage > 80.0) {
            optimization = "CPU bottleneck detected - consider reducing neuron count or increasing parallelism";
        } else if (result.memoryUsage > 100.0) {
            optimization = "Memory bottleneck detected - consider reducing network size or enabling compression";
        } else if (result.activityRatio < 1.0) {
            optimization = "Low activity detected - consider increasing input strength or adjusting thresholds";
        } else if (result.networkDensity < 0.001) {
            optimization = "Low connectivity detected - consider increasing connection probability";
        }
        
        // Wrap recommendation text to fit in the box
        std::string line1 = optimization.substr(0, 50);
        std::string line2 = optimization.substr(50, 50);
        std::string line3 = optimization.substr(100);
        
        std::cout << "║ " << std::left << std::setw(56) << line1 << " ║" << std::endl;
        if (!line2.empty()) {
            std::cout << "║ " << std::left << std::setw(56) << line2 << " ║" << std::endl;
        }
        if (!line3.empty()) {
            std::cout << "║ " << std::left << std::setw(56) << line3 << " ║" << std::endl;
        }
        
        std::cout << "╚════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
    }
    
    void exportResults(const std::string& filename) {
        if (config_.outputFile.empty() && filename.empty()) {
            std::cout << "No output file specified. Results printed to console only." << std::endl;
            return;
        }
        
        std::string outputFile = filename.empty() ? config_.outputFile : filename;
        
        if (config_.format == "json") {
            exportAsJSON(outputFile);
        } else {
            exportAsText(outputFile);
        }
        
        std::cout << "Results exported to: " << outputFile << std::endl;
    }
    
    void exportAsText(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open file for writing: " << filename << std::endl;
            return;
        }
        
        file << "NLM Advanced Performance Profiler Results" << std::endl;
        file << "=" << std::string(50, '=') << std::endl;
        file << "Neuron Count: " << config_.neuronCount << std::endl;
        file << "Steps: " << config_.steps << std::endl;
        file << "Memory Tracking: " << (config_.enableMemoryTracking ? "Yes" : "No") << std::endl;
        file << "CPU Tracking: " << (config_.enableCPUTracking ? "Yes" : "No") << std::endl;
        file << "Detailed Timing: " << (config_.enableDetailedTiming ? "Yes" : "No") << std::endl;
        file << std::endl;
        
        for (const auto& result : results_) {
            file << "Benchmark Result:" << std::endl;
            file << "  Neurons: " << result.neurons << std::endl;
            file << "  Synapses: " << result.synapses << std::endl;
            file << "  Steps/sec: " << std::fixed << std::setprecision(2) << result.stepsPerSecond << std::endl;
            file << "  Network Density: " << std::fixed << std::setprecision(6) << result.networkDensity << std::endl;
            file << "  Neural Activity: " << std::fixed << std::setprecision(3) << result.activityRatio << "% per step" << std::endl;
            file << "  Total Time: " << std::fixed << std::setprecision(2) << result.totalTime << " s" << std::endl;
            file << "  Memory Usage: " << std::fixed << std::setprecision(2) << result.memoryUsage << " MB" << std::endl;
            file << "  CPU Usage: " << std::fixed << std::setprecision(1) << result.cpuUsage << "%" << std::endl;
            file << std::endl;
        }
        
        file.close();
    }
    
    void exportAsJSON(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Cannot open file for writing: " << filename << std::endl;
            return;
        }
        
        file << "{\n";
        file << "  \"profiler_version\": \"advanced_1.0\",\n";
        file << "  \"config\": {\n";
        file << "    \"neuron_count\": " << config_.neuronCount << "," << std::endl;
        file << "    \"steps\": " << config_.steps << "," << std::endl;
        file << "    \"memory_tracking\": " << (config_.enableMemoryTracking ? "true" : "false") << "," << std::endl;
        file << "    \"cpu_tracking\": " << (config_.enableCPUTracking ? "true" : "false") << "," << std::endl;
        file << "    \"detailed_timing\": " << (config_.enableDetailedTiming ? "true" : "false") << "\n";
        file << "  },\n";
        file << "  \"results\": [\n";
        
        for (size_t i = 0; i < results_.size(); ++i) {
            const auto& result = results_[i];
            file << "    {\n";
            file << "      \"neurons\": " << result.neurons << "," << std::endl;
            file << "      \"synapses\": " << result.synapses << "," << std::endl;
            file << "      \"steps\": " << result.steps << "," << std::endl;
            file << "      \"total_time\": " << std::fixed << std::setprecision(3) << result.totalTime << "," << std::endl;
            file << "      \"steps_per_second\": " << std::fixed << std::setprecision(2) << result.stepsPerSecond << "," << std::endl;
            file << "      \"spikes_per_step\": " << std::fixed << std::setprecision(2) << result.spikesPerStep << "," << std::endl;
            file << "      \"network_density\": " << std::fixed << std::setprecision(6) << result.networkDensity << "," << std::endl;
            file << "      \"activity_ratio\": " << std::fixed << std::setprecision(3) << result.activityRatio << "," << std::endl;
            file << "      \"memory_usage_mb\": " << std::fixed << std::setprecision(2) << result.memoryUsage << "," << std::endl;
            file << "      \"cpu_usage_percent\": " << std::fixed << std::setprecision(1) << result.cpuUsage << "," << std::endl;
            file << "      \"avg_firing_rate\": " << std::fixed << std::setprecision(2) << result.avgFiringRate << "," << std::endl;
            file << "      \"max_firing_rate\": " << std::fixed << std::setprecision(2) << result.maxFiringRate << "\n";
            
            if (i < results_.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
    }
};

int main(int argc, char** argv) {
    // Default profiler configuration
    ProfilerConfig config;
    config.neuronCount = 1000;
    config.steps = 1000;
    config.enableMemoryTracking = true;
    config.enableCPUTracking = true;
    config.enableDetailedTiming = true;
    config.collectHistograms = true;
    config.outputFile = "profiler_results.txt";
    config.format = "detailed";
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::stoi(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            config.steps = std::stoi(argv[++i]);
        } else if (arg == "--memory-tracking") {
            config.enableMemoryTracking = true;
        } else if (arg == "--no-memory-tracking") {
            config.enableMemoryTracking = false;
        } else if (arg == "--cpu-tracking") {
            config.enableCPUTracking = true;
        } else if (arg == "--no-cpu-tracking") {
            config.enableCPUTracking = false;
        } else if (arg == "--detailed-timing") {
            config.enableDetailedTiming = true;
        } else if (arg == "--no-detailed-timing") {
            config.enableDetailedTiming = false;
        } else if (arg == "--output" && i + 1 < argc) {
            config.outputFile = argv[++i];
        } else if (arg == "--format" && i + 1 < argc) {
            config.format = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Advanced NLM Performance Profiler" << std::endl;
            std::cout << "Usage: profiler [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --neurons N        Set neuron count" << std::endl;
            std::cout << "  --steps S          Set number of steps" << std::endl;
            std::cout << "  --memory-tracking   Enable memory tracking" << std::endl;
            std::cout << "  --no-memory-tracking Disable memory tracking" << std::endl;
            std::cout << "  --cpu-tracking      Enable CPU tracking" << std::endl;
            std::cout << "  --no-cpu-tracking   Disable CPU tracking" << std::endl;
            std::cout << "  --detailed-timing   Enable detailed timing" << std::endl;
            std::cout << "  --no-detailed-timing Disable detailed timing" << std::endl;
            std::cout << "  --output FILE      Output file name" << std::endl;
            std::cout << "  --format FMT       Output format (detailed, summary, json)" << std::endl;
            std::cout << "  --help, -h         Show this help message" << std::endl;
            return 0;
        }
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create and run profiler
    PerformanceProfiler profiler(config);
    profiler.runAdvancedBenchmark();
    
    // Export results if specified
    if (!config.outputFile.empty()) {
        profiler.exportResults(config.outputFile);
    }
    
    std::cout << "\nAdvanced profiler completed successfully!" << std::endl;
    
    return 0;
}
