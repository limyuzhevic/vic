// Benchmark CLI Tools
#ifndef NLM_BENCHMARK_CLI_H
#define NLM_BENCHMARK_CLI_H

#include <map>
#include <string>

class BenchmarkCli {
public:
    static void runBenchmark(const std::map<std::string, std::string>& options);
    static void analyzeBenchmark(const std::map<std::string, std::string>& options);
    static void compareBenchmarks(const std::map<std::string, std::string>& options);
    static void generateBenchmarkReport(const std::map<std::string, std::string>& options);
    static void exportBenchmark(const std::map<std::string, std::string>& options);
    static void importBenchmark(const std::map<std::string, std::string>& options);
    static void scheduleBenchmark(const std::map<std::string, std::string>& options);
    static void monitorBenchmark(const std::map<std::string, std::string>& options);
};

#endif // NLM_BENCHMARK_CLI_H
