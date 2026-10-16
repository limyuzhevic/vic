// NLM Comprehensive Benchmark Suite Implementation
// Enhanced benchmarking with multiple configuration options and detailed metrics

#include "BenchmarkSuite.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <thread>

namespace nlm {

// BenchmarkRun implementation
BenchmarkRun::BenchmarkRun(std::shared_ptr<Config> config, size_t neurons, size_t steps,
                         const std::string& configName)
    : config_(config), neuronCount_(neurons), steps_(steps), configName_(configName) {
}

BenchmarkRun::~BenchmarkRun() {
}

void BenchmarkRun::setConfiguration(const std::string& name, const BenchmarkConfig::Configuration& config) {
    // Apply configuration settings
    if (config.enableMemoryPool) {
        enableMemoryPool(true);
    }
    if (config.enableSOAVectorization) {
        enableSOAVectorization(true);
    }
    if (config.enableEventQueue) {
        enableEventQueue(true);
    }
    if (config.enableSparseConnectivity) {
        enableSparseConnectivity(true);
    }
    if (config.enableMultithreading) {
        enableMultithreading(true, config.threadCount);
    }
}

BenchmarkMetrics BenchmarkRun::run() {
    BenchmarkMetrics metrics;
    metrics.neuronCount = neuronCount_;
    metrics.steps = steps_;
    metrics.configurationName = configName_;
    metrics.startTime = std::chrono::steady_clock::now();
    
    try {
        metrics = runBenchmark();
        metrics.endTime = std::chrono::steady_clock::now();
        metrics.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            metrics.endTime - metrics.startTime).count();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Benchmark run failed: " + std::string(e.what()));
        metrics.wallClockTime = 0.0;
        metrics.simulationTime = 0.0;
    }
    
    return metrics;
}

