// Performance Profiling Tools for NLM
// Comprehensive benchmarking and profiling utilities

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <memory>
#include <string>
#include <algorithm>
#include <iomanip>

namespace nlm {

class PerformanceProfiler {
public:
    struct BenchmarkResult {
        std::string name;
        double timeSeconds;
        size_t neuronCount;
        size_t stepCount;
        float memoryUsageMB;
        float cpuUsagePercent;
        std::string description;
    };
    
    struct ProfileData {
        std::vector<BenchmarkResult> benchmarks;
        double totalTime;
        size_t totalSteps;
        std::string sessionInfo;
    };
    
private:
    std::vector<BenchmarkResult> results_;
    std::chrono::high_resolution_clock::time_point sessionStart_;
    std::string sessionName_;
    
public:
    PerformanceProfiler(const std::string& name = "NLM Performance Session")
        : sessionName_(name) {
        sessionStart_ = std::chrono::high_resolution_clock::now();
        std::cout << "Starting performance profiling session: " << name << std::endl;
    }
    
    ~PerformanceProfiler() {
        endSession();
    }
    
    // Benchmark brain initialization
    BenchmarkResult benchmarkBrainInitialization(std::shared_ptr<Config> config, 
                                                 const std::string& description = "Brain initialization") {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        BenchmarkResult result;
        result.name = "brain_initialization";
        result.timeSeconds = duration.count() / 1000.0;
        result.neuronCount = config->getOr<int64_t>("neuron_count", 100);
        result.stepCount = 0;
        result.memoryUsageMB = 0.0f;  // Would need system monitoring
        result.cpuUsagePercent = 0.0f;  // Would need system monitoring
        result.description = description;
        
        if (success) {
            results_.push_back(result);
            std::cout << "✓ " << description << ": " << result.timeSeconds << "s" << std::endl;
        } else {
            std::cerr << "✗ " << description << ": FAILED" << std::endl;
        }
        
        return result;
    }
    
