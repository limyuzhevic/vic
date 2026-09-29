#include "Performance.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>

namespace nlm {
namespace Performance {

struct PerformanceProfiler::Impl {
    std::vector<MeasurementResult> measurements;
    bool running = false;
    std::chrono::system_clock::time_point startTime;
    std::mutex mutex;
    double totalTime = 0.0;
};

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>()) {}

PerformanceProfiler::~PerformanceProfiler() = default;

void PerformanceProfiler::start() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!pImpl->running) {
        pImpl->running = true;
        pImpl->startTime = std::chrono::system_clock::now();
    }
}

void PerformanceProfiler::stop() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->running) {
        auto endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - pImpl->startTime);
        pImpl->totalTime += duration.count() / 1000.0; // Convert to seconds
        pImpl->running = false;
    }
}

bool PerformanceProfiler::isRunning() const {
    return pImpl->running;
}

void PerformanceProfiler::addMeasurement(const MeasurementResult& measurement) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->measurements.push_back(measurement);
}

std::vector<MeasurementResult> PerformanceProfiler::getMeasurements(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<MeasurementResult> result;
    std::copy_if(pImpl->measurements.begin(), pImpl->measurements.end(),
                std::back_inserter(result),
                [&name](const MeasurementResult& m) { return m.name == name; });
    return result;
}

std::vector<MeasurementResult> PerformanceProfiler::getAllMeasurements() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->measurements;
}

void PerformanceProfiler::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->measurements.clear();
}

std::string PerformanceProfiler::generateReport() const {
    std::ostringstream oss;
    oss << "=== Performance Profiler Report ===" << std::endl;
    oss << "Status: " << (isRunning() ? "Running" : "Stopped") << std::endl;
    oss << "Total Time: " << getTotalTime() << " seconds" << std::endl;
    oss << "Measurement Count: " << getMeasurementCount() << std::endl;
    oss << std::endl;
    
    if (pImpl->measurements.empty()) {
        oss << "No measurements recorded." << std::endl;
        return oss.str();
    }
    
    oss << "Measurements:" << std::endl;
    oss << std::left << std::setw(20) << "Name" 
        << std::setw(15) << "Value" 
        << std::setw(10) << "Unit" 
        << std::setw(20) << "Type" << std::endl;
    oss << std::string(65, '-') << std::endl;
    
    for (const auto& measurement : pImpl->measurements) {
        oss << std::left << std::setw(20) << measurement.name
            << std::setw(15) << measurement.value
            << std::setw(10) << measurement.unit
            << std::setw(20) << categoryToString(measurement.type) << std::endl;
    }
    
    return oss.str();
}

double PerformanceProfiler::getTotalTime() const {
    return pImpl->totalTime;
}

double PerformanceProfiler::getAverageTime(const std::string& name) const {
    auto measurements = getMeasurements(name);
    if (measurements.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& m : measurements) {
        total += m.value;
    }
    return total / measurements.size();
}

size_t PerformanceProfiler::getMeasurementCount() const {
    return pImpl->measurements.size();
}

std::string PerformanceProfiler::categoryToString(MeasurementType type) {
    static const std::unordered_map<MeasurementType, std::string> typeNames = {
        {MeasurementType::Time, "Time"},
        {MeasurementType::Memory, "Memory"},
        {MeasurementType::CPU, "CPU"},
        {MeasurementType::Network, "Network"},
        {MeasurementType::Custom, "Custom"}
    };
    
    auto it = typeNames.find(type);
    return it != typeNames.end() ? it->second : "Unknown";
}

// ResourceMonitor implementation
struct ResourceMonitor::Impl {
    std::vector<ProcessResourceUsage> monitoredProcesses;
    std::mutex mutex;
};

ResourceMonitor::ResourceMonitor() : pImpl(std::make_unique<Impl>()) {}

ResourceMonitor::~ResourceMonitor() = default;

namespace {
    // Helper function to get current process ID
    inline pid_t getCurrentProcessId() {
        return static_cast<pid_t>(::getpid());
    }
}

// Since we're in a sandboxed environment, we'll provide mock implementations
// that can be replaced with real implementations if needed

size_t ResourceMonitor::getMemoryUsage() const {
    // Mock implementation - in real system, this would read from /proc/self/status or similar
    return 1024 * 1024; // Mock: 1MB
}

double ResourceMonitor::getCPUUsage() const {
    // Mock implementation - in real system, this would calculate CPU usage
    return 10.5; // Mock: 10.5%
}

std::chrono::seconds ResourceMonitor::getSystemUptime() const {
    // Mock implementation - in real system, this would read system uptime
    return std::chrono::seconds(3600); // Mock: 1 hour
}

int ResourceMonitor::getThreadCount() const {
    // Mock implementation - in real system, this would count threads
    return 4; // Mock: 4 threads
}

void ResourceMonitor::monitorProcess(pid_t pid) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // Add to monitored processes
    ProcessResourceUsage usage;
    usage.pid = pid;
    // Fill other fields with mock data
    getProcessUsage(pid); // This will populate the rest
    pImpl->monitoredProcesses.push_back(usage);
}