BenchmarkMetrics BenchmarkRun::runBenchmark() {
    BenchmarkMetrics metrics;
    
    // Create configuration based on current settings
    auto benchConfig = std::make_shared<Config>();
    benchConfig->set("neuron_count", static_cast<int64_t>(neuronCount_));
    benchConfig->set("region_count", static_cast<int64_t>(1));
    benchConfig->set("connection_probability", 0.1f);
    benchConfig->set("stdp_ltp_weight", 0.01f);
    benchConfig->set("stdp_ltd_weight", 0.012f);
    
    // Create brain with current configuration
    auto brain = std::make_shared<Brain>(benchConfig);
    brain->initialize();
    
    // Record start time
    auto startWall = std::chrono::high_resolution_clock::now();
    auto startSim = std::chrono::high_resolution_clock::now();
    
    // Run simulation
    for (size_t step = 0; step < steps_; ++step) {
        // Inject some input to generate activity
        for (size_t i = 0; i < std::min(size_t(10), neuronCount_ / 10); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        // Record performance metrics periodically
        if (step % 100 == 0) {
            measureMemoryUsage(metrics);
            measureCPUUsage(metrics);
        }
    }
    
    // Record end time
    auto endWall = std::chrono::high_resolution_clock::now();
    auto endSim = std::chrono::high_resolution_clock::now();
    
    metrics.wallClockTime = std::chrono::duration<double>(endWall - startWall).count();
    metrics.simulationTime = std::chrono::duration<double>(endSim - startSim).count();
    
    // Collect final metrics
    metrics.neuronCount = brain->getTotalNeuronCount();
    metrics.synapseCount = brain->getTotalSynapseCount();
    metrics.totalSpikeCount = brain->getTotalSpikeCount();
    metrics.firingNeuronCount = brain->getFiringNeuronCount();
    metrics.averageFiringRate = brain->getAverageFiringRate();
    
    // Calculate derived metrics
    if (metrics.wallClockTime > 0.0) {
        metrics.stepsPerSecond = static_cast<double>(steps_) / metrics.wallClockTime;
    }
    
    if (metrics.neuronCount > 0) {
        metrics.memoryPerNeuron = metrics.memoryUsage / metrics.neuronCount;
        metrics.memoryPerSynapse = metrics.memoryUsage / metrics.synapseCount;
        metrics.memoryEfficiency = static_cast<double>(metrics.synapseCount) / metrics.memoryUsage;
        metrics.cpuPerNeuron = metrics.cpuUsage / metrics.neuronCount;
        metrics.firingDensity = metrics.averageFiringRate * 1000.0 / metrics.neuronCount;
        metrics.efficiency = metrics.stepsPerSecond / metrics.neuronCount;
    }
    
    // Validate performance
    validatePerformance(metrics);
    
    return metrics;
}

void BenchmarkRun::measureMemoryUsage(BenchmarkMetrics& metrics) {
    // Measure memory usage (simplified implementation)
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    
    // Convert KB to MB
    metrics.memoryUsage = usage.ru_maxrss / 1024.0;
}

void BenchmarkRun::measureCPUUsage(BenchmarkMetrics& metrics) {
    // Measure CPU usage (simplified implementation)
    static auto lastTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(currentTime - lastTime).count();
    
    if (elapsed > 0.0) {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        
        double cpuTime = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6;
        metrics.cpuUsage = std::min(100.0, (cpuTime / elapsed) * 100.0);
    }
    
    lastTime = currentTime;
}

void BenchmarkRun::validatePerformance(const BenchmarkMetrics& metrics) {
    // Validate against performance thresholds
    if (metrics.stepsPerSecond < config_.minStepsPerSecond) {
        NLM_LOG_WARNING("Benchmark performance below threshold: " + 
                       std::to_string(metrics.stepsPerSecond) + " < " +
                       std::to_string(config_.minStepsPerSecond) + " steps/sec");
    }
    
    if (metrics.memoryUsage > config_.maxMemoryUsage) {
        NLM_LOG_WARNING("Benchmark memory usage above threshold: " + 
                       std::to_string(metrics.memoryUsage) + " > " +
                       std::to_string(config_.maxMemoryUsage) + " MB");
    }
}

std::string BenchmarkRun::generateReport(const BenchmarkMetrics& metrics) const {
    std::ostringstream oss;
    oss << "Benchmark Report\n";
    oss << "===============\n\n";
    oss << "Configuration: " << configName_ << "\n";
    oss << "Neurons: " << metrics.neuronCount << "\n";
    oss << "Synapses: " << metrics.synapseCount << "\n";
    oss << "Steps: " << metrics.steps << "\n\n";
    oss << "Performance:\n";
    oss << "  Wall Clock Time: " << std::fixed << std::setprecision(3) 
        << metrics.wallClockTime << "s\n";
    oss << "  Steps per Second: " << std::fixed << std::setprecision(2) 
        << metrics.stepsPerSecond << "\n";
    oss << "  Simulation Time: " << std::fixed << std::setprecision(3) 
        << metrics.simulationTime << "s\n\n";
    oss << "Memory:\n";
    oss << "  Usage: " << std::fixed << std::setprecision(2) << metrics.memoryUsage << " MB\n";
    oss << "  Per Neuron: " << std::fixed << std::setprecision(6) 
        << metrics.memoryPerNeuron << " MB\n";
    oss << "  Per Synapse: " << std::fixed << std::setprecision(6) 
        << metrics.memoryPerSynapse << " MB\n\n";
    oss << "CPU:\n";
    oss << "  Usage: " << std::fixed << std::setprecision(1) << metrics.cpuUsage << "%\n";
    oss << "  Per Neuron: " << std::fixed << std::setprecision(8) 
        << metrics.cpuPerNeuron << "%\n\n";
    oss << "Neural Activity:\n";
    oss << "  Total Spikes: " << metrics.totalSpikeCount << "\n";
    oss << "  Firing Neurons: " << metrics.firingNeuronCount << "\n";
    oss << "  Average Firing Rate: " << std::fixed << std::setprecision(2) 
        << metrics.averageFiringRate << " Hz\n";
    oss << "  Firing Density: " << std::fixed << std::setprecision(2) 
        << metrics.firingDensity << " Hz/1000neurons\n\n";
    
    return oss.str();
}

// BenchmarkSuite implementation
BenchmarkSuite::BenchmarkSuite() {
    // Enable default phases
    enablePhase("basic");
    enablePhase("scaling");
    enablePhase("configuration");
    
    // Configure default stress testing
    stressTestingEnabled_ = false;
    stressTestSteps_ = 1000;
    stressTestDuration_ = 60.0;
}

BenchmarkSuite::~BenchmarkSuite() {
}

void BenchmarkSuite::configure(const BenchmarkConfig& config) {
    config_ = config;
}

void BenchmarkSuite::addConfiguration(const std::string& name, const BenchmarkConfig::Configuration& config) {
    configurations_[name] = config;
}

void BenchmarkSuite::removeConfiguration(const std::string& name) {
    configurations_.erase(name);
}

void BenchmarkSuite::clearConfigurations() {
    configurations_.clear();
}

void BenchmarkSuite::enablePhase(const std::string& phase) {
    if (std::find(enabledPhases_.begin(), enabledPhases_.end(), phase) == enabledPhases_.end()) {
        enabledPhases_.push_back(phase);
    }
}

void BenchmarkSuite::disablePhase(const std::string& phase) {
    auto it = std::find(enabledPhases_.begin(), enabledPhases_.end(), phase);
    if (it != enabledPhases_.end()) {
        enabledPhases_.erase(it);
    }
}

bool BenchmarkSuite::isPhaseEnabled(const std::string& phase) const {
    return std::find(enabledPhases_.begin(), enabledPhases_.end(), phase) != enabledPhases_.end();
}

BenchmarkSuiteResult BenchmarkSuite::run() {
    return run(config_);
}

BenchmarkSuiteResult BenchmarkSuite::run(const BenchmarkConfig& config) {
    BenchmarkSuiteResult result;
    result.suiteName = config.benchmarkSuiteName;
    result.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    startTime_ = std::chrono::steady_clock::now();
    result.totalTime = 0.0;
    
    std::vector<BenchmarkMetrics> allResults;
    
    try {
        // Run basic benchmarks if enabled
        if (isPhaseEnabled("basic") && config.runBasicBenchmark) {
            NLM_LOG_INFO("Running basic benchmarks...");
            runBasicBenchmarkPhase(allResults);
        }
        
        // Run scaling benchmarks if enabled
        if (isPhaseEnabled("scaling") && config.runScalingBenchmark) {
            NLM_LOG_INFO("Running scaling benchmarks...");
            runScalingBenchmarkPhase(allResults);
        }
        
        // Run configuration benchmarks if enabled
        if (isPhaseEnabled("configuration") && !configurations_.empty()) {
            NLM_LOG_INFO("Running configuration benchmarks...");
            runConfigurationBenchmarkPhase(allResults);
        }
        
        // Run stress tests if enabled
        if (stressTestingEnabled_) {
            NLM_LOG_INFO("Running stress tests...");
            runStressTestPhase(allResults);
        }
        
        // Aggregate results
        result = aggregateResults(allResults);
        
        result.completedTime = std::chrono::steady_clock::now();
        result.totalTime = std::chrono::duration<double>(result.completedTime - startTime_).count();
        
        // Generate report if enabled
        if (config.generateDetailedReports) {
            generateSuiteReport(result);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Benchmark suite failed: " + std::string(e.what()));
        result.detectedIssues.push_back("Suite execution failed: " + std::string(e.what()));
    }
    
    return result;
}

void BenchmarkSuite::runBasicBenchmarkPhase(std::vector<BenchmarkMetrics>& results) {
    // Run benchmarks on predefined scales
    size_t neuronCount = 0;
    size_t stepCount = 0;
    
    for (size_t i = 0; i < config_.neuronCounts.size() && 
         i < config_.stepCounts.size(); ++i) {
        if (neuronCount > 0 && stepCount > 0) {
            // Run default configuration
            auto metrics = runSingleBenchmark(neuronCount, stepCount, 
                                            config_.configurations.empty() ? 
                                            createDefaultConfiguration() : 
                                            config_.configurations[0]);
            results.push_back(metrics);
        }
        
        neuronCount = config_.neuronCounts[i];
        stepCount = config_.stepCounts[i];
    }
}

void BenchmarkSuite::runScalingBenchmarkPhase(std::vector<BenchmarkMetrics>& results) {
    // Run additional scaling benchmarks with finer granularity
    std::vector<size_t> scalingNeuronCounts = {50, 100, 200, 500, 1000, 2000};
    
    for (size_t neuronCount : scalingNeuronCounts) {
        size_t steps = std::min(neuronCount / 2, size_t(1000));
        
        auto metrics = runSingleBenchmark(neuronCount, steps, 
                                        config_.configurations.empty() ? 
                                        createDefaultConfiguration() : 
                                        config_.configurations[0]);
        results.push_back(metrics);
    }
}

void BenchmarkSuite::runConfigurationBenchmarkPhase(std::vector<BenchmarkMetrics>& results) {
    // Run benchmarks for each configuration
    for (const auto& [name, config] : configurations_) {
        NLM_LOG_INFO("Running benchmarks for configuration: " + name);
        
        for (size_t i = 0; i < config_.neuronCounts.size() && 
             i < config_.stepCounts.size(); ++i) {
            size_t neuronCount = config_.neuronCounts[i];
            size_t steps = config_.stepCounts[i];
            
            auto metrics = runSingleBenchmark(neuronCount, steps, config);
            results.push_back(metrics);
        }
    }
}

void BenchmarkSuite::runStressTestPhase(std::vector<BenchmarkMetrics>& results) {
    // Run stress tests
    StressTestSuite stressSuite;
    stressSuite.configure(
        config_.neuronCounts.back(),
        stressTestSteps_,
        stressTestDuration_
    );
    
    auto stressResults = stressSuite.run();
    results.insert(results.end(), stressResults.begin(), stressResults.end());
}

BenchmarkMetrics BenchmarkSuite::runSingleBenchmark(size_t neurons, size_t steps,
                                                  const BenchmarkConfig::Configuration& config) {
    // Create benchmark run instance
    BenchmarkRun benchmark(
        std::make_shared<Config>(),
        neurons,
        steps
    );
    
    // Apply configuration
    benchmark.setConfiguration("stress_test", config);
    
    // Run benchmark
    BenchmarkMetrics metrics = benchmark.run();
    
    // Add configuration settings to metrics
    metrics.memoryPoolEnabled = config.enableMemoryPool;
    metrics.soaVectorizationEnabled = config.enableSOAVectorization;
    metrics.eventQueueEnabled = config.enableEventQueue;
    metrics.sparseConnectivityEnabled = config.enableSparseConnectivity;
    metrics.multithreadingEnabled = config.enableMultithreading;
    metrics.simdLevel = config.simdLevel;
    
    return metrics;
}

BenchmarkConfig::Configuration BenchmarkSuite::createDefaultConfiguration() const {
    BenchmarkConfig::Configuration config;
    config.name = "default";
    config.enableMemoryPool = true;
    config.enableSOAVectorization = true;
    config.enableEventQueue = true;
    config.enableSparseConnectivity = true;
    config.enableMultithreading = true;
    config.threadCount = std::thread::hardware_concurrency();
    config.simdLevel = "AVX2";
    return config;
}

void BenchmarkSuite::analyzeScaling(std::vector<BenchmarkMetrics>& results) {
    // Analyze scaling behavior
    if (results.empty()) return;
    
    // Extract scaling data
    for (const auto& metrics : results) {
        result_.scalingData.push_back(
            std::make_pair(metrics.neuronCount, metrics.stepsPerSecond)
        );
    }
    
    // Sort by neuron count
    std::sort(result_.scalingData.begin(), result_.scalingData.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
}

void BenchmarkSuite::analyzeConfigurations(std::vector<BenchmarkMetrics>& results) {
    // Analyze performance across configurations
    std::map<std::string, std::vector<double>> configPerformance;
    
    for (const auto& metrics : results) {
        configPerformance[metrics.configurationName].push_back(metrics.stepsPerSecond);
    }
    
    for (const auto& [name, perfValues] : configPerformance) {
        double sum = std::accumulate(perfValues.begin(), perfValues.end(), 0.0);
        double avg = sum / perfValues.size();
        result_.configPerformance[name] = avg;
    }
}

void BenchmarkSuite::detectPerformanceIssues(std::vector<BenchmarkMetrics>& results) {
    // Detect performance issues across all benchmarks
    for (const auto& metrics : results) {
        // Check memory usage
        if (metrics.memoryUsage > config_.maxMemoryUsage) {
            result_.detectedIssues.push_back(
                "High memory usage: " + std::to_string(metrics.neuronCount) + 
                " neurons using " + std::to_string(metrics.memoryUsage) + " MB"
            );
        }
        
        // Check performance
        if (metrics.stepsPerSecond < config_.minStepsPerSecond) {
            result_.detectedIssues.push_back(
                "Low performance: " + std::to_string(metrics.neuronCount) + 
                " neurons achieving only " + std::to_string(metrics.stepsPerSecond) + " steps/sec"
            );
        }
        
        // Check efficiency
        if (metrics.efficiency < 0.001) { // Less than 0.001 steps/sec per neuron
            result_.detectedIssues.push_back(
                "Poor efficiency: " + std::to_string(metrics.neuronCount) + 
                " neurons have efficiency " + std::to_string(metrics.efficiency)
            );
        }
    }
}

void BenchmarkSuite::generateSuiteReport(const BenchmarkSuiteResult& result) {
    // Generate comprehensive benchmark report
    std::filesystem::create_directories(config_.outputDirectory);
    
    std::string reportFile = config_.outputDirectory + "/benchmark_suite_report.txt";
    std::ofstream report(reportFile);
    
    if (report.is_open()) {
        report << "NLM Benchmark Suite Report\n";
        report << "==========================\n\n";
        report << "Suite: " << result.suiteName << "\n";
        report << "Total Time: " << std::fixed << std::setprecision(3) << result.totalTime << "s\n";
        report << "Benchmarks Run: " << result.totalBenchmarks << "\n";
        report << "Total Neurons Tested: " << result.totalNeuronsTested << "\n\n";
        
        report << "Performance Summary:\n";
        report << "  Average Steps/sec: " << std::fixed << std::setprecision(2) 
               << result.avgStepsPerSecond << "\n";
        report << "  Min Steps/sec: " << std::fixed << std::setprecision(2) 
               << result.minStepsPerSecond << "\n";
        report << "  Max Steps/sec: " << std::fixed << std::setprecision(2) 
               << result.maxStepsPerSecond << "\n";
        report << "  Std Dev: " << std::fixed << std::setprecision(2) 
               << result.stdDevStepsPerSecond << "\n\n";
        
        report << "Memory Summary:\n";
        report << "  Average Usage: " << std::fixed << std::setprecision(2) 
               << result.avgMemoryUsage << " MB\n";
        report << "  Min Usage: " << std::fixed << std::setprecision(2) 
               << result.minMemoryUsage << " MB\n";
        report << "  Max Usage: " << std::fixed << std::setprecision(2) 
               << result.maxMemoryUsage << " MB\n\n";
        
        report << "Scaling Analysis:\n";
        for (const auto& [neurons, perf] : result.scalingData) {
            report << "  " << neurons << " neurons: " << std::fixed << std::setprecision(2) 
                   << perf << " steps/sec\n";
        }
        report << "\n";
        
        report << "Configuration Analysis:\n";
        for (const auto& [name, perf] : result.configPerformance) {
            report << "  " << name << ": " << std::fixed << std::setprecision(2) 
                   << perf << " steps/sec\n";
        }
        report << "\n";
        
        if (!result.detectedIssues.empty()) {
            report << "Performance Issues Detected:\n";
            for (const auto& issue : result.detectedIssues) {
                report << "  - " << issue << "\n";
            }
            report << "\n";
        }
        
        report << "Quality Score: " << std::fixed << std::setprecision(3) 
               << result.benchmarkQuality << "\n";
        report << "Regression Risk: " << std::fixed << std::setprecision(3) 
               << result.regressionRisk << "\n";
        
        report.close();
        
        NLM_LOG_INFO("Benchmark suite report written to: " + reportFile);
    }
}

std::string BenchmarkSuite::formatResults(const std::vector<BenchmarkMetrics>& results) const {
    std::ostringstream oss;
    oss << "Benchmark Results Summary\n";
    oss << "==========================\n\n";
    
    for (const auto& metrics : results) {
        oss << "Configuration: " << metrics.configurationName << "\n";
        oss << "Neurons: " << metrics.neuronCount << " | Steps: " << metrics.steps << "\n";
        oss << "Steps/sec: " << std::fixed << std::setprecision(2) << metrics.stepsPerSecond << "\n";
        oss << "Memory: " << std::fixed << std::setprecision(2) << metrics.memoryUsage << " MB\n";
        oss << "CPU: " << std::fixed << std::setprecision(1) << metrics.cpuUsage << "%\n";
        oss << "Efficiency: " << std::fixed << std::setprecision(6) << metrics.efficiency << " steps/sec/neuron\n\n";
    }
    
    return oss.str();
}

double BenchmarkSuite::calculateQualityScore(const std::vector<BenchmarkMetrics>& results) const {
    if (results.empty()) return 0.0;
    
    double totalScore = 0.0;
    
    for (const auto& metrics : results) {
        double score = 0.0;
        
        // Performance score (0-1)
        double performanceScore = std::min(1.0, metrics.stepsPerSecond / 1000.0);
        score += performanceScore * 0.4;
        
        // Memory efficiency score (0-1)
        double memoryScore = std::min(1.0, 1000.0 / std::max(1.0, metrics.memoryPerNeuron));
        score += memoryScore * 0.3;
        
        // CPU efficiency score (0-1)
        double cpuScore = std::min(1.0, 100.0 / std::max(1.0, metrics.cpuPerNeuron));
        score += cpuScore * 0.2;
        
        // Neural activity score (0-1)
        double activityScore = std::min(1.0, metrics.averageFiringRate / 50.0);
        score += activityScore * 0.1;
        
        totalScore += score;
    }
    
    return totalScore / results.size();
}

double BenchmarkSuite::calculateRegressionRisk(const std::vector<BenchmarkMetrics>& results) const {
    if (results.size() < 3) return 0.0;
    
    // Calculate coefficient of variation for key metrics
    std::vector<double> stepsPerSecond;
    for (const auto& metrics : results) {
        stepsPerSecond.push_back(metrics.stepsPerSecond);
    }
    
    double mean = std::accumulate(stepsPerSecond.begin(), stepsPerSecond.end(), 0.0) / stepsPerSecond.size();
    double sumSquares = 0.0;
    
    for (double value : stepsPerSecond) {
        double diff = value - mean;
        sumSquares += diff * diff;
    }
    
    double stdDev = std::sqrt(sumSquares / (stepsPerSecond.size() - 1));
    double coefficientOfVariation = stdDev / mean;
    
    return std::min(1.0, coefficientOfVariation * 10.0);
}

BenchmarkSuiteResult BenchmarkSuite::aggregateResults(std::vector<BenchmarkMetrics>& results) {
    BenchmarkSuiteResult result;
    
    if (results.empty()) return result;
    
    // Set basic metadata
    result.suiteName = config_.benchmarkSuiteName;
    result.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // Calculate totals
    result.totalBenchmarks = results.size();
    result.totalNeuronsTested = 0;
    
    for (const auto& metrics : results) {
        result.totalNeuronsTested += metrics.neuronCount;
    }
    
    // Calculate performance statistics
    std::vector<double> stepsPerSecond;
    std::vector<double> memoryUsages;
    
    for (const auto& metrics : results) {
        stepsPerSecond.push_back(metrics.stepsPerSecond);
        memoryUsages.push_back(metrics.memoryUsage);
    }
    
    if (!stepsPerSecond.empty()) {
        double sum = std::accumulate(stepsPerSecond.begin(), stepsPerSecond.end(), 0.0);
        result.avgStepsPerSecond = sum / stepsPerSecond.size();
        
        double sumSquares = 0.0;
        for (double value : stepsPerSecond) {
            double diff = value - result.avgStepsPerSecond;
            sumSquares += diff * diff;
        }
        
        result.stdDevStepsPerSecond = std::sqrt(sumSquares / (stepsPerSecond.size() - 1));
        result.minStepsPerSecond = *std::min_element(stepsPerSecond.begin(), stepsPerSecond.end());
        result.maxStepsPerSecond = *std::max_element(stepsPerSecond.end(), stepsPerSecond.begin());
    }
    
    if (!memoryUsages.empty()) {
        double memSum = std::accumulate(memoryUsages.begin(), memoryUsages.end(), 0.0);
        result.avgMemoryUsage = memSum / memoryUsages.size();
        result.minMemoryUsage = *std::min_element(memoryUsages.begin(), memoryUsages.end());
        result.maxMemoryUsage = *std::max_element(memoryUsages.begin(), memoryUsages.end());
    }
    
    // Analyze scaling and configurations
    analyzeScaling(results);
    analyzeConfigurations(results);
    
    // Detect issues
    detectPerformanceIssues(results);
    
    // Calculate quality and risk
    result.benchmarkQuality = calculateQualityScore(results);
    result.regressionRisk = calculateRegressionRisk(results);
    
    return result;
}

// PerformanceRegressionTester implementation
PerformanceRegressionTester::PerformanceRegressionTester() : baselineLoaded_(false) {
}

PerformanceRegressionTester::~PerformanceRegressionTester() {
}

void PerformanceRegressionTester::configure(const std::string& baselineFile) {
    baselineFile_ = baselineFile;
}

bool PerformanceRegressionTester::loadBaseline(const std::string& filename) {
    // For now, implement simple loading
    baselineFile_ = filename;
    baselineLoaded_ = !filename.empty();
    return baselineLoaded_;
}

bool PerformanceRegressionTester::saveBaseline(const std::string& filename) {
    // For now, placeholder implementation
    baselineFile_ = filename;
    baselineLoaded_ = true;
    return true;
}

std::vector<PerformanceRegressionTester::RegressionResult> 
PerformanceRegressionTester::testRegression(const BenchmarkSuiteResult& current) {
    std::vector<RegressionResult> regressions;
    
    if (!baselineLoaded_) return regressions;
    
    // Compare key metrics with baseline
    std::map<std::string, double> baselineValues;
    std::map<std::string, double> currentValues;
    
    // Extract baseline values (simplified)
    baselineValues["avgStepsPerSecond"] = baselineData_.result.avgStepsPerSecond;
    baselineValues["avgMemoryUsage"] = baselineData_.result.avgMemoryUsage;
    baselineValues["benchmarkQuality"] = baselineData_.result.benchmarkQuality;
    
    // Extract current values
    currentValues["avgStepsPerSecond"] = current.avgStepsPerSecond;
    currentValues["avgMemoryUsage"] = current.avgMemoryUsage;
    currentValues["benchmarkQuality"] = current.benchmarkQuality;
    
    // Compare each metric
    for (const auto& [metric, currentVal] : currentValues) {
        if (baselineValues.find(metric) != baselineValues.end()) {
            RegressionResult result;
            result.metric = metric;
            result.currentValue = currentVal;
            result.baselineValue = baselineValues[metric];
            result.difference = currentVal - baselineValues[metric];
            result.percentageChange = (result.difference / baselineValues[metric]) * 100.0;
            result.status = determineRegressionStatus(result.percentageChange);
            result.severity = determineRegressionSeverity(result.percentageChange);
            result.recommendation = generateRecommendation(metric, result.percentageChange);
            
            regressions.push_back(result);
        }
    }
    
    return regressions;
}

bool PerformanceRegressionTester::isRegressionDetected(const std::vector<RegressionResult>& results) const {
    for (const auto& result : results) {
        if (result.severity == "HIGH" || result.severity == "CRITICAL") {
            return true;
        }
    }
    return false;
}

double PerformanceRegressionTester::calculateRegressionScore(const std::vector<RegressionResult>& results) const {
    double totalScore = 0.0;
    
    for (const auto& result : results) {
        double score = 0.0;
        
        if (result.status == "REGRESSED") {
            score = std::abs(result.percentageChange);
        } else if (result.status == "IMPROVED") {
            score = std::abs(result.percentageChange) * 0.5;
        }
        
        // Apply severity multiplier
        if (result.severity == "HIGH" || result.severity == "CRITICAL") {
            score *= 2.0;
        }
        
        totalScore += score;
    }
    
    return std::min(100.0, totalScore);
}

PerformanceRegressionTester::RegressionResult 
PerformanceRegressionTester::compareMetric(const std::string& metricName,
                                          double currentValue,
                                          double baselineValue) const {
    RegressionResult result;
    result.metric = metricName;
    result.currentValue = currentValue;
    result.baselineValue = baselineValue;
    result.difference = currentValue - baselineValue;
    result.percentageChange = (result.difference / baselineValue) * 100.0;
    result.status = determineRegressionStatus(result.percentageChange);
    result.severity = determineRegressionSeverity(result.percentageChange);
    result.recommendation = generateRecommendation(metricName, result.percentageChange);
    
    return result;
}

std::string PerformanceRegressionTester::determineRegressionStatus(double change) const {
    if (std::abs(change) < 5.0) {
        return "STABLE";
    } else if (change > 5.0) {
        return "IMPROVED";
    } else {
        return "REGRESSED";
    }
}

std::string PerformanceRegressionTester::determineRegressionSeverity(double change) const {
    double absChange = std::abs(change);
    
    if (absChange > 20.0) {
        return "CRITICAL";
    } else if (absChange > 10.0) {
        return "HIGH";
    } else if (absChange > 5.0) {
        return "MEDIUM";
    } else {
        return "LOW";
    }
}

std::string PerformanceRegressionTester::generateRecommendation(const std::string& metric, 
                                                                double change) const {
    if (metric == "avgStepsPerSecond") {
        if (change < -10.0) {
            return "Investigate performance bottlenecks in neural processing.";
        } else if (change > 10.0) {
            return "Performance optimization successful.";
        }
    } else if (metric == "avgMemoryUsage") {
        if (change > 10.0) {
            return "Optimize memory usage through pooling and compression.";
        }
    } else if (metric == "benchmarkQuality") {
        if (change < -5.0) {
            return "Review benchmark suite and test conditions.";
        }
    }
    
    return "Continue monitoring. No immediate action required.";
}

// ComparativeBenchmark implementation
ComparativeBenchmark::ComparativeBenchmark() {
}

ComparativeBenchmark::~ComparativeBenchmark() {
}

void ComparativeBenchmark::addBenchmarkResult(const std::string& version, 
                                            const BenchmarkSuiteResult& result) {
    results_[version] = result;
}

void ComparativeBenchmark::removeBenchmarkResult(const std::string& version) {
    results_.erase(version);
}

void ComparativeBenchmark::clearResults() {
    results_.clear();
}

std::vector<ComparativeBenchmark::ComparativeResult> ComparativeBenchmark::compareVersions() {
    std::vector<ComparativeResult> comparativeResults;
    
    if (results_.empty()) return comparativeResults;
    
    // Extract key metrics from all versions
    std::map<std::string, std::map<std::string, double>> metricValues;
    
    for (const auto& [version, result] : results_) {
        metricValues["avgStepsPerSecond"][version] = result.avgStepsPerSecond;
        metricValues["avgMemoryUsage"][version] = result.avgMemoryUsage;
        metricValues["benchmarkQuality"][version] = result.benchmarkQuality;
    }
    
    // Compare each metric
    for (const auto& [metricName, versionValues] : metricValues) {
        ComparativeResult result;
        result.metric = metricName;
        result.versionValues = versionValues;
        
        // Find best and worst
        double bestVal = 0.0, worstVal = std::numeric_limits<double>::max();
        std::string bestVersion, worstVersion;
        
        for (const auto& [version, value] : versionValues) {
            if (metricName == "avgStepsPerSecond" || metricName == "benchmarkQuality") {
                // Higher is better for these metrics
                if (value > bestVal) {
                    bestVal = value;
                    bestVersion = version;
                }
                if (value < worstVal) {
                    worstVal = value;
                    worstVersion = version;
                }
            } else {
                // Lower is better for memory usage
                if (value < bestVal) {
                    bestVal = value;
                    bestVersion = version;
                }
                if (value > worstVal) {
                    worstVal = value;
                    worstVersion = version;
                }
            }
        }
        
        result.bestValue = bestVal;
        result.bestVersion = bestVersion;
        result.worstValue = worstVal;
        result.worstVersion = worstVersion;
        
        // Calculate coefficient of variation
        double sum = 0.0, sumSquares = 0.0;
        for (const auto& value : versionValues) {
            sum += value.second;
            sumSquares += value.second * value.second;
        }
        
        double mean = sum / versionValues.size();
        double variance = (sumSquares / versionValues.size()) - (mean * mean);
        double stdDev = std::sqrt(std::max(0.0, variance));
        result.coefficientOfVariation = (mean > 0.0) ? (stdDev / mean) : 0.0;
        
        // Determine trend
        result.trendDirection = determineTrend(getVersionValuesInOrder(metricName));
        
        comparativeResults.push_back(result);
    }
    
    return comparativeResults;
}

std::vector<double> ComparativeBenchmark::getVersionValuesInOrder(const std::string& metricName) const {
    std::vector<std::pair<uint64_t, double>> timestampedValues;
    
    for (const auto& [version, result] : results_) {
        double value = 0.0;
        if (metricName == "avgStepsPerSecond") {
            value = result.avgStepsPerSecond;
        } else if (metricName == "avgMemoryUsage") {
            value = result.avgMemoryUsage;
        } else if (metricName == "benchmarkQuality") {
            value = result.benchmarkQuality;
        }
        
        timestampedValues.push_back(std::make_pair(result.timestamp, value));
    }
    
    // Sort by timestamp
    std::sort(timestampedValues.begin(), timestampedValues.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    std::vector<double> values;
    for (const auto& pair : timestampedValues) {
        values.push_back(pair.second);
    }
    
    return values;
}

ComparativeBenchmark::ComparativeResult 
ComparativeBenchmark::analyzeMetric(const std::string& metricName,
                                   const std::map<std::string, double>& values) const {
    ComparativeResult result;
    result.metric = metricName;
    result.versionValues = values;
    
    // Find best and worst
    double bestVal = 0.0, worstVal = std::numeric_limits<double>::max();
    std::string bestVersion, worstVersion;
    
    for (const auto& [version, value] : values) {
        if (value > bestVal) {
            bestVal = value;
            bestVersion = version;
        }
        if (value < worstVal) {
            worstVal = value;
            worstVersion = version;
        }
    }
    
    result.bestValue = bestVal;
    result.bestVersion = bestVersion;
    result.worstValue = worstVal;
    result.worstVersion = worstVersion;
    
    // Calculate coefficient of variation
    double sum = 0.0, sumSquares = 0.0;
    for (const auto& value : values) {
        sum += value.second;
        sumSquares += value.second * value.second;
    }
    
    double mean = sum / values.size();
    double variance = (sumSquares / values.size()) - (mean * mean);
    double stdDev = std::sqrt(std::max(0.0, variance));
    result.coefficientOfVariation = (mean > 0.0) ? (stdDev / mean) : 0.0;
    
    return result;
}

std::string ComparativeBenchmark::determineTrend(const std::vector<double>& values) const {
    if (values.size() < 2) return "insufficient_data";
    
    // Simple linear regression to determine trend
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    size_t n = values.size();
    
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        double y = values[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    
    if (slope > 0.01) {
        return "improving";
    } else if (slope < -0.01) {
        return "declining";
    } else {
        return "stable";
    }
}

// StressTestSuite implementation
StressTestSuite::StressTestSuite() : stressSteps_(0), stressDuration_(0.0) {
}

StressTestSuite::~StressTestSuite() {
}

void StressTestSuite::configure(size_t neuronCount, size_t steps, double duration) {
    stressNeuronCount_ = neuronCount;
    stressSteps_ = steps;
    stressDuration_ = duration;
}

void StressTestSuite::addStressTest(StressTestType type, const std::string& description) {
    StressTestScenario scenario;
    scenario.type = type;
    scenario.description = description;
    
    // Add appropriate setup and run functions based on test type
    switch (type) {
        case StressTestType::MemoryPressure:
            scenario.setup = [this](BenchmarkMetrics& metrics) {
                metrics.memoryUsage *= 2.0; // Simulate memory pressure
            };
            scenario.runStep = [this](size_t step, double time) {
                BenchmarkMetrics metrics;
                metrics.neuronCount = stressNeuronCount_;
                metrics.steps = stressSteps_;
                metrics.wallClockTime = stressDuration_;
                metrics.simulationTime = time;
                metrics.stepsPerSecond = (stressSteps_ / stressDuration_) * (1.0 - (time / stressDuration_));
                metrics.memoryUsage = 512.0 + (time * 10.0); // Increasing memory usage
                metrics.cpuUsage = 80.0 + std::sin(time * 2.0) * 20.0; // Oscillating CPU
                return metrics;
            };
            break;
        
        case StressTestType::CPUUtilization:
            scenario.setup = [this](BenchmarkMetrics& metrics) {};
            scenario.runStep = [this](size_t step, double time) {
                BenchmarkMetrics metrics;
                metrics.neuronCount = stressNeuronCount_;
                metrics.steps = stressSteps_;
                metrics.wallClockTime = stressDuration_;
                metrics.simulationTime = time;
                metrics.stepsPerSecond = (stressSteps_ / stressDuration_) * (0.5 + 0.5 * std::sin(time));
                metrics.cpuUsage = 95.0 + std::cos(time * 0.5) * 10.0; // High CPU with variations
                metrics.memoryUsage = 256.0 + time * 5.0; // Increasing memory
                return metrics;
            };
            break;
        
        default:
            // Default implementation
            scenario.setup = [this](BenchmarkMetrics& metrics) {};
            scenario.runStep = [this](size_t step, double time) {
                BenchmarkMetrics metrics;
                metrics.neuronCount = stressNeuronCount_;
                metrics.steps = stressSteps_;
                metrics.wallClockTime = stressDuration_;
                metrics.simulationTime = time;
                metrics.stepsPerSecond = (stressSteps_ / stressDuration_) * (1.0 - (time * 0.1));
                metrics.memoryUsage = 256.0 + time * 2.0;
                metrics.cpuUsage = 70.0 + std::sin(time) * 15.0;
                return metrics;
            };
            break;
    }
    
    stressTests_.push_back(scenario);
}

std::vector<BenchmarkMetrics> StressTestSuite::run() {
    std::vector<BenchmarkMetrics> results;
    
    for (const auto& scenario : stressTests_) {
        BenchmarkMetrics metrics = runStressTest(scenario);
        results.push_back(metrics);
    }
    
    return results;
}

BenchmarkMetrics StressTestSuite::runStressTest(const StressTestScenario& scenario) {
    BenchmarkMetrics metrics;
    metrics.neuronCount = stressNeuronCount_;
    metrics.steps = stressSteps_;
    metrics.wallClockTime = stressDuration_;
    metrics.configurationName = "stress_test";
    
    // Setup test
    scenario.setup(metrics);
    
    // Run stress test
    auto startTime = std::chrono::steady_clock::now();
    double timeStep = stressDuration_ / stressSteps_;
    
    for (size_t step = 0; step < stressSteps_; ++step) {
        double currentTime = step * timeStep;
        BenchmarkMetrics stepMetrics = scenario.runStep(step, currentTime);
        
        // Update aggregate metrics
        metrics.wallClockTime = std::max(metrics.wallClockTime, stepMetrics.wallClockTime);
        metrics.simulationTime = std::max(metrics.simulationTime, stepMetrics.simulationTime);
        metrics.memoryUsage = std::max(metrics.memoryUsage, stepMetrics.memoryUsage);
        metrics.cpuUsage = std::max(metrics.cpuUsage, stepMetrics.cpuUsage);
        
        // Track stability
        metrics.stability += std::abs(stepMetrics.stepsPerSecond - stepMetrics.sustainedPerformance);
        metrics.stressSteps++;
    }
    
    auto endTime = std::chrono::steady_clock::now();
    metrics.wallClockTime = std::chrono::duration<double>(endTime - startTime).count();
    
    if (metrics.stressSteps > 0) {
        metrics.stability /= metrics.stressSteps;
    }
    
    // Calculate sustained performance
    double totalStepsPerSecond = 0.0;
    for (size_t step = 0; step < stressSteps_; ++step) {
        double currentTime = step * timeStep;
        BenchmarkMetrics stepMetrics = scenario.runStep(step, currentTime);
        totalStepsPerSecond += stepMetrics.stepsPerSecond;
    }
    
    metrics.sustainedPerformance = totalStepsPerSecond / stressSteps_;
    
    return metrics;
}

// BenchmarkManager implementation
BenchmarkManager::BenchmarkManager()
    : regressionTestingEnabled_(false)
    , running_(false)
    , progress_(0.0)
    , currentStatus_("Initialized") {
}

BenchmarkManager::~BenchmarkManager() {
}

void BenchmarkManager::initialize(std::shared_ptr<Config> config) {
    config_ = config;
}

void BenchmarkManager::configure(const BenchmarkConfig& config) {
    benchmarkConfig_ = config;
    benchmarkSuite_.configure(config);
}

BenchmarkSuiteResult BenchmarkManager::runBenchmarkSuite() {
    return runBenchmarkSuite(benchmarkConfig_);
}

BenchmarkSuiteResult BenchmarkManager::runBenchmarkSuite(const BenchmarkConfig& config) {
    running_ = true;
    progress_ = 0.0;
    currentStatus_ = "Running benchmark suite";
    
    startTime_ = std::chrono::steady_clock::now();
    
    try {
        // Run benchmark suite
        BenchmarkSuiteResult result = benchmarkSuite_.run(config);
        
        // Perform regression testing if enabled
        if (regressionTestingEnabled_) {
            currentStatus_ = "Performing regression testing";
            progress_ = 0.5;
            
            std::vector<BenchmarkSuiteResult> regressions = regressionTester_.testRegression(result);
            
            if (regressionTester_.isRegressionDetected(regressions)) {
                currentStatus_ = "REGRESSION DETECTED";
            } else {
                currentStatus_ = "No regressions detected";
            }
            
            progress_ = 0.7;
        }
        
        // Update progress
        currentStatus_ = "Generating reports";
        progress_ = 0.8;
        
        // Generate output files
        generateOutputFiles(result);
        
        // Complete
        progress_ = 1.0;
        currentStatus_ = "Benchmark complete";
        
        return result;
        
    } catch (const std::exception& e) {
        currentStatus_ = "Error: " + std::string(e.what());
        NLM_LOG_ERROR("Benchmark manager failed: " + std::string(e.what()));
        return BenchmarkSuiteResult{};
    }
}

void BenchmarkManager::updateProgress(double progress, const std::string& status) {
    std::lock_guard<std::mutex> lock(benchmarkSuite_.getMutex()); // TODO: Add mutex
    
    progress_ = std::min(1.0, std::max(0.0, progress));
    currentStatus_ = status;
}

void BenchmarkManager::generateOutputFiles(const BenchmarkSuiteResult& result) {
    // Generate JSON report
    if (jsonOutputEnabled_) {
        generateJSONReport(result);
    }
    
    // Generate CSV report
    if (csvOutputEnabled_) {
        generateCSVReport(result);
    }
    
    // Generate analysis report
    if (benchmarkConfig_.generateDetailedReports) {
        std::filesystem::create_directories(outputDirectory_);
        
        std::string analysisFile = outputDirectory_ + "/benchmark_analysis.txt";
        std::ofstream analysis(analysisFile);
        
        if (analysis.is_open()) {
            analysis << "NLM Benchmark Analysis\n";
            analysis << "======================\n\n";
            analysis << "Suite: " << result.suiteName << "\n";
            analysis << "Completed at: " << result.timestamp << "\n\n";
            
            analysis << "Performance Summary:\n";
            analysis << "  Average Steps/sec: " << std::fixed << std::setprecision(2) 
                     << result.avgStepsPerSecond << "\n";
            analysis << "  Performance Std Dev: " << std::fixed << std::setprecision(2) 
                     << result.stdDevStepsPerSecond << "\n\n";
            
            analysis << "Memory Summary:\n";
            analysis << "  Average Usage: " << std::fixed << std::setprecision(2) 
                     << result.avgMemoryUsage << " MB\n\n";
            
            analysis << "Quality Assessment:\n";
            analysis << "  Benchmark Quality: " << std::fixed << std::setprecision(3) 
                     << result.benchmarkQuality << "\n";
            analysis << "  Regression Risk: " << std::fixed << std::setprecision(3) 
                     << result.regressionRisk << "\n\n";
            
            if (!result.detectedIssues.empty()) {
                analysis << "Issues Detected:\n";
                for (const auto& issue : result.detectedIssues) {
                    analysis << "  - " << issue << "\n";
                }
                analysis << "\n";
            }
            
            analysis.close();
        }
    }
}

void BenchmarkManager::generateJSONReport(const BenchmarkSuiteResult& result) {
    // Generate JSON report (simplified)
    std::string jsonFile = outputDirectory_ + "/benchmark_results.json";
    std::ofstream json(jsonFile);
    
    if (json.is_open()) {
        json << "{\n";
        json << "  \"suiteName\": \"" << result.suiteName << "\",\n";
        json << "  \"timestamp\": " << result.timestamp << ",\n";
        json << "  \"totalBenchmarks\": " << result.totalBenchmarks << ",\n";
        json << "  \"totalNeuronsTested\": " << result.totalNeuronsTested << ",\n";
        json << "  \"avgStepsPerSecond\": " << std::fixed << std::setprecision(2) 
             << result.avgStepsPerSecond << ",\n";
        json << "  \"avgMemoryUsage\": " << std::fixed << std::setprecision(2) 
             << result.avgMemoryUsage << ",\n";
        json << "  \"benchmarkQuality\": " << std::fixed << std::setprecision(3) 
             << result.benchmarkQuality << ",\n";
        json << "  \"regressionRisk\": " << std::fixed << std::setprecision(3) 
             << result.regressionRisk << ",\n";
        json << "  \"detectedIssues\": [";
        
        for (size_t i = 0; i < result.detectedIssues.size(); ++i) {
            if (i > 0) json << ", ";
            json << "\"" << result.detectedIssues[i] << "\"";
        }
        
        json << "],\n";
        json << "  \"configPerformance\": {";
        
        bool firstConfig = true;
        for (const auto& [name, perf] : result.configPerformance) {
            if (!firstConfig) json << ", ";
            firstConfig = false;
            json << "\"" << name << "\": " << std::fixed << std::setprecision(2) << perf;
        }
        
        json << "}\n";
        json << "}\n";
        
        json.close();
    }
}

void BenchmarkManager::generateCSVReport(const BenchmarkSuiteResult& result) {
    // Generate CSV report (simplified)
    std::string csvFile = outputDirectory_ + "/benchmark_results.csv";
    std::ofstream csv(csvFile);
    
    if (csv.is_open()) {
        csv << "Metric,Value\n";
        csv << "Suite Name," << result.suiteName << "\n";
        csv << "Total Benchmarks," << result.totalBenchmarks << "\n";
        csv << "Total Neurons Tested," << result.totalNeuronsTested << "\n";
        csv << "Average Steps/sec," << std::fixed << std::setprecision(2) 
            << result.avgStepsPerSecond << "\n";
        csv << "Min Steps/sec," << std::fixed << std::setprecision(2) 
            << result.minStepsPerSecond << "\n";
        csv << "Max Steps/sec," << std::fixed << std::setprecision(2) 
            << result.maxStepsPerSecond << "\n";
        csv << "Performance Std Dev," << std::fixed << std::setprecision(2) 
            << result.stdDevStepsPerSecond << "\n";
        csv << "Average Memory Usage (MB)," << std::fixed << std::setprecision(2) 
            << result.avgMemoryUsage << "\n";
        csv << "Benchmark Quality," << std::fixed << std::setprecision(3) 
            << result.benchmarkQuality << "\n";
        csv << "Regression Risk," << std::fixed << std::setprecision(3) 
            << result.regressionRisk << "\n";
        
        csv.close();
    }
}

} // namespace nlm


// PerformanceRegressionTester implementation
PerformanceRegressionTester::PerformanceRegressionTester() : baselineLoaded_(false) {
}

PerformanceRegressionTester::~PerformanceRegressionTester() {
}

void PerformanceRegressionTester::configure(const std::string& baselineFile) {
    baselineFile_ = baselineFile;
}

bool PerformanceRegressionTester::loadBaseline(const std::string& filename) {
    // For now, implement simple loading
    baselineFile_ = filename;
    baselineLoaded_ = !filename.empty();
    return baselineLoaded_;
}

bool PerformanceRegressionTester::saveBaseline(const std::string& filename) {
    // For now, placeholder implementation
    baselineFile_ = filename;
    baselineLoaded_ = true;
    return true;
}

std::vector<PerformanceRegressionTester::RegressionResult> 
PerformanceRegressionTester::testRegression(const BenchmarkSuiteResult& current) {
    std::vector<RegressionResult> regressions;
    
    if (!baselineLoaded_) return regressions;
    
    // Compare key metrics with baseline
    std::map<std::string, double> baselineValues;
    std::map<std::string, double> currentValues;
    
    // Extract baseline values (simplified)
    baselineValues["avgStepsPerSecond"] = baselineData_.result.avgStepsPerSecond;
    baselineValues["avgMemoryUsage"] = baselineData_.result.avgMemoryUsage;
    baselineValues["benchmarkQuality"] = baselineData_.result.benchmarkQuality;
    
    // Extract current values
    currentValues["avgStepsPerSecond"] = current.avgStepsPerSecond;
    currentValues["avgMemoryUsage"] = current.avgMemoryUsage;
    currentValues["benchmarkQuality"] = current.benchmarkQuality;
    
    // Compare each metric
    for (const auto& [metric, currentVal] : currentValues) {
        if (baselineValues.find(metric) != baselineValues.end()) {
            RegressionResult result;
            result.metric = metric;
            result.currentValue = currentVal;
            result.baselineValue = baselineValues[metric];
            result.difference = currentVal - baselineValues[metric];
            result.percentageChange = (result.difference / baselineValues[metric]) * 100.0;
            result.status = determineRegressionStatus(result.percentageChange);
            result.severity = determineRegressionSeverity(result.percentageChange);
            result.recommendation = generateRecommendation(metric, result.percentageChange);
            
            regressions.push_back(result);
        }
    }
    
    return regressions;
}

bool PerformanceRegressionTester::isRegressionDetected(const std::vector<RegressionResult>& results) const {
    for (const auto& result : results) {
        if (result.severity == "HIGH" || result.severity == "CRITICAL") {
            return true;
        }
    }
    return false;
}

double PerformanceRegressionTester::calculateRegressionScore(const std::vector<RegressionResult>& results) const {
    double totalScore = 0.0;
    
    for (const auto& result : results) {
        double score = 0.0;
        
        if (result.status == "REGRESSED") {
            score = std::abs(result.percentageChange);
        } else if (result.status == "IMPROVED") {
            score = std::abs(result.percentageChange) * 0.5;
        }
        
        // Apply severity multiplier
        if (result.severity == "HIGH" || result.severity == "CRITICAL") {
            score *= 2.0;
        }
        
        totalScore += score;
    }
    
    return std::min(100.0, totalScore);
}

PerformanceRegressionTester::RegressionResult 
PerformanceRegressionTester::compareMetric(const std::string& metricName,
                                          double currentValue,
                                          double baselineValue) const {
    RegressionResult result;
    result.metric = metricName;
    result.currentValue = currentValue;
    result.baselineValue = baselineValue;
    result.difference = currentValue - baselineValue;
    result.percentageChange = (result.difference / baselineValue) * 100.0;
    result.status = determineRegressionStatus(result.percentageChange);
    result.severity = determineRegressionSeverity(result.percentageChange);
    result.recommendation = generateRecommendation(metricName, result.percentageChange);
    
    return result;
}

std::string PerformanceRegressionTester::determineRegressionStatus(double change) const {
    if (std::abs(change) < 5.0) {
        return "STABLE";
    } else if (change > 5.0) {
        return "IMPROVED";
    } else {
        return "REGRESSED";
    }
}

std::string PerformanceRegressionTester::determineRegressionSeverity(double change) const {
    double absChange = std::abs(change);
    
    if (absChange > 20.0) {
        return "CRITICAL";
    } else if (absChange > 10.0) {
        return "HIGH";
    } else if (absChange > 5.0) {
        return "MEDIUM";
    } else {
        return "LOW";
    }
}

std::string PerformanceRegressionTester::generateRecommendation(const std::string& metric, 
                                                                double change) const {
    if (metric == "avgStepsPerSecond") {
        if (change < -10.0) {
            return "Investigate performance bottlenecks in neural processing.";
        } else if (change > 10.0) {
            return "Performance optimization successful.";
        }
    } else if (metric == "avgMemoryUsage") {
        if (change > 10.0) {
            return "Optimize memory usage through pooling and compression.";
        }
    } else if (metric == "benchmarkQuality") {
        if (change < -5.0) {
            return "Review benchmark suite and test conditions.";
        }
    }
    
    return "Continue monitoring. No immediate action required.";
}

// ComparativeBenchmark implementation
ComparativeBenchmark::ComparativeBenchmark() {
}

ComparativeBenchmark::~ComparativeBenchmark() {
}

void ComparativeBenchmark::addBenchmarkResult(const std::string& version, 
                                            const BenchmarkSuiteResult& result) {
    results_[version] = result;
}

void ComparativeBenchmark::removeBenchmarkResult(const std::string& version) {
    results_.erase(version);
}

void ComparativeBenchmark::clearResults() {
    results_.clear();
}

std::vector<ComparativeBenchmark::ComparativeResult> ComparativeBenchmark::compareVersions() {
    std::vector<ComparativeResult> comparativeResults;
    
    if (results_.empty()) return comparativeResults;
    
    // Extract key metrics from all versions
    std::map<std::string, std::map<std::string, double>> metricValues;
    
    for (const auto& [version, result] : results_) {
        metricValues["avgStepsPerSecond"][version] = result.avgStepsPerSecond;
        metricValues["avgMemoryUsage"][version] = result.avgMemoryUsage;
        metricValues["benchmarkQuality"][version] = result.benchmarkQuality;
    }
    
    // Compare each metric
    for (const auto& [metricName, versionValues] : metricValues) {
        ComparativeResult result;
        result.metric = metricName;
        result.versionValues = versionValues;
        
        // Find best and worst
        double bestVal = 0.0, worstVal = std::numeric_limits<double>::max();
        std::string bestVersion, worstVersion;
        
        for (const auto& [version, value] : versionValues) {
            if (metricName == "avgStepsPerSecond" || metricName == "benchmarkQuality") {
                // Higher is better for these metrics
                if (value > bestVal) {
                    bestVal = value;
                    bestVersion = version;
                }
                if (value < worstVal) {
                    worstVal = value;
                    worstVersion = version;
                }
            } else {
                // Lower is better for memory usage
                if (value < bestVal) {
                    bestVal = value;
                    bestVersion = version;
                }
                if (value > worstVal) {
                    worstVal = value;
                    worstVersion = version;
                }
            }
        }
        
        result.bestValue = bestVal;
        result.bestVersion = bestVersion;
        result.worstValue = worstVal;
        result.worstVersion = worstVersion;
        
        // Calculate coefficient of variation
        double sum = 0.0, sumSquares = 0.0;
        for (const auto& value : versionValues) {
            sum += value.second;
            sumSquares += value.second * value.second;
        }
        
        double mean = sum / versionValues.size();
        double variance = (sumSquares / versionValues.size()) - (mean * mean);
        double stdDev = std::sqrt(std::max(0.0, variance));
        result.coefficientOfVariation = (mean > 0.0) ? (stdDev / mean) : 0.0;
        
        // Determine trend
        result.trendDirection = determineTrend(getVersionValuesInOrder(metricName));
        
        comparativeResults.push_back(result);
    }
    
    return comparativeResults;
}

std::vector<double> ComparativeBenchmark::getVersionValuesInOrder(const std::string& metricName) const {
    std::vector<std::pair<uint64_t, double>> timestampedValues;
    
    for (const auto& [version, result] : results_) {
        double value = 0.0;
        if (metricName == "avgStepsPerSecond") {
            value = result.avgStepsPerSecond;
        } else if (metricName == "avgMemoryUsage") {
            value = result.avgMemoryUsage;
        } else if (metricName == "benchmarkQuality") {
            value = result.benchmarkQuality;
        }
        
        timestampedValues.push_back(std::make_pair(result.timestamp, value));
    }
    
    // Sort by timestamp
    std::sort(timestampedValues.begin(), timestampedValues.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    std::vector<double> values;
    for (const auto& pair : timestampedValues) {
        values.push_back(pair.second);
    }
    
    return values;
}

ComparativeBenchmark::ComparativeResult 
ComparativeBenchmark::analyzeMetric(const std::string& metricName,
                                   const std::map<std::string, double>& values) const {
    ComparativeResult result;
    result.metric = metricName;
    result.versionValues = values;
    
    // Find best and worst
    double bestVal = 0.0, worstVal = std::numeric_limits<double>::max();
    std::string bestVersion, worstVersion;
    
    for (const auto& [version, value] : values) {
        if (value > bestVal) {
            bestVal = value;
            bestVersion = version;
        }
        if (value < worstVal) {
            worstVal = value;
            worstVersion = version;
        }
    }
    
    result.bestValue = bestVal;
    result.bestVersion = bestVersion;
    result.worstValue = worstVal;
    result.worstVersion = worstVersion;
    
    // Calculate coefficient of variation
    double sum = 0.0, sumSquares = 0.0;
    for (const auto& value : values) {
        sum += value.second;
        sumSquares += value.second * value.second;
    }
    
    double mean = sum / values.size();
    double variance = (sumSquares / values.size()) - (mean * mean);
    double stdDev = std::sqrt(std::max(0.0, variance));
    result.coefficientOfVariation = (mean > 0.0) ? (stdDev / mean) : 0.0;
    
    return result;
}

std::string ComparativeBenchmark::determineTrend(const std::vector<double>& values) const {
    if (values.size() < 2) return "insufficient_data";
    
    // Simple linear regression to determine trend
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    size_t n = values.size();
    
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        double y = values[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    
    if (slope > 0.01) {
        return "improving";
    } else if (slope < -0.01) {
        return "declining";
    } else {
        return "stable";
    }
}

// StressTestSuite implementation
StressTestSuite::StressTestSuite() : stressSteps_(0), stressDuration_(0.0) {
}

StressTestSuite::~StressTestSuite() {
}

void StressTestSuite::configure(size_t neuronCount, size_t steps, double duration) {
    stressNeuronCount_ = neuronCount;
    stressSteps_ = steps;
    stressDuration_ = duration;
}

void StressTestSuite::addStressTest(StressTestType type, const std::string& description) {
    StressTestScenario scenario;
    scenario.type = type;
    scenario.description = description;
    
    // Add appropriate setup and run functions based on test type
    switch (type) {
        case StressTestType::MemoryPressure:
            scenario.setup = [this](BenchmarkMetrics& metrics) {
                metrics.memoryUsage *= 2.0; // Simulate memory pressure
            };
            scenario.runStep = [this](size_t step, double time) {
                BenchmarkMetrics metrics;
                metrics.neuronCount = stressNeuronCount_;
                metrics.steps = stressSteps_;
                metrics.wallClockTime = stressDuration_;
                metrics.simulationTime = time;
                metrics.stepsPerSecond = (stressSteps_ / stressDuration_) * (1.0 - (time / stressDuration_));
                metrics.memoryUsage = 512.0 + (time * 10.0); // Increasing memory usage
                metrics.cpuUsage = 80.0 + std::sin(time * 2.0) * 20.0; // Oscillating CPU
                return metrics;
            };
            break;
        
        case StressTestType::CPUUtilization:
            scenario.setup = [this](BenchmarkMetrics& metrics) {};
            scenario.runStep = [this](size_t step, double time) {
                BenchmarkMetrics metrics;
                metrics.neuronCount = stressNeuronCount_;
                metrics.steps = stressSteps_;
                metrics.wallClockTime = stressDuration_;
                metrics.simulationTime = time;
                metrics.stepsPerSecond = (stressSteps_ / stressDuration_) * (0.5 + 0.5 * std::sin(time));
                metrics.cpuUsage = 95.0 + std::cos(time * 0.5) * 10.0; // High CPU with variations
                metrics.memoryUsage = 256.0 + time * 5.0; // Increasing memory
                return metrics;
            };
            break;
        
        default:
            // Default implementation
            scenario.setup = [this](BenchmarkMetrics& metrics) {};
            scenario.runStep = [this](size_t step, double time) {
                BenchmarkMetrics metrics;
                metrics.neuronCount = stressNeuronCount_;
                metrics.steps = stressSteps_;
                metrics.wallClockTime = stressDuration_;
                metrics.simulationTime = time;
                metrics.stepsPerSecond = (stressSteps_ / stressDuration_) * (1.0 - (time * 0.1));
                metrics.memoryUsage = 256.0 + time * 2.0;
                metrics.cpuUsage = 70.0 + std::sin(time) * 15.0;
                return metrics;
            };
            break;
    }
    
    stressTests_.push_back(scenario);
}

std::vector<BenchmarkMetrics> StressTestSuite::run() {
    std::vector<BenchmarkMetrics> results;
    
    for (const auto& scenario : stressTests_) {
        BenchmarkMetrics metrics = runStressTest(scenario);
        results.push_back(metrics);
    }
    
    return results;
}

BenchmarkMetrics StressTestSuite::runStressTest(const StressTestScenario& scenario) {
    BenchmarkMetrics metrics;
    metrics.neuronCount = stressNeuronCount_;
    metrics.steps = stressSteps_;
    metrics.wallClockTime = stressDuration_;
    metrics.configurationName = "stress_test";
    
    // Setup test
    scenario.setup(metrics);
    
    // Run stress test
    auto startTime = std::chrono::steady_clock::now();
    double timeStep = stressDuration_ / stressSteps_;
    
    for (size_t step = 0; step < stressSteps_; ++step) {
        double currentTime = step * timeStep;
        BenchmarkMetrics stepMetrics = scenario.runStep(step, currentTime);
        
        // Update aggregate metrics
        metrics.wallClockTime = std::max(metrics.wallClockTime, stepMetrics.wallClockTime);
        metrics.simulationTime = std::max(metrics.simulationTime, stepMetrics.simulationTime);
        metrics.memoryUsage = std::max(metrics.memoryUsage, stepMetrics.memoryUsage);
        metrics.cpuUsage = std::max(metrics.cpuUsage, stepMetrics.cpuUsage);
        
        // Track stability
        metrics.stability += std::abs(stepMetrics.stepsPerSecond - stepMetrics.sustainedPerformance);
        metrics.stressSteps++;
    }
    
    auto endTime = std::chrono::steady_clock::now();
    metrics.wallClockTime = std::chrono::duration<double>(endTime - startTime).count();
    
    if (metrics.stressSteps > 0) {
        metrics.stability /= metrics.stressSteps;
    }
    
    // Calculate sustained performance
    double totalStepsPerSecond = 0.0;
    for (size_t step = 0; step < stressSteps_; ++step) {
        double currentTime = step * timeStep;
        BenchmarkMetrics stepMetrics = scenario.runStep(step, currentTime);
        totalStepsPerSecond += stepMetrics.stepsPerSecond;
    }
    
    metrics.sustainedPerformance = totalStepsPerSecond / stressSteps_;
    
    return metrics;
}

// BenchmarkManager implementation
BenchmarkManager::BenchmarkManager()
    : regressionTestingEnabled_(false)
    , running_(false)
    , progress_(0.0)
    , currentStatus_("Initialized") {
}

BenchmarkManager::~BenchmarkManager() {
}

void BenchmarkManager::initialize(std::shared_ptr<Config> config) {
    config_ = config;
}

void BenchmarkManager::configure(const BenchmarkConfig& config) {
    benchmarkConfig_ = config;
    benchmarkSuite_.configure(config);
}

BenchmarkSuiteResult BenchmarkManager::runBenchmarkSuite() {
    return runBenchmarkSuite(benchmarkConfig_);
}

BenchmarkSuiteResult BenchmarkManager::runBenchmarkSuite(const BenchmarkConfig& config) {
    running_ = true;
    progress_ = 0.0;
    currentStatus_ = "Running benchmark suite";
    
    startTime_ = std::chrono::steady_clock::now();
    
    try {
        // Run benchmark suite
        BenchmarkSuiteResult result = benchmarkSuite_.run(config);
        
        // Perform regression testing if enabled
        if (regressionTestingEnabled_) {
            currentStatus_ = "Performing regression testing";
            progress_ = 0.5;
            
            std::vector<BenchmarkSuiteResult> regressions = regressionTester_.testRegression(result);
            
            if (regressionTester_.isRegressionDetected(regressions)) {
                currentStatus_ = "REGRESSION DETECTED";
            } else {
                currentStatus_ = "No regressions detected";
            }
            
            progress_ = 0.7;
        }
        
        // Update progress
        currentStatus_ = "Generating reports";
        progress_ = 0.8;
        
        // Generate output files
        generateOutputFiles(result);
        
        // Complete
        progress_ = 1.0;
        currentStatus_ = "Benchmark complete";
        
        return result;
        
    } catch (const std::exception& e) {
        currentStatus_ = "Error: " + std::string(e.what());
        NLM_LOG_ERROR("Benchmark manager failed: " + std::string(e.what()));
        return BenchmarkSuiteResult{};
    }
}

void BenchmarkManager::updateProgress(double progress, const std::string& status) {
    std::lock_guard<std::mutex> lock(benchmarkSuite_.getMutex()); // TODO: Add mutex
    
    progress_ = std::min(1.0, std::max(0.0, progress));
    currentStatus_ = status;
}

void BenchmarkManager::generateOutputFiles(const BenchmarkSuiteResult& result) {
    // Generate JSON report
    if (jsonOutputEnabled_) {
        generateJSONReport(result);
    }
    
    // Generate CSV report
    if (csvOutputEnabled_) {
        generateCSVReport(result);
    }
    
    // Generate analysis report
    if (benchmarkConfig_.generateDetailedReports) {
        std::filesystem::create_directories(outputDirectory_);
        
        std::string analysisFile = outputDirectory_ + "/benchmark_analysis.txt";
        std::ofstream analysis(analysisFile);
        
        if (analysis.is_open()) {
            analysis << "NLM Benchmark Analysis\n";
            analysis << "======================\n\n";
            analysis << "Suite: " << result.suiteName << "\n";
            analysis << "Completed at: " << result.timestamp << "\n\n";
            
            analysis << "Performance Summary:\n";
            analysis << "  Average Steps/sec: " << std::fixed << std::setprecision(2) 
                     << result.avgStepsPerSecond << "\n";
            analysis << "  Performance Std Dev: " << std::fixed << std::setprecision(2) 
                     << result.stdDevStepsPerSecond << "\n\n";
            
            analysis << "Memory Summary:\n";
            analysis << "  Average Usage: " << std::fixed << std::setprecision(2) 
                     << result.avgMemoryUsage << " MB\n\n";
            
            analysis << "Quality Assessment:\n";
            analysis << "  Benchmark Quality: " << std::fixed << std::setprecision(3) 
                     << result.benchmarkQuality << "\n";
            analysis << "  Regression Risk: " << std::fixed << std::setprecision(3) 
                     << result.regressionRisk << "\n\n";
            
            if (!result.detectedIssues.empty()) {
                analysis << "Issues Detected:\n";
                for (const auto& issue : result.detectedIssues) {
                    analysis << "  - " << issue << "\n";
                }
                analysis << "\n";
            }
            
            analysis.close();
        }
    }
}

void BenchmarkManager::generateJSONReport(const BenchmarkSuiteResult& result) {
    // Generate JSON report (simplified)
    std::string jsonFile = outputDirectory_ + "/benchmark_results.json";
    std::ofstream json(jsonFile);
    
    if (json.is_open()) {
        json << "{\n";
        json << "  \"suiteName\": \"" << result.suiteName << "\",\n";
        json << "  \"timestamp\": " << result.timestamp << ",\n";
        json << "  \"totalBenchmarks\": " << result.totalBenchmarks << ",\n";
        json << "  \"totalNeuronsTested\": " << result.totalNeuronsTested << ",\n";
        json << "  \"avgStepsPerSecond\": " << std::fixed << std::setprecision(2) 
             << result.avgStepsPerSecond << ",\n";
        json << "  \"avgMemoryUsage\": " << std::fixed << std::setprecision(2) 
             << result.avgMemoryUsage << ",\n";
        json << "  \"benchmarkQuality\": " << std::fixed << std::setprecision(3) 
             << result.benchmarkQuality << ",\n";
        json << "  \"regressionRisk\": " << std::fixed << std::setprecision(3) 
             << result.regressionRisk << ",\n";
        json << "  \"detectedIssues\": [";
        
        for (size_t i = 0; i < result.detectedIssues.size(); ++i) {
            if (i > 0) json << ", ";
            json << "\"" << result.detectedIssues[i] << "\"";
        }
        
        json << "],\n";
        json << "  \"configPerformance\": {";
        
        bool firstConfig = true;
        for (const auto& [name, perf] : result.configPerformance) {
            if (!firstConfig) json << ", ";
            firstConfig = false;
            json << "\"" << name << "\": " << std::fixed << std::setprecision(2) << perf;
        }
        
        json << "}\n";
        json << "}\n";
        
        json.close();
    }
}

void BenchmarkManager::generateCSVReport(const BenchmarkSuiteResult& result) {
    // Generate CSV report (simplified)
    std::string csvFile = outputDirectory_ + "/benchmark_results.csv";
    std::ofstream csv(csvFile);
    
    if (csv.is_open()) {
        csv << "Metric,Value\n";
        csv << "Suite Name," << result.suiteName << "\n";
        csv << "Total Benchmarks," << result.totalBenchmarks << "\n";
        csv << "Total Neurons Tested," << result.totalNeuronsTested << "\n";
        csv << "Average Steps/sec," << std::fixed << std::setprecision(2) 
            << result.avgStepsPerSecond << "\n";
        csv << "Min Steps/sec," << std::fixed << std::setprecision(2) 
            << result.minStepsPerSecond << "\n";
        csv << "Max Steps/sec," << std::fixed << std::setprecision(2) 
            << result.maxStepsPerSecond << "\n";
        csv << "Performance Std Dev," << std::fixed << std::setprecision(2) 
            << result.stdDevStepsPerSecond << "\n";
        csv << "Average Memory Usage (MB)," << std::fixed << std::setprecision(2) 
            << result.avgMemoryUsage << "\n";
        csv << "Benchmark Quality," << std::fixed << std::setprecision(3) 
            << result.benchmarkQuality << "\n";
        csv << "Regression Risk," << std::fixed << std::setprecision(3) 
            << result.regressionRisk << "\n";
        
        csv.close();
    }
}

} // namespace nlm
