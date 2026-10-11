// Scalability benchmarking implementations

#include "ScalabilityBenchmarks.hpp"
#include "../benchmarks/Benchmark.hpp"

namespace nlm {
namespace benchmark {
namespace scalability {

// ScalabilityTester implementation

std::vector<ScalabilityTester::ScalabilityResult> ScalabilityTester::testNeuronScalability(const std::vector<size_t>& neuronCounts) {
    std::vector<ScalabilityTester::ScalabilityResult> results;
    
    for (size_t neuronCount : neuronCounts) {
        ScalabilityTester::ScalabilityResult result;
        result.testName = "NeuronScalability_" + std::to_string(neuronCount) + "neurons";
        result.neurons = static_cast<double>(neuronCount);
        result.regions = 1; // Fixed for neuron scalability test
        result.connectionDensity = 0.1; // Fixed connection density
        
        // Simulate benchmark run
        PerformanceMonitor monitor;
        monitor.start();
        
        // Simulate simulation steps
        size_t steps = 1000;
        for (size_t i = 0; i < steps; ++i) {
            // Simulate neuron operations
            for (size_t j = 0; j < neuronCount; ++j) {
                monitor.incrementOperations();
                if (j % 100 == 0) {
                    monitor.incrementMemoryUsage(sizeof(float) * 50);
                }
            }
        }
        
        monitor.stop(result.metrics);
        
        result.timePerStep = result.metrics.wallClockTime / steps;
        result.memoryPerNeuron = result.metrics.memoryBytes / neuronCount;
        result.confidenceInterval = 0.05; // Placeholder
        
        // Calculate scalability factor (time per neuron for fixed work)
        result.scalabilityFactor = (result.timePerStep * steps) / neuronCount;
        
        results.push_back(result);
    }
    
    return results;
}

std::vector<ScalabilityTester::ScalabilityResult> ScalabilityTester::testRegionScalability(const std::vector<size_t>& regionCounts) {
    std::vector<ScalabilityTester::ScalabilityResult> results;
    
    const size_t baseNeurons = 10000;
    
    for (size_t regionCount : regionCounts) {
        ScalabilityTester::ScalabilityResult result;
        result.testName = "RegionScalability_" + std::to_string(regionCount) + "regions";
        result.neurons = baseNeurons;
        result.regions = static_cast<double>(regionCount);
        result.connectionDensity = 0.01;
        
        PerformanceMonitor monitor;
        monitor.start();
        
        size_t steps = 500;
        for (size_t i = 0; i < steps; ++i) {
            // Simulate region-specific operations
            for (size_t r = 0; r < regionCount; ++r) {
                size_t neuronsPerRegion = baseNeurons / regionCount;
                for (size_t j = 0; j < neuronsPerRegion; ++j) {
                    monitor.incrementOperations();
                    monitor.incrementMemoryUsage(sizeof(float) * 30);
                }
            }
        }
        
        monitor.stop(result.metrics);
        
        result.timePerStep = result.metrics.wallClockTime / steps;
        result.memoryPerNeuron = result.metrics.memoryBytes / baseNeurons;
        result.confidenceInterval = 0.05;
        
        // Calculate scalability factor (time per region for fixed work)
        result.scalabilityFactor = (result.timePerStep * steps) / regionCount;
        
        results.push_back(result);
    }
    
    return results;
}

std::vector<ScalabilityTester::ScalabilityResult> ScalabilityTester::testConnectionDensityScalability(const std::vector<double>& densities) {
    std::vector<ScalabilityTester::ScalabilityResult> results;
    
    const size_t neuronCount = 1000;
    const size_t regionCount = 5;
    
    for (double density : densities) {
        ScalabilityTester::ScalabilityResult result;
        result.testName = "ConnectionDensityScalability_" + std::to_string(static_cast<int>(density * 100)) + "%";
        result.neurons = neuronCount;
        result.regions = regionCount;
        result.connectionDensity = density;
        
        PerformanceMonitor monitor;
        monitor.start();
        
        size_t steps = 200;
        for (size_t i = 0; i < steps; ++i) {
            // Simulate synaptic operations based on connection density
            size_t synapseCount = static_cast<size_t>(neuronCount * neuronCount * density * 0.01); // Rough estimate
            
            for (size_t j = 0; j < synapseCount; ++j) {
                monitor.incrementOperations();
                monitor.incrementMemoryUsage(sizeof(float) * 15);
                
                if (j % 100 == 0) {
                    monitor.recordCacheMiss();
                }
            }
        }
        
        monitor.stop(result.metrics);
        
        result.timePerStep = result.metrics.wallClockTime / steps;
        result.memoryPerNeuron = result.metrics.memoryBytes / neuronCount;
        result.confidenceInterval = 0.05;
        
        // Calculate scalability factor (time for different connection densities)
        result.scalabilityFactor = result.timePerStep / density; // How time scales with density
        
        results.push_back(result);
    }
    
    return results;
}

double ScalabilityTester::calculateTimeComplexity(const std::vector<size_t>& neuronCounts, const std::vector<double>& times) {
    if (neuronCounts.size() != times.size() || neuronCounts.size() < 2) {
        return 1.0; // Return linear as default
    }
    
    // Fit power law: time = a * neurons^b
    // log(time) = log(a) + b * log(neurons)
    std::vector<double> logX, logY;
    for (size_t i = 0; i < neuronCounts.size(); ++i) {
        logX.push_back(std::log(static_cast<double>(neuronCounts[i])));
        logY.push_back(std::log(times[i]));
    }
    
    auto regression = AdvancedStatistics::linearRegression(logX, logY);
    return regression.first; // This is the exponent 'b'
}

double ScalabilityTester::calculateSpaceComplexity(const std::vector<size_t>& neuronCounts, const std::vector<double>& memoryUsage) {
    if (neuronCounts.size() != memoryUsage.size() || neuronCounts.size() < 2) {
        return 1.0; // Return linear as default
    }
    
    // Fit power law: memory = a * neurons^b
    std::vector<double> logX, logY;
    for (size_t i = 0; i < neuronCounts.size(); ++i) {
        logX.push_back(std::log(static_cast<double>(neuronCounts[i])));
        logY.push_back(std::log(memoryUsage[i]));
    }
    
    auto regression = AdvancedStatistics::linearRegression(logX, logY);
    return regression.first; // This is the exponent 'b'
}

std::string ScalabilityTester::analyzeScalabilityResults(const std::vector<ScalabilityResult>& results) {
    std::stringstream analysis;
    analysis << "Scalability Analysis Results\n";
    analysis << "==========================\n\n";
    
    if (results.empty()) {
        analysis << "No results to analyze.\n";
        return analysis.str();
    }
    
    // Extract neuron scalability results
    std::vector<double> neuronScalability;
    for (const auto& result : results) {
        if (result.testName.find("NeuronScalability") != std::string::npos) {
            neuronScalability.push_back(result.scalabilityFactor);
        }
    }
    
    if (!neuronScalability.empty()) {
        analysis << "Neuron Scalability:\n";
        for (size_t i = 0; i < neuronScalability.size(); ++i) {
            analysis << "  Neurons: " << results[i].neurons << " - Scalability Factor: " << neuronScalability[i] << "\n";
        }
        analysis << "  Average: " << Statistics::mean(neuronScalability) << "\n\n";
    }
    
    // Calculate scaling efficiency
    if (neuronScalability.size() >= 2) {
        double firstFactor = neuronScalability[0];
        double lastFactor = neuronScalability.back();
        double scalingEfficiency = lastFactor / firstFactor;
        analysis << "Scaling Efficiency: " << scalingEfficiency << "\n";
        
        if (scalingEfficiency > 0.8) {
            analysis << "  -> Good scalability (near-linear)\n";
        } else if (scalingEfficiency > 0.5) {
            analysis << "  -> Moderate scalability\n";
        } else {
            analysis << "  -> Poor scalability (sub-linear)\n";
        }
    }
    
    return analysis.str();
}

void ScalabilityTester::generateScalabilityReport(const std::vector<ScalabilityResult>& results, const std::string& outputPath) {
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << outputPath << std::endl;
        return;
    }
    
