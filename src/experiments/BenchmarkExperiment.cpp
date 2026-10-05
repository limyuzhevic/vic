#include "BenchmarkExperiment.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

namespace nlm {

BenchmarkExperiment::BenchmarkExperiment(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
    : Experiment(brain, config) {
    profiler_ = std::make_shared<PerformanceProfiler>();
}

bool BenchmarkExperiment::setup() {
    std::cout << "Setting up benchmark experiment..." << std::endl;
    
    // Configure profiler with default benchmark settings
    benchmarkConfig_ = PerformanceProfiler::createCpuBenchmarkConfig(std::chrono::seconds(60));
    benchmarkConfig_.detailed = true;
    benchmarkConfig_.outputFile = "benchmark_results.json";
    
    std::cout << "Benchmark configured with " << benchmarkConfig_.iterations << " iterations" << std::endl;
    return true;
}

bool BenchmarkExperiment::run() {
    std::cout << "Running benchmark experiment..." << std::endl;
    
    // Run CPU benchmark
    if (!runCpuBenchmark(std::chrono::seconds(60))) {
        std::cout << "CPU benchmark failed" << std::endl;
        return false;
    }
    
    // Run memory benchmark
    if (!runMemoryBenchmark(std::chrono::seconds(30))) {
        std::cout << "Memory benchmark failed" << std::endl;
        return false;
    }
    
    // Run scalability benchmark
    if (!runScalabilityBenchmark(std::chrono::seconds(120))) {
        std::cout << "Scalability benchmark failed" << std::endl;
        return false;
    }
    
    std::cout << "Benchmark experiment completed successfully" << std::endl;
    return true;
}

bool BenchmarkExperiment::cleanup() {
    std::cout << "Cleaning up benchmark experiment..." << std::endl;
    
    // Save results
    std::string report = generateReport();
    std::cout << "Benchmark Report:" << std::endl;
    std::cout << report << std::endl;
    
    // Export results
    if (!benchmarkConfig_.outputFile.empty()) {
        if (exportResults(benchmarkConfig_.outputFile, benchmarkConfig_.format)) {
            std::cout << "Results exported to " << benchmarkConfig_.outputFile << std::endl;
        } else {
            std::cout << "Failed to export results" << std::endl;
        }
    }
    
    return true;
}

bool BenchmarkExperiment::runCpuBenchmark(std::chrono::seconds duration) {
    std::cout << "\n=== Running CPU Benchmark ===" << std::endl;
    std::cout << "Duration: " << std::chrono::duration_cast<std::chrono::seconds>(duration).count() << " seconds" << std::endl;
    
    BenchmarkConfig cpuConfig = PerformanceProfiler::createCpuBenchmarkConfig(duration);
    
    if (profiler_->runBenchmark(cpuConfig, "cpu_benchmark")) {
        std::cout << "CPU benchmark completed successfully" << std::endl;
        
        // Store results
        auto results = profiler_->getPerformanceData();
        for (const auto& result : results) {
            BenchmarkResult benchmarkResult;
            benchmarkResult.benchmarkName = "cpu_benchmark";
            benchmarkResult.executionTime = result.wallTime.count();
            benchmarkResult.throughput = result.throughput;
            benchmarkResult.memoryUsage = result.memoryUsage;
            benchmarkResult.peakMemoryUsage = result.peakMemoryUsage;
            benchmarkResult.efficiency = profiler_->calculateEfficiency(result, cpuConfig);
            benchmarkResult.operationsCompleted = result.instructionCount;
            benchmarkResult.timestamp = std::chrono::system_clock::now();
            
            results_.push_back(benchmarkResult);
        }
        
        return true;
    }
    
    return false;
}

bool BenchmarkExperiment::runMemoryBenchmark(std::chrono::seconds duration) {
    std::cout << "\n=== Running Memory Benchmark ===" << std::endl;
    std::cout << "Duration: " << std::chrono::duration_cast<std::chrono::seconds>(duration).count() << " seconds" << std::endl;
    
    BenchmarkConfig memoryConfig = PerformanceProfiler::createMemoryBenchmarkConfig(duration);
    
    if (profiler_->runBenchmark(memoryConfig, "memory_benchmark")) {
        std::cout << "Memory benchmark completed successfully" << std::endl;
        
        // Store results
        auto results = profiler_->getPerformanceData();
        for (const auto& result : results) {
            BenchmarkResult benchmarkResult;
            benchmarkResult.benchmarkName = "memory_benchmark";
            benchmarkResult.executionTime = result.wallTime.count();
            benchmarkResult.throughput = result.throughput;
            benchmarkResult.memoryUsage = result.memoryUsage;
            benchmarkResult.peakMemoryUsage = result.peakMemoryUsage;
            benchmarkResult.efficiency = profiler_->calculateEfficiency(result, memoryConfig);
            benchmarkResult.operationsCompleted = result.instructionCount;
            benchmarkResult.timestamp = std::chrono::system_clock::now();
            
            results_.push_back(benchmarkResult);
        }
        
        return true;
    }
    
    return false;
}

bool BenchmarkExperiment::runScalabilityBenchmark(std::chrono::seconds duration) {
    std::cout << "\n=== Running Scalability Benchmark ===" << std::endl;
    std::cout << "Duration: " << std::chrono::duration_cast<std::chrono::seconds>(duration).count() << " seconds" << std::endl;
    
    BenchmarkConfig scalabilityConfig = PerformanceProfiler::createScalabilityBenchmarkConfig(duration);
    
    if (profiler_->runBenchmark(scalabilityConfig, "scalability_benchmark")) {
        std::cout << "Scalability benchmark completed successfully" << std::endl;
        
        // Store results
        auto results = profiler_->getPerformanceData();
        for (const auto& result : results) {
            BenchmarkResult benchmarkResult;
            benchmarkResult.benchmarkName = "scalability_benchmark";
            benchmarkResult.executionTime = result.wallTime.count();
            benchmarkResult.throughput = result.throughput;
            benchmarkResult.memoryUsage = result.memoryUsage;
            benchmarkResult.peakMemoryUsage = result.peakMemoryUsage;
            benchmarkResult.efficiency = profiler_->calculateEfficiency(result, scalabilityConfig);
            benchmarkResult.operationsCompleted = result.instructionCount;
            benchmarkResult.timestamp = std::chrono::system_clock::now();
            
            results_.push_back(benchmarkResult);
        }
        
        return true;
    }
    
    return false;
}

std::string BenchmarkExperiment::generateReport() const {
    std::stringstream ss;
    ss << "NLM Benchmark Experiment Report" << std::endl;
    ss << std::string(50, '=') << std::endl << std::endl;
    
    ss << "Total Benchmark Runs: " << results_.size() << std::endl;
    ss << std::endl;
    
    if (results_.empty()) {
        return ss.str();
    }
    
    ss << "Benchmark Summary:" << std::endl;
    ss << "----------------" << std::endl;
    
    double totalExecutionTime = 0;
    double totalThroughput = 0;
    double totalMemory = 0;
    double totalEfficiency = 0;
    
    for (size_t i = 0; i < results_.size(); ++i) {
        const auto& result = results_[i];
        
        totalExecutionTime += result.executionTime;
        totalThroughput += result.throughput;
        totalMemory += result.memoryUsage;
        totalEfficiency += result.efficiency;
        
        ss << std::setw(3) << (i + 1) << ". " << result.benchmarkName << std::endl;
        ss << std::setw(6) << "  Execution Time: " << std::fixed << std::setprecision(4) 
           << result.executionTime << " s" << std::endl;
        ss << std::setw(6) << "  Throughput: " << std::fixed << std::setprecision(2)
           << result.throughput << " ops/sec" << std::endl;
        ss << std::setw(6) << "  Memory Usage: " << (result.memoryUsage / 1024 / 1024) << " MB" << std::endl;
        ss << std::setw(6) << "  Peak Memory: " << (result.peakMemoryUsage / 1024 / 1024) << " MB" << std::endl;
        ss << std::setw(6) << "  Efficiency: " << std::fixed << std::setprecision(2) << result.efficiency << "%" << std::endl;
        ss << std::setw(6) << "  Operations: " << result.operationsCompleted << std::endl;
        ss << std::endl;
    }
    
    ss << "Overall Statistics:" << std::endl;
    ss << "-------------------" << std::endl;
    ss << "Average Execution Time: " << std::fixed << std::setprecision(4)
       << (totalExecutionTime / results_.size()) << " s" << std::endl;
    ss << "Average Throughput: " << std::fixed << std::setprecision(2)
       << (totalThroughput / results_.size()) << " ops/sec" << std::endl;
    ss << "Average Memory Usage: " << std::fixed << std::setprecision(2)
       << (totalMemory / results_.size() / 1024 / 1024) << " MB" << std::endl;
    ss << "Average Efficiency: " << std::fixed << std::setprecision(2)
       << (totalEfficiency / results_.size()) << "%" << std::endl;
    
    return ss.str();
}

bool BenchmarkExperiment::exportResults(const std::string& filepath, const std::string& format) const {
    if (format == "json") {
        nlohmann::json jsonData;
        
        jsonData["experiment_name"] = "benchmark_experiment";
        jsonData["run_timestamp"] = std::chrono::system_clock::now();
        jsonData["total_runs"] = results_.size();
        
        nlohmann::json resultsJson;
        for (const auto& result : results_) {
            nlohmann::json resultJson;
            resultJson["benchmark_name"] = result.benchmarkName;
            resultJson["execution_time"] = result.executionTime;
            resultJson["throughput"] = result.throughput;
            resultJson["memory_usage"] = result.memoryUsage;
            resultJson["peak_memory_usage"] = result.peakMemoryUsage;
            resultJson["efficiency"] = result.efficiency;
            resultJson["operations_completed"] = result.operationsCompleted;
            resultJson["timestamp"] = result.timestamp;
            
            resultsJson.push_back(resultJson);
        }
        
        jsonData["results"] = resultsJson;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(2) << jsonData << std::endl;
        
    } else {
        // Default to text format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateReport();
    }
    
    return true;
}

} // namespace nlm
