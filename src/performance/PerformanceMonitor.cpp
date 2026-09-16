// Performance Monitor Implementation

#include "PerformanceMonitor.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct PerformanceMonitor::Impl {
    // Core monitoring state
    bool running_ = false;
    bool realTimeMonitoring_ = false;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::duration<double> interval_ = std::chrono::duration<double>(1.0);
    
    // Metric history
    std::vector<double> stepTimes_;
    std::vector<size_t> spikeCounts_;
    std::vector<size_t> activeNeuronCounts_;
    
    // Running averages
    double avgStepTimeMs_ = 0.0;
    double avgSpikesPerStep_ = 0.0;
    double avgActiveNeurons_ = 0.0;
    
    // Counters
    uint64_t totalSteps_ = 0;
    uint64_t totalSpikes_ = 0;
    uint64_t totalActiveNeurons_ = 0;
    
    // Performance thresholds
    double stepTimeThresholdMs_ = 100.0;  // 100ms threshold for step time
    double spikesPerStepThreshold_ = 10.0;  // 10 spikes per step threshold
    double activeNeuronsThreshold_ = 1000.0;  // 1000 active neurons threshold
    
    // Real-time monitoring thread
    std::thread monitoringThread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stopMonitoring_{false};
    
    // File output
    std::string outputFile_;
    bool autoSave_ = false;
    size_t autoSaveIntervalSteps_ = 1000;
    size_t autoSaveStepCount_ = 0;
};

PerformanceMonitor::PerformanceMonitor() : pImpl_(std::make_unique<Impl>()) {
    // Initialize with reasonable defaults
    pImpl_->stepTimes_.reserve(1000);
    pImpl_->spikeCounts_.reserve(1000);
    pImpl_->activeNeuronCounts_.reserve(1000);
}

PerformanceMonitor::~PerformanceMonitor() {
    stop();
}

void PerformanceMonitor::start() {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    if (pImpl_->running_) {
        return;
    }
    
    pImpl_->running_ = true;
    pImpl_->startTime_ = std::chrono::high_resolution_clock::now();
    pImpl_->stopMonitoring_ = false;
    
    // Clear existing data
    pImpl_->stepTimes_.clear();
    pImpl_->spikeCounts_.clear();
    pImpl_->activeNeuronCounts_.clear();
    pImpl_->totalSteps_ = 0;
    pImpl_->totalSpikes_ = 0;
    pImpl_->totalActiveNeurons_ = 0;
    pImpl_->avgStepTimeMs_ = 0.0;
    pImpl_->avgSpikesPerStep_ = 0.0;
    pImpl_->avgActiveNeurons_ = 0.0;
    
    // Start real-time monitoring thread
    pImpl_->monitoringThread_ = std::thread([this]() {
        while (!pImpl_->stopMonitoring_) {
            std::unique_lock<std::mutex> lock(pImpl_->mutex_);
            
            // Record current metrics
            recordStep(0.0, 0, 0);  // Placeholder values
            
            // Update running averages
            updateRunningAverages();
            
            // Check thresholds
            checkThresholds();
            
            // Auto-save if enabled
            if (pImpl_->autoSave_ && ++pImpl_->autoSaveStepCount_ >= pImpl_->autoSaveIntervalSteps_) {
                saveToFile();
                pImpl_->autoSaveStepCount_ = 0;
            }
            
            // Wait for next interval
            pImpl_->cv_.wait_for(lock, pImpl_->interval_);
        }
    });
}

void PerformanceMonitor::stop() {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    if (!pImpl_->running_) {
        return;
    }
    
    pImpl_->stopMonitoring_ = true;
    pImpl_->cv_.notify_all();
    
    if (pImpl_->monitoringThread_.joinable()) {
        pImpl_->monitoringThread_.join();
    }
    
    pImpl_->running_ = false;
}

