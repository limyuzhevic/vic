#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <iomanip>

namespace nlm {

/**
 * Performance metrics structure for tracking execution time, memory usage,
 * CPU utilization, operation counts, and resource consumption during neural simulation.
 */
struct PerformanceMetrics {
    // Timing metrics
    double brainInitializationTimeMs = 0.0;      // Brain initialization duration
    double simulationSetupTimeMs = 0.0;           // Simulation setup duration
    double stepExecutionTimeMs = 0.0;             // Average step execution time
    double totalExecutionTimeMs = 0.0;            // Total wall clock time
    double plasticityComputationTimeMs = 0.0;     // Plasticity computation duration
    double predictionComputationTimeMs = 0.0;     // Prediction computation duration
    
    // Performance metrics
    double cpuUsagePercent = 0.0;                // Average CPU utilization percentage
    double memoryUsageMB = 0.0;                   // Current memory usage in MB
    double peakMemoryMB = 0.0;                    // Peak memory usage in MB
    double simulationSpeed = 0.0;                // Steps per second
    
    // Operation counts
    uint64_t spikeCount = 0;                      // Total number of spikes
    uint64_t synapseCount = 0;                    // Total number of synapses
    uint64_t neuronCount = 0;                     // Total number of neurons
    uint64_t activeNeuronCount = 0;               // Average active neurons per step
    uint64_t firingNeuronCount = 0;               // Average firing neurons per step
    uint64_t plasticityEvents = 0;                // Number of plasticity events
    uint64_t predictionSteps = 0;                 // Number of prediction steps
    
    // Brain state metrics
    double averageFiringRate = 0.0;              // Average firing rate in Hz
    double spikeDensity = 0.0;                    // Spikes per neuron per second
    double synapticWeightMean = 0.0;              // Mean synaptic weight
    double synapticWeightStdDev = 0.0;            // Synaptic weight standard deviation
    double excitatoryRatio = 0.0;                // Ratio of excitatory synapses
    
    // Step-level metrics (for detailed profiling)
    std::vector<double> stepTimesMs;             // Individual step execution times
    std::vector<uint64_t> spikeCountsPerStep;    // Spikes per simulation step
    std::vector<uint64_t> activeNeuronCountsPerStep; // Active neurons per step
    
    // Component timing metrics
    std::unordered_map<std::string, double> componentTimesMs; // Component-specific timing
    
    // Configuration metadata
    std::string configName;                       // Configuration name
    uint64_t simulationSteps = 0;                 // Number of simulation steps run
    uint64_t startTime = 0;                       // Unix timestamp at start
    uint64_t endTime = 0;                         // Unix timestamp at end
    
    // Configuration for different comparison scenarios
    struct Configuration {
        std::string name;
        std::string description;
        std::unordered_map<std::string, std::string> parameters;
        
        Configuration() : name(""), description("") {}
        Configuration(const std::string& n, const std::string& d) 
            : name(n), description(d) {}
    };
    
    // Default constructor
    PerformanceMetrics() : startTime(0), endTime(0) {}
    
    // Calculate derived metrics
    void calculateDerivedMetrics() {
        if (!stepTimesMs.empty()) {
            // Calculate average step time
            double sum = 0.0;
            double min = stepTimesMs[0];
            double max = stepTimesMs[0];
            for (double time : stepTimesMs) {
                sum += time;
                min = std::min(min, time);
                max = std::max(max, time);
            }
            stepExecutionTimeMs = sum / stepTimesMs.size();
            
            // Calculate standard deviation of step times
            if (stepTimesMs.size() > 1) {
                double varianceSum = 0.0;
                for (double time : stepTimesMs) {
                    double diff = time - stepExecutionTimeMs;
                    varianceSum += diff * diff;
                }
                stepTimeStdDev = std::sqrt(varianceSum / (stepTimesMs.size() - 1));
            }
        }
        
        // Calculate derived performance metrics
        if (totalExecutionTimeMs > 0) {
            simulationSpeed = simulationSteps / (totalExecutionTimeMs / 1000.0); // steps per second
            cpuUsagePercent = std::min(100.0, cpuUsagePercent); // Cap at 100%
            
            // Calculate firing rate
            if (neuronCount > 0 && totalExecutionTimeMs > 0) {
                averageFiringRate = (spikeCount / neuronCount) / (totalExecutionTimeMs / 1000.0);
                spikeDensity = (spikeCount / static_cast<double>(simulationSteps)) / neuronCount;
            }
        }
    }
    
