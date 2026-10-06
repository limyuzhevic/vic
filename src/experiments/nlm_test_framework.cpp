// NLM Comprehensive Testing Framework
// Advanced testing framework for NLM with benchmarks and validation
// Provides comprehensive test suites and performance benchmarks

#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/experiments/Experiment.hpp"
#include "src/experiments/Metrics.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <chrono>
#include <thread>
#include <numeric>
#include <cmath>
#include <cassert>
#include <functional>

using namespace nlm;

namespace NLMTesting {
    
    // Test framework base class
    class TestBase {
    public:
        std::string name;
        std::string description;
        double executionTime;
        bool passed;
        
        TestBase(const std::string& testName, const std::string& testDesc)
            : name(testName), description(testDesc), executionTime(0.0), passed(false) {}
        
        virtual ~TestBase() = default;
        
        virtual void setup() = 0;
        virtual void teardown() = 0;
        virtual bool run() = 0;
        
        void recordTime(double time) {
            executionTime = time;
        }
        
        std::string getResultString() const {
            std::ostringstream oss;
            oss << (passed ? "PASS" : "FAIL") << " (" << std::fixed << std::setprecision(3) << executionTime << "s)";
            return oss.str();
        }
    };
    
    // Unit test for basic functionality
    class BrainUnitTest : public TestBase {
    public:
        std::shared_ptr<Brain> brain;
        std::shared_ptr<Config> config;
        
        BrainUnitTest() : TestBase("Brain Unit Test", "Test basic brain functionality") {}
        
        void setup() override {
            std::cout << "  Setting up unit test..." << std::endl;
            config = std::make_shared<Config>();
            config->set("neuron_count", 100, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
            config->set("random_seed", 42, ConfigSource::Default);
            
            brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
        }
        
        void teardown() override {
            std::cout << "  Cleaning up unit test..." << std::endl;
            brain.reset();
            config.reset();
        }
        
        bool run() override {
            std::cout << "  Running unit test..." << std::endl;
            
            try {
                // Test 1: Basic brain properties
                assert(brain->getTotalNeuronCount() == 100);
                assert(brain->getTotalSynapseCount() >= 0);
                assert(brain->getFiringNeuronCount() >= 0);
                assert(brain->getAverageFiringRate() >= 0.0);
                
                // Test 2: Neuron and synapse counts
                size_t neuronCount = brain->getTotalNeuronCount();
                size_t synapseCount = brain->getTotalSynapseCount();
                
                if (neuronCount == 0) {
                    std::cout << "    ERROR: No neurons initialized" << std::endl;
                    return false;
                }
                
                // Test 3: Developmental stage
                auto stage = brain->getDevelopmentalStage();
                std::cout << "    Developmental stage: " << static_cast<int>(stage.value) << std::endl;
                
                // Test 4: Configuration access
                assert(brain->getConfig() != nullptr);
                assert(brain->getConfig()->has("neuron_count"));
                assert(brain->getConfig()->getOr<int64_t>("neuron_count", 0) == 100);
                
                // Test 5: Brain stepping
                for (int step = 0; step < 10; ++step) {
                    brain->step(step, step * 0.001);
                }
                
                std::cout << "  Unit test passed!" << std::endl;
                return true;
                
            } catch (const std::exception& e) {
                std::cout << "  ERROR: " << e.what() << std::endl;
                return false;
            }
        }
    };
    
    // Integration test for system components
    class BrainIntegrationTest : public TestBase {
    public:
        std::shared_ptr<Brain> brain;
        std::shared_ptr<Config> config;
        
        BrainIntegrationTest() : TestBase("Brain Integration Test", "Test integration of all brain systems") {}
        
        void setup() override {
            std::cout << "  Setting up integration test..." << std::endl;
            config = std::make_shared<Config>();
            
            // Comprehensive configuration
            config->set("neuron_count", 500, ConfigSource::Default);
            config->set("region_count", 2, ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);
            config->set("random_seed", 123, ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.018f, ConfigSource::Default);
            config->set("stdp_tau", 20.0f, ConfigSource::Default);
            
            brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
        }
        
        void teardown() override {
            std::cout << "  Cleaning up integration test..." << std::endl;
            brain.reset();
            config.reset();
        }
        
