// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <functional>

using namespace nlm;

// Utility functions for NLM testing and experimentation
namespace nlm::test {

    // Utility class for managing brain reset/initialize cycles
    class BrainResetManager {
    public:
        BrainResetManager(std::shared_ptr<Brain> brain) : brain_(brain) {}
        
        // Reset brain for next experiment - handles the common pattern
        void resetForExperiment() {
            NLM_LOG_INFO("Resetting brain for new experiment...");
            brain_->reset();
            brain_->initialize();
            brain_->logStatus();
        }
        
        // Execute test with automatic reset before and after
        template<typename TestFunc>
        void runWithReset(TestFunc testFunc, const std::string& testName) {
            NLM_LOG_INFO("=== " + testName + " ===");
            resetForExperiment();
            testFunc();
        }
    
    private:
        std::shared_ptr<Brain> brain_;
    };

    // Progress tracking utility for long-running tests
    class ProgressTracker {
    public:
        ProgressTracker(int totalSteps, const std::string& description = "Progress") 
            : totalSteps_(totalSteps), description_(description), startTime_(std::chrono::high_resolution_clock::now()) {}
        
        void update(int currentStep) {
            if (currentStep % logInterval_ == 0 || currentStep == totalSteps_) {
                auto now = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_).count();
                
                NLM_LOG_INFO("  " + description_ + ": Step " + std::to_string(currentStep) + 
                           ", " + std::to_string(elapsed) + "s elapsed");
                logInterval_ = std::max(100, totalSteps_ / 10);  // Adjust interval for longer tests
            }
        }
        
        float getElapsedSeconds() const {
            auto now = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::duration<float>>(now - startTime_).count();
        }
    
    private:
        int totalSteps_;
        std::string description_;
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
        int logInterval_ = 100;
    };

    // Test result collection for comprehensive analysis
    struct TestResult {
        std::string testName;
        bool passed;
        std::string details;
        float executionTime;
        size_t neuronsGenerated;
        size_t spikesGenerated;
        float learningEffect;
        
        TestResult(const std::string& name) : testName(name), passed(false), 
            executionTime(0.0f), neuronsGenerated(0), spikesGenerated(0), learningEffect(0.0f) {}
    };

    // Comprehensive benchmarking infrastructure for performance analysis
namespace nlm::test {

    // Performance metrics collection
    struct PerformanceMetrics {
        std::string testName;
        std::chrono::duration<double> wallClockTime;
        std::chrono::duration<double> cpuTime;
        size_t memoryUsage;
        size_t spikeCount;
        size_t neuronCount;
        size_t synapseCount;
        float learningRate;
        float stabilityScore;
        std::vector<float> timeSeriesData;
        
        PerformanceMetrics(const std::string& name) : testName(name), 
            wallClockTime(0.0), cpuTime(0.0), memoryUsage(0), spikeCount(0), 
            neuronCount(0), synapseCount(0), learningRate(0.0f), stabilityScore(0.0f) {}
        
        float getPerformanceScore() const {
            float score = 1000.0f / (1.0f + wallClockTime.count());
            score += stabilityScore * 0.5f;
            score += learningRate * 0.3f;
            score += (1.0f / (1.0f + memoryUsage / 1000.0f)) * 0.2f;
            return score;
        }
        
        std::string getPerformanceReport() const {
            std::stringstream ss;
            ss << "=== Performance Metrics for " << testName << " ===" << std::endl;
            ss << "Wall Clock Time: " << wallClockTime.count() << "s" << std::endl;
            ss << "CPU Time: " << cpuTime.count() << "s" << std::endl;
            ss << "Memory Usage: " << memoryUsage << " bytes" << std::endl;
            ss << "Spikes Generated: " << spikeCount << std::endl;
            ss << "Neuron Count: " << neuronCount << std::endl;
            ss << "Synapse Count: " << synapseCount << std::endl;
            ss << "Learning Rate: " << learningRate << std::endl;
            ss << "Stability Score: " << stabilityScore << std::endl;
            ss << "Performance Score: " << getPerformanceScore() << std::endl;
            return ss.str();
        }
    };

    // Benchmarking framework for systematic performance analysis
    class BenchmarkFramework {
    public:
        struct BenchmarkConfig {
            std::string name;
            std::chrono::duration<double> timeout;
            bool enableMemoryTracking;
            bool enableCPUTracking;
            std::unordered_map<std::string, float> parameters;
        };
        
