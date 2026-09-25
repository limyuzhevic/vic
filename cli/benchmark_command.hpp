// NLM Benchmark Command - Run performance benchmarks
// Handles performance benchmarking of NLM operations

#pragma once

#include "cli/nlm_cli.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <sstream>

namespace nlm {

class BenchmarkCommand {
public:
    static bool execute(NLMCLI& cli, const std::map<std::string, std::string>& args);
    static void showHelp();
    static std::vector<BenchmarkResult> runBenchmarks(const std::vector<std::pair<size_t, size_t>>& benchmarks, std::shared_ptr<Config> config);
    static void printBenchmarkResults(const std::vector<BenchmarkResult>& results);
    static std::string formatBenchmarkNumber(double value, int precision = 2);
    static std::string formatBenchmarkTime(double seconds);
    static bool saveBenchmarkResults(const std::string& format, const std::string& filename, const std::vector<BenchmarkResult>& results);
    
private:
    struct BenchmarkResult {
        size_t neurons;
        size_t synapses;
        size_t steps;
        double totalTime;      // seconds
        double stepsPerSecond;
        double spikesPerStep;
        double synapsesPerNeuron;
    };
};
