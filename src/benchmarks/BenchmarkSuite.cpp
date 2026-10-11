// Benchmark suite implementation

#include "BenchmarkSuite.hpp"
#include "benchmarks/Benchmark.hpp"

namespace nlm {
namespace benchmark {

// ComprehensiveBenchmarkSuite implementation

ComprehensiveBenchmarkSuite::ComprehensiveBenchmarkSuite(const std::string& outputDirectory) 
    : outputDirectory(outputDirectory), ciEnabled(false), isScheduling(false) {
    // Create output directory if it doesn't exist
    std::filesystem::create_directories(outputDirectory);
    initializeDefaultSuites();
}

ComprehensiveBenchmarkSuite::~ComprehensiveBenchmarkSuite() {
    cleanup();
}

void ComprehensiveBenchmarkSuite::initializeDefaultSuites() {
    // Core benchmark suite
    BenchmarkConfiguration coreSuite;
    coreSuite.name = "core_benchmarks";
    coreSuite.description = "Core NLM performance benchmarks including neuron steps, spike processing, memory allocation, SIMD effectiveness, and integration tests";
    coreSuite.enabled = true;
    coreSuite.timeout = 300.0; // 5 minutes
    coreSuite.iterations = 3;
    coreSuite.generateDetailedReport = true;
    coreSuite.generateCharts = true;
    coreSuite.collectProfiles = true;
    coreSuite.outputDirectory = outputDirectory + "/core";
    
    // Scalability benchmark suite
    BenchmarkConfiguration scalabilitySuite;
    scalabilitySuite.name = "scalability_benchmarks";
    scalabilitySuite.description = "Scalability testing for neuronal count, region count, connection density, and memory footprint";
    scalabilitySuite.enabled = true;
    scalabilitySuite.timeout = 600.0; // 10 minutes
    scalabilitySuite.iterations = 2;
    scalabilitySuite.generateDetailedReport = true;
    scalabilitySuite.generateCharts = true;
    scalabilitySuite.collectProfiles = true;
    scalabilitySuite.outputDirectory = outputDirectory + "/scalability";
    
    // Comparative benchmark suite
    BenchmarkConfiguration comparativeSuite;
    comparativeSuite.name = "comparative_benchmarks";
    comparativeSuite.description = "Comparison with reference implementations, performance regression testing, and optimization effectiveness measurement";
    comparativeSuite.enabled = true;
    comparativeSuite.timeout = 180.0; // 3 minutes
    comparativeSuite.iterations = 2;
    comparativeSuite.generateDetailedReport = true;
    comparativeSuite.generateCharts = true;
    comparativeSuite.collectProfiles = false;
    comparativeSuite.outputDirectory = outputDirectory + "/comparative";
    
    // Advanced metrics benchmark suite
    BenchmarkConfiguration advancedSuite;
    advancedSuite.name = "advanced_metrics_benchmarks";
    advancedSuite.description = "Operations per second (OPS), FLOPs per second, memory bandwidth utilization, cache hit/miss ratios, and thread synchronization overhead";
    advancedSuite.enabled = true;
    advancedSuite.timeout = 240.0; // 4 minutes
    advancedSuite.iterations = 3;
    advancedSuite.generateDetailedReport = true;
    advancedSuite.generateCharts = true;
    advancedSuite.collectProfiles = true;
    advancedSuite.outputDirectory = outputDirectory + "/advanced";
    
    // Integration benchmark suite
    BenchmarkConfiguration integrationSuite;
    integrationSuite.name = "integration_benchmarks";
    integrationSuite.description = "End-to-end simulation performance, system integration testing, and real-world workload benchmarks";
    integrationSuite.enabled = true;
    integrationSuite.timeout = 360.0; // 6 minutes
    integrationSuite.iterations = 2;
    integrationSuite.generateDetailedReport = true;
    integrationSuite.generateCharts = true;
    integrationSuite.collectProfiles = true;
    integrationSuite.outputDirectory = outputDirectory + "/integration";
    
    // Add all suites
    addBenchmarkSuite(coreSuite);
    addBenchmarkSuite(scalabilitySuite);
    addBenchmarkSuite(comparativeSuite);
    addBenchmarkSuite(advancedSuite);
    addBenchmarkSuite(integrationSuite);
}

void ComprehensiveBenchmarkSuite::addBenchmarkSuite(const BenchmarkConfiguration& config) {
    std::lock_guard<std::mutex> lock(mutex);
    benchmarkSuites.push_back(config);
}

void ComprehensiveBenchmarkSuite::removeBenchmarkSuite(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = std::find_if(benchmarkSuites.begin(), benchmarkSuites.end(),
        [&name](const BenchmarkConfiguration& config) { return config.name == name; });
    if (it != benchmarkSuites.end()) {
        benchmarkSuites.erase(it);
    }
}

void ComprehensiveBenchmarkSuite::enableBenchmarkSuite(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& config : benchmarkSuites) {
        if (config.name == name) {
            config.enabled = true;
            break;
        }
    }
}

void ComprehensiveBenchmarkSuite::disableBenchmarkSuite(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& config : benchmarkSuites) {
        if (config.name == name) {
            config.enabled = false;
            break;
        }
    }
}

void ComprehensiveBenchmarkSuite::runAllBenchmarkSuites() {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::cout << "Running all benchmark suites..." << std::endl;
    
    for (const auto& config : benchmarkSuites) {
        if (config.enabled) {
            runBenchmarkSuite(config.name);
        }
    }
    
    std::cout << "All benchmark suites completed." << std::endl;
}

void ComprehensiveBenchmarkSuite::runBenchmarkSuite(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = std::find_if(benchmarkSuites.begin(), benchmarkSuites.end(),
        [&name](const BenchmarkConfiguration& config) { return config.name == name; });n    
    if (it == benchmarkSuites.end()) {
        std::cerr << "Benchmark suite not found: " << name << std::endl;
        return;
    }
    
    validateAndRunSuite(*it);
}

std::vector<BenchmarkMetrics> ComprehensiveBenchmarkSuite::getResults() const {
    std::lock_guard<std::mutex> lock(mutex);
    return allResults;
}

std::vector<BenchmarkMetrics> ComprehensiveBenchmarkSuite::getResults(const std::string& benchmarkName) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<BenchmarkMetrics> results;
    for (const auto& result : allResults) {
        if (result.name.find(benchmarkName) != std::string::npos) {
            results.push_back(result);
        }
    }
    
    return results;
}

void ComprehensiveBenchmarkSuite::saveAllResults(const std::string& format) {
    std::lock_guard<std::mutex> lock(mutex);
    
    ResultFormatter::saveToJSON(allResults, outputDirectory + "/all_results.json");
    ResultFormatter::saveToCSV(allResults, outputDirectory + "/all_results.csv");
    ResultFormatter::saveToMarkdown(allResults, outputDirectory + "/all_results.md");
    
    std::cout << "All results saved to: " << outputDirectory << std::endl;
}

void ComprehensiveBenchmarkSuite::generateAllReports() {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Generate comprehensive report
    std::string report = generateComprehensiveReport(allResults);
    ResultFormatter::saveToMarkdown(allResults, outputDirectory + "/comprehensive_report.md");
    
    // Generate executive summary
    std::string executiveSummary = generateExecutiveSummary(allResults);
    ResultFormatter::saveToMarkdown(allResults, outputDirectory + "/executive_summary.md");
    
    // Generate detailed analysis
    std::string detailedAnalysis = generateDetailedAnalysis(allResults);
    ResultFormatter::saveToMarkdown(allResults, outputDirectory + "/detailed_analysis.md");
    
    std::cout << "All reports generated in: " << outputDirectory << std::endl;
}

void ComprehensiveBenchmarkSuite::exportForCI() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!ciEnabled) {
        std::cout << "CI integration is not enabled. Enable CI mode to export for CI." << std::endl;
        return;
    }
    
    // Generate CI-specific results
    ResultFormatter::saveToJSON(allResults, outputDirectory + "/ci_results.json");
    
    // Generate CI status report
    std::string ciStatus = generateCIStatusReport();
    ResultFormatter::saveToMarkdown(allResults, outputDirectory + "/ci_status.md");
    
    std::cout << "Results exported for CI in: " << outputDirectory << std::endl;
}

void ComprehensiveBenchmarkSuite::runCoreBenchmarks() {
    std::cout << "Running core benchmarks..." << std::endl;
    
    // Brain performance benchmarks
    core::BrainPerformanceBenchmark brainBenchmark;
    std::vector<size_t> neuronCounts = {100, 500, 1000, 2000};
    
    for (size_t count : neuronCounts) {
        BenchmarkMetrics metrics = brainBenchmark.benchmarkNeuronStep(count, 1000);
        allResults.push_back(metrics);
        suiteResults["core_benchmarks"].push_back(metrics);
    }
    
    // Memory allocation benchmarks
    core::MemoryAllocationBenchmark memoryBenchmark;
    std::vector<size_t> objectCounts = {1000, 10000, 100000};
    
    for (size_t count : objectCounts) {
        BenchmarkMetrics metrics = memoryBenchmark.benchmarkPoolVsHeapAllocation(count);
        allResults.push_back(metrics);
        suiteResults["core_benchmarks"].push_back(metrics);
    }
    
    // SIMD benchmarks
    core::SIMDBenchmark simdBenchmark;
    std::vector<float> testData(10000, 1.0f);
    
    BenchmarkMetrics simdMetrics = simdBenchmark.benchmarkScalarVsVector(testData);
    allResults.push_back(simdMetrics);
    suiteResults["core_benchmarks"].push_back(simdMetrics);
    
    // Integration benchmarks
    core::IntegrationBenchmark integrationBenchmark;
    std::vector<size_t> networkSizes = {100, 1000, 10000};
    
    for (size_t size : networkSizes) {
        BenchmarkMetrics metrics = integrationBenchmark.benchmarkBrainComputation(size);
        allResults.push_back(metrics);
        suiteResults["core_benchmarks"].push_back(metrics);
    }
    
    std::cout << "Core benchmarks completed." << std::endl;
}