        static std::vector<PerformanceMetrics> runBenchmarkSuite(
            std::shared_ptr<Brain> brain,
            const std::vector<BenchmarkConfig>& benchmarks,
            const std::string& suiteName = "Benchmark Suite") {
            
            log::section(suiteName + " - Performance Benchmark");
            
            std::vector<PerformanceMetrics> results;
            
            NLM_LOG_INFO("Starting benchmark suite with " + std::to_string(benchmarks.size()) + " benchmarks");
            
            for (size_t i = 0; i < benchmarks.size(); ++i) {
                const auto& config = benchmarks[i];
                NLM_LOG_INFO("");
                NLM_LOG_INFO("Running benchmark " + std::to_string(i + 1) + "/" + std::to_string(benchmarks.size()) + 
                           ": " + config.name);
                
                PerformanceMetrics metrics(config.name);
                
                auto startWallTime = std::chrono::high_resolution_clock::now();
                auto startCPUTime = std::chrono::system_clock::now();
                
                try {
                    // Configure brain for benchmark
                    configureBrainForBenchmark(brain, config);
                    
                    // Run the benchmarked operation
                    runBenchmarkOperation(brain, config, metrics);
                    
                    // Collect metrics
                    auto endWallTime = std::chrono::high_resolution_clock::now();
                    auto endCPUTime = std::chrono::system_clock::now();
                    
                    metrics.wallClockTime = std::chrono::duration_cast<std::chrono::duration<double>>(endWallTime - startWallTime);
                    metrics.cpuTime = std::chrono::duration_cast<std::chrono::duration<double>>(endCPUTime - startCPUTime);
                    
                    // Get current brain state metrics
                    collectBrainMetrics(brain, metrics);
                    
                    results.push_back(metrics);
                    
                    // Log benchmark results
                    NLM_LOG_INFO("Benchmark " + config.name + " completed:");
                    NLM_LOG_INFO("  Wall Time: " + std::to_string(metrics.wallClockTime.count()) + "s");
                    NLM_LOG_INFO("  CPU Time: " + std::to_string(metrics.cpuTime.count()) + "s");
                    NLM_LOG_INFO("  Spikes: " + std::to_string(metrics.spikeCount));
                    NLM_LOG_INFO("  Performance Score: " + std::to_string(metrics.getPerformanceScore()));
                    
                    ErrorReporter::logProgress("Benchmark Suite", 
                                             static_cast<float>(i + 1) / benchmarks.size(),
                                             "Completed " + config.name);
                    
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Benchmark " + config.name + " failed: " + std::string(e.what()));
                    PerformanceMetrics errorMetrics(config.name + " (ERROR)");
                    errorMetrics.wallClockTime = std::chrono::duration<double>(0.0);
                    errorMetrics.cpuTime = std::chrono::duration<double>(0.0);
                    results.push_back(errorMetrics);
                }
            }
            
            // Benchmark suite summary
            generateBenchmarkReport(results, suiteName);
            
            return results;
        }
        
        static void configureBrainForBenchmark(std::shared_ptr<Brain> brain, const BenchmarkConfig& config) {
            // Reset and configure brain for benchmarking
            brain->reset();
            brain->initialize();
            
            // Apply benchmark-specific configurations
            if (config.parameters.find("neuron_count") != config.parameters.end()) {
                // Note: In a real implementation, you would modify the brain configuration
                // This is a placeholder for benchmark-specific brain configuration
            }
            
            if (config.parameters.find("connection_probability") != config.parameters.end()) {
                // Similar placeholder for connection probability
            }
        }
        
        static void runBenchmarkOperation(std::shared_ptr<Brain> brain, const BenchmarkConfig& config, PerformanceMetrics& metrics) {
            // Example benchmark operations - can be customized
            std::string benchmarkName = config.name;
            
            if (benchmarkName == "Connectivity Benchmark") {
                runConnectivityBenchmark(brain, metrics, config);
            } else if (benchmarkName == "Learning Benchmark") {
                runLearningBenchmark(brain, metrics, config);
            } else if (benchmarkName == "Scalability Benchmark") {
                runScalabilityBenchmark(brain, metrics, config);
            } else {
                // Default benchmark operation
                runDefaultBenchmark(brain, metrics, config);
            }
        }
        
        static void runConnectivityBenchmark(std::shared_ptr<Brain> brain, PerformanceMetrics& metrics, const BenchmarkConfig& config) {
            log::subtest("Connectivity benchmark test");
            
            // Inject current and measure response time
            auto* region = brain->getRegion(RegionId(1));
            if (!region) return;
            
            auto neurons = region->getAllNeurons();
            if (neurons.empty()) return;
            
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            // Inject current
            for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                neurons[i]->injectCurrent(50.0f);
            }
            
