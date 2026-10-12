#include "Profiler.hpp"
#include "../core/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>

namespace nlm {

struct Profiler::Impl {
    // Profiling state
    bool profilingEnabled;
    bool globalProfiling;
    double samplingInterval; // seconds
    
    // Performance sampling
    std::vector<PerformanceSample> samples;
    size_t maxSamples;
    Timestamp lastSampleTime;
    
    // Time complexity measurements
    std::vector<TimeComplexityMeasure> timeComplexityMeasures;
    size_t maxTimeComplexityMeasures;
    
    // Bottlenecks
    std::vector<PerformanceBottleneck> bottlenecks;
    size_t maxBottlenecks;
    
    // System state
    SystemSnapshot currentSnapshot;
    
    // Runtime parameters
    std::map<std::string, std::map<std::string, double>> runtimeParameters;
    
    // Timing
    Timestamp sessionStartTime;
    Timestamp sessionEndTime;
    
    Impl() : profilingEnabled(false), globalProfiling(false), samplingInterval(0.1),
             maxSamples(10000), maxTimeComplexityMeasures(1000), maxBottlenecks(100),
             sessionStartTime(0), sessionEndTime(0) {}
};

Profiler::Profiler() : pImpl(std::make_unique<Impl>()) {}
Profiler::~Profiler() = default;

bool Profiler::initialize() {
    try {
        pImpl->profilingEnabled = true;
        pImpl->sessionStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        Logger::getInstance().logInfo("Profiler initialized");
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to initialize profiler: " + std::string(e.what()));
        return false;
    }
}

void Profiler::startProfiling(const std::string& operationName) {
    if (!pImpl->profilingEnabled) return;
    
    Timestamp startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    TimeComplexityMeasure measure;
    measure.operationName = operationName;
    measure.timestamp = startTime;
    measure.wallClockTime = 0.0;
    measure.cpuTime = 0.0;
    measure.memoryAllocated = 0;
    measure.cacheMisses = 0.0f;
    measure.branchMispredictions = 0.0f;
    
    pImpl->timeComplexityMeasures.push_back(measure);
}

void Profiler::stopProfiling(const std::string& operationName) {
    if (!pImpl->profilingEnabled) return;
    
    Timestamp endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    for (auto& measure : pImpl->timeComplexityMeasures) {
        if (measure.operationName == operationName && measure.wallClockTime == 0.0) {
            measure.wallClockTime = static_cast<double>(endTime - measure.timestamp);
            break;
        }
    }
}

void Profiler::startGlobalProfiling() {
    pImpl->globalProfiling = true;
    pImpl->lastSampleTime = pImpl->sessionStartTime;
}

void Profiler::stopGlobalProfiling() {
    pImpl->globalProfiling = false;
    pImpl->sessionEndTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

bool Profiler::isProfiling() const {
    return pImpl->globalProfiling;
}

void Profiler::takeSample() {
    if (!pImpl->profilingEnabled || !pImpl->globalProfiling) return;
    
    Timestamp currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    if ((currentTime - pImpl->lastSampleTime) < (pImpl->samplingInterval * 1000)) {
        return; // Too soon for another sample
    }
    
    PerformanceSample sample;
    sample.timestamp = currentTime;
    sample.cpuUsage = 0.0; // Placeholder - would need actual system monitoring
    sample.gpuUsage = 0.0; // Placeholder
    sample.memoryUsage = 0; // Placeholder
    sample.allocatedMemory = 0; // Placeholder
    sample.diskIO = 0.0f; // Placeholder
    sample.networkIO = 0.0f; // Placeholder
    sample.threadCount = 0; // Placeholder
    sample.activeThreadCount = 0; // Placeholder
    sample.contextSwitches = 0.0f; // Placeholder
    sample.interrupts = 0.0f; // Placeholder
    
    pImpl->samples.push_back(sample);
    pImpl->lastSampleTime = currentTime;
    
    // Clean old samples
    while (pImpl->samples.size() > pImpl->maxSamples) {
        pImpl->samples.erase(pImpl->samples.begin());
    }
}

void Profiler::takeSample(const PerformanceSample& sample) {
    if (!pImpl->profilingEnabled) return;
    
    pImpl->samples.push_back(sample);
    while (pImpl->samples.size() > pImpl->maxSamples) {
        pImpl->samples.erase(pImpl->samples.begin());
    }
}

void Profiler::recordTimeComplexity(const TimeComplexityMeasure& measure) {
    if (!pImpl->profilingEnabled) return;
    
    pImpl->timeComplexityMeasures.push_back(measure);
    while (pImpl->timeComplexityMeasures.size() > pImpl->maxTimeComplexityMeasures) {
        pImpl->timeComplexityMeasures.erase(pImpl->timeComplexityMeasures.begin());
    }
}

void Profiler::recordTimeComplexity(const std::string& operationName, double executionTime, double cpuTime) {
    TimeComplexityMeasure measure;
    measure.operationName = operationName;
    measure.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    measure.executionTime = executionTime;
    measure.cpuTime = cpuTime;
    measure.wallClockTime = executionTime;
    
    recordTimeComplexity(measure);
}

void Profiler::recordBottleneck(const PerformanceBottleneck& bottleneck) {
    if (!pImpl->profilingEnabled) return;
    
    pImpl->bottlenecks.push_back(bottleneck);
    while (pImpl->bottlenecks.size() > pImpl->maxBottlenecks) {
        pImpl->bottlenecks.erase(pImpl->bottlenecks.begin());
    }
}

void Profiler::recordBottleneck(const std::string& componentName, double impactPercentage, double executionTime) {
    PerformanceBottleneck bottleneck;
    bottleneck.componentName = componentName;
    bottleneck.impactPercentage = impactPercentage;
    bottleneck.executionTime = executionTime;
    bottleneck.percentageOfTotalTime = (executionTime / 1000.0) * 100.0; // Assuming 1 second baseline
    bottleneck.isBlocking = (impactPercentage > 50.0);
    bottleneck.severity = std::min(1.0f, impactPercentage / 100.0f);
    bottleneck.detectedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    recordBottleneck(bottleneck);
}

void Profiler::updateSystemState(const std::string& state) {
    pImpl->currentSnapshot.systemState = state;
}

void Profiler::updateResourceUtilization(const std::map<std::string, float>& utilization) {
    pImpl->currentSnapshot.resourceUtilization = utilization;
}

void Profiler::updatePerformanceMetrics(const std::map<std::string, double>& metrics) {
    pImpl->currentSnapshot.performanceMetrics = metrics;
}

void Profiler::analyzePerformance() {
    // Analyze samples for trends and anomalies
    if (pImpl->samples.size() < 2) return;
    
    // Calculate averages
    double totalCPU = 0.0, totalGPU = 0.0, totalMemory = 0.0;
    size_t count = 0;
    
    for (const auto& sample : pImpl->samples) {
        totalCPU += sample.cpuUsage;
        totalGPU += sample.gpuUsage;
        totalMemory += static_cast<double>(sample.memoryUsage);
        count++;
    }
    
    if (count > 0) {
        pImpl->currentSnapshot.performanceMetrics["average_cpu_usage"] = totalCPU / count;
        pImpl->currentSnapshot.performanceMetrics["average_gpu_usage"] = totalGPU / count;
        pImpl->currentSnapshot.performanceMetrics["average_memory_usage"] = totalMemory / count;
    }
}

void Profiler::identifyBottlenecks() {
    // Simple bottleneck identification based on execution times
    if (pImpl->timeComplexityMeasures.empty()) return;
    
    double totalExecutionTime = 0.0;
    for (const auto& measure : pImpl->timeComplexityMeasures) {
        totalExecutionTime += measure.wallClockTime;
    }
    
    std::map<std::string, double> componentTimes;
    for (const auto& measure : pImpl->timeComplexityMeasures) {
        componentTimes[measure.operationName] += measure.wallClockTime;
    }
    
    // Identify potential bottlenecks (operations taking > 10% of total time)
    for (const auto& [componentName, time] : componentTimes) {
        double percentage = (totalExecutionTime > 0) ? (time / totalExecutionTime) * 100.0 : 0.0;
        if (percentage > 10.0) {
            recordBottleneck(componentName, percentage, time);
        }
    }
}

void Profiler::generatePerformanceReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().logError("Failed to open performance report file: " + filename);
        return;
    }
    
