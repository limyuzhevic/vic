// PerformanceAnalyzer.cpp
#include "PerformanceAnalyzer.cpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cmath>

using json = nlohmann::json;

namespace nlm {

PerformanceAnalyzer::PerformanceAnalyzer() = default;
PerformanceAnalyzer::~PerformanceAnalyzer() = default;

std::vector<PerformanceAnalyzer::BottleneckAnalysis> PerformanceAnalyzer::analyzeBottlenecks(
    const PerformanceProfiler::ProfileResult& profile,
    const TimelineProfiler& timeline,
    const MemoryProfiler& memory,
    const CPUProfiler& cpu) const {
    std::vector<BottleneckAnalysis> bottlenecks;
    
    // Analyze profile data
    double totalTime = profile.totalWallTime;
    if (totalTime <= 0) return bottlenecks;
    
    for (const auto& funcProfile : profile.functionProfiles) {
        if (funcProfile.totalTime > totalTime * 0.3) { // Top 30% of time
            BottleneckAnalysis bottleneck;
            bottleneck.type = "cpu";
            bottleneck.severity = funcProfile.totalTime / totalTime;
            bottleneck.location = funcProfile.functionName;
            bottleneck.description = "Function consuming significant CPU time: " + funcProfile.functionName;
            bottleneck.impact = funcProfile.totalTime / totalTime;
            bottleneck.recommendation = "Consider optimizing this function: profile, reduce calls, use SIMD, or parallelize";
            bottlenecks.push_back(bottleneck);
        }
    }
    
    // Analyze timeline data
    if (!timeline.getEvents().empty()) {
        BottleneckAnalysis bottleneck;
        bottleneck.type = "synchronization";
        bottleneck.severity = 0.2; // Simplified
        bottleneck.location = "main thread";
        bottleneck.description = "Event-driven processing may introduce synchronization overhead";
        bottleneck.impact = 0.2;
        bottleneck.recommendation = "Consider improving synchronization mechanisms or using lock-free data structures";
        bottlenecks.push_back(bottleneck);
    }
    
    // Analyze memory data
    if (memory.detectMemoryLeaks()) {
        BottleneckAnalysis bottleneck;
        bottleneck.type = "memory";
        bottleneck.severity = 0.8;
        bottleneck.location = "memory management";
        bottleneck.description = "Memory leaks detected";
        bottleneck.impact = 0.8;
        bottleneck.recommendation = "Implement proper memory management, use memory pools, and ensure proper deallocation";
        bottlenecks.push_back(bottleneck);
    }
    
    // Analyze CPU data
    auto cpuBottlenecks = cpu.analyzeCPUUsage();
    for (const auto& bottleneck : cpuBottlenecks) {
        BottleneckAnalysis analysis;
        analysis.type = bottleneck.location == "CPU bound" ? "cpu" : "memory";
        analysis.severity = bottleneck.cpuPercentage / 100.0;
        analysis.location = "thread pool";
        analysis.description = bottleneck.description;
        analysis.impact = bottleneck.cpuPercentage / 100.0;
        analysis.recommendation = bottleneck.recommendation;
        bottlenecks.push_back(analysis);
    }
    
    return bottlenecks;
}

PerformanceAnalyzer::ScalabilityAnalysis PerformanceAnalyzer::analyzeScalability(
    const std::vector<BenchmarkRun>& runs) const {
    ScalabilityAnalysis analysis;
    
    if (runs.size() < 2) return analysis;
    
    // Sort by scale (neuron count)
    std::vector<BenchmarkRun> sortedRuns = runs;
    std::sort(sortedRuns.begin(), sortedRuns.end(),
              [](const BenchmarkRun& a, const BenchmarkRun& b) {
                  return a.scale.neurons < b.scale.neurons;
              });
    
    // Calculate scaling efficiency
    double baseNeurons = sortedRuns.front().scale.neurons;
    double baseTime = sortedRuns.front().wallClockTime;
    double basePerformance = baseNeurons / baseTime;
    
    double maxEfficiency = 0;
    std::string worstBottleneck;
    double worstSeverity = 0;
    
    for (size_t i = 1; i < sortedRuns.size(); ++i) {
        const auto& currentRun = sortedRuns[i];
        double scaleFactor = double(currentRun.scale.neurons) / baseNeurons;
        
        if (baseTime > 0 && currentRun.wallClockTime > 0) {
            double actualSpeedup = baseTime / currentRun.wallClockTime;
            double efficiency = actualSpeedup / scaleFactor;
            
            if (efficiency < maxEfficiency) maxEfficiency = efficiency;
            if (efficiency < analysis.scalingEfficiency) analysis.scalingEfficiency = efficiency;
            
            if (efficiency < 0.5) { // Poor efficiency
                if (efficiency < worstSeverity) {
                    worstSeverity = efficiency;
                    worstBottleneck = "inefficient scaling";
                }
            }
        }
    }
    
    analysis.scalingEfficiency = maxEfficiency > 0 ? maxEfficiency : 0;
    analysis.parallelEfficiency = analysis.scalingEfficiency; // Simplified
    
    if (!worstBottleneck.empty()) {
        analysis.identifiedBottleneck = worstBottleneck;
        analysis.bottleneckSeverity = worstSeverity;
    } else {
        analysis.identifiedBottleneck = "none";
        analysis.bottleneckSeverity = 0;
    }
    
    // Generate recommendations
    if (analysis.scalingEfficiency < 0.3) {
        analysis.recommendations.push_back("Poor scaling detected. Consider:");
        analysis.recommendations.push_back("  - Improving cache locality and data layout");
        analysis.recommendations.push_back("  - Reducing synchronization overhead");
        analysis.recommendations.push_back("  - Using more efficient algorithms");
    } else if (analysis.scalingEfficiency < 0.7) {
        analysis.recommendations.push_back("Moderate scaling. Consider optimizations:");
        analysis.recommendations.push_back("  - Optimizing memory access patterns");
        analysis.recommendations.push_back("  - Reducing contention in shared resources");
    } else {
        analysis.recommendations.push_back("Good scaling performance");
        analysis.recommendations.push_back("  - Continue current optimization strategy");
        analysis.recommendations.push_back("  - Consider micro-optimizations");
    }
    
    // Project 10x and 100x performance
    analysis.projectedPerformance10x = analysis.scalingEfficiency * 10;
    analysis.projectedPerformance100x = analysis.scalingEfficiency * 100;
    
    return analysis;
}

PerformanceAnalyzer::LoadTestResult PerformanceAnalyzer::runLoadTest(
    const std::string& testType,
    const std::vector<std::string>& scenarios,
    std::function<std::shared_ptr<class Brain>(uint64_t neurons)> createBrain,
    std::function<bool(class Brain* brain, uint64_t steps)> runSimulation
) const {
    LoadTestResult result;
    result.success = false;
    result.errorMessage = "Load test not implemented";
    
    return result;
}

PerformanceAnalyzer::StressTestResult PerformanceAnalyzer::runStressTest(
    const std::string& stressType,
    uint64_t maxScale,
    std::function<std::shared_ptr<class Brain>(uint64_t neurons)> createBrain,
    std::function<bool(class Brain* brain, uint64_t steps)> runSimulation
) const {
    StressTestResult result;
    result.testPassed = false;
    result.failureMode = "not implemented";
    
    return result;
}

std::vector<PerformanceAnalyzer::RegressionResult> PerformanceAnalyzer::detectRegression(
    const std::vector<BenchmarkRun>& baseline,
    const std::vector<BenchmarkRun>& current
) const {
    std::vector<RegressionResult> regressions;
    
    if (baseline.empty() || current.empty()) return regressions;
    
    // Compare key metrics for each scale
    for (const auto& currRun : current) {
        // Find corresponding baseline run (same scale)
        auto baselineRun = std::find_if(baseline.begin(), baseline.end(),
            [&currRun](const BenchmarkRun& run) {
                return run.scale.name == currRun.scale.name;
            });
        
        if (baselineRun != baseline.end()) {
            // Compare performance metrics
            double baselineTime = baselineRun->wallClockTime;
            double currentTime = currRun.wallClockTime;
            
            if (baselineTime > 0) {
                double regression = (currentTime - baselineTime) / baselineTime;
                
                RegressionResult reg;
                reg.metric = "performance_time";
                reg.baselineValue = baselineTime;
                reg.currentValue = currentTime;
                reg.regressionPercentage = regression * 100;
                reg.regressionDetected = regression > 0.05; // 5% regression threshold
                reg.severity = reg.regressionDetected ? (regression > 0.2 ? "high" : "medium") : "low";
                reg.recommendation = "Investigate regression causes: check for memory leaks, CPU bottlenecks, or algorithmic changes";
                
                regressions.push_back(reg);
            }
        }
    }
    
    return regressions;
}

std::vector<PerformanceAnalyzer::OptimizationRecommendation> PerformanceAnalyzer::generateRecommendations(
    const std::vector<BottleneckAnalysis>& bottlenecks,
    const PerformanceProfiler::ProfileResult& profile,
    const TimelineProfiler& timeline,
    const MemoryProfiler& memory,
    const CPUProfiler& cpu,
    const ThreadProfiler& threads
) const {
    std::vector<OptimizationRecommendation> recommendations;
    
    // Analyze bottlenecks and generate recommendations
    for (const auto& bottleneck : bottlenecks) {
        OptimizationRecommendation rec;
        
        if (bottleneck.type == "cpu") {
            rec.category = "cpu";
            rec.title = "CPU Optimization";
            rec.description = "Optimize CPU-bound functions to improve performance";
            rec.expectedImprovement = bottleneck.severity * 100;
            rec.implementationCost = bottleneck.severity > 0.5 ? "high" : "medium";
            rec.risk = bottleneck.severity > 0.7 ? "high" : "low";
            rec.implementationSteps = {
                "Profile the CPU-intensive functions",
                "Identify hotspots using performance profilers",
                "Optimize hot code paths",
                "Consider SIMD vectorization",
                "Implement parallelization where possible"
            };
        } else if (bottleneck.type == "memory") {
            rec.category = "memory";
            rec.title = "Memory Optimization";
            rec.description = "Reduce memory usage and improve memory efficiency";
            rec.expectedImprovement = bottleneck.severity * 100;
            rec.implementationCost = bottleneck.severity > 0.6 ? "high" : "medium";
            rec.risk = bottleneck.severity > 0.8 ? "high" : "low";
            rec.implementationSteps = {
                "Implement memory pools",
                "Use cache-friendly data structures",
                "Reduce dynamic allocations",
                "Optimize memory access patterns",
                "Consider custom allocators"
            };
        } else if (bottleneck.type == "synchronization") {
            rec.category = "synchronization";
            rec.title = "Synchronization Optimization";
            rec.description = "Reduce synchronization overhead and contention";
            rec.expectedImprovement = bottleneck.severity * 80;
            rec.implementationCost = "medium";
            rec.risk = "medium";
            rec.implementationSteps = {
                "Identify synchronization bottlenecks",
                "Use lock-free data structures",
                "Reduce critical section sizes",
                "Implement thread-local storage",
                "Consider work-stealing algorithms"
            };
        }
        
        recommendations.push_back(rec);
    }
    
    return recommendations;
}

std::string PerformanceAnalyzer::generateAnalysisReport(
    const std::vector<BottleneckAnalysis>& bottlenecks,
    const ScalabilityAnalysis& scalability,
    const std::vector<RegressionResult>& regressions,
    const std::vector<OptimizationRecommendation>& recommendations
) const {
    std::stringstream ss;
    ss << "Performance Analysis Report\n";
    ss << "==========================\n\n";
    
    ss << "=== Performance Bottlenecks ===\n";
    if (bottlenecks.empty()) {
        ss << "No significant bottlenecks detected.\n\n";
    } else {
        for (const auto& bottleneck : bottlenecks) {
            ss << "Type: " << bottleneck.type << "\n";
            ss << "  Severity: " << bottleneck.severity * 100 << "%\n";
            ss << "  Location: " << bottleneck.location << "\n";
            ss << "  Impact: " << bottleneck.impact * 100 << "%\n";
            ss << "  Description: " << bottleneck.description << "\n";
            ss << "  Recommendation: " << bottleneck.recommendation << "\n\n";
        }
    }
    
    ss << "=== Scalability Analysis ===\n";
    ss << "Scaling efficiency: " << scalability.scalingEfficiency * 100 << "%\n";
    ss << "Parallel efficiency: " << scalability.parallelEfficiency * 100 << "%\n";
    ss << "Identified bottleneck: " << scalability.identifiedBottleneck << "\n";
    ss << "Bottleneck severity: " << scalability.bottleneckSeverity * 100 << "%\n\n";
    
    for (const auto& recommendation : scalability.recommendations) {
        ss << "  - " << recommendation << "\n";
    }
    ss << "\n";
    
    ss << "Projected performance: 10x: " << scalability.projectedPerformance10x << "%, 100x: " << scalability.projectedPerformance100x << "%\n\n";
    
    ss << "=== Performance Regression Analysis ===\n";
    if (regressions.empty()) {
        ss << "No significant performance regressions detected.\n\n";
    } else {
        for (const auto& regression : regressions) {
            ss << "Metric: " << regression.metric << "\n";
            ss << "  Baseline: " << regression.baselineValue << "\n";
            ss << "  Current: " << regression.currentValue << "\n";
            ss << "  Regression: " << regression.regressionPercentage << "%\n";
            ss << "  Severity: " << regression.severity << "\n";
            ss << "  Recommendation: " << regression.recommendation << "\n\n";
        }
    }
    
    ss << "=== Optimization Recommendations ===\n";
    if (recommendations.empty()) {
        ss << "No optimization recommendations generated.\n\n";
    } else {
        for (const auto& recommendation : recommendations) {
            ss << "Category: " << recommendation.category << "\n";
            ss << "  Title: " << recommendation.title << "\n";
            ss << "  Description: " << recommendation.description << "\n";
            ss << "  Expected improvement: " << recommendation.expectedImprovement << "%\n";
            ss << "  Implementation cost: " << recommendation.implementationCost << "\n";
            ss << "  Risk: " << recommendation.risk << "\n";
            ss << "  Steps:\n";
            for (const auto& step : recommendation.implementationSteps) {
                ss << "    - " << step << "\n";
            }
            ss << "\n";
        }
    }
    
    return ss.str();
}

} // namespace nlm
