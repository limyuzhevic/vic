#include "PerformanceMetrics.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace nlm {

struct PerformanceMetrics::Impl {
    bool monitoringEnabled = false;
    RealTimeMetrics currentMetrics;
    std::vector<RealTimeMetrics> realTimeHistory;
    std::vector<BenchmarkResult> benchmarks;
    
    std::unordered_map<std::string, std::vector<double>> metricHistory;
    std::unordered_map<std::string, std::vector<PerformanceCounter>> executionTimes;
    
    PerformanceCounter currentBenchmark;
    Timestamp firstMeasurementTime = 0.0;
    Timestamp lastMeasurementTime = 0.0;
    size_t totalRealTimeSamples = 0;
    
    double cpuUsage = 0.0;
    double memoryUsage = 0.0;
    double memoryAvailable = 0.0;
    
    Impl() {
        currentMetrics.timestamp = 0.0;
        currentMetrics.currentStep = 0;
    }
};

PerformanceMetrics::PerformanceMetrics() : pImpl(new Impl) {}

PerformanceMetrics::~PerformanceMetrics() {
    // Cleanup on destruction
}

void PerformanceMetrics::startMonitoring() {
    pImpl->monitoringEnabled = true;
    pImpl->firstMeasurementTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    pImpl->lastMeasurementTime = pImpl->firstMeasurementTime;
    
    NLM_LOG_INFO("Performance monitoring started");
}

void PerformanceMetrics::stopMonitoring() {
    pImpl->monitoringEnabled = false;
    NLM_LOG_INFO("Performance monitoring stopped");
}

bool PerformanceMetrics::isMonitoring() const {
    return pImpl->monitoringEnabled;
}

void PerformanceMetrics::recordRealTimeMetrics(const RealTimeMetrics& metrics) {
    pImpl->currentMetrics = metrics;
    pImpl->realTimeHistory.push_back(metrics);
    
    if (pImpl->firstMeasurementTime == 0.0) {
        pImpl->firstMeasurementTime = metrics.timestamp;
    }
    pImpl->lastMeasurementTime = metrics.timestamp;
    pImpl->totalRealTimeSamples++;
    
    // Keep only recent history (last 10000 samples)
    if (pImpl->realTimeHistory.size() > 10000) {
        pImpl->realTimeHistory.erase(pImpl->realTimeHistory.begin());
    }
    
    NLM_LOG_TRACE("Real-time metrics recorded: " + std::to_string(metrics.currentStep) + 
                  " steps, " + std::to_string(metrics.spikeCount) + " spikes");
}

RealTimeMetrics PerformanceMetrics::getCurrentRealTimeMetrics() const {
    return pImpl->currentMetrics;
}

void PerformanceMetrics::startBenchmark(const std::string& name, const std::string& description) {
    pImpl->currentBenchmark.name = name;
    pImpl->currentBenchmark.description = description;
    pImpl->currentBenchmark.startTime = std::chrono::high_resolution_clock::now();
    pImpl->currentBenchmark.endTime = pImpl->currentBenchmark.startTime;
    pImpl->currentBenchmark.duration = 0.0;
    
    NLM_LOG_INFO("Benchmark started: " + name + " - " + description);
}

void PerformanceMetrics::endBenchmark() {
    if (pImpl->currentBenchmark.startTime.time_since_epoch().count() == 0) {
        return;
    }
    
    pImpl->currentBenchmark.endTime = std::chrono::high_resolution_clock::now();
    pImpl->currentBenchmark.duration = 
        std::chrono::duration<double>(pImpl->currentBenchmark.endTime - pImpl->currentBenchmark.startTime).count();
    
    pImpl->benchmarks.push_back(pImpl->currentBenchmark);
    
    // Keep only recent benchmarks (last 100)
    if (pImpl->benchmarks.size() > 100) {
        pImpl->benchmarks.erase(pImpl->benchmarks.begin());
    }
    
    NLM_LOG_INFO("Benchmark completed: " + pImpl->currentBenchmark.name + 
                 " - Duration: " + std::to_string(pImpl->currentBenchmark.duration) + "s");
    
    pImpl->currentBenchmark.name.clear();
    pImpl->currentBenchmark.description.clear();
}

