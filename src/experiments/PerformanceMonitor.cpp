// Performance Monitoring Tool
// Utility for monitoring NLM brain simulation performance

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>
#include <cmath>
#include <algorithm>

// Performance metrics collection
struct PerformanceMetrics {
    uint64_t totalSteps = 0;
    uint64_t totalSpikes = 0;
    uint64_t totalTimeSteps = 0;
    float avgFiringRate = 0.0f;
    float avgSynapticWeight = 0.0f;
    float totalReward = 0.0f;
    float maxFiringRate = 0.0f;
    float minFiringRate = 0.0f;
    uint64_t startTime = 0;
    uint64_t endTime = 0;
    std::chrono::high_resolution_clock::time_point wallStart;
    std::chrono::high_resolution_clock::time_point wallEnd;
    
    PerformanceMetrics() {
        wallStart = std::chrono::high_resolution_clock::now();
    }
    
    void recordStep(uint64_t steps, uint64_t spikes) {
        totalSteps += steps;
        totalSpikes += spikes;
    }
    
    void updateMetrics(const nlm::Brain& brain, float reward) {
        totalTimeSteps++;
        avgFiringRate = (avgFiringRate * (totalTimeSteps - 1) + brain.getAverageFiringRate()) / totalTimeSteps;
        maxFiringRate = std::max(maxFiringRate, brain.getAverageFiringRate());
        minFiringRate = std::min(minFiringRate, brain.getAverageFiringRate());
        totalReward += reward;
    }
    
    void endTiming() {
        wallEnd = std::chrono::high_resolution_clock::now();
        endTime = time(nullptr);
    }
    
    double getWallClockTime() const {
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(wallEnd - wallStart);
        return duration.count();
    }
    
    double getStepsPerSecond() const {
        if (totalSteps == 0) return 0.0;
        double wallTime = getWallClockTime();
        return static_cast<double>(totalSteps) / wallTime;
    }
    
    double getSpikesPerSecond() const {
        if (totalSteps == 0) return 0.0;
        double wallTime = getWallClockTime();
        return static_cast<double>(totalSpikes) / wallTime;
    }
    
    double getFiringRate() const {
        return avgFiringRate;
    }
    
    std::string getMetricsSummary() const {
        std::ostringstream oss;
        oss << "Performance Summary:" << std::endl;
        oss << "  Simulation Steps: " << totalSteps << std::endl;
        oss << "  Total Spikes: " << totalSpikes << std::endl;
        oss << "  Average Firing Rate: " << std::fixed << std::setprecision(2) << avgFiringRate << " Hz" << std::endl;
        oss << "  Max Firing Rate: " << std::fixed << std::setprecision(2) << maxFiringRate << " Hz" << std::endl;
        oss << "  Min Firing Rate: " << std::fixed << std::setprecision(2) << minFiringRate << " Hz" << std::endl;
        oss << "  Steps per Second: " << std::fixed << std::setprecision(2) << getStepsPerSecond() << std::endl;
        oss << "  Spikes per Second: " << std::fixed << std::setprecision(2) << getSpikesPerSecond() << std::endl;
        oss << "  Total Reward: " << std::fixed << std::setprecision(4) << totalReward << std::endl;
        oss << "  Wall Clock Time: " << std::fixed << std::setprecision(2) << getWallClockTime() << " seconds" << std::endl;
        return oss.str();
    }
};

// Resource monitoring
class ResourceMonitor {
public:
    struct ResourceUsage {
        float cpuUsage = 0.0f;
        float memoryUsageMB = 0.0f;
        float memoryUsagePercent = 0.0f;
        uint64_t timestamp = 0;
    };
    
    ResourceMonitor() : running_(false), monitoringThread_() {}
    ~ResourceMonitor() {
        stop();
    }
    
    void startMonitoring() {
        running_ = true;
        monitoringThread_ = std::thread(&ResourceMonitor::monitorLoop, this);
    }
    
    void stop() {
        running_ = false;
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
    }
    
    const std::vector<ResourceUsage>& getResourceHistory() const {
        return resourceHistory_;
    }
    