    // Generate JSON output
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"brainInitializationTimeMs\": " << std::fixed << std::setprecision(3) << brainInitializationTimeMs << ",\n";
        ss << "  \"simulationSetupTimeMs\": " << std::fixed << std::setprecision(3) << simulationSetupTimeMs << ",\n";
        ss << "  \"stepExecutionTimeMs\": " << std::fixed << std::setprecision(3) << stepExecutionTimeMs << ",\n";
        ss << "  \"totalExecutionTimeMs\": " << std::fixed << std::setprecision(3) << totalExecutionTimeMs << ",\n";
        ss << "  \"plasticityComputationTimeMs\": " << std::fixed << std::setprecision(3) << plasticityComputationTimeMs << ",\n";
        ss << "  \"predictionComputationTimeMs\": " << std::fixed << std::setprecision(3) << predictionComputationTimeMs << ",\n";
        ss << "  \"cpuUsagePercent\": " << std::fixed << std::setprecision(2) << cpuUsagePercent << ",\n";
        ss << "  \"memoryUsageMB\": " << std::fixed << std::setprecision(2) << memoryUsageMB << ",\n";
        ss << "  \"peakMemoryMB\": " << std::fixed << std::setprecision(2) << peakMemoryMB << ",\n";
        ss << "  \"simulationSpeed\": " << std::fixed << std::setprecision(2) << simulationSpeed << ",\n";
        ss << "  \"spikeCount\": " << spikeCount << ",\n";
        ss << "  \"synapseCount\": " << synapseCount << ",\n";
        ss << "  \"neuronCount\": " << neuronCount << ",\n";
        ss << "  \"activeNeuronCount\": " << activeNeuronCount << ",\n";
        ss << "  \"firingNeuronCount\": " << firingNeuronCount << ",\n";
        ss << "  \"plasticityEvents\": " << plasticityEvents << ",\n";
        ss << "  \"predictionSteps\": " << predictionSteps << ",\n";
        ss << "  \"averageFiringRate\": " << std::fixed << std::setprecision(3) << averageFiringRate << ",\n";
        ss << "  \"spikeDensity\": " << std::fixed << std::setprecision(3) << spikeDensity << ",\n";
        ss << "  \"synapticWeightMean\": " << std::fixed << std::setprecision(4) << synapticWeightMean << ",\n";
        ss << "  \"synapticWeightStdDev\": " << std::fixed << std::setprecision(4) << synapticWeightStdDev << ",\n";
        ss << "  \"excitatoryRatio\": " << std::fixed << std::setprecision(4) << excitatoryRatio << ",\n";
        ss << "  \"simulationSteps\": " << simulationSteps << ",\n";
        
        // Add component times if any
        if (!componentTimesMs.empty()) {
            ss << "  \"componentTimesMs\": {\n";
            bool first = true;
            for (const auto& [component, time] : componentTimesMs) {
                if (!first) ss << ",\n";
                ss << "    \"" << component << "\": " << std::fixed << std::setprecision(3) << time;
                first = false;
            }
            ss << "\n  },\n";
        }
        
        ss << "  \"startTime\": " << startTime << ",\n";
        ss << "  \"endTime\": " << endTime << "\n";
        ss << "}";
        return ss.str();
    }
    
