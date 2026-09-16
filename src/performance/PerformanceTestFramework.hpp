// Performance Test Framework Implementation

#include "PerformanceTestFramework.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct PerformanceTestFramework::Impl {
    // Test configuration
    PerformanceTestConfig config_;
    
    // Test results
    std::vector<PerformanceTestResult> results_;
    std::vector<PerformanceTestCase> testCases_;
    
    // Test state
    bool running_ = false;
    size_t currentTestIndex_ = 0;
    std::thread testThread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> stopTests_{false};
    
    // Performance baseline
    std::map<std::string, double> baselineMetrics_;
    std::string baselineFile_;
    
    // Reporting
    bool generateReports_ = true;
    std::string reportDir_;
    std::string reportFormat_ = "json";
};

PerformanceTestFramework::PerformanceTestFramework() : pImpl_(std::make_unique<Impl>()) {
    // Initialize with reasonable defaults
    pImpl_->config_.name = "NLM Performance Tests";
    pImpl_->config_.description = "Comprehensive performance testing for NLM neural simulation";
    pImpl_->config_.testSuite = "default";
    pImpl_->config_.iterations = 3;
    pImpl_->config_.warmupIterations = 1;
    pImpl_->config_.timeoutSeconds = 300;
    pImpl_->config_.memoryLimitMB = 2048;
    pImpl_->config_.threadCount = 1;
    pImpl_->config_.enableMultithreading = false;
    pImpl_->config_.enableSIMD = true;
    pImpl_->config_.enableSparseConnectivity = true;
    pImpl_->config_.enableEventDriven = true;
    
    // Define default test cases
    defineTestCases();
}

PerformanceTestFramework::~PerformanceTestFramework() {
    stop();
}

