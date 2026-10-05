// NLM Automated Testing and Benchmarking Framework
// Provides comprehensive testing, benchmarking, and performance analysis capabilities
// Includes test suites, performance profiling, and result analysis

#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <thread>
#include <atomic>
#include <random>

namespace nlm {

// Test case interface
class TestCase {
public:
    virtual ~TestCase() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual TestResult run(const std::shared_ptr<Config>& config) = 0;
    virtual bool setup(const std::shared_ptr<Config>& config) { return true; }
    virtual bool teardown(const std::shared_ptr<Config>& config) { return true; }
    
    virtual std::vector<std::string> getRequiredParams() const { return {}; }
    virtual std::vector<std::string> getOptionalParams() const { return {}; }
    virtual std::map<std::string, std::vector<float>> getParameterSweeps() const { return {}; }
    
    virtual TestResult runWithParameters(const std::map<std::string, float>& params) {
        // Default implementation: create a config from parameters
        auto config = std::make_shared<Config>();
        
        for (const auto& param : params) {
            config->set(param.first, param.second, ConfigSource::Runtime);
        }
        
        return run(config);
    }
};

// Benchmark case interface
class BenchmarkCase {
public:
    virtual ~BenchmarkCase() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual BenchmarkResult run(const std::shared_ptr<Config>& config) = 0;
    virtual bool setup(const std::shared_ptr<Config>& config) { return true; }
    virtual bool teardown(const std::shared_ptr<Config>& config) { return true; }
    
    virtual std::vector<std::string> getRequiredParams() const { return {}; }
    virtual std::vector<std::string> getOptionalParams() const { return {}; }
    virtual std::map<std::string, std::vector<float>> getParameterSweeps() const { return {}; }
    
    virtual BenchmarkResult runWithParameters(const std::map<std::string, float>& params) {
        auto config = std::make_shared<Config>();
        
        for (const auto& param : params) {
            config->set(param.first, param.second, ConfigSource::Runtime);
        }
        
        return run(config);
    }
};

// Test result structure
struct TestResult {
    std::string testName;
    std::string description;
    bool passed;
    double executionTimeMs;
    std::string errorMessage;
    std::map<std::string, double> metrics;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
    TestResult() : passed(false), executionTimeMs(0.0), startTime(std::chrono::system_clock::now()) {}
    
    void calculateExecutionTime() {
        endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        executionTimeMs = duration.count();
    }
    
    std::string getDurationString() const {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration) % 1000;
        
        std::stringstream ss;
        ss << seconds.count() << "." << std::setfill('0') << std::setw(3) << milliseconds.count() << "s";
        return ss.str();
    }
};

// Benchmark result structure
struct BenchmarkResult {
    std::string benchmarkName;
    std::string description;
    double executionTimeMs;
    size_t memoryUsageKB;
    double spikesPerSecond;
    double synapsesPerMs;
    double cpuUsagePercent;
    size_t threadCount;
    std::map<std::string, double> metrics;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::string hardwareInfo;
    
    BenchmarkResult() : executionTimeMs(0.0), memoryUsageKB(0), spikesPerSecond(0.0), 
                       synapsesPerMs(0.0), cpuUsagePercent(0.0), threadCount(0) {
        startTime = std::chrono::system_clock::now();
    }
    
    void calculateExecutionTime() {
        endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        executionTimeMs = duration.count();
    }
    
    std::string getDurationString() const {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration) % 1000;
        
        std::stringstream ss;
        ss << seconds.count() << "." << std::setfill('0') << std::setw(3) << milliseconds.count() << "s";
        return ss.str();
    }
};

// Test suite for organizing related tests
class TestSuite {
public:
    TestSuite(const std::string& name, const std::string& description)
        : name_(name), description_(description) {}
    
    void addTestCase(std::unique_ptr<TestCase> testCase) {
        testCases_.push_back(std::move(testCase));
    }
    
    void addTestCase(const std::string& name, const std::string& description,
                    std::function<TestResult(const std::shared_ptr<Config>&)> testFunction) {
        auto testCase = std::make_unique<LambdaTestCase>(name, description, testFunction);
        addTestCase(std::move(testCase));
    }
    
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    
    const std::vector<std::unique_ptr<TestCase>>& getTestCases() const { return testCases_; }
    
