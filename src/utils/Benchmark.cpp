// Benchmark.cpp
#include "utils/Benchmark.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

struct Benchmark::Impl {
    std::shared_ptr<nlm::Config> config;
    std::chrono::high_resolution_clock::time_point startTime;
    double totalTime;
    std::vector<BenchmarkResult> results;
};

Benchmark::Benchmark() : pImpl(std::make_unique<Impl>()) {
    pImpl->totalTime = 0.0;
}

void Benchmark::setConfig(std::shared_ptr<nlm::Config> config) {
    pImpl->config = config;
}

void Benchmark::start() {
    pImpl->startTime = std::chrono::high_resolution_clock::now();
}

void Benchmark::stop(const std::string& operation) {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - pImpl->startTime);
    
    double seconds = duration.count() / 1000000.0;
    pImpl->totalTime += seconds;
    
    BenchmarkResult result;
    result.operation = operation;
    result.duration = seconds;
    result.iterations = 1;
    result.averageDuration = seconds;
    result.minDuration = seconds;
    result.maxDuration = seconds;
    result.samples.push_back(seconds);
    
    pImpl->results.push_back(result);
}

std::vector<BenchmarkResult> Benchmark::getResults() const {
    return pImpl->results;
}

void Benchmark::printResults() const {
    if (pImpl->results.empty()) {
        std::cout << "No benchmark results available.\n";
        return;
    }
    
    std::cout << "\nBenchmark Results:" << std::endl;
    std::cout << std::left << std::setw(20) << "Operation"
              << std::setw(15) << "Duration (s)"
              << std::setw(15) << "Average (s)"
              << std::setw(12) << "Min (s)"
              << std::setw(12) << "Max (s)"
              << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    for (const auto& result : pImpl->results) {
        std::cout << std::left << std::setw(20) << result.operation
                  << std::setw(15) << std::setprecision(6) << result.duration
                  << std::setw(15) << std::setprecision(6) << result.averageDuration
                  << std::setw(12) << std::setprecision(6) << result.minDuration
                  << std::setw(12) << std::setprecision(6) << result.maxDuration
                  << std::endl;
    }
    
    std::cout << std::endl << "Total time: " << std::setprecision(6) << pImpl->totalTime << " seconds\n";
}

void Benchmark::clear() {
    pImpl->results.clear();
    pImpl->totalTime = 0.0;
}

double Benchmark::getTotalTime() const {
    return pImpl->totalTime;
}
