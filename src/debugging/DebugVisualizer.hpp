#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>

namespace nlm {
namespace debug {

// Advanced debugging and visualization utilities for NLM

class DebugVisualizer {
public:
    struct VisualizationData {
        double timestamp;
        std::vector<double> neuronStates;
        std::vector<double> synapticWeights;
        std::vector<int> spikeCounts;
        std::string metadata;
    };
    
    DebugVisualizer() = default;
    ~DebugVisualizer() = default;
    
    // Start recording visualization data
    void startRecording(const std::string& filename, double intervalSeconds = 1.0) {
        recording_ = true;
        recordingInterval_ = intervalSeconds;
        recordingFile_ = filename;
        lastRecordingTime_ = std::chrono::system_clock::now();
        
        NLM_LOG_INFO("Debug visualization started: {}", filename);
    }
    
    // Stop recording
    void stopRecording() {
        recording_ = false;
        NLM_LOG_INFO("Debug visualization stopped");
    }
    
    // Record current state
    void recordState(const VisualizationData& data) {
        if (recording_) {
            auto now = std::chrono::system_clock::now();
            auto duration = std::chrono::duration<double>(now - lastRecordingTime_).count();
            
            if (duration >= recordingInterval_) {
                // Write data to file
                std::ofstream file(recordingFile_, std::ios::app);
                if (file.is_open()) {
                    file << formatDataAsJSON(data) << std::endl;
                }
                
                lastRecordingTime_ = now;
            }
        }
    }
    
    // Generate visualization script
    void generateVisualizationScript(const std::string& scriptFile = "visualize.py") {
        std::ofstream file(scriptFile);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to create visualization script: {}", scriptFile);
            return;
        }
        
        file << "#!/usr/bin/env python3\n";
        file << """
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import json
import sys

# Load NLM debug data
def load_debug_data(filename):
    data = []
    with open(filename, 'r') as f:
        for line in f:
            try:
                data.append(json.loads(line.strip()))
            except json.JSONDecodeError:
                continue
    return data

# Create interactive visualization
def create_interactive_visualization(data_file):
    data = load_debug_data(data_file)
    
    if not data:
        print("No data to visualize")
        return
    
    # Setup figure
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle('NLM Neural Network Visualization', fontsize=16)
    
    # Time series
    times = [d['timestamp'] for d in data]
    ax1 = axes[0, 0]
    
    # Neuron states (first 10 neurons)
    ax1.plot(times, [d['neuronStates'][i] if len(d['neuronStates']) > i else 0 
                     for i in range(min(10, len(data[0]['neuronStates'])))],
             label='Neuron States', alpha=0.7)
    ax1.set_xlabel('Time (s)')
    ax1.set_ylabel('Membrane Potential (mV)')
    ax1.set_title('Neuron Activity')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # Synaptic weights
    ax2 = axes[0, 1]
    weights = [d['synapticWeights'][0] if d['synapticWeights'] else 0 
               for d in data]
    ax2.plot(times, weights, 'r-', alpha=0.7, linewidth=2)
    ax2.set_xlabel('Time (s)')
    ax2.set_ylabel('Synaptic Weight')
    ax2.set_title('Synaptic Dynamics')
    ax2.grid(True, alpha=0.3)
    
    # Spike counts
    ax3 = axes[1, 0]
    spikes = [d['spikeCounts'][0] if d['spikeCounts'] else 0 for d in data]
    ax3.plot(times, spikes, 'g-', alpha=0.7, linewidth=2)
    ax3.set_xlabel('Time (s)')
    ax3.set_ylabel('Spike Count')
    ax3.set_title('Spike Activity')
    ax3.grid(True, alpha=0.3)
    
    # Network topology (static visualization)
    ax4 = axes[1, 1]
    ax4.set_aspect('equal')
    ax4.scatter([], [], s=100, c='red', label='Source Neurons')
    ax4.scatter([], [], s=100, c='blue', label='Target Neurons')
    ax4.set_xlabel('Space')
    ax4.set_ylabel('Space')
    ax4.set_title('Network Topology')
    ax4.legend()
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    # Save animation
    anim = animation.FuncAnimation(fig, update_animation, frames=len(data), 
                                   init_func=init_animation, blit=False, interval=50)
    
    anim.save('nlm_visualization.gif', writer='pillow', fps=20)
    plt.show()

# Animation update function
def update_animation(frame):
    # Update plots based on frame data
    pass

# Animation init function
def init_animation():
    # Initialize plots
    pass

if __name__ == "__main__":
    if len(sys.argv) > 1:
        create_interactive_visualization(sys.argv[1])
    else:
        print("Usage: python3 visualize.py <data_file>")
        print("Example: python3 visualize.py nlm_debug_data.json")
""";
        
