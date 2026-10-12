// Performance Monitoring and Profiling Infrastructure Implementation
// Detailed implementation of performance monitoring classes

#include "performance/PerformanceMonitor.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <thread>
#include <iomanip>
#include <fstream>

namespace nlm {

// HighResolutionTimer implementation
HighResolutionTimer::HighResolutionTimer() : startTime_(std::chrono::high_resolution_clock::now()) {}

void HighResolutionTimer::reset() {
    startTime_ = std::chrono::high_resolution_clock::now();
}

double HighResolutionTimer::elapsedSeconds() const {
    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(endTime - startTime_).count();
}

double HighResolutionTimer::elapsedMilliseconds() const {
    return elapsedSeconds() * 1000.0;
}

uint64_t HighResolutionTimer::elapsedNanoseconds() const {
    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime_).count();
}

void HighResolutionTimer::startTiming(const std::string& operation) {
    operationTimes_[operation] = std::chrono::high_resolution_clock::now();
}

double HighResolutionTimer::stopTiming(const std::string& operation) {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto it = operationTimes_.find(operation);
    if (it == operationTimes_.end()) {
        return 0.0;
    }
    
    double duration = std::chrono::duration<double>(endTime - it->second).count();
    operationTimes_.erase(it);
    return duration;
}

const auto& HighResolutionTimer::getOperationTimes() const {
    return operationTimes_;
}

// PerformanceStatistics implementation
void PerformanceStatistics::addDataPoint(const PerformanceDataPoint& point) {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    dataPoints_.push_back(point);
    updateStatistics(point);
}

double PerformanceStatistics::getAverageStepsPerSecond() const {
    if (dataPoints_.size() < 2) return 0.0;
    double totalTime = dataPoints_.back().simulationTime - dataPoints_.front().simulationTime;
    double totalSteps = static_cast<double>(dataPoints_.back().simulationStep - dataPoints_.front().simulationStep);
    return totalTime > 0 ? totalSteps / totalTime : 0.0;
}

double PerformanceStatistics::getAverageNeuronUpdatesPerSecond() const {
    if (dataPoints_.empty()) return 0.0;
    double totalTime = getDurationSeconds();
    if (totalTime <= 0) return 0.0;
    
    double totalUpdates = 0.0;
    for (const auto& point : dataPoints_) {
        totalUpdates += point.neuronUpdates;
    }
    return totalUpdates / totalTime / dataPoints_.size();
}

double PerformanceStatistics::getAverageSpikeEventsPerSecond() const {
    if (dataPoints_.empty()) return 0.0;
    double totalTime = getDurationSeconds();
    if (totalTime <= 0) return 0.0;
    
    double totalSpikes = 0.0;
    for (const auto& point : dataPoints_) {
        totalSpikes += point.spikeEvents;
    }
    return totalSpikes / totalTime / dataPoints_.size();
}

uint64_t PerformanceStatistics::getMaxSpikeEvents() const {
    uint64_t max = 0;
    for (const auto& point : dataPoints_) {
        max = std::max(max, point.spikeEvents);
    }
    return max;
}

double PerformanceStatistics::getAverageCPUUsage() const {
    if (dataPoints_.empty()) return 0.0;
    
    double total = 0.0;
    size_t count = 0;
    for (const auto& point : dataPoints_) {
        if (point.cpuUsage > 0.0) {
            total += point.cpuUsage;
            count++;
        }
    }
    return count > 0 ? total / count : 0.0;
}

size_t PerformanceStatistics::getAverageMemoryUsage() const {
    if (dataPoints_.empty()) return 0;
    
    uint64_t total = 0;
    size_t count = 0;
    for (const auto& point : dataPoints_) {
        if (point.memoryUsage > 0) {
            total += point.memoryUsage;
            count++;
        }
    }
    return count > 0 ? total / count : 0;
}

std::vector<double> PerformanceStatistics::getSimulationTimeSeries() const {
    std::vector<double> times;
    times.reserve(dataPoints_.size());
    for (const auto& point : dataPoints_) {
        times.push_back(point.simulationTime);
    }
    return times;
}