    // Benchmark simulation steps
    BenchmarkResult benchmarkSimulationSteps(std::shared_ptr<Config> config, 
                                            size_t steps,
                                            const std::string& description = "Simulation steps") {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Optional: Create agent and world for more realistic profiling
        nlm::SimpleWorld world;
        world.configure(config->getOr<int64_t>("vision_width", 16),
                       config->getOr<int64_t>("vision_height", 16),
                       config->getOr<int64_t>("vision_width", 16),
                       config->getOr<int64_t>("vision_height", 16));
        
        nlm::AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Run simulation
        for (size_t step = 0; step < steps; ++step) {
            // Get sensory input (if world is configured)
            if (config->getOr<bool>("enable_sensory", true)) {
                auto percept = world.getSensoryPercept();
                agentBrain.processSensoryInput(percept);
                
                auto action = agentBrain.decodeMotorCommand();
                world.applyMotorCommand(action, step * config->getOr<double>("simulation_timestep", 0.001));
                
                agentBrain.applyRewardModulation(0.1f, agentBrain.getPredictionError());
                agentBrain.updateDevelopment(config->getOr<double>("simulation_timestep", 0.001));
            }
            
            // Core brain simulation
            brain.step(step);
            
            // Progress reporting for long benchmarks
            if (steps > 100 && step % 100 == 0) {
                std::cout << "  Progress: " << (step * 100.0 / steps) << "% completed" << std::endl;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        BenchmarkResult result;
        result.name = "simulation_steps";
        result.timeSeconds = duration.count() / 1000.0;
        result.neuronCount = config->getOr<int64_t>("neuron_count", 100);
        result.stepCount = steps;
        result.memoryUsageMB = 0.0f;
        result.cpuUsagePercent = 0.0f;
        result.description = description;
        
        results_.push_back(result);
        std::cout << "✓ " << description << ": " << result.timeSeconds << "s for " << steps << " steps" << std::endl;
        std::cout << "  Performance: " << (steps / result.timeSeconds) << " steps/second" << std::endl;
        
        return result;
    }
    
    // Benchmark memory operations
    BenchmarkResult benchmarkMemoryOperations(std::shared_ptr<Config> config,
                                             size_t memorySteps,
                                             const std::string& description = "Memory operations") {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Create agent for memory integration
        nlm::SimpleWorld world;
        nlm::AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Run memory-intensive operations
        for (size_t step = 0; step < memorySteps; ++step) {
            // Process sensory input
            auto percept = world.getSensoryPercept();
            agentBrain.processSensoryInput(percept);
            
            // Apply reward to trigger memory consolidation
            float reward = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
            agentBrain.applyRewardModulation(reward, agentBrain.getPredictionError());
            
            // Update development for memory changes
            agentBrain.updateDevelopment(0.001);
            
            brain.step(step);
            
            // Store memory periodically
            if (step % 10 == 0) {
                // This would actually trigger memory storage in a real scenario
                // For profiling, we simulate the operation
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        BenchmarkResult result;
        result.name = "memory_operations";
        result.timeSeconds = duration.count() / 1000.0;
        result.neuronCount = config->getOr<int64_t>("neuron_count", 100);
        result.stepCount = memorySteps;
        result.memoryUsageMB = 0.0f;
        result.cpuUsagePercent = 0.0f;
        result.description = description;
        
        results_.push_back(result);
        std::cout << "✓ " << description << ": " << result.timeSeconds << "s for " << memorySteps << " steps" << std::endl;
        
        return result;
    }
    
    // Benchmark plasticity learning
    BenchmarkResult benchmarkPlasticityLearning(std::shared_ptr<Config> config,
                                                size_t learningCycles,
                                                const std::string& description = "Plasticity learning") {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Get regions for plasticity testing
        auto regions = brain.getRegions();
        if (regions.empty()) {
            std::cerr << "✗ " << description << ": No regions available for plasticity testing" << std::endl;
            return BenchmarkResult{"", 0.0, 0, 0, 0.0f, 0.0f, "FAILED: No regions"};
        }
        
        // Enable plasticity on all synapses
        for (const auto& region : regions) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Hebbian and STDP
            }
        }
        
        // Run learning cycles
        float totalLearning = 0.0f;
        for (size_t cycle = 0; cycle < learningCycles; ++cycle) {
            // Create correlated activity pattern
            auto neurons = regions[0]->getAllNeurons();
            if (neurons.size() >= 2) {
                // Pre-post spike pattern for LTP
                neurons[0]->injectCurrent(60.0f);
                brain.step(cycle * 2, cycle * 2 * 0.001);
                
                neurons[1]->injectCurrent(60.0f);
                brain.step(cycle * 2 + 1, (cycle * 2 + 1) * 0.001);
                
                // More correlated activity
                for (int i = 0; i < 5; ++i) {
                    neurons[0]->injectCurrent(30.0f);
                    brain.step(cycle * 10 + i, (cycle * 10 + i) * 0.001);
                    
                    neurons[1]->injectCurrent(30.0f);
                    brain.step(cycle * 10 + i + 1, (cycle * 10 + i + 1) * 0.001);
                }
            }
            
            totalLearning += brain.getAverageFiringRate();
            brain.step(cycle * 20);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        BenchmarkResult result;
        result.name = "plasticity_learning";
        result.timeSeconds = duration.count() / 1000.0;
        result.neuronCount = config->getOr<int64_t>("neuron_count", 100);
        result.stepCount = learningCycles * 20;
        result.memoryUsageMB = 0.0f;
        result.cpuUsagePercent = 0.0f;
        result.description = description;
        
        results_.push_back(result);
        std::cout << "✓ " << description << ": " << result.timeSeconds << "s, Learning rate: " 
                  << (totalLearning / learningCycles) << " Hz average" << std::endl;
        
        return result;
    }
    
    // Benchmark complete system integration
    BenchmarkResult benchmarkCompleteIntegration(std::shared_ptr<Config> config,
                                                const std::string& description = "Complete system integration") {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        nlm::Brain brain(config);
        bool brainInitialized = brain.initialize();
        
        if (!brainInitialized) {
            std::cerr << "✗ " << description << ": Brain initialization failed" << std::endl;
            return BenchmarkResult{"", 0.0, 0, 0, 0.0f, 0.0f, "FAILED: Brain initialization"};
        }
        
        // Create complete agent-world system
        nlm::SimpleWorld world;
        world.configure(100, 80, 20, 16);
        
        nlm::AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        world.setAgentStart(50, 40);
        
        // Run complete integration test
        size_t steps = config->getOr<int64_t>("integration_steps", 200);
        float totalReward = 0.0f;
        
        for (size_t step = 0; step < steps; ++step) {
            // Complete system loop
            auto percept = world.getSensoryPercept();
            agentBrain.processSensoryInput(percept);
            
            auto action = agentBrain.decodeMotorCommand();
            auto result = world.applyMotorCommand(action, step * config->getOr<double>("simulation_timestep", 0.001));
            totalReward += result.reward;
            
            agentBrain.applyRewardModulation(result.reward, agentBrain.getPredictionError());
            agentBrain.updateDevelopment(config->getOr<double>"simulation_timestep", 0.001));
            
            brain.step(step);
            
            // Report progress for long tests
            if (steps > 100 && step % 50 == 0) {
                std::cout << "  Integration progress: " << (step * 100.0 / steps) << "%" << std::endl;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        BenchmarkResult result;
        result.name = "complete_integration";
        result.timeSeconds = duration.count() / 1000.0;
        result.neuronCount = config->getOr<int64_t>("neuron_count", 100);
        result.stepCount = steps;
        result.memoryUsageMB = 0.0f;
        result.cpuUsagePercent = 0.0f;
        result.description = description;
        
        results_.push_back(result);
        std::cout << "✓ " << description << ": " << result.timeSeconds << "s, Total reward: " 
                  << totalReward << ""std::endl;
        
        return result;
    }
    
    // End profiling session and save results
    void endSession() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - sessionStart_);
        
        std::cout << std::endl;
        std::cout << "Performance profiling session completed: " << sessionName_ << std::endl;
        std::cout << "Total session time: " << duration.count() << " seconds" << std::endl;
        
        // Calculate session summary
        ProfileData profile;
        profile.benchmarks = results_;
        profile.totalTime = duration.count();
        profile.totalSteps = 0;
        
        for (const auto& result : results_) {
            profile.totalSteps += result.stepCount;
        }
        
        profile.sessionInfo = "NLM Performance Session: " + sessionName_;
        
        // Save results to file
        saveProfileData(profile, "nlm_performance_profile.txt");
        
        // Print summary table
        printProfileSummary(profile);
    }
    
    // Save profile data to file
    void saveProfileData(const ProfileData& profile, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not save profile data to " << filename << std::endl;
            return;
        }
        
        file << "NLM Performance Profile" << std::endl;
        file << "=======================" << std::endl;
        file << "Session: " << profile.sessionInfo << std::endl;
        file << "Total Time: " << profile.totalTime << "s" << std::endl;
        file << "Total Steps: " << profile.totalSteps << std::endl;
        file << std::endl;
        
        file << "Benchmark Results:" << std::endl;
        file << std::left << std::setw(30) << "Test Name" 
              << std::right << std::setw(12) << "Time (s)" 
              << std::right << std::setw(12) << "Neurons" 
              << std::right << std::setw(12) << "Steps" 
              << std::endl;
        file << std::string(78, '-') << std::endl;
        
        for (const auto& result : profile.benchmarks) {
            file << std::left << std::setw(30) << result.name
                 << std::right << std::setw(12) << std::fixed << std::setprecision(3) << result.timeSeconds
                 << std::right << std::setw(12) << result.neuronCount
                 << std::right << std::setw(12) << result.stepCount
                 << std::endl;
        }
        
        file.close();
        std::cout << "Profile data saved to: " << filename << std::endl;
    }
    
    // Print profile summary to console
    void printProfileSummary(const ProfileData& profile) {
        std::cout << std::endl;
        std::cout << "Performance Profile Summary:" << std::endl;
        std::cout << "=========================" << std::endl;
        
        double totalSessionTime = profile.totalTime;
        size_t totalSteps = profile.totalSteps;
        
        std::cout << std::left << std::setw(30) << "Metric" 
                  << std::right << std::setw(20) << "Value" << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        
        std::cout << std::left << std::setw(30) << "Total session time"
                  << std::right << std::setw(20) << profile.totalTime << "s" << std::endl;
        
        std::cout << std::left << std::setw(30) << "Total simulation steps"
                  << std::right << std::setw(20) << profile.totalSteps << std::endl;
        
        if (totalSessionTime > 0) {
            double stepsPerSecond = totalSteps / totalSessionTime;
            std::cout << std::left << std::setw(30) << "Steps per second"
                      << std::right << std::setw(20) << stepsPerSecond << std::endl;
            
            double avgTimePerStep = (totalSessionTime * 1000.0) / totalSteps;  // in milliseconds
            std::cout << std::left << std::setw(30) << "Average time per step"
                      << std::right << std::setw(20) << avgTimePerStep << "ms" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Benchmark Details:" << std::endl;
        std::cout << std::left << std::setw(30) << "Test Name" 
                  << std::right << std::setw(12) << "Time" 
                  << std::right << std::setw(12) << "Neurons" 
                  << std::right << std::setw(12) << "Steps"
                  << std::right << std::setw(15) << "Steps/s" << std::endl;
        std::cout << std::string(83, '-') << std::endl;
        
        for (const auto& result : profile.benchmarks) {
            double stepsPerSecond = (result.stepCount > 0) ? (result.stepCount / result.timeSeconds) : 0.0;
            std::cout << std::left << std::setw(30) << result.name
                      << std::right << std::setw(12) << std::fixed << std::setprecision(3) << result.timeSeconds
                      << std::right << std::setw(12) << result.neuronCount
                      << std::right << std::setw(12) << result.stepCount
                      << std::right << std::setw(15) << stepsPerSecond << std::endl;
        }
        
        std::cout << std::endl;
    }
};

// Convenience functions for common benchmarking scenarios
class BenchmarkSuite {
public:
    static void runQuickValidation(std::shared_ptr<Config> config) {
        PerformanceProfiler profiler("Quick Validation Benchmark");
        
        std::cout << "Running quick validation benchmark..." << std::endl;
        
        // Test brain creation and initialization
        auto initResult = profiler.benchmarkBrainInitialization(config, 
                                                                 "Quick brain initialization");
        
        // Test basic simulation
        auto simResult = profiler.benchmarkSimulationSteps(config, 100, 
                                                           "Quick simulation (100 steps)");
        
        std::cout << std::endl << "Quick validation complete!" << std::endl;
    }
    
    static void runComprehensiveBenchmark(std::shared_ptr<Config> config) {
        PerformanceProfiler profiler("Comprehensive NLM Benchmark");
        
        std::cout << "Running comprehensive benchmark..." << std::endl;
        std::cout << "This will take several minutes. Please wait." << std::endl;
        std::cout << std::endl;
        
        // Brain initialization benchmark
        profiler.benchmarkBrainInitialization(config, "Comprehensive initialization");
        
        // Basic simulation benchmark
        profiler.benchmarkSimulationSteps(config, 500, 
                                          "Medium-scale simulation (500 steps)");
        
        // Memory operations benchmark
        profiler.benchmarkMemoryOperations(config, 300, 
                                           "Memory-intensive operations (300 steps)");
        
        // Plasticity learning benchmark
        profiler.benchmarkPlasticityLearning(config, 50, 
                                             "Plasticity learning cycles (50 cycles)");
        
        // Complete integration benchmark
        profiler.benchmarkCompleteIntegration(config, 
                                              "Full system integration test");
        
        std::cout << std::endl << "Comprehensive benchmark complete!" << std::endl;
    }
    
    static void runScalabilityTest(std::shared_ptr<Config> config) {
        PerformanceProfiler profiler("NLM Scalability Test");
        
        std::cout << "Running scalability test..." << std::endl;
        
        // Test with different brain sizes
        std::vector<size_t> neuronCounts = {100, 500, 1000, 2000};
        
        for (size_t neuronCount : neuronCounts) {
            config->set("neuron_count", static_cast<int64_t>(neuronCount), 
                       nlm::ConfigSource::Default);
            
            std::string desc = "Scalability test: " + std::to_string(neuronCount) + " neurons";
            
            auto result = profiler.benchmarkSimulationSteps(config, 100, desc);
            
            // Small delay between tests
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        std::cout << std::endl << "Scalability test complete!" << std::endl;
    }
    
    static void runAdaptiveBenchmark(std::shared_ptr<Config> config) {
        PerformanceProfiler profiler("Adaptive NLM Benchmark");
        
        std::cout << "Running adaptive benchmark with parameter variations..." << std::endl;
        
        // Test different timesteps
        std::vector<double> timesteps = {0.001, 0.005, 0.01, 0.05};
        
        for (double timestep : timesteps) {
            config->set("simulation_timestep", timestep, nlm::ConfigSource::Default);
            
            std::string desc = "Timestep test: " + std::to_string(timestep) + "s";
            
            auto result = profiler.benchmarkSimulationSteps(config, 200, desc);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
        
        // Test different connection probabilities
        std::vector<float> conn_probs = {0.05f, 0.1f, 0.15f, 0.2f};
        
        for (float conn_prob : conn_probs) {
            config->set("connection_probability", conn_prob, nlm::ConfigSource::Default);
            
            std::string desc = "Connection probability test: " + std::to_string(conn_prob);
            
            auto result = profiler.benchmarkSimulationSteps(config, 150, desc);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
        
        std::cout << std::endl << "Adaptive benchmark complete!" << std::endl;
    }
};

} // namespace nlm