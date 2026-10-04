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

// OptimizationRecommender: Automated optimization suggestions
class OptimizationRecommender {
public:
    OptimizationRecommender();
    ~OptimizationRecommender();
    
    // Configuration
    void setEnableRecommendationGeneration(bool enable) { enableRecommendationGeneration_ = enable; }
    void setMinRecommendationConfidence(double confidence) { minRecommendationConfidence_ = confidence; }
    void setMaxRecommendations(size_t max) { maxRecommendations_ = max; }
    
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
    void setBottleneckDetector(std::shared_ptr<BottleneckDetector> detector) { 
        bottleneckDetector_ = detector; 
    }
    void setMemoryProfiler(std::shared_ptr<MemoryProfiler> profiler) { 
        memoryProfiler_ = profiler; 
    }
    void setResourceMonitor(std::shared_ptr<ResourceMonitor> monitor) { 
        resourceMonitor_ = monitor; 
    }
    void setScalingAnalyzer(std::shared_ptr<ScalingAnalyzer> analyzer) { 
        scalingAnalyzer_ = analyzer; 
    }
    void setParallelPerformanceAnalyzer(std::shared_ptr<ParallelPerformanceAnalyzer> analyzer) { 
        parallelPerformanceAnalyzer_ = analyzer; 
    }
    void setVectorizationAnalyzer(std::shared_ptr<VectorizationAnalyzer> analyzer) { 
        vectorizationAnalyzer_ = analyzer; 
    }
    void setCacheAnalyzer(std::shared_ptr<CacheAnalyzer> analyzer) { 
        cacheAnalyzer_ = analyzer; 
    }
    void setMemoryAccessPatternAnalyzer(std::shared_ptr<MemoryAccessPatternAnalyzer> analyzer) { 
        memoryAccessPatternAnalyzer_ = analyzer; 
    }
    
    // Recommendation generation
    std::vector<std::string> generateRecommendations() const;
    std::string generateOptimizationReport() const;
    std::string generatePriorityRecommendations() const;
    
    // Specific recommendation types
    std::vector<std::string> generateMemoryRecommendations() const;
    std::vector<std::string> generateParallelRecommendations() const;
    std::vector<std::string> generateVectorizationRecommendations() const;
    std::vector<std::string> generateCacheRecommendations() const;
    std::vector<std::string> generateAlgorithmRecommendations() const;
    std::vector<std::string> generateResourceRecommendations() const;
    std::vector<std::string> generateScalingRecommendations() const;
    
    // Integration with existing NLM optimization features
    std::vector<std::string> integrateWithNLMFeatures() const;
    std::string integrateWithVisualizationSystem() const;
    std::string integrateWithAnalysisTools() const;
    std::string integrateWithDebuggingSystem() const;
    
    // Monitoring features
    std::string generateRealTimeRecommendations() const;
    std::string generateHistoricalRecommendations() const;
    std::string generateTrendBasedRecommendations() const;
    
    // Advanced features
    std::vector<std::string> generateRegressionDetectionRecommendations() const;
    std::vector<std::string> generatePerformanceImpactRecommendations() const;
    std::vector<std::string> generatePlatformValidationRecommendations() const;
    
    // Export
    bool exportRecommendations(const std::string& filepath) const;
    std::string exportRecommendationsAsString() const;
    
    // Statistics
    size_t getTotalRecommendations() const { return totalRecommendations_; }
    std::string getRecommendationSummary() const;
    
private:
    // Helper methods
    void cleanupOldRecommendations();
    double calculateRecommendationConfidence(const std::string& recommendation) const;
    bool isRecommendationValid(const std::string& recommendation) const;
    std::string categorizeRecommendation(const std::string& recommendation) const;
    std::string getRecommendationPriority(const std::string& recommendation) const;
    
    std::shared_ptr<PerformanceProfiler> performanceProfiler_;
    std::shared_ptr<FunctionCallProfiler> functionCallProfiler_;
    std::shared_ptr<AlgorithmProfiler> algorithmProfiler_;
    std::shared_ptr<BottleneckDetector> bottleneckDetector_;
    std::shared_ptr<MemoryProfiler> memoryProfiler_;
    std::shared_ptr<ResourceMonitor> resourceMonitor_;
    std::shared_ptr<ScalingAnalyzer> scalingAnalyzer_;
    std::shared_ptr<ParallelPerformanceAnalyzer> parallelPerformanceAnalyzer_;
    std::shared_ptr<VectorizationAnalyzer> vectorizationAnalyzer_;
    std::shared_ptr<CacheAnalyzer> cacheAnalyzer_;
    std::shared_ptr<MemoryAccessPatternAnalyzer> memoryAccessPatternAnalyzer_;
    
    bool enableRecommendationGeneration_;
    double minRecommendationConfidence_;
    size_t maxRecommendations_;
    size_t totalRecommendations_;
    
    // Recommendation cache
    std::vector<std::string> recentRecommendations_;
    mutable std::mutex recommendationsMutex_;
};

} // namespace nlm