    // Generate human-readable summary
    std::string toHumanReadable() const {
        std::stringstream ss;
        ss << "=== Performance Metrics Summary ===\n";
        ss << "\n";
        ss << "Timing:\n";
        ss << "  Brain Initialization:    " << std::fixed << std::setprecision(3) << brainInitializationTimeMs << " ms\n";
        ss << "  Simulation Setup:        " << std::fixed << std::setprecision(3) << simulationSetupTimeMs << " ms\n";
        ss << "  Average Step Time:       " << std::fixed << std::setprecision(3) << stepExecutionTimeMs << " ms\n";
        ss << "  Total Execution Time:    " << std::fixed << std::setprecision(3) << totalExecutionTimeMs << " ms\n";
        ss << "  Plasticity Computation:  " << std::fixed << std::setprecision(3) << plasticityComputationTimeMs << " ms\n";
        ss << "  Prediction Computation:  " << std::fixed << std::setprecision(3) << predictionComputationTimeMs << " ms\n";
        ss << "  Simulation Speed:        " << std::fixed << std::setprecision(2) << simulationSpeed << " steps/second\n";
        ss << "\n";
        ss << "Resource Usage:\n";
        ss << "  CPU Utilization:         " << std::fixed << std::setprecision(2) << cpuUsagePercent << "%\n";
        ss << "  Memory Usage:            " << std::fixed << std::setprecision(2) << memoryUsageMB << " MB\n";
        ss << "  Peak Memory:             " << std::fixed << std::setprecision(2) << peakMemoryMB << " MB\n";
        ss << "\n";
        ss << "Activity Statistics:\n";
        ss << "  Total Spikes:            " << spikeCount << "\n";
        ss << "  Total Synapses:          " << synapseCount << "\n";
        ss << "  Total Neurons:           " << neuronCount << "\n";
        ss << "  Average Active Neurons:  " << activeNeuronCount << "\n";
        ss << "  Average Firing Neurons:  " << firingNeuronCount << "\n";
        ss << "  Plasticity Events:       " << plasticityEvents << "\n";
        ss << "  Prediction Steps:        " << predictionSteps << "\n";
        ss << "\n";
        ss << "Derived Metrics:\n";
        ss << "  Average Firing Rate:     " << std::fixed << std::setprecision(3) << averageFiringRate << " Hz\n";
        ss << "  Spike Density:           " << std::fixed << std::setprecision(3) << spikeDensity << " spikes/(neuron·sec)\n";
        ss << "  Mean Synaptic Weight:    " << std::fixed << std::setprecision(4) << synapticWeightMean << "\n";
        ss << "  Synaptic Weight StdDev:  " << std::fixed << std::setprecision(4) << synapticWeightStdDev << "\n";
        ss << "  Excitatory Ratio:       " << std::fixed << std::setprecision(4) << excitatoryRatio << "\n";
        ss << "\n";
        ss << "Configuration:\n";
        ss << "  Name:                    " << configName << "\n";
        ss << "  Steps Run:               " << simulationSteps << "\n";
        ss << "  Start Time:              " << startTime << "\n";
        ss << "  End Time:                " << endTime << "\n";
        
        // Add component times if any
        if (!componentTimesMs.empty()) {
            ss << "\nComponent Times (ms):\n";
            for (const auto& [component, time] : componentTimesMs) {
                ss << "  " << component << ": " << std::fixed << std::setprecision(3) << time << " ms\n";
            }
        }
        
        ss << "\n";
        return ss.str();
    }
    
