#include "Benchmark.hpp"

namespace nlm {
namespace benchmark {

// Statistics implementation

double Statistics::mean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double Statistics::variance(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    double m = mean(values);
    double sq_sum = std::accumulate(values.begin(), values.end(), 0.0,
        [m](double sum, double val) { return sum + (val - m) * (val - m); });
    return sq_sum / values.size();
}

double Statistics::standardDeviation(const std::vector<double>& values) {
    return std::sqrt(variance(values));
}

double Statistics::median(std::vector<double> values) {
    if (values.empty()) return 0.0;
    std::sort(values.begin(), values.end());
    size_t n = values.size();
    if (n % 2 == 0) {
        return (values[n/2 - 1] + values[n/2]) / 2.0;
    } else {
        return values[n/2];
    }
}

double Statistics::percentile(std::vector<double> values, double p) {
    if (values.empty()) return 0.0;
    std::sort(values.begin(), values.end());
    double percentile = (p / 100.0) * (values.size() - 1);
    size_t lower = static_cast<size_t>(std::floor(percentile));
    size_t upper = static_cast<size_t>(std::ceil(percentile));
    
    if (lower == upper) {
        return values[lower];
    }
    
    double weight = percentile - lower;
    return values[lower] * (1.0 - weight) + values[upper] * weight;
}

std::vector<double> Statistics::movingAverage(const std::vector<double>& values, size_t windowSize) {
    std::vector<double> result;
    if (values.empty() || windowSize == 0) return result;
    
    size_t halfWindow = windowSize / 2;
    result.reserve(values.size());
    
    for (size_t i = 0; i < values.size(); ++i) {
        size_t start = std::max(size_t(0), i - halfWindow);
        size_t end = std::min(values.size(), i + halfWindow + 1);
        
        double sum = std::accumulate(values.begin() + start, values.begin() + end, 0.0);
        result.push_back(sum / (end - start));
    }
    
    return result;
}

double Statistics::confidenceInterval(const std::vector<double>& values, double confidence) {
    if (values.empty()) return 0.0;
    double mean = Statistics::mean(values);
    double stddev = Statistics::standardDeviation(values);
    double alpha = 1.0 - confidence;
    double z = 1.96; // For 95% confidence
    double margin = z * stddev / std::sqrt(values.size());
    return margin;
}

bool Statistics::isSignificant(double pValue, double alpha) {
    return pValue < alpha;
}

// PerformanceMonitor implementation

void PerformanceMonitor::stop(BenchmarkMetrics& metrics) {
    double wallClock = getElapsedWallClock();
    double cpuTime = getElapsedCPU();
    
    metrics.wallClockTime = wallClock;
    metrics.cpuTime = cpuTime;
    metrics.operations = operationCount.load();
    metrics.memoryBytes = static_cast<size_t>(totalMemoryUsage.load());
    metrics.cacheHits = cacheHits.load();
    metrics.cacheMisses = cacheMisses.load();
    metrics.cacheHitRatio = getCacheHitRatio();
    
    // Calculate derived metrics
    if (wallClock > 0.0) {
        metrics.opsPerSecond = static_cast<double>(operationCount.load()) / wallClock;
        metrics.memoryBandwidth = static_cast<double>(totalMemoryUsage.load()) / wallClock;
    }
    
    if (cpuTime > 0.0) {
        metrics.flopsPerSecond = static_cast<double>(operationCount.load() * 2) / cpuTime; // Assume 2 FLOPs per operation
    }
}

// MemoryProfiler implementation

void MemoryProfiler::recordAllocation(size_t size, void* address, const std::string& site) {
    std::lock_guard<std::mutex> lock(mutex);
    AllocationInfo info;
    info.size = size;
    info.address = address;
    info.timestamp = std::chrono::high_resolution_clock::now();
    info.allocationSite = site;
    allocations.push_back(info);
}

deallocate(size_t size) {
    totalAllocations.fetch_add(1);
    totalMemoryUsage.fetch_add(static_cast<double>(size));
    MemoryProfiler::instance().recordAllocation(size, ptr, "unknown");
}

deallocateFree(void* ptr, size_t size) {
    totalDeallocations.fetch_add(1);
    totalMemoryUsage.fetch_sub(static_cast<double>(size));
    MemoryProfiler::instance().recordDeallocation(ptr, size);
}

namespace memory_pool {
    void* allocate(size_t size) {
        void* ptr = std::malloc(size);
        if (ptr) {
            MemoryProfiler::instance().recordAllocation(size, ptr, "memory_pool");
        }
        return ptr;
    }
    
