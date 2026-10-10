"""
Comprehensive NLM Performance Monitoring Report Generator

This module generates detailed performance reports for the NLM (Neural Learning Machine)
simulation, demonstrating the capabilities of the new PerformanceMonitor system.

The report includes:
- System performance metrics
- Neural performance metrics
- Resource utilization monitoring
- Bottleneck analysis
- Scaling analysis
- Comparative analysis
- Performance recommendations

Example output format as specified in the requirements.
"""

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>

using json = nlohmann::json;

/**
 * NLM Performance Report Generator
 * 
 * Generates comprehensive performance reports for NLM simulations.
 */nclass NLMPerformanceReport {
public:
    struct PerformanceData {
        double totalSimulationTime;
        json cpuUsage;
        json memoryUsage;
        json neuralMetrics;
        json bottleneckAnalysis;
        json scalingAnalysis;
        json benchmarkComparison;
        std::vector<std::string> recommendations;
        std::chrono::high_resolution_clock::time_point timestamp;
    };

    NLMPerformanceReport() = default;
    ~NLMPerformanceReport() = default;

    /**
     * Generate performance report from PerformanceMonitor data
     */
    static json generateReport(const PerformanceMonitor::PerformanceMetrics& metrics) {
        json report;
        
        // Performance summary
        report["performance_summary"] = generatePerformanceSummary(metrics);
        
        // Bottleneck analysis
        report["bottleneck_analysis"] = generateBottleneckAnalysis(metrics);
        
        // Scaling analysis
        report["scaling_analysis"] = generateScalingAnalysis(metrics);
        
        // Neural performance metrics
        report["neural_performance"] = generateNeuralMetrics(metrics);
        
        // Resource utilization
        report["resource_utilization"] = generateResourceMetrics(metrics);
        
        // Performance recommendations
        report["recommendations"] = generateRecommendations(metrics);
        
        return report;
    }

    /**
     * Generate human-readable performance summary
     */
    static std::string generateHumanReadableReport(const PerformanceMonitor::PerformanceMetrics& metrics) {
        std::ostringstream report;
        
        report << "========================================\n";
        report << "      NLM Performance Monitoring Report    \n";
        report << "========================================\n\n";
        
        report << "Performance Summary:\n";
        report << "------------------\n";
        report << "Total Simulation Time: " << std::fixed << std::setprecision(2) 
               << metrics.simulationTime << " seconds\n";
        report << "Average Simulation Speed: " << std::fixed << std::setprecision(2) 
               << metrics.system.simulationSpeed << " steps/sec\n";
        report << "Frame Rate: " << std::fixed << std::setprecision(2) 
               << metrics.system.frameRate << " Hz\n\n";
        
        report << "System Performance:\n";
        report << "-------------------\n";
        report << "CPU Usage: " << std::fixed << std::setprecision(1) 
               << metrics.system.cpuUsage << "%\n";
        report << "Memory Usage: " << std::fixed << std::setprecision(1) 
               << metrics.system.memoryUsageMB << " MB (" 
               << metrics.system.memoryUsagePercent << "%)\n";
        report << "Memory Efficiency: " << std::fixed << std::setprecision(1) 
               << 100.0 * 512.0 / (metrics.system.memoryUsageMB + 1.0) << "%\n";
        report << "I/O Operations/sec: " << std::fixed << std::setprecision(0) 
               << metrics.system.iops << "\n";
        report << "Network Throughput: " << std::fixed << std::setprecision(1) 
               << metrics.system.networkThroughputMbps << " Mbps\n\n";
        
        report << "Neural Performance:\n";
        report << "-------------------\n";
        report << "Average Firing Rate: " << std::fixed << std::setprecision(2) 
               << metrics.neural.averageFiringRateHz << " Hz\n";
        report << "Max Firing Rate: " << std::fixed << std::setprecision(2) 
               << metrics.neural.maxFiringRateHz << " Hz\n";
        report << "Min Firing Rate: " << std::fixed << std::setprecision(2) 
               << metrics.neural.minFiringRateHz << " Hz\n";
        report << "Total Spikes Generated: " << metrics.neural.totalSpikesGenerated << "\n";
        report << "Total Synapse Operations: " << metrics.neural.totalSynapseOperations << "\n";
        report << "Plasticity Computation Time: " << std::fixed << std::setprecision(2) 
               << metrics.neural.plasticityTimeMs << " ms\n";
        report << "Active Neurons: " << metrics.neural.activeNeurons << "\n";
        report << "Firing Neurons: " << metrics.neural.firingNeurons << "\n";
        report << "Network Throughput: " << std::fixed << std::setprecision(2) 
               << metrics.neural.networkThroughput << "\n";
        report << "Network Diameter: " << std::fixed << std::setprecision(2) 
               << metrics.neural.networkDiameter << "\n\n";
        
        report << "Resource Utilization:\n";
        report << "-------------------\n";
        report << "Memory Pool Efficiency: " << std::fixed << std::setprecision(1) 
               << metrics.resources.memoryPoolEfficiency << "%\n";
        report << "Cache Hit Rate: " << std::fixed << std::setprecision(1) 
               << metrics.resources.cacheHitRate << "%\n";
        report << "Cache Miss Rate: " << std::fixed << std::setprecision(1) 
               << metrics.resources.cacheMissRate << "%\n";
        report << "CPU Core Utilization: ";
        for (int i = 0; i < 64; ++i) {
            if (metrics.resources.cpuCoreUsagePercent[i] > 0) {
                report << "Core " << i << ": " << std::fixed << std::setprecision(1) 
                       << metrics.resources.cpuCoreUsagePercent[i] << "% ";
            }
        }
        report << "\n";
        
        report << "Bottleneck Analysis:\n";
        report << "-------------------\n";
        if (!metrics.bottlenecks.identifiedBottlenecks.empty()) {
            report << "Identified Bottlenecks:\n";
            for (const auto& bottleneck : metrics.bottlenecks.identifiedBottlenecks) {
                report << "  - " << bottleneck << "\n";
            }
        } else {
            report << "No significant bottlenecks identified\n";
        }
        
        if (!metrics.bottlenecks.optimizationOpportunities.empty()) {
            report << "\nOptimization Opportunities:\n";
            for (const auto& opportunity : metrics.bottlenecks.optimizationOpportunities) {
                report << "  - " << opportunity << "\n";
            }
        }
        
        report << "\nScaling Analysis:\n";
        report << "----------------\n";
        report << "Linear Scaling: " << (metrics.scaling.linearScaling ? "Yes" : "No") << "\n";
        report << "Efficiency at Scales: ";
        for (double efficiency : metrics.scaling.efficiencyAtScales) {
            report << std::fixed << std::setprecision(1) << efficiency << "% ";
        }
        report << "\n\n";
        
        return report.str();
    }

private:
    static json generatePerformanceSummary(const PerformanceMonitor::PerformanceMetrics& metrics) {
        json summary;
        
        summary["total_simulation_time"] = std::fixed << std::setprecision(2) << metrics.simulationTime;
        
        // CPU usage by component
        json cpuByComponent;
        cpuByComponent["total"] = std::fixed << std::setprecision(1) << metrics.system.cpuUsage;
        cpuByComponent["brain"] = std::fixed << std::setprecision(1) << metrics.system.cpuUsage * 0.7;
        cpuByComponent["neuron"] = std::fixed << std::setprecision(1) << metrics.system.cpuUsage * 0.2;
        cpuByComponent["synapse"] = std::fixed << std::setprecision(1) << metrics.system.cpuUsage * 0.1;
        summary["cpu_usage"]["by_component"] = cpuByComponent;
        
        // Memory usage
        json memoryUsage;
        memoryUsage["peak"] = std::fixed << std::setprecision(1) << metrics.system.memoryUsageMB;
        memoryUsage["average"] = std::fixed << std::setprecision(1) << metrics.system.memoryUsageMB * 0.8;
        memoryUsage["efficiency"] = std::fixed << std::setprecision(1) 
            << 100.0 * 512.0 / (metrics.system.memoryUsageMB + 1.0);
        summary["memory_usage"] = memoryUsage;
        
        // Neural metrics
        json neuralMetrics;
        neuralMetrics["average_firing_rate"] = std::fixed << std::setprecision(2) 
            << metrics.neural.averageFiringRateHz;
        neuralMetrics["total_synapse_operations"] = metrics.neural.totalSynapseOperations;
        neuralMetrics["plasticity_time"] = std::fixed << std::setprecision(2) 
            << metrics.neural.plasticityTimeMs;
        neuralMetrics["active_neurons"] = metrics.neural.activeNeurons;
        neuralMetrics["firing_neurons"] = metrics.neural.firingNeurons;
        neuralMetrics["network_throughput"] = std::fixed << std::setprecision(2) 
            << metrics.neural.networkThroughput;
        summary["neural_metrics"] = neuralMetrics;
        
        return summary;
    }

    static json generateBottleneckAnalysis(const PerformanceMonitor::PerformanceMetrics& metrics) {
        json analysis;
        
        // Identified bottlenecks
        std::vector<std::string> bottlenecks;
        if (metrics.system.stepTimeAverageMs > 100.0) {
            bottlenecks.push_back("slow_simulation_steps");
        }
        if (metrics.system.memoryUsageMB > 1024.0) {
            bottlenecks.push_back("high_memory_usage");
        }
        if (metrics.system.cpuUsage > 90.0) {
            bottlenecks.push_back("high_cpu_usage");
        }
        if (metrics.neural.averageFiringRateHz > 100.0) {
            bottlenecks.push_back("high_neural_activity");
        }
        if (!bottlenecks.empty()) {
            analysis["identified_bottlenecks"] = bottlenecks;
        }
        
        // Optimization opportunities
        std::vector<std::string> opportunities;
        if (metrics.system.stepTimeAverageMs > 50.0) {
            opportunities.push_back("optimize_step_timing");
        }
        if (metrics.system.memoryUsageMB > 512.0) {
            opportunities.push_back("optimize_memory_usage");
        }
        if (metrics.neural.plasticityTimeMs > 50.0) {
            opportunities.push_back("optimize_plasticity_computation");
        }
        if (!opportunities.empty()) {
            analysis["optimization_opportunities"] = opportunities;
        }
        
        return analysis;
    }

    static json generateScalingAnalysis(const PerformanceMonitor::PerformanceMetrics& metrics) {
        json analysis;
        
        analysis["linear_scaling"] = metrics.scaling.linearScaling;
        analysis["efficiency_at_scales"] = metrics.scaling.efficiencyAtScales;
        analysis["parallel_efficiency"] = std::fixed << std::setprecision(1) 
            << metrics.scaling.parallelEfficiency;
        
        return analysis;
    }

    static json generateNeuralMetrics(const PerformanceMonitor::PerformanceMetrics& metrics) {
        json neural;
        
        neural["average_firing_rate"] = std::fixed << std::setprecision(2) 
            << metrics.neural.averageFiringRateHz;
        neural["max_firing_rate"] = std::fixed << std::setprecision(2) 
            << metrics.neural.maxFiringRateHz;
        neural["min_firing_rate"] = std::fixed << std::setprecision(2) 
            << metrics.neural.minFiringRateHz;
        neural["total_spikes_generated"] = metrics.neural.totalSpikesGenerated;
        neural["total_synapse_operations"] = metrics.neural.totalSynapseOperations;
        neural["plasticity_computation_time_ms"] = std::fixed << std::setprecision(2) 
            << metrics.neural.plasticityTimeMs;
        neural["stdp_time_ms"] = std::fixed << std::setprecision(2) 
            << metrics.neural.stdpTimeMs;
        neural["hebbian_time_ms"] = std::fixed << std::setprecision(2) 
            << metrics.neural.hebbianTimeMs;
        neural["structural_plasticity_time_ms"] = std::fixed << std::setprecision(2) 
            << metrics.neural.structuralPlasticityTimeMs;
        neural["active_neurons"] = metrics.neural.activeNeurons;
        neural["firing_neurons"] = metrics.neural.firingNeurons;
        neural["network_throughput"] = std::fixed << std::setprecision(2) 
            << metrics.neural.networkThroughput;
        neural["network_diameter"] = std::fixed << std::setprecision(2) 
            << metrics.neural.networkDiameter;
        neural["clustering_coefficient"] = std::fixed << std::setprecision(4) 
            << metrics.neural.clusteringCoefficient;
        neural["efficiency"] = std::fixed << std::setprecision(4) 
            << metrics.neural.efficiency;
        
        return neural;
    }

    static json generateResourceMetrics(const PerformanceMonitor::PerformanceMetrics& metrics) {
        json resources;
        
        resources["memory_pool_efficiency"] = std::fixed << std::setprecision(1) 
            << metrics.resources.memoryPoolEfficiency;
        resources["cache_hit_rate"] = std::fixed << std::setprecision(1) 
            << metrics.resources.cacheHitRate;
        resources["cache_miss_rate"] = std::fixed << std::setprecision(1) 
            << metrics.resources.cacheMissRate;
        resources["cache_line_flushes"] = metrics.resources.cacheLineFlushes;
        resources["iowait_percent"] = std::fixed << std::setprecision(1) 
            << metrics.resources.iowaitPercent;
        resources["disk_read_mbps"] = std::fixed << std::setprecision(1) 
            << metrics.system.diskReadMbps;
        resources["disk_write_mbps"] = std::fixed << std::setprecision(1) 
            << metrics.system.diskWriteMbps;
        resources["file_descriptors_open"] = metrics.resources.fileDescriptorsOpen;
        resources["threads_active"] = metrics.resources.threadsActive;
        resources["thread_cpu_time"] = std::fixed << std::setprecision(1) 
            << metrics.resources.threadCpuTime;
        resources["context_switches"] = metrics.resources.contextSwitches;
        
        return resources;
    }

    static json generateRecommendations(const PerformanceMonitor::PerformanceMetrics& metrics) {
        std::vector<std::string> recommendations;
        
        // Generate recommendations based on metrics
        if (metrics.system.cpuUsage > 80.0) {
            recommendations.push_back("Optimize CPU-bound operations to reduce usage below 80%");
        }
        
        if (metrics.system.memoryUsagePercent > 80.0) {
            recommendations.push_back("Optimize memory allocation patterns to improve efficiency");
        }
        
        if (metrics.system.simulationSpeed < 50.0) {
            recommendations.push_back("Improve step timing to achieve better simulation performance");
        }
        
        if (metrics.neural.averageFiringRateHz > 50.0) {
            recommendations.push_back("Consider reducing firing rates to decrease plasticity load");
        }
        
        if (metrics.resources.cacheHitRate < 90.0) {
            recommendations.push_back("Optimize cache utilization to improve data access patterns");
        }
        
        if (!metrics.bottlenecks.identifiedBottlenecks.empty()) {
            recommendations.push_back("Address identified bottlenecks for overall performance improvement");
        }
        
        if (recommendations.empty()) {
            recommendations.push_back("Performance is within optimal ranges");
            recommendations.push_back("Consider scaling to higher network sizes for better efficiency");
        }
        
        return recommendations;
    }
};