    file << "=== NLM Performance Report ===" << std::endl;
    file << "Session Duration: " << (pImpl->sessionEndTime - pImpl->sessionStartTime) << " ms" << std::endl;
    file << "Samples Recorded: " << pImpl->samples.size() << std::endl;
    file << "Time Complexity Measurements: " << pImpl->timeComplexityMeasures.size() << std::endl;
    file << "Bottlenecks Identified: " << pImpl->bottlenecks.size() << std::endl;
    file << std::endl;
    
    // Performance samples
    file << "=== Performance Samples ===" << std::endl;
    file << "Timestamp,CPU%,GPU%,Memory(MB),ThreadCount" << std::endl;
    for (const auto& sample : pImpl->samples) {
        file << sample.timestamp << "," 
             << std::fixed << std::setprecision(2) << sample.cpuUsage << "," 
             << sample.gpuUsage << "," 
             << (sample.memoryUsage / (1024 * 1024)) << "," 
             << sample.threadCount << std::endl;
    }
    file << std::endl;
    
    // Time complexity
    file << "=== Time Complexity Measurements ===" << std::endl;
    file << "Operation,ExecutionTime(us),CPUTime(us),MemoryAllocated(Bytes)" << std::endl;
    for (const auto& measure : pImpl->timeComplexityMeasures) {
        file << measure.operationName << "," 
             << std::fixed << std::setprecision(2) << measure.executionTime << "," 
             << measure.cpuTime << "," 
             << measure.memoryAllocated << std::endl;
    }
    file << std::endl;
    
