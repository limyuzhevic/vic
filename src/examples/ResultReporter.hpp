// Result reporting and analysis utilities
// Refactored from monolithic main.cpp functionality

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>

namespace nlm {
namespace examples {

// Performance metrics for benchmarking
struct PerformanceMetrics {
    double cpuTime;           // CPU time in seconds
    double memoryUsage;       // Memory usage in MB
    double simulationTime;    // Simulation time in seconds
    double spikeRate;         // Spikes per second
    double throughput;        // Operations per second
    size_t totalOperations;
    std::string timestamp;
    
    PerformanceMetrics()
        : cpuTime(0.0), memoryUsage(0.0), simulationTime(0.0)
        , spikeRate(0.0), throughput(0.0), totalOperations(0) {}
};

// Result reporter for experiments and benchmarks
class ResultReporter {
public:
    // Print results to console
    static void printConsole(const std::string& title, const std::string& content);
    
    // Export results to file (JSON format)
    static bool exportJson(const std::string& filename, const std::string& content);
    
    // Export results to file (CSV format)
    static bool exportCsv(const std::string& filename, const std::vector<std::string>& headers, 
                         const std::vector<std::vector<std::string>>& data);
    
    // Export results to file (HTML format)
    static bool exportHtml(const std::string& filename, const std::string& title, 
                          const std::string& content);
    
    // Generate performance report
    static std::string generatePerformanceReport(const PerformanceMetrics& metrics);
    
    // Generate experiment report
    static std::string generateExperimentReport(const std::string& experimentName,
                                                 const std::vector<std::string>& parameters,
                                                 const std::string& results);
    
    // Generate comparison report for multiple runs
    static std::string generateComparisonReport(const std::vector<std::string>& experimentNames,
                                                 const std::vector<std::string>& resultContents);
    
    // Print system information
    static void printSystemInfo();
    
    // Print timestamp
    static std::string getCurrentTimestamp();
    
    // Format duration for display
    static std::string formatDuration(double seconds);
    
    // Format number with appropriate units
    static std::string formatNumber(double number, const std::string& unit = "");
};

}} // namespace nlm::examples