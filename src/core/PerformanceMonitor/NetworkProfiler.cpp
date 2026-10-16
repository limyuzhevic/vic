#include "PerformanceMonitor/NetworkProfiler.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <thread>
#include <chrono>
#include <numeric>
#include <map>
#include <unordered_map>
#include <set>
#include <sstream>
#include <iostream>

namespace nlm {

// Helper functions for NetworkProfiler
namespace {
    std::string formatTime(double seconds) {
        if (seconds < 1.0) return std::to_string(static_cast<int>(seconds * 1000)) + " ms";
        if (seconds < 60.0) return std::to_string(static_cast<int>(seconds)) + " s";
        if (seconds < 3600.0) return std::to_string(static_cast<int>(seconds / 60)) + " m";
        return std::to_string(static_cast<int>(seconds / 3600)) + " h";
    }
    
    double calculateAverage(const std::deque<std::tuple<size_t, double, double>>& events) {
        if (events.empty()) return 0.0;
        
        double sum = 0.0;
        for (const auto& event : events) {
            sum += std::get<1>(event); // time
        }
        
        return sum / events.size();
    }
    
    double calculateStandardDeviation(const std::deque<std::tuple<size_t, double, double>>& events, double mean) {
        if (events.size() < 2) return 0.0;
        
        double sumSq = 0.0;
        for (const auto& event : events) {
            double value = std::get<1>(event) - mean;
            sumSq += value * value;
        }
        
        return std::sqrt(sumSq / events.size());
    }
    
    double calculateSynchronousity(const std::deque<std::tuple<size_t, size_t, double, double>>& activities) {
        if (activities.empty()) return 0.0;
        
        // Calculate coefficient of variation of active neurons
        std::vector<double> firingRates;
        for (const auto& activity : activities) {
            size_t active = std::get<0>(activity);
            size_t total = std::get<1>(activity);
            if (total > 0) {
                firingRates.push_back(static_cast<double>(active) / total);
            }
        }
        
        if (firingRates.size() < 2) return 0.0;
        
        double sum = 0.0;
        double sumSq = 0.0;
        for (double rate : firingRates) {
            sum += rate;
            sumSq += rate * rate;
        }
        
        double mean = sum / firingRates.size();
        double stdDev = std::sqrt((sumSq / firingRates.size()) - (mean * mean));
        
        // Lower standard deviation = higher synchronousity
        return (1.0 - (stdDev / mean)) * 100.0;
    }
}

NetworkProfiler::NetworkProfiler() : pImpl(std::make_unique<Impl>()) {
    // Initialize network profiling
    initialized_ = true;
}

NetworkProfiler::~NetworkProfiler() {
    shutdown();
}

void NetworkProfiler::initialize() {
    if (initialized_) {
        return;
    }
    
    initialized_ = true;
    
    // Log initialization
    if (logger_) {
        logger_->info("Network profiler initialized");
    }
}

void NetworkProfiler::shutdown() {
    if (!initialized_) {
        return;
    }
    
    // Clear integration objects
    logger_.reset();
    statisticsCollector_.reset();
    realTimeMonitor_.reset();
    
    initialized_ = false;
    profiling_ = false;
    
    // Log shutdown
    if (logger_) {
        logger_->info("Network profiler shutdown");
    }
}

void NetworkProfiler::recordSpikePropagation(size_t neuronId, double time, double delay) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    spikePropagationEvents_.push_back({neuronId, time, delay});
    
    // Keep history size manageable
    while (spikePropagationEvents_.size() > spikePropagationHistoryLimit_) {
        spikePropagationEvents_.pop_front();
    }
    
    // Notify callback
    if (networkCallback_) {
        networkCallback_("spike_propagation", delay);
    }
}

void NetworkProfiler::recordSynapticOperation(const std::string& operation, size_t synapseCount,
                                             double time, double weightChange) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    synapticOperations_.push_back({operation, synapseCount, time, weightChange});
    
    // Keep history size manageable
    while (synapticOperations_.size() > operationHistoryLimit_) {
        synapticOperations_.pop_front();
    }
}