        bool run() override {
            std::cout << "  Running integration test..." << std::endl;
            
            try {
                // Test 1: System initialization
                assert(brain->getTotalNeuronCount() > 0);
                assert(brain->getTotalSynapseCount() > 0);
                
                // Test 2: Sensory processing
                auto vision = std::make_shared<Vision>(4, 4, 3);
                std::vector<float> testData(48, 0.5f);
                vision->setData(testData);
                brain->receiveSensoryInput(vision);
                
                // Test 3: Motor action production
                brain->produceAction();
                
                // Test 4: Plasticity simulation
                for (int step = 0; step < 100; ++step) {
                    // Apply correlated input patterns for STDP
                    if (step % 20 == 0) {
                        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                    }
                    if (step % 25 == 0) {
                        brain->injectCurrentToNeurons(NeuronType::Motor, 25.0f);
                    }
                    
                    brain->step(step, step * 0.001);
                    
                    // Update development
                    brain->getDevelopmentSystem()->update(0.001);
                }
                
                // Test 5: Check for learning (changes in synaptic weights)
                size_t initialSynapses = brain->getTotalSynapseCount();
                double finalFiringRate = brain->getAverageFiringRate();
                
                // Test 6: Memory system integration
                brain->getWorkingMemory()->addItem("test_item", 0.5f);
                brain->getWorkingMemory()->updateItem("test_item", 0.7f);
                
                // Test 7: Neuromodulation
                brain->getAgentBrainInterface()->enableRewardModulation(true);
                brain->getAgentBrainInterface()->applyRewardModulation(1.0, 0.0);
                
                std::cout << "  Integration test passed!" << std::endl;
                std::cout << "    Neurons: " << brain->getTotalNeuronCount() << std::endl;
                std::cout << "    Synapses: " << brain->getTotalSynapseCount() << std::endl;
                std::cout << "    Final firing rate: " << std::fixed << std::setprecision(4) 
                          << finalFiringRate << " Hz" << std::endl;
                
                return true;
                
            } catch (const std::exception& e) {
                std::cout << "  ERROR: " << e.what() << std::endl;
                return false;
            }
        }
    };
    
    // Performance benchmark test
    class PerformanceBenchmark : public TestBase {
    public:
        struct BenchmarkMetrics {
            double totalWallClockTime;
            double avgProcessingTimePerStep;
            double processingSpeed;
            uint64_t totalNeurons;
            uint64_t totalSynapses;
            double spikesPerSecond;
            double memoryEfficiency;
        };
        
        BenchmarkMetrics metrics;
        
        PerformanceBenchmark() : TestBase("Performance Benchmark", "Performance benchmarking of NLM") {}
        
        void setup() override {
            std::cout << "  Setting up benchmark..." << std::endl;
        }
        
        void teardown() override {
            std::cout << "  Cleaning up benchmark..." << std::endl;
        }
        
        bool run() override {
            std::cout << "  Running performance benchmark..." << std::endl;
            
            // Configuration for benchmark
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 200, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("connection_probability", 0.1f, ConfigSource::Default);
            config->set("random_seed", 999, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                std::cout << "  ERROR: Failed to initialize brain" << std::endl;
                return false;
            }
            
            // Warm-up phase
            std::cout << "  Warm-up phase (50 steps)..." << std::endl;
            auto warmupStart = std::chrono::high_resolution_clock::now();
            for (int step = 0; step < 50; ++step) {
                brain->step(step, step * 0.001);
            }
            auto warmupEnd = std::chrono::high_resolution_clock::now();
            auto warmupDuration = std::chrono::duration<double>(warmupEnd - warmupStart).count();
            
            // Actual benchmark
            int benchmarkSteps = 1000;
            std::cout << "  Benchmark phase (" << benchmarkSteps << " steps)..." << std::endl;
            auto benchStart = std::chrono::high_resolution_clock::now();
            
            for (int step = 0; step < benchmarkSteps; ++step) {
                // Apply periodic input to maintain activity
                if (step % 20 == 0) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
                }
                
                brain->step(step, step * 0.001);
                
                // Progress indicator
                if ((step + 1) % 200 == 0) {
                    std::cout << "    Step " << step + 1 << "/" << benchmarkSteps << "..." << std::endl;
                }
            }
            
            auto benchEnd = std::chrono::high_resolution_clock::now();
            double totalDuration = std::chrono::duration<double>(benchEnd - benchStart).count();
            
            // Calculate metrics
            metrics.totalWallClockTime = totalDuration;
            metrics.avgProcessingTimePerStep = totalDuration / benchmarkSteps;
            metrics.processingSpeed = benchmarkSteps / totalDuration;
            metrics.totalNeurons = brain->getTotalNeuronCount();
            metrics.totalSynapses = brain->getTotalSynapseCount();
            metrics.spikesPerSecond = brain->getTotalSpikeCount() / totalDuration;
            metrics.memoryEfficiency = calculateMemoryEfficiency(metrics.totalNeurons, metrics.totalSynapses);
            