BenchmarkResult PerformanceMetrics::getCurrentBenchmark() const {
    return pImpl->currentBenchmark;
}

std::vector<BenchmarkResult> PerformanceMetrics::getAllBenchmarks() const {
    return pImpl->benchmarks;
}

void PerformanceMetrics::recordMetric(const std::string& name, double value, const std::string& unit) {
    pImpl->metricHistory[name].push_back(value);
    
    // Keep only recent history (last 10000 samples per metric)
    auto& history = pImpl->metricHistory[name];
    if (history.size() > 10000) {
        history.erase(history.begin(), history.begin() + 100);
    }
}

double PerformanceMetrics::getMetric(const std::string& name) const {
    auto it = pImpl->metricHistory.find(name);
    if (it == pImpl->metricHistory.end() || it->second.empty()) {
        return 0.0;
    }
    return it->second.back();
}

std::vector<double> PerformanceMetrics::getMetricHistory(const std::string& name) const {
    auto it = pImpl->metricHistory.find(name);
    if (it == pImpl->metricHistory.end()) {
        return std::vector<double>();
    }
    return it->second;
}

std::vector<std::string> PerformanceMetrics::getMetricNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->metricHistory) {
        names.push_back(pair.first);
    }
    return names;
}

double PerformanceMetrics::compareBenchmarks(const std::string& benchmark1, const std::string& benchmark2) const {
    // Find benchmarks
    auto it1 = std::find_if(pImpl->benchmarks.begin(), pImpl->benchmarks.end(),
                           [&benchmark1](const BenchmarkResult& b) { return b.benchmarkName == benchmark1; });
    auto it2 = std::find_if(pImpl->benchmarks.begin(), pImpl->benchmarks.end(),
                           [&benchmark2](const BenchmarkResult& b) { return b.benchmarkName == benchmark2; });
    
    if (it1 == pImpl->benchmarks.end() || it2 == pImpl->benchmarks.end()) {
        return 0.0;
    }
    
    // Compare based on spikes per second
    double spikes1 = it1->spikesPerSecond;
    double spikes2 = it2->spikesPerSecond;
    
    if (spikes2 == 0.0) {
        return 0.0;
    }
    
    return (spikes1 / spikes2) - 1.0; // (benchmark1 - benchmark2) / benchmark2
}

PerformanceAnalysis PerformanceMetrics::analyzePerformance(const std::string& benchmarkName) const {
    PerformanceAnalysis analysis;
    analysis.analysisName = "performance_analysis_" + benchmarkName;
    analysis.analysisDescription = "Analysis of performance for benchmark: " + benchmarkName;
    analysis.analysisTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    // Find benchmark
    auto it = std::find_if(pImpl->benchmarks.begin(), pImpl->benchmarks.end(),
                           [&benchmarkName](const BenchmarkResult& b) { return b.benchmarkName == benchmarkName; });
    
    if (it == pImpl->benchmarks.end()) {
        analysis.summary(); // Default summary
        return analysis;
    }
    
    const BenchmarkResult& benchmark = *it;
    
    // Identify bottlenecks
    if (benchmark.spikesPerSecond < 1000.0) {
        analysis.identifiedBottlenecks.push_back("Low spike processing rate");
    }
    if (benchmark.memoryUsage > 1000.0) {
        analysis.identifiedBottlenecks.push_back("High memory usage");
    }
    if (benchmark.cpuUtilization < 50.0) {
        analysis.identifiedBottlenecks.push_back("Underutilized CPU");
    }
    
    if (analysis.identifiedBottlenecks.empty()) {
        analysis.identifiedBottlenecks.push_back("No major bottlenecks identified");
    }
    
    // Optimization opportunities
    analysis.optimizationOpportunities.push_back("Implement SIMD optimization for neural computations");
    analysis.optimizationOpportunities.push_back("Consider event-driven spike processing");
    analysis.optimizationOpportunities.push_back("Optimize memory access patterns");
    
    // Performance metrics
    analysis.totalExecutionTime = benchmark.duration;
    analysis.cpuTimePercentage = benchmark.cpuUtilization;
    analysis.memoryTimePercentage = 100.0 - benchmark.cpuUtilization;
    
    // Scaling analysis
    analysis.scalingEfficiency = benchmark.spikesPerSecond / (benchmark.neuronCount * benchmark.synapseCount);
    analysis.memoryEfficiency = 1.0 / (benchmark.memoryUsage / benchmark.neuronCount);
    analysis.computationalEfficiency = benchmark.spikesPerSecond / (benchmark.neuronCount * benchmark.synapseCount);
    
    // Recommendations
    analysis.recommendations.push_back("Increase batch processing of spikes");
    analysis.recommendations.push_back("Optimize synaptic weight storage");
    analysis.recommendations.push_back("Consider parallel processing for large networks");
    
    // Calculate improvements
    double spikeImprovement = (benchmark.spikesPerSecond > 5000.0) ? 0.1 : 0.2;
    double memoryImprovement = (benchmark.memoryUsage > 500.0) ? 0.15 : 0.05;
    double cpuImprovement = (benchmark.cpuUtilization < 70.0) ? 0.1 : 0.05;
    
    analysis.suggestedImprovements.push_back({"Spike processing", spikeImprovement});
    analysis.suggestedImprovements.push_back({"Memory usage", memoryImprovement});
    analysis.suggestedImprovements.push_back({"CPU utilization", cpuImprovement});
    
    // Confidence score
    analysis.confidenceScore = 0.8; // Simplified
    analysis.componentScores["bottleneck_detection"] = 0.9;
    analysis.componentScores["scaling_analysis"] = 0.8;
    analysis.componentScores["optimization_suggestions"] = 0.7;
    
    return analysis;
}

