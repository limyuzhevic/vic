#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <algorithm>
#include <cmath>
#include <thread>
#include <functional>

namespace nlm {

// Forward declarations
class Config;
class Brain;
class AgentBrain;
class SimpleWorld;
class SensoryPercept;
class MotorCommand;
class RandomGenerator;
class SimulationClock;
class Neuron;
class Synapse;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class EpisodicMemoryItem;
class DevelopmentalStage;

// Test result structure
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double executionTimeMs;
    std::map<std::string, std::string> metrics;
    
    TestResult(const std::string& testName, bool success, const std::string& msg = "")
        : name(testName), passed(success), message(msg), executionTimeMs(0.0) {}
};

// Benchmark result structure
struct BenchmarkResult {
    std::string name;
    std::string testName;
    double executionTimeMs;
    double operationsPerSecond;
    std::map<std::string, double> metrics;
    
    BenchmarkResult(const std::string& benchName, const std::string& testName, double time, double opsPerSec)
        : name(benchName), testName(testName), executionTimeMs(time), operationsPerSecond(opsPerSec) {}
};

// Comprehensive testing framework
class TestFramework {
public:
    TestFramework() = default;
    
    // Test management
    void addTest(const std::string& name, std::function<bool()> testFunc) {
        tests_.emplace_back(name, testFunc);
    }
    
    void addBenchmark(const std::string& name, std::function<void()> benchmarkFunc, 
                     size_t iterations = 1000) {
        benchmarks_.emplace_back(name, benchmarkFunc, iterations);
    }
    
    // Test execution
    std::vector<TestResult> runAllTests() {
        std::vector<TestResult> results;
        
        std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
        std::cout << "Running " << tests_.size() << " tests..." << std::endl << std::endl;
        
        for (const auto& test : tests_) {
            auto start = std::chrono::high_resolution_clock::now();
            
            try {
                bool passed = test.func();
                auto end = std::chrono::high_resolution_clock::now();
                
                double executionTime = std::chrono::duration<double, std::milli>(end - start).count();
                results.emplace_back(test.name, passed);
                results.back().executionTimeMs = executionTime;
                
                std::cout << "Running test: " << test.name << "... ";
                if (passed) {
                    std::cout << "PASSED (" << std::fixed << std::setprecision(3) << executionTime << "ms)" << std::endl;
                } else {
                    std::cout << "FAILED" << std::endl;
                }
            } catch (const std::exception& e) {
                auto end = std::chrono::high_resolution_clock::now();
                
                double executionTime = std::chrono::duration<double, std::milli>(end - start).count();
                results.emplace_back(test.name, false, std::string("Exception: ") + e.what());
                results.back().executionTimeMs = executionTime;
                
                std::cout << "Running test: " << test.name << "... ";
                std::cout << "ERROR: " << e.what() << std::endl;
            }
        }
        
        return results;
    }
    
    // Benchmark execution
    std::vector<BenchmarkResult> runAllBenchmarks() {
        std::vector<BenchmarkResult> results;
        
        std::cout << "=== NLM Performance Benchmark Suite ===" << std::endl;
        std::cout << "Running " << benchmarks_.size() << " benchmarks..." << std::endl << std::endl;
        
        for (const auto& benchmark : benchmarks_) {
            std::cout << "Running benchmark: " << benchmark.name << "...";
            
            auto start = std::chrono::high_resolution_clock::now();
            
            try {
                benchmark.func();
                
                auto end = std::chrono::high_resolution_clock::now();
                double executionTime = std::chrono::duration<double, std::milli>(end - start).count();
                double opsPerSecond = (benchmark.iterations * 1000.0) / executionTime;
                
                results.emplace_back(benchmark.name, benchmark.testName, executionTime, opsPerSecond);
                
                std::cout << " COMPLETED (" << std::fixed << std::setprecision(3) << executionTime << "ms, " << opsPerSecond << " ops/sec)" << std::endl;
            } catch (const std::exception& e) {
                auto end = std::chrono::high_resolution_clock::now();
                
                double executionTime = std::chrono::duration<double, std::milli>(end - start).count();
                
                std::cout << " ERROR: " << e.what() << std::endl;
            }
        }
        
        return results;
    }
    
    // Report generation
    void generateReport(const std::vector<TestResult>& testResults, 
                       const std::vector<BenchmarkResult>& benchmarkResults,
                       const std::string& outputFile = "test_report.json") const {
        
        std::cout << "\n=== Generating Test Report ===" << std::endl;
        
        // Generate summary statistics
        int totalTests = testResults.size();
        int passedTests = 0;
        int failedTests = 0;
        
        for (const auto& result : testResults) {
            if (result.passed) {
                passedTests++;
            } else {
                failedTests++;
            }
        }
        
        double totalTestTime = 0.0;
        for (const auto& result : testResults) {
            totalTestTime += result.executionTimeMs;
        }
        
        // Generate console report
        std::cout << "\n=== TEST SUMMARY ===" << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        std::cout << "Success rate: " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0.0) << "%" << std::endl;
        std::cout << "Total execution time: " << std::fixed << std::setprecision(3) << totalTestTime << "ms" << std::endl;
        std::cout << "Average test time: " << (totalTests > 0 ? totalTestTime / totalTests : 0.0) << "ms" << std::endl;
        
