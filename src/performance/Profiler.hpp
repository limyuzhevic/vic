#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/AdvancedConfig.hpp"

namespace nlm {

// Performance measurement utilities for NLM
class PerformanceProfiler {
public:
    struct Measurement {
        std::string operation;
        double duration;           // in milliseconds
        size_t memoryUsage;       // in bytes
        size_t neuronCount;       // number of neurons involved
        double spikeCount;        // number of spikes
        double learningRate;      // learning rate during operation
        double plasticityLevel;   // plasticity level during operation
        double temperature;       // system temperature (if applicable)
        std::chrono::high_resolution_clock::time_point timestamp;
        
        Measurement(const std::string& op, double durationMs = 0.0,
                   size_t memoryBytes = 0, size_t neurons = 0,
                   double spikes = 0.0, double lr = 0.0, double plasticity = 0.0,
                   double temp = 0.0)
            : operation(op), duration(durationMs), memoryUsage(memoryBytes),
              neuronCount(neurons), spikeCount(spikes), learningRate(lr),
              plasticityLevel(plasticity), temperature(temp) {
            timestamp = std::chrono::high_resolution_clock::now();
        }
    };
    
    struct BenchmarkResult {
        std::string benchmarkName;
        double totalTime;          // in milliseconds
        double avgTimePerStep;     // in milliseconds
        double minTime;            // in milliseconds
        double maxTime;            // in milliseconds
        double stdDev;             // standard deviation
        size_t totalNeurons;       // average number of neurons
        size_t totalSynapses;      // average number of synapses
        double totalSpikes;        // total spikes generated
        double finalLearningRate;  // final learning rate
        double finalPlasticity;    // final plasticity level
        std::vector<Measurement> measurements;
        
        // Performance metrics
        double performanceScore;   // composite performance score
        double efficiencyScore;    // efficiency score (spikes per ms)
        double scalabilityScore;   // scalability score (neuron count per ms)
    };
    
    struct ProfileResult {
        std::string profileName;
        double totalTime;          // in milliseconds
        double cpuUsage;           // percentage (0-100)
        double memoryUsage;        // in MB
        double neuronActivity;     // average neuron activity level
        double networkEfficiency;  // network efficiency score
        double learningProgress;   // learning progress score
        std::vector<Measurement> measurements;
        
        // Detailed metrics
        std::unordered_map<std::string, double> metrics;
        
        std::string toString() const {
            std::ostringstream oss;
            oss << "Profile: " << profileName << "\n";
            oss << "  Total Time: " << std::fixed << std::setprecision(2) << totalTime << " ms\n";
            oss << "  CPU Usage: " << std::fixed << std::setprecision(1) << cpuUsage << "%\n";
            oss << "  Memory Usage: " << std::fixed << std::setprecision(2) << memoryUsage << " MB\n";
            oss << "  Neuron Activity: " << std::fixed << std::setprecision(3) << neuronActivity << "\n";
            oss << "  Network Efficiency: " << std::fixed << std::setprecision(3) << networkEfficiency << "\n";
            oss << "  Learning Progress: " << std::fixed << std::setprecision(3) << learningProgress << "\n";
            return oss.str();
        }
    };
    
    // Timer for measuring operation duration
    class Timer {
    public:
        Timer() : startTime_(std::chrono::high_resolution_clock::now()) {}
        
        double elapsedMs() const {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
            return static_cast<double>(duration.count());
        }
        
        void reset() {
            startTime_ = std::chrono::high_resolution_clock::now();
        }
        
    private:
        std::chrono::high_resolution_clock::time_point startTime_;
    };
    
    PerformanceProfiler() = default;
    ~PerformanceProfiler() = default;
    
    // Disable copying
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    
    // Start profiling a brain simulation
    void startBrainProfiling(const std::shared_ptr<Brain>& brain, const std::string& profileName);
    
    // Stop profiling
    ProfileResult stopBrainProfiling(const std::shared_ptr<Brain>& brain);
    
    // Start benchmarking an operation
    void startBenchmark(const std::string& benchmarkName, const std::shared_ptr<Brain>& brain = nullptr);
    
    // Stop benchmarking
    BenchmarkResult stopBenchmark(const std::string& benchmarkName, const std::shared_ptr<Brain>& brain = nullptr);
    
    // Record a measurement
    void recordMeasurement(const std::string& profileName, const Measurement& measurement);
    
    // Get current profile result
    ProfileResult getCurrentProfile() const;
    
    // Get benchmark result
    BenchmarkResult getBenchmark(const std::string& benchmarkName) const;
    
    // Get all profiles
    std::vector<ProfileResult> getAllProfiles() const;
    
    // Get all benchmarks
    std::vector<BenchmarkResult> getAllBenchmarks() const;
    
    // Clear all data
    void clear();
    
    // Save profile to file
    bool saveProfileToFile(const std::string& filename, const std::string& profileName) const;
    
    // Load profile from file
    bool loadProfileFromFile(const std::string& filename, std::string& profileName);
    
    // Save all profiles to file
    bool saveAllProfilesToFile(const std::string& filename) const;
    
    // Load all profiles from file
    bool loadAllProfilesFromFile(const std::string& filename);
    
    // Get performance statistics
    std::string getPerformanceStatistics() const;
    
    // Compare profiles
    std::string compareProfiles(const std::vector<std::string>& profileNames) const;
    
    // Get optimization recommendations
    std::string getOptimizationRecommendations() const;
    
    // Reset profiling for a specific profile
    void resetProfile(const std::string& profileName);
    
