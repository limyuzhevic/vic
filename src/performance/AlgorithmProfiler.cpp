#include "AlgorithmProfiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_set>

namespace nlm {

AlgorithmProfiler::AlgorithmProfiler()
    : profilingInterval_(0.1),
      enableParameterProfiling_(true),
      enableConvergenceTracking_(true),
      minSamples_(10),
      totalOperations_(0)
{
}

AlgorithmProfiler::~AlgorithmProfiler() = default;

void AlgorithmProfiler::recordAlgorithmOperation(const std::string& algorithmName,
                                                const std::string& operation,
                                                double time,
                                                const std::string& parameters,
                                                const std::string& context,
                                                double convergence,
                                                size_t iteration) {
    AlgorithmOperation op(algorithmName, operation, time, parameters, context, convergence, iteration);
    
    std::lock_guard<std::mutex> lock(operationsMutex_);
    operations_.push_back(op);
    totalOperations_++;
    
    // Track active algorithms
    std::lock_guard<std::mutex> activeLock(activeMutex_);
    activeAlgorithms_[algorithmName].insert(operation);
    
    // Track convergence history
    if (enableConvergenceTracking_) {
        std::lock_guard<std::mutex> convLock(convergenceMutex_);
        convergenceHistory_[algorithmName][iteration] = convergence;
    }
    
    // Cleanup old operations periodically
    if (operations_.size() % 1000 == 0) {
        cleanupOldOperations();
    }
}

double AlgorithmProfiler::getAverageTime(const std::string& algorithmName, const std::string& operation) const {
    std::vector<double> times;
    {
        std::lock_guard<std::mutex> lock(operationsMutex_);
        for (const auto& op : operations_) {
            if (op.algorithmName == algorithmName && op.operation == operation) {
                times.push_back(op.time);
            }
        }
    }
    
    if (times.empty()) return 0.0;
    
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    return sum / times.size();
}

double AlgorithmProfiler::getMinTime(const std::string& algorithmName, const std::string& operation) const {
    std::vector<double> times;
    {
        std::lock_guard<std::mutex> lock(operationsMutex_);
        for (const auto& op : operations_) {
            if (op.algorithmName == algorithmName && op.operation == operation) {
                times.push_back(op.time);
            }
        }
    }
    
    if (times.empty()) return 0.0;
    
    return *std::min_element(times.begin(), times.end());
}

double AlgorithmProfiler::getMaxTime(const std::string& algorithmName, const std::string& operation) const {
    std::vector<double> times;
    {
        std::lock_guard<std::mutex> lock(operationsMutex_);
        for (const auto& op : operations_) {
            if (op.algorithmName == algorithmName && op.operation == operation) {
                times.push_back(op.time);
            }
        }
    }
    
    if (times.empty()) return 0.0;
    
    return *std::max_element(times.begin(), times.end());
}

double AlgorithmProfiler::getStandardDeviation(const std::string& algorithmName, const std::string& operation) const {
    std::vector<double> times;
    {
        std::lock_guard<std::mutex> lock(operationsMutex_);
        for (const auto& op : operations_) {
            if (op.algorithmName == algorithmName && op.operation == operation) {
                times.push_back(op.time);
            }
        }
    }
    
    if (times.empty()) return 0.0;
    
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    double mean = sum / times.size();
    
    double sqSum = 0.0;
    for (double time : times) {
        sqSum += (time - mean) * (time - mean);
    }
    
    return std::sqrt(sqSum / times.size());
}

std::vector<std::string> AlgorithmProfiler::getAlgorithmNames() const {
    std::vector<std::string> algorithms;
    {
        std::lock_guard<std::mutex> lock(activeMutex_);
        algorithms.reserve(activeAlgorithms_.size());
        for (const auto& [algo, ops] : activeAlgorithms_) {
            algorithms.push_back(algo);
        }
    }
    return algorithms;
}

std::vector<std::string> AlgorithmProfiler::getAlgorithmOperations(const std::string& algorithmName) const {
    std::vector<std::string> operations;
    {
        std::lock_guard<std::mutex> lock(activeMutex_);
        auto it = activeAlgorithms_.find(algorithmName);
        if (it != activeAlgorithms_.end()) {
            operations.reserve(it->second.size());
            for (const auto& op : it->second) {
                operations.push_back(op);
            }
        }
    }
    return operations;
}

size_t AlgorithmProfiler::getOperationCount(const std::string& algorithmName, const std::string& operation) const {
    std::lock_guard<std::mutex> lock(operationsMutex_);
    size_t count = 0;
    for (const auto& op : operations_) {
        if (op.algorithmName == algorithmName && op.operation == operation) {
            count++;
        }
    }
    return count;
}

double AlgorithmProfiler::getConvergence(const std::string& algorithmName, const std::string& operation, size_t iteration) const {
    std::lock_guard<std::mutex> lock(convergenceMutex_);
    auto algoIt = convergenceHistory_.find(algorithmName);
    if (algoIt != convergenceHistory_.end()) {
        auto iterIt = algoIt->second.find(iteration);
        if (iterIt != algoIt->second.end()) {
            return iterIt->second;
        }
    }
    return 0.0;
}

double AlgorithmProfiler::getAverageConvergence(const std::string& algorithmName, const std::string& operation) const {
    std::lock_guard<std::mutex> lock(convergenceMutex_);
    double totalConvergence = 0.0;
    size_t count = 0;
    
    for (const auto& [iter, conv] : convergenceHistory_[algorithmName]) {
        totalConvergence += conv;
        count++;
    }
    
    if (count == 0) return 0.0;
    return totalConvergence / count;
}

std::string AlgorithmProfiler::analyzeAlgorithmEfficiency() const {
    std::ostringstream oss;
    oss << "Algorithm Efficiency Analysis:\n";
    oss << "==========================\n\n";
    
    auto algorithmNames = getAlgorithmNames();
    
    oss << "Algorithm Performance Ranking:\n";
    std::vector<std::pair<std::string, double>> efficiencyScores;
    
    for (const auto& algo : algorithmNames) {
        auto operations = getAlgorithmOperations(algo);
        double totalTime = 0.0;
        size_t totalOps = 0;
        
        for (const auto& op : operations) {
            double avgTime = getAverageTime(algo, op);
            size_t count = getOperationCount(algo, op);
            totalTime += avgTime * count;
            totalOps += count;
        }
        
        double efficiencyScore = totalOps > 0 ? (1.0 / totalTime) : 0.0;
        efficiencyScores.emplace_back(algo, efficiencyScore);
    }
    
    std::sort(efficiencyScores.begin(), efficiencyScores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (size_t i = 0; i < std::min(efficiencyScores.size(), size_t(10)); ++i) {
        const auto& [algo, score] = efficiencyScores[i];
        double avgTime = 0.0;
        size_t totalOps = 0;
        
        for (const auto& op : getAlgorithmOperations(algo)) {
            size_t count = getOperationCount(algo, op);
            avgTime += getAverageTime(algo, op) * count;
            totalOps += count;
        }
        
        if (totalOps > 0) avgTime /= totalOps;
        
        oss << i + 1 << ". " << algo << "\n";
        oss << "   Efficiency Score: " << score << "\n";
        oss << "   Average Operation Time: " << avgTime << "s\n";
        oss << "   Total Operations: " << totalOps << "\n";
        
        if (score < 0.1) {
            oss << "   *** LOW EFFICIENCY ***\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string AlgorithmProfiler::analyzeConvergenceBehavior() const {
    std::ostringstream oss;
    oss << "Algorithm Convergence Behavior:\n";
    oss << "============================\n\n";
    
    std::lock_guard<std::mutex> lock(convergenceMutex_);
    
    oss << "Convergence History by Algorithm:\n";
    for (const auto& [algo, iterations] : convergenceHistory_) {
        oss << "  " << algo << ":\n";
        
        std::vector<std::pair<size_t, double>> sortedIterations(
            iterations.begin(), iterations.end());
        std::sort(sortedIterations.begin(), sortedIterations.end());
        
        for (const auto& [iter, conv] : sortedIterations) {
            oss << "    Iteration " << iter << ": " << conv << "\n";
        }
        
        double avgConvergence = getAverageConvergence(algo, "");
        oss << "    Average Convergence: " << avgConvergence << "\n";
        oss << "    Convergence Trend: " << calculateConvergenceTrend(algo, "") << "\n\n";
    }
    
    return oss.str();
}

std::string AlgorithmProfiler::identifySlowOperations() const {
    std::ostringstream oss;
    oss << "Slow Algorithm Operation Identification:\n";
    oss << "===================================\n\n";
    
    auto algorithmNames = getAlgorithmNames();
    
    oss << "Operations with High Average Times:\n";
    for (const auto& algo : algorithmNames) {
        auto operations = getAlgorithmOperations(algo);
        
        for (const auto& op : operations) {
            double avgTime = getAverageTime(algo, op);
            size_t count = getOperationCount(algo, op);
            
            if (count >= minSamples_ && avgTime > profilingInterval_ * 5) {
                oss << "  " << algo << "." << op << " (avg: " << avgTime << "s, ops: " << count << ")\n";
                
                // Suggest optimizations
                if (avgTime > profilingInterval_ * 10) {
                    oss << "    *** CRITICAL - Optimization needed ***\n";
                } else if (avgTime > profilingInterval_ * 7) {
                    oss << "    *** HIGH PRIORITY - Optimization recommended ***\n";
                } else {
                    oss << "    *** MEDIUM PRIORITY - Consider optimization ***\n";
                }
            }
        }
    }
    
    oss << "\nOperations with High Variance:\n";
    for (const auto& algo : algorithmNames) {
        auto operations = getAlgorithmOperations(algo);
        
        for (const auto& op : operations) {
            double stddev = getStandardDeviation(algo, op);
            double avgTime = getAverageTime(algo, op);
            size_t count = getOperationCount(algo, op);
            
            if (count >= minSamples_ && avgTime > 0 && (stddev / avgTime) > 0.5) {
                oss << "  " << algo << "." << op << " (avg: " << avgTime << "s, stddev: " << stddev << "s, cv: " << (stddev / avgTime) << ")\n";
                oss << "    *** HIGH VARIANCE - Inconsistent performance ***\n";
            }
        }
    }
    
    return oss.str();
}

std::string AlgorithmProfiler::analyzeParameterImpact() const {
    std::ostringstream oss;
    oss << "Parameter Impact Analysis:\n";
    oss << "=========================\n\n";
    
    if (!enableParameterProfiling_) {
        oss << "Parameter profiling is disabled.\n";
        return oss.str();
    }
    
    std::lock_guard<std::mutex> lock(operationsMutex_);
    
    std::unordered_map<std::string, std::vector<double>> parameterTimes;
    
    // Group operations by parameter signature
    for (const auto& op : operations_) {
        std::string paramSig = getParameterSignature(op.parameters);
        parameterTimes[paramSig].push_back(op.time);
    }
    
    oss << "Performance by Parameter Configuration:\n";
    for (const auto& [params, times] : parameterTimes) {
        if (times.size() < minSamples_) continue;
        
        double avgTime = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
        double minTime = *std::min_element(times.begin(), times.end());
        double maxTime = *std::max_element(times.begin(), times.end());
        
        oss << "  Parameters: " << params << "\n";
        oss << "    Samples: " << times.size() << "\n";
        oss << "    Average time: " << avgTime << "s\n";
        oss << "    Min time: " << minTime << "s\n";
        oss << "    Max time: " << maxTime << "s\n";
        
        if (maxTime > avgTime * 2) {
            oss << "    *** HIGH VARIATION ***\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

bool AlgorithmProfiler::exportProfile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << exportProfileAsString();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::string AlgorithmProfiler::exportProfileAsString() const {
    std::ostringstream oss;
    oss << "Algorithm Profile\n";
    oss << "================\n\n";
    
    oss << "Total Algorithm Operations: " << totalOperations_ << "\n";
    oss << "Active Algorithms: " << activeAlgorithms_.size() << "\n\n";
    
    auto algorithmNames = getAlgorithmNames();
    
    oss << "Algorithm Operation Statistics:\n";
    for (const auto& algo : algorithmNames) {
        auto operations = getAlgorithmOperations(algo);
        
        oss << "  " << algo << ":\n";
        
        for (const auto& op : operations) {
            size_t count = getOperationCount(algo, op);
            double avgTime = getAverageTime(algo, op);
            double minTime = getMinTime(algo, op);
            double maxTime = getMaxTime(algo, op);
            double stddev = getStandardDeviation(algo, op);
            
            oss << "    " << op << ":\n";
            oss << "      Operations: " << count << "\n";
            oss << "      Average time: " << avgTime << "s\n";
            oss << "      Min time: " << minTime << "s\n";
            oss << "      Max time: " << maxTime << "s\n";
            oss << "      Std Dev: " << stddev << "s\n";
            oss << "      Coefficient of Variation: " << (avgTime > 0 ? (stddev / avgTime) : 0) << "\n";
        }
        oss << "\n";
    }
    
    oss << "\nConvergence History:\n";
    for (const auto& [algo, iterations] : convergenceHistory_) {
        oss << "  " << algo << ":\n";
        for (const auto& [iter, conv] : iterations) {
            oss << "    Iteration " << iter << ": " << conv << "\n";
        }
    }
    
    return oss.str();
}

void AlgorithmProfiler::cleanupOldOperations() {
    // Remove operations older than 1 hour
    auto now = std::chrono::system_clock::now();
    auto oneHourAgo = now - std::chrono::hours(1);
    
    std::lock_guard<std::mutex> lock(operationsMutex_);
    auto it = std::remove_if(operations_.begin(), operations_.end(),
                             [oneHourAgo](const AlgorithmOperation& op) {
                                 return op.timestamp < oneHourAgo;
                             });
    operations_.erase(it, operations_.end());
}

double AlgorithmProfiler::calculateEfficiencyScore(const AlgorithmOperation& op) const {
    double timeScore = op.time * 1000; // Scale to milliseconds
    double convergenceScore = (1.0 - std::abs(op.convergence - 1.0)) * 100; // Better convergence = higher score
    
    return (timeScore + convergenceScore) / 2.0;
}

std::string AlgorithmProfiler::getParameterSignature(const std::string& parameters) const {
    // Simple parameter signature for grouping
    if (parameters.empty()) return "default";
    
    // Could be enhanced to parse and normalize parameters
    return parameters;
}

bool AlgorithmProfiler::isSlowOperation(const std::string& algorithmName, const std::string& operation, double avgTime) const {
    return avgTime > profilingInterval_ * 10;
}

double AlgorithmProfiler::calculateConvergenceTrend(const std::string& algorithmName, const std::string& operation) const {
    std::lock_guard<std::mutex> lock(convergenceMutex_);
    
    auto algoIt = convergenceHistory_.find(algorithmName);
    if (algoIt == convergenceHistory_.end() || algoIt->second.size() < 2) {
        return 0.0;
    }
    
    std::vector<std::pair<size_t, double>> sortedIterations(
        algoIt->second.begin(), algoIt->second.end());
    std::sort(sortedIterations.begin(), sortedIterations.end());
    
    if (sortedIterations.size() < 2) return 0.0;
    
    // Calculate linear regression slope
    size_t n = sortedIterations.size();
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(sortedIterations[i].first);
        double y = sortedIterations[i].second;
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double denominator = (n * sumX2 - sumX * sumX);
    if (std::abs(denominator) < 1e-10) return 0.0;
    
    double slope = (n * sumXY - sumX * sumY) / denominator;
    
    if (slope > 0.01) {
        return 1.0; // Improving
    } else if (slope < -0.01) {
        return -1.0; // Degrading
    } else {
        return 0.0; // Stable
    }
}

} // namespace nlm