    void deallocate(void* ptr, size_t size) {
        if (ptr) {
            MemoryProfiler::instance().recordDeallocation(ptr, size);
            std::free(ptr);
        }
    }
}

// BenchmarkRunner implementation

BenchmarkRunner::BenchmarkRunner(const std::string& outputDir, const std::string& name, bool reports, bool charts)
    : outputDirectory(outputDir), benchmarkName(name), generateReports(reports), generateCharts(charts) {}

void BenchmarkRunner::addResult(const BenchmarkMetrics& metrics) {
    results.push_back(metrics);
}

void BenchmarkRunner::runBenchmarkSuite() {
    runCoreBenchmarks();
    runScalabilityBenchmarks();
    runComparativeBenchmarks();
    runAdvancedMetricsBenchmarks();
    runIntegrationBenchmarks();
}

void BenchmarkRunner::saveResults(const std::string& format) {
    if (format == "json") {
        ResultFormatter::saveToJSON(results, outputDirectory + "/" + benchmarkName + ".json");
    } else if (format == "csv") {
        ResultFormatter::saveToCSV(results, outputDirectory + "/" + benchmarkName + ".csv");
    } else if (format == "markdown") {
        ResultFormatter::saveToMarkdown(results, outputDirectory + "/" + benchmarkName + ".md");
    }
}

void BenchmarkRunner::generateReport() {
    if (generateReports) {
        std::string reportPath = outputDirectory + "/" + benchmarkName + "_report.md";
        ResultFormatter::saveToMarkdown(results, reportPath);
    }
}

void BenchmarkRunner::generateCharts() {
    if (generateCharts) {
        // TODO: Implement chart generation
        std::cout << "Generating charts for benchmark: " << benchmarkName << std::endl;
    }
}

// Flamegraph utilities implementation

namespace flamegraph {
    FlameGraphNode buildFlameGraph(const std::vector<ProfileEvent>& events) {
        FlameGraphNode root;
        root.name = "root";
        root.selfTime = 0.0;
        root.totalTime = 0.0;
        
        for (const auto& event : events) {
            FlameGraphNode* current = &root;
            
            // Build the flame graph hierarchy based on thread and function
            size_t pos = 0;
            std::string remaining = event.functionName;
            
            while (!remaining.empty()) {
                size_t separator = remaining.find_first_of(".:");
                std::string segment;
                
                if (separator == std::string::npos) {
                    segment = remaining;
                    remaining.clear();
                } else {
                    segment = remaining.substr(0, separator);
                    remaining = remaining.substr(separator + 1);
                }
                
                // Find or create child node
                auto it = std::find_if(current->children.begin(), current->children.end(),
                    [&segment](const FlameGraphNode& node) { return node.name == segment; });
                
                if (it == current->children.end()) {
                    current->children.push_back(FlameGraphNode{segment, 0.0, 0.0, std::vector<FlameGraphNode>(), event.threadId});
                    it = std::prev(current->children.end());
                }
                
                current = &(*it);
                current->totalTime += std::chrono::duration<double>(event.endTime - event.startTime).count();
            }
        }
        
        return root;
    }
    
    void exportFlameGraph(const FlameGraphNode& root, const std::string& outputPath) {
        // TODO: Implement flame graph export
        std::cout << "Exporting flame graph to: " << outputPath << std::endl;
    }
    
    void generateSVG(const FlameGraphNode& root, const std::string& outputPath) {
        // TODO: Implement SVG generation
        std::cout << "Generating SVG flame graph to: " << outputPath << std::endl;
    }
}

// ResultFormatter implementation

std::string ResultFormatter::formatMetrics(const BenchmarkMetrics& metrics) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Benchmark: " << metrics.name << "\n";
    ss << "  Wall Clock Time: " << metrics.wallClockTime << "s\n";
    ss << "  Operations/s: " << metrics.opsPerSecond << "\n";
    ss << "  FLOPs/s: " << metrics.flopsPerSecond << "\n";
    ss << "  Memory Usage: " << metrics.memoryBytes << " bytes\n";
    ss << "  Cache Hit Ratio: " << metrics.cacheHitRatio * 100 << "%\n";
    ss << "  SIMD Speedup: " << metrics.simdSpeedup << "x\n";
    ss << "  Vectorization Efficiency: " << metrics.vectorizationEfficiency * 100 << "%\n";
    
    return ss.str();
}

std::string ResultFormatter::formatComparison(const ComparativeBenchmarker::ComparisonResult& result) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Comparison: " << result.testName << "\n";
    ss << "  Reference Implementation: " << result.referenceImplementation << "\n";
    ss << "  Current Implementation: " << result.currentImplementation << "\n";
    ss << "  Speedup: " << result.speedup << "x\n";
    ss << "  Memory Improvement: " << result.memoryImprovement << "%\n";
    ss << "  Quality Score: " << result.qualityScore << "\n";
    ss << "  Regression Risk: " << result.regressionRisk << "\n";
    
    return ss.str();
}

