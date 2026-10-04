// Performance Monitoring and Profiling System Implementation
// Clean implementation with proper separation of declarations and implementations

#include <set>
#include "performance/PerformanceMonitoring.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Performance metrics toJSON implementation
std::string PerformanceMetrics::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"simulationTime\": " << simulationTime << ",\n";
    oss << "  \"realTimeFactor\": " << realTimeFactor << ",\n";
    oss << "  \"neuronsPerSecond\": " << neuronsPerSecond << ",\n";
    oss << "  \"synapsesPerSecond\": " << synapsesPerSecond << ",\n";
    oss << "  \"spikesPerSecond\": " << spikesPerSecond << ",\n";
    oss << "  \"avgStepTimeMs\": " << avgStepTimeMs << ",\n";
    oss << "  \"totalMemoryMB\": " << totalMemoryMB << ",\n";
    oss << "  \"activeNeurons\": " << activeNeurons << ",\n";
    oss << "  \"avgFiringRateHz\": " << avgFiringRateHz << ",\n";
    oss << "  \"cpuUtilizationPercent\": " << cpuUtilizationPercent << "\n";
    oss << "}\n";
    return oss.str();
}

// Performance Monitor implementation
PerformanceMonitor::PerformanceMonitor() : running_(false), currentStep_(0), totalSteps_(0),
    totalSpikes_(0), activeNeurons_(0), startTime_(std::chrono::high_resolution_clock::now()) {}

PerformanceMonitor::~PerformanceMonitor() {
    if (running_) {
        stop();
    }
}

void PerformanceMonitor::start() {
    running_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
    stepTimes_.clear();
    spikeCounts_.clear();
    activeNeuronCounts_.clear();
    totalSteps_ = 0;
    totalSpikes_ = 0;
    activeNeurons_ = 0;
    
    NLM_LOG_INFO("Performance monitoring started");
}

void PerformanceMonitor::stop() {
    running_ = false;
    NLM_LOG_INFO("Performance monitoring stopped");
}

