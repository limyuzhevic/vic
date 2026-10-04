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
#include <map>

namespace nlm {

// ParallelPerformanceAnalyzer: Analysis of parallel processing efficiency
class ParallelPerformanceAnalyzer {
public:
    ParallelPerformanceAnalyzer();
    ~ParallelPerformanceAnalyzer();
    
    // Configuration
    void setEnableParallelProfiling(bool enable) { enableParallelProfiling_ = enable; }
    void setMinParallelSamples(size_t min) { minParallelSamples_ = min; }
    void setEnableLoadBalanceAnalysis(bool enable) { enableLoadBalanceAnalysis_ = enable; }
    void setEnableContentionAnalysis(bool enable) { enableContentionAnalysis_ = enable; }
    
    // Parallel performance tracking
    void recordParallelExecution(const std::string& operation, size_t threads,
                                 double time, double efficiency,
                                 double loadBalance, double contention,
                                 const std::string& context = "");
    
    // Query results
    double getAverageTime(const std::string& operation, size_t threads) const;
    double getAverageEfficiency(const std::string& operation, size_t threads) const;
    double getAverageLoadBalance(const std::string& operation, size_t threads) const;
    double getAverageContention(const std::string& operation, size_t threads) const;
    
    // Parallel operation listing
    std::vector<std::string> getParallelOperations() const;
    std::vector<size_t> getThreadCounts() const;
    
    // Analysis
    std::string analyzeParallelEfficiency() const;
    std::string analyzeLoadBalance() const;
    std::string analyzeResourceContention() const;
    std::string identifyParallelBottlenecks() const;
    
    // Statistics
    size_t getTotalParallelExecutions() const { return totalExecutions_; }
    std::string getOptimalThreadCount(const std::string& operation) const;
    std::string analyzeScalingLaw(const std::string& operation) const;
    
    // Export
    bool exportParallelProfile(const std::string& filepath) const;
    std::string exportParallelProfileAsString() const;
    
    // Real-time analysis
    std::string getRealTimeParallelStats() const;
    
private:
    struct ParallelExecution {
        std::chrono::system_clock::time_point timestamp;
        std::string operation;
        size_t threads;
        double time;
        double efficiency;
        double loadBalance;
        double contention;
        std::string context;
        
        ParallelExecution(const std::string& op, size_t threads, double time,
                         double efficiency, double loadBalance, double contention,
                         const std::string& ctx)
            : operation(op), threads(threads), time(time), efficiency(efficiency),
              loadBalance(loadBalance), contention(contention), context(ctx) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    std::vector<ParallelExecution> executions_;
    mutable std::mutex executionsMutex_;
    
    bool enableParallelProfiling_;
    bool enableLoadBalanceAnalysis_;
    bool enableContentionAnalysis_;
    size_t minParallelSamples_;
    size_t totalExecutions_;
    
    // Helper methods
    void cleanupOldExecutions();
    double calculateParallelEfficiency(const std::string& operation, size_t threads) const;
    double calculateLoadBalanceScore(const std::string& operation, size_t threads) const;
    double calculateContentionScore(const std::string& operation, size_t threads) const;
    std::string getThreadScalingDescription(size_t threads) const;
    bool isLoadBalanced(double loadBalance) const;
    bool isContentionHigh(double contention) const;
};

} // namespace nlm