    void printResourceSummary() const {
        if (resourceHistory_.empty()) {
            std::cout << "No resource data collected" << std::endl;
            return;
        }
        
        std::cout << "Resource Usage Summary:" << std::endl;
        std::cout << "  Samples collected: " << resourceHistory_.size() << std::endl;
        
        float maxCpu = 0.0f, maxMemory = 0.0f;
        float avgCpu = 0.0f, avgMemory = 0.0f;
        
        for (const auto& usage : resourceHistory_) {
            maxCpu = std::max(maxCpu, usage.cpuUsage);
            maxMemory = std::max(maxMemory, usage.memoryUsageMB);
            avgCpu += usage.cpuUsage;
            avgMemory += usage.memoryUsageMB;
        }
        
        avgCpu /= resourceHistory_.size();
        avgMemory /= resourceHistory_.size();
        
        std::cout << "  Average CPU Usage: " << std::fixed << std::setprecision(2) << avgCpu << "%" << std::endl;
        std::cout << "  Max CPU Usage: " << std::fixed << std::setprecision(2) << maxCpu << "%" << std::endl;
        std::cout << "  Average Memory Usage: " << std::fixed << std::setprecision(2) << avgMemory << " MB" << std::endl;
        std::cout << "  Max Memory Usage: " << std::fixed << std::setprecision(2) << maxMemory << " MB" << std::endl;
    }
    
private:
    void monitorLoop() {
        while (running_) {
            ResourceUsage usage;
            usage.timestamp = time(nullptr);
            
            // Simplified resource monitoring - in a real implementation,
            // we would use platform-specific APIs to get actual CPU and memory usage
            usage.cpuUsage = getSimulatedCpuUsage();
            usage.memoryUsageMB = getSimulatedMemoryUsageMB();
            usage.memoryUsagePercent = getSimulatedMemoryUsagePercent();
            
            std::lock_guard<std::mutex> lock(mutex_);
            resourceHistory_.push_back(usage);
            
            // Keep only recent samples (last 1000)
            if (resourceHistory_.size() > 1000) {
                resourceHistory_.erase(resourceHistory_.begin());
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    float getSimulatedCpuUsage() {
        // Simulate CPU usage based on activity
        // In a real implementation, we would use psutil (Linux), GetSystemTimes (Windows), or similar
        return 50.0f + std::rand() % 30; // Simulate 50-80% CPU usage
    }
    
    float getSimulatedMemoryUsageMB() {
        // Simulate memory usage
        // In a real implementation, we would use platform-specific APIs
        return 500.0f + std::rand() % 500; // Simulate 500-1000 MB memory usage
    }
    
    float getSimulatedMemoryUsagePercent() {
        // Simulate memory usage percentage
        return 30.0f + std::rand() % 40; // Simulate 30-70% memory usage
    }
    
    bool running_;
    std::thread monitoringThread_;
    std::vector<ResourceUsage> resourceHistory_;
    std::mutex mutex_;
};

// Simulation benchmark
class SimulationBenchmark {
public:
    struct BenchmarkResult {
        uint64_t neuronCount;
        uint64_t regionCount;
        uint64_t connectionProbability;
        float simulationTimestep;
        uint64_t maxSteps;
        double wallClockTime;
        double stepsPerSecond;
        float finalFiringRate;
        float totalReward;
        std::string benchmarkName;
    };
    
    BenchmarkResult runBenchmark(const std::string& configFile, const std::string& benchmarkName = "benchmark") {
        std::cout << "Running benchmark: " << benchmarkName << std::endl;
        std::cout << "Config file: " << configFile << std::endl;
        
        // Initialize brain with configuration
        auto config = std::make_shared<nlm::Config>();
        
        if (!config->loadFromFile(configFile)) {
            std::cerr << "ERROR: Failed to load configuration file" << std::endl;
            throw std::runtime_error("Failed to load configuration file");
        }
        
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain" << std::endl;
            throw std::runtime_error("Failed to initialize brain");
        }
        
        // Run simulation with performance monitoring
        PerformanceMetrics metrics;
        uint64_t steps = config->getOr<uint64_t>("max_steps", 1000);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (uint64_t step = 0; step < steps; ++step) {
            // Simple activity simulation
            brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
            
            // Record metrics every 100 steps
            if (step % 100 == 0) {
                metrics.recordStep(100, brain->getTotalSpikeCount());
                metrics.updateMetrics(*brain, 0.0f); // No reward for benchmark
            }
            
            // Progress indicator
            if (step % 1000 == 0) {
                std::cout << "  Progress: " << (step * 100.0 / steps) << "%" << std::endl;
            }
        }
        
        metrics.endTiming();
        
        // Get final metrics
        BenchmarkResult result;
        result.neuronCount = config->getOr<uint64_t>("neuron_count", 1000);
        result.regionCount = config->getOr<uint64_t>("region_count", 1);
        result.connectionProbability = config->getOr<uint64_t>("connection_probability", 10);
        result.simulationTimestep = config->getOr<double>("simulation_timestep", 0.001);
        result.maxSteps = steps;
        result.wallClockTime = metrics.getWallClockTime();
        result.stepsPerSecond = metrics.getStepsPerSecond();
        result.finalFiringRate = brain->getAverageFiringRate();
        result.totalReward = metrics.totalReward;
        result.benchmarkName = benchmarkName;
        
        std::cout << "Benchmark completed successfully!" << std::endl;
        return result;
    }
    
    void printBenchmarkResults(const BenchmarkResult& result) {
        std::cout << "\nBenchmark Results:" << std::endl;
        std::cout << "  Name: " << result.benchmarkName << std::endl;
        std::cout << "  Configuration:" << std::endl;
        std::cout << "    Neurons: " << result.neuronCount << std::endl;
        std::cout << "    Regions: " << result.regionCount << std::endl;
        std::cout << "    Connection Probability: " << (result.connectionProbability / 100.0f) << std::endl;
        std::cout << "    Simulation Timestep: " << result.simulationTimestep << "s" << std::endl;
        std::cout << "    Max Steps: " << result.maxSteps << std::endl;
        std::cout << "  Performance:" << std::endl;
        std::cout << "    Wall Clock Time: " << std::fixed << std::setprecision(2) << result.wallClockTime << " seconds" << std::endl;
        std::cout << "    Steps per Second: " << std::fixed << std::setprecision(2) << result.stepsPerSecond << std::endl;
        std::cout << "    Final Firing Rate: " << std::fixed << std::setprecision(2) << result.finalFiringRate << " Hz" << std::endl;
        std::cout << "    Total Reward: " << std::fixed << std::setprecision(4) << result.totalReward << std::endl;
    }
    
    void compareBenchmarks(const std::vector<BenchmarkResult>& results) {
        if (results.size() < 2) {
            std::cout << "Need at least 2 benchmarks for comparison" << std::endl;
            return;
        }
        
        std::cout << "\nBenchmark Comparison:" << std::endl;
        std::cout << "===================" << std::endl;
        
        for (const auto& result : results) {
            printBenchmarkResults(result);
            std::cout << std::endl;
        }
    }
};

// Usage information
void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --benchmark <name> <config_file>  Run a performance benchmark" << std::endl;
    std::cout << "  --monitor <config_file>           Run simulation with resource monitoring" << std::endl;
    std::cout << "  --help                            Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --benchmark " << "fast" << " configs/default.cfg" << std::endl;
    std::cout << "  " << programName << " --benchmark " << "detailed" << " configs/default.cfg" << std::endl;
    std::cout << "  " << programName << " --monitor configs/default.cfg" << std::endl;
}

int main(int argc, char* argv[]) {
    // Set program name for usage message
    const char* programName = "nlm_perf_monitor";
    
    // Check for help flag
    if (argc == 2 && std::string(argv[1]) == "--help") {
        printUsage(programName);
        return 0;
    }
    
    // Parse command line arguments
    std::string benchmarkName;
    std::string configFile;
    bool hasBenchmark = false;
    bool hasMonitor = false;
    
    // Look for benchmark option
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--benchmark" && i + 2 < argc) {
            benchmarkName = argv[++i];
            configFile = argv[++i];
            hasBenchmark = true;
        } else if (arg == "--monitor" && i + 1 < argc) {
            configFile = argv[++i];
            hasMonitor = true;
        } else if (arg == "--help") {
            printUsage(programName);
            return 0;
        } else {
            std::cerr << "ERROR: Unknown argument: " << arg << std::endl;
            printUsage(programName);
            return 1;
        }
    }
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    logger->setLevel(LogLevel::Info);
    Logger::setGlobal(logger);
    
    try {
        if (hasBenchmark) {
            SimulationBenchmark benchmark;
            auto result = benchmark.runBenchmark(configFile, benchmarkName);
            benchmark.printBenchmarkResults(result);
        } else if (hasMonitor) {
            std::cout << "Starting simulation with resource monitoring..." << std::endl;
            
            // Initialize brain with configuration
            auto config = std::make_shared<nlm::Config>();
            if (!config->loadFromFile(configFile)) {
                std::cerr << "ERROR: Failed to load configuration file" << std::endl;
                return 1;
            }
            
            auto brain = std::make_shared<nlm::Brain>(config);
            if (!brain->initialize()) {
                std::cerr << "ERROR: Failed to initialize brain" << std::endl;
                return 1;
            }
            
            // Start resource monitoring
            ResourceMonitor monitor;
            monitor.startMonitoring();
            
            // Run simulation
            uint64_t steps = config->getOr<uint64_t>("max_steps", 1000);
            
            for (uint64_t step = 0; step < steps; ++step) {
                brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
                
                if (step % 100 == 0) {
                    std::cout << "  Step " << step << "/" << steps 
                             << " (Firing rate: " << std::fixed << std::setprecision(2) 
                             << brain->getAverageFiringRate() << " Hz)" << std::endl;
                }
                
                if (step % 500 == 0) {
                    monitor.printResourceSummary();
                }
            }
            
            // Stop monitoring and show results
            monitor.stop();
            monitor.printResourceSummary();
            
            std::cout << "Simulation completed successfully!" << std::endl;
        } else {
            std::cerr << "ERROR: No action specified" << std::endl;
            printUsage(programName);
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\nPerformance monitoring completed successfully!" << std::endl;
    return 0;
}