PerformanceAnalysis PerformanceMetrics::analyzeScaling(const std::vector<size_t>& neuronCounts) const {
    PerformanceAnalysis analysis;
    analysis.analysisName = "scaling_analysis";
    analysis.analysisDescription = "Scaling analysis across neuron counts";
    analysis.analysisTime = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    
    if (neuronCounts.size() < 2) {
        analysis.summary(); // Default summary
        return analysis;
    }
    
    // Find benchmarks for different neuron counts
    std::vector<const BenchmarkResult*> benchmarksForCounts;
    for (size_t neuronCount : neuronCounts) {
        auto it = std::find_if(pImpl->benchmarks.begin(), pImpl->benchmarks.end(),
                               [&neuronCount](const BenchmarkResult& b) { return b.neuronCount == neuronCount; });
        if (it != pImpl->benchmarks.end()) {
            benchmarksForCounts.push_back(&(*it));
        }
    }
    
    if (benchmarksForCounts.size() < 2) {
        analysis.identifiedBottlenecks.push_back("Insufficient scaling data points");
        return analysis;
    }
    
    // Analyze scaling efficiency
    double totalScalingGain = 0.0;
    size_t validComparisons = 0;
    
    for (size_t i = 1; i < benchmarksForCounts.size(); ++i) {
        const BenchmarkResult* prev = benchmarksForCounts[i - 1];
        const BenchmarkResult* curr = benchmarksForCounts[i];
        
        double neuronScale = static_cast<double>(curr->neuronCount) / prev->neuronCount;
        double spikesScale = curr->spikesPerSecond / prev->spikesPerSecond;
        
        double scalingEfficiency = spikesScale / neuronScale;
        totalScalingGain += scalingEfficiency;
        validComparisons++;
        
        if (scalingEfficiency < 0.8) {
            analysis.identifiedBottlenecks.push_back("Poor scaling efficiency at " + 
                                                     std::to_string(curr->neuronCount) + " neurons");
        }
    }
    
    if (validComparisons > 0) {
        analysis.scalingEfficiency = totalScalingGain / validComparisons;
    } else {
        analysis.scalingEfficiency = 0.0;
    }
    
    // Memory efficiency analysis
    double totalMemoryEfficiency = 0.0;
    for (size_t i = 0; i < benchmarksForCounts.size(); ++i) {
        const BenchmarkResult* benchmark = benchmarksForCounts[i];
        double memoryPerNeuron = benchmark->memoryUsage / benchmark->neuronCount;
        analysis.optimizationOpportunities.push_back("Optimize memory per neuron for " + 
                                                      std::to_string(benchmark->neuronCount) + " neurons");
    }
    
    // Generate recommendations based on scaling results
    analysis.recommendations.push_back("Implement better load balancing for larger networks");
    analysis.recommendations.push_back("Consider hierarchical processing for scalability");
    analysis.recommendations.push_back("Optimize data locality for larger neuron counts");
    
    analysis.confidenceScore = 0.7;
    
    return analysis;
}