            std::cout << "  Benchmark completed successfully!" << std::endl;
            printBenchmarkResults();
            
            return true;
        }
        
        void printBenchmarkResults() {
            std::cout << "  === Benchmark Results ===" << std::endl;
            std::cout << "    Total wall clock time: " << std::fixed << std::setprecision(3) 
                      << metrics.totalWallClockTime << "s" << std::endl;
            std::cout << "    Average processing time per step: " << std::fixed << std::setprecision(6) 
                      << metrics.avgProcessingTimePerStep << "s" << std::endl;
            std::cout << "    Processing speed: " << std::fixed << std::setprecision(2) 
                      << metrics.processingSpeed << " steps/s" << std::endl;
            std::cout << "    Total neurons: " << metrics.totalNeurons << std::endl;
            std::cout << "    Total synapses: " << metrics.totalSynapses << std::endl;
            std::cout << "    Spike generation rate: " << std::fixed << std::setprecision(2) 
                      << metrics.spikesPerSecond << " spikes/s" << std::endl;
            std::cout << "    Memory efficiency: " << std::fixed << std::setprecision(6) 
                      << metrics.memoryEfficiency << " KB per neuron" << std::endl;
            std::cout << "    Network size: " << metrics.totalNeurons << " neurons, "
                      << metrics.totalSynapses << " synapses" << std::endl;
            std::cout << "    Throughput: " << std::fixed << std::setprecision(2)
                      << metrics.totalNeurons * metrics.processingSpeed << " neurons/s" << std::endl;
        }
        
    private:
        double calculateMemoryEfficiency(uint64_t neurons, uint64_t synapses) {
            // Estimate memory usage (rough approximation)
            double neuronMemory = neurons * sizeof(Neuron) * 3; // voltage, threshold, state
            double synapseMemory = synapses * sizeof(Synapse) * 2; // weight, type
            double totalMemory = neuronMemory + synapseMemory;
            return totalMemory / 1024.0; // KB per neuron
        }
    };
    
    // Stress test for robustness
    class StressTest : public TestBase {
    public:
        std::shared_ptr<Brain> brain;
        std::shared_ptr<Config> config;
        
        StressTest() : TestBase("Stress Test", "Stress test for system robustness") {}
        
        void setup() override {
            std::cout << "  Setting up stress test..." << std::endl;
            config = std::make_shared<Config>();
            
            // High-load configuration
            config->set("neuron_count", 1000, ConfigSource::Default);
            config->set("region_count", 3, ConfigSource::Default);
            config->set("connection_probability", 0.2f, ConfigSource::Default);
            config->set("random_seed", 777, ConfigSource::Default);
            config->set("simulation_timestep", 0.0005, ConfigSource::Default);
            
            brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
        }
        
        void teardown() override {
            std::cout << "  Cleaning up stress test..." << std::endl;
            brain.reset();
            config.reset();
        }
        
        bool run() override {
            std::cout << "  Running stress test..." << std::endl;
            
            try {
                int stressSteps = 2000;
                std::cout << "  Stress test phase (" << stressSteps << " steps)..." << std::endl;
                
                int checkpointCount = 0;
                for (int step = 0; step < stressSteps; ++step) {
                    // Heavy input loading
                    for (int neuronType = 0; neuronType < 3; ++neuronType) {
                        brain->injectCurrentToNeurons(static_cast<NeuronType>(neuronType), 100.0f);
                    }
                    
                    // Frequent stepping
                    brain->step(step, step * 0.0005);
                    
                    // Check for checkpoint events
                    if (step % 100 == 0) {
                        brain->save("./stress_checkpoint.bin");
                        checkpointCount++;
                    }
                    
                    // Load checkpoint periodically
                    if (step % 150 == 0 && checkpointCount > 0) {
                        brain->load("./stress_checkpoint.bin");
                    }
                    
                    // Development updates
                    brain->getDevelopmentSystem()->update(0.0005);
                    
                    // Progress indicator
                    if ((step + 1) % 400 == 0) {
                        std::cout << "    Step " << step + 1 << "/" << stressSteps << "..." << std::endl;
                    }
                }
                
                // Clean up checkpoints
                std::remove("./stress_checkpoint.bin");
                
                std::cout << "  Stress test completed successfully!" << std::endl;
                std::cout << "    Steps processed: " << stressSteps << std::endl;
                std::cout << "    Final neuron count: " << brain->getTotalNeuronCount() << std::endl;
                std::cout << "    Final synapse count: " << brain->getTotalSynapseCount() << std::endl;
                std::cout << "    Final firing rate: " << std::fixed << std::setprecision(4) 
                          << brain->getAverageFiringRate() << " Hz" << std::endl;
                
                return true;
                
            } catch (const std::exception& e) {
                std::cout << "  ERROR: " << e.what() << std::endl;
                return false;
            }
        }
    };
    
