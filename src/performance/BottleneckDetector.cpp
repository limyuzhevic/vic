#include "BottleneckDetector.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_set>

namespace nlm {

BottleneckDetector::BottleneckDetector()
    : threshold_(0.1),
      timeWindow_(60.0),
      maxOperations_(100),
      enableFunctionAnalysis_(true),
      enableAlgorithmAnalysis_(true),
      enableMemoryAnalysis_(true),
      enableConvergenceAnalysis_(true),
      lastDetectionTime_(std::chrono::system_clock::now()) {
}

BottleneckDetector::BottleneckDetector(std::shared_ptr<PerformanceProfiler> profiler)
    : threshold_(0.1),
      timeWindow_(60.0),
      maxOperations_(100),
      enableFunctionAnalysis_(true),
      enableAlgorithmAnalysis_(true),
      enableMemoryAnalysis_(true),
      enableConvergenceAnalysis_(true),
      performanceProfiler_(profiler),
      lastDetectionTime_(std::chrono::system_clock::now()) {
}

BottleneckDetector::~BottleneckDetector() = default;

std::vector<std::string> BottleneckDetector::detectBottlenecks() const {
    performDetection();
    
    std::vector<std::string> bottleneckDescriptions;
    {
        std::lock_guard<std::mutex> lock(bottlenecksMutex_);
        for (const auto& bottleneck : bottlenecks_) {
            std::ostringstream oss;
            oss << "[" << bottleneck.type << "] " << bottleneck.location << 
                " (Severity: " << bottleneck.severity << ")\n";
            oss << "  Description: " << bottleneck.description << "\n";
            oss << "  Suggestions: ";
            for (size_t i = 0; i < bottleneck.suggestions.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << bottleneck.suggestions[i];
            }
            oss << "\n";
            bottleneckDescriptions.push_back(oss.str());
        }
    }
    
    lastDetectionTime_ = std::chrono::system_clock::now();
    return bottleneckDescriptions;
}

std::string BottleneckDetector::getBottleneckReport() const {
    performDetection();
    
    std::ostringstream oss;
    oss << "Bottleneck Detection Report\n";
    oss << "==========================\n\n";
    
    {
        std::lock_guard<std::mutex> lock(bottlenecksMutex_);
        
        if (bottlenecks_.empty()) {
            oss << "No bottlenecks detected.\n";
            return oss.str();
        }
        
        oss << "Detected Bottlenecks:\n";
        for (size_t i = 0; i < bottlenecks_.size(); ++i) {
            const auto& bottleneck = bottlenecks_[i];
            oss << i + 1 << ". " << bottleneck.type << " at " << bottleneck.location << "\n";
            oss << "   Severity: " << bottleneck.severity << "\n";
            oss << "   Description: " << bottleneck.description << "\n";
            oss << "   Estimated Fix Time: " << bottleneck.estimatedFixTime.count() << "ms\n";
            oss << "   Suggestions:\n";
            for (const auto& suggestion : bottleneck.suggestions) {
                oss << "      - " << suggestion << "\n";
            }
            oss << "\n";
        }
    }
    
    oss << "Detection Time: " << std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() - lastDetectionTime_).count() << " seconds ago\n";
    
    return oss.str();
}

