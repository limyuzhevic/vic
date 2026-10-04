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

// AlgorithmProfiler: Profile specific algorithms and operations
class AlgorithmProfiler {
public:
    AlgorithmProfiler();
    ~AlgorithmProfiler();
    
    // Configuration
    void setProfilingInterval(double interval) { profilingInterval_ = interval; }
    void setMinSamples(size_t minSamples) { minSamples_ = minSamples; }
    void setEnableParameterProfiling(bool enable) { enableParameterProfiling_ = enable; }
    void setEnableConvergenceTracking(bool enable) { enableConvergenceTracking_ = enable; }
    
    // Algorithm operation tracking
    void recordAlgorithmOperation(const std::string& algorithmName,
                                 const std::string& operation,
                                 double time,
                                 const std::string& parameters = "",
                                 const std::string& context = "",
                                 double convergence = 0.0,
                                 size_t iteration = 0);
    
    // Query results
    double getAverageTime(const std::string& algorithmName, const std::string& operation) const;
    double getMinTime(const std::string& algorithmName, const std::string& operation) const;
    double getMaxTime(const std::string& algorithmName, const std::string& operation) const;
    double getStandardDeviation(const std::string& algorithmName, const std::string& operation) const;
    
    // Algorithm listing
    std::vector<std::string> getAlgorithmNames() const;
    std::vector<std::string> getAlgorithmOperations(const std::string& algorithmName) const;
    size_t getOperationCount(const std::string& algorithmName, const std::string& operation) const;
    
    // Convergence tracking
    double getConvergence(const std::string& algorithmName, const std::string& operation, size_t iteration) const;
    double getAverageConvergence(const std::string& algorithmName, const std::string& operation) const;
    
    // Analysis
    std::string analyzeAlgorithmEfficiency() const;
    std::string analyzeConvergenceBehavior() const;
    std::string identifySlowOperations() const;
    std::string analyzeParameterImpact() const;
    
    // Statistics
    size_t getTotalOperations() const { return totalOperations_; }
    size_t getActiveAlgorithms() const { return activeAlgorithms_.size(); }
    
    // Export
    bool exportProfile(const std::string& filepath) const;
    std::string exportProfileAsString() const;
    
private:
    struct AlgorithmOperation {
        std::chrono::system_clock::time_point timestamp;
        std::string algorithmName;
        std::string operation;
        double time;
        std::string parameters;
        std::string context;
        double convergence;
        size_t iteration;
        
        AlgorithmOperation(const std::string& algo, const std::string& op, double time,
                          const std::string& params, const std::string& ctx,
                          double conv, size_t iter)
            : algorithmName(algo), operation(op), time(time), parameters(params),
              context(ctx), convergence(conv), iteration(iter) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    std::vector<AlgorithmOperation> operations_;
    mutable std::mutex operationsMutex_;
    
    std::unordered_map<std::string, std::unordered_set<std::string>> activeAlgorithms_;
    mutable std::mutex activeMutex_;
    
    std::unordered_map<std::string, std::unordered_map<size_t, double>> convergenceHistory_;
    mutable std::mutex convergenceMutex_;
    
    double profilingInterval_;
    bool enableParameterProfiling_;
    bool enableConvergenceTracking_;
    size_t minSamples_;
    size_t totalOperations_;
    
    // Helper methods
    void cleanupOldOperations();
    double calculateEfficiencyScore(const AlgorithmOperation& op) const;
    std::string getParameterSignature(const std::string& parameters) const;
    bool isSlowOperation(const std::string& algorithmName, const std::string& operation, double avgTime) const;
    double calculateConvergenceTrend(const std::string& algorithmName, const std::string& operation) const;
};

} // namespace nlm