    file << "NLM Scalability Benchmark Report\n";
    file << "==============================\n\n";
    
    for (const auto& result : results) {
        file << "Test: " << result.testName << "\n";
        file << "  Neurons: " << result.neurons << "\n";
        file << "  Regions: " << result.regions << "\n";
        file << "  Connection Density: " << result.connectionDensity << "\n";
        file << "  Time per Step (s): " << result.timePerStep << "\n";
        file << "  Memory per Neuron (bytes): " << result.memoryPerNeuron << "\n";
        file << "  Scalability Factor: " << result.scalabilityFactor << "\n";
        file << "  Confidence Interval: " << result.confidenceInterval << "\n\n";
    }
    
    file << analyzeScalabilityResults(results);
    
    file.close();
    std::cout << "Generated scalability report: " << outputPath << std::endl;
}

// ComparativeBenchmarker implementation

ComparativeBenchmarker::ComparativeBenchmarker(const std::string& referenceDirectory) : referenceDirectory(referenceDirectory) {}

void ComparativeBenchmarker::setReferenceImplementation(const std::string& path) {
    referenceDirectory = path;
    std::cout << "Set reference implementation directory: " << path << std::endl;
}

std::vector<ComparativeBenchmarker::ComparisonResult> ComparativeBenchmarker::compareWithReference(const std::string& testName, const BenchmarkMetrics& currentMetrics) {
    std::vector<ComparisonResult> results;
    
    // In a real implementation, this would load reference metrics from files
    // For now, create placeholder results
    ComparisonResult comparison;
    comparison.testName = testName;
    comparison.referenceImplementation = referenceDirectory;
    comparison.currentImplementation = "current";
    comparison.speedup = 1.5; // Placeholder
    comparison.memoryImprovement = 0.2; // 20% improvement
    comparison.qualityScore = 0.95; // High quality
    comparison.regressionRisk = 0.1; // 10% risk
    comparison.confidence = 0.95;
    comparison.severity = "low";
    comparison.recommendation = "Performance is acceptable with minor improvements possible.";
    
    results.push_back(comparison);
    
    return results;
}

double ComparativeBenchmarker::calculateRegressionRisk(const std::vector<BenchmarkMetrics>& metrics) {
    if (metrics.empty()) return 1.0;
    
    // Calculate regression risk based on metric variability
    std::vector<double> opsPerSecond;
    std::vector<double> flopsPerSecond;
    std::vector<double> memoryBandwidth;
    
    for (const auto& metric : metrics) {
        opsPerSecond.push_back(metric.opsPerSecond);
        flopsPerSecond.push_back(metric.flopsPerSecond);
        memoryBandwidth.push_back(metric.memoryBandwidth);
    }
    
    double avgOPS = Statistics::mean(opsPerSecond);
    double avgFLOPS = Statistics::mean(flopsPerSecond);
    double avgBW = Statistics::mean(memoryBandwidth);
    
    double varianceOPS = Statistics::variance(opsPerSecond);
    double varianceFLOPS = Statistics::variance(flopsPerSecond);
    double varianceBW = Statistics::variance(memoryBandwidth);
    
    // Calculate normalized risk scores
    double riskOPS = (varianceOPS / (avgOPS * avgOPS)) * 100.0;
    double riskFLOPS = (varianceFLOPS / (avgFLOPS * avgFLOPS)) * 100.0;
    double riskBW = (varianceBW / (avgBW * avgBW)) * 100.0;
    
    return (riskOPS + riskFLOPS + riskBW) / 3.0;
}

double ComparativeBenchmarker::calculateQualityScore(const BenchmarkMetrics& metrics) {
    double score = 0.0;
    
    // Score based on various metrics
    score += std::min(metrics.opsPerSecond / 1000000.0, 1.0) * 0.3; // OPS scoring (target 1M ops/s)
    score += std::min(metrics.flopsPerSecond / 1000000000.0, 1.0) * 0.3; // FLOPs/s scoring (target 1G FLOPs/s)
    score += std::min(1.0 - (metrics.memoryBandwidth / 10000000000.0), 1.0) * 0.2; // Memory bandwidth scoring
    score += metrics.cacheHitRatio * 0.2; // Cache hit ratio scoring
    
    return score;
}

std::string ComparativeBenchmarker::generateComparisonReport(const std::vector<ComparisonResult>& results) {
    std::stringstream report;
    report << "Comparative Benchmark Report\n";
    report << "============================\n\n";
    
    for (const auto& result : results) {
        report << "Test: " << result.testName << "\n";
        report << "  Reference: " << result.referenceImplementation << "\n";
        report << "  Current: " << result.currentImplementation << "\n";
        report << "  Speedup: " << result.speedup << "x\n";
        report << "  Memory Improvement: " << (result.memoryImprovement * 100) << "%\n";
        report << "  Quality Score: " << result.qualityScore << "\n";
        report << "  Regression Risk: " << (result.regressionRisk * 100) << "%\n";
        report << "  Confidence: " << result.confidence << "\n";
        report << "  Severity: " << result.severity << "\n";
        report << "  Recommendation: " << result.recommendation << "\n\n";
    }
    
    return report.str();
}

void ComparativeBenchmarker::saveComparisonResults(const std::vector<ComparisonResult>& results, const std::string& outputPath) {
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << outputPath << std::endl;
        return;
    }
    
