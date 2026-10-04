#include "OptimizationRecommender.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_set>

namespace nlm {

OptimizationRecommender::OptimizationRecommender()
    : enableRecommendationGeneration_(true),
      minRecommendationConfidence_(0.7),
      maxRecommendations_(10),
      totalRecommendations_(0) {
}

OptimizationRecommender::~OptimizationRecommender() = default;

std::vector<std::string> OptimizationRecommender::generateRecommendations() const {
    std::lock_guard<std::mutex> lock(recommendationsMutex_);
    
    std::vector<std::string> recommendations;
    
    if (!enableRecommendationGeneration_) {
        return recommendations;
    }
    
    // Generate different types of recommendations
    auto memoryRecs = generateMemoryRecommendations();
    auto parallelRecs = generateParallelRecommendations();
    auto vectorizationRecs = generateVectorizationRecommendations();
    auto cacheRecs = generateCacheRecommendations();
    auto algorithmRecs = generateAlgorithmRecommendations();
    auto resourceRecs = generateResourceRecommendations();
    auto scalingRecs = generateScalingRecommendations();
    
    // Combine all recommendations
    recommendations.insert(recommendations.end(), memoryRecs.begin(), memoryRecs.end());
    recommendations.insert(recommendations.end(), parallelRecs.begin(), parallelRecs.end());
    recommendations.insert(recommendations.end(), vectorizationRecs.begin(), vectorizationRecs.end());
    recommendations.insert(recommendations.end(), cacheRecs.begin(), cacheRecs.end());
    recommendations.insert(recommendations.end(), algorithmRecs.begin(), algorithmRecs.end());
    recommendations.insert(recommendations.end(), resourceRecs.begin(), resourceRecs.end());
    recommendations.insert(recommendations.end(), scalingRecs.begin(), scalingRecs.end());
    
    // Add integration recommendations
    auto integrationRecs = integrateWithNLMFeatures();
    recommendations.insert(recommendations.end(), integrationRecs.begin(), integrationRecs.end());
    
    // Sort by priority
    std::sort(recommendations.begin(), recommendations.end(),
              [this](const std::string& a, const std::string& b) {
                  return getRecommendationPriority(a) > getRecommendationPriority(b);
              });
    
    // Limit number of recommendations
    if (recommendations.size() > maxRecommendations_) {
        recommendations.resize(maxRecommendations_);
    }
    
    totalRecommendations_ += recommendations.size();
    
    // Cache recent recommendations
    recentRecommendations_ = recommendations;
    
    return recommendations;
}

std::string OptimizationRecommender::generateOptimizationReport() const {
    std::ostringstream oss;
    oss << "=== OPTIMIZATION RECOMMENDATIONS REPORT ===\n\n";
    
    auto recommendations = generateRecommendations();
    
    oss << "Generated Recommendations: " << recommendations.size() << "\n\n";
    
    // Categorize recommendations
    std::unordered_map<std::string, std::vector<std::string>> categorized;
    
    for (const auto& recommendation : recommendations) {
        std::string category = categorizeRecommendation(recommendation);
        categorized[category].push_back(recommendation);
    }
    
    // Report by category
    oss << "Recommendations by Category:\n";
    for (const auto& [category, recs] : categorized) {
        oss << "  " << category << ": " << recs.size() << " recommendations\n";
    }
    oss << "\n";
    
    // Detailed recommendations by priority
    oss << "Priority Recommendations:\n";
    oss << generatePriorityRecommendations();
    
    // Integration status
    oss << "\nIntegration with NLM Features:\n";
    oss << integrateWithNLMFeatures();
    
    // Performance impact analysis
    oss << "\nPerformance Impact Analysis:\n";
    auto impactRecs = generatePerformanceImpactRecommendations();
    for (const auto& rec : impactRecs) {
        oss << "  " << rec << "\n";
    }
    
    return oss.str();
}

std::string OptimizationRecommender::generatePriorityRecommendations() const {
    std::ostringstream oss;
    
    auto recommendations = generateRecommendations();
    
    oss << "HIGH PRIORITY:\n";
    for (const auto& recommendation : recommendations) {
        if (getRecommendationPriority(recommendation) == "HIGH") {
            oss << "  " << recommendation << "\n";
        }
    }
    
    oss << "\nMEDIUM PRIORITY:\n";
    for (const auto& recommendation : recommendations) {
        if (getRecommendationPriority(recommendation) == "MEDIUM") {
            oss << "  " << recommendation << "\n";
        }
    }
    
    oss << "\nLOW PRIORITY:\n";
    for (const auto& recommendation : recommendations) {
        if (getRecommendationPriority(recommendation) == "LOW") {
            oss << "  " << recommendation << "\n";
        }
    }
    
    return oss.str();
}

std::vector<std::string> OptimizationRecommender::generateMemoryRecommendations() const {
    std::vector<std::string> recommendations;
    
    if (memoryProfiler_) {
        auto leaks = memoryProfiler_->detectMemoryLeaks();
        if (!leaks.empty()) {
            recommendations.push_back("CRITICAL: Memory leaks detected - implement memory pooling or use smart pointers");
        }
        
        size_t currentUsage = memoryProfiler_->getCurrentUsage();
        if (currentUsage > memoryProfiler_->getLeakedAllocationCount() * 1024) { // Heuristic
            recommendations.push_back("HIGH: High memory usage detected - consider reducing object allocations");
        }
        
        recommendations.push_back("MEDIUM: Optimize memory allocation patterns for better cache locality");
    }
    
    recommendations.push_back("Consider using memory pools for frequently allocated objects");
    recommendations.push_back("Implement memory reuse strategies to reduce allocation overhead");
    recommendations.push_back("Use contiguous memory layouts for better spatial locality");
    
    return recommendations;
}

std::vector<std::string> OptimizationRecommender::generateParallelRecommendations() const {
    std::vector<std::string> recommendations;
    
    if (parallelPerformanceAnalyzer_) {
        std::string optimalThreads = parallelPerformanceAnalyzer_->getOptimalThreadCount("main_operation");
        if (!optimalThreads.empty()) {
            recommendations.push_back("OPTIMAL: Use " + optimalThreads + " threads for optimal parallel efficiency");
        }
        
        auto bottlenecks = parallelPerformanceAnalyzer_->identifyParallelBottlenecks();
        if (!bottlenecks.empty()) {
            recommendations.push_back("CRITICAL: Parallel bottlenecks detected - analyze and fix");
        }
    }
    
    recommendations.push_back("HIGH: Implement work-stealing for better load balancing");
    recommendations.push_back("MEDIUM: Use thread-local storage for thread-safe operations");
    recommendations.push_back("LOW: Consider using lock-free data structures where applicable");
    
    return recommendations;
}

std::vector<std::string> OptimizationRecommender::generateVectorizationRecommendations() const {
    std::vector<std::string> recommendations;
    
    if (vectorizationAnalyzer_) {
        std::string optimalWidth = vectorizationAnalyzer_->getOptimalVectorWidth("main_operation");
        if (!optimalWidth.empty()) {
            recommendations.push_back("OPTIMAL: Use " + optimalWidth + "-wide vectorization for maximum performance");
        }
        
        auto bottlenecks = vectorizationAnalyzer_->identifyVectorizationBottlenecks();
        if (!bottlenecks.empty()) {
            recommendations.push_back("CRITICAL: Vectorization bottlenecks detected - optimize memory access");
        }
    }
    
    recommendations.push_back("HIGH: Use SIMD instructions for numerical computations");
    recommendations.push_back("MEDIUM: Implement data alignment for better vector performance");
    recommendations.push_back("LOW: Consider using compiler auto-vectorization directives");
    
    return recommendations;
}

std::vector<std::string> OptimizationRecommender::generateCacheRecommendations() const {
    std::vector<std::string> recommendations;
    
    if (cacheAnalyzer_) {
        std::string optimalConfig = cacheAnalyzer_->getOptimalCacheConfiguration("main_operation");
        if (!optimalConfig.empty()) {
            recommendations.push_back("OPTIMAL: Use " + optimalConfig + " for optimal cache performance");
        }
        
        auto bottlenecks = cacheAnalyzer_->identifyCacheBottlenecks();
        if (!bottlenecks.empty()) {
            recommendations.push_back("CRITICAL: Cache bottlenecks detected - optimize data layout");
        }
    }
    
    recommendations.push_back("HIGH: Implement blocking/tiling for better cache utilization");
    recommendations.push_back("MEDIUM: Use loop interchange to improve cache locality");
    recommendations.push_back("LOW: Consider cache-friendly data structures");
    
    return recommendations;
}

std::vector<std::string> OptimizationRecommender::generateAlgorithmRecommendations() const {
    std::vector<std::string> recommendations;
    
    if (algorithmProfiler_) {
        recommendations.push_back("ANALYZE: Profile algorithm performance for optimization opportunities");
        recommendations.push_back("OPTIMAL: Use more efficient algorithms where applicable");
    }
    
    recommendations.push_back("HIGH: Implement lookup tables for expensive computations");
    recommendations.push_back("MEDIUM: Use approximation algorithms where precision allows");
    recommendations.push_back("LOW: Consider algorithmic complexity improvements");
    
    return recommendations;
}

std::vector<std::string> OptimizationRecommender::generateResourceRecommendations() const {
    std::vector<std::string> recommendations;
    
    if (resourceMonitor_) {
        if (resourceMonitor_->isResourceUsageHigh()) {
            recommendations.push_back("CRITICAL: Resource usage is high - consider resource optimization");
        }
        
        std::string stats = resourceMonitor_->getRealTimeResourceStats();
        recommendations.push_back("ANALYZE: " + stats.substr(0, 100) + "...");
    }
    
    recommendations.push_back("HIGH: Monitor resource usage during peak operations");
    recommendations.push_back("MEDIUM: Implement resource scaling based on workload");
    recommendations.push_back("LOW: Use resource-efficient algorithms");
    
    return recommendations;
}

std::vector<std::string> OptimizationRecommender::generateScalingRecommendations() const {
    std::vector<std::string> recommendations;
    
    if (scalingAnalyzer_) {
        std::string analysis = scalingAnalyzer_->analyzeScalingBehavior();
        if (analysis.find("POOR SCALING") != std::string::npos) {
            recommendations.push_back("CRITICAL: Poor scaling detected - analyze and fix parallel implementation");
        }
        
        std::string efficiency = scalingAnalyzer_->analyzeEfficiencyAtDifferentScales();
        if (efficiency.find("LOW EFFICIENCY") != std::string::npos) {
            recommendations.push_back("HIGH: Improve parallel efficiency at scale");
        }
    }
    
    recommendations.push_back("HIGH: Implement weak scaling optimization");
    recommendations.push_back("MEDIUM: Use better parallelization strategies");
    recommendations.push_back("LOW: Consider problem decomposition approaches");
    
    return recommendations;
}

std::vector<std::string> OptimizationRecommender::integrateWithNLMFeatures() const {
    std::vector<std::string> recommendations;
    
    recommendations.push_back("INTEGRATE: Connect with NLM SIMD optimization features");
    recommendations.push_back("INTEGRATE: Leverage existing parallel processing infrastructure");
    recommendations.push_back("INTEGRATE: Utilize NLM memory pool allocators");
    recommendations.push_back("INTEGRATE: Apply NLM optimization strategies to profiling data");
    
    return recommendations;
}

std::string OptimizationRecommender::integrateWithVisualizationSystem() const {
    std::ostringstream oss;
    oss << "=== INTEGRATION WITH VISUALIZATION SYSTEM ===\n\n";
    
    oss << "Visualization Integration:\n";
    oss << "  - Connect performance metrics to visualization dashboard\n";
    oss << "  - Visualize optimization recommendations in real-time\n";
    oss << "  - Show performance trends and bottlenecks\n";
    oss << "  - Display resource usage patterns\n";
    oss << "  - Integrate with NLM performance monitoring\n";
    
    return oss.str();
}

std::string OptimizationRecommender::integrateWithAnalysisTools() const {
    std::ostringstream oss;
    oss << "=== INTEGRATION WITH ANALYSIS TOOLS ===\n\n";
    
    oss << "Analysis Tool Integration:\n";
    oss << "  - Feed performance data to analysis tools\n";
    oss << "  - Provide optimization recommendations to analysis system\n";
    oss << "  - Enable cross-analysis correlation\n";
    oss << "  - Support automated performance tuning\n";
    oss << "  - Integrate with NLM performance analysis\n";
    
    return oss.str();
}

std::string OptimizationRecommender::integrateWithDebuggingSystem() const {
    std::ostringstream oss;
    oss << "=== INTEGRATION WITH DEBUGGING SYSTEM ===\n\n";
    
    oss << "Debugging System Integration:\n";
    oss << "  - Provide optimization recommendations during debugging\n";
    oss << "  - Connect performance issues to optimization suggestions\n";
    oss << "  - Enable performance debugging workflows\n";
    oss << "  - Integrate with NLM debugging infrastructure\n";
    
    return oss.str();
}

std::string OptimizationRecommender::generateRealTimeRecommendations() const {
    std::ostringstream oss;
    oss << "=== REAL-TIME OPTIMIZATION RECOMMENDATIONS ===\n\n";
    
    oss << "Real-Time Monitoring:\n";
    oss << "  - Monitor current performance metrics\n";
    oss << "  - Detect emerging bottlenecks\n";
    oss << "  - Provide immediate optimization suggestions\n";
    oss << "  - Track recommendation effectiveness\n";
    
    return oss.str();
}

std::string OptimizationRecommender::generateHistoricalRecommendations() const {
    std::ostringstream oss;
    oss << "=== HISTORICAL OPTIMIZATION RECOMMENDATIONS ===\n\n";
    
    oss << "Historical Analysis:\n";
    oss << "  - Analyze past performance patterns\n";
    oss << "  - Identify recurring optimization opportunities\n";
    oss << "  - Track recommendation success rates\n";
    oss << "  - Learn from historical optimization data\n";
    
    return oss.str();
}

std::string OptimizationRecommender::generateTrendBasedRecommendations() const {
    std::ostringstream oss;
    oss << "=== TREND-BASED OPTIMIZATION RECOMMENDATIONS ===\n\n";
    
    oss << "Trend Analysis:\n";
    oss << "  - Analyze performance trends over time\n";
    oss << "  - Predict future performance issues\n";
    oss << "  - Recommend proactive optimizations\n";
    oss << "  - Adjust recommendations based on trends\n";
    
    return oss.str();
}

std::vector<std::string> OptimizationRecommender::generateRegressionDetectionRecommendations() const {
    std::vector<std::string> recommendations;
    
    recommendations.push_back("REGRESSION DETECTION: Implement automated performance regression detection");
    recommendations.push_back("MONITORING: Track performance metrics over time for regression analysis");
    recommendations.push_back("ALERTING: Set up alerts for performance degradation");
    recommendations.push_back("ANALYSIS: Analyze regression patterns to identify root causes");
    
    return recommendations;
}

std::vector<std::string> OptimizationRecommender::generatePerformanceImpactRecommendations() const {
    std::vector<std::string> recommendations;
    
    recommendations.push_back("IMPACT ANALYSIS: Measure performance impact of code changes");
    recommendations.push_back("COMPARISON: Compare performance across different configurations");
    recommendations.push_back("VALIDATION: Validate optimization effectiveness");
    recommendations.push_back("DOCUMENTATION: Document performance improvements");
    
    return recommendations;
}

std::vector<std::string> OptimizationRecommender::generatePlatformValidationRecommendations() const {
    std::vector<std::string> recommendations;
    
    recommendations.push_back("PLATFORM VALIDATION: Test optimizations across different platforms");
    recommendations.push_back("COMPATIBILITY: Ensure optimizations work across hardware");
    recommendations.push_back("SCALABILITY: Validate optimizations at scale");
    recommendations.push_back("RELIABILITY: Test optimizations in production");
    
    return recommendations;
}

bool OptimizationRecommender::exportRecommendations(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << exportRecommendationsAsString();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::string OptimizationRecommender::exportRecommendationsAsString() const {
    std::ostringstream oss;
    oss << "Optimization Recommendations\n";
    oss << "=========================\n\n";
    
    oss << "Recommendation Summary:\n";
    oss << "  Total Recommendations Generated: " << totalRecommendations_ << "\n";
    oss << "  Recent Recommendations Cached: " << recentRecommendations_.size() << "\n\n";
    
    oss << "Generated Recommendations:\n";
    auto recommendations = generateRecommendations();
    for (size_t i = 0; i < recommendations.size(); ++i) {
        oss << i + 1 << ". " << recommendations[i] << "\n";
    }
    
    return oss.str();
}

std::string OptimizationRecommender::getRecommendationSummary() const {
    std::ostringstream oss;
    oss << "=== OPTIMIZATION RECOMMENDATION SUMMARY ===\n\n";
    
    auto recommendations = generateRecommendations();
    
    oss << "Total Recommendations: " << totalRecommendations_ << "\n";
    oss << "Average Recommendation Confidence: Not calculated\n\n";
    
    // Count by category
    std::unordered_map<std::string, size_t> categoryCounts;
    for (const auto& recommendation : recommendations) {
        std::string category = categorizeRecommendation(recommendation);
        categoryCounts[category]++;
    }
    
    oss << "Recommendations by Category:\n";
    for (const auto& [category, count] : categoryCounts) {
        oss << "  " << category << ": " << count << "\n";
    }
    
    oss << "\nPriority Breakdown:\n";
    size_t highPriority = 0, mediumPriority = 0, lowPriority = 0;
    for (const auto& recommendation : recommendations) {
        std::string priority = getRecommendationPriority(recommendation);
        if (priority == "HIGH") highPriority++;
        else if (priority == "MEDIUM") mediumPriority++;
        else if (priority == "LOW") lowPriority++;
    }
    
    oss << "  High Priority: " << highPriority << "\n";
    oss << "  Medium Priority: " << mediumPriority << "\n";
    oss << "  Low Priority: " << lowPriority << "\n";
    
    return oss.str();
}

void OptimizationRecommender::cleanupOldRecommendations() {
    // Remove old recommendations from cache
    std::lock_guard<std::mutex> lock(recommendationsMutex_);
    recentRecommendations_.clear();
}

double OptimizationRecommender::calculateRecommendationConfidence(const std::string& recommendation) const {
    // Simple confidence calculation based on recommendation content
    double confidence = 0.5; // Default confidence
    
    if (recommendation.find("CRITICAL") != std::string::npos) {
        confidence += 0.3;
    } else if (recommendation.find("OPTIMAL") != std::string::npos) {
        confidence += 0.2;
    }
    
    if (recommendation.find("HIGH") != std::string::npos) {
        confidence += 0.1;
    }
    
    return std::min(confidence, 1.0); // Cap at 1.0
}

bool OptimizationRecommender::isRecommendationValid(const std::string& recommendation) const {
    return !recommendation.empty() && recommendation.length() > 10;
}

std::string OptimizationRecommender::categorizeRecommendation(const std::string& recommendation) const {
    if (recommendation.find("MEMORY") != std::string::npos || recommendation.find("MEMORY") != std::string::npos) {
        return "Memory Optimization";
    } else if (recommendation.find("PARALLEL") != std::string::npos) {
        return "Parallel Optimization";
    } else if (recommendation.find("VECTOR") != std::string::npos) {
        return "Vectorization";
    } else if (recommendation.find("CACHE") != std::string::npos) {
        return "Cache Optimization";
    } else if (recommendation.find("ALGORITHM") != std::string::npos) {
        return "Algorithm Optimization";
    } else if (recommendation.find("RESOURCE") != std::string::npos) {
        return "Resource Management";
    } else if (recommendation.find("SCALING") != std::string::npos) {
        return "Scaling";
    } else {
        return "General Optimization";
    }
}

std::string OptimizationRecommender::getRecommendationPriority(const std::string& recommendation) const {
    if (recommendation.find("CRITICAL") != std::string::npos || 
        recommendation.find("OPTIMAL") != std::string::npos) {
        return "HIGH";
    } else if (recommendation.find("HIGH") != std::string::npos || 
               recommendation.find("MEDIUM") != std::string::npos) {
        return "MEDIUM";
    } else {
        return "LOW";
    }
}

} // namespace nlm