    // Bottlenecks
    file << "=== Performance Bottlenecks ===" << std::endl;
    file << "Component,Impact%,ExecutionTime(ms),Severity,IsBlocking" << std::endl;
    for (const auto& bottleneck : pImpl->bottlenecks) {
        file << bottleneck.componentName << "," 
             << std::fixed << std::setprecision(2) << bottleneck.impactPercentage << "," 
             << bottleneck.executionTime << "," 
             << bottleneck.severity << "," 
             << bottleneck.isBlocking << std::endl;
    }
    
    file.close();
    
    Logger::getInstance().logInfo("Performance report generated: " + filename);
}

void Profiler::setSamplingInterval(double seconds) {
    pImpl->samplingInterval = seconds;
}

void Profiler::setMaxSamples(size_t maxSamples) {
    pImpl->maxSamples = maxSamples;
    while (pImpl->samples.size() > maxSamples) {
        pImpl->samples.erase(pImpl->samples.begin());
    }
}

void Profiler::setMaxTimeComplexityMeasures(size_t maxMeasures) {
    pImpl->maxTimeComplexityMeasures = maxMeasures;
    while (pImpl->timeComplexityMeasures.size() > maxMeasures) {
        pImpl->timeComplexityMeasures.erase(pImpl->timeComplexityMeasures.begin());
    }
}

void Profiler::setMaxBottlenecks(size_t maxBottlenecks) {
    pImpl->maxBottlenecks = maxBottlenecks;
    while (pImpl->bottlenecks.size() > maxBottlenecks) {
        pImpl->bottlenecks.erase(pImpl->bottlenecks.begin());
    }
}

void Profiler::enableDetailedProfiling(bool enable) {
    if (enable) {
        pImpl->samplingInterval = 0.05; // More frequent sampling
    } else {
        pImpl->samplingInterval = 0.5; // Less frequent sampling
    }
}

size_t Profiler::getSampleCount() const {
    return pImpl->samples.size();
}

size_t Profiler::getTimeComplexityMeasureCount() const {
    return pImpl->timeComplexityMeasures.size();
}

size_t Profiler::getBottleneckCount() const {
    return pImpl->bottlenecks.size();
}

double Profiler::getAverageCPUUsage() const {
    if (pImpl->samples.empty()) return 0.0;
    
    double totalCPU = 0.0;
    for (const auto& sample : pImpl->samples) {
        totalCPU += sample.cpuUsage;
    }
    
    return totalCPU / pImpl->samples.size();
}

double Profiler::getAverageGPUUsage() const {
    if (pImpl->samples.empty()) return 0.0;
    
    double totalGPU = 0.0;
    for (const auto& sample : pImpl->samples) {
        totalGPU += sample.gpuUsage;
    }
    
    return totalGPU / pImpl->samples.size();
}

size_t Profiler::getAverageMemoryUsage() const {
    if (pImpl->samples.empty()) return 0;
    
    size_t totalMemory = 0;
    for (const auto& sample : pImpl->samples) {
        totalMemory += sample.memoryUsage;
    }
    
    return totalMemory / pImpl->samples.size();
}

void Profiler::adjustParameter(const std::string& componentName, const std::string& parameter, double value) {
    pImpl->runtimeParameters[componentName][parameter] = value;
}

double Profiler::getParameterValue(const std::string& componentName, const std::string& parameter) const {
    auto it1 = pImpl->runtimeParameters.find(componentName);
    if (it1 != pImpl->runtimeParameters.end()) {
        auto it2 = it1->second.find(parameter);
        if (it2 != it1->second.end()) {
            return it2->second;
        }
    }
    return 0.0;
}

} // namespace nlm