std::vector<uint64_t> PerformanceStatistics::getSpikeEventTimeSeries() const {
    std::vector<uint64_t> spikes;
    spikes.reserve(dataPoints_.size());
    for (const auto& point : dataPoints_) {
        spikes.push_back(point.spikeEvents);
    }
    return spikes;
}

std::string PerformanceStatistics::generateReport() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    oss << "Performance Statistics Report\n";
    oss << "=============================\n\n";
    oss << "Data Points Collected: " << dataPoints_.size() << "\n";
    oss << "Time Span: " << getDurationSeconds() << " seconds\n\n";
    
    oss << "Performance Metrics:\n";
    oss << "-------------------\n";
    oss << "Average Steps/sec: " << getAverageStepsPerSecond() << "\n";
    oss << "Average Neuron Updates/sec: " << getAverageNeuronUpdatesPerSecond() << "\n";
    oss << "Average Spike Events/sec: " << getAverageSpikeEventsPerSecond() << "\n";
    oss << "Max Spike Events (single sample): " << getMaxSpikeEvents() << "\n";
    oss << "Average CPU Usage: " << getAverageCPUUsage() << "%\n";
    oss << "Average Memory Usage: " << getAverageMemoryUsage() / (1024 * 1024) << " MB\n\n";
    
    oss << "Cache Performance:\n";
    oss << "-------------------\n";
    oss << "L1 Cache Hit Rate: " << (getAverageL1CacheHitRate() * 100) << "%\n";
    oss << "L2 Cache Hit Rate: " << (getAverageL2CacheHitRate() * 100) << "%\n";
    oss << "L3 Cache Hit Rate: " << (getAverageL3CacheHitRate() * 100) << "%\n\n";
    
    // Add time series analysis
    oss << "Time Series Analysis:\n";
    oss << "--------------------\n";
    oss << "Simulation Steps: " << dataPoints_.size() << " data points\n";
    
    if (!dataPoints_.empty()) {
        double minTime = dataPoints_.front().simulationTime;
        double maxTime = dataPoints_.back().simulationTime;
        oss << "Time Range: " << minTime << " - " << maxTime << " seconds\n";
        
        if (maxTime > minTime) {
            double avgTime = (minTime + maxTime) / 2.0;
            oss << "Average Time: " << avgTime << " seconds\n";
        }
    }
    
    return oss.str();
}

void PerformanceStatistics::exportToCSV(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open CSV file for writing: " + filename);
        return;
    }
    
    file << "timestamp,simulation_step,simulation_time,neuron_updates,synapse_updates,spike_events,synaptic_weights,memory_usage,thread_pool_size,active_threads" << std::endl;
    
    for (const auto& point : dataPoints_) {
        file << point.timestamp << ","
             << point.simulationStep << ","
             << point.simulationTime << ","
             << point.neuronUpdates << ","
             << point.synapseUpdates << ","
             << point.spikeEvents << ","
             << point.synapticWeights << ","
             << point.memoryUsage << ","
             << point.threadPoolSize << ","
             << point.activeThreads << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Performance data exported to CSV: " + filename);
}

void PerformanceStatistics::updateStatistics(const PerformanceDataPoint& point) {
    if (!dataPoints_.empty()) {
        const auto& last = dataPoints_.back();
        double timeSinceLast = point.simulationTime - last.simulationTime;
        
        if (timeSinceLast > 0) {
            // Update rate statistics
            double stepsDelta = static_cast<double>(point.simulationStep - last.simulationStep);
            double neuronUpdatesDelta = static_cast<double>(point.neuronUpdates - last.neuronUpdates);
            double spikeEventsDelta = static_cast<double>(point.spikeEvents - last.spikeEvents);
            
            stepsPerSecond_ = stepsDelta / timeSinceLast;
            neuronUpdatesPerSecond_ = neuronUpdatesDelta / timeSinceLast;
            spikeEventsPerSecond_ = spikeEventsDelta / timeSinceLast;
        }
    }
}