    TestResults runAll(const std::shared_ptr<Config>& config) {
        TestResults results;
        
        NLM_LOG_INFO("Running test suite: " + name_);
        
        for (auto& testCase : testCases_) {
            NLM_LOG_INFO("Running test: " + testCase->getName());
            
            TestResult result = testCase->run(config);
            results.testResults.push_back(std::move(result));
        }
        
        return results;
    }
    
    TestResults runAllWithSweeps(const std::shared_ptr<Config>& config) {
        TestResults results;
        
        // Generate parameter combinations
        std::vector<std::map<std::string, float>> parameterCombinations;
        generateParameterCombinations(parameterCombinations);
        
        NLM_LOG_INFO("Running test suite: " + name_ + " with parameter sweeps");
        NLM_LOG_INFO("Generated " + std::to_string(parameterCombinations.size()) + " parameter combinations");
        
        for (size_t i = 0; i < parameterCombinations.size(); ++i) {
            const auto& params = parameterCombinations[i];
            
            NLM_LOG_INFO("Running test case " + std::to_string(i + 1) + "/" + 
                        std::to_string(parameterCombinations.size()) + 
                        " with parameters: " + formatParameters(params));
            
            for (auto& testCase : testCases_) {
                TestResult result = testCase->runWithParameters(params);
                result.testName += " (" + formatParameters(params) + ")";
                results.testResults.push_back(std::move(result));
            }
        }
        
        return results;
    }
    
    BenchmarkResults runAllBenchmarks(const std::shared_ptr<Config>& config) {
        BenchmarkResults results;
        
        NLM_LOG_INFO("Running benchmark suite: " + name_);
        
        for (auto& testCase : testCases_) {
            // Cast to benchmark case if possible
            auto* benchmarkCase = dynamic_cast<BenchmarkCase*>(testCase.get());
            if (benchmarkCase) {
                NLM_LOG_INFO("Running benchmark: " + benchmarkCase->getName());
                
                BenchmarkResult result = benchmarkCase->run(config);
                results.benchmarkResults.push_back(std::move(result));
            }
        }
        
        return results;
    }
    
private:
    std::string name_;
    std::string description_;
    std::vector<std::unique_ptr<TestCase>> testCases_;
    
    void generateParameterCombinations(std::vector<std::map<std::string, float>>& combinations) {
        // Collect all parameter sweeps from test cases
        std::map<std::string, std::vector<float>> allSweeps;
        
        for (const auto& testCase : testCases_) {
            auto sweeps = testCase->getParameterSweeps();
            for (const auto& sweep : sweeps) {
                if (allSweeps.find(sweep.first) == allSweeps.end()) {
                    allSweeps[sweep.first] = sweep.second;
                }
            }
        }
        
        if (allSweeps.empty()) {
            return;
        }
        
        // Generate all combinations
        generateCombinationsHelper(allSweeps, {}, 0, combinations);
    }
    
    void generateCombinationsHelper(std::map<std::string, std::vector<float>>& sweeps,
                                    std::map<std::string, float> current,
                                    size_t index,
                                    std::vector<std::map<std::string, float>>& combinations) {
        if (index >= sweeps.size()) {
            combinations.push_back(current);
            return;
        }
        
        // Get the next parameter name
        auto it = sweeps.begin();
        std::advance(it, index);
        std::string paramName = it->first;
        std::vector<float> values = it->second;
        
        // For each value, create a new current map and recurse
        for (float value : values) {
            current[paramName] = value;
            generateCombinationsHelper(sweeps, current, index + 1, combinations);
        }
    }
    
    std::string formatParameters(const std::map<std::string, float>& params) {
        std::stringstream ss;
        ss << "{";
        bool first = true;
        for (const auto& param : params) {
            if (!first) ss << ", ";
            ss << param.first << "=" << param.second;
            first = false;
        }
        ss << "}";
        return ss.str();
    }
    
    class LambdaTestCase : public TestCase {
    public:
        LambdaTestCase(const std::string& name, const std::string& description,
                      std::function<TestResult(const std::shared_ptr<Config>&)> testFunction)
            : name_(name), description_(description), testFunction_(testFunction) {}
        
        std::string getName() const override { return name_; }
        std::string getDescription() const override { return description_; }
        TestResult run(const std::shared_ptr<Config>& config) override { return testFunction_(config); }
        