        std::cout << "\n=== FAILED TESTS ===" << std::endl;
        for (const auto& result : testResults) {
            if (!result.passed) {
                std::cout << "- " << result.name << ": " << result.message << std::endl;
            }
        }
        
        // Generate benchmark summary
        if (!benchmarkResults.empty()) {
            std::cout << "\n=== BENCHMARK SUMMARY ===" << std::endl;
            
            double totalBenchmarkTime = 0.0;
            for (const auto& result : benchmarkResults) {
                totalBenchmarkTime += result.executionTimeMs;
            }
            
            std::cout << "Total benchmarks: " << benchmarkResults.size() << std::endl;
            std::cout << "Total benchmark time: " << std::fixed << std::setprecision(3) << totalBenchmarkTime << "ms" << std::endl;
            
            std::cout << "\n=== DETAILED BENCHMARKS ===" << std::endl;
            for (const auto& result : benchmarkResults) {
                std::cout << result.name << ": " << std::fixed << std::setprecision(3) << result.executionTimeMs << "ms (" << result.operationsPerSecond << " ops/sec)" << std::endl;
            }
        }
        
        std::cout << "\n=== REPORT GENERATION COMPLETE ===" << std::endl;
    }
    
    // Configuration-based test suites
    void runTestSuite(const std::string& suiteName) {
        if (suiteName == "unit") {
            runUnitTests();
        } else if (suiteName == "performance") {
            runPerformanceTests();
        } else if (suiteName == "integration") {
            runIntegrationTests();
        } else if (suiteName == "all") {
            runUnitTests();
            runPerformanceTests();
            runIntegrationTests();
        } else {
            std::cout << "Unknown test suite: " << suiteName << std::endl;
        }
    }
    