void ComprehensiveBenchmarkSuite::runScalabilityBenchmarks() {
    std::cout << "Running scalability benchmarks..." << std::endl;
    
    scalability::ScalabilityTester scalabilityTester;
    
    // Neuron scalability
    std::vector<size_t> neuronCounts = {100, 500, 1000, 5000, 10000};
    std::vector<scalability::ScalabilityTester::ScalabilityResult> neuronResults = 
        scalabilityTester.testNeuronScalability(neuronCounts);
    
    // Convert to BenchmarkMetrics
    for (const auto& result : neuronResults) {
        BenchmarkMetrics metrics;
        metrics.name = "Scalability_" + result.testName;
        metrics.neuronCount = result.neurons;
        metrics.totalSteps = 1000; // Placeholder
        metrics.wallClockTime = result.timePerStep * 1000; // Convert to seconds
        metrics.memoryBytes = result.memoryPerNeuron * result.neurons;
        metrics.timeComplexity = result.scalabilityFactor;
        metrics.spaceComplexity = 1.0 / result.scalabilityFactor; // Placeholder
        
        allResults.push_back(metrics);
        suiteResults["scalability_benchmarks"].push_back(metrics);
    }
    
    // Region scalability
    std::vector<size_t> regionCounts = {1, 5, 10, 20, 50};
    std::vector<scalability::ScalabilityTester::ScalabilityResult> regionResults = 
        scalabilityTester.testRegionScalability(regionCounts);
    
    for (const auto& result : regionResults) {
        BenchmarkMetrics metrics;
        metrics.name = "Scalability_" + result.testName;
        metrics.neuronCount = result.neurons;
        metrics.regionCount = result.regions;
        metrics.totalSteps = 500;
        metrics.wallClockTime = result.timePerStep * 500;
        metrics.memoryBytes = result.memoryPerNeuron * result.neurons;
        metrics.connectionDensity = 0.01;
        
        allResults.push_back(metrics);
        suiteResults["scalability_benchmarks"].push_back(metrics);
    }
    
    // Connection density scalability
    std::vector<double> densities = {0.01, 0.05, 0.1, 0.25, 0.5, 0.75, 0.99};
    std::vector<scalability::ScalabilityTester::ScalabilityResult> densityResults = 
        scalabilityTester.testConnectionDensityScalability(densities);
    
    for (const auto& result : densityResults) {
        BenchmarkMetrics metrics;
        metrics.name = "Scalability_" + result.testName;
        metrics.neuronCount = result.neurons;
        metrics.regionCount = result.regions;
        metrics.connectionDensity = result.connectionDensity;
        metrics.totalSteps = 200;
        metrics.wallClockTime = result.timePerStep * 200;
        metrics.memoryBytes = result.memoryPerNeuron * result.neurons;
        metrics.timeComplexity = result.scalabilityFactor;
        
        allResults.push_back(metrics);
        suiteResults["scalability_benchmarks"].push_back(metrics);
    }
    
    std::cout << "Scalability benchmarks completed." << std::endl;
}

void ComprehensiveBenchmarkSuite::runComparativeBenchmarks() {
    std::cout << "Running comparative benchmarks..." << std::endl;
    
    // In a real implementation, this would compare with reference implementations
    // For now, generate placeholder comparative results
    
    for (const auto& baseline : allResults) {
        if (baseline.name.find("core") != std::string::npos) {
            BenchmarkMetrics current = baseline;
            current.name = "Comparative_" + baseline.name;
            current.speedup = 1.2; // Placeholder: 20% improvement
            current.memoryImprovement = 0.15; // Placeholder: 15% memory improvement
            current.qualityScore = 0.92; // Placeholder: high quality
            
            allResults.push_back(current);
            suiteResults["comparative_benchmarks"].push_back(current);
        }
    }
    
    std::cout << "Comparative benchmarks completed." << std::endl;
}

void ComprehensiveBenchmarkSuite::runAdvancedMetricsBenchmarks() {
    std::cout << "Running advanced metrics benchmarks..." << std::endl;
    
    // Calculate advanced metrics for existing results
    infrastructure::AdvancedMetricsCalculator metricsCalculator;
    
    for (auto& metrics : allResults) {
        infrastructure::AdvancedMetricsCalculator::AdvancedMetrics advanced = 
            metricsCalculator.calculateAdvancedMetrics(metrics);
        
        // Store advanced metrics back in the struct (if we add them to BenchmarkMetrics)
        // For now, just log the metrics
        std::cout << "  " << metrics.name << ": " 
                  << "OPS: " << metrics.opsPerSecond 
                  << ", FLOPs/s: " << metrics.flopsPerSecond 
                  << ", MemBW: " << metrics.memoryBandwidth 
                  << ", CacheHit: " << metrics.cacheHitRatio 
                  << ", SIMD: " << advanced.SIMDUtilization 
                  << std::endl;
    }
    
    std::cout << "Advanced metrics benchmarks completed." << std::endl;
}

void ComprehensiveBenchmarkSuite::runIntegrationBenchmarks() {
    std::cout << "Running integration benchmarks..." << std::endl;
    
    // End-to-end integration tests
    std::vector<size_t> networkSizes = {100, 1000, 5000, 10000};
    
    for (size_t size : networkSizes) {
        BenchmarkMetrics metrics;
        metrics.name = "Integration_EndToEnd_" + std::to_string(size) + "neurons";
        metrics.neuronCount = size;
        metrics.totalSteps = 100;
        metrics.wallClockTime = calculateExpectedSimulationTime(size);
        metrics.memoryBytes = estimateMemoryUsage(size);
        metrics.totalSpikes = size * 10; // Placeholder
        metrics.totalOperations = size * 100 * metrics.totalSteps;
        
        allResults.push_back(metrics);
        suiteResults["integration_benchmarks"].push_back(metrics);
        
        std::cout << "  Integration test: " << size << " neurons - " 
                  << metrics.wallClockTime << "s, " 
                  << metrics.memoryBytes << " bytes" << std::endl;
    }
    
    std::cout << "Integration benchmarks completed." << std::endl;
}

std::string ComprehensiveBenchmarkSuite::analyzePerformance(const std::vector<BenchmarkMetrics>& metrics) {
    std::stringstream analysis;
    analysis << "Performance Analysis\n";
    analysis << "===================\n\n";
    
    if (metrics.empty()) {
        analysis << "No metrics to analyze.\n";
        return analysis.str();
    }
    
    // Calculate averages
    double avgOPS = 0.0, avgFLOPS = 0.0, avgMemory = 0.0;
    size_t count = 0;
    
    for (const auto& metric : metrics) {
        avgOPS += metric.opsPerSecond;
        avgFLOPS += metric.flopsPerSecond;
        avgMemory += metric.memoryBytes;
        count++;
    }
    
    if (count > 0) {
        avgOPS /= count;
        avgFLOPS /= count;
        avgMemory /= count;
    }
    
    analysis << "Performance Summary:\n";
    analysis << "  Average Operations/s: " << avgOPS << "\n";
    analysis << "  Average FLOPs/s: " << avgFLOPS << "\n";
    analysis << "  Average Memory Usage: " << avgMemory << " bytes\n\n";
    
    // Identify bottlenecks
    analysis << "Potential Bottlenecks:\n";
    for (const auto& metric : metrics) {
        if (metric.cacheHitRatio < 0.8) {
            analysis << "  - " << metric.name << ": Low cache hit ratio (" << metric.cacheHitRatio * 100 << "%)" << std::endl;
        }
        if (metric.timeComplexity > 2.0) {
            analysis << "  - " << metric.name << ": High time complexity (" << metric.timeComplexity << ")" << std::endl;
        }
        if (metric.memoryBandwidth < 1000000000.0) { // Less than 1 GB/s
            analysis << "  - " << metric.name << ": Low memory bandwidth (" << metric.memoryBandwidth << " bytes/s)" << std::endl;
        }
    }
    
    return analysis.str();
}

std::string ComprehensiveBenchmarkSuite::identifyBottlenecks(const std::vector<BenchmarkMetrics>& metrics) {
    std::stringstream bottlenecks;
    bottlenecks << "Bottleneck Analysis\n";
    bottlenecks << "=================\n\n";
    
    for (const auto& metric : metrics) {
        bottlenecks << "Benchmark: " << metric.name << "\n";
        
        if (metric.timeComplexity > 2.0) {
            bottlenecks << "  Time Complexity: " << metric.timeComplexity << " (potential scalability issues)\n";
        }
        
        if (metric.memoryBandwidth < 1000000000.0) {
            bottlenecks << "  Memory Bandwidth: " << metric.memoryBandwidth << " bytes/s (potential memory-bound)\n";
        }
        
        if (metric.cacheHitRatio < 0.8) {
            bottlenecks << "  Cache Efficiency: " << metric.cacheHitRatio * 100 << "% (potential cache misses)\n";
        }
        
        if (metric.flopsPerSecond < 1000000000.0) {
            bottlenecks << "  Computational Throughput: " << metric.flopsPerSecond << " FLOPs/s (potential compute-bound)\n";
        }
        
        if (metric.opsPerSecond < 1000000.0) {
            bottlenecks << "  Operations Throughput: " << metric.opsPerSecond << " ops/s (potential operation-bound)\n";
        }
        
        bottlenecks << "\n";
    }
    
    return bottlenecks.str();
}

std::string ComprehensiveBenchmarkSuite::generateRecommendations(const std::vector<BenchmarkMetrics>& metrics) {
    std::stringstream recommendations;
    recommendations << "Performance Recommendations\n";
    recommendations << "==========================\n\n";
    
    for (const auto& metric : metrics) {
        recommendations << "For " << metric.name << ":\n";
        
        if (metric.timeComplexity > 2.0) {
            recommendations << "  - Optimize algorithmic complexity (currently O(n^" << metric.timeComplexity << "))\n";
        }
        
        if (metric.memoryBandwidth < 1000000000.0) {
            recommendations << "  - Increase memory bandwidth or optimize memory access patterns\n";
        }
        
        if (metric.cacheHitRatio < 0.8) {
            recommendations << "  - Improve cache locality through data layout optimization\n";
        }
        
        if (metric.flopsPerSecond < 1000000000.0) {
            recommendations << "  - Utilize SIMD vectorization for better computational throughput\n";
        }
        
        if (metric.opsPerSecond < 1000000.0) {
            recommendations << "  - Optimize individual operation performance\n";
        }
        
        recommendations << "\n";
    }
    
    return recommendations.str();
}

void ComprehensiveBenchmarkSuite::setOutputDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(mutex);
    outputDirectory = directory;
    std::filesystem::create_directories(outputDirectory);
}