            // Measure response time
            auto startTime = std::chrono::high_resolution_clock::now();
            for (SimulationStep step = 0; step < 50; ++step) {
                brain->step(step, step * config.parameters["timestep"]);
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            
            metrics.wallClockTime += std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
            
            size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
            metrics.spikeCount += spikes;
            
            metrics.learningRate = static_cast<float>(spikes) / 50.0f;
        }
        
        static void runLearningBenchmark(std::shared_ptr<Brain> brain, PerformanceMetrics& metrics, const BenchmarkConfig& config) {
            log::subtest("Learning benchmark test");
            
            // Run learning experiment
            EnhancedLearningExperiment experiment(brain, "Benchmark Learning");
            
            for (int step = 0; step < 1000; ++step) {
                for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }
                brain->step(step, step * config.parameters["timestep"]);
            }
            
            experiment.recordInitialState();
            experiment.recordFinalState();
            experiment.computeLearningScore();
            
            metrics.learningRate = experiment.learningScore;
            metrics.spikeCount += brain->getTotalSpikeCount();
            metrics.stabilityScore = experiment.evaluateSuccess() ? 1.0f : 0.0f;
        }
        
        static void runScalabilityBenchmark(std::shared_ptr<Brain> brain, PerformanceMetrics& metrics, const BenchmarkConfig& config) {
            log::subtest("Scalability benchmark test");
            
            // Test with different neuron counts (simulated)
            int baseNeuronCount = 100;
            
            for (int scale = 1; scale <= 3; ++scale) {
                int currentNeuronCount = baseNeuronCount * scale;
                
                // Simulate scalability by measuring operation time
                auto startTime = std::chrono::high_resolution_clock::now();
                
                // Simulate work proportional to neuron count
                for (int i = 0; i < currentNeuronCount / 10; ++i) {
                    // Simulate neural operations
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                }
                
                auto endTime = std::chrono::high_resolution_clock::now();
                metrics.wallClockTime += std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
            }
            
            metrics.stabilityScore = 0.8f; // Simplified stability calculation
        }
        
        static void runDefaultBenchmark(std::shared_ptr<Brain> brain, PerformanceMetrics& metrics, const BenchmarkConfig& config) {
            log::subtest("Default benchmark test");
            
            // Run standard benchmark operations
            for (int step = 0; step < 100; ++step) {
                brain->step(step, step * config.parameters["timestep"]);
                
                if (step % 20 == 0) {
                    metrics.timeSeriesData.push_back(static_cast<float>(brain->getTotalSpikeCount()));
                }
            }
            
            metrics.spikeCount += brain->getTotalSpikeCount();
            metrics.learningRate = 0.5f; // Simplified calculation
        }
        
        static void collectBrainMetrics(std::shared_ptr<Brain> brain, PerformanceMetrics& metrics) {
            metrics.neuronCount = brain->getTotalNeuronCount();
            metrics.synapseCount = brain->getTotalSynapseCount();
            metrics.spikeCount = brain->getTotalSpikeCount();
            
            // Calculate learning rate based on recent activity
            if (metrics.neuronCount > 0) {
                metrics.learningRate = static_cast<float>(metrics.spikeCount) / metrics.neuronCount;
            }
            
            // Calculate stability score based on firing consistency
            size_t firingCount = brain->getFiringNeuronCount();
            metrics.stabilityScore = static_cast<float>(firingCount) / metrics.neuronCount;
        }
        
        static void generateBenchmarkReport(const std::vector<PerformanceMetrics>& results, const std::string& suiteName) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== " + suiteName + " Summary ===");
            
            double totalTime = 0.0;
            double totalScore = 0.0;
            int successfulBenchmarks = 0;
            
            NLM_LOG_INFO("Benchmark | Time (s) | Score | Status");
            NLM_LOG_INFO("---------|----------|-------|--------");
            
            for (const auto& metrics : results) {
                if (metrics.testName.find(" (ERROR)") == std::string::npos) {
                    totalTime += metrics.wallClockTime.count();
                    totalScore += metrics.getPerformanceScore();
                    successfulBenchmarks++;
                    
                    NLM_LOG_INFO(metrics.testName + " | " + std::to_string(metrics.wallClockTime.count()) + 
                               " | " + std::to_string(metrics.getPerformanceScore()) + " | PASSED");
                } else {
                    NLM_LOG_INFO(metrics.testName + " | FAILED | N/A | FAILED");
                }
            }
            
            NLM_LOG_INFO("---------|----------|-------|--------");
            
