#include "BenchmarkSuite.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// BenchmarkRunner implementation
void BenchmarkRunner::registerBenchmark(std::unique_ptr<IBenchmarkCase> benchmark) {
    benchmarks.push_back(std::move(benchmark));
}

BenchmarkResult BenchmarkRunner::runBenchmark(const std::string& name, const BenchmarkConfig& config) {
    for (const auto& benchmark : benchmarks) {
        if (benchmark->getName() == name) {
            if (!benchmark->validateConfig(config)) {
                return createFailedResult(benchmark->getName(), "Invalid configuration");
            }
            
            benchmark->setup(config);
            auto result = benchmark->run(config);
            benchmark->teardown(config);
            
            return result;
        }
    }
    
    return createFailedResult(name, "Benchmark not found");
}

std::vector<BenchmarkResult> BenchmarkRunner::runAllBenchmarks(const BenchmarkConfig& config) {
    std::vector<BenchmarkResult> results;
    
    for (const auto& benchmark : benchmarks) {
        if (config.verbose) {
            std::cout << "Running benchmark: " << benchmark->getName() << std::endl;
            std::cout << "Description: " << benchmark->getDescription() << std::endl;
        }
        
        results.push_back(runBenchmark(benchmark->getName(), config));
        
        if (config.verbose) {
            std::cout << "Result: " << results.back().status << " (Score: " << results.back().score << ")" << std::endl;
        }
    }
    
    return results;
}

BenchmarkStatistics BenchmarkRunner::calculateStatistics(const std::vector<BenchmarkResult>& results) {
    BenchmarkStatistics stats;
    
    if (results.empty()) {
        return stats;
    }
    
    for (const auto& result : results) {
        if (result.status == "PASS") {
            stats.successfulIterations++;
        } else {
            stats.failedIterations++;
        }
        
        stats.totalIterations++;
        
        // Update timing statistics
        if (result.wallClockTime < stats.minWallClockTime) {
            stats.minWallClockTime = result.wallClockTime;
        }
        if (result.wallClockTime > stats.maxWallClockTime) {
            stats.maxWallClockTime = result.wallClockTime;
        }
        
        // Update firing rate statistics
        if (result.averageFiringRate < stats.minFiringRate) {
            stats.minFiringRate = result.averageFiringRate;
        }
        if (result.averageFiringRate > stats.maxFiringRate) {
            stats.maxFiringRate = result.averageFiringRate;
        }
        
        // Update memory statistics
        if (result.peakMemoryUsage < stats.minMemoryUsage) {
            stats.minMemoryUsage = result.peakMemoryUsage;
        }
        if (result.peakMemoryUsage > stats.maxMemoryUsage) {
            stats.maxMemoryUsage = result.peakMemoryUsage;
        }
        
        // Update score statistics
        if (result.score < stats.minScore) {
            stats.minScore = result.score;
        }
        if (result.score > stats.maxScore) {
            stats.maxScore = result.score;
        }
    }
    
    // Calculate averages and standard deviations
    if (stats.totalIterations > 0) {
        double totalWallClock = 0;
        double totalFiringRate = 0;
        double totalMemory = 0;
        double totalScore = 0;
        
        for (const auto& result : results) {
            totalWallClock += result.wallClockTime;
            totalFiringRate += result.averageFiringRate;
            totalMemory += result.peakMemoryUsage;
            totalScore += result.score;
        }
        
        stats.averageWallClockTime = totalWallClock / stats.totalIterations;
        stats.averageFiringRate = totalFiringRate / stats.totalIterations;
        stats.averageMemoryUsage = totalMemory / stats.totalIterations;
        stats.averageScore = totalScore / stats.totalIterations;
        
        // Calculate standard deviations
        double wallClockVariance = 0;
        double firingRateVariance = 0;
        double memoryVariance = 0;
        double scoreVariance = 0;
        
        for (const auto& result : results) {
            wallClockVariance += std::pow(result.wallClockTime - stats.averageWallClockTime, 2);
            firingRateVariance += std::pow(result.averageFiringRate - stats.averageFiringRate, 2);
            memoryVariance += std::pow(result.peakMemoryUsage - stats.averageMemoryUsage, 2);
            scoreVariance += std::pow(result.score - stats.averageScore, 2);
        }
        
        stats.stdDevWallClockTime = std::sqrt(wallClockVariance / stats.totalIterations);
        stats.stdDevFiringRate = std::sqrt(firingRateVariance / stats.totalIterations);
        stats.stdDevFiringRate = std::sqrt(memoryVariance / stats.totalIterations);
        stats.stdDevFiringRate = std::sqrt(scoreVariance / stats.totalIterations);
    }
    
    // Determine overall status
    if (stats.failedIterations == 0) {
        stats.overallStatus = "ALL_PASS";
    } else if (stats.successfulIterations == 0) {
        stats.overallStatus = "ALL_FAIL";
    } else {
        stats.overallStatus = "MIXED";
    }
    
    return stats;
}

std::string BenchmarkRunner::generateReport(const std::vector<BenchmarkResult>& results, const BenchmarkStatistics& stats) {
    std::ostringstream oss;
    
    oss << "=== NLM Benchmark Suite Report ===\n";
    oss << "Generated: " << getCurrentTimestamp() << "\n";
    oss << "Total Benchmarks Run: " << results.size() << "\n";
    oss << "Successful: " << stats.successfulIterations << "\n";
    oss << "Failed: " << stats.failedIterations << "\n";
    oss << "Overall Status: " << stats.overallStatus << "\n";
    
    oss << "\nBenchmark Results:\n";
    for (const auto& result : results) {
        oss << "  " << result.testName << ": " << result.status 
            << " (Score: " << result.score << "%, Time: " << result.wallClockTime << "us)";
        if (!result.details.empty()) {
            oss << " - " << result.details;
        }
        oss << "\n";
    }
    
    oss << "\nPerformance Statistics:\n";
    oss << "  Wall Clock Time: " << stats.averageWallClockTime << "us (±" << stats.stdDevWallClockTime << "us)\n";
    oss << "  Firing Rate: " << stats.averageFiringRate << "Hz (±" << stats.stdDevFiringRate << "Hz)\n";
    oss << "  Memory Usage: " << stats.averageMemoryUsage << " bytes (±" << stats.stdDevFiringRate << " bytes)\n";
    oss << "  Performance Score: " << stats.averageScore << "\n";
    
    return oss.str();
}

void BenchmarkRunner::saveResultsToFile(const std::vector<BenchmarkResult>& results, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    // Write CSV header
    file << "Benchmark,Iteration,Status,WallClockTime,CPUTime,ExecutionTime,SetupTime,TeardownTime,AverageFiringRate,PeakFiringRate,NeuronCount,SynapseCount,SpikeEvents,PeakMemory,CurrentMemory,SpikesPerNeuron,SynapticEfficiency,PlasticityRate,Score,Details\n";
    
    for (const auto& result : results) {
        file << result.testName << "," << result.iteration << "," << result.status << "," << result.wallClockTime << "," << result.cpuTime << "," << result.executionTime << "," << result.setupTime << "," << result.teardownTime << "," 
             << result.averageFiringRate << "," << result.peakFiringRate << "," << result.neuronCount << "," << result.synapseCount << "," << 
             result.spikeEventCount << "," << result.peakMemoryUsage << "," << result.currentMemoryUsage << "," << result.spikesPerNeuron << "," << 
             result.synapticEfficiency << "," << result.plasticityRate << "," << result.score << ",";
        
        // Escape quotes in details
        std::string escapedDetails = result.details;
        std::replace(escapedDetails.begin(), escapedDetails.end(), '"', '"');
        file << '"' << escapedDetails << '"' << "\n";
    }
    
    file.close();
}

std::vector<std::string> BenchmarkRunner::generateOptimizationRecommendations(const BenchmarkStatistics& stats) {
    std::vector<std::string> recommendations;
    
    if (stats.overallStatus == "ALL_FAIL") {
        recommendations.push_back("All benchmarks failed. Review configuration and system resources.");
        recommendations.push_back("Consider increasing benchmark duration or reducing test complexity.");
    } else if (stats.overallStatus == "MIXED") {
        recommendations.push_back("Some benchmarks failed. Focus on failing benchmarks first.");
        recommendations.push_back("Analyze common failure patterns to identify system bottlenecks.");
    } else if (stats.averageScore < 70.0f) {
        recommendations.push_back("Performance score is below target. Consider the following optimizations:");
        recommendations.push_back("  - Profile failing benchmarks to identify bottlenecks");
        recommendations.push_back("  - Optimize memory usage patterns");
        recommendations.push_back("  - Reduce synchronization overhead");
        recommendations.push_back("  - Consider hardware-specific optimizations (SIMD, threading)");
    } else if (stats.stdDevWallClockTime > stats.averageWallClockTime * 0.5) {
        recommendations.push_back("High variability in benchmark results. This may indicate:");
        recommendations.push_back("  - Resource contention (CPU, memory, I/O)");
        recommendations.push_back("  - Random effects in the simulation");
        recommendations.push_back("  - Insufficient warm-up iterations");
    } else {
        recommendations.push_back("Performance is within acceptable ranges. No immediate optimizations needed.");
        recommendations.push_back("Consider stress testing to identify potential bottlenecks.");
    }
    
    return recommendations;
}

BenchmarkResult BenchmarkRunner::createFailedResult(const std::string& testName, const std::string& error) {
    BenchmarkResult result;
    result.testName = testName;
    result.status = "ERROR";
    result.details = error;
    return result;
}

std::string BenchmarkRunner::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Convenience functions implementation
void Benchmark::registerDefaultBenchmarks() {
    globalRunner.registerBenchmark(std::make_unique<NeuronBenchmark>());
    globalRunner.registerBenchmark(std::make_unique<SpikeBenchmark>());
    globalRunner.registerBenchmark(std::make_unique<PlasticityBenchmark>());
    globalRunner.registerBenchmark(std::make_unique<MemoryBenchmark>());
}

std::vector<BenchmarkResult> Benchmark::runBenchmarks(const BenchmarkConfig& config) {
    return globalRunner.runAllBenchmarks(config);
}

BenchmarkResult Benchmark::runBenchmark(const std::string& name, const BenchmarkConfig& config) {
    return globalRunner.runBenchmark(name, config);
}

std::string Benchmark::generateReport(const std::vector<BenchmarkResult>& results, const BenchmarkStatistics& stats) {
    return globalRunner.generateReport(results, stats);
}

void Benchmark::saveResults(const std::vector<BenchmarkResult>& results, const std::string& filename) {
    globalRunner.saveResultsToFile(results, filename);
}

std::vector<std::string> Benchmark::getOptimizationRecommendations(const BenchmarkStatistics& stats) {
    return globalRunner.generateOptimizationRecommendations(stats);
}

} // namespace nlm