void NetworkProfiler::recordPlasticityOperation(const std::string& plasticityType, size_t neuronCount,
                                               double learningRate, double time) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    plasticityOperations_.push_back({plasticityType, neuronCount, learningRate, time});
    
    // Keep history size manageable
    while (plasticityOperations_.size() > operationHistoryLimit_) {
        plasticityOperations_.pop_front();
    }
}

void NetworkProfiler::recordNetworkActivity(size_t activeNeurons, size_t totalNeurons,
                                           double averageFiringRate, double synchrony) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    networkActivity_.push_back({activeNeurons, totalNeurons, averageFiringRate, synchrony});
    
    // Keep history size manageable
    while (networkActivity_.size() > operationHistoryLimit_) {
        networkActivity_.pop_front();
    }
}

void NetworkProfiler::recordConnectivityChange(size_t oldConnections, size_t newConnections,
                                              double rewiringRate) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    connectivityChanges_.push_back({oldConnections, newConnections, rewiringRate});
    
    // Keep history size manageable
    while (connectivityChanges_.size() > operationHistoryLimit_) {
        connectivityChanges_.pop_front();
    }
}

void NetworkProfiler::integrateWithLogger(std::shared_ptr<Logger> logger) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    logger_ = logger;
    
    if (logger_) {
        logger_->info("Network profiler integrated with Logger");
    }
}

void NetworkProfiler::integrateWithStatisticsCollector(std::shared_ptr<StatisticsCollector> collector) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    statisticsCollector_ = collector;
    
    if (statisticsCollector_) {
        if (logger_) {
            logger_->info("Network profiler integrated with StatisticsCollector");
        }
    }
}

void NetworkProfiler::integrateWithRealTimeMonitor(std::shared_ptr<RealTimeMonitor> monitor) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    realTimeMonitor_ = monitor;
    
    if (realTimeMonitor_) {
        if (logger_) {
            logger_->info("Network profiler integrated with RealTimeMonitor");
        }
    }
}

void NetworkProfiler::setupNetworkProfilingForMode(const std::string& mode) {
    setupNetworkProfilingForMode(mode, 0);
}

void NetworkProfiler::setupNetworkProfilingForMode(const std::string& mode, size_t neuronCount) {
    // Setup network profiling for specific simulation mode
    profiling_ = true;
    
    // Configure based on mode
    if (mode == "benchmark" || mode == "scaling_benchmark") {
        enableSpikeTimingAnalysis_ = true;
        enableSynapticAnalysis_ = true;
    }
    
    // Record mode-specific metrics
    if (logger_) {
        logger_->info("Network profiler setup for mode: " + mode);
    }
}

void NetworkProfiler::profileNetworkOperation(const std::string& operation, size_t neuronCount,
                                             double time, size_t spikeCount) {
    // Record operation metrics
    recordNetworkActivity(neuronCount / 2, neuronCount, 0.0, 0.0);
    recordSpikePropagation(0, time, time);
    
    // Log operation
    if (logger_) {
        logger_->debug("Network operation: " + operation + " neurons: " + 
                      std::to_string(neuronCount) + " spikes: " + std::to_string(spikeCount));
    }
}

void NetworkProfiler::startProfiling() {
    profiling_ = true;
}

void NetworkProfiler::stopProfiling() {
    profiling_ = false;
}