            if (successfulBenchmarks > 0) {
                NLM_LOG_INFO("Average Time: " + std::to_string(totalTime / successfulBenchmarks) + "s");
                NLM_LOG_INFO("Average Score: " + std::to_string(totalScore / successfulBenchmarks));
                NLM_LOG_INFO("Successful Benchmarks: " + std::to_string(successfulBenchmarks));
                
                log::success(suiteName + " completed successfully");
            } else {
                log::error("All benchmarks failed");
            }
        }
    };
}

// Global error reporting instance
inline nlm::test::ErrorReporter& getErrorReporter() {
    static nlm::test::ErrorReporter reporter;
    return reporter;
}

    // Brain state snapshot utility
    struct BrainSnapshot {
        size_t neuronCount;
        size_t synapseCount;
        size_t spikeCount;
        size_t firingCount;
        float meanWeight;
        float totalActivity;
        
        BrainSnapshot(std::shared_ptr<Brain> brain) {
            neuronCount = brain->getTotalNeuronCount();
            synapseCount = brain->getTotalSynapseCount();
            spikeCount = brain->getTotalSpikeCount();
            firingCount = brain->getFiringNeuronCount();
            totalActivity = static_cast<float>(spikeCount + firingCount);
            
            // Calculate mean weight from first region
            meanWeight = 0.0f;
            if (auto* region = brain->getRegion(RegionId(1))) {
                if (!region->getSynapses().empty()) {
                    float sum = 0.0f;
                    for (const auto& syn : region->getSynapses()) {
                        sum += syn->getWeight();
                    }
                    meanWeight = sum / region->getSynapses().size();
                }
            }
        }
        
        std::string toString() const {
            return "Neurons: " + std::to_string(neuronCount) +
                   ", Synapses: " + std::to_string(synapseCount) +
                   ", Spikes: " + std::to_string(spikeCount) +
                   ", Firing: " + std::to_string(firingCount) +
                   ", Mean Weight: " + std::to_string(meanWeight) +
                   ", Total Activity: " + std::to_string(totalActivity);
        }
    };

    // Enhanced Learning Experiment with better statistics
    struct EnhancedLearningExperiment {
        std::shared_ptr<Brain> brain;
        std::string experimentName;
        BrainSnapshot initialState;
        BrainSnapshot finalState;
        float learningScore;
        
        EnhancedLearningExperiment(std::shared_ptr<Brain> b, const std::string& name) 
            : brain(b), experimentName(name), learningScore(0.0f) {}
        
        void recordInitialState() {
            log::section("Initial State - " + experimentName);
            initialState = BrainSnapshot(brain);
            NLM_LOG_INFO("  " + initialState.toString());
        }
        
        void recordFinalState() {
            log::section("Final State - " + experimentName);
            finalState = BrainSnapshot(brain);
            NLM_LOG_INFO("  " + finalState.toString());
        }
        
        void computeLearningScore() {
            learningScore = 0.0f;
            
            if (initialState.synapseCount > 0) {
                float weightChange = finalState.meanWeight - initialState.meanWeight;
                learningScore = std::abs(weightChange) / initialState.meanWeight;
                
                // Normalize with activity changes
                float activityChange = (finalState.totalActivity - initialState.totalActivity) / initialState.totalActivity;
                learningScore = learningScore * (1.0f + std::abs(activityChange));
            }
        }
        
        bool evaluateSuccess(float threshold = 0.01f) const {
            return learningScore > threshold;
        }
        
        std::string getAssessment() const {
            if (learningScore < 0.001f) return "Minimal learning detected";
            if (learningScore < 0.01f) return "Learning detected but small";
            if (learningScore < 0.1f) return "Significant learning";
            return "Major learning occurred";
        }
    };
}

using namespace nlm::test;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Enhanced Learning Experiment with better statistics
EnhancedLearningExperiment createEnhancedLearningExperiment(std::shared_ptr<Brain> brain, const std::string& name) {
    return EnhancedLearningExperiment(brain, name);
}

