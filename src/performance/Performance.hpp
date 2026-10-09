#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <functional>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

// Performance measurement and profiling tools
// Phase 6: Advanced profiling for optimization and analysis

class Timer {
public:
    Timer() : startTime(), elapsedTime(0.0), running(false) {}
    
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
        running = true;
    }
    
    void stop() {
        if (running) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            elapsedTime += duration.count() / 1000.0; // Convert to milliseconds
            running = false;
        }
    }
    
    void reset() {
        elapsedTime = 0.0;
        running = false;
    }
    
    double getElapsedTime() const { return elapsedTime; }
    bool isRunning() const { return running; }
    
    // RAII-style timing
    class ScopedTimer {
    public:
        explicit ScopedTimer(Timer& timer) : timer(timer) {
            timer.start();
        }
        
        ~ScopedTimer() {
            timer.stop();
        }
        
        double getElapsedTime() const {
            return timer.getElapsedTime();
        }
        
    private:
        Timer& timer;
    };
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    double elapsedTime;
    bool running;
};

class Profiler {
public:
    struct ProfileData {
        std::string name;
        double totalTime;
        double minTime;
        double maxTime;
        size_t callCount;
        double averageTime;
        
        ProfileData() : totalTime(0), minTime(0), maxTime(0), callCount(0), averageTime(0) {}
    };
    
    using ProfileCallback = std::function<void(const ProfileData&)>;
    
public:
    Profiler() = default;
    
    // Start profiling a section
    void startProfile(const std::string& name);
    
    // End profiling a section
    void stopProfile(const std::string& name);
    
    // Get profile data
    const ProfileData* getProfile(const std::string& name) const;
    const std::map<std::string, ProfileData>& getAllProfiles() const;
    
    // Clear all profile data
    void clear();
    
    // Get profiling statistics
    double getTotalTime() const;
    size_t getProfileCount() const;
    
    // Export profiling data
    std::string exportProfileData() const;
    void exportProfileToFile(const std::string& filepath) const;
    
    // Register callback for profile updates
    void registerCallback(ProfileCallback callback);
    
    // Get current active profiles
    std::vector<std::string> getActiveProfiles() const;
    
    // Merge with another profiler
    void merge(const Profiler& other);
    
    // Reset and start new profiling session
    void resetSession();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class PerformanceAnalyzer {
public:
    struct BenchmarkResult {
        std::string testName;
        double executionTime;
        double memoryUsage;
        size_t operationsPerSecond;
        std::map<std::string, double> metrics;
        
        BenchmarkResult() : executionTime(0), memoryUsage(0), operationsPerSecond(0) {}
        BenchmarkResult(const std::string& name) : testName(name), executionTime(0), memoryUsage(0), operationsPerSecond(0) {}
    };
    
    using BenchmarkFunction = std::function<void()>;
    
public:
    PerformanceAnalyzer() = default;
    
    // Run a benchmark test
    BenchmarkResult runBenchmark(const std::string& name, BenchmarkFunction benchmark, 
                                 size_t iterations = 100, bool warmUp = true);
    
    // Run a comparison benchmark
    std::vector<BenchmarkResult> runComparisonBenchmark(
        const std::vector<std::pair<std::string, BenchmarkFunction>>& benchmarks,
        size_t iterations = 100);
    
    // Profile brain components
    void profileBrainComponents(std::shared_ptr<class Brain> brain);
    
    // Profile simulation performance
    void profileSimulation(const std::string& testName, std::shared_ptr<class Brain> brain,
                          SimulationStep steps, Timestamp timestep = 0.001);
    
    // Profile memory usage
    double measureMemoryUsage() const;
    
    // Get performance recommendations
    std::vector<std::string> getPerformanceRecommendations() const;
    
    // Export analysis results
    std::string exportAnalysis() const;
    void exportAnalysisToFile(const std::string& filepath) const;
    
    // Set analysis parameters
    void setWarmUpIterations(size_t iterations) { warmUpIterations_ = iterations; }
    void setBenchmarkIterations(size_t iterations) { benchmarkIterations_ = iterations; }
    void setMemorySamplingInterval(size_t interval) { memorySamplingInterval_ = interval; }
    
private:
    size_t warmUpIterations_ = 10;
    size_t benchmarkIterations_ = 100;
    size_t memorySamplingInterval_ = 1000;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class PerformanceMonitor {
public:
    struct SystemMetrics {
        double cpuUsage;
        double memoryUsage;
        double diskUsage;
        std::vector<double> processMetrics;
        double timestamp;
        
        SystemMetrics() : cpuUsage(0), memoryUsage(0), diskUsage(0), timestamp(0) {}
    };
    
    struct NeuralMetrics {
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        float excitationInhibitionRatio;
        size_t totalSpikeCount;
        double lastUpdateTime;
        
        NeuralMetrics() : totalNeurons(0), totalSynapses(0), activeNeurons(0), 
                         firingNeurons(0), averageFiringRate(0), 
                         excitationInhibitionRatio(0), totalSpikeCount(0), 
                         lastUpdateTime(0) {}
    };
    
public:
    PerformanceMonitor() = default;
    
    // Start monitoring
    void startMonitoring(size_t samplingInterval = 100);
    
    // Stop monitoring
    void stopMonitoring();
    
    // Get latest system metrics
    SystemMetrics getSystemMetrics() const;
    
    // Get latest neural metrics
    NeuralMetrics getNeuralMetrics() const;
    
    // Get monitoring history
    const std::vector<SystemMetrics>& getSystemHistory() const { return systemHistory_; }
    const std::vector<NeuralMetrics>& getNeuralHistory() const { return neuralHistory_; }
    
    // Record a system snapshot
    void recordSystemSnapshot();
    
    // Record neural snapshot from brain
    void recordNeuralSnapshot(std::shared_ptr<class Brain> brain);
    
    // Calculate statistics
    double getAverageCpuUsage() const;
    double getMaxCpuUsage() const;
    double getAverageMemoryUsage() const;
    double getMaxMemoryUsage() const;
    
    // Generate performance report
    std::string generatePerformanceReport() const;
    
    // Export monitoring data
    std::string exportMonitoringData() const;
    void exportMonitoringToFile(const std::string& filepath) const;
    
    // Clear history
    void clearHistory();
    
private:
    size_t samplingInterval_;
    bool monitoring_ = false;
    
    mutable std::vector<SystemMetrics> systemHistory_;
    mutable std::vector<NeuralMetrics> neuralHistory_;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm