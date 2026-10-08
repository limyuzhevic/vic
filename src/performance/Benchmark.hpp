// Performance Benchmarking and Optimization Tools
// File: src/performance/Benchmark.hpp
#pragma once

#include "Profiler.hpp"
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace nlm {

class Benchmark {
public:
    struct BenchmarkResult {
        std::string name;
        std::string category;
        double duration_ms;
        double throughput;
        double memory_usage;
        double cpu_usage;
        double gflops;
        int64_t operations_per_second;
        double accuracy;
        bool passed;
        std::string details;
        
        BenchmarkResult(const std::string& n, const std::string& c, double dur, double thr = 0.0,
                       double mem = 0.0, double cpu = 0.0, double gflops = 0.0, 
                       int64_t ops = 0, double acc = 0.0, bool pass = true)
            : name(n), category(c), duration_ms(dur), throughput(thr), memory_usage(mem),
              cpu_usage(cpu), gflops(gflops), operations_per_second(ops), accuracy(acc),
              passed(pass) {}
    };

    Benchmark(const std::string& name = "Default Benchmark");
    ~Benchmark();
    
    // Run benchmark with operation count
    template<typename Func>
    BenchmarkResult run(const std::string& operation_name, const std::string& category,
                       Func&& operation, int iterations = 1000, 
                       bool validate = true, const std::string& validation_description = "");
    
    // Run benchmark with data
    template<typename Func, typename Data>
    BenchmarkResult runWithData(const std::string& operation_name, const std::string& category,
                               Func&& operation, Data&& data, int iterations = 1000,
                               bool validate = true, const std::string& validation_description = "");
    
    // Specialized benchmark functions
    static BenchmarkResult benchmarkJsonParsing(const std::string& content, int iterations = 1000);
    static BenchmarkResult benchmarkYamlParsing(const std::string& content, int iterations = 1000);
    static BenchmarkResult benchmarkConfigAccess(const nlm::Config& config, int iterations = 1000);
    static BenchmarkResult benchmarkNeuronLIF(const NeuronState& state, int iterations = 1000);
    static BenchmarkResult benchmarkSynapseTransmission(const std::vector<float>& weights, int iterations = 1000);
    static BenchmarkResult benchmarkMemoryAccess(const std::vector<std::pair<std::string, float>>& data, int iterations = 1000);
    static BenchmarkResult benchmarkRewardCalculation(const Observation& observation, int iterations = 1000);
    static BenchmarkResult benchmarkSTDPUpdate(const std::vector<Timestamp>& spikes, float deltaT, int iterations = 1000);
    static BenchmarkResult benchmarkMemoryConsolidation(int memory_size, int iterations = 1000);
    static BenchmarkResult benchmarkPredictionInference(const std::vector<float>& observations, int iterations = 1000);
    
    // Performance analysis
    std::vector<BenchmarkResult> getResults() const { return results_; }
    std::vector<BenchmarkResult> getResultsByCategory(const std::string& category) const;
    std::vector<BenchmarkResult> getResultsByPerformance() const;
    
    // Analysis methods
    double getAverageDurationByCategory(const std::string& category) const;
    double getThroughputByCategory(const std::string& category) const;
    BenchmarkResult getBestByDuration(const std::string& category) const;
    BenchmarkResult getBestByThroughput(const std::string& category) const;
    
    // Reporting
    std::string generateReport() const;
    void saveReport(const std::string& filename) const;
    void reset();
    
    // Configuration
    void setWarmupIterations(int iterations) { warmup_iterations_ = iterations; }
    void setMinIterations(int iterations) { min_iterations_ = iterations; }
    void setMaxIterations(int iterations) { max_iterations_ = iterations; }
    void setTimeoutMs(double timeout) { timeout_ms_ = timeout; }
    void enableDetailedTiming(bool enabled) { detailed_timing_ = enabled; }
    
private:
    std::vector<BenchmarkResult> results_;
    std::string benchmark_name_;
    int warmup_iterations_ = 10;
    int min_iterations_ = 100;
    int max_iterations_ = 10000;
    double timeout_ms_ = 5000.0;
    bool detailed_timing_ = true;
    
    double measureDuration(std::function<void()> operation) const;
    bool validateResult(const std::function<bool()>& validation, const std::string& description);
};

// Specialized benchmark suites for different components
class ConfigBenchmark {
public:
    static BenchmarkResult benchmarkJsonParsing(const std::string& content, int iterations = 1000);
    static BenchmarkResult benchmarkYamlParsing(const std::string& content, int iterations = 1000);
    static BenchmarkResult benchmarkConfigLoad(const std::string& file, int iterations = 1000);
    static BenchmarkResult benchmarkConfigAccess(const nlm::Config& config, const std::vector<std::string>& keys, int iterations = 1000);
    static BenchmarkResult benchmarkConfigValidation(const nlm::Config& config, int iterations = 1000);
    static BenchmarkResult benchmarkConfigInheritance(const std::vector<std::string>& files, int iterations = 1000);
};

class LoggerBenchmark {
public:
    static BenchmarkResult benchmarkLogEntryCreation(const std::string& message, int iterations = 10000);
    static BenchmarkResult benchmarkLogFormatting(const LogEntry& entry, int iterations = 10000);
    static BenchmarkResult benchmarkLogOutput(const std::string& filepath, int iterations = 1000);
    static BenchmarkResult benchmarkLogLevelFiltering(int iterations = 10000);
};

class RandomBenchmark {
public:
    static BenchmarkResult benchmarkUniformInt(int min, int max, int iterations = 100000);
    static BenchmarkResult benchmarkUniformReal(float min, float max, int iterations = 100000);
    static BenchmarkResult benchmarkNormal(float mean, float stddev, int iterations = 100000);
    static BenchmarkResult benchmarkBernoulli(float probability, int iterations = 100000);
    static BenchmarkResult benchmarkShuffle(const std::vector<int>& data, int iterations = 10000);
    static BenchmarkResult benchmarkRandomAccess(const std::vector<int>& data, int iterations = 100000);
};

class NeuronBenchmark {
public:
    static BenchmarkResult benchmarkLIFStep(const NeuronState& state, double dt, int iterations = 100000);
    static BenchmarkResult benchmarkSynapticInput(const std::vector<float>& inputs, int iterations = 100000);
    static BenchmarkResult benchmarkSpikeDetection(const NeuronState& state, int iterations = 100000);
    static BenchmarkResult benchmarkRefractoryUpdate(uint32_t remaining, uint32_t period, int iterations = 100000);
    static BenchmarkResult benchmarkTotalStep(const Neuron& neuron, Timestamp currentTime, int iterations = 10000);
};

class SynapseBenchmark {
public:
    static BenchmarkResult benchmarkTransmission(const std::vector<float>& weights, int iterations = 100000);
    static BenchmarkResult benchmarkPlasticityUpdate(float delta, float learningRate, int iterations = 100000);
    static BenchmarkResult benchmarkDelayProcessing(const std::vector<float>& delays, int iterations = 100000);
    static BenchmarkResult benchmarkStrengthUpdate(const std::vector<float>& strengths, int iterations = 100000);
};

class MemoryBenchmark {
public:
    static BenchmarkResult benchmarkWorkingMemoryAccess(const std::vector<std::pair<std::string, float>>& data, 
                                                       int iterations = 100000);
    static BenchmarkResult benchmarkEpisodicMemorySearch(const std::string& pattern, int iterations = 10000);
    static BenchmarkResult benchmarkMemoryConsolidation(const std::vector<float>& memories, int iterations = 1000);
    static BenchmarkResult benchmarkMemoryCompetition(const std::vector<float>& strengths, int iterations = 10000);
};

class NeuromodulatorBenchmark {
public:
    static BenchmarkResult benchmarkRewardPredictionError(const std::vector<float>& rewards, const std::vector<float>& values, int iterations = 10000);
    static BenchmarkResult benchmarkCuriosityUpdate(const std::vector<float>& uncertainties, const std::vector<float>& novelties, int iterations = 10000);
    static BenchmarkResult benchmarkNoveltyDetection(const std::vector<std::vector<float>>& patterns, int iterations = 10000);
    static BenchmarkResult benchmarkPredictionError(const std::vector<float>& predictions, const std::vector<float>& actuals, int iterations = 10000);
};

class PlasticityBenchmark {
public:
    static BenchmarkResult benchmarkSTDPUpdate(const std::vector<std::pair<Timestamp, Timestamp>>& spike_pairs, int iterations = 10000);
    static BenchmarkResult benchmarkHebbianLearning(const std::vector<float>& pre_synaptic, const std::vector<float>& post_synaptic, int iterations = 10000);
    static BenchmarkResult benchmarkStructuralPlasticity(const std::vector<std::pair<int, int>>& connections, int iterations = 1000);
    static BenchmarkResult benchmarkHomeostaticRegulation(const std::vector<float>& weights, int iterations = 1000);
};

class PredictionBenchmark {
public:
    static BenchmarkResult benchmarkForwardModel(const std::vector<std::vector<float>>& states, const std::vector<std::vector<float>>& actions, int iterations = 1000);
    static BenchmarkResult benchmarkInference(const std::vector<std::vector<float>>& observations, int iterations = 1000);
    static BenchmarkResult benchmarkPlanning(const std::vector<std::vector<std::vector<float>>>& trajectories, int iterations = 100);
    static BenchmarkResult benchmarkConfidenceCalculation(const std::vector<float>& predictions, int iterations = 10000);
};

// Global benchmark runner
class BenchmarkRunner {
public:
    static BenchmarkRunner& getInstance();
    
    void addBenchmark(const std::string& name, std::function<void(Benchmark&)> benchmark_func);
    void runAllBenchmarks();
    void runBenchmarksByCategory(const std::string& category);
    void saveBenchmarkResults(const std::string& filename) const;
    std::string generateBenchmarkReport() const;
    
    // Convenience methods for running specific benchmark suites
    void runConfigBenchmarks();
    void runNeuralBenchmarks();
    void runMemoryBenchmarks();
    void runNeuromodulationBenchmarks();
    void runPlasticityBenchmarks();
    void runPredictionBenchmarks();
    
private:
    BenchmarkRunner() = default;
    ~BenchmarkRunner() = default;
    BenchmarkRunner(const BenchmarkRunner&) = delete;
    BenchmarkRunner& operator=(const BenchmarkRunner&) = delete;
    
    std::vector<std::pair<std::string, std::function<void(Benchmark&)>>> benchmark_suites_;
};

} // namespace nlm