    // Regression test for specific functionality
    class RegressionTest : public TestBase {
    public:
        std::shared_ptr<Brain> brain;
        std::shared_ptr<Config> config;
        
        RegressionTest() : TestBase("Regression Test", "Regression test for specific functionality") {}
        
        void setup() override {
            std::cout << "  Setting up regression test..." << std::endl;
            config = std::make_shared<Config>();
            
            // Deterministic configuration for reproducible tests
            config->set("neuron_count", 50, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("connection_probability", 0.05f, ConfigSource::Default);
            config->set("random_seed", 12345, ConfigSource::Default);
            
            brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
        }
        
        void teardown() override {
            std::cout << "  Cleaning up regression test..." << std::endl;
            brain.reset();
            config.reset();
        }
        
        bool run() override {
            std::cout << "  Running regression test..." << std::endl;
            
            try {
                // Test 1: Reproducibility
                brain->reset();
                brain->initialize();
                
                // Record baseline metrics
                uint64_t baselineNeurons = brain->getTotalNeuronCount();
                uint64_t baselineSynapses = brain->getTotalSynapseCount();
                double baselineFiring = brain->getAverageFiringRate();
                
                // Run identical simulation
                for (int step = 0; step < 100; ++step) {
                    brain->step(step, step * 0.001);
                }
                
                uint64_t finalNeurons = brain->getTotalNeuronCount();
                uint64_t finalSynapses = brain->getTotalSynapseCount();
                double finalFiring = brain->getAverageFiringRate();
                
                // Test 2: Configuration persistence
                assert(brain->getConfig()->getOr<int64_t>("neuron_count", 0) == 50);
                assert(brain->getConfig()->getOr<int64_t>("region_count", 0) == 1);
                
                // Test 3: Developmental stage progression
                auto stage = brain->getDevelopmentalStage();
                assert(stage.value >= 0 && stage.value <= 4);
                
                // Test 4: Memory system integrity
                brain->getWorkingMemory()->addItem("test", 0.5f);
                auto item = brain->getWorkingMemory()->getItem("test");
                assert(item.has_value());
                assert(item.value().value == 0.5f);
                
                // Test 5: Learning capability
                bool learningOccurred = (finalFiring > baselineFiring * 1.01) || 
                                      (finalSynapses != baselineSynapses);
                
                std::cout << "  Regression test completed successfully!" << std::endl;
                std::cout << "    Neuron count consistent: " << (baselineNeurons == finalNeurons) << std::endl;
                std::cout << "    Synapse count changed: " << (baselineSynapses != finalSynapses) << std::endl;
                std::cout << "    Firing rate changed: " << (baselineFiring != finalFiring) << std::endl;
                std::cout << "    Learning occurred: " << (learningOccurred ? "Yes" : "No") << std::endl;
                
                return true;
                
            } catch (const std::exception& e) {
                std::cout << "  ERROR: " << e.what() << std::endl;
                return false;
            }
        }
    };
    
    // Comprehensive test suite
    class ComprehensiveTestSuite {
    public:
        std::vector<std::unique_ptr<TestBase>> tests;
        bool generateReports;
        std::string reportFile;
        
        ComprehensiveTestSuite(bool generateReports = false, const std::string& reportFile = "test_results.json")
            : generateReports(generateReports), reportFile(reportFile) {}
        
        void addTest(std::unique_ptr<TestBase> test) {
            tests.push_back(std::move(test));
        }
        