    // Generate CSV output
    std::string toCSV() const {
        std::stringstream ss;
        ss << "Metric,Value\n";
        ss << "brain_initialization_time_ms," << std::fixed << std::setprecision(3) << brainInitializationTimeMs << "\n";
        ss << "simulation_setup_time_ms," << std::fixed << std::setprecision(3) << simulationSetupTimeMs << "\n";
        ss << "step_execution_time_ms," << std::fixed << std::setprecision(3) << stepExecutionTimeMs << "\n";
        ss << "total_execution_time_ms," << std::fixed << std::setprecision(3) << totalExecutionTimeMs << "\n";
        ss << "plasticity_computation_time_ms," << std::fixed << std::setprecision(3) << plasticityComputationTimeMs << "\n";
        ss << "prediction_computation_time_ms," << std::fixed << std::setprecision(3) << predictionComputationTimeMs << "\n";
        ss << "cpu_usage_percent," << std::fixed << std::setprecision(2) << cpuUsagePercent << "\n";
        ss << "memory_usage_mb," << std::fixed << std::setprecision(2) << memoryUsageMB << "\n";
        ss << "peak_memory_mb," << std::fixed << std::setprecision(2) << peakMemoryMB << "\n";
        ss << "simulation_speed," << std::fixed << std::setprecision(2) << simulationSpeed << "\n";
        ss << "spike_count," << spikeCount << "\n";
        ss << "synapse_count," << synapseCount << "\n";
        ss << "neuron_count," << neuronCount << "\n";
        ss << "active_neuron_count," << activeNeuronCount << "\n";
        ss << "firing_neuron_count," << firingNeuronCount << "\n";
        ss << "plasticity_events," << plasticityEvents << "\n";
        ss << "prediction_steps," << predictionSteps << "\n";
        ss << "average_firing_rate," << std::fixed << std::setprecision(3) << averageFiringRate << "\n";
        ss << "spike_density," << std::fixed << std::setprecision(3) << spikeDensity << "\n";
        ss << "synaptic_weight_mean," << std::fixed << std::setprecision(4) << synapticWeightMean << "\n";
        ss << "synaptic_weight_stddev," << std::fixed << std::setprecision(4) << synapticWeightStdDev << "\n";
        ss << "excitatory_ratio," << std::fixed << std::setprecision(4) << excitatoryRatio << "\n";
        ss << "simulation_steps," << simulationSteps << "\n";
        
        // Add component times if any
        for (const auto& [component, time] : componentTimesMs) {
            std::string safeComponent = component;
            // Replace spaces and special characters for CSV column names
            std::replace(safeComponent.begin(), safeComponent.end(), ' ', '_');
            std::replace(safeComponent.begin(), safeComponent.end(), '(', '_');
            std::replace(safeComponent.begin(), safeComponent.end(), ')', '_');
            ss << "component_time_ms_" << safeComponent << "," << std::fixed << std::setprecision(3) << time << "\n";
        }
        
        return ss.str();
    }
};

/**
 * Performance monitor for tracking execution metrics during neural simulation.
 * Provides timing measurements, resource usage tracking, and detailed profiling capabilities.
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start monitoring (call at beginning of monitoring period)
    void start();
    
    // Stop monitoring (call at end of monitoring period)
    void stop();
    
    // Record a simulation step for performance tracking
    void recordStep(double stepTimeMs, uint64_t spikes, uint64_t activeNeurons);
    
    // Record plasticity computation time
    void recordPlasticityTime(double timeMs, uint64_t events);
    
    // Record prediction computation time
    void recordPredictionTime(double timeMs);
    
    // Record component execution time
    void recordComponentTime(const std::string& component, double timeMs);
    
    // Get current metrics (averaged over collected data)
    PerformanceMetrics getCurrentMetrics() const;
    
    // Get snapshot of current metrics
    PerformanceMetrics getSnapshot() const;
    
    // Reset all counters
    void reset();
    
    // Check if performance is stable over a window
    bool isStable(size_t windowSize = 100, double threshold = 0.1) const;
    
    // Get step times
    const std::vector<double>& getStepTimes() const { return stepTimes_; }
    
    // Get spike counts per step
    const std::vector<uint64_t>& getSpikeCounts() const { return spikeCountsPerStep_; }
    
    // Get active neuron counts per step
    const std::vector<uint64_t>& getActiveNeuronCounts() const { return activeNeuronCountsPerStep_; }
    
    // Set performance metrics (for integration with main execution)
    void setPerformanceMetrics(const PerformanceMetrics& metrics) { currentMetrics_ = metrics; }
    
    // Get performance metrics
    const PerformanceMetrics& getPerformanceMetrics() const { return currentMetrics_; }
    
private:
    bool running_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    std::vector<double> stepTimes_;
    std::vector<uint64_t> spikeCountsPerStep_;
    std::vector<uint64_t> activeNeuronCountsPerStep_;
    
    // Component timing storage
    std::unordered_map<std::string, std::vector<double>> componentTimes_;
    
    // Running averages
    double avgStepTimeMs_;
    double avgSpikesPerStep_;
    double avgActiveNeurons_;
    
    // Counters
    uint64_t totalSteps_;
    uint64_t totalSpikes_;
    
    // Current metrics for output
    PerformanceMetrics currentMetrics_;
};

} // namespace nlm

#endif // NLM_PERFORMANCE_METRICS_HPP