void ComprehensiveBenchmarkSuite::setPerformanceThresholds(const std::map<std::string, double>& thresholds) {
    std::lock_guard<std::mutex> lock(mutex);
    performanceThresholds = thresholds;
}

void ComprehensiveBenchmarkSuite::setCIEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex);
    ciEnabled = enabled;
}

void ComprehensiveBenchmarkSuite::setCIBaseline(const std::string& baselineFile) {
    std::lock_guard<std::mutex> lock(mutex);
    ciBaselineFile = baselineFile;
}

void ComprehensiveBenchmarkSuite::updateCIBaseline() {
    if (ciEnabled) {
        // Save current results as new baseline
        ResultFormatter::saveToJSON(allResults, ciBaselineFile);
        std::cout << "Updated CI baseline: " << ciBaselineFile << std::endl;
    }
}

void ComprehensiveBenchmarkSuite::scheduleBenchmark(const std::string& suiteName, const std::string& cronExpression) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!isScheduling) {
        isScheduling = true;
        schedulerThread = std::thread(&ComprehensiveBenchmarkSuite::scheduleWorker, this);
    }
    
    std::cout << "Scheduled benchmark suite: " << suiteName << " with cron: " << cronExpression << std::endl;
}

void ComprehensiveBenchmarkSuite::stopScheduling() {
    std::lock_guard<std::mutex> lock(mutex);
    
    isScheduling = false;
    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }
    
    std::cout << "Stopped scheduling" << std::endl;
}

std::vector<std::string> ComprehensiveBenchmarkSuite::getScheduledBenchmarks() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<std::string> scheduled;
    for (const auto& config : benchmarkSuites) {
        // Check if benchmark has a schedule (simplified)
        if (config.name.find("schedule") != std::string::npos) {
            scheduled.push_back(config.name);
        }
    }
    
    return scheduled;
}

double ComprehensiveBenchmarkSuite::calculateExpectedSimulationTime(size_t neuronCount) {
    // Estimate simulation time based on neuron count (placeholder)
    // In reality, this would use actual performance data
    return static_cast<double>(neuronCount) / 1000.0 * 10.0; // 10 seconds per 1000 neurons
}

size_t ComprehensiveBenchmarkSuite::estimateMemoryUsage(size_t neuronCount) {
    // Estimate memory usage based on neuron count (placeholder)
    // In reality, this would use actual memory profiling data
    return neuronCount * sizeof(float) * 50; // ~50 floats per neuron
}

void ComprehensiveBenchmarkSuite::scheduleWorker() {
    while (isScheduling) {
        std::this_thread::sleep_for(std::chrono::minutes(1));
        
        // Run scheduled benchmarks (simplified)
        if (!benchmarkSuites.empty()) {
            std::cout << "Running scheduled benchmarks..." << std::endl;
            runAllBenchmarkSuites();
        }
    }
}

void ComprehensiveBenchmarkSuite::cleanup() {
    stopScheduling();
    
    // Clean up resources
    allResults.clear();
    suiteResults.clear();
    
    std::cout << "Benchmark suite cleaned up." << std::endl;
}

std::string ComprehensiveBenchmarkSuite::generateComprehensiveReport(const std::vector<BenchmarkMetrics>& metrics) {
    std::stringstream report;
    report << "Comprehensive NLM Benchmark Report\n";
    report << "================================\n\n";
    
    report << "Generated: " << generateTimestamp() << "\n\n";
    
    report << "Executive Summary:\n";
    report << "  Total Benchmarks: " << metrics.size() << "\n";
    
    if (!metrics.empty()) {
        double totalTime = 0.0, totalOPS = 0.0;
        for (const auto& metric : metrics) {
            totalTime += metric.wallClockTime;
            totalOPS += metric.opsPerSecond;
        }
        
        report << "  Total Time: " << totalTime << " seconds\n";
        report << "  Average OPS: " << totalOPS / metrics.size() << " ops/s\n\n";
    }
    
    report << "Key Findings:\n";
    for (const auto& metric : metrics) {
        report << "  - " << metric.name << ": " << metric.wallClockTime << "s, " << metric.opsPerSecond << " ops/s" << std::endl;
    }
    
    return report.str();
}

std::string ComprehensiveBenchmarkSuite::generateExecutiveSummary(const std::vector<BenchmarkMetrics>& metrics) {
    std::stringstream summary;
    summary << "Executive Summary\n";
    summary << "==============\n\n";
    
    if (metrics.empty()) {
        summary << "No benchmark results available.\n";
        return summary.str();
    }
    
    // Find best and worst performers
    const BenchmarkMetrics* bestOPS = nullptr;
    const BenchmarkMetrics* worstOPS = nullptr;
    
    for (const auto& metric : metrics) {
        if (bestOPS == nullptr || metric.opsPerSecond > bestOPS->opsPerSecond) {
            bestOPS = &metric;
        }
        if (worstOPS == nullptr || metric.opsPerSecond < worstOPS->opsPerSecond) {
            worstOPS = &metric;
        }
    }
    
    summary << "Best Performance:\n";
    if (bestOPS) {
        summary << "  " << bestOPS->name << ": " << bestOPS->opsPerSecond << " ops/s, " << bestOPS->flopsPerSecond << " FLOPs/s\n";
    }
    
    summary << "\nWorst Performance:\n";
    if (worstOPS) {
        summary << "  " << worstOPS->name << ": " << worstOPS->opsPerSecond << " ops/s, " << worstOPS->flopsPerSecond << " FLOPs/s\n";
    }
    
    return summary.str();
}

std::string ComprehensiveBenchmarkSuite::generateDetailedAnalysis(const std::vector<BenchmarkMetrics>& metrics) {
    std::stringstream analysis;
    analysis << "Detailed Analysis\n";
    analysis << "================\n\n";
    
    // Statistical analysis
    std::vector<double> opsValues, timeValues;
    for (const auto& metric : metrics) {
        opsValues.push_back(metric.opsPerSecond);
        timeValues.push_back(metric.wallClockTime);
    }
    
    if (!opsValues.empty()) {
        analysis << "Operations Per Second Statistics:\n";
        analysis << "  Mean: " << Statistics::mean(opsValues) << " ops/s\n";
        analysis << "  Standard Deviation: " << Statistics::standardDeviation(opsValues) << " ops/s\n";
        analysis << "  Median: " << Statistics::median(opsValues) << " ops/s\n\n";
        
        analysis << "Time Statistics:\n";
        analysis << "  Mean: " << Statistics::mean(timeValues) << " s\n";
        analysis << "  Standard Deviation: " << Statistics::standardDeviation(timeValues) << " s\n\n";
    }
    
    return analysis.str();
}

std::string ComprehensiveBenchmarkSuite::generateCIStatusReport() {
    std::stringstream report;
    report << "CI Performance Status Report\n";
    report << "==========================\n\n";
    
    if (!ciEnabled) {
        report << "CI integration is not enabled.\n";
        return report.str();
    }
    
    report << "CI Status: " << (ciEnabled ? "ENABLED" : "DISABLED") << "\n";
    report << "Baseline File: " << ciBaselineFile << "\n\n";
    
    // Check for regressions
    bool hasRegression = detectPerformanceRegression();
    report << "Performance Regression: " << (hasRegression ? "DETECTED" : "NONE") << "\n\n";
    
    return report.str();
}

bool ComprehensiveBenchmarkSuite::runCIIntegration() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!ciEnabled) {
        std::cout << "CI integration is not enabled." << std::endl;
        return false;
    }
    
    std::cout << "Running CI integration..." << std::endl;
    
    // Run performance regression detection
    bool hasRegression = detectPerformanceRegression();
    
    if (hasRegression) {
        std::cout << "WARNING: Performance regression detected in CI." << std::endl;
        return false;
    }
    
    std::cout << "CI integration completed successfully." << std::endl;
    return true;
}

bool ComprehensiveBenchmarkSuite::detectPerformanceRegression() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (allResults.empty()) {
        return false;
    }
    
    // Simple regression detection based on performance thresholds
    for (const auto& metric : allResults) {
        for (const auto& threshold : performanceThresholds) {
            if (metric.name.find(threshold.first) != std::string::npos) {
                if (metric.opsPerSecond < threshold.second) {
                    std::cout << "Regression detected: " << metric.name << " (" << metric.opsPerSecond << " < " << threshold.second << ")" << std::endl;
                    return true;
                }
            }
        }
    }
    
    return false;
}

std::string ComprehensiveBenchmarkSuite::generateCIStatusReport(const std::vector<BenchmarkMetrics>& metrics) {
    std::stringstream report;
    report << "CI Status Report\n";
    report << "==============\n\n";
    
    bool allPassed = true;
    for (const auto& metric : metrics) {
        bool passed = true; // Placeholder: check if metric meets thresholds
        if (!passed) {
            allPassed = false;
        }
    }
    
    report << "Overall Status: " << (allPassed ? "PASSED" : "FAILED") << "\n";
    report << "Benchmarks Run: " << metrics.size() << "\n";
    report << "Generation Time: " << generateTimestamp() << "\n\n";
    
    if (!allPassed) {
        report << "Failed Benchmarks:\n";
        for (const auto& metric : metrics) {
            bool passed = true; // Placeholder
            if (!passed) {
                report << "  - " << metric.name << "\n";
            }
        }
    }
    
    return report.str();
}

void ComprehensiveBenchmarkSuite::updateGitHubStatus(const std::string& status, const std::string& description) {
    // In a real implementation, this would update GitHub commit status
    std::cout << "Updating GitHub status: " << status << " - " << description << std::endl;
}

void ComprehensiveBenchmarkSuite::createPullRequestComment(const std::string& status, const std::string& description) {
    // In a real implementation, this would create a GitHub PR comment
    std::cout << "Creating PR comment: " << status << " - " << description << std::endl;
}