double PerformanceStatistics::getDurationSeconds() const {
    if (dataPoints_.size() < 2) return 0.0;
    return dataPoints_.back().simulationTime - dataPoints_.front().simulationTime;
}

double PerformanceStatistics::getAverageL1CacheHitRate() const {
    double total = 0.0;
    size_t count = 0;
    for (const auto& point : dataPoints_) {
        if (point.l1CacheHits > 0.0f || point.l2CacheHits > 0.0f || point.l3CacheHits > 0.0f) {
            total += point.l1CacheHits;
            count++;
        }
    }
    return count > 0 ? total / count : 0.0;
}

double PerformanceStatistics::getAverageL2CacheHitRate() const {
    double total = 0.0;
    size_t count = 0;
    for (const auto& point : dataPoints_) {
        if (point.l1CacheHits > 0.0f || point.l2CacheHits > 0.0f || point.l3CacheHits > 0.0f) {
            total += point.l2CacheHits;
            count++;
        }
    }
    return count > 0 ? total / count : 0.0;
}

double PerformanceStatistics::getAverageL3CacheHitRate() const {
    double total = 0.0;
    size_t count = 0;
    for (const auto& point : dataPoints_) {
        if (point.l1CacheHits > 0.0f || point.l2CacheHits > 0.0f || point.l3CacheHits > 0.0f) {
            total += point.l3CacheHits;
            count++;
        }
    }
    return count > 0 ? total / count : 0.0;
}

// PerformanceMonitor implementation
PerformanceMonitor::PerformanceMonitor(const std::shared_ptr<Config>& config)
    : config_(config), timer_(), running_(false), samplingThread_(), samplingInterval_(SamplingInterval::MEDIUM),
      totalSteps_(0), totalNeuronUpdates_(0), totalSynapseUpdates_(0), totalSpikeEvents_(0) {
    
    if (config_) {
        std::string interval = config_->getOr<std::string>("performance_sampling_interval", "medium");
        if (interval == "high") {
            samplingInterval_ = SamplingInterval::HIGH;
        } else if (interval == "low") {
            samplingInterval_ = SamplingInterval::LOW;
        } else if (interval == "minimal") {
            samplingInterval_ = SamplingInterval::MINIMAL;
        }
    }
    
    // Initialize logger
    logger_ = spdlog::get("nlm");
    if (!logger_) {
        logger_ = spdlog::stdout_color_mt("nlm");
        logger_->set_level(spdlog::level::info);
    }
    
    // Get thread count from config if available
    threadCount_ = std::thread::hardware_concurrency();
    if (config_) {
        threadCount_ = config_->getOr<size_t>("thread_count", threadCount_);
    }
}

PerformanceMonitor::~PerformanceMonitor() {
    stop();
}

void PerformanceMonitor::start() {
    if (running_) {
        return;
    }
    
    running_ = true;
    samplingThread_ = std::thread(&PerformanceMonitor::samplingThread, this);
    
    NLM_LOG_INFO("Performance monitoring started");
}

void PerformanceMonitor::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    if (samplingThread_.joinable()) {
        samplingThread_.join();
    }
    
    NLM_LOG_INFO("Performance monitoring stopped");
}

void PerformanceMonitor::recordStep(uint64_t step, uint64_t neuronUpdates, uint64_t synapseUpdates, 
                                   uint64_t spikeEvents, uint64_t synapticWeights) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    totalSteps_ = step;
    totalNeuronUpdates_ += neuronUpdates;
    totalSynapseUpdates_ += synapseUpdates;
    totalSpikeEvents_ += spikeEvents;
    
    currentDataPoint_.simulationStep = step;
    currentDataPoint_.neuronUpdates = neuronUpdates;
    currentDataPoint_.synapseUpdates = synapseUpdates;
    currentDataPoint_.spikeEvents = spikeEvents;
    currentDataPoint_.synapticWeights = synapticWeights;
}