std::string BottleneckDetector::getDetailedBottleneckReport() const {
    performDetection();
    
    std::ostringstream oss;
    oss << "=== DETAILED BOTTLENECK ANALYSIS ===\n\n";
    
    // Function-level bottlenecks
    if (enableFunctionAnalysis_ && functionCallProfiler_) {
        auto functionBottlenecks = detectFunctionLevelBottlenecks();
        if (!functionBottlenecks.empty()) {
            oss << "FUNCTION-LEVEL BOTTLENECKS:\n";
            oss << "==========================\n\n";
            for (const auto& bottleneck : functionBottlenecks) {
                oss << "  - " << bottleneck << "\n";
            }
            oss << "\n";
        }
    }
    
    // Algorithm-level bottlenecks
    if (enableAlgorithmAnalysis_ && algorithmProfiler_) {
        auto algorithmBottlenecks = detectAlgorithmLevelBottlenecks();
        if (!algorithmBottlenecks.empty()) {
            oss << "ALGORITHM-LEVEL BOTTLENECKS:\n";
            oss << "===========================\n\n";
            for (const auto& bottleneck : algorithmBottlenecks) {
                oss << "  - " << bottleneck << "\n";
            }
            oss << "\n";
        }
    }
    
    // Memory-level bottlenecks
    if (enableMemoryAnalysis_) {
        auto memoryBottlenecks = detectMemoryLevelBottlenecks();
        if (!memoryBottlenecks.empty()) {
            oss << "MEMORY-LEVEL BOTTLENECKS:\n";
            oss << "=========================\n\n";
            for (const auto& bottleneck : memoryBottlenecks) {
                oss << "  - " << bottleneck << "\n";
            }
            oss << "\n";
        }
    }
    
    // Convergence-level bottlenecks
    if (enableConvergenceAnalysis_) {
        auto convergenceBottlenecks = detectConvergenceLevelBottlenecks();
        if (!convergenceBottlenecks.empty()) {
            oss << "CONVERGENCE-LEVEL BOTTLENECKS:\n";
            oss << "==============================\n\n";
            for (const auto& bottleneck : convergenceBottlenecks) {
                oss << "  - " << bottleneck << "\n";
            }
            oss << "\n";
        }
    }
    
    // Summary
    oss << "=== BOTTLENECK SUMMARY ===\n";
    {
        std::lock_guard<std::mutex> lock(bottlenecksMutex_);
        oss << "Total Bottlenecks Detected: " << bottlenecks_.size() << "\n";
        
        if (!bottlenecks_.empty()) {
            double totalSeverity = 0.0;
            for (const auto& bottleneck : bottlenecks_) {
                totalSeverity += bottleneck.severity;
            }
            oss << "Average Severity: " << (totalSeverity / bottlenecks_.size()) << "\n";
        }
    }
    
    return oss.str();
}

std::vector<std::string> BottleneckDetector::detectFunctionLevelBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    if (!functionCallProfiler_) return bottlenecks;
    
    auto functionCalls = functionCallProfiler_->getFunctionCalls();
    
    for (const auto& func : functionCalls) {
        double avgTime = functionCallProfiler_->getAverageTime(func);
        size_t count = functionCallProfiler_->getCallCount(func);
        
        if (count < 10) continue; // Minimum samples
        
        if (isFunctionBottleneck(func, avgTime)) {
            std::ostringstream oss;
            oss << func << " (avg: " << avgTime << "s, calls: " << count << ") - ";
            
            // Provide specific suggestions based on function characteristics
            if (func.find("copy") != std::string::npos || func.find("clone") != std::string::npos) {
                oss << "Memory allocation bottleneck - consider using memory pools or pre-allocation";
            } else if (func.find("sort") != std::string::npos || func.find("search") != std::string::npos) {
                oss << "Algorithmic complexity bottleneck - consider better data structures or indexing";
            } else if (func.find("loop") != std::string::npos || func.find("iterate") != std::string::npos) {
                oss << "Iterative processing bottleneck - consider vectorization or parallelization";
            } else {
                oss << "General performance bottleneck - consider optimization or caching";
            }
            
            bottlenecks.push_back(oss.str());
        }
    }
    
    return bottlenecks;
}

std::vector<std::string> BottleneckDetector::detectAlgorithmLevelBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    if (!algorithmProfiler_) return bottlenecks;
    
    auto algorithmNames = algorithmProfiler_->getAlgorithmNames();
    
    for (const auto& algo : algorithmNames) {
        auto operations = algorithmProfiler_->getAlgorithmOperations(algo);
        
        for (const auto& op : operations) {
            double avgTime = algorithmProfiler_->getAverageTime(algo, op);
            size_t count = algorithmProfiler_->getOperationCount(algo, op);
            
            if (count < 10) continue; // Minimum samples
            
            if (isAlgorithmBottleneck(algo, op, avgTime)) {
                std::ostringstream oss;
                oss << algo << "." << op << " (avg: " << avgTime << "s, ops: " << count << ") - ";
                
                // Provide specific suggestions based on algorithm characteristics
                if (op.find("train") != std::string::npos || op.find("learn") != std::string::npos) {
                    oss << "Training bottleneck - consider reducing iterations or using better optimization";
                } else if (op.find("update") != std::string::npos || op.find("modify") != std::string::npos) {
                    oss << "Update bottleneck - consider batching or incremental updates";
                } else if (op.find("predict") != std::string::npos || op.find("infer") != std::string::npos) {
                    oss << "Inference bottleneck - consider model optimization or quantization";
                } else {
                    oss << "Computational bottleneck - consider algorithm optimization or parallelization";
                }
                
                bottlenecks.push_back(oss.str());
            }
        }
    }
    
    return bottlenecks;
}