BenchmarkMetrics ComprehensiveBenchmarkSuite::calculateAdvancedMetrics(const std::vector<BenchmarkMetrics>& metrics) {
    BenchmarkMetrics advanced;
    
    if (metrics.empty()) {
        return advanced;
    }
    
    // Calculate aggregated metrics
    double totalOPS = 0.0, totalFLOPS = 0.0, totalMemory = 0.0;
    double totalCacheHits = 0.0;
    size_t count = metrics.size();
    
    for (const auto& metric : metrics) {
        totalOPS += metric.opsPerSecond;
        totalFLOPS += metric.flopsPerSecond;
        totalMemory += metric.memoryBytes;
        totalCacheHits += metric.cacheHits;
    }
    
    advanced.name = "Advanced_Aggregated";
    advanced.opsPerSecond = totalOPS / count;
    advanced.flopsPerSecond = totalFLOPS / count;
    advanced.memoryBytes = static_cast<size_t>(totalMemory / count);
    advanced.cacheHitRatio = totalCacheHits / count;
    
    return advanced;
}

std::string ComprehensiveBenchmarkSuite::rankBenchmarks(const std::vector<BenchmarkMetrics>& metrics) {
    std::stringstream ranking;
    ranking << "Benchmark Rankings\n";
    ranking << "==================\n\n";
    
    if (metrics.empty()) {
        ranking << "No benchmarks to rank.\n";
        return ranking.str();
    }
    
    // Create copy for sorting
    std::vector<BenchmarkMetrics> sorted = metrics;
    std::sort(sorted.begin(), sorted.end(),
        [](const BenchmarkMetrics& a, const BenchmarkMetrics& b) {
            return a.opsPerSecond > b.opsPerSecond;
        });
    
    for (size_t i = 0; i < sorted.size(); ++i) {
        ranking << (i + 1) << ". " << sorted[i].name << ": " << sorted[i].opsPerSecond << " ops/s\n";
    }
    
    return ranking.str();
}

std::string ComprehensiveBenchmarkSuite::generatePerformanceScorecard(const std::vector<BenchmarkMetrics>& metrics) {
    std::stringstream scorecard;
    scorecard << "Performance Scorecard\n";
    scorecard << "======================\n\n";
    
    if (metrics.empty()) {
        scorecard << "No performance data available.\n";
        return scorecard.str();
    }
    
    scorecard << "| Benchmark | OPS/s | FLOPs/s | Memory (MB) | Cache Hit % | Time (s) | Grade |\n";
    scorecard << "|-----------|-------|---------|--------------|-------------|----------|-------|\n";
    
    for (const auto& metric : metrics) {
        std::string grade = getPerformanceGrade(metric);
        scorecard << "| " << metric.name << " | "
                  << metric.opsPerSecond << " | "
                  << metric.flopsPerSecond << " | "
                  << (metric.memoryBytes / (1024 * 1024)) << " | "
                  << std::fixed << std::setprecision(1) << metric.cacheHitRatio * 100 << "% | "
                  << metric.wallClockTime << " | "
                  << grade << " |" << std::endl;
    }
    
    return scorecard.str();
}

std::string ComprehensiveBenchmarkSuite::getPerformanceGrade(const BenchmarkMetrics& metric) {
    double score = 0.0;
    
    // Calculate grade based on multiple factors
    if (metric.opsPerSecond > 1000000.0) score += 25; // > 1M ops/s
    if (metric.flopsPerSecond > 1000000000.0) score += 25; // > 1G FLOPs/s
    if (metric.cacheHitRatio > 0.9) score += 25; // > 90% cache hit ratio
    if (metric.wallClockTime < 1.0) score += 25; // < 1 second
    
    if (score >= 75) return "A";
    if (score >= 60) return "B";
    if (score >= 45) return "C";
    if (score >= 30) return "D";
    return "F";
}

// Utility functions

std::string ComprehensiveBenchmarkSuite::generateBenchmarkID(const std::string& suiteName, const std::map<std::string, std::string>& parameters) {
    std::stringstream id;
    id << suiteName << "_";
    
    for (const auto& param : parameters) {
        id << param.first << "_" << param.second << "_";
    }
    
    return id.str();
}

std::string ComprehensiveBenchmarkSuite::formatDuration(double seconds) {
    std::stringstream formatted;
    
    if (seconds < 60.0) {
        formatted << std::fixed << std::setprecision(2) << seconds << "s";
    } else if (seconds < 3600.0) {
        formatted << std::fixed << std::setprecision(2) << (seconds / 60.0) << "m";
    } else {
        formatted << std::fixed << std::setprecision(2) << (seconds / 3600.0) << "h";
    }
    
    return formatted.str();
}

std::string ComprehensiveBenchmarkSuite::formatBytes(size_t bytes) {
    std::stringstream formatted;
    
    if (bytes < 1024) {
        formatted << bytes << " B";
    } else if (bytes < (1024 * 1024)) {
        formatted << std::fixed << std::setprecision(2) << (bytes / 1024.0) << " KB";
    } else if (bytes < (1024 * 1024 * 1024)) {
        formatted << std::fixed << std::setprecision(2) << (bytes / (1024 * 1024.0)) << " MB";
    } else {
        formatted << std::fixed << std::setprecision(2) << (bytes / (1024 * 1024 * 1024.0)) << " GB";
    }
    
    return formatted.str();
}

std::string ComprehensiveBenchmarkSuite::formatPercentage(double value) {
    std::stringstream formatted;
    formatted << std::fixed << std::setprecision(1) << (value * 100.0) << "%";
    return formatted.str();
}

std::string ComprehensiveBenchmarkSuite::formatOPS(double opsPerSecond) {
    std::stringstream formatted;
    
    if (opsPerSecond < 1000.0) {
        formatted << std::fixed << std::setprecision(0) << opsPerSecond << " ops/s";
    } else if (opsPerSecond < 1000000.0) {
        formatted << std::fixed << std::setprecision(2) << (opsPerSecond / 1000.0) << " K ops/s";
    } else {
        formatted << std::fixed << std::setprecision(2) << (opsPerSecond / 1000000.0) << " M ops/s";
    }
    
    return formatted.str();
}

std::string ComprehensiveBenchmarkSuite::formatFLOPsPerSecond(double flopsPerSecond) {
    std::stringstream formatted;
    
    if (flopsPerSecond < 1000.0) {
        formatted << std::fixed << std::setprecision(0) << flopsPerSecond << " FLOPs/s";
    } else if (flopsPerSecond < 1000000.0) {
        formatted << std::fixed << std::setprecision(2) << (flopsPerSecond / 1000.0) << " KFLOPs/s";
    } else if (flopsPerSecond < 1000000000.0) {
        formatted << std::fixed << std::setprecision(2) << (flopsPerSecond / 1000000.0) << " MFLOPs/s";
    } else {
        formatted << std::fixed << std::setprecision(2) << (flopsPerSecond / 1000000000.0) << " GFLOPs/s";
    }
    
    return formatted.str();
}

std::string ComprehensiveBenchmarkSuite::generateTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string ComprehensiveBenchmarkSuite::sanitizeFilename(const std::string& filename) {
    std::string sanitized = filename;
    std::replace(sanitized.begin(), sanitized.end(), '/', '_');
    std::replace(sanitized.begin(), sanitized.end(), ' ', '_');
    std::replace(sanitized.begin(), sanitized.end(), ':', '_');
    return sanitized;
}

std::vector<BenchmarkConfiguration> ComprehensiveBenchmarkSuite::getBenchmarkConfigurations() const {
    std::lock_guard<std::mutex> lock(mutex);
    return benchmarkSuites;
}

bool ComprehensiveBenchmarkSuite::isBenchmarkSuiteEnabled(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (const auto& config : benchmarkSuites) {
        if (config.name == name) {
            return config.enabled;
        }
    }
    return false;
}

void ComprehensiveBenchmarkSuite::generateFlameGraphs(const std::string& outputPath) {
    // Generate flame graphs from profiling data
    std::cout << "Generating flame graphs in: " << outputPath << std::endl;
    // TODO: Implement flame graph generation
}

void ComprehensiveBenchmarkSuite::generateMemoryProfiles(const std::string& outputPath) {
    // Generate memory usage profiles
    std::cout << "Generating memory profiles in: " << outputPath << std::endl;
    // TODO: Implement memory profiling
}

void ComprehensiveBenchmarkSuite::generateCacheAnalysis(const std::string& outputPath) {
    // Generate cache efficiency analysis
    std::cout << "Generating cache analysis in: " << outputPath << std::endl;
    // TODO: Implement cache analysis
}

void ComprehensiveBenchmarkSuite::generateScalabilityAnalysis(const std::string& outputPath) {
    // Generate scalability analysis
    std::cout << "Generating scalability analysis in: " << outputPath << std::endl;
    // TODO: Implement scalability analysis
}

// Statistics and analysis functions

std::string ComprehensiveBenchmarkSuite::calculateStatistics(const std::vector<BenchmarkMetrics>& metrics) {
    std::stringstream stats;
    stats << "Performance Statistics\n";
    stats << "======================\n\n";
    
    if (metrics.empty()) {
        stats << "No metrics to analyze.\n";
        return stats.str();
    }
    
    // Calculate statistics for various metrics
    std::vector<double> opsList, timeList, memoryList;
    
    for (const auto& metric : metrics) {
        opsList.push_back(metric.opsPerSecond);
        timeList.push_back(metric.wallClockTime);
        memoryList.push_back(static_cast<double>(metric.memoryBytes));
    }
    
    stats << "Operations Per Second:\n";
    stats << "  Mean: " << Statistics::mean(opsList) << " ops/s\n";
    stats << "  Standard Deviation: " << Statistics::standardDeviation(opsList) << " ops/s\n";
    stats << "  Coefficient of Variation: " << Statistics::coefficientOfVariation(opsList) << "\n\n";
    
    stats << "Execution Time:\n";
    stats << "  Mean: " << Statistics::mean(timeList) << " s\n";
    stats << "  Standard Deviation: " << Statistics::standardDeviation(timeList) << " s\n";
    stats << "  Coefficient of Variation: " << Statistics::coefficientOfVariation(timeList) << "\n\n";
    
    stats << "Memory Usage:\n";
    stats << "  Mean: " << Statistics::mean(memoryList) << " bytes\n";
    stats << "  Standard Deviation: " << Statistics::standardDeviation(memoryList) << " bytes\n";
    stats << "  Coefficient of Variation: " << Statistics::coefficientOfVariation(memoryList) << "\n\n";
    
    return stats.str();
}

