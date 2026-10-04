#include "ParallelPerformanceAnalyzer.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_set>

namespace nlm {

ParallelPerformanceAnalyzer::ParallelPerformanceAnalyzer()
    : enableParallelProfiling_(true),
      enableLoadBalanceAnalysis_(true),
      enableContentionAnalysis_(true),
      minParallelSamples_(5),
      totalExecutions_(0) {
}

ParallelPerformanceAnalyzer::~ParallelPerformanceAnalyzer() = default;

void ParallelPerformanceAnalyzer::recordParallelExecution(const std::string& operation, size_t threads,
                                                         double time, double efficiency,
                                                         double loadBalance, double contention,
                                                         const std::string& context) {
    ParallelExecution execution(operation, threads, time, efficiency, loadBalance, contention, context);
    
    std::lock_guard<std::mutex> lock(executionsMutex_);
    executions_.push_back(execution);
    totalExecutions_++;
    
    // Cleanup old executions periodically
    if (executions_.size() % 1000 == 0) {
        cleanupOldExecutions();
    }
}

double ParallelPerformanceAnalyzer::getAverageTime(const std::string& operation, size_t threads) const {
    std::lock_guard<std::mutex> lock(executionsMutex_);
    
    std::vector<double> times;
    for (const auto& execution : executions_) {
        if (execution.operation == operation && execution.threads == threads) {
            times.push_back(execution.time);
        }
    }
    
    if (times.empty()) return 0.0;
    
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    return sum / times.size();
}

double ParallelPerformanceAnalyzer::getAverageEfficiency(const std::string& operation, size_t threads) const {
    std::lock_guard<std::mutex> lock(executionsMutex_);
    
    std::vector<double> efficiencies;
    for (const auto& execution : executions_) {
        if (execution.operation == operation && execution.threads == threads) {
            efficiencies.push_back(execution.efficiency);
        }
    }
    
    if (efficiencies.empty()) return 0.0;
    
    double sum = std::accumulate(efficiencies.begin(), efficiencies.end(), 0.0);
    return sum / efficiencies.size();
}

double ParallelPerformanceAnalyzer::getAverageLoadBalance(const std::string& operation, size_t threads) const {
    std::lock_guard<std::mutex> lock(executionsMutex_);
    
    std::vector<double> balances;
    for (const auto& execution : executions_) {
        if (execution.operation == operation && execution.threads == threads) {
            balances.push_back(execution.loadBalance);
        }
    }
    
    if (balances.empty()) return 0.0;
    
    double sum = std::accumulate(balances.begin(), balances.end(), 0.0);
    return sum / balances.size();
}

double ParallelPerformanceAnalyzer::getAverageContention(const std::string& operation, size_t threads) const {
    std::lock_guard<std::mutex> lock(executionsMutex_);
    
    std::vector<double> contentions;
    for (const auto& execution : executions_) {
        if (execution.operation == operation && execution.threads == threads) {
            contentions.push_back(execution.contention);
        }
    }
    
    if (contentions.empty()) return 0.0;
    
    double sum = std::accumulate(contentions.begin(), contentions.end(), 0.0);
    return sum / contentions.size();
}

std::vector<std::string> ParallelPerformanceAnalyzer::getParallelOperations() const {
    std::lock_guard<std::mutex> lock(executionsMutex_);
    
    std::vector<std::string> operations;
    std::unordered_set<std::string> uniqueOps;
    for (const auto& execution : executions_) {
        if (uniqueOps.find(execution.operation) == uniqueOps.end()) {
            uniqueOps.insert(execution.operation);
            operations.push_back(execution.operation);
        }
    }
    
    return operations;
}

std::vector<size_t> ParallelPerformanceAnalyzer::getThreadCounts() const {
    std::lock_guard<std::mutex> lock(executionsMutex_);
    
    std::vector<size_t> threads;
    std::unordered_set<size_t> uniqueThreads;
    for (const auto& execution : executions_) {
        if (uniqueThreads.find(execution.threads) == uniqueThreads.end()) {
            uniqueThreads.insert(execution.threads);
            threads.push_back(execution.threads);
        }
    }
    
    std::sort(threads.begin(), threads.end());
    return threads;
}

std::string ParallelPerformanceAnalyzer::analyzeParallelEfficiency() const {
    std::ostringstream oss;
    oss << "Parallel Efficiency Analysis:\n";
    oss << "=========================\n\n";
    
    auto operations = getParallelOperations();
    auto threadCounts = getThreadCounts();
    
    oss << "Parallel Efficiency by Operation:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        double maxEfficiency = 0.0;
        size_t optimalThreads = 0;
        
        for (size_t threads : threadCounts) {
            double efficiency = getAverageEfficiency(operation, threads);
            if (efficiency > maxEfficiency) {
                maxEfficiency = efficiency;
                optimalThreads = threads;
            }
            
            oss << "    " << threads << " threads: " << (efficiency * 100) << "%\n";
            
            if (efficiency > 0.9) {
                oss << "      *** EXCELLENT EFFICIENCY ***\n";
            } else if (efficiency > 0.7) {
                oss << "      *** GOOD EFFICIENCY ***\n";
            } else if (efficiency > 0.5) {
                oss << "      *** MODERATE EFFICIENCY ***\n";
            } else {
                oss << "      *** POOR EFFICIENCY ***\n";
            }
        }
        
        oss << "    Optimal Thread Count: " << optimalThreads << " (" << (maxEfficiency * 100) << "% efficiency)\n";
        oss << "\n";
    }
    
