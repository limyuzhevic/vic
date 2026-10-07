#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

// Monitoring configuration
struct MonitoringConfig {
    // Monitoring scope
    bool monitorNeurons = true;            // Monitor neuron activity
    bool monitorSynapses = true;           // Monitor synaptic activity
    bool monitorMemory = true;             // Monitor memory systems
    bool monitorCognition = true;          // Monitor cognitive processes
    bool monitorDevelopment = true;        // Monitor development processes
    bool monitorNeuromodulation = true;     // Monitor neuromodulation systems
    
    // Monitoring frequency
    size_t monitoringInterval = 100;        // Monitoring interval in steps
    bool realTimeMonitoring = false;        // Enable real-time monitoring
    double realTimeUpdateRate = 10.0;       // Real-time update rate in Hz
    
    // Metrics to track
    bool trackFiringRate = true;           // Track firing rates
    bool trackSpikeCount = true;           // Track spike counts
    bool trackMemoryUsage = true;          // Track memory usage
    bool trackPerformance = true;          // Track performance metrics
    bool trackConnectivity = true;         // Track connectivity changes
    
    // Output control
    bool enableLogging = true;             // Enable logging
    std::string logLevel = "info";         // Log level (debug, info, warning, error)
    std::string logFormat = "text";        // Log format (text, json, csv)
    bool saveToFile = false;                // Save monitoring data to file
    std::string outputFile = "./monitoring.log";  // Output file path
    
    // Visualization
    bool enableVisualization = false;       // Enable visualization
    std::string visualizationTool = "matplotlib";  // Visualization tool
    bool savePlots = false;                 // Save plots to file
    std::string plotFormat = "png";        // Plot format
    
    // Custom metrics
    std::unordered_map<std::string, bool> customMetrics;  // Custom metrics to track
    std::unordered_map<std::string, std::string> metricCategories;  // Metric categories
    
    // Filtering
    bool filterHighMetrics = true;          // Filter out high metrics
    bool filterLowMetrics = true;           // Filter out low metrics
    float highMetricThreshold = 1.0f;       // High metric threshold
    float lowMetricThreshold = 0.0f;        // Low metric threshold
    
    MonitoringConfig() : monitorNeurons(true), monitorSynapses(true), monitorMemory(true),
                        monitorCognition(true), monitorDevelopment(true), monitorNeuromodulation(true),
                        monitoringInterval(100), realTimeMonitoring(false), realTimeUpdateRate(10.0),
                        trackFiringRate(true), trackSpikeCount(true), trackMemoryUsage(true),
                        trackPerformance(true), trackConnectivity(true),
                        enableLogging(true), logLevel("info"), logFormat("text"),
                        saveToFile(false), outputFile("./monitoring.log"),
                        enableVisualization(false), visualizationTool("matplotlib"),
                        savePlots(false), plotFormat("png") {}
};

} // namespace nlm