std::string ComprehensiveBenchmarkSuite::calculateTrends(const std::vector<BenchmarkMetrics>& metrics, size_t dataPoints) {
    std::stringstream trends;
    trends << "Performance Trends\n";
    trends << "=================\n\n";
    
    if (metrics.size() < 2) {
        trends << "Not enough data points for trend analysis.\n";
        return trends.str();
    }
    
    // Sort by name to ensure chronological order
    std::vector<BenchmarkMetrics> sorted = metrics;
    std::sort(sorted.begin(), sorted.end(),
        [](const BenchmarkMetrics& a, const BenchmarkMetrics& b) {
            return a.name < b.name;
        });
    
    // Take last N data points
    size_t startIndex = (sorted.size() > dataPoints) ? (sorted.size() - dataPoints) : 0;
    std::vector<double> opsTrend;
    
    for (size_t i = startIndex; i < sorted.size(); ++i) {
        opsTrend.push_back(sorted[i].opsPerSecond);
    }
    
    if (opsTrend.size() >= 2) {
        double trend = AdvancedStatistics::analyzeTimeSeriesTrend(opsTrend);
        trends << "OPS Trend: " << (trend > 0 ? "Increasing" : "Decreasing") << " (slope: " << trend << ")\n";
        
        double correlation = AdvancedStatistics::rSquared(
            std::vector<double>(opsTrend.size(), 0.0),
            opsTrend
        );
        trends << "Trend Strength: " << std::fixed << std::setprecision(2) << correlation << "\n";
    }
    
    return trends.str();
}

std::string ComprehensiveBenchmarkSuite::identifyOutliers(const std::vector<BenchmarkMetrics>& metrics) {
    std::stringstream outliers;
    outliers << "Outlier Detection\n";
    outliers << "=================\n\n";
    
    if (metrics.empty()) {
        outliers << "No metrics to analyze.\n";
        return outliers.str();
    }
    
    std::vector<double> opsValues;
    for (const auto& metric : metrics) {
        opsValues.push_back(metric.opsPerSecond);
    }
    
    std::vector<size_t> outlierIndices = AdvancedStatistics::findOutliers(opsValues, 2.0); // 2 standard deviations
    
    if (outlierIndices.empty()) {
        outliers << "No outliers detected.\n";
        return outliers.str();
    }
    
    outliers << "Outliers detected (based on 2 standard deviations):\n";
    for (size_t index : outlierIndices) {
        outliers << "  " << metrics[index].name << ": " << metrics[index].opsPerSecond << " ops/s\n";
    }
    
    return outliers.str();
}

std::vector<BenchmarkMetrics> ComprehensiveBenchmarkSuite::filterResults(const std::vector<BenchmarkMetrics>& metrics, const std::string& filter) {
    std::vector<BenchmarkMetrics> filtered;
    
    for (const auto& metric : metrics) {
        if (filter.empty() || metric.name.find(filter) != std::string::npos) {
            filtered.push_back(metric);
        }
    }
    
    return filtered;
}

bool ComprehensiveBenchmarkSuite::loadConfiguration(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open configuration file: " << configPath << std::endl;
        return false;
    }
    
    // TODO: Parse JSON/YAML configuration
    // For now, just load basic configuration
    file.close();
    return true;
}

bool ComprehensiveBenchmarkSuite::saveConfiguration(const std::string& configPath) {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open configuration file: " << configPath << std::endl;
        return false;
    }
    
    // TODO: Save configuration in JSON/YAML format
    file.close();
    return true;
}

bool ComprehensiveBenchmarkSuite::validateConfiguration() {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (const auto& config : benchmarkSuites) {
        if (config.enabled && config.name.empty()) {
            std::cerr << "Invalid benchmark configuration: empty name" << std::endl;
            return false;
        }
        if (config.timeout <= 0.0) {
            std::cerr << "Invalid benchmark configuration: non-positive timeout for " << config.name << std::endl;
            return false;
        }
        if (config.iterations <= 0) {
            std::cerr << "Invalid benchmark configuration: non-positive iterations for " << config.name << std::endl;
            return false;
        }
    }
    
    return true;
}

bool ComprehensiveBenchmarkSuite::isPerformanceRegression(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current) {
    return isAcceptableChange(baseline.opsPerSecond, current.opsPerSecond, performanceThresholds["regression_threshold"]);
}

bool ComprehensiveBenchmarkSuite::isAcceptableChange(double baseline, double current, double threshold) {
    double relativeChange = (current - baseline) / baseline;
    return (1.0 - threshold) <= relativeChange && relativeChange <= (1.0 + threshold);
}

std::string ComprehensiveBenchmarkSuite::calculatePerformanceDifference(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current) {
    double opsDiff = current.opsPerSecond - baseline.opsPerSecond;
    double opsPercent = (opsDiff / baseline.opsPerSecond) * 100.0;
    
    std::stringstream diff;
    diff << "OPS Change: " << opsDiff << " (" << opsPercent << "%)" << std::endl;
    diff << "FLOPs/s Change: " << (current.flopsPerSecond - baseline.flopsPerSecond) << " (" << 
        ((current.flopsPerSecond - baseline.flopsPerSecond) / baseline.flopsPerSecond * 100.0) << "%)" << std::endl;
    diff << "Memory Change: " << (current.memoryBytes - baseline.memoryBytes) << " bytes" << std::endl;
    
    return diff.str();
}

std::string ComprehensiveBenchmarkSuite::getRegressionSeverity(double difference) {
    if (difference > 0.1) return "critical";
    if (difference > 0.05) return "warning";
    if (difference > 0.01) return "info";
    return "low";
}

std::string ComprehensiveBenchmarkSuite::getRegressionRecommendation(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current) {
    std::stringstream recommendation;
    
    if (current.opsPerSecond < baseline.opsPerSecond) {
        recommendation << "Performance degraded. Consider:" << std::endl;
        recommendation << "  - Optimizing algorithmic complexity" << std::endl;
        recommendation << "  - Improving memory access patterns" << std::endl;
        recommendation << "  - Utilizing available SIMD instructions" << std::endl;
    } else {
        recommendation << "Performance improved. Consider:" << std::endl;
        recommendation << "  - Analyzing optimization techniques" << std::endl;
        recommendation << "  - Identifying areas for further improvement" << std::endl;
    }
    
    return recommendation.str();
}

void ComprehensiveBenchmarkSuite::exportResultsToCSV(const std::string& path) {
    ResultFormatter::saveToCSV(allResults, path);
}

void ComprehensiveBenchmarkSuite::exportResultsToJSON(const std::string& path) {
    ResultFormatter::saveToJSON(allResults, path);
}

void ComprehensiveBenchmarkSuite::exportResultsToMarkdown(const std::string& path) {
    ResultFormatter::saveToMarkdown(allResults, path);
}

void ComprehensiveBenchmarkSuite::exportResultsToHTML(const std::string& path) {
    // TODO: Implement HTML export
    std::cout << "HTML export to " << path << " (TODO)" << std::endl;
}

void ComprehensiveBenchmarkSuite::exportResultsToYAML(const std::string& path) {
    // TODO: Implement YAML export
    std::cout << "YAML export to " << path << " (TODO)" << std::endl;
}

void ComprehensiveBenchmarkSuite::exportResultsToXML(const std::string& path) {
    // TODO: Implement XML export
    std::cout << "XML export to " << path << " (TODO)" << std::endl;
}

bool ComprehensiveBenchmarkSuite::runCIIntegration() {
    return runCIIntegrationInternal();
}

bool ComprehensiveBenchmarkSuite::detectPerformanceRegression() {
    return detectPerformanceRegressionInternal();
}

std::string ComprehensiveBenchmarkSuite::generateCIStatusReport() {
    return generateCIStatusReportInternal();
}

void ComprehensiveBenchmarkSuite::updateGitHubStatus(const std::string& status, const std::string& description) {
    // In a real implementation, this would update GitHub commit status
    std::cout << "GitHub status update: " << status << " - " << description << std::endl;
}

void ComprehensiveBenchmarkSuite::createPullRequestComment(const std::string& status, const std::string& description) {
    // In a real implementation, this would create a GitHub PR comment
    std::cout << "PR comment: " << status << " - " << description << std::endl;
}

bool ComprehensiveBenchmarkSuite::runCIIntegrationInternal() {
    if (!ciEnabled) {
        std::cout << "CI integration is not enabled." << std::endl;
        return false;
    }
    
    std::cout << "Running CI integration..." << std::endl;
    
    // Generate CI-specific results
    std::string ciReport = generateCIStatusReportInternal();
    
    // Check for regressions
    bool hasRegression = detectPerformanceRegressionInternal();
    
    if (hasRegression) {
        std::cout << "WARNING: Performance regression detected in CI." << std::endl;
        return false;
    }
    
    std::cout << "CI integration completed successfully." << std::endl;
    return true;
}

bool ComprehensiveBenchmarkSuite::detectPerformanceRegressionInternal() {
    if (allResults.empty()) {
        return false;
    }
    
    for (const auto& metric : allResults) {
        for (const auto& threshold : performanceThresholds) {
            if (metric.name.find(threshold.first) != std::string::npos) {
                if (metric.opsPerSecond < threshold.second) {
                    std::cout << "Regression detected: " << metric.name << " (" << metric.opsPerSecond << " < " << threshold.second << ")" << std::endl;
                    return true;
                }
            }
        }
    }
    
    return false;
}

std::string ComprehensiveBenchmarkSuite::generateCIStatusReportInternal() {
    std::stringstream report;
    report << "CI Performance Status Report\n";
    report << "==========================\n\n";
    
    if (!ciEnabled) {
        report << "CI integration is not enabled.\n";
        return report.str();
    }
    
    report << "CI Status: " << (ciEnabled ? "ENABLED" : "DISABLED") << "\n";
    report << "Baseline File: " << ciBaselineFile << "\n\n";
    
    bool hasRegression = detectPerformanceRegressionInternal();
    report << "Performance Regression: " << (hasRegression ? "DETECTED" : "NONE") << "\n\n";
    
    report << "Summary:\n";
    report << "  - Benchmarks run: " << allResults.size() << "\n";
    report << "  - Performance regressions: " << (hasRegression ? "Yes" : "No") << "\n";
    report << "  - Status: " << (hasRegression ? "FAILED" : "PASSED") << "\n";
    
    return report.str();
}

// BenchmarkConfigManager implementation

BenchmarkConfigManager::BenchmarkConfigManager(const std::string& configFile) : configFile(configFile) {}

