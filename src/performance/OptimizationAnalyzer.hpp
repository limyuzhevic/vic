#pragma once

/**
 * Performance Analysis Tools for NLM Brain
 * 
 * Tools for identifying optimization opportunities and analyzing performance bottlenecks
 */

#include "PerformanceProfiler.hpp"
#include <algorithm>
#include <sstream>
#include <unordered_set>

namespace nlm {

OptimizationAnalyzer::OptimizationOpportunity OptimizationAnalyzer::analyzeMemoryAllocation(
    const std::unordered_map<std::string, size_t>& allocCounts,
    const std::unordered_map<std::string, size_t>& deallocCounts) const {
    OptimizationOpportunity opportunity;
    opportunity.optimizationType = "memory_allocation";
    opportunity.potentialImprovement = 0.0;
    opportunity.currentCost = 0.0;
    opportunity.description = "Analyze memory allocation patterns";
    opportunity.implementationComplexity = "medium";
    opportunity.implementationSteps = {
        "Profile memory allocation sites",
        "Identify high-frequency allocations",
        "Implement memory pools",
        "Reduce heap fragmentation"
    };
    opportunity.risks = {
        "Potential memory leaks",
        "Increased memory usage",
        "Complex integration"
    };
    
    // Calculate potential improvement
    size_t totalAllocs = 0, totalDeallocs = 0;
    for (const auto& pair : allocCounts) totalAllocs += pair.second;
    for (const auto& pair : deallocCounts) totalDeallocs += pair.second;
    
    if (totalAllocs > 0) {
        opportunity.currentCost = static_cast<double>(totalAllocs) / 1000.0;
        opportunity.potentialImprovement = std::min(1.0, opportunity.currentCost / 100.0);
    }
    
    return opportunity;
}

OptimizationAnalyzer::OptimizationOpportunity OptimizationAnalyzer::analyzeCacheUsage(
    const OperationProfiler::ProfilingData* data) const {
    OptimizationOpportunity opportunity;
    opportunity.optimizationType = "cache_usage";
    opportunity.potentialImprovement = 0.0;
    opportunity.currentCost = 0.0;
    opportunity.description = "Improve cache locality";
    opportunity.implementationComplexity = "high";
    opportunity.implementationSteps = {
        "Convert to Structure-of-Arrays (SoA)",
        "Optimize data layout",
        "Align data to cache lines",
        "Use SIMD operations"
    };
    opportunity.risks = {
        "Increased code complexity",
        "Memory overhead",
        "Implementation time"
    };
    
    if (data) {
        opportunity.currentCost = data->avgTimeMicroseconds;
        opportunity.potentialImprovement = std::min(1.0, opportunity.currentCost / 1000.0);
    }
    
    return opportunity;
}

OptimizationAnalyzer::OptimizationOpportunity OptimizationAnalyzer::analyzeThreadContention(
    const PerformanceMetricsCollector::BrainMetrics& metrics) const {
    OptimizationOpportunity opportunity;
    opportunity.optimizationType = "thread_contention";
    opportunity.potentialImprovement = 0.0;
    opportunity.currentCost = 0.0;
    opportunity.description = "Reduce thread contention";
    opportunity.implementationComplexity = "medium";
    opportunity.implementationSteps = {
        "Reduce lock scope",
        "Use lock-free data structures",
        "Improve load balancing",
        "Reduce shared resource access"
    };
    opportunity.risks = {
        "Race conditions",
        "Deadlocks",
        "Complex synchronization"
    };
    
    if (metrics.threadCount > 0 && metrics.activeThreads < metrics.threadCount * 0.5) {
        opportunity.currentCost = 1.0 - (static_cast<double>(metrics.activeThreads) / metrics.threadCount);
        opportunity.potentialImprovement = opportunity.currentCost * 0.8;
    }
    
    return opportunity;
}

std::string OptimizationAnalyzer::generateOptimizationReport(
    const std::vector<OptimizationOpportunity>& opportunities) const {
    std::stringstream ss;
    ss << "=== Optimization Analysis Report ===\n";
    ss << "Total optimization opportunities: " << opportunities.size() << "\n\n";
    
    // Group opportunities by type
    std::unordered_map<std::string, std::vector<OptimizationOpportunity>> opportunitiesByType;
    for (const auto& opportunity : opportunities) {
        opportunitiesByType[opportunity.optimizationType].push_back(opportunity);
    }
    
    // Report by type
    for (const auto& pair : opportunitiesByType) {
        ss << "=== " << pair.first << " ===\n";
        for (const auto& opportunity : pair.second) {
            ss << "Opportunity: " << opportunity.description << "\n";
            ss << "  Potential improvement: " << (opportunity.potentialImprovement * 100) << "%\n";
            ss << "  Implementation complexity: " << opportunity.implementationComplexity << "\n";
            ss << "  Current cost: " << opportunity.currentCost << "\n";
            ss << "  Risks: ";
            for (size_t i = 0; i < opportunity.risks.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << opportunity.risks[i];
            }
            ss << "\n\n";
        }
    }
    
    // Get prioritized opportunities
    std::vector<size_t> priorities = prioritizeOpportunities(opportunities);
    ss << "=== Prioritized Optimization Opportunities ===\n";
    for (size_t i = 0; i < std::min(size_t(5), priorities.size()); ++i) {
        ss << i + 1 << ". " << opportunities[priorities[i]].description << "\n";
    }
    
    return ss.str();
}

std::vector<size_t> OptimizationAnalyzer::prioritizeOpportunities(
    const std::vector<OptimizationOpportunity>& opportunities) const {
    std::vector<size_t> indices;
    indices.reserve(opportunities.size());
    for (size_t i = 0; i < opportunities.size(); ++i) {
        indices.push_back(i);
    }
    
    // Sort by potential improvement * (1 - complexity weight)
    std::sort(indices.begin(), indices.end(),
              [&](size_t a, size_t b) {
                  double complexityA = (opportunities[a].implementationComplexity == "low" ? 0.3 :
                                      opportunities[a].implementationComplexity == "medium" ? 0.6 : 0.9);
                  double complexityB = (opportunities[b].implementationComplexity == "low" ? 0.3 :
                                      opportunities[b].implementationComplexity == "medium" ? 0.6 : 0.9);
                  
                  double scoreA = opportunities[a].potentialImprovement * (1.0 - complexityA);
                  double scoreB = opportunities[b].potentialImprovement * (1.0 - complexityB);
                  
                  return scoreA > scoreB;
              });
    
    return indices;
}

std::vector<std::string> OptimizationAnalyzer::getRecommendations(
    const PerformanceMetricsCollector::BottleneckAnalysis& bottleneck) const {
    return bottleneck.recommendations;
}

} // namespace nlm

#endif // NLM_OPTIMIZATION_ANALYZER_HPP