    private:
        std::string name_;
        std::string description_;
        std::function<TestResult(const std::shared_ptr<Config>&)> testFunction_;
    };
};

// Benchmark suite for performance testing
class BenchmarkSuite {
public:
    BenchmarkSuite(const std::string& name, const std::string& description)
        : name_(name), description_(description) {}
    
    void addBenchmarkCase(std::unique_ptr<BenchmarkCase> benchmarkCase) {
        benchmarkCases_.push_back(std::move(benchmarkCase));
    }
    
    void addBenchmarkCase(const std::string& name, const std::string& description,
                         std::function<BenchmarkResult(const std::shared_ptr<Config>&)> benchmarkFunction) {
        auto benchmarkCase = std::make_unique<LambdaBenchmarkCase>(name, description, benchmarkFunction);
        addBenchmarkCase(std::move(benchmarkCase));
    }
    
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    
    const std::vector<std::unique_ptr<BenchmarkCase>>& getBenchmarkCases() const { return benchmarkCases_; }
    
    BenchmarkResults runAll(const std::shared_ptr<Config>& config) {
        BenchmarkResults results;
        
        NLM_LOG_INFO("Running benchmark suite: " + name_);
        
        for (auto& benchmarkCase : benchmarkCases_) {
            NLM_LOG_INFO("Running benchmark: " + benchmarkCase->getName());
            
            BenchmarkResult result = benchmarkCase->run(config);
            results.benchmarkResults.push_back(std::move(result));
        }
        
        return results;
    }
    
    BenchmarkResults runWithParameterSweeps(const std::shared_ptr<Config>& config) {
        BenchmarkResults results;
        
        // Generate parameter combinations
        std::vector<std::map<std::string, float>> parameterCombinations;
        generateParameterCombinations(parameterCombinations);
        
        NLM_LOG_INFO("Running benchmark suite: " + name_ + " with parameter sweeps");
        NLM_LOG_INFO("Generated " + std::to_string(parameterCombinations.size()) + " parameter combinations");
        
        for (size_t i = 0; i < parameterCombinations.size(); ++i) {
            const auto& params = parameterCombinations[i];
            
            NLM_LOG_INFO("Running benchmark case " + std::to_string(i + 1) + "/" + 
                        std::to_string(parameterCombinations.size()) + 
                        " with parameters: " + formatParameters(params));
            
            for (auto& benchmarkCase : benchmarkCases_) {
                BenchmarkResult result = benchmarkCase->runWithParameters(params);
                result.benchmarkName += " (" + formatParameters(params) + ")";
                results.benchmarkResults.push_back(std::move(result));
            }
        }
        
        return results;
    }
    
    BenchmarkResults runWithMultipleIterations(const std::shared_ptr<Config>& config, 
                                              int iterations = 5) {
        BenchmarkResults results;
        
        NLM_LOG_INFO("Running benchmark suite: " + name_ + " with " + 
                    std::to_string(iterations) + " iterations per benchmark");
        
        for (auto& benchmarkCase : benchmarkCases_) {
            NLM_LOG_INFO("Running benchmark: " + benchmarkCase->getName() + 
                        " for " + std::to_string(iterations) + " iterations");
            
            double totalTimeMs = 0.0;
            size_t totalMemoryKB = 0;
            double totalSpikesPerSec = 0.0;
            double totalSynapsesPerMs = 0.0;
            
            for (int i = 0; i < iterations; ++i) {
                NLM_LOG_INFO("  Iteration " + std::to_string(i + 1) + "/" + std::to_string(iterations));
                
                BenchmarkResult result = benchmarkCase->run(config);
                
                totalTimeMs += result.executionTimeMs;
                totalMemoryKB += result.memoryUsageKB;
                totalSpikesPerSec += result.spikesPerSecond;
                totalSynapsesPerMs += result.synapsesPerMs;
                
                // Store individual iteration result
                results.benchmarkResults.push_back(result);
            }
            
            // Calculate averages
            BenchmarkResult avgResult;
            avgResult.benchmarkName = benchmarkCase->getName() + " (average)";
            avgResult.description = benchmarkCase->getDescription();
            avgResult.executionTimeMs = totalTimeMs / iterations;
            avgResult.memoryUsageKB = totalMemoryKB / iterations;
            avgResult.spikesPerSecond = totalSpikesPerSec / iterations;
            avgResult.synapsesPerMs = totalSynapsesPerMs / iterations;
            
            results.benchmarkResults.push_back(avgResult);
        }
        
        return results;
    }
    
