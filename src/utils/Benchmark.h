// Benchmark.h
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include "core/Config/Config.hpp"

struct BenchmarkResult {
    std::string operation;
    double duration;  // seconds
    size_t iterations;
    double averageDuration;
    double minDuration;
    double maxDuration;
    std::vector<double> samples;
};

class Benchmark {
public:
    Benchmark();
    ~Benchmark() = default;
    
    // Set configuration
    void setConfig(std::shared_ptr<nlm::Config> config);
    
    // Start timing
    void start();
    
    // Stop timing and record result
    void stop(const std::string& operation = "default");
    
    // Get all results
    std::vector<BenchmarkResult> getResults() const;
    
    // Print results
    void printResults() const;
    
    // Clear results
    void clear();
    
    // Get total time
    double getTotalTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