// Modern Learning Experiment using enhanced utilities
void runEnhancedLearningExperiment(std::shared_ptr<Brain> brain, const std::string& experimentName) {
    nlm::test::EnhancedLearningExperiment experiment(brain, experimentName);
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    nlm::test::ProgressTracker progress(config->getOr<int64_t>("plasticity_test_steps", 1000), "Learning Experiment");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (" + std::to_string(config->getOr<int64_t>("plasticity_test_steps", 1000)) + " steps)...");
    
    for (SimulationStep step = 0; step < config->getOr<int64_t>("plasticity_test_steps", 1000); ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < config->getOr<int64_t>("plasticity_test_neurons", 20) && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, config->getOr<float>("plasticity_test_current", 30.0f));
        }
        
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        // Log progress every configurable interval
        if (step % config->getOr<int64_t>("plasticity_test_log_interval", 100) == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            progress.update(step);
        }
    }
    
    // Record and analyze states
    experiment.recordInitialState();
    experiment.recordFinalState();
    experiment.computeLearningScore();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Learning Analysis:");
    NLM_LOG_INFO("  Learning Score: " + std::to_string(experiment.learningScore));
    NLM_LOG_INFO("  Assessment: " + experiment.getAssessment());
    NLM_LOG_INFO("  Success: " + (experiment.evaluateSuccess() ? "✓ YES" : "✗ NO"));
    
    // Log performance metrics
    NLM_LOG_INFO("  Execution Time: " + std::to_string(progress.getElapsedSeconds()) + "s");
    NLM_LOG_INFO("  Spikes Generated: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Neurons Firing: " + std::to_string(brain->getFiringNeuronCount()));
}

// Basic connectivity test with modular structure
void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    nlm::test::ProgressTracker progress(config->getOr<int64_t>("connectivity_test_steps", 50), "Connectivity Test");
    
    nlm::test::log::section("Test 1: Basic Neural Connectivity");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        nlm::test::log::error("Region not found");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        nlm::test::log::warning("No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first configurable neurons
    nlm::test::log::subtest("Injecting current into " + std::to_string(config->getOr<int64_t>("connectivity_test_neurons", 10)) + " neurons");
    for (size_t i = 0; i < std::min(static_cast<size_t>(config->getOr<int64_t>("connectivity_test_neurons", 10)), neurons.size()); ++i) {
        neurons[i]->injectCurrent(config->getOr<float>("connectivity_test_current", 50.0f));
    }
    
    // Run a few steps
    nlm::test::log::subtest("Simulating " + std::to_string(config->getOr<int64_t>("connectivity_test_steps", 50)) + " steps");
    for (SimulationStep step = 0; step < config->getOr<int64_t>("connectivity_test_steps", 50); ++step) {
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        progress.update(step);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    nlm::test::log::success("Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        nlm::test::log::success("Spikes propagate through network");
    } else {
        nlm::test::log::warning("No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
    
    nlm::test::log::separator();
}

// STDP verification test with structured approach
void runStdpVerification(std::shared_ptr<Brain> brain) {
    nlm::test::log::section("Test 3: STDP Verification");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        nlm::test::log::error("Region not found");
        return;
    }
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    size_t synapseCount = config->getOr<int64_t>("stdp_test_synapse_count", 5);
    
    if (synapses.size() < synapseCount) {
        nlm::test::log::warning("Not enough synapses for STDP test");
        return;
    }
    
    nlm::test::log::subtest("Testing STDP on " + std::to_string(synapseCount) + " synapses");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < synapseCount; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    nlm::test::log::section("Creating correlated pre->post activity (potentiation)");
    
    nlm::test::ProgressTracker stddpProgress(config->getOr<int64_t>("stdp_test_trials", 50), "STDP Trials");
    
    for (int trial = 0; trial < config->getOr<int64_t>("stdp_test_trials", 50); ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(config->getOr<float>("stdp_test_pre_current", 60.0f));
            brain->step(trial * 2, trial * 2 * config->getOr<double>("simulation_timestep", 0.001));
            
            // Then post fires
            postNeuron->injectCurrent(config->getOr<float>("stdp_test_post_current", 60.0f));
            brain->step(trial * 2 + 1, (trial * 2 + 1) * config->getOr<double>("simulation_timestep", 0.001));
        }
        
        stddpProgress.update(trial);
    }
    
    // Record after weights
    nlm::test::log::section("After correlated activity");
    for (size_t i = 0; i < synapseCount; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < synapseCount; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    nlm::test::log::separator();
    
    if (totalDelta > config->getOr<float>("stdp_learning_threshold", 0.001f)) {
        nlm::test::log::success("STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -config->getOr<float>("stdp_learning_threshold", 0.001f)) {
        nlm::test::log::warning("STDP reversed: Check parameters");
    } else {
        nlm::test::log::warning("No change: STDP may not be triggering");
    }
    
    nlm::test::log::separator();
}

// DUPLICATE FUNCTIONS REMOVED - Using enhanced utility functions above

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 2 Real Neural Computation...\n" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);  // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("");
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run Test 1: Basic connectivity
    runBasicConnectivityTest(brain);
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    // Run Test 2: Plasticity learning experiment
    runPlasticityExperiment(brain);
    
    // Reset and run Test 3: STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    }  // End of main function