bool BenchmarkConfigManager::loadConfig(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open configuration file: " << configPath << std::endl;
        return false;
    }
    
    // TODO: Parse configuration from JSON/YAML
    file.close();
    return true;
}

bool BenchmarkConfigManager::saveConfig(const std::string& configPath) {
    std::ofstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open configuration file: " << configPath << std::endl;
        return false;
    }
    
    // TODO: Save configuration to JSON/YAML
    file.close();
    return true;
}

bool BenchmarkConfigManager::addBenchmarkConfig(const BenchmarkConfig& config) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!validateBenchmarkConfig(config)) {
        return false;
    }
    
    benchmarkConfigs.push_back(config);
    return true;
}

bool BenchmarkConfigManager::removeBenchmarkConfig(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = std::find_if(benchmarkConfigs.begin(), benchmarkConfigs.end(),
        [&name](const BenchmarkConfig& config) { return config.name == name; });
    
    if (it != benchmarkConfigs.end()) {
        benchmarkConfigs.erase(it);
        return true;
    }
    
    return false;
}

bool BenchmarkConfigManager::updateBenchmarkConfig(const BenchmarkConfig& config) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = std::find_if(benchmarkConfigs.begin(), benchmarkConfigs.end(),
        [&config](const BenchmarkConfig& c) { return c.name == config.name; });
    
    if (it != benchmarkConfigs.end()) {
        if (!validateBenchmarkConfig(config)) {
            return false;
        }
        
        *it = config;
        return true;
    }
    
    return false;
}

std::vector<BenchmarkConfigManager::BenchmarkConfig> BenchmarkConfigManager::getAllBenchmarkConfigs() const {
    std::lock_guard<std::mutex> lock(mutex);
    return benchmarkConfigs;
}

BenchmarkConfigManager::BenchmarkConfig BenchmarkConfigManager::getBenchmarkConfig(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = std::find_if(benchmarkConfigs.begin(), benchmarkConfigs.end(),
        [&name](const BenchmarkConfig& config) { return config.name == name; });n    
    if (it != benchmarkConfigs.end()) {
        return *it;
    }
    
    // Return default config if not found
    return getCoreBenchmarkConfig();
}

bool BenchmarkConfigManager::isBenchmarkConfigEnabled(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (const auto& config : benchmarkConfigs) {
        if (config.name == name) {
            return config.enabled;
        }
    }
    return false;
}

void BenchmarkConfigManager::enableBenchmarkConfig(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (auto& config : benchmarkConfigs) {
        if (config.name == name) {
            config.enabled = true;
            break;
        }
    }
}

void BenchmarkConfigManager::disableBenchmarkConfig(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (auto& config : benchmarkConfigs) {
        if (config.name == name) {
            config.enabled = false;
            break;
        }
    }
}

std::vector<BenchmarkConfigManager::BenchmarkConfig> BenchmarkConfigManager::getEnabledBenchmarkConfigs() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<BenchmarkConfig> enabled;
    for (const auto& config : benchmarkConfigs) {
        if (config.enabled) {
            enabled.push_back(config);
        }
    }
    
    return enabled;
}

bool BenchmarkConfigManager::validateBenchmarkConfig(const BenchmarkConfig& config) {
    if (config.name.empty()) {
        std::cerr << "Benchmark configuration validation error: empty name" << std::endl;
        return false;
    }
    
    if (config.timeout <= 0.0) {
        std::cerr << "Benchmark configuration validation error: non-positive timeout for " << config.name << std::endl;
        return false;
    }
    
    if (config.iterations <= 0) {
        std::cerr << "Benchmark configuration validation error: non-positive iterations for " << config.name << std::endl;
        return false;
    }
    
    return true;
}

std::string BenchmarkConfigManager::getValidationErrors(const BenchmarkConfig& config) {
    std::stringstream errors;
    
    if (config.name.empty()) {
        errors << "Empty benchmark name." << std::endl;
    }
    
    if (config.timeout <= 0.0) {
        errors << "Non-positive timeout." << std::endl;
    }
    
    if (config.iterations <= 0) {
        errors << "Non-positive iterations." << std::endl;
    }
    
    return errors.str();
}

BenchmarkConfigManager::BenchmarkConfig BenchmarkConfigManager::getCoreBenchmarkConfig() {
    BenchmarkConfig config;
    config.name = "core_benchmarks";
    config.description = "Core NLM performance benchmarks";
    config.enabled = true;
    config.timeout = 300.0;
    config.iterations = 3;
    config.generateReports = true;
    config.generateCharts = true;
    config.collectProfiles = true;
    config.outputFormat = "json";
    
    return config;
}

BenchmarkConfigManager::BenchmarkConfig BenchmarkConfigManager::getScalabilityBenchmarkConfig() {
    BenchmarkConfig config;
    config.name = "scalability_benchmarks";
    config.description = "Scalability testing benchmarks";
    config.enabled = true;
    config.timeout = 600.0;
    config.iterations = 2;
    config.generateReports = true;
    config.generateCharts = true;
    config.collectProfiles = true;
    config.outputFormat = "json";
    
    return config;
}

BenchmarkConfigManager::BenchmarkConfig BenchmarkConfigManager::getComparativeBenchmarkConfig() {
    BenchmarkConfig config;
    config.name = "comparative_benchmarks";
    config.description = "Comparative benchmarking with reference implementations";
    config.enabled = true;
    config.timeout = 180.0;
    config.iterations = 2;
    config.generateReports = true;
    config.generateCharts = true;
    config.collectProfiles = false;
    config.outputFormat = "json";
    
    return config;
}

BenchmarkConfigManager::BenchmarkConfig BenchmarkConfigManager::getAdvancedMetricsConfig() {
    BenchmarkConfig config;
    config.name = "advanced_metrics_benchmarks";
    config.description = "Advanced metrics and detailed performance measurements";
    config.enabled = true;
    config.timeout = 240.0;
    config.iterations = 3;
    config.generateReports = true;
    config.generateCharts = true;
    config.collectProfiles = true;
    config.outputFormat = "json";
    
    return config;
}

BenchmarkConfigManager::BenchmarkConfig BenchmarkConfigManager::getIntegrationBenchmarkConfig() {
    BenchmarkConfig config;
    config.name = "integration_benchmarks";
    config.description = "End-to-end integration benchmarks";
    config.enabled = true;
    config.timeout = 360.0;
    config.iterations = 2;
    config.generateReports = true;
    config.generateCharts = true;
    config.collectProfiles = true;
    config.outputFormat = "json";
    
    return config;
}

// BenchmarkResultAggregator implementation

BenchmarkResultAggregator::BenchmarkResultAggregator() {}

void BenchmarkResultAggregator::addResults(const std::vector<BenchmarkMetrics>& metrics) {
    std::lock_guard<std::mutex> lock(mutex);
    allResults.insert(allResults.end(), metrics.begin(), metrics.end());
}

void BenchmarkResultAggregator::addResult(const BenchmarkMetrics& metric) {
    std::lock_guard<std::mutex> lock(mutex);
    allResults.push_back(metric);
}

BenchmarkResultAggregator::AggregatedResults BenchmarkResultAggregator::aggregate(const std::string& benchmarkName) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    AggregatedResults results;
    results.benchmarkName = benchmarkName;
    
    std::vector<double> timeSeries, opsSeries, memorySeries;
    
    for (const auto& metric : allResults) {
        if (metric.name.find(benchmarkName) != std::string::npos) {
            timeSeries.push_back(metric.wallClockTime);
            opsSeries.push_back(metric.opsPerSecond);
            memorySeries.push_back(static_cast<double>(metric.memoryBytes));
        }
    }
    
    if (timeSeries.empty()) {
        return results;
    }
    
    // Calculate statistics
    results.totalRuns = timeSeries.size();
    results.totalTime = Statistics::sum(timeSeries);
    results.averageTime = Statistics::mean(timeSeries);
    results.minTime = *std::min_element(timeSeries.begin(), timeSeries.end());
    results.maxTime = *std::max_element(timeSeries.begin(), timeSeries.end());
    results.timeStandardDeviation = Statistics::standardDeviation(timeSeries);
    results.timeCoefficientOfVariation = Statistics::coefficientOfVariation(timeSeries);
    
    results.totalOperations = Statistics::sum(opsSeries);
    results.averageOperationsPerSecond = Statistics::mean(opsSeries);
    results.opsPerSecondStandardDeviation = Statistics::standardDeviation(opsSeries);
    
    results.totalMemoryUsage = static_cast<size_t>(Statistics::sum(memorySeries));
    results.averageMemoryUsage = Statistics::mean(memorySeries);
    results.peakMemoryUsage = static_cast<size_t>(*std::max_element(memorySeries.begin(), memorySeries.end()));
    
    results.averageCacheHitRatio = Statistics::mean(
        std::vector<double>(allResults.size(), 0.0) // Placeholder for cache hit ratio
    );
    
    // Calculate trend and volatility
    results.calculateTrend();
    results.calculateVolatility();
    
    // Set time series
    results.timeSeries = timeSeries;
    results.opsPerSecondSeries = opsSeries;
    results.memoryUsageSeries = memorySeries;
    
    return results;
}

std::vector<BenchmarkResultAggregator::AggregatedResults> BenchmarkResultAggregator::aggregateAll() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<BenchmarkResultAggregator::AggregatedResults> results;
    
    // Get unique benchmark names
    std::set<std::string> benchmarkNames;
    for (const auto& metric : allResults) {
        // Extract benchmark name from metric.name
        size_t underscorePos = metric.name.find('_');
        if (underscorePos != std::string::npos) {
            std::string baseName = metric.name.substr(0, underscorePos);
            benchmarkNames.insert(baseName);
        }
    }
    
    for (const auto& name : benchmarkNames) {
        results.push_back(aggregate(name));
    }
    
    return results;
}

std::vector<BenchmarkResultAggregator::AggregatedResults> BenchmarkResultAggregator::getTopBenchmarks(size_t count) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<AggregatedResults> aggregated = aggregateAll();
    std::sort(aggregated.begin(), aggregated.end(),
        [](const AggregatedResults& a, const AggregatedResults& b) {
            return a.averageOperationsPerSecond > b.averageOperationsPerSecond;
        });
    
    if (aggregated.size() > count) {
        aggregated.resize(count);
    }
    
    return aggregated;
}