    // Export profile data for analysis
    std::string exportProfileData(const std::string& profileName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    static double calculateStandardDeviation(const std::vector<double>& values);
    static double calculateMean(const std::vector<double>& values);
    static double calculateEfficiencyScore(const Measurement& measurement);
    static double calculateScalabilityScore(const Measurement& measurement);
    static double calculatePerformanceScore(const Measurement& measurement);
    
    // Calculate system metrics
    static double calculateCPUMetrics(const ProfileResult& profile);
    static double calculateMemoryMetrics(const ProfileResult& profile);
    static double calculateActivityMetrics(const ProfileResult& profile);
    static double calculateEfficiencyMetrics(const ProfileResult& profile);
};

// High-level benchmarking functions
class NLMBenchmarker {
public:
    NLMBenchmarker() = default;
    ~NLMBenchmarker() = default;
    
    // Disable copying
    NLMBenchmarker(const NLMBenchmarker&) = delete;
    NLMBenchmarker& operator=(const NLMBenchmarker&) = delete;
    
    // Benchmark brain initialization
    static BenchmarkResult benchmarkBrainInitialization(const AdvancedConfig& config);
    
    // Benchmark step execution
    static BenchmarkResult benchmarkStepExecution(const std::shared_ptr<Brain>& brain, 
                                                   size_t steps, const std::string& name = "step_execution");
    
    // Benchmark learning performance
    static BenchmarkResult benchmarkLearningPerformance(const std::shared_ptr<Brain>& brain,
                                                        size_t steps, const std::string& name = "learning_performance");
    
    // Benchmark memory usage
    static BenchmarkResult benchmarkMemoryUsage(const std::shared_ptr<Brain>& brain,
                                                 size_t steps, const std::string& name = "memory_usage");
    
    // Benchmark plasticity
    static BenchmarkResult benchmarkPlasticity(const std::shared_ptr<Brain>& brain,
                                                size_t steps, const std::string& name = "plasticity");
    
    // Benchmark sensory processing
    static BenchmarkResult benchmarkSensoryProcessing(const std::shared_ptr<Brain>& brain,
                                                      size_t steps, const std::string& name = "sensory_processing");
    
    // Benchmark motor output
    static BenchmarkResult benchmarkMotorOutput(const std::shared_ptr<Brain>& brain,
                                                size_t steps, const std::string& name = "motor_output");
    
    // Benchmark complete simulation
    static BenchmarkResult benchmarkCompleteSimulation(const std::shared_ptr<Brain>& brain,
                                                       size_t steps, const std::string& name = "complete_simulation");
    
    // Run all benchmarks
    static std::vector<BenchmarkResult> runAllBenchmarks(const std::shared_ptr<Brain>& brain, size_t steps);
    
    // Compare benchmark results
    static std::string compareBenchmarkResults(const std::vector<BenchmarkResult>& results);
    
    // Get benchmark summary
    static std::string getBenchmarkSummary(const std::vector<BenchmarkResult>& results);
};

// Profiling utilities for agent brain
class AgentProfiler {
public:
    AgentProfiler() = default;
    ~AgentProfiler() = default;
    
    // Profile agent sensory processing
    static ProfileResult profileAgentSensoryProcessing(const std::shared_ptr<AgentBrain>& agent,
                                                       const std::shared_ptr<Brain>& brain,
                                                       size_t steps, const std::string& name = "agent_sensory_processing");
    
    // Profile agent motor output
    static ProfileResult profileAgentMotorOutput(const std::shared_ptr<AgentBrain>& agent,
                                                const std::shared_ptr<Brain>& brain,
                                                size_t steps, const std::string& name = "agent_motor_output");
    
    // Profile agent learning
    static ProfileResult profileAgentLearning(const std::shared_ptr<AgentBrain>& agent,
                                              const std::shared_ptr<Brain>& brain,
                                              size_t steps, const std::string& name = "agent_learning");
    
    // Profile agent development
    static ProfileResult profileAgentDevelopment(const std::shared_ptr<AgentBrain>& agent,
                                                const std::shared_ptr<Brain>& brain,
                                                size_t steps, const std::string& name = "agent_development");
    
    // Profile complete agent simulation
    static ProfileResult profileCompleteAgentSimulation(const std::shared_ptr<AgentBrain>& agent,
                                                       const std::shared_ptr<Brain>& brain,
                                                       size_t steps, const std::string& name = "complete_agent_simulation");
    
    // Compare agent profiles
    static std::string compareAgentProfiles(const std::vector<ProfileResult>& profiles);
    
    // Get agent profiling statistics
    static std::string getAgentProfilingStatistics(const std::vector<ProfileResult>& profiles);
};

// Performance analysis utilities
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer() = default;
    ~PerformanceAnalyzer() = default;
    
    // Analyze performance bottlenecks
    static std::string analyzePerformanceBottlenecks(const ProfileResult& profile);
    
    // Analyze scalability
    static std::string analyzeScalability(const std::vector<BenchmarkResult>& benchmarks);
    
    // Analyze learning efficiency
    static std::string analyzeLearningEfficiency(const ProfileResult& profile);
    
    // Analyze memory efficiency
    static std::string analyzeMemoryEfficiency(const ProfileResult& profile);
    
    // Generate performance report
    static std::string generatePerformanceReport(const ProfileResult& profile,
                                                 const std::vector<BenchmarkResult>& benchmarks);
    
    // Identify optimization opportunities
    static std::vector<std::string> identifyOptimizationOpportunities(const ProfileResult& profile,
                                                                       const std::vector<BenchmarkResult>& benchmarks);
    
    // Calculate performance score
    static double calculatePerformanceScore(const ProfileResult& profile,
                                           const std::vector<BenchmarkResult>& benchmarks);
    
    // Rank configurations by performance
    static std::vector<std::pair<std::string, double>> rankConfigurations(
        const std::vector<std::pair<std::string, ProfileResult>>& profiles,
        const std::vector<BenchmarkResult>& benchmarks);
};

} // namespace nlm
