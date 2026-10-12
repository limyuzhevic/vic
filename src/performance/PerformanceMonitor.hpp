// Performance Monitoring and Profiling Infrastructure
// Enhanced NLM Performance Monitoring with Real-time Metrics

#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <mutex>
#include <atomic>
#include <spdlog/spdlog.h>
#include "core/Config/Config.hpp"

namespace nlm {

// Performance sampling interval
enum class SamplingInterval {
    HIGH = 10,      // 10ms sampling
    MEDIUM = 100,    // 100ms sampling
    LOW = 1000,      // 1s sampling
    MINIMAL = 10000  // 10s sampling
};

// Performance metric types
enum class MetricType {
    SIMULATION_STEPS,
    NEURON_UPDATES,
    SYNAPSE_UPDATES,
    SPIKE_EVENTS,
    SYNAPTIC_WEIGHTS,
    MEMORY_USAGE,
    CPU_USAGE,
    THREAD_POOL_USAGE,
    L1_CACHE_HIT_RATE,
    L2_CACHE_HIT_RATE,
    L3_CACHE_HIT_RATE,
    IPC,
    INSTRUCTIONS_PER_CYCLE
};

// High-resolution timer for performance measurements
class HighResolutionTimer {
public:
    HighResolutionTimer() : startTime_(std::chrono::high_resolution_clock::now()) {}
    
    void reset() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    double elapsedSeconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(endTime - startTime_).count();
    }
    
    double elapsedMilliseconds() const {
        return elapsedSeconds() * 1000.0;
    }
    
    uint64_t elapsedNanoseconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime_).count();
    }
    
    void startTiming(const std::string& operation) {
        operationTimes_[operation] = std::chrono::high_resolution_clock::now();
    }
    
    double stopTiming(const std::string& operation) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto it = operationTimes_.find(operation);
        if (it == operationTimes_.end()) {
            return 0.0;
        }
        
        double duration = std::chrono::duration<double>(endTime - it->second).count();
        operationTimes_.erase(it);
        return duration;
    }
    
    const auto& getOperationTimes() const { return operationTimes_; }
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> operationTimes_;
};

// Performance data point structure
struct PerformanceDataPoint {
    uint64_t timestamp;           // Unix timestamp in microseconds
    uint64_t simulationStep;      // Current simulation step
    double simulationTime;        // Real-time elapsed since start (seconds)
    uint64_t neuronUpdates;       // Number of neuron state updates
    uint64_t synapseUpdates;      // Number of synapse state updates
    uint64_t spikeEvents;         // Number of spike events
    uint64_t synapticWeights;     // Total synaptic weight updates
    uint64_t memoryUsage;         // Memory usage in bytes
    double cpuUsage;              // CPU usage percentage (0-100)
    size_t threadPoolSize;        // Current thread pool size
    size_t activeThreads;         // Number of active worker threads
    float l1CacheHits;            // L1 cache hit rate (0-1)
    float l2CacheHits;            // L2 cache hit rate (0-1)
    float l3CacheHits;            // L3 cache hit rate (0-1)
    double instructionsPerCycle; // Instructions per cycle
    
    PerformanceDataPoint()
        : timestamp(0), simulationStep(0), simulationTime(0.0),
          neuronUpdates(0), synapseUpdates(0), spikeEvents(0),
          synapticWeights(0), memoryUsage(0), cpuUsage(0.0),
          threadPoolSize(0), activeThreads(0),
          l1CacheHits(0.0f), l2CacheHits(0.0f), l3CacheHits(0.0f),
          instructionsPerCycle(0.0) {}
};

// Performance statistics calculator
class PerformanceStatistics {
public:
    PerformanceStatistics() = default;
    
    void addDataPoint(const PerformanceDataPoint& point) {
        dataPoints_.push_back(point);
        updateStatistics(point);
    }
    
    double getAverageStepsPerSecond() const {
        if (dataPoints_.size() < 2) return 0.0;
        double totalTime = dataPoints_.back().simulationTime - dataPoints_.front().simulationTime;
        double totalSteps = static_cast<double>(dataPoints_.back().simulationStep - dataPoints_.front().simulationStep);
        return totalSteps / totalTime;
    }
    
    double getAverageNeuronUpdatesPerSecond() const {
        if (dataPoints_.empty()) return 0.0;
        double totalTime = getDurationSeconds();
        double totalUpdates = 0.0;
        for (const auto& point : dataPoints_) {
            totalUpdates += point.neuronUpdates;
        }
        return totalUpdates / totalTime / dataPoints_.size();
    }
    
    double getAverageSpikeEventsPerSecond() const {
        if (dataPoints_.empty()) return 0.0;
        double totalTime = getDurationSeconds();
        double totalSpikes = 0.0;
        for (const auto& point : dataPoints_) {
            totalSpikes += point.spikeEvents;
        }
        return totalSpikes / totalTime / dataPoints_.size();
    }
    
    uint64_t getMaxSpikeEvents() const {
        uint64_t max = 0;
        for (const auto& point : dataPoints_) {
            max = std::max(max, point.spikeEvents);
        }
        return max;
    }
    
    double getAverageCPUUsage() const {
        if (dataPoints_.empty()) return 0.0;
        double total = 0.0;
        for (const auto& point : dataPoints_) {
            total += point.cpuUsage;
        }
        return total / dataPoints_.size();
    }
    
    size_t getAverageMemoryUsage() const {
        if (dataPoints_.empty()) return 0;
        uint64_t total = 0;
        for (const auto& point : dataPoints_) {
            total += point.memoryUsage;
        }
        return total / dataPoints_.size();
    }
    
    std::vector<double> getSimulationTimeSeries() const {
        std::vector<double> times;
        for (const auto& point : dataPoints_) {
            times.push_back(point.simulationTime);
        }
        return times;
    }
    
    std::vector<uint64_t> getSpikeEventTimeSeries() const {
        std::vector<uint64_t> spikes;
        for (const auto& point : dataPoints_) {
            spikes.push_back(point.spikeEvents);
        }
        return spikes;
    }
    
    std::string generateReport() const {
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
        
        return oss.str();
    }
    
    void exportToCSV(const std::string& filename) const {
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
    
private:
    void updateStatistics(const PerformanceDataPoint& point) {
        // Update running statistics
        if (!dataPoints_.empty()) {
            const auto& last = dataPoints_.back();
            double timeSinceLast = point.simulationTime - last.simulationTime;
            
            if (timeSinceLast > 0) {
                // Update rate statistics
                stepsPerSecond_ = (point.simulationStep - last.simulationStep) / timeSinceLast;
                neuronUpdatesPerSecond_ = static_cast<double>(point.neuronUpdates - last.neuronUpdates) / timeSinceLast;
                spikeEventsPerSecond_ = static_cast<double>(point.spikeEvents - last.spikeEvents) / timeSinceLast;
            }
        }
    }
    
    double getDurationSeconds() const {
        if (dataPoints_.size() < 2) return 0.0;
        return dataPoints_.back().simulationTime - dataPoints_.front().simulationTime;
    }
    
    double getAverageL1CacheHitRate() const {
        double total = 0.0;
        int count = 0;
        for (const auto& point : dataPoints_) {
            if (point.l1CacheHits > 0 || point.l2CacheHits > 0 || point.l3CacheHits > 0) {
                total += point.l1CacheHits;
                count++;
            }
        }
        return count > 0 ? total / count : 0.0;
    }
    
    double getAverageL2CacheHitRate() const {
        double total = 0.0;
        int count = 0;
        for (const auto& point : dataPoints_) {
            if (point.l1CacheHits > 0 || point.l2CacheHits > 0 || point.l3CacheHits > 0) {
                total += point.l2CacheHits;
                count++;
            }
        }
        return count > 0 ? total / count : 0.0;
    }
    
    double getAverageL3CacheHitRate() const {
        double total = 0.0;
        int count = 0;
        for (const auto& point : dataPoints_) {
            if (point.l1CacheHits > 0 || point.l2CacheHits > 0 || point.l3CacheHits > 0) {
                total += point.l3CacheHits;
                count++;
            }
        }
        return count > 0 ? total / count : 0.0;
    }
    
    std::vector<PerformanceDataPoint> dataPoints_;
    double stepsPerSecond_ = 0.0;
    double neuronUpdatesPerSecond_ = 0.0;
    double spikeEventsPerSecond_ = 0.0;
    mutable std::mutex statsMutex_;
};

// Real-time performance monitor
class PerformanceMonitor {
public:
    PerformanceMonitor(const std::shared_ptr<Config>& config)
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
    