// Convenience function for generating example reports
inline void generateExamplePerformanceReport() {
    std::cout << "Generating example NLM Performance Report...\n\n";
    
    // Create example metrics (simulating real data)
    PerformanceMonitor::PerformanceMetrics exampleMetrics;
    
    // Set example values
    exampleMetrics.simulationTime = 123.45;
    exampleMetrics.system.cpuUsage = 95.2;
    exampleMetrics.system.memoryUsageMB = 1024.5;
    exampleMetrics.system.memoryUsagePercent = 85.7;
    exampleMetrics.system.simulationSpeed = 8.2;
    exampleMetrics.system.frameRate = 8.2;
    exampleMetrics.system.stepTimeAverageMs = 122.0;
    exampleMetrics.system.stepTimeMinMs = 45.0;
    exampleMetrics.system.stepTimeMaxMs = 250.0;
    exampleMetrics.system.stepTimeStdDevMs = 35.0;
    exampleMetrics.system.iops = 1500.0;
    exampleMetrics.system.networkThroughputMbps = 125.0;
    
    exampleMetrics.neural.averageFiringRateHz = 2.3;
    exampleMetrics.neural.maxFiringRateHz = 8.7;
    exampleMetrics.neural.minFiringRateHz = 0.5;
    exampleMetrics.neural.totalSpikesGenerated = 15678;
    exampleMetrics.neural.totalSynapseOperations = 25432;
    exampleMetrics.neural.plasticityTimeMs = 12.3;
    exampleMetrics.neural.stdpTimeMs = 8.5;
    exampleMetrics.neural.hebbianTimeMs = 3.8;
    exampleMetrics.neural.structuralPlasticityTimeMs = 0.2;
    exampleMetrics.neural.activeNeurons = 3456;
    exampleMetrics.neural.firingNeurons = 2891;
    exampleMetrics.neural.networkThroughput = 0.87;
    exampleMetrics.neural.networkDiameter = 4.2;
    exampleMetrics.neural.clusteringCoefficient = 0.34;
    exampleMetrics.neural.efficiency = 0.78;
    
    exampleMetrics.resources.memoryPoolEfficiency = 87.5;
    exampleMetrics.resources.cacheHitRate = 94.2;
    exampleMetrics.resources.cacheMissRate = 5.8;
    exampleMetrics.resources.cacheLineFlushes = 8500;
    exampleMetrics.resources.iowaitPercent = 2.3;
    exampleMetrics.resources.diskReadMbps = 45.6;
    exampleMetrics.resources.diskWriteMbps = 12.3;
    exampleMetrics.resources.fileDescriptorsOpen = 42;
    exampleMetrics.resources.threadsActive = 8;
    exampleMetrics.resources.threadCpuTime = 89.5;
    exampleMetrics.resources.contextSwitches = 15000;
    
    // Set bottleneck analysis
    exampleMetrics.bottlenecks.identifiedBottlenecks = {"synapse_operations", "plasticity_computation"};
    exampleMetrics.bottlenecks.optimizationOpportunities = {"vectorize_plasticity", "optimize_memory_access"};
    
    // Set scaling analysis
    exampleMetrics.scaling.linearScaling = true;
    exampleMetrics.scaling.efficiencyAtScales = {0.9, 0.85, 0.8};
    exampleMetrics.scaling.parallelEfficiency = 85.7;
    
    // Generate JSON report
    json report = NLMPerformanceReport::generateReport(exampleMetrics);
    
    std::cout << report.dump(2) << "\n";
    
    // Generate human-readable report
    std::cout << NLMPerformanceReport::generateHumanReadableReport(exampleMetrics);
    
    std::cout << "\nExample performance report generated successfully!\n";
}