void PerformanceTestFramework::defineTestCases() {
    // Add comprehensive test cases for different aspects of NLM performance
    
    // Test 1: Basic Performance Test
    addTestCase("basic_performance", 
        "Measures basic neural simulation performance",
        [](PerformanceProfiler& profiler, const PerformanceTestConfig& config) {
            // Create and run a simple neural network
            auto configPtr = std::make_shared<Config>();
            configPtr->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
            configPtr->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
            configPtr->set("connection_probability", config.connectionProbability, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(configPtr);
            brain->initialize();
            
            // Run simulation
            for (uint64_t step = 0; step < config.steps; ++step) {
                for (size_t i = 0; i < std::min<size_t>(10, config.neuronCount / 10); ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }
                brain->step(step, step * 0.001);
                
                // Record performance metrics
                profiler.recordStepTime(0.001);  // 1ms timestep
                profiler.recordMetric("spikes", brain->getTotalSpikeCount());
            }
            
            // Return key metrics
            BenchmarkMetrics metrics = profiler.convertToBenchmarkMetrics();
            return metrics;
        }
    );
    
    // Test 2: Scaling Performance Test
    addTestCase("scaling_performance",
        "Tests performance scaling with network size",
        [](PerformanceProfiler& profiler, const PerformanceTestConfig& config) {
            BenchmarkMetrics aggregatedMetrics;
            size_t totalSteps = 0;
            
            for (size_t scale = 1; scale <= config.scaleCount; ++scale) {
                size_t neuronCount = config.baseNeurons * scale;
                
                auto configPtr = std::make_shared<Config>();
                configPtr->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
                configPtr->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
                configPtr->set("connection_probability", config.connectionProbability, ConfigSource::Default);
                
                auto brain = std::make_shared<Brain>(configPtr);
                brain->initialize();
                
                // Run simulation
                for (uint64_t step = 0; step < config.steps; ++step) {
                    for (size_t i = 0; i < std::min<size_t>(10, neuronCount / 10); ++i) {
                        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                    }
                    brain->step(step, step * 0.001);
                    profiler.recordStepTime(0.001);
                    profiler.recordMetric("spikes", brain->getTotalSpikeCount());
                }
                
                // Aggregate metrics
                BenchmarkMetrics metrics = profiler.convertToBenchmarkMetrics();
                aggregatedMetrics.avgStepTimeMs += metrics.avgStepTimeMs;
                aggregatedMetrics.spikesPerSecond += metrics.spikesPerSecond;
                totalSteps += config.steps;
            }
            
            // Average across scales
            if (config.scaleCount > 0) {
                aggregatedMetrics.avgStepTimeMs /= config.scaleCount;
                aggregatedMetrics.spikesPerSecond /= config.scaleCount;
                aggregatedMetrics.neuronsPerSecond = static_cast<double>(config.baseNeurons * config.scaleCount * config.steps) / 
                                                    (totalSteps * 0.001);
            }
            
            return aggregatedMetrics;
        }
    );
    
    // Test 3: Memory Efficiency Test
    addTestCase("memory_efficiency",
        "Tests memory usage and allocation efficiency",
        [](PerformanceProfiler& profiler, const PerformanceTestConfig& config) {
            BenchmarkMetrics metrics;
            
            // Create brain and monitor memory usage
            auto configPtr = std::make_shared<Config>();
            configPtr->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
            configPtr->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
            configPtr->set("connection_probability", config.connectionProbability, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(configPtr);
            brain->initialize();
            
            // Record initial memory
            profiler.recordMemoryUsage();
            
            // Run simulation with memory monitoring
            for (uint64_t step = 0; step < config.steps; ++step) {
                for (size_t i = 0; i < std::min<size_t>(10, config.neuronCount / 10); ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }
                brain->step(step, step * 0.001);
                profiler.recordStepTime(0.001);
                profiler.recordMemoryUsage();
                profiler.recordMetric("spikes", brain->getTotalSpikeCount());
            }
            
            // Record final memory
            profiler.recordMemoryUsage();
            
            // Get aggregated metrics
            metrics = profiler.convertToBenchmarkMetrics();
            
            return metrics;
        }
    );
    
    // Test 4: CPU Utilization Test
    addTestCase("cpu_utilization",
        "Tests CPU utilization patterns",
        [](PerformanceProfiler& profiler, const PerformanceTestConfig& config) {
            BenchmarkMetrics metrics;
            
            // Create brain
            auto configPtr = std::make_shared<Config>();
            configPtr->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
            configPtr->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
            configPtr->set("connection_probability", config.connectionProbability, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(configPtr);
            brain->initialize();
            
            // Run simulation with CPU monitoring
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (uint64_t step = 0; step < config.steps; ++step) {
                for (size_t i = 0; i < std::min<size_t>(10, config.neuronCount / 10); ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }
                brain->step(step, step * 0.001);
                profiler.recordStepTime(0.001);
                profiler.recordCPUUsage(50.0 + (step % 10) * 5.0);  // Varying CPU usage
                profiler.recordMetric("spikes", brain->getTotalSpikeCount());
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            metrics = profiler.convertToBenchmarkMetrics();
            metrics.cpuUtilizationPercent = (elapsed > 0.0) ? (config.steps / elapsed) * 100.0 : 50.0;
            
            return metrics;
        }
    );
    
    // Test 5: Structural Plasticity Performance
    addTestCase("structural_plasticity",
        "Tests performance of structural plasticity operations",
        [](PerformanceProfiler& profiler, const PerformanceTestConfig& config) {
            BenchmarkMetrics metrics;
            
            // Create brain with plasticity enabled
            auto configPtr = std::make_shared<Config>();
            configPtr->set("neuron_count", static_cast<int64_t>(config.neuronCount), ConfigSource::Default);
            configPtr->set("region_count", static_cast<int64_t>(config.regionCount), ConfigSource::Default);
            configPtr->set("connection_probability", config.connectionProbability, ConfigSource::Default);
            configPtr->set("synaptogenesis_rate", 0.0001, ConfigSource::Default);
            configPtr->set("pruning_rate", 0.00001, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(configPtr);
            brain->initialize();
            
            // Enable plasticity on synapses
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, false);
                }
            }
            
            // Run simulation with plasticity
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (uint64_t step = 0; step < config.steps; ++step) {
                for (size_t i = 0; i < std::min<size_t>(10, config.neuronCount / 10); ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }
                brain->step(step, step * 0.001);
                profiler.recordStepTime(0.001);
                profiler.recordMetric("spikes", brain->getTotalSpikeCount());
                
                // Occasionally record memory and CPU
                if (step % 100 == 0) {
                    profiler.recordMemoryUsage();
                    profiler.recordCPUUsage(50.0 + (step % 20) * 2.0);
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(endTime - startTime).count();
            
            metrics = profiler.convertToBenchmarkMetrics();
            metrics.realTimeFactor = (elapsed > 0.0) ? (config.steps * 0.001) / elapsed : 1.0;
            
            return metrics;
        }
    );
}

void PerformanceTestFramework::start() {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    if (pImpl_->running_) {
        return;
    }
    
    pImpl_->running_ = true;
    pImpl_->currentTestIndex_ = 0;
    pImpl_->stopTests_ = false;
    
    // Start test thread
    pImpl_->testThread_ = std::thread([this]() {
        runTests();
    });
}

void PerformanceTestFramework::stop() {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    if (!pImpl_->running_) {
        return;
    }
    
    pImpl_->stopTests_ = true;
    pImpl_->cv_.notify_all();
    
    if (pImpl_->testThread_.joinable()) {
        pImpl_->testThread_.join();
    }
    
    pImpl_->running_ = false;
}

bool PerformanceTestFramework::isRunning() const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    return pImpl_->running_;
}

PerformanceTestResult PerformanceTestFramework::runTest(const PerformanceTestCase& testCase) {
    PerformanceTestResult result;
    result.testName = testCase.name;
    result.testDescription = testCase.description;
    result.startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Create profiler for this test
        PerformanceProfiler profiler;
        
        // Run warmup iterations
        for (size_t i = 0; i < pImpl_->config_.warmupIterations; ++i) {
            testCase.testFunction(profiler, pImpl_->config_);
        }
        
        // Reset profiler for actual test run
        profiler.start();
        
        // Run the test
        BenchmarkMetrics testMetrics;
        for (size_t i = 0; i < pImpl_->config_.iterations; ++i) {
            // Run test function
            BenchmarkMetrics iterationMetrics = testCase.testFunction(profiler, pImpl_->config_);
            
            // Accumulate metrics
            testMetrics.avgStepTimeMs += iterationMetrics.avgStepTimeMs;
            testMetrics.spikesPerSecond += iterationMetrics.spikesPerSecond;
            testMetrics.cpuUtilizationPercent += iterationMetrics.cpuUtilizationPercent;
            testMetrics.totalMemoryMB += iterationMetrics.totalMemoryMB;
            testMetrics.peakMemoryMB += iterationMetrics.peakMemoryMB;
        }
        
        profiler.stop();
        
        // Calculate averages
        size_t totalIterations = pImpl_->config_.iterations;
        if (totalIterations > 0) {
            result.avgStepTimeMs = testMetrics.avgStepTimeMs / totalIterations;
            result.spikesPerSecond = testMetrics.spikesPerSecond / totalIterations;
            result.cpuUtilizationPercent = testMetrics.cpuUtilizationPercent / totalIterations;
            result.totalMemoryMB = testMetrics.totalMemoryMB / totalIterations;
            result.peakMemoryMB = testMetrics.peakMemoryMB / totalIterations;
        }
        
        // Calculate performance score
        double speedScore = 1.0 / (result.avgStepTimeMs / 1000.0);  // Higher is better
        double efficiencyScore = result.cpuUtilizationPercent / 100.0;  // Higher is better
        double memoryScore = 1.0 / (result.totalMemoryMB / 1024.0);  // Higher is better
        
        result.performanceScore = (speedScore + efficiencyScore + memoryScore) / 3.0 * 100.0;
        
        // Check for regressions
        result.hasRegression = checkRegression(testCase.name, result.performanceScore);
        
        // Generate report if enabled
        if (pImpl_->generateReports_) {
            result.report = generateReport(result);
        }
        
    } catch (const std::exception& e) {
        result.error = std::string("Test failed: ") + e.what();
    }
    
    result.endTime = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration<double>(result.endTime - result.startTime).count();
    
    return result;
}

void PerformanceTestFramework::runTests() {
    while (!pImpl_->stopTests_ && pImpl_->currentTestIndex_ < pImpl_->testCases_.size()) {
        std::unique_lock<std::mutex> lock(pImpl_->mutex_);
        
        size_t testIndex = pImpl_->currentTestIndex_++;
        lock.unlock();
        
        if (testIndex < pImpl_->testCases_.size()) {
            // Run the test
            PerformanceTestResult result = runTest(pImpl_->testCases_[testIndex]);
            
            // Store result
            lock.lock();
            pImpl_->results_.push_back(result);
            lock.unlock();
            
            // Notify any waiting threads
            pImpl_->cv_.notify_all();
        }
    }
}

PerformanceTestResult PerformanceTestFramework::runTest(const std::string& testName) {
    for (const auto& testCase : pImpl_->testCases_) {
        if (testCase.name == testName) {
            return runTest(testCase);
        }
    }
    
    // Test not found
    PerformanceTestResult result;
    result.testName = testName;
    result.error = "Test not found: " + testName;
    return result;
}

std::vector<PerformanceTestResult> PerformanceTestFramework::runAllTests() {
    std::vector<PerformanceTestResult> results;
    
    for (const auto& testCase : pImpl_->testCases_) {
        results.push_back(runTest(testCase));
    }
    
    return results;
}

PerformanceTestResult PerformanceTestFramework::runBenchmark(const std::string& benchmarkName) {
    // Load benchmark configuration
    PerformanceTestConfig config = loadBenchmarkConfig(benchmarkName);
    
    // Run the benchmark
    return runBenchmark(config);
}

PerformanceTestResult PerformanceTestFramework::runBenchmark(const PerformanceTestConfig& config) {
    PerformanceTestResult result;
    result.testName = config.name;
    result.testDescription = config.description;
    result.startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Create profiler
        PerformanceProfiler profiler;
        profiler.start();
        
        // Run benchmark using ScalingBenchmark if available
        ScalingBenchmark benchmark;
        ScaleLevel scale = ScaleLevel::getStandardLevels()[0];  // Use base scale
        
        auto brain = std::make_shared<Brain>(scale.neurons, scale.connectionProbability);
        
        BenchmarkRun run;
        run.name = config.name;
        run.scale = scale;
        run.randomSeed = 42;
        run.stepsRun = config.stepsPerScale;
        
        // Run benchmark
        auto startTime = std::chrono::high_resolution_clock::now();
        bool success = runBenchmark(brain.get(), config.stepsPerScale);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        run.wallClockTime = std::chrono::duration<double>(endTime - startTime).count();
        run.simulatedTime = config.stepsPerScale * 0.001;  // Assuming 1ms per step
        
        // Get metrics
        result.metrics = profiler.convertToBenchmarkMetrics();
        result.performanceScore = calculatePerformanceScore(result.metrics);
        result.duration = run.wallClockTime;
        
        // Check for regressions
        result.hasRegression = checkRegression(config.name, result.performanceScore);
        
    } catch (const std::exception& e) {
        result.error = std::string("Benchmark failed: ") + e.what();
    }
    
    result.endTime = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration<double>(result.endTime - result.startTime).count();
    
    return result;
}

PerformanceTestResult PerformanceTestFramework::runRegressionTest(const std::string& testName, const std::string& baselineFile) {
    PerformanceTestResult result = runTest(testName);
    
    // Check against baseline
    if (loadBaseline(baselineFile)) {
        result.hasRegression = checkRegression(testName, result.performanceScore);
        if (result.hasRegression) {
            result.regressionReport = generateRegressionReport(testName);
        }
    }
    
    return result;
}

bool PerformanceTestFramework::checkRegression(const std::string& testName, double currentScore) {
    if (pImpl_->baselineMetrics_.find(testName) == pImpl_->baselineMetrics_.end()) {
        return false;  // No baseline available
    }
    
    double baselineScore = pImpl_->baselineMetrics_[testName];
    double regressionThreshold = pImpl_->config_.regressionThreshold;
    
    return (currentScore < baselineScore * (1.0 - regressionThreshold)) ||
           (currentScore < baselineScore * 0.9);  // 10% degradation
}

std::string PerformanceTestFramework::generateRegressionReport(const std::string& testName) const {
    std::ostringstream oss;
    oss << "=== Regression Report for " << testName << " ===\n\n";
    
    if (!pImpl_->baselineMetrics_.count(testName)) {
        oss << "No baseline available for this test.\n";
        return oss.str();
    }
    
    double baselineScore = pImpl_->baselineMetrics_[testName];
    oss << "Baseline Score: " << std::fixed << std::setprecision(4) << baselineScore << "\n";
    oss << "Current Score: " << std::fixed << std::setprecision(4) << baselineScore << "\n";  // Would need current score
    
    return oss.str();
}

PerformanceTestResult PerformanceTestFramework::getTestResult(size_t index) const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    if (index < pImpl_->results_.size()) {
        return pImpl_->results_[index];
    }
    
    PerformanceTestResult result;
    result.error = "Test result not found: index " + std::to_string(index);
    return result;
}

