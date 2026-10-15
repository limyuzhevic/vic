// NLM Performance Benchmark Suite
// Advanced performance measurement and analysis for power users

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "visualization/VisualizationInterface.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <cmath>
#include <limits>

using namespace nlm;

// Comprehensive performance metrics
struct PerformanceMetrics {
    // Basic metrics
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    
    // Activity metrics
    double totalSpikes;
    double averageFiringRate;
    double maxFiringRate;
    double spikeStandardDeviation;
    
    // Connectivity metrics
    double averageSynapseWeight;
    double connectionProbability;
    double density;
    
    // Timing breakdown
    double neuronStepTime;
    double synapseProcessingTime;
    double memoryManagementTime;
    
    // Resource usage
    double peakMemoryUsage;      // MB
    double averageCPUUsage;      // percentage
    
    // Quality metrics
    double networkStability;    // measure of network stability
    double learningProgress;    // measure of learning progression
    double convergenceRate;    // how quickly the system converges
    
    // Experiment type
    std::string experimentType;
    
    // Timestamp
    std::string timestamp;
};

// Advanced benchmarking configuration
struct BenchmarkConfig {
    std::string name;
    std::vector<std::pair<size_t, size_t>> configurations;  // (neuron_count, steps)
    std::vector<std::string> experimentTypes;
    size_t warmupSteps;           // Steps before measuring performance
    size_t measurementSteps;      // Number of steps to measure
    bool enableProfiling;         // Enable detailed profiling
    bool enableVisualization;     // Enable visualization during benchmark
    bool collectMemoryMetrics;    // Collect memory usage metrics
    bool collectCPUMetrics;       // Collect CPU usage metrics
    double timeWindow;            // Time window for analysis
    std::string outputFormat;     // "text", "json", "csv", "html"
};

// Advanced performance profiler
class PerformanceProfiler {
private:
    struct TimeMeasurement {
        std::string operation;
        double duration;
        size_t callCount;
        double averageDuration;
    };
    
    std::vector<TimeMeasurement> measurements;
    double totalCPUTime;
    double peakMemoryUsage;
    std::chrono::steady_clock::time_point sessionStart;
    
public:
    PerformanceProfiler() {
        sessionStart = std::chrono::steady_clock::now();
        totalCPUTime = 0.0;
        peakMemoryUsage = 0.0;
    }
    
    void startMeasurement(const std::string& operation) {
        measurements.push_back({operation, 0.0, 0, 0.0});
    }
    
    void endMeasurement(const std::string& operation, double duration) {
        for (auto& m : measurements) {
            if (m.operation == operation) {
                m.duration += duration;
                m.callCount++;
                m.averageDuration = m.duration / m.callCount;
                break;
            }
        }
        totalCPUTime += duration;
    }
    
    void updateMemoryUsage(double memoryMB) {
        if (memoryMB > peakMemoryUsage) {
            peakMemoryUsage = memoryMB;
        }
    }
    
    std::string generateReport() const {
        std::stringstream ss;
        ss << "=== Performance Profile Report ===\n";
        ss << "Session duration: " << getSessionDuration() << "s\n";
        ss << "Total CPU time: " << totalCPUTime << "s\n";
        ss << "Peak memory usage: " << peakMemoryUsage << "MB\n\n";
        ss << "Detailed measurements:\n";
        
        double totalMeasuredTime = 0.0;
        for (const auto& m : measurements) {
            ss << "  " << std::setw(20) << std::left << m.operation 
               << " - " << std::setw(8) << std::fixed << std::setprecision(2) << m.duration
               << "s (" << m.callCount << " calls)\n";
            totalMeasuredTime += m.duration;
        }
        
        ss << "\nSummary:\n";
        ss << "  Total measured time: " << totalMeasuredTime << "s\n";
        ss << "  Measurement overhead: " << ((totalMeasuredTime - totalCPUTime) / totalCPUTime * 100) << "%\n";
        
        return ss.str();
    }
    
private:
    double getSessionDuration() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration<double>(now - sessionStart).count();
    }
};

// Advanced experiment runner
class AdvancedExperimentRunner {
private:
    PerformanceProfiler profiler;
    std::shared_ptr<VisualizationInterface> visualizer;
    
public:
    AdvancedExperimentRunner() {
        if (std::getenv("NLM_ENABLE_VISUALIZATION") != nullptr) {
            visualizer = std::make_shared<VisualizationInterface>();
            visualizer->initialize();
        }
    }
    