void PerformanceMonitor::recordStep(double stepTimeMs, size_t spikes, size_t activeNeurons) {
    if (!running_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    stepTimes_.push_back(stepTimeMs);
    spikeCounts_.push_back(spikes);
    activeNeuronCounts_.push_back(activeNeurons);
    
    totalSteps_++;
    totalSpikes_ += spikes;
    activeNeurons_ = activeNeurons;
}

PerformanceMetrics PerformanceMonitor::getCurrentMetrics(size_t windowSize) const {
    PerformanceMetrics metrics;
    
    if (stepTimes_.empty()) {
        return metrics;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t actualWindow = std::min(windowSize, static_cast<size_t>(stepTimes_.size()));
    
    double sumStepTime = 0.0;
    double minStepTime = std::numeric_limits<double>::max();
    double maxStepTime = 0.0;
    size_t sumSpikes = 0;
    size_t sumActiveNeurons = 0;
    
    for (size_t i = stepTimes_.size() - actualWindow; i < stepTimes_.size(); ++i) {
        sumStepTime += stepTimes_[i];
        minStepTime = std::min(minStepTime, stepTimes_[i]);
        maxStepTime = std::max(maxStepTime, stepTimes_[i]);
        sumSpikes += spikeCounts_[i];
        sumActiveNeurons += activeNeuronCounts_[i];
    }
    
    metrics.avgStepTimeMs = sumStepTime / actualWindow;
    metrics.minStepTimeMs = minStepTime;
    metrics.maxStepTimeMs = maxStepTime;
    
    double variance = 0.0;
    for (size_t i = stepTimes_.size() - actualWindow; i < stepTimes_.size(); ++i) {
        variance += (stepTimes_[i] - metrics.avgStepTimeMs) * (stepTimes_[i] - metrics.avgStepTimeMs);
    }
    metrics.stepTimeStdDev = std::sqrt(variance / actualWindow);
    
    metrics.spikesPerSecond = static_cast<size_t>(sumSpikes / (actualWindow * 0.001));
    metrics.activeNeurons = sumActiveNeurons / actualWindow;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime_);
    metrics.simulationTime = duration.count();
    
    metrics.neuronsPerSecond = totalSpikes_ / metrics.simulationTime;
    
    return metrics;
}

PerformanceMetrics PerformanceMonitor::getSnapshot() const {
    return getCurrentMetrics(stepTimes_.size());
}

bool PerformanceMonitor::isStable(size_t windowSize, double threshold) const {
    if (stepTimes_.size() < windowSize) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    double windowAvg = 0.0;
    for (size_t i = stepTimes_.size() - windowSize; i < stepTimes_.size(); ++i) {
        windowAvg += stepTimes_[i];
    }
    windowAvg /= windowSize;
    
    double maxDeviation = 0.0;
    for (size_t i = stepTimes_.size() - windowSize; i < stepTimes_.size(); ++i) {
        double deviation = std::abs(stepTimes_[i] - windowAvg) / windowAvg;
        maxDeviation = std::max(maxDeviation, deviation);
    }
    
    return maxDeviation <= threshold;
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    stepTimes_.clear();
    spikeCounts_.clear();
    activeNeuronCounts_.clear();
    totalSteps_ = 0;
    totalSpikes_ = 0;
    activeNeurons_ = 0;
}

// Neural Profiler implementation - simplified for compilation
NeuralProfiler::NeuralProfiler() : running_(false), startStep_(0), currentStep_(0) {}

NeuralProfiler::~NeuralProfiler() {
    if (running_) {
        stop();
    }
}

void NeuralProfiler::startProfiling() {
    running_ = true;
    startStep_ = 0;
    currentStep_ = 0;
    NLM_LOG_INFO("Neural profiling started");
}

void NeuralProfiler::stopProfiling() {
    running_ = false;
    NLM_LOG_INFO("Neural profiling stopped");
}

void NeuralProfiler::recordNeuralActivity(const Brain* brain, SimulationStep step) {
    if (!running_ || !brain) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    currentStep_ = step;
    
    neuronActivities_[step] = std::map<NeuronId, float>();
}

struct NeuralProfiler::NeuralProfile NeuralProfiler::getProfileData() const {
    NeuralProfile profile;
    profile.totalSpikes = spikeHistory_.size();
    
    std::set<NeuronId> uniqueNeurons;
    for (const auto& spike : spikeHistory_) {
        uniqueNeurons.insert(std::get<1>(spike));
    }
    profile.uniqueActiveNeurons = uniqueNeurons.size();
    
    return profile;
}

// Memory Profiler implementation
MemoryProfiler::MemoryProfiler() : running_(false), startTime_(std::chrono::high_resolution_clock::now()),
    peakMemoryMB_(0), currentMemoryMB_(0) {}

MemoryProfiler::~MemoryProfiler() {
    if (running_) {
        stop();
    }
}

void MemoryProfiler::startProfiling() {
    running_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
    samples_.clear();
    peakMemoryMB_ = 0;
    currentMemoryMB_ = 0;
    NLM_LOG_INFO("Memory profiling started");
}

void MemoryProfiler::stopProfiling() {
    running_ = false;
    NLM_LOG_INFO("Memory profiling stopped");
}

void MemoryProfiler::record() {
    if (!running_) return;
    samples_.push_back(0); // Simplified measurement
    currentMemoryMB_ = 0;
    peakMemoryMB_ = std::max(peakMemoryMB_, currentMemoryMB_);
}

MemoryProfiler::MemoryProfile MemoryProfiler::getMemoryProfile() const {
    MemoryProfile profile;
    profile.samples = samples_;
    profile.startTime = startTime_;
    profile.endTime = std::chrono::high_resolution_clock::now();
    
    if (!samples_.empty()) {
        profile.peakMemoryMB = *std::max_element(samples_.begin(), samples_.end());
        profile.averageMemoryMB = std::accumulate(samples_.begin(), samples_.end(), 0.0) / samples_.size();
        profile.minMemoryMB = *std::min_element(samples_.begin(), samples_.end());
        
        if (samples_.size() > 1) {
            double start = static_cast<double>(samples_.front());
            double end = static_cast<double>(samples_.back());
            profile.memoryGrowthRate = (end - start) / samples_.size();
        }
    }
    
    return profile;
}

// Algorithm Profiler implementation
AlgorithmProfiler::AlgorithmProfiler() : running_(false), stepCount_(0) {}

AlgorithmProfiler::~AlgorithmProfiler() {
    if (running_) {
        stop();
    }
}

void AlgorithmProfiler::start() {
    running_ = true;
    stepCount_ = 0;
    NLM_LOG_INFO("Algorithm profiling started");
}

void AlgorithmProfiler::stop() {
    running_ = false;
    NLM_LOG_INFO("Algorithm profiling stopped");
}

void AlgorithmProfiler::recordStep(double stepTimeMs, size_t neuronCount, size_t synapseCount,
    size_t spikeCount, size_t activeNeurons) {
    if (!running_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    stepCount_++;
    stepData_.push_back({stepCount_, stepTimeMs, neuronCount, synapseCount, spikeCount, activeNeurons});
}

AlgorithmProfiler::AlgorithmProfile AlgorithmProfiler::getProfile() const {
    AlgorithmProfile profile;
    
    if (!running_) {
        return profile;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    profile.stepData = stepData_;
    
    if (!stepData_.empty()) {
        double sumStepTime = 0.0;
        double sumSqStepTime = 0.0;
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        size_t totalSpikes = 0;
        
        for (const auto& [step, stepTime, neurons, synapses, spikes, active] : stepData_) {
            sumStepTime += stepTime;
            sumSqStepTime += stepTime * stepTime;
            totalNeurons += neurons;
            totalSynapses += synapses;
            totalSpikes += spikes;
        }
        
        profile.avgStepTime = sumStepTime / stepData_.size();
        profile.avgStepTimeStdDev = std::sqrt((sumSqStepTime / stepData_.size()) - 
            (profile.avgStepTime * profile.avgStepTime));
        
        profile.complexityScore = static_cast<double>(stepData_.size()) * 
            std::log10(static_cast<double>(stepData_.size() + 1));
        profile.efficiencyScore = totalSpikes / (totalNeurons + 1);
    }
    
    return profile;
}

} // namespace nlm