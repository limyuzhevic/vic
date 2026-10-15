// Experiment runner for CLI
// Manages predefined experiments and benchmark tests

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "cli_parser.hpp"

namespace nlm {

// Experiment metadata
struct ExperimentInfo {
    std::string name;
    std::string description;
    size_t defaultSteps;
    std::string configFile;
    bool requiresAgent;
    bool requiresWorld;
    
    ExperimentInfo(const std::string& n, const std::string& desc, size_t steps,
                  const std::string& config = "", bool agent = false, 
                  bool world = false)
        : name(n), description(desc), defaultSteps(steps), configFile(config),
          requiresAgent(agent), requiresWorld(world) {}
};

// Experiment results
struct ExperimentResult {
    std::string experimentName;
    size_t stepsCompleted;
    size_t totalSteps;
    size_t spikesGenerated;
    float averageFiringRate;
    double executionTime;
    bool success;
    std::string errorMessage;
    std::map<std::string, std::string> metrics;
    
    ExperimentResult(const std::string& name, size_t total)
        : experimentName(name), stepsCompleted(0), totalSteps(total),
          spikesGenerated(0), averageFiringRate(0.0f), executionTime(0.0),
          success(false) {}
};

// Benchmark metrics
struct BenchmarkResult {
    std::string benchmarkName;
    double executionTime;
    size_t stepsPerSecond;
    size_t totalSpikes;
    float averageFiringRate;
    std::map<std::string, double> performanceMetrics;
    
    BenchmarkResult(const std::string& name)
        : benchmarkName(name), executionTime(0.0), stepsPerSecond(0),
          totalSpikes(0), averageFiringRate(0.0f) {}
};

// Experiment runner for CLI
class ExperimentRunner {
public:
    ExperimentRunner(std::shared_ptr<Config> config);
    ~ExperimentRunner();
    
    // Run predefined experiment
    CommandResult runExperiment(const std::string& name, size_t steps,
                               const std::string& config = "");
    
    // Run benchmark test
    CommandResult runBenchmark(const std::string& name, size_t steps = 1000);
    
    // Validate configuration and system integrity
    CommandResult validate() const;
    
    // Get available experiments
    std::vector<ExperimentInfo> getAvailableExperiments() const;
    
    // Get available benchmarks
    std::vector<std::string> getAvailableBenchmarks() const;
    
private:
    std::shared_ptr<Config> config_;
    
    // Run Phase 3 experiment
    CommandResult runPhase3Experiment(size_t steps);
    
    // Run Phase 4 experiment
    CommandResult runPhase4Experiment(size_t steps);
    
    // Run Phase 5 experiment
    CommandResult runPhase5Experiment(size_t steps);
    
    // Run Phase 6 experiment
    CommandResult runPhase6Experiment(size_t steps);
    
    // Run basic connectivity test
    CommandResult runBasicConnectivityTest(std::shared_ptr<Brain> brain, size_t steps);
    
    // Run plasticity experiment
    CommandResult runPlasticityExperiment(std::shared_ptr<Brain> brain, size_t steps);
    
    // Run STDP verification
    CommandResult runStdpVerification(std::shared_ptr<Brain> brain, size_t steps);
    
    // Get experiment metadata
    ExperimentInfo getPhase3ExperimentInfo() const;
    ExperimentInfo getPhase4ExperimentInfo() const;
    ExperimentInfo getPhase5ExperimentInfo() const;
    ExperimentInfo getPhase6ExperimentInfo() const;
    
    // Get benchmark metadata
    std::string getBasicBenchmarkName() const { return "basic_connectivity"; }
    std::string getPlasticityBenchmarkName() const { return "plasticity_learning"; }
    std::string getStdpBenchmarkName() const { return "stdp_verification"; }
    
    // Get experiment by name
    ExperimentInfo getExperimentByName(const std::string& name) const;
    
    // Get benchmark by name
    std::string getBenchmarkByName(const std::string& name) const;
};

} // namespace nlm