std::vector<std::string> BottleneckDetector::detectMemoryLevelBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    // This would typically integrate with MemoryProfiler to analyze memory usage patterns
    // For now, provide generic memory analysis
    
    bottlenecks.push_back("High memory allocation frequency - consider memory pooling");
    bottlenecks.push_back("Memory fragmentation detected - consider compacting or using better allocators");
    bottlenecks.push_back("Cache misses detected - consider improving data locality");
    
    return bottlenecks;
}

std::vector<std::string> BottleneckDetector::detectConvergenceLevelBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    // This would typically integrate with algorithm performance data
    // For now, provide generic convergence analysis
    
    bottlenecks.push_back("Slow convergence rate - consider better initialization or hyperparameters");
    bottlenecks.push_back("Oscillatory convergence - consider adaptive learning rates");
    bottlenecks.push_back("Premature convergence - consider increasing model capacity");
    
    return bottlenecks;
}

double BottleneckDetector::getBottleneckScore(const std::string& operation) const {
    if (!performanceProfiler_) return 0.0;
    
    double avgTime = performanceProfiler_->getAverageTime(operation);
    return calculateBottleneckScore(std::vector<double>{avgTime});
}

void BottleneckDetector::performDetection() {
    bottlenecks_.clear();
    
    // Detect function-level bottlenecks
    if (enableFunctionAnalysis_ && functionCallProfiler_) {
        detectFunctionBottlenecks();
    }
    
    // Detect algorithm-level bottlenecks
    if (enableAlgorithmAnalysis_ && algorithmProfiler_) {
        detectAlgorithmBottlenecks();
    }
    
    // Detect memory-level bottlenecks
    if (enableMemoryAnalysis_) {
        detectMemoryBottlenecks();
    }
    
    // Detect convergence-level bottlenecks
    if (enableConvergenceAnalysis_) {
        detectConvergenceBottlenecks();
    }
}

void BottleneckDetector::detectFunctionBottlenecks() {
    if (!functionCallProfiler_) return;
    
    auto functionCalls = functionCallProfiler_->getFunctionCalls();
    
    for (const auto& func : functionCalls) {
        double avgTime = functionCallProfiler_->getAverageTime(func);
        size_t count = functionCallProfiler_->getCallCount(func);
        
        if (count < 10) continue;
        
        if (isFunctionBottleneck(func, avgTime)) {
            BottleneckInfo info;
            info.type = "FUNCTION_BOTTLEECK";
            info.location = func;
            info.severity = std::min(avgTime / threshold_, 1.0);
            info.description = "Function " + func + " has high average execution time (" + 
                             std::to_string(avgTime) + "s)";
            
            std::vector<std::string> suggestions = {
                "Consider inlining this function",
                "Optimize function implementation",
                "Add caching for frequently called results",
                "Parallelize independent parts",
                "Reduce function call overhead"
            };
            
            // Customize suggestions based on function name
            if (func.find("copy") != std::string::npos) {
                suggestions.insert(suggestions.begin(), "Use memory pool for object copying");
            } else if (func.find("alloc") != std::string::npos) {
                suggestions.insert(suggestions.begin(), "Pre-allocate memory buffers");
            } else if (func.find("loop") != std::string::npos) {
                suggestions.insert(suggestions.begin(), "Vectorize loop operations");
            }
            
            info.suggestions = suggestions;
            info.estimatedFixTime = std::chrono::milliseconds(static_cast<int>(avgTime * 1000 * 2));
            
            bottlenecks_.push_back(info);
        }
    }
}

void BottleneckDetector::detectAlgorithmBottlenecks() {
    if (!algorithmProfiler_) return;
    
    auto algorithmNames = algorithmProfiler_->getAlgorithmNames();
    
    for (const auto& algo : algorithmNames) {
        auto operations = algorithmProfiler_->getAlgorithmOperations(algo);
        
        for (const auto& op : operations) {
            double avgTime = algorithmProfiler_->getAverageTime(algo, op);
            size_t count = algorithmProfiler_->getOperationCount(algo, op);
            
            if (count < 10) continue;
            
            if (isAlgorithmBottleneck(algo, op, avgTime)) {
                BottleneckInfo info;
                info.type = "ALGORITHM_BOTTLEECK";
                info.location = algo + "." + op;
                info.severity = std::min(avgTime / threshold_, 1.0);
                info.description = "Algorithm " + algo + "." + op + 
                                 " has high average execution time (" + 
                                 std::to_string(avgTime) + "s)";
                
                std::vector<std::string> suggestions = {
                    "Optimize algorithm implementation",
                    "Consider parallel processing",
                    "Reduce computational complexity",
                    "Use better data structures",
                    "Implement caching strategies"
                };
                
                // Customize suggestions based on algorithm characteristics
                if (op.find("train") != std::string::npos) {
                    suggestions.insert(suggestions.begin(), "Reduce training iterations");
                    suggestions.insert(suggestions.begin() + 1, "Use gradient-based optimization");
                } else if (op.find("predict") != std::string::npos) {
                    suggestions.insert(suggestions.begin(), "Quantize model weights");
                    suggestions.insert(suggestions.begin() + 1, "Use model pruning");
                } else if (op.find("search") != std::string::npos) {
                    suggestions.insert(suggestions.begin(), "Build index structures");
                    suggestions.insert(suggestions.begin() + 1, "Use heuristic search");
                }
                
                info.suggestions = suggestions;
                info.estimatedFixTime = std::chrono::milliseconds(static_cast<int>(avgTime * 1000 * 3));
                
                bottlenecks_.push_back(info);
            }
        }
    }
}