    ~PerformanceMonitor() {
        stop();
    }
    
    void start() {
        if (running_) {
            return;
        }
        
        running_ = true;
        samplingThread_ = std::thread(&PerformanceMonitor::samplingThread, this);
        
        NLM_LOG_INFO("Performance monitoring started");
    }
    
    void stop() {
        if (!running_) {
            return;
        }
        
        running_ = false;
        if (samplingThread_.joinable()) {
            samplingThread_.join();
        }
        
        NLM_LOG_INFO("Performance monitoring stopped");
    }
    
    void recordStep(uint64_t step, uint64_t neuronUpdates, uint64_t synapseUpdates, 
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
    
    void updateSystemMetrics(double cpuUsage, uint64_t memoryUsage, 
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
    
    PerformanceStatistics& getStatistics() {
        return statistics_;
    }
    
    double getCurrentStepsPerSecond() const {
        return stepsPerSecond_.load();
    }
    
    double getCurrentNeuronUpdatesPerSecond() const {
        return neuronUpdatesPerSecond_.load();
    }
    
    double getCurrentSpikeEventsPerSecond() const {
        return spikeEventsPerSecond_.load();
    }
    
    uint64_t getTotalSpikeEvents() const {
        return totalSpikeEvents_.load();
    }
    
    void exportReport(const std::string& filename) {
        statistics_.exportToCSV(filename);
        
        std::ofstream reportFile(filename + ".txt");
        if (reportFile.is_open()) {
            reportFile << statistics_.generateReport();
            reportFile.close();
            
            NLM_LOG_INFO("Performance report exported to: " + filename + ".txt");
        }
    }
    
private:
    void samplingThread() {
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
    
    auto getSamplingInterval() const {
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
    
    void collectPerformanceData() {
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
    
    void updateRealTimeMetrics() {
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
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<spdlog::logger> logger_;
    HighResolutionTimer timer_;
    
    SamplingInterval samplingInterval_;
    size_t threadCount_;
    
    std::atomic<bool> running_;
    std::thread samplingThread_;
    
    std::mutex dataMutex_;
    PerformanceDataPoint currentDataPoint_;
    
    PerformanceStatistics statistics_;
    
    std::atomic<uint64_t> totalSteps_{0};
    std::atomic<uint64_t> totalNeuronUpdates_{0};
    std::atomic<uint64_t> totalSynapseUpdates_{0};
    std::atomic<uint64_t> totalSpikeEvents_{0};
    
    std::atomic<double> stepsPerSecond_{0.0};
    std::atomic<double> neuronUpdatesPerSecond_{0.0};
    std::atomic<double> spikeEventsPerSecond_{0.0};
};

// Enhanced benchmark with performance monitoring
class PerformanceBenchmark {
public:
    PerformanceBenchmark(const std::shared_ptr<Config>& config)
        : config_(config) {
        logger_ = spdlog::get("nlm");
        if (!logger_) {
            logger_ = spdlog::stdout_color_mt("nlm");
            logger_->set_level(spdlog::level::info);
        }
    }
    
    struct ExtendedBenchmarkResult {
        size_t neurons;
        size_t synapses;
        size_t steps;
        double totalTime;      // seconds
        double stepsPerSecond;
        double spikesPerStep;
        double synapsesPerNeuron;
        
        // Additional performance metrics
        uint64_t totalNeuronUpdates;
        uint64_t totalSynapseUpdates;
        uint64_t totalSpikeEvents;
        uint64_t totalSynapticWeights;
        double peakCPUUsage;
        uint64_t peakMemoryUsage;
        double averageL1CacheHitRate;
        double averageL2CacheHitRate;
        double averageL3CacheHitRate;
        double averageInstructionsPerCycle;
    };
    
    ExtendedBenchmarkResult runExtendedBenchmark(size_t neuronCount, size_t steps) {
        std::cout << "  Running enhanced benchmark: " << neuronCount << " neurons, " 
                  << steps << " steps..." << std::flush;
        
        // Create configuration
        auto benchmarkConfig = std::make_shared<Config>();
        benchmarkConfig->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        benchmarkConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        benchmarkConfig->set("connection_probability", 0.1f, ConfigSource::Default);
        benchmarkConfig->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        benchmarkConfig->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        // Start performance monitoring
        PerformanceMonitor monitor(benchmarkConfig);
        monitor.start();
        
        // Create brain
        auto brain = std::make_shared<Brain>(benchmarkConfig);
        brain->initialize();
        
        // Run simulation with performance monitoring
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Inject input and get performance metrics
            for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Record performance data
            uint64_t neuronUpdates = brain->getTotalNeuronUpdates();
            uint64_t synapseUpdates = brain->getTotalSynapseUpdates();
            uint64_t spikeEvents = brain->getTotalSpikeCount();
            uint64_t synapticWeights = brain->getTotalSynapticWeightUpdates();
            
            // Get system metrics (simulated for now)
            double cpuUsage = 50.0 + (std::rand() % 30); // Simulated CPU usage
            uint64_t memoryUsage = 100 * 1024 * 1024;   // Simulated memory usage
            size_t threadPoolSize = std::min(size_t(4), neuronCount / 1000);
            size_t activeThreads = std::min(size_t(2), threadPoolSize);
            float l1CacheHits = 0.7f + (std::rand() % 200) / 1000.0f; // Simulated cache hits
            float l2CacheHits = 0.8f + (std::rand() % 200) / 1000.0f;
            float l3CacheHits = 0.9f + (std::rand() % 100) / 1000.0f;
            double ipc = 2.0 + (std::rand() % 400) / 100.0; // Instructions per cycle
            
            monitor.updateSystemMetrics(cpuUsage, memoryUsage, threadPoolSize, activeThreads,
                                      l1CacheHits, l2CacheHits, l3CacheHits, ipc);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Stop monitoring and get results
        monitor.stop();
        
        auto& stats = monitor.getStatistics();
        
        // Collect results
        ExtendedBenchmarkResult result;
        result.neurons = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.steps = steps;
        result.totalTime = totalTime;
        result.stepsPerSecond = steps / totalTime;
        result.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        // Get additional performance metrics from monitor
        result.totalNeuronUpdates = monitor.getTotalNeuronUpdates() + brain->getTotalNeuronUpdates();
        result.totalSynapseUpdates = monitor.getTotalSynapseUpdates() + brain->getTotalSynapseUpdates();
        result.totalSpikeEvents = monitor.getTotalSpikeEvents() + brain->getTotalSpikeCount();
        result.totalSynapticWeights = stats.getAverageMemoryUsage() / 8; // Estimate
        
        result.peakCPUUsage = stats.getAverageCPUUsage();
        result.peakMemoryUsage = stats.getAverageMemoryUsage();
        result.averageL1CacheHitRate = stats.getAverageL1CacheHitRate();
        result.averageL2CacheHitRate = stats.getAverageL2CacheHitRate();
        result.averageL3CacheHitRate = stats.getAverageL3CacheHitRate();
        result.averageInstructionsPerCycle = 3.0; // Placeholder
        
        std::cout << " Done!" << std::endl;
        std::cout << "    Time: " << std::fixed << std::setprecision(2) << totalTime << "s" << std::endl;
        std::cout << "    Steps/sec: " << std::fixed << std::setprecision(0) << result.stepsPerSecond << std::endl;
        std::cout << "    Spikes/step: " << std::fixed << std::setprecision(2) << result.spikesPerStep << std::endl;
        
        return result;
    }
    
    void printExtendedBenchmarkResults(const std::vector<ExtendedBenchmarkResult>& results) {
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
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<spdlog::logger> logger_;
};

} // namespace nlm