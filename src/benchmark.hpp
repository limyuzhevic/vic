#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <string>
#include <vector>

namespace nlm {
    
    // Benchmark result structure for performance measurements
    struct BenchmarkResult {
        std::string name;
        size_t neurons;
        size_t synapses;
        size_t steps;
        double totalTime;
        double opsPerSecond;
        double memoryUsage;
        std::vector<double> metrics;
    };
    
    // Benchmark suite interface
    class NLMBenchmarkSuite {
    public:
        static void runBenchmarks();
        static void runScalabilityTests();
        static void runMemoryUsageTests();
        static void runIntegrationPerformanceTests();
        
    private:
        static void benchmarkNeuronCreation();
        static void benchmarkSynapseFormation();
        static void benchmarkSpikeDelivery();
        static void benchmarkMemoryOperations();
        static void benchmarkHebbianUpdates();
        static void benchmarkStructuralPlasticity();
        static void benchmarkSynaptogenesis();
        static void benchmarkPruning();
        static void benchmarkMaturation();
        static void benchmarkCuriosity();
        static void benchmarkNovelty();
        static void benchmarkPredictionError();
        static void benchmarkWorkingMemory();
        static void benchmarkEpisodicMemory();
        static void benchmarkFullBrainSimulation();
        
        static void printBenchmarkResult(const BenchmarkResult& result);
        static void printBenchmarkReport(const std::vector<BenchmarkResult>& results);
        static void printScalabilityReport(const std::vector<BenchmarkResult>& results);
        static bool containsMemoryMetrics(const std::vector<BenchmarkResult>& results);
        static void saveBenchmarkResults(const std::vector<BenchmarkResult>& results, const std::string& filename);
    };
    
} // namespace nlm

#endif // BENCHMARK_HPP