ResourceMonitor::ProcessResourceUsage ResourceMonitor::getProcessUsage(pid_t pid) const {
    ProcessResourceUsage usage;
    usage.pid = pid;
    usage.memoryUsage = getMemoryUsage();
    usage.cpuUsage = getCPUUsage();
    usage.threadCount = getThreadCount();
    usage.startTime = getSystemUptime();
    return usage;
}

std::vector<ResourceMonitor::ProcessResourceUsage> ResourceMonitor::getAllProcessUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<ProcessResourceUsage> result;
    result.push_back(getProcessUsage(getCurrentProcessId()));
    return result;
}

// NeuralSimulationProfiler implementation
struct NeuralSimulationProfiler::Impl {
    Statistics stats;
    std::mutex mutex;
};

NeuralSimulationProfiler::NeuralSimulationProfiler() : pImpl(std::make_unique<Impl>()) {}

NeuralSimulationProfiler::~NeuralSimulationProfiler() = default;

void NeuralSimulationProfiler::profileNeuronStep(int neuronCount, double executionTime) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats.totalNeuronSteps++;
    pImpl->stats.totalNeuronStepTime += executionTime;
}

void NeuralSimulationProfiler::profileSpikePropagation(int spikeCount, double propagationTime) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats.totalSpikeCount += spikeCount;
    pImpl->stats.totalSpikePropagationTime += propagationTime;
}

void NeuralSimulationProfiler::profilePlasticityUpdates(int updateCount, double updateTime) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats.totalPlasticityUpdates += updateCount;
    pImpl->stats.totalPlasticityUpdateTime += updateTime;
}

void NeuralSimulationProfiler::profileMemoryOperation(const std::string& operation, double executionTime, size_t dataSize) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats.totalMemoryOperations++;
    pImpl->stats.totalMemoryOperationTime += executionTime;
}

void NeuralSimulationProfiler::profilePrediction(int predictionCount, double computationTime) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats.totalPredictions += predictionCount;
    pImpl->stats.totalPredictionTime += computationTime;
}

std::string NeuralSimulationProfiler::generateNeuralReport() const {
    std::ostringstream oss;
    oss << "=== Neural Simulation Performance ===" << std::endl;
    
    Statistics stats = getStatistics();
    
    if (stats.totalNeuronSteps == 0) {
        oss << "No neural simulation data available." << std::endl;
        return oss.str();
    }
    
    oss << "Neuron Steps: " << stats.totalNeuronSteps << std::endl;
    oss << "Total Neuron Step Time: " << stats.totalNeuronStepTime << " seconds" << std::endl;
    oss << "Average Neuron Step Time: " << (stats.totalNeuronStepTime / stats.totalNeuronSteps) << " seconds" << std::endl;
    oss << std::endl;
    
    oss << "Spike Count: " << stats.totalSpikeCount << std::endl;
    oss << "Total Spike Propagation Time: " << stats.totalSpikePropagationTime << " seconds" << std::endl;
    if (stats.totalSpikeCount > 0) {
        oss << "Average Spike Propagation Time: " << (stats.totalSpikePropagationTime / stats.totalSpikeCount) << " seconds" << std::endl;
    }
    oss << std::endl;
    
    oss << "Plasticity Updates: " << stats.totalPlasticityUpdates << std::endl;
    oss << "Total Plasticity Update Time: " << stats.totalPlasticityUpdateTime << " seconds" << std::endl;
    if (stats.totalPlasticityUpdates > 0) {
        oss << "Average Plasticity Update Time: " << (stats.totalPlasticityUpdateTime / stats.totalPlasticityUpdates) << " seconds" << std::endl;
    }
    oss << std::endl;
    
    oss << "Memory Operations: " << stats.totalMemoryOperations << std::endl;
    oss << "Total Memory Operation Time: " << stats.totalMemoryOperationTime << " seconds" << std::endl;
    if (stats.totalMemoryOperations > 0) {
        oss << "Average Memory Operation Time: " << (stats.totalMemoryOperationTime / stats.totalMemoryOperations) << " seconds" << std::endl;
    }
    oss << std::endl;
    
    oss << "Predictions: " << stats.totalPredictions << std::endl;
    oss << "Total Prediction Time: " << stats.totalPredictionTime << " seconds" << std::endl;
    if (stats.totalPredictions > 0) {
        oss << "Average Prediction Time: " << (stats.totalPredictionTime / stats.totalPredictions) << " seconds" << std::endl;
    }
    
    return oss.str();
}

Statistics NeuralSimulationProfiler::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

void NeuralSimulationProfiler::clearStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->stats = Statistics();
}

} // namespace nlm
namespace Performance {

// Global performance monitoring instances
PerformanceProfiler& getGlobalProfiler() {
    static PerformanceProfiler profiler;
    return profiler;
}

ResourceMonitor& getResourceMonitor() {
    static ResourceMonitor monitor;
    return monitor;
}

NeuralSimulationProfiler& getNeuralProfiler() {
    static NeuralSimulationProfiler neuralProfiler;
    return neuralProfiler;
}

} // namespace Performance

} // namespace nlm