    file << generateComparisonReport(results);
    
    file.close();
    std::cout << "Saved comparison results: " << outputPath << std::endl;
}

// PerformanceRegressionTester implementation

PerformanceRegressionTester::PerformanceRegressionTester(double alpha, double performanceThreshold) 
    : significanceLevel(alpha), performanceThreshold(performanceThreshold) {}

void PerformanceRegressionTester::setBaseline(const std::vector<BenchmarkMetrics>& baseline) {
    baselineMetrics = baseline;
    std::cout << "Set baseline with " << baseline.size() << " metrics" << std::endl;
}

std::vector<PerformanceRegressionTester::RegressionTestResult> PerformanceRegressionTester::runRegressionTests(const std::vector<BenchmarkMetrics>& currentMetrics) {
    std::vector<RegressionTestResult> results;
    
    if (baselineMetrics.empty()) {
        std::cout << "No baseline set for regression testing" << std::endl;
        return results;
    }
    
    for (const auto& baselineMetric : baselineMetrics) {
        for (const auto& currentMetric : currentMetrics) {
            if (baselineMetric.name == currentMetric.name) {
                RegressionTestResult testResult;
                testResult.testName = baselineMetric.name;
                testResult.passed = isAcceptableChange(baselineMetric.opsPerSecond, currentMetric.opsPerSecond, performanceThreshold);
                testResult.threshold = performanceThreshold;
                testResult.actualValue = currentMetric.opsPerSecond;
                testResult.difference = currentMetric.opsPerSecond - baselineMetric.opsPerSecond;
                testResult.percentChange = (testResult.difference / baselineMetric.opsPerSecond) * 100.0;
                testResult.pValue = calculatePValue(std::vector<double>(1, baselineMetric.opsPerSecond), std::vector<double>(1, currentMetric.opsPerSecond));
                testResult.isStatisticallySignificant = testResult.pValue < significanceLevel;
                testResult.recommendation = generateRecommendation(testResult);
                testResult.severity = severityFromPValue(testResult.pValue);
                
                results.push_back(testResult);
                break;
            }
        }
    }
    
    return results;
}

bool PerformanceRegressionTester::isAcceptableChange(double baseline, double current, double threshold) {
    double relativeChange = (current - baseline) / baseline;
    return (1.0 - threshold) <= relativeChange && relativeChange <= (1.0 + threshold);
}

double PerformanceRegressionTester::calculateEffectSize(const std::vector<double>& baseline, const std::vector<double>& current) {
    if (baseline.empty() || current.empty()) return 0.0;
    
    double baselineMean = Statistics::mean(baseline);
    double currentMean = Statistics::mean(current);
    double pooledStddev = std::sqrt((Statistics::variance(baseline) + Statistics::variance(current)) / 2.0);
    
    if (pooledStddev == 0.0) return 0.0;
    
    return std::abs(currentMean - baselineMean) / pooledStddev;
}

double PerformanceRegressionTester::calculatePValue(const std::vector<double>& baseline, const std::vector<double>& current) {
    // Simplified p-value calculation using t-test
    if (baseline.size() < 2 || current.size() < 2) return 1.0;
    
    double baselineMean = Statistics::mean(baseline);
    double currentMean = Statistics::mean(current);
    double baselineStddev = Statistics::standardDeviation(baseline);
    double currentStddev = Statistics::standardDeviation(current);
    
    double pooledStddev = std::sqrt((baselineStddev * baselineStddev / baseline.size()) + 
                                   (currentStddev * currentStddev / current.size()));
    
    if (pooledStddev == 0.0) return 1.0;
    
    double tStatistic = std::abs(baselineMean - currentMean) / pooledStddev;
    
    // Simplified p-value from t-statistic (two-tailed test)
    return 2.0 * (1.0 - 0.5 * std::erf(std::abs(tStatistic) / std::sqrt(2.0)));
}

std::string PerformanceRegressionTester::severityFromPValue(double pValue) {
    if (pValue < 0.01) return "critical";
    if (pValue < 0.05) return "warning";
    if (pValue < 0.1) return "info";
    return "low";
}

std::string PerformanceRegressionTester::generateRegressionReport(const std::vector<RegressionTestResult>& results) {
    std::stringstream report;
    report << "Performance Regression Test Report\n";
    report << "==================================\n\n";
    
    int passed = 0, failed = 0;
    for (const auto& result : results) {
        if (result.passed) {
            passed++;
            report << "✓ PASS: " << result.testName << " (" << result.percentChange << "% change)\n";
        } else {
            failed++;
            report << "✗ FAIL: " << result.testName << " (" << result.percentChange << "% change)\n";
        }
    }
    
    report << "\nSummary: " << passed << " passed, " << failed << " failed\n";
    
    if (failed > 0) {
        report << "\nRecommendations:\n";
        for (const auto& result : results) {
            if (!result.passed) {
                report << "- " << result.testName << ": " << result.recommendation << "\n";
            }
        }
    }
    
    return report.str();
}

void PerformanceRegressionTester::saveRegressionResults(const std::vector<RegressionTestResult>& results, const std::string& outputPath) {
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << outputPath << std::endl;
        return;
    }
    