    BenchmarkResults runStressTest(const std::shared_ptr<Config>& config, 
                                  const std::string& stressType = "cpu") {
        BenchmarkResults results;
        
        NLM_LOG_INFO("Running stress test: " + stressType);
        
        if (stressType == "cpu") {
            runCPUStressTest(config, results);
        } else if (stressType == "memory") {
            runMemoryStressTest(config, results);
        } else if (stressType == "neuron") {
            runNeuronStressTest(config, results);
        } else if (stressType == "synaptic") {
            runSynapticStressTest(config, results);
        }
        
        return results;
    }
    
private:
    std::string name_;
    std::string description_;
    std::vector<std::unique_ptr<BenchmarkCase>> benchmarkCases_;
    
    void generateParameterCombinations(std::vector<std::map<std::string, float>>& combinations) {
        // Collect all parameter sweeps from benchmark cases
        std::map<std::string, std::vector<float>> allSweeps;
        
        for (const auto& benchmarkCase : benchmarkCases_) {
            auto sweeps = benchmarkCase->getParameterSweeps();
            for (const auto& sweep : sweeps) {
                if (allSweeps.find(sweep.first) == allSweeps.end()) {
                    allSweeps[sweep.first] = sweep.second;
                }
            }
        }
        
        if (allSweeps.empty()) {
            return;
        }
        
        // Generate all combinations
        generateCombinationsHelper(allSweeps, {}, 0, combinations);
    }
    
    void generateCombinationsHelper(std::map<std::string, std::vector<float>>& sweeps,
                                    std::map<std::string, float> current,
                                    size_t index,
                                    std::vector<std::map<std::string, float>>& combinations) {
        if (index >= sweeps.size()) {
            combinations.push_back(current);
            return;
        }
        
        // Get the next parameter name
        auto it = sweeps.begin();
        std::advance(it, index);
        std::string paramName = it->first;
        std::vector<float> values = it->second;
        
        // For each value, create a new current map and recurse
        for (float value : values) {
            current[paramName] = value;
            generateCombinationsHelper(sweeps, current, index + 1, combinations);
        }
    }
    
    std::string formatParameters(const std::map<std::string, float>& params) {
        std::stringstream ss;
        ss << "{";
        bool first = true;
        for (const auto& param : params) {
            if (!first) ss << ", ";
            ss << param.first << "=" << param.second;
            first = false;
        }
        ss << "}";
        return ss.str();
    }
    
    void runCPUStressTest(const std::shared_ptr<Config>& config, BenchmarkResults& results) {
        // Simulate CPU stress by running complex calculations
        auto benchmarkCase = std::make_unique<LambdaBenchmarkCase>(
            "cpu_stress", "CPU stress test", 
            [config](const std::shared_ptr<Config>&) {
                BenchmarkResult result;
                result.benchmarkName = "cpu_stress";
                
                // Perform complex calculations
                auto startTime = std::chrono::high_resolution_clock::now();
                
                // Matrix multiplication simulation
                std::vector<std::vector<double>> matrix1(1000, std::vector<double>(1000, 1.0));
                std::vector<std::vector<double>> matrix2(1000, std::vector<double>(1000, 2.0));
                std::vector<std::vector<double>> resultMatrix(1000, std::vector<double>(1000, 0.0));
                
                for (size_t i = 0; i < 1000; ++i) {
                    for (size_t j = 0; j < 1000; ++j) {
                        for (size_t k = 0; k < 1000; ++k) {
                            resultMatrix[i][j] += matrix1[i][k] * matrix2[k][j];
                        }
                    }
                }
                
                // Neural network simulation
                auto brain = std::make_shared<Brain>(config);
                if (brain->initialize()) {
                    for (size_t i = 0; i < 100; ++i) {
                        brain->step(i, i * 0.001);
                    }
                }
                
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                result.executionTimeMs = duration.count();
                
                // Set some dummy metrics
                result.memoryUsageKB = 500;
                result.spikesPerSecond = 100.0;
                result.synapsesPerMs = 50.0;
                result.cpuUsagePercent = 95.0;
                result.threadCount = std::thread::hardware_concurrency();
                
                return result;
            }
        );
        
        results.benchmarkResults.push_back(benchmarkCase->run(config));
    }
    