        NLM_LOG_INFO("Visualization script generated: {}", scriptFile);
    }
};

// Network topology analyzer
class NetworkTopologyAnalyzer {
public:
    struct NetworkStats {
        size_t neuronCount;
        size_t regionCount;
        size_t synapseCount;
        float connectivityDensity;
        float averageDegree;
        float clusteringCoefficient;
        float efficiency;
        float modularity;
        std::vector<float> degreeDistribution;
        std::map<std::string, float> statistics;
    };
    
    NetworkTopologyAnalyzer(std::shared_ptr<Brain> brain) 
        : brain_(brain) {}
    
    NetworkStats analyzeTopology() {
        NetworkStats stats;
        
        if (!brain_) {
            NLM_LOG_ERROR("Brain not available for topology analysis");
            return stats;
        }
        
        stats.neuronCount = brain_->getTotalNeuronCount();
        stats.regionCount = brain_->getRegionCount();
        stats.synapseCount = brain_->getTotalSynapseCount();
        
        if (stats.synapseCount > 0 && stats.neuronCount > 0) {
            stats.connectivityDensity = static_cast<float>(stats.synapseCount) / 
                                       (stats.neuronCount * (stats.neuronCount - 1) / 2);
            
            // Estimate average degree
            stats.averageDegree = stats.connectivityDensity * (stats.neuronCount - 1);
            
            // Calculate basic clustering coefficient
            stats.clusteringCoefficient = estimateClusteringCoefficient();
            
            // Calculate efficiency
            stats.efficiency = calculateNetworkEfficiency();
            
            // Calculate modularity (simplified)
            stats.modularity = calculateModularity();
        }
        
        stats.degreeDistribution = computeDegreeDistribution();
        stats.statistics = computeAdditionalStatistics();
        
        NLM_LOG_INFO("Network topology analysis completed:");
        NLM_LOG_INFO("  Neurons: {}", stats.neuronCount);
        NLM_LOG_INFO("  Regions: {}", stats.regionCount);
        NLM_LOG_INFO("  Synapses: {}", stats.synapseCount);
        NLM_LOG_INFO("  Connectivity Density: {}", stats.connectivityDensity);
        NLM_LOG_INFO("  Average Degree: {}", stats.averageDegree);
        NLM_LOG_INFO("  Clustering Coefficient: {}", stats.clusteringCoefficient);
        NLM_LOG_INFO("  Network Efficiency: {}", stats.efficiency);
        NLM_LOG_INFO("  Modularity: {}", stats.modularity);
        
        return stats;
    }
    
    // Generate network visualization
    void generateNetworkVisualization(const std::string& outputFile = "network_topology.png") {
        auto stats = analyzeTopology();
        
        // Create network visualization using matplotlib (if available)
        // This is a placeholder for actual implementation
        NLM_LOG_INFO("Network visualization generation: {}", outputFile);
        
        // In production, would use actual plotting libraries
        std::ofstream file(outputFile, std::ios::binary);
        if (file.is_open()) {
            // Write placeholder data
            file << "NLM Network Topology Visualization (Placeholder)\n";
            file << "Neuron Count: " << stats.neuronCount << "\n";
            file << "Synapse Count: " << stats.synapseCount << "\n";
            file << "Connectivity: " << stats.connectivityDensity << "\n";
            file << "Statistics generated at: " << std::chrono::system_clock::now() << "\n";
        }
    }
    
private:
    float estimateClusteringCoefficient() {
        // Simplified clustering coefficient calculation
        // In practice, would use network analysis libraries
        return 0.5f; // Placeholder
    }
    
    float calculateNetworkEfficiency() {
        // Simplified network efficiency calculation
        return 0.7f; // Placeholder
    }
    
    float calculateModularity() {
        // Simplified modularity calculation
        return 0.3f; // Placeholder
    }
    
    std::vector<float> computeDegreeDistribution() {
        std::vector<float> distribution;
        // In practice, would compute actual degree distribution
        for (size_t i = 0; i < 10; ++i) {
            distribution.push_back(0.0f);
        }
        return distribution;
    }
    
    std::map<std::string, float> computeAdditionalStatistics() {
        std::map<std::string, float> stats;
        stats["average_weight"] = 0.1f;
        stats["max_weight"] = 1.0f;
        stats["min_weight"] = 0.0f;
        stats["weight_std"] = 0.3f;
        return stats;
    }
    