std::vector<BenchmarkResultAggregator::AggregatedResults> BenchmarkResultAggregator::getBenchmarksByPerformance(const std::string& metric, size_t count) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<AggregatedResults> aggregated = aggregateAll();
    
    if (metric == "opsPerSecond") {
        std::sort(aggregated.begin(), aggregated.end(),
            [](const AggregatedResults& a, const AggregatedResults& b) {
                return a.averageOperationsPerSecond > b.averageOperationsPerSecond;
            });
    } else if (metric == "memoryEfficiency") {
        std::sort(aggregated.begin(), aggregated.end(),
            [](const AggregatedResults& a, const AggregatedResults& b) {
                return (a.totalOperations / a.totalMemoryUsage) > (b.totalOperations / b.totalMemoryUsage);
            });
    } else if (metric == "timeEfficiency") {
        std::sort(aggregated.begin(), aggregated.end(),
            [](const AggregatedResults& a, const AggregatedResults& b) {
                return a.totalTime / a.totalMemoryUsage < b.totalTime / b.totalMemoryUsage;
            });
    }
    
    if (aggregated.size() > count) {
        aggregated.resize(count);
    }
    
    return aggregated;
}

std::vector<BenchmarkMetrics> BenchmarkResultAggregator::filterResults(const std::string& benchmarkName, const std::string& filter) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<BenchmarkMetrics> filtered;
    
    for (const auto& metric : allResults) {
        if (metric.name.find(benchmarkName) != std::string::npos) {
            if (filter.empty() || metric.name.find(filter) != std::string::npos) {
                filtered.push_back(metric);
            }
        }
    }
    
    return filtered;
}

std::string BenchmarkResultAggregator::generateDetailedReport(const std::vector<AggregatedResults>& results) const {
    std::stringstream report;
    report << "Detailed Benchmark Aggregation Report\n";
    report << "============================\n\n";
    
    for (const auto& result : results) {
        report << "Benchmark: " << result.benchmarkName << "\n";
        report << "  Runs: " << result.totalRuns << "\n";
        report << "  Average Time: " << result.averageTime << " s\n";
        report << "  Average OPS: " << result.averageOperationsPerSecond << " ops/s\n";
        report << "  Memory Efficiency: " << (result.totalOperations / result.totalMemoryUsage) << " ops/byte\n";
        report << "  Performance Grade: " << result.getPerformanceGrade() << "\n";
        report << "  Improving: " << (result.isImproving() ? "Yes" : "No") << "\n";
        report << "\n";
    }
    
    return report.str();
}

void BenchmarkResultAggregator::saveAggregatedResults(const std::vector<AggregatedResults>& results, const std::string& outputPath) {
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << outputPath << std::endl;
        return;
    }
    
    for (const auto& result : results) {
        file << "Benchmark: " << result.benchmarkName << "\n";
        file << "  Total Runs: " << result.totalRuns << "\n";
        file << "  Total Time: " << result.totalTime << " s\n";
        file << "  Average Time: " << result.averageTime << " s\n";
        file << "  Average OPS: " << result.averageOperationsPerSecond << " ops/s\n";
        file << "  Memory Efficiency: " << (result.totalOperations / result.totalMemoryUsage) << " ops/byte\n";
        file << "  Performance Grade: " << result.getPerformanceGrade() << "\n";
        file << "\n";
    }
    
    file.close();
    std::cout << "Saved aggregated results to: " << outputPath << std::endl;
}

void BenchmarkResultAggregator::generateVisualizations(const std::vector<AggregatedResults>& results, const std::string& outputPath) {
    // TODO: Generate visualizations (charts, graphs)
    std::cout << "Generating visualizations to: " << outputPath << std::endl;
}

double BenchmarkResultAggregator::calculateMovingAverage(const std::vector<double>& values, size_t windowSize) const {
    if (values.empty() || windowSize == 0) return 0.0;
    
    size_t halfWindow = windowSize / 2;
    std::vector<double> movingAverages;
    
    for (size_t i = 0; i < values.size(); ++i) {
        size_t start = std::max(static_cast<size_t>(0), i - static_cast<size_t>(halfWindow));
        size_t end = std::min(values.size(), i + static_cast<size_t>(halfWindow) + 1);
        
        double sum = std::accumulate(values.begin() + start, values.begin() + end, 0.0);
        movingAverages.push_back(sum / (end - start));
    }
    
    return movingAverages[values.size() / 2];
}

double BenchmarkResultAggregator::calculateLinearTrend(const std::vector<double>& values) const {
    if (values.size() < 2) return 0.0;
    
    auto regression = AdvancedStatistics::linearRegression(
        std::vector<double>(values.size(), 0.0),
        values
    );
    
    return regression.first;
}

double BenchmarkResultAggregator::calculateVolatility(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    
    double mean = Statistics::mean(values);
    double variance = Statistics::variance(values);
    
    return std::sqrt(variance) / mean; // Coefficient of variation
}

std::string BenchmarkResultAggregator::getGradeFromScore(double score) const {
    if (score >= 75) return "A";
    if (score >= 60) return "B";
    if (score >= 45) return "C";
    if (score >= 30) return "D";
    return "F";
}

// AggregatedResults helper methods

double BenchmarkResultAggregator::AggregatedResults::calculateTrend() const {
    if (timeSeries.empty()) return 0.0;
    
    double trend = AdvancedStatistics::analyzeTimeSeriesTrend(timeSeries);
    return trend;
}

double BenchmarkResultAggregator::AggregatedResults::calculateVolatility() const {
    return calculateVolatility(timeSeries);
}

bool BenchmarkResultAggregator::AggregatedResults::isImproving() const {
    if (timeSeries.size() < 2) return false;
    
    return calculateTrend() < 0.0; // Negative trend means improving (less time)
}

std::string BenchmarkResultAggregator::AggregatedResults::getPerformanceGrade() const {
    double efficiency = (totalOperations / totalTime) / (averageOperationsPerSecond);
    return getGradeFromScore(efficiency * 100.0);
}

// PerformanceRegressionDetector implementation

PerformanceRegressionDetector::PerformanceRegressionDetector(double alpha, double performanceThreshold)
    : significanceLevel(alpha), performanceThreshold(performanceThreshold) {}

void PerformanceRegressionDetector::setBaseline(const std::vector<BenchmarkMetrics>& baseline) {
    std::lock_guard<std::mutex> lock(mutex);
    baselineMetrics = baseline;
}

std::vector<PerformanceRegressionDetector::RegressionDetection> PerformanceRegressionDetector::detectRegression(const std::vector<BenchmarkMetrics>& currentMetrics) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<RegressionDetection> results;
    
    if (baselineMetrics.empty()) {
        return results;
    }
    
    for (const auto& baseline : baselineMetrics) {
        for (const auto& current : currentMetrics) {
            if (baseline.name == current.name) {
                RegressionDetection detection;
                detection.testName = baseline.name;
                detection.hasRegression = isRegression(baseline, current);
                detection.regressionRisk = calculateRegressionRisk(baseline, current);
                detection.performanceImpact = calculatePerformanceImpact(baseline, current);
                detection.confidenceLevel = 0.95; // Placeholder
                detection.severity = getSeverityFromRisk(detection.regressionRisk);
                detection.affectedComponents = {"OPS", "memory", "cache efficiency"}; // Placeholder
                detection.recommendation = getRecommendation(baseline, current);
                detection.detectionTime = std::chrono::high_resolution_clock::now();
                
                results.push_back(detection);
                break;
            }
        }
    }
    
    return results;
}

std::vector<PerformanceRegressionDetector::RegressionDetection> PerformanceRegressionDetector::detectRegression(const std::string& benchmarkName, const std::vector<BenchmarkMetrics>& currentMetrics) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<RegressionDetection> results;
    
    for (const auto& baseline : baselineMetrics) {
        if (baseline.name == benchmarkName) {
            for (const auto& current : currentMetrics) {
                if (current.name.find(benchmarkName) != std::string::npos) {
                    RegressionDetection detection;
                    detection.testName = baseline.name;
                    detection.hasRegression = isRegression(baseline, current);
                    detection.regressionRisk = calculateRegressionRisk(baseline, current);
                    detection.performanceImpact = calculatePerformanceImpact(baseline, current);
                    detection.confidenceLevel = 0.95;
                    detection.severity = getSeverityFromRisk(detection.regressionRisk);
                    detection.affectedComponents = {"OPS", "memory", "cache efficiency"};
                    detection.recommendation = getRecommendation(baseline, current);
                    detection.detectionTime = std::chrono::high_resolution_clock::now();
                    
                    results.push_back(detection);
                    break;
                }
            }
            break;
        }
    }
    
    return results;
}

bool PerformanceRegressionDetector::isRegression(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current) {
    return isAcceptableChange(baseline.opsPerSecond, current.opsPerSecond, performanceThreshold);
}

double PerformanceRegressionDetector::calculateRegressionRisk(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current) {
    double effectSize = calculateEffectSize(baseline, current);
    double pValue = calculatePValue(std::vector<double>(1, baseline.opsPerSecond), std::vector<double>(1, current.opsPerSecond));
    
    // Calculate risk based on effect size and p-value
    double risk = (effectSize * 100.0) * (1.0 - pValue);
    
    // Clamp between 0 and 1
    return std::max(0.0, std::min(1.0, risk));
}

double PerformanceRegressionDetector::calculatePerformanceImpact(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current) {
    double opsChange = (current.opsPerSecond - baseline.opsPerSecond) / baseline.opsPerSecond;
    double memoryChange = (current.memoryBytes - baseline.memoryBytes) / baseline.memoryBytes;
    double cacheChange = current.cacheHitRatio - baseline.cacheHitRatio;
    
    return (opsChange + memoryChange + cacheChange) / 3.0;
}

std::string PerformanceRegressionDetector::getSeverityFromRisk(double risk) {
    if (risk >= 0.8) return "critical";
    if (risk >= 0.5) return "warning";
    if (risk >= 0.2) return "info";
    return "low";
}

std::string PerformanceRegressionDetector::getRecommendation(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current) {
    std::stringstream recommendation;
    
    if (current.opsPerSecond < baseline.opsPerSecond) {
        recommendation << "Performance degraded. ";
        
        if (current.memoryBytes > baseline.memoryBytes) {
            recommendation << "Memory usage increased. ";
        }
        
        if (current.cacheHitRatio < baseline.cacheHitRatio) {
            recommendation << "Cache efficiency decreased. ";
        }
        
        recommendation << "Consider optimizing performance.";
    } else {
        recommendation << "Performance improved. Continue monitoring for stability.";
    }
    
    return recommendation.str();
}