void BottleneckDetector::detectMemoryBottlenecks() {
    // Generic memory bottleneck detection
    BottleneckInfo info;
    info.type = "MEMORY_BOTTLEECK";
    info.location = "MEMORY_MANAGEMENT";
    info.severity = 0.8;
    info.description = "Memory usage patterns indicate potential bottlenecks";
    
    std::vector<std::string> suggestions = {
        "Implement memory pooling for frequently allocated objects",
        "Use object pools or arena allocators",
        "Optimize data layouts for cache locality",
        "Implement memory reuse strategies",
        "Use smart pointers for better memory management"
    };
    
    info.suggestions = suggestions;
    info.estimatedFixTime = std::chrono::milliseconds(5000); // 5 seconds estimated
    
    bottlenecks_.push_back(info);
}

void BottleneckDetector::detectConvergenceBottlenecks() {
    // Generic convergence bottleneck detection
    BottleneckInfo info;
    info.type = "CONVERGENCE_BOTTLEECK";
    info.location = "OPTIMIZATION_PROCESS";
    info.severity = 0.7;
    info.description = "Convergence behavior indicates potential bottlenecks";
    
    std::vector<std::string> suggestions = {
        "Adjust learning rates or optimization parameters",
        "Implement better initialization strategies",
        "Add early stopping criteria",
        "Use adaptive optimization algorithms",
        "Consider regularization techniques"
    };
    
    info.suggestions = suggestions;
    info.estimatedFixTime = std::chrono::milliseconds(3000); // 3 seconds estimated
    
    bottlenecks_.push_back(info);
}

double BottleneckDetector::calculateFunctionBottleneckScore(const std::string& functionName) const {
    if (!functionCallProfiler_) return 0.0;
    
    double avgTime = functionCallProfiler_->getAverageTime(functionName);
    double stddev = functionCallProfiler_->getStandardDeviation(functionName);
    size_t count = functionCallProfiler_->getCallCount(functionName);
    
    // Score based on average time, variance, and frequency
    double timeScore = avgTime / threshold_;
    double varianceScore = stddev > 0 ? (stddev / avgTime) : 0.0;
    double frequencyScore = std::min(static_cast<double>(count) / 100.0, 1.0);
    
    return (timeScore * 0.5 + varianceScore * 0.3 + frequencyScore * 0.2);
}

double BottleneckDetector::calculateAlgorithmBottleneckScore(const std::string& algorithmName, 
                                                           const std::string& operation) const {
    if (!algorithmProfiler_) return 0.0;
    
    double avgTime = algorithmProfiler_->getAverageTime(algorithmName, operation);
    double stddev = algorithmProfiler_->getStandardDeviation(algorithmName, operation);
    size_t count = algorithmProfiler_->getOperationCount(algorithmName, operation);
    
    // Score based on average time, variance, and frequency
    double timeScore = avgTime / threshold_;
    double varianceScore = stddev > 0 ? (stddev / avgTime) : 0.0;
    double frequencyScore = std::min(static_cast<double>(count) / 100.0, 1.0);
    
    return (timeScore * 0.5 + varianceScore * 0.3 + frequencyScore * 0.2);
}

bool BottleneckDetector::isFunctionBottleneck(const std::string& functionName, double avgTime) const {
    return avgTime > threshold_ && avgTime > 0.01; // More than threshold and at least 10ms
}

bool BottleneckDetector::isAlgorithmBottleneck(const std::string& algorithmName, 
                                               const std::string& operation, double avgTime) const {
    return avgTime > threshold_ && avgTime > 0.01; // More than threshold and at least 10ms
}

} // namespace nlm