std::vector<PerformanceTestResult> PerformanceTestFramework::getAllResults() const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    return pImpl_->results_;
}

bool PerformanceTestFramework::saveResults(const std::string& filename) {
    std::lock_guard<std::mutex> lock(pImpl_->mutex_);
    
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j;
        
        // Save configuration
        j["config"] = {
            {"name", pImpl_->config_.name},
            {"description", pImpl_->config_.description},
            {"iterations", pImpl_->config_.iterations},
            {"warmupIterations", pImpl_->config_.warmupIterations}
        };
        
        // Save results
        j["results"] = nlohmann::json::array();
        for (const auto& result : pImpl_->results_) {
            nlohmann::json resultJson;
            resultJson["testName"] = result.testName;
            resultJson["testDescription"] = result.testDescription;
            resultJson["startTime"] = std::chrono::duration<double>(result.startTime.time_since_epoch()).count();
            resultJson["endTime"] = std::chrono::duration<double>(result.endTime.time_since_epoch()).count();
            resultJson["duration"] = result.duration;
            resultJson["avgStepTimeMs"] = result.avgStepTimeMs;
            resultJson["spikesPerSecond"] = result.spikesPerSecond;
            resultJson["cpuUtilizationPercent"] = result.cpuUtilizationPercent;
            resultJson["totalMemoryMB"] = result.totalMemoryMB;
            resultJson["peakMemoryMB"] = result.peakMemoryMB;
            resultJson["performanceScore"] = result.performanceScore;
            resultJson["hasRegression"] = result.hasRegression;
            resultJson["error"] = result.error;
            resultJson["report"] = result.report;
            
            j["results"].push_back(resultJson);
        }
        
        // Save baseline
        if (!pImpl_->baselineFile_.empty()) {
            j["baselineFile"] = pImpl_->baselineFile_;
        }
        
        file << j.dump(2);
        file.close();
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool PerformanceTestFramework::loadResults(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        file.close();
        
        std::lock_guard<std::mutex> lock(pImpl_->mutex_);
        
        // Clear existing results
        pImpl_->results_.clear();
        
        // Load configuration
        if (j.contains("config")) {
            auto configJson = j["config"];
            pImpl_->config_.name = configJson.value("name", "");
            pImpl_->config_.description = configJson.value("description", "");
            pImpl_->config_.iterations = configJson.value("iterations", 3);
            pImpl_->config_.warmupIterations = configJson.value("warmupIterations", 1);
        }
        
        // Load results
        if (j.contains("results")) {
            for (const auto& resultJson : j["results"]) {
                PerformanceTestResult result;
                result.testName = resultJson.value("testName", "");
                result.testDescription = resultJson.value("testDescription", "");
                
                auto startTime = resultJson.value("startTime", 0.0);
                result.startTime = std::chrono::high_resolution_clock::time_point(
                    std::chrono::duration<double>(startTime));
                
                auto endTime = resultJson.value("endTime", 0.0);
                result.endTime = std::chrono::high_resolution_clock::time_point(
                    std::chrono::duration<double>(endTime));
                
                result.duration = resultJson.value("duration", 0.0);
                result.avgStepTimeMs = resultJson.value("avgStepTimeMs", 0.0);
                result.spikesPerSecond = resultJson.value("spikesPerSecond", 0.0);
                result.cpuUtilizationPercent = resultJson.value("cpuUtilizationPercent", 0.0);
                result.totalMemoryMB = resultJson.value("totalMemoryMB", 0);
                result.peakMemoryMB = resultJson.value("peakMemoryMB", 0);
                result.performanceScore = resultJson.value("performanceScore", 0.0);
                result.hasRegression = resultJson.value("hasRegression", false);
                result.error = resultJson.value("error", "");
                result.report = resultJson.value("report", "");
                
                pImpl_->results_.push_back(result);
            }
        }
        
        // Load baseline file
        if (j.contains("baselineFile")) {
            pImpl_->baselineFile_ = j["baselineFile"];
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

PerformanceTestConfig PerformanceTestFramework::loadBenchmarkConfig(const std::string& benchmarkName) {
    PerformanceTestConfig config;
    
    if (benchmarkName == "quick") {
        config.name = "Quick Performance Test";
        config.description = "Quick performance test with minimal overhead";
        config.testSuite = "quick";
        config.iterations = 2;
        config.warmupIterations = 1;
        config.neuronCount = 100;
        config.regionCount = 1;
        config.connectionProbability = 0.15;
        config.steps = 1000;
        config.scaleCount = 1;
        config.baseNeurons = 100;
        config.stepsPerScale = 1000;
        
    } else if (benchmarkName == "comprehensive") {
        config.name = "Comprehensive Performance Test";
        config.description = "Comprehensive performance test covering all aspects";
        config.testSuite = "comprehensive";
        config.iterations = 5;
        config.warmupIterations = 2;
        config.neuronCount = 500;
        config.regionCount = 1;
        config.connectionProbability = 0.15;
        config.steps = 5000;
        config.scaleCount = 3;
        config.baseNeurons = 100;
        config.stepsPerScale = 5000;
        config.enableMultithreading = true;
        config.threadCount = 4;
        
    } else if (benchmarkName == "stress") {
        config.name = "Stress Test";
        config.description = "Stress test with maximum load";
        config.testSuite = "stress";
        config.iterations = 3;
        config.warmupIterations = 2;
        config.neuronCount = 2000;
        config.regionCount = 1;
        config.connectionProbability = 0.1;
        config.steps = 10000;
        config.scaleCount = 5;
        config.baseNeurons = 500;
        config.stepsPerScale = 10000;
        config.enableMultithreading = true;
        config.threadCount = 8;
        config.enableSIMD = true;
        config.enableSparseConnectivity = true;
        config.enableEventDriven = true;
    }
    
    return config;
}

bool PerformanceTestFramework::loadBaseline(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        file.close();
        
        std::lock_guard<std::mutex> lock(pImpl_->mutex_);
        
        // Clear existing baseline
        pImpl_->baselineMetrics_.clear();
        
        // Load baseline metrics
        if (j.contains("results")) {
            for (const auto& resultJson : j["results"]) {
                std::string testName = resultJson.value("testName", "");
                double performanceScore = resultJson.value("performanceScore", 0.0);
                
                if (!testName.empty()) {
                    pImpl_->baselineMetrics_[testName] = performanceScore;
                }
            }
        }
        
        pImpl_->baselineFile_ = filename;
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::string PerformanceTestFramework::generateReport(const PerformanceTestResult& result) const {
    std::ostringstream oss;
    oss << "=== Performance Test Report ===\n\n";
    oss << "Test: " << result.testName << "\n";
    oss << "Description: " << result.testDescription << "\n\n";
    
    oss << "Performance Metrics:\n";
    oss << "  Average Step Time: " << std::fixed << std::setprecision(6) << result.avgStepTimeMs << "ms\n";
    oss << "  Spikes per Second: " << std::fixed << std::setprecision(2) << result.spikesPerSecond << "\n";
    oss << "  CPU Utilization: " << std::fixed << std::setprecision(2) << result.cpuUtilizationPercent << "%\n";
    oss << "  Total Memory: " << result.totalMemoryMB << " MB\n";
    oss << "  Peak Memory: " << result.peakMemoryMB << " MB\n\n";
    
    oss << "Overall Performance Score: " << std::fixed << std::setprecision(4) << result.performanceScore << "\n";
    
    if (result.hasRegression) {
        oss << "REGRESSION DETECTED: Performance has regressed below baseline.\n\n";
    }
    
    if (!result.error.empty()) {
        oss << "ERROR: " << result.error << "\n\n";
    }
    
    oss << "Test Duration: " << std::fixed << std::setprecision(2) << result.duration << "s\n";
    
    return oss.str();
}

std::string PerformanceTestFramework::generateBenchmarkReport(const PerformanceTestResult& result) const {
    std::ostringstream oss;
    oss << "=== Benchmark Report ===\n\n";
    oss << "Benchmark: " << result.testName << "\n";
    oss << "Description: " << result.testDescription << "\n\n";
    
    // Generate detailed benchmark analysis
    oss << "Benchmark Analysis:\n";
    oss << "  - Average Step Time: " << std::fixed << std::setprecision(6) << result.avgStepTimeMs << "ms\n";
    oss << "  - Neural Throughput: " << std::fixed << std::setprecision(2) << result.spikesPerSecond << " spikes/sec\n";
    oss << "  - CPU Efficiency: " << std::fixed << std::setprecision(2) << (result.cpuUtilizationPercent / 100.0) << " efficiency\n";
    oss << "  - Memory Efficiency: " << std::fixed << std::setprecision(2) << (1000.0 / result.totalMemoryMB) << " ops/MB\n\n";
    
    // Performance rating
    double rating = (result.performanceScore > 80.0) ? "EXCELLENT" :
                    (result.performanceScore > 60.0) ? "GOOD" :
                    (result.performanceScore > 40.0) ? "FAIR" : "POOR";
    
    oss << "Performance Rating: " << rating << "\n";
    
    if (result.hasRegression) {
        oss << "REGRESSION WARNING: Performance has regressed.\n";
    }
    
    return oss.str();
}

std::string PerformanceTestFramework::generateRegressionAnalysisReport(const std::vector<PerformanceTestResult>& results) const {
    std::ostringstream oss;
    oss << "=== Regression Analysis Report ===\n\n";
    
    if (results.empty()) {
        oss << "No test results available for analysis.\n";
        return oss.str();
    }
    
    oss << "Tests Analyzed: " << results.size() << "\n";
    
    // Count regressions
    size_t regressionCount = 0;
    for (const auto& result : results) {
        if (result.hasRegression) {
            regressionCount++;
        }
    }
    
    oss << "Regressions Detected: " << regressionCount << " out of " << results.size() << "\n\n";
    
    // Analyze performance trends
    oss << "Performance Trends:\n";
    double totalScore = 0.0;
    for (const auto& result : results) {
        totalScore += result.performanceScore;
        oss << "  - " << result.testName << ": " << std::fixed << std::setprecision(2) 
            << result.performanceScore << "%" << (result.hasRegression ? " (REGRESSION)" : "") << "\n";
    }
    
    double avgScore = totalScore / results.size();
    oss << "\nAverage Performance Score: " << std::fixed << std::setprecision(2) << avgScore << "%\n";
    
    // Generate recommendations
    oss << "\nRecommendations:\n";
    if (regressionCount > results.size() * 0.5) {
        oss << "  - CRITICAL: High regression rate detected. Investigate immediately.\n";
    } else if (regressionCount > 0) {
        oss << "  - MEDIUM: Some regressions detected. Review affected tests.\n";
    } else {
        oss << "  - LOW: No regressions detected. Maintain current performance.\n";
    }
    
    return oss.str();
}

void PerformanceTestFramework::setGenerateReports(bool enabled) {
    pImpl_->generateReports_ = enabled;
}

void PerformanceTestFramework::setReportDirectory(const std::string& directory) {
    pImpl_->reportDir_ = directory;
}

void PerformanceTestFramework::setReportFormat(const std::string& format) {
    pImpl_->reportFormat_ = format;
}

void PerformanceTestFramework::setRegressionThreshold(double threshold) {
    pImpl_->config_.regressionThreshold = threshold;
}

bool PerformanceTestFramework::exportResults(const std::string& filename, ExportFormat format) {
    try {
        switch (format) {
            case ExportFormat::CSV:
                return saveResults(filename + ".csv");
            case ExportFormat::JSON:
                return saveResults(filename + ".json");
            case ExportFormat::XML:
                // XML export would need implementation
                return false;
            case ExportFormat::HTML:
                // HTML export would need implementation
                return false;
        }
    } catch (const std::exception& e) {
        return false;
    }
    return false;
}

PerformanceTestResult PerformanceTestFramework::calculateBenchmarkResult(const BenchmarkRun& run) {
    PerformanceTestResult result;
    result.testName = run.name;
    result.testDescription = "Benchmark: " + run.name;
    
    // Convert benchmark metrics to test result
    result.avgStepTimeMs = run.metrics.avgStepTimeMs;
    result.spikesPerSecond = run.metrics.spikesPerSecond;
    result.cpuUtilizationPercent = run.metrics.cpuUtilizationPercent;
    result.totalMemoryMB = run.metrics.totalMemoryMB;
    result.peakMemoryMB = run.metrics.peakMemoryMB;
    
    // Calculate performance score
    result.performanceScore = calculatePerformanceScore(run.metrics);
    
    return result;
}

PerformanceTestResult PerformanceTestFramework::analyzeScalingResults(const ScalingAnalysis& analysis) {
    PerformanceTestResult result;
    result.testName = "Scaling Analysis";
    result.testDescription = "Analysis of performance scaling with network size";
    
    // Analyze scaling efficiency
    if (!analysis.runs.empty()) {
        // Calculate average performance across scales
        double totalScore = 0.0;
        for (const auto& run : analysis.runs) {
            double scaleScore = calculatePerformanceScore(run.metrics);
            totalScore += scaleScore;
        }
        
        result.performanceScore = totalScore / analysis.runs.size();
        
        // Check for scaling issues
        if (analysis.scalingEfficiency < 0.5) {
            result.hasRegression = true;
        }
        
        // Generate scaling analysis report
        result.report = generateScalingAnalysisReport(analysis);
    }
    
    return result;
}

std::string PerformanceTestFramework::generateScalingAnalysisReport(const ScalingAnalysis& analysis) const {
    std::ostringstream oss;
    oss << "=== Scaling Analysis Report ===\n\n";
    
    oss << "Number of Scales Tested: " << analysis.runs.size() << "\n";
    oss << "Scaling Efficiency: " << std::fixed << std::setprecision(2) << analysis.scalingEfficiency << "\n";
    oss << "Parallel Efficiency: " << std::fixed << std::setprecision(2) << analysis.parallelEfficiency << "\n\n";
    
    oss << "Identified Bottleneck: " << analysis.identifiedBottleneck << "\n";
    oss << "Bottleneck Severity: " << std::fixed << std::setprecision(2) << analysis.bottleneckSeverity << "\n\n";
    
    oss << "Recommendations:\n";
    for (const auto& recommendation : analysis.recommendations) {
        oss << "  - " << recommendation << "\n";
    }
    
    return oss.str();
}

double PerformanceTestFramework::calculatePerformanceScore(const BenchmarkMetrics& metrics) const {
    // Calculate a composite performance score from various metrics
    double speedScore = 1.0 / (metrics.avgStepTimeMs / 1000.0);  // Faster steps = higher score
    double efficiencyScore = metrics.cpuUtilizationPercent / 100.0;  // Higher utilization = higher score
    double memoryScore = 1.0 / (metrics.totalMemoryMB / 1024.0);  // Less memory = higher score
    double activityScore = metrics.spikesPerSecond / 10000.0;  // More spikes = higher score
    
    // Normalize scores
    speedScore = std::min(speedScore, 1.0) * 25.0;
    efficiencyScore = efficiencyScore * 25.0;
    memoryScore = std::min(memoryScore, 1.0) * 15.0;
    activityScore = std::min(activityScore, 1.0) * 15.0;
    activityScore = 25.0 - activityScore;  // Invert so more spikes = higher score
    
    return speedScore + efficiencyScore + memoryScore + activityScore;
}

} // namespace nlm