void PerformanceMetrics::recordExecutionTime(const std::string& operation, double duration) {
    PerformanceCounter counter;
    counter.name = operation;
    counter.description = "Execution time for " + operation;
    counter.duration = duration;
    
    pImpl->executionTimes[operation].push_back(counter);
    
    // Keep only recent history
    auto& history = pImpl->executionTimes[operation];
    if (history.size() > 1000) {
        history.erase(history.begin(), history.begin() + 100);
    }
}

double PerformanceMetrics::getAverageExecutionTime(const std::string& operation) const {
    auto it = pImpl->executionTimes.find(operation);
    if (it == pImpl->executionTimes.end() || it->second.empty()) {
        return 0.0;
    }
    
    double total = 0.0;
    for (const auto& counter : it->second) {
        total += counter.duration;
    }
    
    return total / it->second.size();
}

double PerformanceMetrics::getMinExecutionTime(const std::string& operation) const {
    auto it = pImpl->executionTimes.find(operation);
    if (it == pImpl->executionTimes.end() || it->second.empty()) {
        return 0.0;
    }
    
    double min = it->second[0].duration;
    for (const auto& counter : it->second) {
        if (counter.duration < min) {
            min = counter.duration;
        }
    }
    
    return min;
}

double PerformanceMetrics::getMaxExecutionTime(const std::string& operation) const {
    auto it = pImpl->executionTimes.find(operation);
    if (it == pImpl->executionTimes.end() || it->second.empty()) {
        return 0.0;
    }
    
    double max = it->second[0].duration;
    for (const auto& counter : it->second) {
        if (counter.duration > max) {
            max = counter.duration;
        }
    }
    
    return max;
}