void PerformanceMonitor::updateSystemMetrics(double cpuUsage, uint64_t memoryUsage, 
                                           size_t threadPoolSize, size_t activeThreads,
                                           float l1CacheHits, float l2CacheHits, float l3CacheHits,
                                           double instructionsPerCycle) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    currentDataPoint_.cpuUsage = cpuUsage;
    currentDataPoint_.memoryUsage = memoryUsage;
    currentDataPoint_.threadPoolSize = threadPoolSize;
    currentDataPoint_.activeThreads = activeThreads;
    currentDataPoint_.l1CacheHits = l1CacheHits;
    currentDataPoint_.l2CacheHits = l2CacheHits;
    currentDataPoint_.l3CacheHits = l3CacheHits;
    currentDataPoint_.instructionsPerCycle = instructionsPerCycle;
}

double PerformanceMonitor::getCurrentStepsPerSecond() const {
    return stepsPerSecond_.load();
}

double PerformanceMonitor::getCurrentNeuronUpdatesPerSecond() const {
    return neuronUpdatesPerSecond_.load();
}

double PerformanceMonitor::getCurrentSpikeEventsPerSecond() const {
    return spikeEventsPerSecond_.load();
}

uint64_t PerformanceMonitor::getTotalSpikeEvents() const {
    return totalSpikeEvents_.load();
}

void PerformanceMonitor::exportReport(const std::string& filename) {
    statistics_.exportToCSV(filename);
    
    std::ofstream reportFile(filename + ".txt");
    if (reportFile.is_open()) {
        reportFile << statistics_.generateReport();
        reportFile.close();
        
        NLM_LOG_INFO("Performance report exported to: " + filename + ".txt");
    }
}

void PerformanceMonitor::samplingThread() {
    auto interval = getSamplingInterval();
    
    while (running_) {
        auto start = std::chrono::steady_clock::now();
        
        // Collect performance data
        collectPerformanceData();
        
        // Calculate real-time metrics
        updateRealTimeMetrics();
        
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        // Sleep for the remaining interval time
        auto sleepTime = interval - duration;
        if (sleepTime > std::chrono::microseconds(0)) {
            std::this_thread::sleep_for(sleepTime);
        }
    }
}

auto PerformanceMonitor::getSamplingInterval() const {
    switch (samplingInterval_) {
        case SamplingInterval::HIGH:
            return std::chrono::milliseconds(10);
        case SamplingInterval::MEDIUM:
            return std::chrono::milliseconds(100);
        case SamplingInterval::LOW:
            return std::chrono::seconds(1);
        case SamplingInterval::MINIMAL:
            return std::chrono::seconds(10);
        default:
            return std::chrono::milliseconds(100);
    }
}

void PerformanceMonitor::collectPerformanceData() {
    auto now = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    // Create data point
    PerformanceDataPoint point;
    point.timestamp = now.count();
    point.simulationTime = timer_.elapsedSeconds();
    point.neuronUpdates = currentDataPoint_.neuronUpdates;
    point.synapseUpdates = currentDataPoint_.synapseUpdates;
    point.spikeEvents = currentDataPoint_.spikeEvents;
    point.synapticWeights = currentDataPoint_.synapticWeights;
    point.cpuUsage = currentDataPoint_.cpuUsage;
    point.memoryUsage = currentDataPoint_.memoryUsage;
    point.threadPoolSize = currentDataPoint_.threadPoolSize;
    point.activeThreads = currentDataPoint_.activeThreads;
    point.l1CacheHits = currentDataPoint_.l1CacheHits;
    point.l2CacheHits = currentDataPoint_.l2CacheHits;
    point.l3CacheHits = currentDataPoint_.l3CacheHits;
    point.instructionsPerCycle = currentDataPoint_.instructionsPerCycle;
    
    // Add to statistics
    statistics_.addDataPoint(point);
}