    file << generateRegressionReport(results);
    
    file.close();
    std::cout << "Saved regression test results: " << outputPath << std::endl;
}

// CrossArchitectureBenchmarker implementation

void CrossArchitectureBenchmarker::discoverArchitectures() {
    // Discover available architectures
    std::cout << "Discovering architectures..." << std::endl;
    
    // In a real implementation, this would detect different CPU architectures,
    // GPU capabilities, etc.
    availableArchitectures = {"x86_64", "ARM64", "GPU_CUDA", "GPU_ROCM"};
}

std::vector<CrossArchitectureBenchmarker::ArchitectureResult> CrossArchitectureBenchmarker::benchmarkArchitecture(const std::string& architectureName) {
    std::vector<ArchitectureResult> results;
    
    ArchitectureResult result;
    result.architecture = architectureName;
    result.cpuInfo = getCPUInfo();
    result.gpuInfo = getGPUInfo();
    result.memoryInfo = getMemoryInfo();
    
    // Simulate benchmarking
    PerformanceMonitor monitor;
    monitor.start();
    
    // Simulate workload
    for (size_t i = 0; i < 10000; ++i) {
        monitor.incrementOperations();
        monitor.incrementMemoryUsage(sizeof(float) * 10);
    }
    
    monitor.stop(result.metrics);
    
    // Calculate relative performance (placeholder)
    result.relativePerformance = 1.0;
    result.efficiencyScore = 0.95;
    
    results.push_back(result);
    
    return results;
}

std::string CrossArchitectureBenchmarker::getCPUInfo() {
    return "x86_64 with AVX2 support";
}

std::string CrossArchitectureBenchmarker::getGPUInfo() {
    return "NVIDIA GPU with CUDA support";
}

std::string CrossArchitectureBenchmarker::getMemoryInfo() {
    return "32GB DDR4 RAM";
}

} // namespace scalability
} // namespace benchmark
} // namespace nlm

#include "ScalabilityBenchmarks.inl"