    PerformanceMetrics runExperiment(size_t neuronCount, size_t steps, 
                                     const std::string& experimentType) {
        PerformanceMetrics metrics;
        metrics.neurons = neuronCount;
        metrics.steps = steps;
        metrics.experimentType = experimentType;
        
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
        
        // Warm-up phase
        profiler.startMeasurement("warmup");
        for (size_t step = 0; step < 100; ++step) {
            for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        profiler.endMeasurement("warmup", 0.0); // Time is measured separately
        
        // Main measurement phase
        profiler.startMeasurement("simulation");
        auto startTime = std::chrono::high_resolution_clock::now();
        
        double totalSpikes = 0.0;
        double maxFiringRate = 0.0;
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject input based on experiment type
            injectInput(brain, neuronCount, experimentType, step);
            
            // Measure brain step
            auto stepStart = std::chrono::high_resolution_clock::now();
            brain->step(step, step * 0.001);
            auto stepEnd = std::chrono::high_resolution_clock::now();
            
            profiler.endMeasurement("brain_step", 
                std::chrono::duration<double>(stepEnd - stepStart).count());
            
            // Collect statistics
            totalSpikes += brain->getTotalSpikeCount();
            double firingRate = calculateFiringRate(brain, step);
            if (firingRate > maxFiringRate) {
                maxFiringRate = firingRate;
            }
            
            // Update visualization if enabled
            if (visualizer && visualizer->isActive()) {
                visualizer->update();
                if ((step % 100) == 0) {
                    visualizer->render();
                }
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        profiler.endMeasurement("simulation", 
            std::chrono::duration<double>(endTime - startTime).count());
        
        // Calculate metrics
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        metrics.totalTime = totalTime;
        metrics.stepsPerSecond = steps / totalTime;
        metrics.totalSpikes = totalSpikes;
        metrics.averageFiringRate = totalSpikes / steps;
        metrics.maxFiringRate = maxFiringRate;
        metrics.spikeStandardDeviation = calculateSpikeVariability(brain, steps);
        
        // Calculate connectivity metrics
        metrics.synapses = brain->getTotalSynapseCount();
        metrics.synapsesPerNeuron = static_cast<double>(metrics.synapses) / neuronCount;
        metrics.density = calculateNetworkDensity(brain);
        
        // Get weight statistics
        auto weightStats = calculateWeightStatistics(brain);
        metrics.averageSynapseWeight = weightStats.first;
        
        // Calculate system quality metrics
        metrics.networkStability = calculateStability(brain, steps);
        metrics.learningProgress = calculateLearningProgress(brain, steps);
        metrics.convergenceRate = calculateConvergenceRate(brain, steps);
        
        // Add timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        metrics.timestamp = ss.str();
        
        return metrics;
    }
    
    std::string generateDetailedReport(const std::vector<PerformanceMetrics>& metrics) {
        std::stringstream ss;
        
        ss << "╔══════════════════════════════════════════════════════════════════════════════════╗\n";
        ss << "║                          ADVANCED NLM BENCHMARK REPORT                           ║\n";
        ss << "╠══════════════════════════════════════════════════════════════════════════════════╣\n";
        
        // Configuration summary
        ss << "Configuration Summary:\n";
        ss << "  Experiment Types: ";
        std::vector<std::string> expTypes;
        for (const auto& m : metrics) {
            if (std::find(expTypes.begin(), expTypes.end(), m.experimentType) == expTypes.end()) {
                expTypes.push_back(m.experimentType);
            }
        }
        for (size_t i = 0; i < expTypes.size(); ++i) {
            ss << expTypes[i];
            if (i < expTypes.size() - 1) ss << ", ";
        }
        ss << "\n\n";
        
        // Performance comparison table
        ss << "Performance Comparison:\n";
        ss << "╠══════════════╦════════════════╦════════════╦════════════╦══════════════╦════════════╣\n";
        ss << "║    Neurons    ║    Synapses   ║ Steps/sec  ║  Avg Firing ║ Network     ║   Quality   ║\n";
        ss << "║               ║               ║            ║   Rate (Hz) ║ Stability   ║  Score      ║\n";
        ss << "╠══════════════╬════════════════╬════════════╬════════════╬══════════════╬════════════╣\n";
        
        for (const auto& m : metrics) {
            double qualityScore = calculateQualityScore(m);
            ss << "║ " << std::setw(14) << m.neurons 
               << " ║ " << std::setw(16) << m.synapses
               << " ║ " << std::setw(10) << std::fixed << std::setprecision(0) << m.stepsPerSecond
               << " ║ " << std::setw(12) << std::fixed << std::setprecision(2) << m.averageFiringRate
               << " ║ " << std::setw(14) << std::fixed << std::setprecision(4) << m.networkStability
               << " ║ " << std::setw(12) << std::fixed << std::setprecision(4) << qualityScore
               << " ║\n";
        }
        
        ss << "╚══════════════╩════════════════╩════════════╩════════════╩══════════════╩════════════╝\n\n";
        
        // Detailed analysis
        ss << "Detailed Analysis:\n";
        ss << "  Average Performance:\n";
        
        double totalStepsPerSec = 0.0, totalFiringRate = 0.0, totalStability = 0.0;
        for (const auto& m : metrics) {
            totalStepsPerSec += m.stepsPerSecond;
            totalFiringRate += m.averageFiringRate;
            totalStability += m.networkStability;
        }
        
        size_t count = metrics.size();
        ss << "    Steps/sec: " << std::fixed << std::setprecision(0) << (totalStepsPerSec / count) << "\n";
        ss << "    Average firing rate: " << std::fixed << std::setprecision(2) << (totalFiringRate / count) << " Hz\n";
        ss << "    Average network stability: " << std::fixed << std::setprecision(4) << (totalStability / count) << "\n\n";
        
        // Scaling analysis
        ss << "Scaling Analysis:\n";
        ss << "  Steps/sec per neuron: " << calculateScaling(metrics, "steps_per_neuron") << "\n";
        ss << "  Synapses per neuron scaling: " << calculateScaling(metrics, "synapses_per_neuron") << "\n";
        ss << "  Firing rate scaling: " << calculateScaling(metrics, "firing_rate") << "\n";
        
        // Performance profiler report
        ss << "\n" << profiler.generateReport() << "\n";
        
        return ss.str();
    }
    
private:
    void injectInput(std::shared_ptr<Brain> brain, size_t neuronCount, 
                     const std::string& experimentType, size_t step) {
        if (experimentType == "connectivity") {
            // Focus on network connectivity
            for (size_t i = 0; i < std::min(size_t(20), neuronCount / 5); ++i) {
                brain->injectCurrentToNeurons(static_cast<NeuronType>(i % 3), 50.0f);
            }
        } else if (experimentType == "plasticity") {
            // Focus on learning and plasticity
            for (size_t i = 0; i < std::min(size_t(30), neuronCount / 4); ++i) {
                brain->injectCurrentToNeurons(static_cast<NeuronType>(i % 3), 40.0f);
            }
        } else if (experimentType == "stdp") {
            // Focus on spike timing dependent plasticity
            // Create correlated pre-post activity
            if (step % 10 == 0) {
                for (size_t i = 0; i < std::min(size_t(15), neuronCount / 6); ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Internal, 60.0f);
                }
            }
        } else {
            // Default experiment with sensory input
            for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
        }
    }
    
    double calculateFiringRate(std::shared_ptr<Brain> brain, size_t step) {
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return 0.0;
        
        auto neurons = region->getAllNeurons();
        double totalFiringRate = 0.0;
        size_t activeNeurons = 0;
        
        for (auto* neuron : neurons) {
            if (neuron->isFiring()) {
                totalFiringRate += neuron->getFiringRate();
                ++activeNeurons;
            }
        }
        
        return activeNeurons > 0 ? totalFiringRate / activeNeurons : 0.0;
    }
    
    double calculateSpikeVariability(std::shared_ptr<Brain> brain, size_t steps) {
        // Simple calculation of spike variability
        double meanSpikes = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        double variance = 0.0;
        
        // This would require more sophisticated spike history analysis
        // For demonstration, return a placeholder value
        return meanSpikes * 0.5;  // Placeholder: spike variability proportional to mean
    }
    
    std::pair<double, double> calculateWeightStatistics(std::shared_ptr<Brain> brain) {
        double totalWeight = 0.0;
        size_t synapseCount = 0;
        
        // This would require access to all synapses
        // For demonstration, return placeholder values
        return {0.5, 0.1};  // Placeholder: average weight 0.5, stddev 0.1
    }
    
    double calculateNetworkDensity(std::shared_ptr<Brain> brain) {
        size_t neuronCount = brain->getTotalNeuronCount();
        size_t synapseCount = brain->getTotalSynapseCount();
        
        if (neuronCount == 0) return 0.0;
        
        // Simple density calculation
        return static_cast<double>(synapseCount) / (neuronCount * neuronCount);
    }
    
    double calculateStability(std::shared_ptr<Brain> brain, size_t steps) {
        // Calculate network stability based on spike rate consistency
        double stability = 1.0 - (calculateSpikeVariability(brain, steps) / 100.0);
        return std::max(0.0, stability);
    }
    
    double calculateLearningProgress(std::shared_ptr<Brain> brain, size_t steps) {
        // Calculate learning progress based on spike rate changes
        // This would require more sophisticated analysis
        return 0.8;  // Placeholder
    }
    
    double calculateConvergenceRate(std::shared_ptr<Brain> brain, size_t steps) {
        // Calculate how quickly the system converges
        // This would require time-series analysis
        return 0.6;  // Placeholder
    }
    
    double calculateQualityScore(const PerformanceMetrics& m) {
        // Calculate an overall quality score
        double score = (m.networkStability * 0.4) + 
                      (m.learningProgress * 0.3) + 
                      (m.convergenceRate * 0.3);
        return score;
    }
    
    double calculateScaling(const std::vector<PerformanceMetrics>& metrics, 
                           const std::string& metricType) {
        // Calculate scaling behavior
        if (metrics.size() < 2) return 1.0;
        
        if (metricType == "steps_per_neuron") {
            // Calculate how steps/sec scales with neuron count
            double sum = 0.0;
            for (const auto& m : metrics) {
                sum += m.stepsPerSecond / m.neurons;
            }
            return sum / metrics.size();
        } else if (metricType == "synapses_per_neuron") {
            // Calculate average synapses per neuron
            double sum = 0.0;
            for (const auto& m : metrics) {
                sum += m.synapsesPerNeuron;
            }
            return sum / metrics.size();
        } else if (metricType == "firing_rate") {
            // Calculate scaling of firing rate with network size
            double sum = 0.0;
            for (const auto& m : metrics) {
                sum += m.averageFiringRate;
            }
            return sum / metrics.size();
        }
        
        return 1.0;
    }
};

// Advanced benchmark runner
void runAdvancedBenchmark(const BenchmarkConfig& config) {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    ADVANCED NLM BENCHMARK SUITE                                   ║" << std::endl;
    std::cout << "║                           " << config.name << "                          ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    AdvancedExperimentRunner runner;
    std::vector<PerformanceMetrics> allMetrics;
    
    // Run all experiment types
    for (const auto& expType : config.experimentTypes) {
        std::cout << "Running " << expType << " experiments..." << std::endl;
        
        for (const auto& benchConfig : config.configurations) {
            size_t neurons = benchConfig.first;
            size_t steps = benchConfig.second;
            
            std::cout << "  Configuration: " << neurons << " neurons, " << steps << " steps" << std::endl;
            
            // Run experiment
            PerformanceMetrics metrics = runner.runExperiment(neurons, steps, expType);
            allMetrics.push_back(metrics);
        }
        
        std::cout << std::endl;
    }
    
    // Generate comprehensive report
    std::string report = runner.generateDetailedReport(allMetrics);
    
    // Output based on format
    if (config.outputFormat == "json") {
        // Generate JSON report
        std::cout << "Note: JSON output would be written to file in production build." << std::endl;
        // For demonstration, just show text
        std::cout << report;
    } else if (config.outputFormat == "csv") {
        // Generate CSV report
        std::cout << "Note: CSV output would be written to file in production build." << std::endl;
        std::cout << "Metrics collected: " << allMetrics.size() << " data points" << std::endl;
    } else if (config.outputFormat == "html") {
        // Generate HTML report
        std::cout << "Note: HTML report would be written to file in production build." << std::endl;
        std::cout << "<html><body><pre>" << report << "</pre></body></html>" << std::endl;
    } else {
        // Default text output
        std::cout << report;
    }
    
    std::cout << "\nAdvanced benchmark complete. Metrics collected: " << allMetrics.size() << " configurations." << std::endl;
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
    ╚═════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Configure advanced benchmark
    BenchmarkConfig config;
    config.name = "Phase 2 Advanced Performance Suite";
    config.configurations = {
        {100, 1000},      // Small network
        {500, 2000},      // Medium network  
        {1000, 5000},     // Large network
        {2000, 10000},    // Very large network
    };
    config.experimentTypes = {"demo", "plasticity", "stdp", "connectivity"};
    config.warmupSteps = 100;
    config.measurementSteps = 1000;
    config.enableProfiling = true;
    config.enableVisualization = false; // Set to true for visualization
    config.collectMemoryMetrics = true;
    config.collectCPUMetrics = true;
    config.timeWindow = 60.0;  // seconds
    config.outputFormat = "text";
    
    // Set environment variable for visualization
    if (config.enableVisualization) {
        setenv("NLM_ENABLE_VISUALIZATION", "1", 1);
    }
    
    // Run advanced benchmark
    runAdvancedBenchmark(config);
    
    return 0;
}