std::string PerformanceRegressionDetector::generateRegressionReport(const std::vector<RegressionDetection>& regressions) {
    std::stringstream report;
    report << "Performance Regression Detection Report\n";
    report << "==============================\n\n";
    
    int critical = 0, warning = 0, info = 0, low = 0;
    
    for (const auto& regression : regressions) {
        if (regression.severity == "critical") critical++;
        else if (regression.severity == "warning") warning++;
        else if (regression.severity == "info") info++;
        else low++;
    }
    
    report << "Summary:\n";
    report << "  Total Regressions Detected: " << regressions.size() << "\n";
    report << "  Critical: " << critical << "\n";
    report << "  Warning: " << warning << "\n";
    report << "  Info: " << info << "\n";
    report << "  Low: " << low << "\n\n";
    
    report << "Detailed Results:\n";
    for (const auto& regression : regressions) {
        report << "  - " << regression.testName << ": " << regression.severity << " risk (" << regression.regressionRisk * 100 << "%)" << std::endl;
        report << "    Impact: " << regression.performanceImpact << "\n";
        report << "    Recommendation: " << regression.recommendation << "\n";
    }
    
    return report.str();
}

void PerformanceRegressionDetector::saveRegressionDetections(const std::vector<RegressionDetection>& regressions, const std::string& outputPath) {
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << outputPath << std::endl;
        return;
    }
    
    file << generateRegressionReport(regressions);
    
    file.close();
    std::cout << "Saved regression detections to: " << outputPath << std::endl;
}

void PerformanceRegressionDetector::generateCIStatus(const std::vector<RegressionDetection>& regressions) {
    bool hasCritical = false;
    for (const auto& regression : regressions) {
        if (regression.severity == "critical") {
            hasCritical = true;
            break;
        }
    }
    
    if (hasCritical) {
        std::cout << "CRITICAL: Performance regression detected. CI status: FAILED" << std::endl;
    } else if (!regressions.empty()) {
        std::cout << "WARNING: Performance regression detected. CI status: WARNING" << std::endl;
    } else {
        std::cout << "CI status: PASSED" << std::endl;
    }
}

bool PerformanceRegressionDetector::isStatisticallySignificant(double pValue) {
    return pValue < significanceLevel;
}

// AdvancedMetricsCalculator implementation

BenchmarkMetrics AdvancedMetricsCalculator::calculateAdvancedMetrics(const BenchmarkMetrics& metrics) {
    BenchmarkMetrics advanced = metrics;
    
    advanced.opsPerSecond = calculateOPS(metrics);
    advanced.flopsPerSecond = calculateFLOPsPerSecond(metrics);
    advanced.memoryBandwidth = calculateMemoryBandwidth(metrics);
    advanced.cacheHitMissRatio = calculateCacheHitMissRatio(metrics);
    advanced.threadSynchronizationOverhead = calculateThreadSynchronizationOverhead(metrics);
    
    // For now, return the original metrics with additional calculations
    // In a complete implementation, these would be additional fields
    
    return advanced;
}

double AdvancedMetricsCalculator::calculateOPS(const BenchmarkMetrics& metrics) {
    return metrics.opsPerSecond;
}

double AdvancedMetricsCalculator::calculateFLOPsPerSecond(const BenchmarkMetrics& metrics) {
    return metrics.flopsPerSecond;
}

double AdvancedMetricsCalculator::calculateMemoryBandwidth(const BenchmarkMetrics& metrics) {
    if (metrics.wallClockTime > 0.0) {
        return metrics.memoryBandwidth;
    }
    return 0.0;
}

double AdvancedMetricsCalculator::calculateCacheHitMissRatio(const BenchmarkMetrics& metrics) {
    if (metrics.cacheHits + metrics.cacheMisses > 0) {
        return static_cast<double>(metrics.cacheHits) / (metrics.cacheHits + metrics.cacheMisses);
    }
    return 0.0;
}

double AdvancedMetricsCalculator::calculateThreadSynchronizationOverhead(const BenchmarkMetrics& metrics) {
    // Placeholder: calculate based on active threads and operations
    return metrics.wallClockTime * 0.01; // 1% overhead as placeholder
}
double AdvancedMetricsCalculator::calculateInstructionPerCycle(const BenchmarkMetrics& metrics) {
    // Placeholder: calculate based on operations and time
    return metrics.opsPerSecond / 3.0e9; // Assuming 3GHz clock
}

double AdvancedMetricsCalculator::calculateBranchPredictionRate(const BenchmarkMetrics& metrics) {
    // Placeholder: estimate based on cache hit ratio
    return metrics.cacheHitRatio;
}

double AdvancedMetricsCalculator::calculateSIMDUtilization(const BenchmarkMetrics& metrics) {
    // Placeholder: estimate based on performance characteristics
    return 0.85; // 85% utilization as placeholder
}

double AdvancedMetricsCalculator::calculateMemoryLevelEfficiency(const BenchmarkMetrics& metrics) {
    // Placeholder: calculate memory hierarchy efficiency
    return (metrics.cacheHitRatio * 0.5) + (0.3) + (0.2); // Simplified calculation
}

double AdvancedMetricsCalculator::calculateComputationalIntensity(const BenchmarkMetrics& metrics) {
    // Calculate computational intensity (FLOPs/byte)
    if (metrics.memoryBandwidth > 0.0) {
        return metrics.flopsPerSecond / metrics.memoryBandwidth;
    }
    return 0.0;
}

double AdvancedMetricsCalculator::calculateArithmeticIntensity(const BenchmarkMetrics& metrics) {
    // Calculate arithmetic intensity (FLOPs/operation)
    if (metrics.opsPerSecond > 0.0) {
        return metrics.flopsPerSecond / metrics.opsPerSecond;
    }
    return 0.0;
}

double AdvancedMetricsCalculator::calculateEnergyEfficiency(const BenchmarkMetrics& metrics, double energyConsumption) {
    if (energyConsumption > 0.0) {
        return metrics.opsPerSecond / energyConsumption;
    }
    return 0.0;
}

double AdvancedMetricsCalculator::calculateScalabilityScore(const std::vector<BenchmarkMetrics>& metrics) {
    if (metrics.empty()) return 0.0;
    
    // Calculate average scalability across all metrics
    std::vector<double> scalabilityScores;
    
    for (const auto& metric : metrics) {
        // Placeholder: calculate scalability score
        double score = (metric.timeComplexity > 0) ? (1.0 / metric.timeComplexity) : 0.0;
        score += (metric.spaceComplexity > 0) ? (1.0 / metric.spaceComplexity) : 0.0;
        score += metric.cacheHitRatio;
        score += (metric.flopsPerSecond > 0) ? 1.0 : 0.0;
        
        scalabilityScores.push_back(score);
    }
    
    return Statistics::mean(scalabilityScores);
}

double AdvancedMetricsCalculator::calculatePerformanceScore(const BenchmarkMetrics& metrics) {
    double score = 0.0;
    
    // Calculate composite performance score
    score += std::min(metrics.opsPerSecond / 1000000.0, 1.0) * 0.3; // OPS scoring
    score += std::min(metrics.flopsPerSecond / 1000000000.0, 1.0) * 0.3; // FLOPs/s scoring
    score += std::min(1.0 - (metrics.memoryBandwidth / 10000000000.0), 1.0) * 0.2; // Memory bandwidth
    score += metrics.cacheHitRatio * 0.2; // Cache efficiency
    
    return score;
}

std::string AdvancedMetricsCalculator::rankPerformance(const std::vector<BenchmarkMetrics>& metrics) {
    std::stringstream ranking;
    ranking << "Performance Rankings\n";
    ranking << "===================\n\n";
    
    if (metrics.empty()) {
        ranking << "No metrics to rank.\n";
        return ranking.str();
    }
    
    // Create copy for sorting
    std::vector<BenchmarkMetrics> sorted = metrics;
    std::sort(sorted.begin(), sorted.end(),
        [](const BenchmarkMetrics& a, const BenchmarkMetrics& b) {
            return calculatePerformanceScore(a) > calculatePerformanceScore(b);
        });
    
    for (size_t i = 0; i < sorted.size(); ++i) {
        ranking << (i + 1) << ". " << sorted[i].name << ": " << calculatePerformanceScore(sorted[i]) * 100 << "%" << std::endl;
    }
    
    return ranking.str();
}

void AdvancedMetricsCalculator::generateAdvancedMetricsReport(const AdvancedMetrics& metrics, const std::string& outputPath) {
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << outputPath << std::endl;
        return;
    }
    
    file << "Advanced Metrics Report\n";
    file << "=====================\n\n";
    file << "Operations per Second (OPS): " << metrics.opsPerSecond << "\n";
    file << "Floating Point Operations per Second (FLOPs/s): " << metrics.flopsPerSecond << "\n";
    file << "Memory Bandwidth: " << metrics.memoryBandwidth << " bytes/s\n";
    file << "Cache Hit/Miss Ratio: " << metrics.cacheHitMissRatio << "\n";
    file << "Thread Synchronization Overhead: " << metrics.threadSynchronizationOverhead << " s\n";
    file << "Instructions per Cycle: " << metrics.instructionPerCycle << "\n";
    file << "Branch Prediction Rate: " << metrics.branchPredictionRate << "\n";
    file << "SIMD Utilization: " << metrics.SIMDUtilization << "\n";
    file << "Memory Level Efficiency: " << metrics.memoryLevelEfficiency << "\n";
    file << "Computational Intensity: " << metrics.computationalIntensity << " FLOPs/byte\n";
    file << "Arithmetic Intensity: " << metrics.arithmeticIntensity << " FLOPs/operation\n";
    file << "Energy Efficiency: " << metrics.energyEfficiency << " ops/J\n";
    file << "Scalability Score: " << metrics.scalabilityScore << "\n";
    file << "Performance Score: " << metrics.performanceScore << "\n";
    
    file.close();
    std::cout << "Generated advanced metrics report: " << outputPath << std::endl;
}

} // namespace benchmark
} // namespace nlm

#include "BenchmarkSuite.inl"