bool PerformanceMetrics::exportMetrics(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "PerformanceMetrics Export" << std::endl;
        file << "=========================" << std::endl;
        file << "Generated: " << std::chrono::duration<double>::clock::now().time_since_epoch().count() << std::endl;
        file << "Total Recorded Metrics: " << getTotalRecordedMetrics() << std::endl;
        file << "Total Benchmarks: " << getTotalBenchmarks() << std::endl;
        file << "Total Real-time Samples: " << getTotalRealTimeSamples() << std::endl;
        
        file << "\nRecorded Metrics:\n";
        for (const std::string& name : getMetricNames()) {
            file << "  " << name << ": " << getMetric(name) << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool PerformanceMetrics::importMetrics(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("  ") == 0 && line.size() > 2 && line.find(": ") != std::string::npos) {
                size_t colon = line.find(": ");
                std::string name = line.substr(2, colon - 2);
                double value = std::stod(line.substr(colon + 2));
                recordMetric(name, value);
            }
        }
        
        file.close();
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool PerformanceMetrics::exportBenchmarks(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Benchmarks Export" << std::endl;
        file << "==================" << std::endl;
        
        for (const auto& benchmark : pImpl->benchmarks) {
            file << "Benchmark: " << benchmark.benchmarkName << std::endl;
            file << "  Description: " << benchmark.benchmarkDescription << std::endl;
            file << "  Duration: " << benchmark.duration << "s" << std::endl;
            file << "  Neurons: " << benchmark.neuronCount << std::endl;
            file << "  Synapses: " << benchmark.synapseCount << std::endl;
            file << "  Spikes/s: " << benchmark.spikesPerSecond << std::endl;
            file << "  Neurons/s: " << benchmark.neuronsPerSecond << std::endl;
            file << "  Synapses/s: " << benchmark.synapsesPerSecond << std::endl;
            file << "  Memory (MB): " << benchmark.memoryUsage << std::endl;
            file << "  CPU (%): " << benchmark.cpuUtilization << std::endl;
            file << "  Summary: " << benchmark.summary << std::endl;
            file << std::endl;
        }
        
        file.close();
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool PerformanceMetrics::importBenchmarks(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        BenchmarkResult currentBenchmark;
        bool readingBenchmark = false;
        
        while (std::getline(file, line)) {
            if (line.find("Benchmark: ") == 0) {
                if (readingBenchmark && !currentBenchmark.benchmarkName.empty()) {
                    pImpl->benchmarks.push_back(currentBenchmark);
                }
                currentBenchmark = BenchmarkResult();
                currentBenchmark.benchmarkName = line.substr(11);
                readingBenchmark = true;
            } else if (line.find("  Description: ") == 0 && readingBenchmark) {
                currentBenchmark.benchmarkDescription = line.substr(16);
            } else if (line.find("  Duration: ") == 0 && readingBenchmark) {
                currentBenchmark.duration = std::stod(line.substr(11, line.find('s') - 11));
            } else if (line.find("  Neurons: ") == 0 && readingBenchmark) {
                currentBenchmark.neuronCount = std::stoull(line.substr(10));
            } else if (line.find("  Synapses: ") == 0 && readingBenchmark) {
                currentBenchmark.synapseCount = std::stoull(line.substr(11));
            } else if (line.find("  Spikes/s: ") == 0 && readingBenchmark) {
                currentBenchmark.spikesPerSecond = std::stod(line.substr(11));
            } else if (line.find("  Neurons/s: ") == 0 && readingBenchmark) {
                currentBenchmark.neuronsPerSecond = std::stod(line.substr(12));
            } else if (line.find("  Synapses/s: ") == 0 && readingBenchmark) {
                currentBenchmark.synapsesPerSecond = std::stod(line.substr(13));
            } else if (line.find("  Memory (MB): ") == 0 && readingBenchmark) {
                currentBenchmark.memoryUsage = std::stod(line.substr(15));
            } else if (line.find("  CPU (%): ") == 0 && readingBenchmark) {
                currentBenchmark.cpuUtilization = std::stod(line.substr(10));
            } else if (line.find("  Summary: ") == 0 && readingBenchmark) {
                currentBenchmark.summary = line.substr(10);
            } else if (line.empty() && readingBenchmark) {
                if (!currentBenchmark.benchmarkName.empty()) {
                    pImpl->benchmarks.push_back(currentBenchmark);
                    currentBenchmark = BenchmarkResult();
                    readingBenchmark = false;
                }
            }
        }
        
        if (readingBenchmark && !currentBenchmark.benchmarkName.empty()) {
            pImpl->benchmarks.push_back(currentBenchmark);
        }
        
        file.close();
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

std::string PerformanceMetrics::generateReport() const {
    return formatMetricsReport("full");
}

std::string PerformanceMetrics::generateBenchmarkReport() const {
    std::stringstream ss;
    ss << "Benchmark Report" << std::endl;
    ss << "===============" << std::endl;
    ss << "Total Benchmarks: " << pImpl->benchmarks.size() << std::endl;
    
    for (const auto& benchmark : pImpl->benchmarks) {
        ss << std::endl << "Benchmark: " << benchmark.benchmarkName << std::endl;
        ss << "  Description: " << benchmark.benchmarkDescription << std::endl;
        ss << "  Duration: " << benchmark.duration << "s" << std::endl;
        ss << "  Performance Summary: " << benchmark.summary << std::endl;
    }
    
    return ss.str();
}

std::string PerformanceMetrics::generateRealTimeReport() const {
    std::stringstream ss;
    ss << "Real-Time Metrics Report" << std::endl;
    ss << "=======================" << std::endl;
    ss << "Current Timestamp: " << pImpl->currentMetrics.timestamp << std::endl;
    ss << "Current Step: " << pImpl->currentMetrics.currentStep << std::endl;
    ss << "Neuron Count: " << pImpl->currentMetrics.neuronCount << std::endl;
    ss << "Synapse Count: " << pImpl->currentMetrics.synapseCount << std::endl;
    ss << "Spike Count: " << pImpl->currentMetrics.spikeCount << std::endl;
    ss << "Firing Rate: " << pImpl->currentMetrics.firingRate << " Hz" << std::endl;
    ss << "Plasticity Rate: " << pImpl->currentMetrics.plasticityRate << std::endl;
    ss << "Learning Rate: " << pImpl->currentMetrics.learningRate << std::endl;
    ss << "Average Synaptic Weight: " << pImpl->currentMetrics.avgSynapticWeight << std::endl;
    ss << "Weight Variance: " << pImpl->currentMetrics.weightVariance << std::endl;
    ss << "Connectivity Density: " << pImpl->currentMetrics.connectivityDensity << std::endl;
    ss << "Active Neuron Count: " << pImpl->currentMetrics.activeNeuronCount << std::endl;
    
    return ss.str();
}

void PerformanceMetrics::updateSystemMetrics() {
    // Simplified system metrics collection
    // In real implementation, would use platform-specific APIs
    
    pImpl->cpuUsage = 50.0 + (std::rand() % 50); // Mock data
    pImpl->memoryUsage = 200.0 + (std::rand() % 300); // Mock data in MB
    pImpl->memoryAvailable = 1024.0 - pImpl->memoryUsage; // Mock data
    
    pImpl->currentMetrics.cpuUsage = pImpl->cpuUsage;
    pImpl->currentMetrics.memoryUsage = pImpl->memoryUsage;
    pImpl->currentMetrics.memoryAvailable = pImpl->memoryAvailable;
}

double PerformanceMetrics::getCpuUsage() const {
    return pImpl->cpuUsage;
}

double PerformanceMetrics::getMemoryUsage() const {
    return pImpl->memoryUsage;
}

double PerformanceMetrics::getMemoryAvailable() const {
    return pImpl->memoryAvailable;
}

void PerformanceMetrics::configureFromExperiment(const ExperimentConfig& config) {
    // Update current metrics from experiment configuration
    pImpl->currentMetrics.plasticityRate = config.getParameter<float>("plasticity_rate", 0.01f);
    pImpl->currentMetrics.learningRate = config.getParameter<float>("learning_rate", 0.001f);
    pImpl->currentMetrics.neuronCount = config.getParameter<size_t>("neuron_count", 1000);
}

bool PerformanceMetrics::validateConfiguration() const {
    // Validate current configuration
    for (const auto& pair : pImpl->metricHistory) {
        const std::string& name = pair.first;
        const std::vector<double>& values = pair.second;
        
        if (values.empty()) {
            return false;
        }
        
        // Check for NaN or infinite values
        for (double value : values) {
            if (std::isnan(value) || std::isinf(value)) {
                return false;
            }
        }
    }
    
    return true;
}

void PerformanceMetrics::clearAllData() {
    pImpl->realTimeHistory.clear();
    pImpl->benchmarks.clear();
    pImpl->metricHistory.clear();
    pImpl->executionTimes.clear();
    
    pImpl->currentMetrics = RealTimeMetrics();
    pImpl->currentBenchmark = PerformanceCounter();
    pImpl->firstMeasurementTime = 0.0;
    pImpl->lastMeasurementTime = 0.0;
    pImpl->totalRealTimeSamples = 0;
}

void PerformanceMetrics::cleanupOldData(Timestamp cutoffTime) {
    // Remove old real-time history
    auto it = std::remove_if(pImpl->realTimeHistory.begin(), pImpl->realTimeHistory.end(),
                             [&cutoffTime](const RealTimeMetrics& metrics) {
                                 return metrics.timestamp < cutoffTime;
                             });
    pImpl->realTimeHistory.erase(it, pImpl->realTimeHistory.end());
    
    // Remove old metrics history
    for (auto& pair : pImpl->metricHistory) {
        auto& history = pair.second;
        auto histIt = std::remove_if(history.begin(), history.end(),
                                     [&cutoffTime](double value) {
                                         // This is simplistic - real implementation would track timestamps
                                         return false; // Keep all for now
                                     });
        history.erase(histIt, history.end());
    }
}

RealTimeMetrics PerformanceMetrics::extractMetricsFromBrain(const std::shared_ptr<Brain>& brain,
                                                           const ExperimentConfig& config,
                                                           const PerformanceMetrics& currentMetrics) {
    RealTimeMetrics metrics;
    
    if (!brain) {
        return metrics;
    }
    
    metrics.timestamp = std::chrono::duration<double>::clock::now().time_since_epoch().count();
    metrics.currentStep = 0; // Would get from brain simulation clock
    
    // Extract basic brain statistics
    metrics.neuronCount = brain->getTotalNeuronCount();
    metrics.synapseCount = brain->getTotalSynapseCount();
    metrics.spikeCount = brain->getTotalSpikeCount();
    
    // Calculate derived metrics
    if (metrics.neuronCount > 0) {
        metrics.firingRate = static_cast<double>(metrics.spikeCount) / 1000.0; // Simplified
        metrics.avgSynapticWeight = 0.5f; // Simplified
        metrics.weightVariance = 0.1f; // Simplified
        metrics.connectivityDensity = static_cast<double>(metrics.synapseCount) / (metrics.neuronCount * metrics.neuronCount);
    }
    
    // Get configuration parameters
    metrics.plasticityRate = config.getParameter<float>("plasticity_rate", 0.01f);
    metrics.learningRate = config.getParameter<float>("learning_rate", 0.001f);
    
    return metrics;
}

void PerformanceMetrics::updateRealTimeMetrics(const ExperimentConfig& config, const std::shared_ptr<Brain>& brain) {
    RealTimeMetrics metrics = extractMetricsFromBrain(brain, config, *this);
    recordRealTimeMetrics(metrics);
    
    if (isMonitoring()) {
        updateSystemMetrics();
    }
}

void PerformanceMetrics::generatePerformanceSummary() {
    // Generate summary of current performance metrics
    std::stringstream ss;
    ss << "Performance Summary:" << std::endl;
    ss << "  Total metrics recorded: " << getTotalRecordedMetrics() << std::endl;
    ss << "  Total benchmarks: " << pImpl->benchmarks.size() << std::endl;
    ss << "  Monitoring enabled: " << (isMonitoring() ? "yes" : "no") << std::endl;
    if (pImpl->lastMeasurementTime > 0) {
        ss << "  Last measurement: " << pImpl->lastMeasurementTime << "s ago" << std::endl;
    }
    
    NLM_LOG_INFO(ss.str());
}

std::string PerformanceMetrics::formatMetricsReport(const std::string& reportType) const {
    if (reportType == "full") {
        return generateReport();
    } else if (reportType == "benchmark") {
        return generateBenchmarkReport();
    } else if (reportType == "realtime") {
        return generateRealTimeReport();
    } else {
        return "Unknown report type: " + reportType;
    }
}

// Static method implementations for type conversion
namespace ParameterType {
    template<typename T>
    std::string ParameterType::toTypeName() {
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, float>) return "float";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::vector<float>>) return "vector<float>";
        else if constexpr (std::is_same_v<T, size_t>) return "size_t";
        else if constexpr (std::is_same_v<T, SimulationStep>) return "SimulationStep";
        return "unknown";
    }
    
    template<typename T>
    ParameterValue ParameterType::convertToVariant(const T& value) {
        if constexpr (std::is_same_v<T, int>) return int(value);
        else if constexpr (std::is_same_v<T, float>) return float(value);
        else if constexpr (std::is_same_v<T, double>) return double(value);
        else if constexpr (std::is_same_v<T, std::string>) return std::string(value);
        else if constexpr (std::is_same_v<T, bool>) return bool(value);
        else if constexpr (std::is_same_v<T, std::vector<float>>) return value;
        else if constexpr (std::is_same_v<T, size_t>) return size_t(value);
        else if constexpr (std::is_same_v<T, SimulationStep>) return SimulationStep(value);
        throw std::runtime_error("Unsupported type for ParameterValue conversion");
    }
    
    template<typename T>
    T ParameterType::convertFromVariant(const ParameterValue& variant) {
        return std::get<T>(variant);
    }
}

} // namespace nlm