    return oss.str();
}

std::string ParallelPerformanceAnalyzer::analyzeLoadBalance() const {
    std::ostringstream oss;
    oss << "Load Balance Analysis:\n";
    oss << "=====================\n\n";
    
    auto operations = getParallelOperations();
    auto threadCounts = getThreadCounts();
    
    oss << "Load Balance Assessment:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        for (size_t threads : threadCounts) {
            double loadBalance = getAverageLoadBalance(operation, threads);
            
            oss << "    " << threads << " threads: " << (loadBalance * 100) << "% balanced\n";
            
            if (isLoadBalanced(loadBalance)) {
                oss << "      *** WELL BALANCED ***\n";
            } else if (loadBalance > 0.5) {
                oss << "      *** MODERATELY BALANCED ***\n";
            } else {
                oss << "      *** POORLY BALANCED ***\n";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string ParallelPerformanceAnalyzer::analyzeResourceContention() const {
    std::ostringstream oss;
    oss << "Resource Contention Analysis:\n";
    oss << "==========================\n\n";
    
    auto operations = getParallelOperations();
    auto threadCounts = getThreadCounts();
    
    oss << "Contention Analysis:\n";
    
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        
        for (size_t threads : threadCounts) {
            double contention = getAverageContention(operation, threads);
            
            oss << "    " << threads << " threads: " << (contention * 100) << "% contention\n";
            
            if (isContentionHigh(contention)) {
                oss << "      *** HIGH CONTENTION ***\n";
            } else if (contention > 0.3) {
                oss << "      *** MODERATE CONTENTION ***\n";
            } else {
                oss << "      *** LOW CONTENTION ***\n";
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string ParallelPerformanceAnalyzer::identifyParallelBottlenecks() const {
    std::ostringstream oss;
    oss << "Parallel Bottleneck Identification:\n";
    oss << "===============================\n\n";
    
    auto operations = getParallelOperations();
    auto threadCounts = getThreadCounts();
    
    oss << "Identified Parallel Bottlenecks:\n";
    
    for (const auto& operation : operations) {
        bool hasBottleneck = false;
        
        for (size_t threads : threadCounts) {
            double efficiency = getAverageEfficiency(operation, threads);
            double loadBalance = getAverageLoadBalance(operation, threads);
            double contention = getAverageContention(operation, threads);
            
            if (efficiency < 0.5) {
                hasBottleneck = true;
                oss << "  " << operation << " with " << threads << " threads: efficiency " << 
                    (efficiency * 100) << "%\n";
                oss << "    *** EFFICIENCY BOTTLENECK ***\n";
                
                if (threads > 2) {
                    oss << "    Suggestion: Reduce thread count\n";
                } else {
                    oss << "    Suggestion: Optimize algorithm\n";
                }
            }
            
            if (!isLoadBalanced(loadBalance)) {
                hasBottleneck = true;
                oss << "  " << operation << " with " << threads << " threads: load imbalance " << 
                    (loadBalance * 100) << "%\n";
                oss << "    *** LOAD IMBALANCE BOTTLENECK ***\n";
                oss << "    Suggestion: Balance workload across threads\n";
            }
            
            if (isContentionHigh(contention)) {
                hasBottleneck = true;
                oss << "  " << operation << " with " << threads << " threads: contention " << 
                    (contention * 100) << "%\n";
                oss << "    *** RESOURCE CONTENTION BOTTLENECK ***\n";
                oss << "    Suggestion: Reduce shared resource usage\n";
            }
        }
        
        if (!hasBottleneck) {
            oss << "  " << operation << ": No parallel bottlenecks detected\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string ParallelPerformanceAnalyzer::getOptimalThreadCount(const std::string& operation) const {
    auto threadCounts = getThreadCounts();
    
    if (threadCounts.empty()) return "No data available";
    
    double maxEfficiency = 0.0;
    size_t optimalThreads = threadCounts[0];
    
    for (size_t threads : threadCounts) {
        double efficiency = getAverageEfficiency(operation, threads);
        if (efficiency > maxEfficiency) {
            maxEfficiency = efficiency;
            optimalThreads = threads;
        }
    }
    
    std::ostringstream oss;
    oss << "Optimal thread count for " << operation << ": " << optimalThreads << 
        " threads (" << (maxEfficiency * 100) << "% efficiency)";
    
    return oss.str();
}

std::string ParallelPerformanceAnalyzer::analyzeScalingLaw(const std::string& operation) const {
    std::ostringstream oss;
    oss << "Scaling Law Analysis for " << operation << ":\n";
    oss << "====================================\n\n";
    
    auto threadCounts = getThreadCounts();
    if (threadCounts.size() < 2) {
        oss << "Insufficient data for scaling law analysis.\n";
        return oss.str();
    }
    
    std::vector<double> efficiencies;
    for (size_t threads : threadCounts) {
        efficiencies.push_back(getAverageEfficiency(operation, threads));
    }
    
    // Simple Amdahl's law analysis
    oss << "Observed Speedup:\n";
    for (size_t i = 0; i < threadCounts.size(); ++i) {
        oss << "  " << threadCounts[i] << " threads: " << (efficiencies[i] * 100) << "%\n";
    }
    
    // Calculate scaling efficiency
    oss << "\nScaling Efficiency:\n";
    for (size_t i = 1; i < threadCounts.size(); ++i) {
        double efficiency = efficiencies[i] / efficiencies[i-1];
        oss << "  From " << threadCounts[i-1] << " to " << threadCounts[i] << " threads: " << 
            (efficiency * 100) << "%\n";
    }
    
    return oss.str();
}

bool ParallelPerformanceAnalyzer::exportParallelProfile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << exportParallelProfileAsString();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::string ParallelPerformanceAnalyzer::exportParallelProfileAsString() const {
    std::ostringstream oss;
    oss << "Parallel Performance Profile\n";
    oss << "==========================\n\n";
    
    oss << "Parallel Analysis Summary:\n";
    oss << "  Total Parallel Executions: " << totalExecutions_ << "\n";
    oss << "  Number of Operations: " << getParallelOperations().size() << "\n";
    oss << "  Number of Thread Counts: " << getThreadCounts().size() << "\n\n";
    
    oss << "Thread Counts Analyzed:\n";
    auto threadCounts = getThreadCounts();
    for (size_t threads : threadCounts) {
        oss << "  " << threads << " threads\n";
    }
    
    oss << "\nParallel Operations:\n";
    auto operations = getParallelOperations();
    for (const auto& operation : operations) {
        oss << "  " << operation << "\n";
    }
    
    oss << "\nParallel Efficiency by Operation:\n";
    for (const auto& operation : operations) {
        oss << "  " << operation << ":\n";
        size_t optimalThreads = getOptimalThreadCount(operation).empty() ? 1 : 
                               std::stoull(getOptimalThreadCount(operation).substr(
                                   getOptimalThreadCount(operation).find_last_of(' ') + 1));
        double efficiency = getAverageEfficiency(operation, optimalThreads);
        oss << "    Optimal: " << optimalThreads << " threads (" << (efficiency * 100) << "% efficiency)\n";
    }
    
    return oss.str();
}

std::string ParallelPerformanceAnalyzer::getRealTimeParallelStats() const {
    std::ostringstream oss;
    oss << "Real-Time Parallel Statistics:\n";
    oss << "==========================\n\n";
    
    oss << "Current Parallel Performance:\n";
    oss << "  Total Parallel Executions: " << totalExecutions_ << "\n";
    oss << "  Active Operations: " << getParallelOperations().size() << "\n";
    oss << "  Thread Counts Analyzed: " << getThreadCounts().size() << "\n";
    
    if (!getParallelOperations().empty() && !getThreadCounts().empty()) {
        oss << "\nRecent Parallel Executions:\n";
        std::lock_guard<std::mutex> lock(executionsMutex_);
        
        size_t recentCount = std::min(executions_.size(), size_t(5));
        for (size_t i = 0; i < recentCount; ++i) {
            const auto& execution = executions_[executions_.size() - 1 - i];
            oss << "  " << execution.operation << " with " << execution.threads << " threads: " <<
                (execution.efficiency * 100) << "% efficiency, " <<
                (execution.loadBalance * 100) << "% balance, " <<
                (execution.contention * 100) << "% contention\n";
        }
    }
    
    return oss.str();
}

void ParallelPerformanceAnalyzer::cleanupOldExecutions() {
    // Remove executions older than 24 hours
    auto now = std::chrono::system_clock::now();
    auto oneDayAgo = now - std::chrono::hours(24);
    
    std::lock_guard<std::mutex> lock(executionsMutex_);
    auto it = std::remove_if(executions_.begin(), executions_.end(),
                             [oneDayAgo](const ParallelExecution& e) {
                                 return e.timestamp < oneDayAgo;
                             });
    executions_.erase(it, executions_.end());
}

double ParallelPerformanceAnalyzer::calculateParallelEfficiency(const std::string& operation, size_t threads) const {
    return getAverageEfficiency(operation, threads);
}

double ParallelPerformanceAnalyzer::calculateLoadBalanceScore(const std::string& operation, size_t threads) const {
    return getAverageLoadBalance(operation, threads);
}

double ParallelPerformanceAnalyzer::calculateContentionScore(const std::string& operation, size_t threads) const {
    return getAverageContention(operation, threads);
}

std::string ParallelPerformanceAnalyzer::getThreadScalingDescription(size_t threads) const {
    if (threads == 1) return "Sequential";
    else if (threads == 2) return "Dual-core";
    else if (threads <= 8) return "Small parallel";
    else if (threads <= 32) return "Medium parallel";
    else return "Large parallel";
}

bool ParallelPerformanceAnalyzer::isLoadBalanced(double loadBalance) const {
    return loadBalance > 0.8;
}

bool ParallelPerformanceAnalyzer::isContentionHigh(double contention) const {
    return contention > 0.3;
}

} // namespace nlm