void NetworkProfiler::analyzeNetworkPerformance() {
    std::ostringstream oss;
    oss << "Network Performance Analysis\n";
    oss << "============================\n\n";
    
    // Analyze spike propagation
    oss << "Spike Propagation Analysis:\n";
    oss << "  Events: " << spikePropagationEvents_.size() << "\n";
    oss << "  Average Delay: " << calculateAverage(spikePropagationEvents_) << " s\n";
    oss << "  Standard Deviation: " << calculateStandardDeviation(spikePropagationEvents_, 
                                                              calculateAverage(spikePropagationEvents_)) << " s\n";
    
    // Analyze synaptic operations
    oss << "\nSynaptic Operations:\n";
    oss << "  Events: " << synapticOperations_.size() << "\n";
    
    // Analyze plasticity operations
    oss << "\nPlasticity Operations:\n";
    oss << "  Events: " << plasticityOperations_.size() << "\n";
    
    // Analyze network dynamics
    oss << "\nNetwork Dynamics:\n";
    oss << "  Activity Samples: " << networkActivity_.size() << "\n";
    oss << "  Average Synchronousity: " << calculateSynchronousity(networkActivity_) << "%\n";
    
    // Generate network performance report
    std::string report = oss.str();
    
    if (logger_) {
        logger_->info(report);
    }
}

void NetworkProfiler::generateNetworkReport() {
    std::ostringstream oss;
    oss << "Network Performance Report\n";
    oss << "=========================\n\n";
    
    // Generate network performance summary
    oss << "Network Performance Summary:\n";
    oss << "  Average Spike Propagation Delay: " << calculateAverage(spikePropagationEvents_) << " s\n";
    oss << "  Average Network Synchronousity: " << calculateSynchronousity(networkActivity_) << "%\n";
    oss << "  Total Spike Propagation Events: " << spikePropagationEvents_.size() << "\n";
    oss << "  Total Synaptic Operations: " << synapticOperations_.size() << "\n";
    oss << "  Total Plasticity Operations: " << plasticityOperations_.size() << "\n";
    
    // Generate network analysis
    analyzeNetworkPerformance();
    
    std::string report = oss.str();
    
    if (logger_) {
        logger_->info(report);
    }
}

void NetworkProfiler::exportNetworkData(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    file << "Network Performance Data\n";
    file << "========================\n\n";
    
    file << "Spike Propagation Events:\n";
    file << "NeuronID,Time,Delay\n";
    for (const auto& event : spikePropagationEvents_) {
        file << std::get<0>(event) << "," << std::get<1>(event) << "," << std::get<2>(event) << "\n";
    }
    
    file << "\nSynaptic Operations:\n";
    file << "Operation,SynapseCount,Time,WeightChange\n";
    for (const auto& event : synapticOperations_) {
        file << std::get<0>(event) << "," << std::get<1>(event) << "," << std::get<2>(event) << "," << std::get<3>(event) << "\n";
    }
    
    file.close();
}

void NetworkProfiler::analyzeNetworkPerformance() const {
    std::ostringstream oss;
    oss << "Network Performance Analysis\n";
    oss << "============================\n\n";
    
    // Analyze spike propagation
    oss << "Spike Propagation Performance:\n";
    double avgDelay = calculateAverage(spikePropagationEvents_);
    oss << "  Average Delay: " << formatTime(avgDelay) << "\n";
    
    double stdDev = calculateStandardDeviation(spikePropagationEvents_, avgDelay);
    oss << "  Standard Deviation: " << formatTime(stdDev) << "\n";
    
    if (avgDelay > 0.01) { // 10ms threshold
        oss << "  WARNING: High spike propagation delay!\n";
    }
    
    // Analyze synaptic operations
    oss << "\nSynaptic Operation Performance:\n";
    oss << "  Total Operations: " << synapticOperations_.size() << "\n";
    
    // Analyze plasticity operations
    oss << "\nPlasticity Operation Performance:\n";
    oss << "  Total Operations: " << plasticityOperations_.size() << "\n";
    
    // Analyze network dynamics
    oss << "\nNetwork Dynamics Analysis:\n";
    double synchrony = calculateSynchronousity(networkActivity_);
    oss << "  Network Synchronousity: " << synchrony << "%\n";
    
    if (logger_) {
        logger_->info(oss.str());
    }
}

double NetworkProfiler::calculateAverageSpikePropagationDelay() const {
    return calculateAverage(spikePropagationEvents_);
}