void PerformanceMonitor::recordStep(double stepTimeMs, size_t spikes, size_t activeNeurons) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    // Add to history
    pImpl_->stepTimes_.push_back(stepTimeMs);
    pImpl_->spikeCounts_.push_back(spikes);
    pImpl_->activeNeuronCounts_.push_back(activeNeurons);
    
    // Update counters
    pImpl_->totalSteps_++;
    pImpl_->totalSpikes_ += spikes;
    pImpl_->totalActiveNeurons_ += activeNeurons;
    
    // Update running averages (simple exponential moving average)
    const double alpha = 0.1;  // Smoothing factor
    pImpl_->avgStepTimeMs_ = alpha * stepTimeMs + (1 - alpha) * pImpl_->avgStepTimeMs_;
    pImpl_->avgSpikesPerStep_ = alpha * static_cast<double>(spikes) + (1 - alpha) * pImpl_->avgSpikesPerStep_;
    pImpl_->avgActiveNeurons_ = alpha * static_cast<double>(activeNeurons) + (1 - alpha) * pImpl_->avgActiveNeurons_;
    
    // Check if we need to limit history size
    if (pImpl_->stepTimes_.size() > 10000) {
        pImpl_->stepTimes_.erase(pImpl_->stepTimes_.begin());
        pImpl_->spikeCounts_.erase(pImpl_->spikeCounts_.begin());
        pImpl_->activeNeuronCounts_.erase(pImpl_->activeNeuronCounts_.begin());
    }
}

BenchmarkMetrics PerformanceMonitor::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    BenchmarkMetrics metrics;
    
    // Calculate basic metrics
    if (!pImpl_->stepTimes_.empty()) {
        // Step timing metrics
        metrics.avgStepTimeMs = pImpl_->avgStepTimeMs_;
        
        // Calculate min/max/stddev
        double sum = std::accumulate(pImpl_->stepTimes_.begin(), pImpl_->stepTimes_.end(), 0.0);
        metrics.avgStepTimeMs = sum / pImpl_->stepTimes_.size();
        
        if (pImpl_->stepTimes_.size() > 1) {
            double sumSquares = 0.0;
            for (double time : pImpl_->stepTimes_) {
                double diff = time - metrics.avgStepTimeMs;
                sumSquares += diff * diff;
            }
            metrics.stepTimeStdDev = std::sqrt(sumSquares / (pImpl_->stepTimes_.size() - 1));
        }
        
        metrics.minStepTimeMs = *std::min_element(pImpl_->stepTimes_.begin(), pImpl_->stepTimes_.end());
        metrics.maxStepTimeMs = *std::max_element(pImpl_->stepTimes_.begin(), pImpl_->stepTimes_.end());
        
        // Performance metrics
        metrics.realTimeFactor = 1.0;  // Would need actual simulation time for accurate calculation
        
        // Neural activity metrics
        if (!pImpl_->spikeCounts_.empty()) {
            double totalSpikes = std::accumulate(pImpl_->spikeCounts_.begin(), pImpl_->spikeCounts_.end(), 0.0);
            metrics.avgFiringRateHz = (totalSpikes / pImpl_->stepTimes_.size()) / 1000.0;  // spikes per second
            
            double totalActive = std::accumulate(pImpl_->activeNeuronCounts_.begin(), pImpl_->activeNeuronCounts_.end(), 0.0);
            metrics.activeNeurons = static_cast<size_t>(totalActive / pImpl_->activeNeuronCounts_.size());
            metrics.firingNeurons = static_cast<size_t>(totalSpikes > 0 ? 1 : 0);  // Simplified
        }
        
        // Memory metrics (placeholder)
        metrics.totalMemoryMB = 1024;  // 1GB placeholder
        metrics.peakMemoryMB = 1024;   // 1GB peak placeholder
    }
    
    // Set other placeholder values
    metrics.neuronsPerSecond = 0.0;
    metrics.synapsesPerSecond = 0.0;
    metrics.spikesPerSecond = 0.0;
    metrics.synapticEventsPerSecond = 0.0;
    metrics.neuronMemoryMB = 0;
    metrics.synapseMemoryMB = 0;
    metrics.eventQueueMemoryMB = 0;
    metrics.avgFiringRateHz = 0.0;
    metrics.spikeDensity = 0.0;
    metrics.synapticWeightMean = 0.5;
    metrics.synapticWeightStdDev = 0.1;
    metrics.excitatoryRatio = 0.8;
    metrics.connectionDensity = 0.15;
    metrics.cpuUtilizationPercent = 50.0;  // 50% placeholder
    metrics.gpuUtilizationPercent = 0.0;   // No GPU placeholder
    metrics.threadCount = 1;
    metrics.taskPerformance = 0.0;
    metrics.learningAccuracy = 0.0;
    metrics.memoryRetention = 0.0;
    
    return metrics;
}