std::string ResultFormatter::formatScalability(const ScalabilityTester::ScalabilityResult& result) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Scalability: " << result.testName << "\n";
    ss << "  Neurons: " << result.neurons << "\n";
    ss << "  Regions: " << result.regions << "\n";
    ss << "  Connection Density: " << result.connectionDensity << "\n";
    ss << "  Time per Step: " << result.timePerStep << "s\n";
    ss << "  Memory per Neuron: " << result.memoryPerNeuron << " bytes\n";
    ss << "  Scalability Factor: " << result.scalabilityFactor << "\n";
    
    return ss.str();
}

void ResultFormatter::saveToCSV(const std::vector<BenchmarkMetrics>& metrics, const std::string& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return;
    }
    
    file << "name,wall_clock_time,cpu_time,operations,total_spikes,total_steps," <<
         "ops_per_second,flops_per_second,spikes_per_second,memory_bytes,peak_memory_bytes," <<
         "allocation_count,deallocation_count,memory_bandwidth,cache_hits,cache_misses," <<
         "cache_hit_ratio,active_threads,thread_utilization,synchronization_overhead," <<
         "simd_operations,scalar_operations,simd_speedup,vectorization_efficiency," <<
         "neuron_count,synapse_count,region_count,connection_density,average_fanout," <<
         "time_complexity,space_complexity" << std::endl;
    
    for (const auto& metric : metrics) {
        file << metric.name << "," << metric.wallClockTime << "," << metric.cpuTime << ","
             << metric.operations << "," << metric.totalSpikes << "," << metric.totalSteps << ","
             << metric.opsPerSecond << "," << metric.flopsPerSecond << "," << metric.spikesPerSecond << ","
             << metric.memoryBytes << "," << metric.peakMemoryBytes << ","
             << metric.allocationCount << "," << metric.deallocationCount << ","
             << metric.memoryBandwidth << "," << metric.cacheHits << "," << metric.cacheMisses << ","
             << metric.cacheHitRatio << "," << metric.activeThreads << ","
             << metric.threadUtilization << "," << metric.synchronizationOverhead << ","
             << metric.simdOperations << "," << metric.scalarOperations << ","
             << metric.simdSpeedup << "," << metric.vectorizationEfficiency << ","
             << metric.neuronCount << "," << metric.synapseCount << ","
             << metric.regionCount << "," << metric.connectionDensity << ","
             << metric.averageFanout << "," << metric.timeComplexity << ","
             << metric.spaceComplexity << std::endl;
    }
    
    file.close();
}

void ResultFormatter::saveToJSON(const std::vector<BenchmarkMetrics>& metrics, const std::string& path) {
    // TODO: Implement JSON serialization
    std::cout << "Saving to JSON: " << path << std::endl;
}

void ResultFormatter::saveToMarkdown(const std::vector<BenchmarkMetrics>& metrics, const std::string& path) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return;
    }
    
    file << "# NLM Benchmark Results\n\n";
    file << "## Summary\n\n";
    file << "| Benchmark | Wall Clock (s) | OPS | FLOPs/s | Memory (bytes) | Cache Hit % |\n";
    file << "|-----------|---------------|-----|----------|----------------|-------------|\n";
    
    for (const auto& metric : metrics) {
        file << "| " << metric.name << " | "
             << std::fixed << std::setprecision(3) << metric.wallClockTime << " | "
             << metric.opsPerSecond << " | "
             << metric.flopsPerSecond << " | "
             << metric.memoryBytes << " | "
             << std::fixed << std::setprecision(1) << metric.cacheHitRatio * 100 << "% |" << std::endl;
    }
    
    file << "\n## Detailed Results\n\n";
    
    for (const auto& metric : metrics) {
        file << "### " << metric.name << "\n\n";
        file << "- **Wall Clock Time**: " << std::fixed << std::setprecision(3) << metric.wallClockTime << "s\n";
        file << "- **CPU Time**: " << std::fixed << std::setprecision(3) << metric.cpuTime << "s\n";
        file << "- **Operations/s**: " << metric.opsPerSecond << "\n";
        file << "- **FLOPs/s**: " << metric.flopsPerSecond << "\n";
        file << "- **Spikes/s**: " << metric.spikesPerSecond << "\n";
        file << "- **Memory Usage**: " << metric.memoryBytes << " bytes\n";
        file << "- **Peak Memory**: " << metric.peakMemoryBytes << " bytes\n";
        file << "- **Cache Hit Ratio**: " << std::fixed << std::setprecision(1) << metric.cacheHitRatio * 100 << "%\n";
        file << "- **SIMD Speedup**: " << metric.simdSpeedup << "x\n";
        file << "- **Vectorization Efficiency**: " << std::fixed << std::setprecision(1) << metric.vectorizationEfficiency * 100 << "%\n";
        file << "- **Thread Utilization**: " << std::fixed << std::setprecision(2) << metric.threadUtilization * 100 << "%\n";
        file << "\n";
    }
    
    file.close();
}

} // namespace benchmark
} // namespace nlm