private:
    struct Test {
        std::string name;
        std::function<bool()> func;
    };
    
    struct Benchmark {
        std::string name;
        std::string testName;
        std::function<void()> func;
        size_t iterations;
    };
    
    std::vector<Test> tests_;
    std::vector<Benchmark> benchmarks_;
    
    void runUnitTests() {
        // Core type tests
        addTest("CoreTypes", []() {
            // Test type definitions
            NeuronId id(42);
            SynapseId synId(100);
            RegionId regId(1);
            PopulationId popId(2);
            
            assert(id.index() == 42);
            assert(synId.index() == 100);
            assert(regId.index() == 1);
            assert(popId.index() == 2);
            
            return true;
        });
        
        // Configuration tests
        addTest("Configuration", []() {
            Config config;
            
            config.set("test_int", 42);
            config.set("test_double", 3.14);
            config.set("test_bool", true);
            config.set("test_string", "hello");
            
            assert(config.getOr<int>("test_int", 0) == 42);
            assert(config.getOr<double>("test_double", 0.0) == 3.14);
            assert(config.getOr<bool>("test_bool", false) == true);
            assert(config.getOr<std::string>("test_string", "") == "hello");
            
            return true;
        });
        
        // Brain creation tests
        addTest("BrainCreation", []() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(100));
            config->set("region_count", static_cast<int64_t>(1));
            
            Brain brain(config);
            assert(brain.getTotalNeuronCount() == 100);
            assert(brain.getRegionCount() == 1);
            
            return true;
        });
        
        // Random number generator tests
        addTest("RandomGenerator", []() {
            RandomGenerator rng(42);
            
            // Test uniform distribution
            float val1 = rng.uniformReal(0.0f, 1.0f);
            float val2 = rng.uniformReal(0.0f, 1.0f);
            
            assert(val1 >= 0.0f && val1 <= 1.0f);
            assert(val2 >= 0.0f && val2 <= 1.0f);
            
            // Test integer distribution
            int intVal = rng.uniformInt(1, 10);
            assert(intVal >= 1 && intVal <= 10);
            
            return true;
        });
        
        // Simulation clock tests
        addTest("SimulationClock", []() {
            SimulationClock clock(0.001);
            
            assert(clock.getTimestep() == 0.001);
            assert(clock.getCurrentStep() == 0);
            assert(clock.getCurrentTime() == 0.0);
            
            clock.step();
            assert(clock.getCurrentStep() == 1);
            assert(clock.getCurrentTime() == 0.001);
            
            return true;
        });
    }
    
    void runPerformanceTests() {
        // Neuron performance benchmark
        addBenchmark("NeuronStepBenchmark", []() {
            // Create a neuron and benchmark step execution
            Neuron neuron(1);
            neuron.setMembranePotential(-70.0f);
            neuron.setThreshold(-55.0f);
            neuron.setRestingPotential(-70.0f);
            neuron.setRefractoryPeriod(5);
            
            Timestamp currentTime = 0.0;
            TimestepDuration dt = 0.001;
            
            for (int i = 0; i < 100; ++i) {
                neuron.stepLIF(currentTime, dt);
                currentTime += dt;
            }
        }, 1000);
        
        // Synapse performance benchmark
        addBenchmark("SynapseUpdateBenchmark", []() {
            // Create a synapse and benchmark update operations
            Synapse synapse(1, NeuronId(1), NeuronId(2));
            
            for (int i = 0; i < 100; ++i) {
                synapse.recordPreSpike(i * 0.001);
                synapse.recordPostSpike(i * 0.001);
                synapse.step(i * 0.001);
            }
        }, 1000);
        
        // Brain initialization benchmark
        addBenchmark("BrainInitBenchmark", []() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(1000));
            config->set("region_count", static_cast<int64_t>(2));
            config->set("connection_probability", 0.1f);
            
            Brain brain(config);
            brain.initialize();
        }, 100);
    }
    
    void runIntegrationTests() {
        // Agent integration test
        addTest("AgentIntegration", []() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(100));
            config->set("region_count", static_cast<int64_t>(1));
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) return false;
            
            AgentBrain agent(brain);
            
            // Create a simple world
            SimpleWorld world;
            world.initialize(16, 16);
            
            agent.initialize(world);
            
            // Test sensory processing
            SensoryPercept percept;
            // Add some dummy sensory data
            for (int i = 0; i < 256; ++i) {
                percept.addVision(i * 0.01f);
            }
            
            agent.processSensoryInput(percept);
            
            // Test motor decoding
            MotorCommand cmd = agent.decodeMotorCommand();
            
            return true;
        });
        
        // Memory integration test
        addTest("MemoryIntegration", []() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(100));
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Test working memory
            auto* workingMemory = brain->getWorkingMemory();
            if (workingMemory) {
                workingMemory->storeToNeuron(NeuronId(1), 0.5f);
                assert(workingMemory->getActiveTraces() > 0);
            }
            
            // Test episodic memory
            auto* episodicMemory = brain->getEpisodicMemory();
            if (episodicMemory) {
                // Store an episode
                EpisodicMemoryItem episode;
                episode.timestamp = 1;
                episode.reward = 1.0f;
                episode.activeNeurons.push_back(NeuronId(1));
                episode.neuronActivations.push_back(0.5f);
                
                episodicMemory->storeEpisode(episode);
                assert(episodicMemory->getEpisodeCount() > 0);
            }
            
            // Test associative memory
            auto* associativeMemory = brain->getAssociativeMemory();
            if (associativeMemory) {
                // Add some associations
                for (int i = 0; i < 10; ++i) {
                    associativeMemory->storeAssociation(i, i + 100);
                }
            }
            
            return true;
        });
        
        // Neuromodulation integration test
        addTest("NeuromodulationIntegration", []() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(100));
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Test dopamine
            auto* dopamine = brain->getDopamine();
            if (dopamine) {
                // Update dopamine level
                dopamine->update(0.001);
                assert(dopamine->getLevel() >= 0.0f && dopamine->getLevel() <= 1.0f);
            }
            
            // Test curiosity
            auto* curiosity = brain->getCuriosity();
            if (curiosity) {
                curiosity->update(0.001);
                assert(curiosity->getLevel() >= 0.0f && curiosity->getLevel() <= 1.0f);
            }
            
            // Test novelty
            auto* novelty = brain->getNovelty();
            if (novelty) {
                novelty->update(0.001);
                assert(novelty->getLevel() >= 0.0f && novelty->getLevel() <= 1.0f);
            }
            
            return true;
        });
        
        // Development integration test
        addTest("DevelopmentIntegration", []() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", static_cast<int64_t>(100));
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Update development system
            auto* development = brain->getDevelopmentSystem();
            if (development) {
                development->update(brain.get(), *brain->getRandomGenerator(), 1000.0);
            }
            
            // Check developmental stage
            auto stage = brain->getDevelopmentalStage();
            assert(stage == DevelopmentalStage::Initial || stage == DevelopmentalStage::CriticalPeriod ||
                   stage == DevelopmentalStage::Maturation || stage == DevelopmentalStage::Adult);
            
            return true;
        });
    }
};

} // namespace nlm

// Convenience function to run all tests
inline void runNLMTests() {
    nlm::TestFramework framework;
    
    // Run all test suites
    std::vector<nlm::TestResult> testResults;
    std::vector<nlm::BenchmarkResult> benchmarkResults;
    
    // Run unit tests
    testResults = framework.runAllTests();
    
    // Run benchmarks
    benchmarkResults = framework.runAllBenchmarks();
    
    // Generate report
    framework.generateReport(testResults, benchmarkResults);
    
    // Exit with appropriate code
    bool allTestsPassed = true;
    for (const auto& result : testResults) {
        if (!result.passed) {
            allTestsPassed = false;
            break;
        }
    }
    
    if (!allTestsPassed) {
        std::cout << "\nSome tests failed!" << std::endl;
        exit(1);
    }
}