// Benchmarker for NLM performance testing
// Refactored from monolithic main.cpp functionality

#pragma once

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <future>
#include <algorithm>

namespace nlm {
namespace examples {

// Benchmark configuration
struct BenchmarkConfig {
    std::string name;                    // Benchmark name
    size_t neuronCount;                  // Number of neurons
    size_t timestepCount;                // Number of timesteps to run
    std::string configFile;              // Configuration file
    bool verbose;                        // Verbose output
    size_t warmupSteps;                  // Warmup steps
    bool parallel;                       // Enable parallel processing
    size_t maxMemory;                    // Maximum memory usage in MB
    std::string outputFormat;            // Output format (json, csv, html)
    
    BenchmarkConfig()
        : neuronCount(1000), timestepCount(1000), verbose(false)
        , warmupSteps(100), parallel(false), maxMemory(1024), outputFormat("json") {}
};

// Benchmark results
struct BenchmarkResult {
    std::string benchmarkName;
    double cpuTime;                     // CPU time in seconds
    double wallTime;                    // Wall clock time in seconds
    double memoryUsage;                 // Memory usage in MB
    double spikeRate;                   // Spikes per second
    double operationsPerSecond;         // Operations per second
    size_t totalSpikes;                 // Total number of spikes
    size_t neuronCount;                 // Number of neurons
    size_t timestepCount;               // Number of timesteps
    double avgFiringRate;               // Average firing rate in Hz
    double stdDevFiringRate;            // Standard deviation of firing rate
    std::string timestamp;             // Timestamp of benchmark run
    bool success;                       // Whether benchmark succeeded
    std::string errorMessage;           // Error message if failed
    
    BenchmarkResult()
        : cpuTime(0.0), wallTime(0.0), memoryUsage(0.0)
        , spikeRate(0.0), operationsPerSecond(0.0)
        , totalSpikes(0), neuronCount(0), timestepCount(0)
        , avgFiringRate(0.0), stdDevFiringRate(0.0)
        , success(false) {}
};

// Benchmarker class for running performance benchmarks
class Benchmarker {
public:
    // Constructor
    Benchmarker();
    
    // Destructor
    ~Benchmarker();
    
    // Disable copying, enable moving
    Benchmarker(const Benchmarker&) = delete;
    Benchmarker& operator=(const Benchmarker&) = delete;
    Benchmarker(Benchmarker&&) noexcept;
    Benchmarker& operator=(Benchmarker&&) noexcept;
    
    // Configure benchmark
    void configure(const BenchmarkConfig& config);
    
    // Run benchmark
    BenchmarkResult runBenchmark();
    
    // Run multiple benchmarks
    std::vector<BenchmarkResult> runBenchmarks(const std::vector<BenchmarkConfig>& configs);
    
    // Export results to file
    bool exportResults(const std::vector<BenchmarkResult>& results, const std::string& filename);
    
    // Export results to file (JSON format)
    bool exportJsonResults(const std::vector<BenchmarkResult>& results, const std::string& filename);
    
    // Export results to file (CSV format)
    bool exportCsvResults(const std::vector<BenchmarkResult>& results, const std::string& filename);
    
    // Export results to file (HTML format)
    bool exportHtmlResults(const std::vector<BenchmarkResult>& results, const std::string& filename);
    
    // Generate comparison report
    std::string generateComparisonReport(const std::vector<BenchmarkResult>& results);
    
    // Get current benchmark configuration
    BenchmarkConfig getCurrentConfig() const;
    
    // Set verbose mode
    void setVerbose(bool verbose);
    
    // Set output format
    void setOutputFormat(const std::string& format);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper function to run benchmark with configuration
    BenchmarkResult runSingleBenchmark(const BenchmarkConfig& config);
    
    // Helper function to measure memory usage
    double getMemoryUsage() const;
    
    // Helper function to measure CPU time
    double getCpuTime() const;
    
    // Helper function to measure wall time
    double getWallTime() const;
};

}} // namespace nlm::examples