double NetworkProfiler::getTotalSpikeCount() const {
    double total = 0.0;
    for (const auto& event : spikePropagationEvents_) {
        total += 1.0; // Each event represents one spike
    }
    return total;
}

double NetworkProfiler::getAverageFiringRate() const {
    if (networkActivity_.empty()) return 0.0;
    
    double totalRate = 0.0;
    for (const auto& activity : networkActivity_) {
        size_t active = std::get<0>(activity);
        size_t total = std::get<1>(activity);
        if (total > 0) {
            totalRate += static_cast<double>(active) / total;
        }
    }
    
    return totalRate / networkActivity_.size();
}

double NetworkProfiler::getNetworkSynchronousity() const {
    return calculateSynchronousity(networkActivity_);
}

double NetworkProfiler::getAveragePlasticityChange() const {
    if (plasticityOperations_.empty()) return 0.0;
    
    double totalChange = 0.0;
    for (const auto& event : plasticityOperations_) {
        totalChange += std::get<3>(event); // weight change
    }
    
    return totalChange / plasticityOperations_.size();
}

double NetworkProfiler::getConnectivityChangeRate() const {
    if (connectivityChanges_.empty()) return 0.0;
    
    double totalChange = 0.0;
    for (const auto& event : connectivityChanges_) {
        totalChange += std::get<2>(event); // rewiring rate
    }
    
    return totalChange / connectivityChanges_.size();
}

std::string NetworkProfiler::analyzeSpikePropagationPerformance() const {
    std::ostringstream oss;
    oss << "Spike Propagation Performance Analysis\n";
    oss << "===================================\n\n";
    
    oss << "Total Spike Propagation Events: " << spikePropagationEvents_.size() << "\n";
    oss << "Average Delay: " << formatTime(calculateAverage(spikePropagationEvents_)) << "\n";
    oss << "Standard Deviation: " << formatTime(calculateStandardDeviation(spikePropagationEvents_, 
                                                                              calculateAverage(spikePropagationEvents_))) << "\n";
    
    if (spikePropagationEvents_.size() > 0) {
        double maxDelay = 0.0;
        for (const auto& event : spikePropagationEvents_) {
            maxDelay = std::max(maxDelay, std::get<2>(event)); // delay
        }
        oss << "Maximum Delay: " << formatTime(maxDelay) << "\n";
    }
    
    return oss.str();
}

std::string NetworkProfiler::analyzeSynapticPerformance() const {
    std::ostringstream oss;
    oss << "Synaptic Operation Performance Analysis\n";
    oss << "=====================================\n\n";
    
    oss << "Total Synaptic Operations: " << synapticOperations_.size() << "\n";
    
    // Group by operation type
    std::map<std::string, size_t> opsByType;
    for (const auto& event : synapticOperations_) {
        opsByType[std::get<0>(event)]++;
    }
    
    oss << "Operations by Type:\n";
    for (const auto& pair : opsByType) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    return oss.str();
}

std::string NetworkProfiler::analyzePlasticityPerformance() const {
    std::ostringstream oss;
    oss << "Plasticity Operation Performance Analysis\n";
    oss << "=========================================\n\n";
    
    oss << "Total Plasticity Operations: " << plasticityOperations_.size() << "\n";
    
    // Group by plasticity type
    std::map<std::string, size_t> plasticityByType;
    double totalLearningRate = 0.0;
    double totalTime = 0.0;
    
    for (const auto& event : plasticityOperations_) {
        std::string type = std::get<0>(event);
        plasticityByType[type]++;
        totalLearningRate += std::get<2>(event); // learning rate
        totalTime += std::get<3>(event); // time
    }
    
    oss << "Plasticity Types:\n";
    for (const auto& pair : plasticityByType) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    if (!plasticityOperations_.empty()) {
        oss << "\nAverage Learning Rate: " << totalLearningRate / plasticityOperations_.size() << "\n";
        oss << "Average Operation Time: " << formatTime(totalTime / plasticityOperations_.size()) << "\n";
    }
    
    return oss.str();
}