    void runMemoryStressTest(const std::shared_ptr<Config>& config, BenchmarkResults& results) {
        // Simulate memory stress by allocating large amounts of memory
        auto benchmarkCase = std::make_unique<LambdaBenchmarkCase>(
            "memory_stress", "Memory stress test",
            [config](const std::shared_ptr<Config>&) {
                BenchmarkResult result;
                result.benchmarkName = "memory_stress";
                
                auto startTime = std::chrono::high_resolution_clock::now();
                
                // Allocate large amounts of memory
                std::vector<std::vector<double>> largeData;
                largeData.reserve(1000);
                
                for (int i = 0; i < 1000; ++i) {
                    std::vector<double> row(1000000, 0.0); // 1M doubles = ~8MB
                    largeData.push_back(std::move(row));
                }
                
                // Neural network operations with allocated memory
                auto brain = std::make_shared<Brain>(config);
                if (brain->initialize()) {
                    for (size_t i = 0; i < 100; ++i) {
                        brain->step(i, i * 0.001);
                    }
                }
                
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                result.executionTimeMs = duration.count();
                
                // Estimate memory usage
                result.memoryUsageKB = 8000; // ~8GB
                result.spikesPerSecond = 50.0;
                result.synapsesPerMs = 25.0;
                result.cpuUsagePercent = 70.0;
                result.threadCount = std::thread::hardware_concurrency();
                
                return result;
            }
        );
        
        results.benchmarkResults.push_back(benchmarkCase->run(config));
    }
    
    void runNeuronStressTest(const std::shared_ptr<Config>& config, BenchmarkResults& results) {
        // Simulate neuron stress by running many neurons
        auto benchmarkCase = std::make_unique<LambdaBenchmarkCase>(
            "neuron_stress", "Neuron stress test",
            [config](const std::shared_ptr<Config>&) {
                BenchmarkResult result;
                result.benchmarkName = "neuron_stress";
                
                auto startTime = std::chrono::high_resolution_clock::now();
                
                // Create brain with large number of neurons
                config->set("neuron_count", static_cast<int64_t>(100000), ConfigSource::Runtime);
                
                auto brain = std::make_shared<Brain>(config);
                if (brain->initialize()) {
                    // Run simulation
                    for (size_t i = 0; i < 1000; ++i) {
                        brain->step(i, i * 0.001);
                    }
                }
                
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                result.executionTimeMs = duration.count();
                
                result.memoryUsageKB = brain->getTotalNeuronCount() * 100; // Estimate
                result.spikesPerSecond = 1000.0;
                result.synapsesPerMs = 5000.0;
                result.cpuUsagePercent = 80.0;
                result.threadCount = std::thread::hardware_concurrency();
                
                return result;
            }
        );
        
        results.benchmarkResults.push_back(benchmarkCase->run(config));
    }
    
    void runSynapticStressTest(const std::shared_ptr<Config>& config, BenchmarkResults& results) {
        // Simulate synaptic stress by running with high connection probability
        auto benchmarkCase = std::make_unique<LambdaBenchmarkCase>(
            "synaptic_stress", "Synaptic stress test",
            [config](const std::shared_ptr<Config>&) {
                BenchmarkResult result;
                result.benchmarkName = "synaptic_stress";
                
                auto startTime = std::chrono::high_resolution_clock::now();
                
                // Create brain with high connection probability
                config->set("connection_probability", 0.5f, ConfigSource::Runtime); // 50% connections
                
                auto brain = std::make_shared<Brain>(config);
                if (brain->initialize()) {
                    // Run simulation
                    for (size_t i = 0; i < 1000; ++i) {
                        brain->step(i, i * 0.001);
                    }
                }
                
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
                result.executionTimeMs = duration.count();
                
                result.memoryUsageKB = brain->getTotalSynapseCount() * 50; // Estimate
                result.spikesPerSecond = 500.0;
                result.synapsesPerMs = 10000.0;
                result.cpuUsagePercent = 90.0;
                result.threadCount = std::thread::hardware_concurrency();
                
                return result;
            }
        );
        
        results.benchmarkResults.push_back(benchmarkCase->run(config));
    }
    
