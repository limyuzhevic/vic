#include "PerformanceOptimizer.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {
namespace profiling {

// PerformanceOptimizer implementation

std::vector<PerformanceOptimizer::OptimizationRecommendation>
PerformanceOptimizer::analyzeBottlenecks(const PerformanceProfiler::PerformanceSnapshot& snapshot) const {
    std::vector<OptimizationRecommendation> recommendations;
    
    // Analyze monitor statistics for bottlenecks
    for (const auto& [type, stats] : snapshot.monitorStats) {
        if (stats.count > 0) {
            OptimizationRecommendation rec;
            rec.id = "bottleneck_" + getMonitorTypeName(type);
            rec.category = "performance";
            
            // Check for high values (indicative of bottlenecks)
            if (type == PerformanceMonitor::MonitorType::SpikeProcessing) {
                if (stats.average > 0.1) {
                    rec.description = "High spike processing time - optimize spike delivery algorithm";
                    rec.expectedImprovement = 0.3; // 30% improvement
                    rec.implementationComplexity = 0.7;
                    rec.riskLevel = 0.3;
                    rec.details["current_average_time"] = std::to_string(stats.average) + "s";
                    rec.details["target_time"] = "0.05s";
                    rec.implementationSteps = {
                        "Profile spike delivery",
                        "Identify slow paths",
                        "Optimize data structures",
                        "Implement vectorization"
                    };
                }
            } else if (type == PerformanceMonitor::MonitorType::PlasticityComputation) {
                if (stats.average > 0.05) {
                    rec.description = "High plasticity computation time - optimize plasticity rules";
                    rec.expectedImprovement = 0.4; // 40% improvement
                    rec.implementationComplexity = 0.8;
                    rec.riskLevel = 0.4;
                    rec.details["current_average_time"] = std::to_string(stats.average) + "s";
                    rec.details["target_time"] = "0.02s";
                    rec.implementationSteps = {
                        "Vectorize plasticity computations",
                        "Optimize eligibility trace updates",
                        "Implement parallel plasticity",
                        "Add caching for frequently used weights"
                    };
                }
            } else if (type == PerformanceMonitor::MonitorType::MemoryUsage) {
                if (stats.average > 100 * 1024 * 1024) { // 100MB
                    rec.description = "High memory usage - optimize memory allocation";
                    rec.expectedImprovement = 0.6; // 60% improvement
                    rec.implementationComplexity = 0.6;
                    rec.riskLevel = 0.2;
                    rec.details["current_average_memory"] = std::to_string(stats.average / (1024 * 1024)) + "MB";
                    rec.details["target_memory"] = "50MB";
                    rec.implementationSteps = {
                        "Implement memory pools",
                        "Optimize object allocation",
                        "Add compression for large arrays",
                        "Implement memory reuse"
                    };
                }
            } else if (type == PerformanceMonitor::MonitorType::SimulationTiming) {
                if (stats.average > 0.1) { // 100ms
                    rec.description = "High simulation step time - optimize simulation loop";
                    rec.expectedImprovement = 0.5; // 50% improvement
                    rec.implementationComplexity = 0.8;
                    rec.riskLevel = 0.4;
                    rec.details["current_average_step_time"] = std::to_string(stats.average) + "s";
                    rec.details["target_step_time"] = "0.05s";
                    rec.implementationSteps = {
                        "Parallelize independent computations",
                        "Optimize data access patterns",
                        "Reduce synchronization overhead",
                        "Implement better caching"
                    };
                }
            }
            
            if (!rec.description.empty()) {
                recommendations.push_back(rec);
            }
        }
    }
    
    return recommendations;
}

std::vector<PerformanceOptimizer::OptimizationRecommendation>
PerformanceOptimizer::analyzeResourceUsage(const PerformanceProfiler::PerformanceSnapshot& snapshot) const {
    std::vector<OptimizationRecommendation> recommendations;
    
    // Check CPU and memory resource usage
    if (snapshot.cpuUsage > 0.8) { // 80%
        OptimizationRecommendation rec;
        rec.id = "cpu_optimization";
        rec.category = "resource";
        rec.description = "High CPU usage - optimize computation efficiency";
        rec.expectedImprovement = 0.3;
        rec.implementationComplexity = 0.7;
        rec.riskLevel = 0.3;
        rec.details["current_cpu_usage"] = std::to_string(snapshot.cpuUsage * 100) + "%";
        rec.details["target_cpu_usage"] = "60%";
        rec.implementationSteps = {
            "Parallelize computations",
            "Optimize hot paths",
            "Reduce unnecessary computations",
            "Implement better algorithmic approaches"
        };
        recommendations.push_back(rec);
    }
    
    if (snapshot.memoryUsage > 512 * 1024 * 1024) { // 512MB
        OptimizationRecommendation rec;
        rec.id = "memory_optimization";
        rec.category = "resource";
        rec.description = "High memory usage - optimize memory management";
        rec.expectedImprovement = 0.6;
        rec.implementationComplexity = 0.6;
        rec.riskLevel = 0.2;
        rec.details["current_memory_usage"] = std::to_string(snapshot.memoryUsage / (1024 * 1024)) + "MB";
        rec.details["target_memory_usage"] = "256MB";
        rec.implementationSteps = {
            "Implement memory pools",
            "Reuse memory allocations",
            "Compress data structures",
            "Optimize data storage"
        };
        recommendations.push_back(rec);
    }
    
    return recommendations;
}

std::vector<PerformanceOptimizer::OptimizationRecommendation>
PerformanceOptimizer::analyzeScalability(const PerformanceProfiler::PerformanceSnapshot& baselineSnapshot,
                                          const PerformanceProfiler::PerformanceSnapshot& currentSnapshot) const {
    std::vector<OptimizationRecommendation> recommendations;
    
    // Compare baseline and current snapshots
    PerformanceProfiler::PerformanceComparison comparison;
    comparison = PerformanceProfiler::PerformanceComparison(); // Simplified
    
    // Check for performance regressions
    if (comparison.overallPerformanceScore < 0.7) {
        OptimizationRecommendation rec;
        rec.id = "scalability_optimization";
        rec.category = "scalability";
        rec.description = "Performance degradation over time - optimize for scalability";
        rec.expectedImprovement = 0.4;
        rec.implementationComplexity = 0.8;
        rec.riskLevel = 0.5;
        rec.details["current_performance_score"] = std::to_string(comparison.overallPerformanceScore);
        rec.details["target_performance_score"] = "0.9";
        rec.implementationSteps = {
            "Profile scalability bottlenecks",
            "Optimize data structures for large datasets",
            "Implement efficient algorithms",
            "Add caching for repeated operations",
            "Optimize memory access patterns"
        };
        recommendations.push_back(rec);
    }
    
    // Check for increasing resource usage
    double baselineCpu = baselineSnapshot.cpuUsage;
    double currentCpu = currentSnapshot.cpuUsage;
    if (currentCpu > baselineCpu * 1.2) { // 20% increase
        OptimizationRecommendation rec;
        rec.id = "cpu_scalability_optimization";
        rec.category = "scalability";
        rec.description = "CPU usage increasing with scale - optimize for efficiency";
        rec.expectedImprovement = 0.3;
        rec.implementationComplexity = 0.7;
        rec.riskLevel = 0.3;
        rec.details["baseline_cpu_usage"] = std::to_string(baselineCpu * 100) + "%";
        rec.details["current_cpu_usage"] = std::to_string(currentCpu * 100) + "%";
        rec.details["target_cpu_usage"] = std::to_string(baselineCpu * 100 * 0.8) + "%";
        rec.implementationSteps = {
            "Optimize algorithms for scale",
            "Implement parallel processing",
            "Reduce synchronization overhead",
            "Optimize memory locality"
        };
        recommendations.push_back(rec);
    }
    
    double baselineMemory = baselineSnapshot.memoryUsage;
    double currentMemory = currentSnapshot.memoryUsage;
    if (currentMemory > baselineMemory * 1.2) { // 20% increase
        OptimizationRecommendation rec;
        rec.id = "memory_scalability_optimization";
        rec.category = "scalability";
        rec.description = "Memory usage increasing with scale - optimize memory efficiency";
        rec.expectedImprovement = 0.5;
        rec.implementationComplexity = 0.6;
        rec.riskLevel = 0.2;
        rec.details["baseline_memory_usage"] = std::to_string(baselineMemory / (1024 * 1024)) + "MB";
        rec.details["current_memory_usage"] = std::to_string(currentMemory / (1024 * 1024)) + "MB";
        rec.details["target_memory_usage"] = std::to_string(baselineMemory * 0.8 / (1024 * 1024)) + "MB";
        rec.implementationSteps = {
            "Implement memory-efficient data structures",
            "Add memory pooling",
            "Compress data",
            "Optimize memory access patterns"
        };
        recommendations.push_back(rec);
    }
    
    return recommendations;
}

void PerformanceOptimizer::integrateWithProfiler(PerformanceProfiler& profiler) {
    // TODO: Integrate with PerformanceProfiler
}

std::string PerformanceOptimizer::getMonitorTypeName(PerformanceMonitor::MonitorType type) const {
    switch (type) {
        case PerformanceMonitor::MonitorType::SpikeProcessing: return "spike_processing";
        case PerformanceMonitor::MonitorType::PlasticityComputation: return "plasticity_computation";
        case PerformanceMonitor::MonitorType::MemoryUsage: return "memory_usage";
        case PerformanceMonitor::MonitorType::SimulationTiming: return "simulation_timing";
        case PerformanceMonitor::MonitorType::CPUGPUUsage: return "cpu_gpu_usage";
        case PerformanceMonitor::MonitorType::IOPerformance: return "io_performance";
        case PerformanceMonitor::MonitorType::Custom: return "custom";
        default: return "unknown";
    }
}

} // namespace profiling
} // namespace nlm