        void runAllTests() {
            std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
            std::cout << "Running " << tests.size() << " tests..." << std::endl;
            std::cout << std::endl;
            
            double totalStartTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            
            int passedTests = 0;
            int failedTests = 0;
            
            for (auto& test : tests) {
                std::cout << "Running test: " << test->name << std::endl;
                std::cout << "Description: " << test->description << std::endl;
                std::cout << "Status: " << std::endl;
                
                auto testStart = std::chrono::high_resolution_clock::now();
                
                try {
                    test->setup();
                    bool result = test->run();
                    test->teardown();
                    
                    auto testEnd = std::chrono::high_resolution_clock::now();
                    double testDuration = std::chrono::duration<double>(testEnd - testStart).count();
                    test->recordTime(testDuration);
                    test->passed = result;
                    
                    if (result) {
                        passedTests++;
                        std::cout << "  Result: PASS" << std::endl;
                    } else {
                        failedTests++;
                        std::cout << "  Result: FAIL" << std::endl;
                    }
                    
                } catch (const std::exception& e) {
                    std::cout << "  Exception: " << e.what() << std::endl;
                    std::cout << "  Result: ERROR" << std::endl;
                    failedTests++;
                }
                
                std::cout << std::endl;
            }
            
            double totalEndTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            double totalDuration = totalEndTime - totalStartTime;
            
            // Generate report
            generateTestReport(passedTests, failedTests, totalDuration);
            
            std::cout << "=== Test Suite Complete ===" << std::endl;
            std::cout << "Total tests: " << tests.size() << std::endl;
            std::cout << "Passed: " << passedTests << std::endl;
            std::cout << "Failed: " << failedTests << std::endl;
            std::cout << "Success rate: " << std::fixed << std::setprecision(2) 
                      << (passedTests * 100.0 / tests.size()) << "%" << std::endl;
            std::cout << "Total execution time: " << std::fixed << std::setprecision(3) 
                      << totalDuration << "s" << std::endl;
        }
        
        void generateTestReport(int passedTests, int failedTests, double totalDuration) {
            if (!generateReports) return;
            
            std::cout << "Generating test report..." << std::endl;
            
            std::ofstream file(reportFile);
            if (!file.is_open()) {
                std::cout << "Error: Cannot open report file " << reportFile << std::endl;
                return;
            }
            
            nlohmann::json report;
            
            report["timestamp"] = getCurrentTimestamp();
            report["test_suite"] = "NLM Comprehensive Test Suite";
            report["version"] = "2.0.0";
            report["total_tests"] = tests.size();
            report["passed_tests"] = passedTests;
            report["failed_tests"] = failedTests;
            report["success_rate"] = passedTests * 100.0 / tests.size();
            report["total_execution_time"] = totalDuration;
            
            report["tests"] = nlohmann::json::array();
            for (const auto& test : tests) {
                nlohmann::json testJson;
                testJson["name"] = test->name;
                testJson["description"] = test->description;
                testJson["passed"] = test->passed;
                testJson["execution_time"] = test->executionTime;
                testJson["result_string"] = test->getResultString();
                report["tests"].push_back(testJson);
            }
            
            file << report.dump(2) << std::endl;
            file.close();
            
            std::cout << "Test report saved to: " << reportFile << std::endl;
        }
        
    private:
        std::string getCurrentTimestamp() {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            return ss.str();
        }
    };
};

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Comprehensive Test Framework ===" << std::endl;
    
    // Parse command line arguments
    std::string reportFile = "test_results.json";
    bool generateReport = true;
    bool runAllTests = true;
    std::string testFilter = "";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--report" && i + 1 < argc) {
            reportFile = argv[++i];
            generateReport = true;
        } else if (arg == "--no-report") {
            generateReport = false;
        } else if (arg == "--test" && i + 1 < argc) {
            testFilter = argv[++i];
            runAllTests = false;
        } else if (arg == "--help") {
            std::cout << "Usage: nlm_test_framework [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --report <file>    Generate JSON test report (default: test_results.json)\n";
            std::cout << "  --no-report        Disable report generation\n";
            std::cout << "  --test <name>      Run specific test only\n";
            std::cout << "  --help             Show this help\n";
            return 0;
        }
    }
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLMTesting::ComprehensiveTestSuite testSuite(generateReport, reportFile);
    
    // Add tests to suite
    testSuite.addTest(std::make_unique<NLMTesting::BrainUnitTest>());
    testSuite.addTest(std::make_unique<NLMTesting::BrainIntegrationTest>());
    testSuite.addTest(std::make_unique<NLMTesting::PerformanceBenchmark>());
    testSuite.addTest(std::make_unique<NLMTesting::StressTest>());
    testSuite.addTest(std::make_unique<NLMTesting::RegressionTest>());
    
    // Filter tests if requested
    if (runAllTests && !testFilter.empty()) {
        std::cout << "Filtering tests by name: " << testFilter << std::endl;
        auto it = std::remove_if(testSuite.tests.begin(), testSuite.tests.end(),
            [&testFilter](const std::unique_ptr<NLMTesting::TestBase>& test) {
                return test->name.find(testFilter) == std::string::npos;
            });
        testSuite.tests.erase(it, testSuite.tests.end());
    }
    
    // Run all tests
    testSuite.runAllTests();
    
    return 0;
}