std::string NetworkProfiler::analyzeNetworkDynamics() const {
    std::ostringstream oss;
    oss << "Network Dynamics Analysis\n";
    oss << "=========================\n\n";
    
    oss << "Network Activity Samples: " << networkActivity_.size() << "\n";
    oss << "Average Synchronousity: " << getNetworkSynchronousity() << "%\n";
    
    // Calculate activity statistics
    if (!networkActivity_.empty()) {
        double totalActive = 0.0;
        double totalNeurons = 0.0;
        
        for (const auto& activity : networkActivity_) {
            totalActive += static_cast<double>(std::get<0>(activity)); // active neurons
            totalNeurons += static_cast<double>(std::get<1>(activity)); // total neurons
        }
        
        double avgActivity = totalNeurons > 0 ? totalActive / totalNeurons : 0.0;
        oss << "Average Network Activity: " << avgActivity * 100.0 << "%\n";
    }
    
    return oss.str();
}

std::vector<std::string> NetworkProfiler::getPerformanceOptimizationSuggestions() const {
    std::vector<std::string> suggestions;
    
    // Suggest based on spike propagation analysis
    if (!spikePropagationEvents_.empty()) {
        double avgDelay = calculateAverage(spikePropagationEvents_);
        if (avgDelay > 0.01) { // 10ms
            suggestions.push_back("Optimize spike propagation mechanisms to reduce transmission delays");
        }
    }
    
    // Suggest based on synaptic operations
    if (synapticOperations_.size() > 1000) { // Threshold for heavy synaptic activity
        suggestions.push_back("Consider optimizing synaptic operations for reduced computational load");
    }
    
    // Suggest based on network dynamics
    if (getNetworkSynchronousity() < 50.0) { // Low synchronousity
        suggestions.push_back("Improve network synchronization for better coordinated activity");
    }
    
    // General suggestions
    suggestions.push_back("Implement efficient data structures for spike transmission");
    suggestions.push_back("Optimize synaptic weight update algorithms");
    suggestions.push_back("Enhance plasticity mechanisms for faster learning");
    
    return suggestions;
}

std::string NetworkProfiler::getNetworkPerformanceSummary() const {
    std::ostringstream oss;
    oss << "Network Performance Summary\n";
    oss << "==========================\n\n";
    
    oss << "Spike Propagation:\n";
    oss << "  Events: " << spikePropagationEvents_.size() << "\n";
    oss << "  Avg Delay: " << formatTime(calculateAverage(spikePropagationEvents_)) << "\n";
    
    oss << "\nSynaptic Operations:\n";
    oss << "  Total: " << synapticOperations_.size() << "\n";
    
    oss << "\nPlasticity Operations:\n";
    oss << "  Total: " << plasticityOperations_.size() << "\n";
    
    oss << "\nNetwork Activity:\n";
    oss << "  Samples: " << networkActivity_.size() << "\n";
    oss << "  Avg Synchronousity: " << getNetworkSynchronousity() << "%\n";
    
    return oss.str();
}

std::string NetworkProfiler::getDetailedPerformanceReport() const {
    std::ostringstream oss;
    oss << "Detailed Network Performance Report\n";
    oss << "==================================\n\n";
    
    oss << "=== Spike Propagation Analysis ===\n";
    oss << analyzeSpikePropagationPerformance() << "\n";
    
    oss << "=== Synaptic Operation Analysis ===\n";
    oss << analyzeSynapticPerformance() << "\n";
    
    oss << "=== Plasticity Operation Analysis ===\n";
    oss << analyzePlasticityPerformance() << "\n";
    
    oss << "=== Network Dynamics Analysis ===\n";
    oss << analyzeNetworkDynamics() << "\n";
    
    oss << "=== Performance Optimization Suggestions ===\n";
    auto suggestions = getPerformanceOptimizationSuggestions();
    for (size_t i = 0; i < suggestions.size(); ++i) {
        oss << (i + 1) << ". " << suggestions[i] << "\n";
    }
    
    return oss.str();
}

} // namespace nlm