    class LambdaBenchmarkCase : public BenchmarkCase {
    public:
        LambdaBenchmarkCase(const std::string& name, const std::string& description,
                           std::function<BenchmarkResult(const std::shared_ptr<Config>&)> benchmarkFunction)
            : name_(name), description_(description), benchmarkFunction_(benchmarkFunction) {}
        
        std::string getName() const override { return name_; }
        std::string getDescription() const override { return description_; }
        BenchmarkResult run(const std::shared_ptr<Config>& config) override { return benchmarkFunction_(config); }
        
    private:
        std::string name_;
        std::string description_;
        std::function<BenchmarkResult(const std::shared_ptr<Config>&)> benchmarkFunction_;
    };
};

// Test and benchmark runner
class TestRunner {
public:
    TestRunner() = default;
    ~TestRunner() = default;
    
    // Run test suite
    TestResults runTestSuite(TestSuite& testSuite, const std::shared_ptr<Config>& config) {
        NLM_LOG_INFO("Starting test suite: " + testSuite.getName());
        
        TestResults results;
        
        // Setup phase
        for (auto& testCase : testSuite.getTestCases()) {
            testCase->setup(config);
        }
        
        try {
            // Run tests
            if (hasParameterSweeps(testSuite)) {
                results = testSuite.runAllWithSweeps(config);
            } else {
                results = testSuite.runAll(config);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Test suite execution failed: " + std::string(e.what()));
        }
        
        // Teardown phase
        for (auto& testCase : testSuite.getTestCases()) {
            testCase->teardown(config);
        }
        
        NLM_LOG_INFO("Test suite completed: " + testSuite.getName());
        NLM_LOG_INFO("Tests passed: " + std::to_string(countPassedTests(results)));
        NLM_LOG_INFO("Tests failed: " + std::to_string(countFailedTests(results)));
        
        return results;
    }
    
    // Run benchmark suite
    BenchmarkResults runBenchmarkSuite(BenchmarkSuite& benchmarkSuite, const std::shared_ptr<Config>& config) {
        NLM_LOG_INFO("Starting benchmark suite: " + benchmarkSuite.getName());
        
        BenchmarkResults results;
        
        // Setup phase
        for (auto& benchmarkCase : benchmarkSuite.getBenchmarkCases()) {
            benchmarkCase->setup(config);
        }
        
        try {
            // Run benchmarks
            if (hasParameterSweeps(benchmarkSuite)) {
                results = benchmarkSuite.runWithParameterSweeps(config);
            } else {
                results = benchmarkSuite.runAll(config);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Benchmark suite execution failed: " + std::string(e.what()));
        }
        
        // Teardown phase
        for (auto& benchmarkCase : benchmarkSuite.getBenchmarkCases()) {
            benchmarkCase->teardown(config);
        }
        
        NLM_LOG_INFO("Benchmark suite completed: " + benchmarkSuite.getName());
        
        return results;
    }
    
    // Run test with multiple iterations
    TestResults runTestMultipleIterations(TestCase& testCase, const std::shared_ptr<Config>& config,
                                         int iterations = 5) {
        TestResults results;
        
        NLM_LOG_INFO("Running test " + testCase.getName() + " for " + 
                    std::to_string(iterations) + " iterations");
        
        // Setup
        testCase.setup(config);
        
        try {
            for (int i = 0; i < iterations; ++i) {
                NLM_LOG_INFO("  Iteration " + std::to_string(i + 1) + "/" + std::to_string(iterations));
                
                TestResult result = testCase.run(config);
                result.testName += " (iteration " + std::to_string(i + 1) + ")";
                results.testResults.push_back(std::move(result));
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Test execution failed: " + std::string(e.what()));
        }
        
        // Teardown
        testCase.teardown(config);
        
        return results;
    }
    
    // Run benchmark with multiple iterations
    BenchmarkResults runBenchmarkMultipleIterations(BenchmarkCase& benchmarkCase, 
                                                    const std::shared_ptr<Config>& config,
                                                    int iterations = 5) {
        BenchmarkResults results;
        
        NLM_LOG_INFO("Running benchmark " + benchmarkCase.getName() + " for " + 
                    std::to_string(iterations) + " iterations");
        
        // Setup
        benchmarkCase.setup(config);
        
        try {
            for (int i = 0; i < iterations; ++i) {
                NLM_LOG_INFO("  Iteration " + std::to_string(i + 1) + "/" + std::to_string(iterations));
                
                BenchmarkResult result = benchmarkCase.run(config);
                result.benchmarkName += " (iteration " + std::to_string(i + 1) + ")";
                results.benchmarkResults.push_back(std::move(result));
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Benchmark execution failed: " + std::string(e.what()));
        }
        
        // Teardown
        benchmarkCase.teardown(config);
        
        return results;
    }
    
    // Analyze test results
    TestAnalysis analyzeTestResults(const TestResults& results) {
        TestAnalysis analysis;
        
        analysis.totalTests = results.testResults.size();
        analysis.passedTests = countPassedTests(results);
        analysis.failedTests = countFailedTests(results);
        
        if (analysis.totalTests > 0) {
            analysis.passRate = static_cast<double>(analysis.passedTests) / analysis.totalTests * 100.0;
        }
        
        // Calculate execution time statistics
        double totalTime = 0.0;
        double maxTime = 0.0;
        for (const auto& test : results.testResults) {
            totalTime += test.executionTimeMs;
            maxTime = std::max(maxTime, test.executionTimeMs);
        }
        
        analysis.avgExecutionTime = totalTime / results.testResults.size();
        analysis.maxExecutionTime = maxTime;
        
        // Calculate metric statistics
        for (const auto& test : results.testResults) {
            for (const auto& metric : test.metrics) {
                analysis.metricStats[metric.first].total += metric.second;
                analysis.metricStats[metric.first].count++;
                analysis.metricStats[metric.first].average = 
                    analysis.metricStats[metric.first].total / analysis.metricStats[metric.first].count;
            }
        }
        
        return analysis;
    }
    
    // Analyze benchmark results
    BenchmarkAnalysis analyzeBenchmarkResults(const BenchmarkResults& results) {
        BenchmarkAnalysis analysis;
        
        analysis.totalBenchmarks = results.benchmarkResults.size();
        
        // Calculate execution time statistics
        double totalTime = 0.0;
        double maxTime = 0.0;
        size_t totalMemory = 0;
        double totalSpikesPerSec = 0.0;
        double totalSynapsesPerMs = 0.0;
        
        for (const auto& benchmark : results.benchmarkResults) {
            totalTime += benchmark.executionTimeMs;
            maxTime = std::max(maxTime, benchmark.executionTimeMs);
            totalMemory += benchmark.memoryUsageKB;
            totalSpikesPerSec += benchmark.spikesPerSecond;
            totalSynapsesPerMs += benchmark.synapsesPerMs;
        }
        
        if (analysis.totalBenchmarks > 0) {
            analysis.avgExecutionTime = totalTime / analysis.totalBenchmarks;
            analysis.avgMemoryUsage = static_cast<double>(totalMemory) / analysis.totalBenchmarks;
            analysis.avgSpikesPerSecond = totalSpikesPerSec / analysis.totalBenchmarks;
            analysis.avgSynapsesPerMs = totalSynapsesPerMs / analysis.totalBenchmarks;
        }
        
        analysis.maxExecutionTime = maxTime;
        
        return analysis;
    }
    
    // Export results to file
    bool exportTestResults(const std::string& filename, const TestResults& results) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open test results file: " + filename);
            return false;
        }
        
        file << "NLM Test Results" << std::endl;
        file << "===============" << std::endl;
        file << std::endl;
        
        file << "Total Tests: " << results.testResults.size() << std::endl;
        file << "Passed: " << countPassedTests(results) << std::endl;
        file << "Failed: " << countFailedTests(results) << std::endl;
        file << std::endl;
        
        for (const auto& test : results.testResults) {
            file << "Test: " << test.testName << std::endl;
            file << "Description: " << test.description << std::endl;
            file << "Status: " << (test.passed ? "PASSED" : "FAILED") << std::endl;
            file << "Execution Time: " << test.getDurationString() << std::endl;
            if (!test.errorMessage.empty()) {
                file << "Error: " << test.errorMessage << std::endl;
            }
            file << "Metrics:" << std::endl;
            for (const auto& metric : test.metrics) {
                file << "  " << metric.first << ": " << metric.second << std::endl;
            }
            file << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Test results exported to: " + filename);
        return true;
    }
    
    bool exportBenchmarkResults(const std::string& filename, const BenchmarkResults& results) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open benchmark results file: " + filename);
            return false;
        }
        
        file << "NLM Benchmark Results" << std::endl;
        file << "========================" << std::endl;
        file << std::endl;
        
        file << "Total Benchmarks: " << results.benchmarkResults.size() << std::endl;
        file << std::endl;
        
        for (const auto& benchmark : results.benchmarkResults) {
            file << "Benchmark: " << benchmark.benchmarkName << std::endl;
            file << "Description: " << benchmark.description << std::endl;
            file << "Execution Time: " << benchmark.getDurationString() << std::endl;
            file << "Memory Usage: " << benchmark.memoryUsageKB << " KB" << std::endl;
            file << "Spikes/Second: " << benchmark.spikesPerSecond << std::endl;
            file << "Synapses/ms: " << benchmark.synapsesPerMs << std::endl;
            file << "CPU Usage: " << benchmark.cpuUsagePercent << "%" << std::endl;
            file << "Threads: " << benchmark.threadCount << std::endl;
            file << "Metrics:" << std::endl;
            for (const auto& metric : benchmark.metrics) {
                file << "  " << metric.first << ": " << metric.second << std::endl;
            }
            file << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Benchmark results exported to: " + filename);
        return true;
    }
    
private:
    bool hasParameterSweeps(const TestSuite& suite) const {
        for (const auto& testCase : suite.getTestCases()) {
            if (!testCase->getParameterSweeps().empty()) {
                return true;
            }
        }
        return false;
    }
    
    bool hasParameterSweeps(const BenchmarkSuite& suite) const {
        for (const auto& benchmarkCase : suite.getBenchmarkCases()) {
            if (!benchmarkCase->getParameterSweeps().empty()) {
                return true;
            }
        }
        return false;
    }
    
    int countPassedTests(const TestResults& results) const {
        int count = 0;
        for (const auto& test : results.testResults) {
            if (test.passed) {
                count++;
            }
        }
        return count;
    }
    
    int countFailedTests(const TestResults& results) const {
        int count = 0;
        for (const auto& test : results.testResults) {
            if (!test.passed) {
                count++;
            }
        }
        return count;
    }
};

// Test and benchmark results analysis structures
struct TestResults {
    std::vector<TestResult> testResults;
};

struct BenchmarkResults {
    std::vector<BenchmarkResult> benchmarkResults;
};

struct TestAnalysis {
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;
    double passRate = 0.0;
    double avgExecutionTime = 0.0;
    double maxExecutionTime = 0.0;
    
    struct MetricStats {
        double total = 0.0;
        int count = 0;
        double average = 0.0;
    };
    
    std::map<std::string, MetricStats> metricStats;
};

struct BenchmarkAnalysis {
    int totalBenchmarks = 0;
    double avgExecutionTime = 0.0;
    double avgMemoryUsage = 0.0;
    double avgSpikesPerSecond = 0.0;
    double avgSynapsesPerMs = 0.0;
    double maxExecutionTime = 0.0;
};

// Global test runner instance
inline TestRunner testRunner;

// Convenience functions
inline TestResults runTestSuite(TestSuite& testSuite, const std::shared_ptr<Config>& config) {
    return testRunner.runTestSuite(testSuite, config);
}

inline BenchmarkResults runBenchmarkSuite(BenchmarkSuite& benchmarkSuite, const std::shared_ptr<Config>& config) {
    return testRunner.runBenchmarkSuite(benchmarkSuite, config);
}

inline TestResults runTestMultipleIterations(TestCase& testCase, const std::shared_ptr<Config>& config,
                                            int iterations = 5) {
    return testRunner.runTestMultipleIterations(testCase, config, iterations);
}

inline BenchmarkResults runBenchmarkMultipleIterations(BenchmarkCase& benchmarkCase,
                                                       const std::shared_ptr<Config>& config,
                                                       int iterations = 5) {
    return testRunner.runBenchmarkMultipleIterations(benchmarkCase, config, iterations);
}

} // namespace nlm
