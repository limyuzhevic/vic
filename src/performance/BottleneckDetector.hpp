#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <memory>

namespace nlm {

// BottleneckDetector: Automatic identification of performance bottlenecks
class BottleneckDetector {
public:
    BottleneckDetector();
    explicit BottleneckDetector(std::shared_ptr<PerformanceProfiler> profiler);
    ~BottleneckDetector();
    
    // Configuration
    void setThreshold(double threshold) { threshold_ = threshold; }
    void setTimeWindow(double window) { timeWindow_ = window; }
    void setMaxOperations(size_t max) { maxOperations_ = max; }
    void setEnableFunctionAnalysis(bool enable) { enableFunctionAnalysis_ = enable; }
    void setEnableAlgorithmAnalysis(bool enable) { enableAlgorithmAnalysis_ = enable; }
    void setEnableMemoryAnalysis(bool enable) { enableMemoryAnalysis_ = enable; }
    void setEnableConvergenceAnalysis(bool enable) { enableConvergenceAnalysis_ = enable; }
    
    // Integration with profilers
    void setPerformanceProfiler(std::shared_ptr<PerformanceProfiler> profiler) { 
        performanceProfiler_ = profiler; 
    }
    void setFunctionCallProfiler(std::shared_ptr<FunctionCallProfiler> profiler) { 
        functionCallProfiler_ = profiler; 
    }
    void setAlgorithmProfiler(std::shared_ptr<AlgorithmProfiler> profiler) { 
        algorithmProfiler_ = profiler; 
    }
    
    // Detection
    std::vector<std::string> detectBottlenecks() const;
    std::string getBottleneckReport() const;
    std::string getDetailedBottleneckReport() const;
    
    // Analysis
    double getBottleneckScore(const std::string& operation) const;
    std::chrono::system_clock::time_point getLastDetectionTime() const { return lastDetectionTime_; }
    
    // Specific bottleneck detection
    std::vector<std::string> detectFunctionLevelBottlenecks() const;
    std::vector<std::string> detectAlgorithmLevelBottlenecks() const;
    std::vector<std::string> detectMemoryLevelBottlenecks() const;
    std::vector<std::string> detectConvergenceLevelBottlenecks() const;
    
    // Integration
    void updateProfiler(std::shared_ptr<PerformanceProfiler> profiler) { 
        performanceProfiler_ = profiler; 
    }
    
private:
    struct BottleneckInfo {
        std::string type;
        std::string location;
        double severity;
        std::vector<std::string> suggestions;
        std::chrono::milliseconds estimatedFixTime;
        std::string description;
    };
    
    std::vector<BottleneckInfo> bottlenecks_;
    mutable std::mutex bottlenecksMutex_;
    
    std::shared_ptr<PerformanceProfiler> performanceProfiler_;
    std::shared_ptr<FunctionCallProfiler> functionCallProfiler_;
    std::shared_ptr<AlgorithmProfiler> algorithmProfiler_;
    
    double threshold_;
    double timeWindow_;
    size_t maxOperations_;
    bool enableFunctionAnalysis_;
    bool enableAlgorithmAnalysis_;
    bool enableMemoryAnalysis_;
    bool enableConvergenceAnalysis_;
    std::chrono::system_clock::time_point lastDetectionTime_;
    
    // Helper methods
    void performDetection();
    void detectFunctionBottlenecks();
    void detectAlgorithmBottlenecks();
    void detectMemoryBottlenecks();
    void detectConvergenceBottlenecks();
    double calculateFunctionBottleneckScore(const std::string& functionName) const;
    double calculateAlgorithmBottleneckScore(const std::string& algorithmName, 
                                            const std::string& operation) const;
    bool isFunctionBottleneck(const std::string& functionName, double avgTime) const;
    bool isAlgorithmBottleneck(const std::string& algorithmName, 
                              const std::string& operation, double avgTime) const;
    std::vector<std::string> analyzeMemoryUsage() const;
    std::vector<std::string> analyzeConvergenceBehavior() const;
};

} // namespace nlm