BenchmarkMetrics PerformanceMonitor::getSnapshot() const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    // Get current metrics and capture them as a snapshot
    BenchmarkMetrics snapshot = getCurrentMetrics();
    
    // Add metadata
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(now - pImpl_->startTime_).count();
    
    snapshot.realTimeFactor = elapsed > 0.0 ? (pImpl_->totalSteps_ * 0.001) / elapsed : 1.0;  // Assuming 1ms per step
    
    return snapshot;
}

bool PerformanceMonitor::isStable(size_t windowSize, double threshold) const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    if (pImpl_->stepTimes_.size() < windowSize) {
        return false;
    }
    
    // Check step time stability
    size_t start = std::max<size_t>(0, pImpl_->stepTimes_.size() - windowSize);
    std::vector<double> recentSteps(pImpl_->stepTimes_.begin() + start, pImpl_->stepTimes_.end());
    
    double mean = std::accumulate(recentSteps.begin(), recentSteps.end(), 0.0) / recentSteps.size();
    double sumSquares = 0.0;
    
    for (double step : recentSteps) {
        double diff = step - mean;
        sumSquares += diff * diff;
    }
    
    double stdDev = std::sqrt(sumSquares / (recentSteps.size() - 1));
    double coefficientOfVariation = (mean > 0.0) ? (stdDev / mean) : 0.0;
    
    return coefficientOfVariation <= threshold;
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    pImpl_->stepTimes_.clear();
    pImpl_->spikeCounts_.clear();
    pImpl_->activeNeuronCounts_.clear();
    pImpl_->totalSteps_ = 0;
    pImpl_->totalSpikes_ = 0;
    pImpl_->totalActiveNeurons_ = 0;
    pImpl_->avgStepTimeMs_ = 0.0;
    pImpl_->avgSpikesPerStep_ = 0.0;
    pImpl_->avgActiveNeurons_ = 0.0;
    pImpl_->autoSaveStepCount_ = 0;
}

const std::vector<double>& PerformanceMonitor::getStepTimes() const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    return pImpl_->stepTimes_;
}

const std::vector<size_t>& PerformanceMonitor::getSpikeCounts() const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    return pImpl_->spikeCounts_;
}

void PerformanceMonitor::updateRunningAverages() {
    // Update running averages using simple exponential moving average
    const double alpha = 0.1;
    
    if (!pImpl_->stepTimes_.empty()) {
        double latestStep = pImpl_->stepTimes_.back();
        pImpl_->avgStepTimeMs_ = alpha * latestStep + (1 - alpha) * pImpl_->avgStepTimeMs_;
    }
    
    if (!pImpl_->spikeCounts_.empty()) {
        size_t latestSpikes = pImpl_->spikeCounts_.back();
        pImpl_->avgSpikesPerStep_ = alpha * static_cast<double>(latestSpikes) + (1 - alpha) * pImpl_->avgSpikesPerStep_;
    }
    
    if (!pImpl_->activeNeuronCounts_.empty()) {
        size_t latestActive = pImpl_->activeNeuronCounts_.back();
        pImpl_->avgActiveNeurons_ = alpha * static_cast<double>(latestActive) + (1 - alpha) * pImpl_->avgActiveNeurons_;
    }
}

void PerformanceMonitor::checkThresholds() {
    // Check if we've exceeded thresholds
    if (pImpl_->avgStepTimeMs_ > pImpl_->stepTimeThresholdMs_) {
        // Log threshold violation
        std::cerr << "[PERFORMANCE WARNING] Step time exceeded threshold: " 
                  << pImpl_->avgStepTimeMs_ << "ms > " 
                  << pImpl_->stepTimeThresholdMs_ << "ms" << std::endl;
    }
    
    if (pImpl_->avgSpikesPerStep_ > pImpl_->spikesPerStepThreshold_) {
        std::cerr << "[PERFORMANCE WARNING] Spikes per step exceeded threshold: " 
                  << pImpl_->avgSpikesPerStep_ << " > " 
                  << pImpl_->spikesPerStepThreshold_ << std::endl;
    }
    
    if (pImpl_->avgActiveNeurons_ > pImpl_->activeNeuronsThreshold_) {
        std::cerr << "[PERFORMANCE WARNING] Active neurons exceeded threshold: " 
                  << pImpl_->avgActiveNeurons_ << " > " 
                  << pImpl_->activeNeuronsThreshold_ << std::endl;
    }
}

bool PerformanceMonitor::saveToFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    std::string fileToSave = filename.empty() ? pImpl_->outputFile_ : filename;
    
    if (fileToSave.empty()) {
        return false;
    }
    
    std::ofstream file(fileToSave, std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    
    // Get current metrics
    BenchmarkMetrics metrics = getCurrentMetrics();
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(now - pImpl_->startTime_).count();
    
    // Write data
    file << std::fixed << std::setprecision(6)
         << elapsed << ","
         << metrics.avgStepTimeMs << ","
         << metrics.stepTimeStdDev << ","
         << metrics.minStepTimeMs << ","
         << metrics.maxStepTimeMs << ","
         << metrics.avgFiringRateHz << ","
         << metrics.activeNeurons << ","
         << metrics.totalMemoryMB << "\n";
    
    file.close();
    return true;
}

bool PerformanceMonitor::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 8) {
            double elapsed = std::stod(tokens[0]);
            double avgStepTime = std::stod(tokens[1]);
            double stdDev = std::stod(tokens[2]);
            double minStepTime = std::stod(tokens[3]);
            double maxStepTime = std::stod(tokens[4]);
            double firingRate = std::stod(tokens[5]);
            size_t activeNeurons = static_cast<size_t>(std::stoull(tokens[6]));
            size_t memoryMB = static_cast<size_t>(std::stoull(tokens[7]));
            
            // Record sample
            recordStep(avgStepTime, static_cast<size_t>(firingRate * 1000.0), activeNeurons);
        }
    }
    
    file.close();
    return true;
}