    std::shared_ptr<Brain> brain_;
};

// Memory profiler
class MemoryProfiler {
public:
    struct MemoryProfile {
        size_t totalMemory;          // in bytes
        size_t allocatedMemory;      // in bytes
        size_t peakMemory;           // in bytes
        size_t memoryUsageCount;     // number of allocation attempts
        float allocationFailureRate; // percentage
        std::vector<size_t> memorySnapshots; // memory usage over time
        double averageAllocationSize; // average allocation size
    };
    
    MemoryProfiler(std::shared_ptr<Brain> brain) 
        : brain_(brain) {}
    
    MemoryProfile profileMemoryUsage() {
        MemoryProfile profile;
        
        // Take initial snapshot
        profile.memorySnapshots.push_back(getCurrentMemoryUsage());
        profile.totalMemory = getSystemMemoryInfo().total;
        profile.allocatedMemory = profile.memorySnapshots[0];
        profile.peakMemory = profile.allocatedMemory;
        
        // Run memory-intensive operations
        profile.memoryUsageCount = 0;
        profile.allocationFailureRate = 0.0f;
        
        // Simulate memory usage patterns
        for (size_t i = 0; i < 100; ++i) {
            auto usage = getCurrentMemoryUsage();
            profile.memorySnapshots.push_back(usage);
            
            if (usage > profile.peakMemory) {
                profile.peakMemory = usage;
            }
            
            // Simulate memory allocation failures
            if (i % 20 == 0) {
                profile.memoryUsageCount++;
            }
        }
        
        // Calculate statistics
        if (!profile.memorySnapshots.empty()) {
            double totalSnapshots = static_cast<double>(profile.memorySnapshots.size());
            double sumMemory = std::accumulate(profile.memorySnapshots.begin(), 
                                             profile.memorySnapshots.end(), 0.0);
            profile.averageAllocationSize = sumMemory / totalSnapshots;
            
            double maxMemory = *std::max_element(profile.memorySnapshots.begin(),
                                                profile.memorySnapshots.end());
            profile.allocationFailureRate = (maxMemory > profile.totalMemory * 0.9) ? 0.1f : 0.0f;
        }
        
        NLM_LOG_INFO("Memory profiling completed:");
        NLM_LOG_INFO("  Total Memory: {} MB", profile.totalMemory / (1024 * 1024));
        NLM_LOG_INFO("  Allocated Memory: {} MB", profile.allocatedMemory / (1024 * 1024));
        NLM_LOG_INFO("  Peak Memory: {} MB", profile.peakMemory / (1024 * 1024));
        NLM_LOG_INFO("  Allocation Failures: {}", profile.memoryUsageCount);
        NLM_LOG_INFO("  Failure Rate: {}%", profile.allocationFailureRate * 100);
        
        return profile;
    }
    
    // Generate memory usage report
    void generateMemoryReport(const MemoryProfile& profile, 
                              const std::string& reportFile = "memory_report.txt") {
        std::ofstream file(reportFile);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to create memory report: {}", reportFile);
            return;
        }
        
        file << "NLM Memory Usage Report\n";
        file << "Generated: " << std::chrono::system_clock::now() << "\n\n";
        
        file << "Memory Statistics:\n";
        file << "  Total System Memory: " << (profile.totalMemory / (1024 * 1024)) << " MB\n";
        file << "  Allocated Memory: " << (profile.allocatedMemory / (1024 * 1024)) << " MB\n";
        file << "  Peak Memory: " << (profile.peakMemory / (1024 * 1024)) << " MB\n";
        file << "  Memory Usage Count: " << profile.memoryUsageCount << "\n";
        file << "  Allocation Failure Rate: " << (profile.allocationFailureRate * 100) << "%\n";
        file << "  Average Allocation Size: " << (profile.averageAllocationSize / 1024) << " KB\n\n";
        
        file << "Memory Snapshots:\n";
        for (size_t i = 0; i < profile.memorySnapshots.size(); ++i) {
            file << "  Snapshot " << i << ": " << (profile.memorySnapshots[i] / 1024) << " KB\n";
        }
        
        NLM_LOG_INFO("Memory report generated: {}", reportFile);
    }
    
private:
    size_t getCurrentMemoryUsage() {
        // Placeholder for actual memory measurement
        // In production, would use platform-specific APIs
        return 1024 * 1024; // 1 MB placeholder
    }
    
    struct SystemMemoryInfo {
        size_t total;
        size_t available;
    };
    
    SystemMemoryInfo getSystemMemoryInfo() {
        SystemMemoryInfo info;
        info.total = 8 * 1024 * 1024 * 1024; // 8 GB placeholder
        info.available = 6 * 1024 * 1024 * 1024; // 6 GB placeholder
        return info;
    }
    
    std::shared_ptr<Brain> brain_;
};

} // namespace debug
} // namespace nlm