void PerformanceMonitor::updateRealTimeMetrics() {
    // Calculate real-time metrics based on changes since last update
    static uint64_t lastStep = 0;
    static double lastSimulationTime = 0.0;
    static uint64_t lastNeuronUpdates = 0;
    static uint64_t lastSynapseUpdates = 0;
    static uint64_t lastSpikeEvents = 0;
    
    double simulationTime = timer_.elapsedSeconds();
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    uint64_t currentStep = currentDataPoint_.simulationStep;
    uint64_t currentNeuronUpdates = currentDataPoint_.neuronUpdates;
    uint64_t currentSynapseUpdates = currentDataPoint_.synapseUpdates;
    uint64_t currentSpikeEvents = currentDataPoint_.spikeEvents;
    
    if (simulationTime > lastSimulationTime && currentStep > lastStep) {
        double timeDelta = simulationTime - lastSimulationTime;
        uint64_t stepDelta = currentStep - lastStep;
        uint64_t neuronDelta = currentNeuronUpdates - lastNeuronUpdates;
        uint64_t synapseDelta = currentSynapseUpdates - lastSynapseUpdates;
        uint64_t spikeDelta = currentSpikeEvents - lastSpikeEvents;
        
        if (timeDelta > 0) {
            stepsPerSecond_.store(static_cast<double>(stepDelta) / timeDelta);
            neuronUpdatesPerSecond_.store(static_cast<double>(neuronDelta) / timeDelta);
            spikeEventsPerSecond_.store(static_cast<double>(spikeDelta) / timeDelta);
        }
    }
    
    // Update last values
    lastStep = currentStep;
    lastSimulationTime = simulationTime;
    lastNeuronUpdates = currentNeuronUpdates;
    lastSynapseUpdates = currentSynapseUpdates;
    lastSpikeEvents = currentSpikeEvents;
}

// PerformanceBenchmark implementation
void PerformanceBenchmark::printExtendedBenchmarkResults(const std::vector<ExtendedBenchmarkResult>& results) {
    std::cout << std::endl;
    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                        Enhanced NLM Performance Benchmark Results                       ║" << std::endl;
    std::cout << "╠══════════════╦══════════════╦═══════════╦═════════════╦════════════════════╦════════════════════╗" << std::endl;
    std::cout << "║    Neurons   ║    Synapses  ║ Steps/sec  ║ Spikes/step ║  Peak CPU%    ║  Peak Memory(MB)  ║" << std::endl;
    std::cout << "╠══════════════╬══════════════╬═══════════╬═════════════╬════════════════════╬════════════════════╗" << std::endl;
    
    for (const auto& r : results) {
        std::cout << "║ " << std::setw(13) << r.neurons 
                  << " ║ " << std::setw(13) << r.synapses
                  << " ║ " << std::setw(11) << std::fixed << std::setprecision(0) << r.stepsPerSecond
                  << " ║ " << std::setw(13) << std::fixed << std::setprecision(2) << r.spikesPerStep
                  << " ║ " << std::setw(16) << std::fixed << std::setprecision(1) << r.peakCPUUsage
                  << " ║ " << std::setw(20) << std::fixed << std::setprecision(1) << r.peakMemoryUsage / (1024 * 1024)
                  << " ║" << std::endl;
    }
    
    std::cout << "╚══════════════╩══════════════╩═══════════╩═════════════╩════════════════════╩════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    // Print cache performance if available
    bool hasCacheData = false;
    for (const auto& r : results) {
        if (r.averageL1CacheHitRate > 0.0) {
            hasCacheData = true;
            break;
        }
    }
    
    if (hasCacheData) {
        std::cout << "Cache Performance:" << std::endl;
        std::cout << "------------------" << std::endl;
        std::cout << "Average L1 Cache Hit Rate: " << std::fixed << std::setprecision(1) 
                  << (results[0].averageL1CacheHitRate * 100) << "%" << std::endl;
        std::cout << "Average L2 Cache Hit Rate: " << std::fixed << std::setprecision(1) 
                  << (results[0].averageL2CacheHitRate * 100) << "%" << std::endl;
        std::cout << "Average L3 Cache Hit Rate: " << std::fixed << std::setprecision(1) 
                  << (results[0].averageL3CacheHitRate * 100) << "%" << std::endl;
        std::cout << std::endl;
    }
}

} // namespace nlm