std::string PerformanceMonitor::generateReport() const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    std::ostringstream oss;
    oss << "=== Performance Monitor Report ===\n\n";
    oss << "Monitoring Started: " << std::fixed << std::setprecision(2) 
        << std::chrono::duration<double>(pImpl_->startTime_.time_since_epoch()).count() << "s ago\n";
    oss << "Total Steps Recorded: " << pImpl_->totalSteps_ << "\n";
    oss << "Total Simulation Time: " << (pImpl_->totalSteps_ * 0.001) << "s\n\n";
    
    // Step timing summary
    if (!pImpl_->stepTimes_.empty()) {
        oss << "Step Timing Summary:\n";
        oss << "  Average: " << std::fixed << std::setprecision(6) << pImpl_->avgStepTimeMs_ << "ms\n";
        oss << "  Minimum: " << std::fixed << std::setprecision(6) << getMinStepTimeMs() << "ms\n";
        oss << "  Maximum: " << std::fixed << std::setprecision(6) << getMaxStepTimeMs() << "ms\n";
        oss << "  Standard Deviation: " << std::fixed << std::setprecision(6) << getStepTimeStdDevMs() << "ms\n\n";
        
        // Performance analysis
        double cv = (pImpl_->avgStepTimeMs_ > 0.0) ? 
            (getStepTimeStdDevMs() / pImpl_->avgStepTimeMs_) * 100.0 : 0.0;
        
        oss << "Performance Analysis:\n";
        oss << "  Coefficient of Variation: " << std::fixed << std::setprecision(2) << cv << "%\n";
        
        if (cv < 10.0) {
            oss << "  Status: STABLE\n";
        } else if (cv < 25.0) {
            oss << "  Status: MODERATELY VARIABLE\n";
        } else {
            oss << "  Status: HIGHLY VARIABLE - Investigation Recommended\n";
        }
    }
    
    // Neural activity summary
    if (!pImpl_->spikeCounts_.empty()) {
        oss << "\nNeural Activity Summary:\n";
        
        double totalSpikes = std::accumulate(pImpl_->spikeCounts_.begin(), pImpl_->spikeCounts_.end(), 0.0);
        double avgSpikesPerStep = (pImpl_->totalSteps_ > 0) ? (totalSpikes / pImpl_->totalSteps_) : 0.0;
        
        oss << "  Average Spikes per Step: " << std::fixed << std::setprecision(2) << avgSpikesPerStep << "\n";
        
        double totalActive = std::accumulate(pImpl_->activeNeuronCounts_.begin(), pImpl_->activeNeuronCounts_.end(), 0.0);
        double avgActiveNeurons = (pImpl_->totalSteps_ > 0) ? (totalActive / pImpl_->totalSteps_) : 0.0;
        
        oss << "  Average Active Neurons: " << std::fixed << std::setprecision(2) << avgActiveNeurons << "\n";
        oss << "  Total Spikes Generated: " << static_cast<size_t>(totalSpikes) << "\n";
    }
    
    // Health status
    oss << "\nHealth Status:\n";
    oss << "  Monitoring Active: " << (pImpl_->running_ ? "Yes" : "No") << "\n";
    oss << "  Samples Collected: " << pImpl_->stepTimes_.size() << "\n";
    
    return oss.str();
}

double PerformanceMonitor::getMinStepTimeMs() const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    if (pImpl_->stepTimes_.empty()) return 0.0;
    return *std::min_element(pImpl_->stepTimes_.begin(), pImpl_->stepTimes_.end());
}

double PerformanceMonitor::getMaxStepTimeMs() const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    if (pImpl_->stepTimes_.empty()) return 0.0;
    return *std::max_element(pImpl_->stepTimes_.begin(), pImpl_->stepTimes_.end());
}

double PerformanceMonitor::getStepTimeStdDevMs() const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    if (pImpl_->stepTimes_.size() < 2) return 0.0;
    
    double mean = getMeanStepTimeMs();
    double sumSquares = 0.0;
    
    for (double step : pImpl_->stepTimes_) {
        double diff = step - mean;
        sumSquares += diff * diff;
    }
    
    return std::sqrt(sumSquares / (pImpl_->stepTimes_.size() - 1));
}

double PerformanceMonitor::getMeanStepTimeMs() const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    if (pImpl_->stepTimes_.empty()) return 0.0;
    
    double sum = std::accumulate(pImpl_->stepTimes_.begin(), pImpl_->stepTimes_.end(), 0.0);
    return sum / pImpl_->stepTimes_.size();
}

void PerformanceMonitor::setOutputFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    pImpl_->outputFile_ = filename;
}

void PerformanceMonitor::setAutoSave(bool enabled, size_t intervalSteps) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    pImpl_->autoSave_ = enabled;
    pImpl_->autoSaveIntervalSteps_ = intervalSteps;
    pImpl_->autoSaveStepCount_ = 0;
}

void PerformanceMonitor::setThresholds(double stepTimeThresholdMs, double spikesPerStepThreshold, double activeNeuronsThreshold) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    pImpl_->stepTimeThresholdMs_ = stepTimeThresholdMs;
    pImpl_->spikesPerStepThreshold_ = spikesPerStepThreshold;
    pImpl_->activeNeuronsThreshold_ = activeNeuronsThreshold;
}

void PerformanceMonitor::setSamplingInterval(double intervalSeconds) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    pImpl_->interval_ = std::chrono::duration<double>(intervalSeconds);
}

} // namespace nlm
