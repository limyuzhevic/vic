#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <unordered_map>
#include <functional>
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "../core/Config/Config.hpp"
#include "Experiment.hpp"
#include "ExperimentRunner.hpp"
#include "Metrics.hpp"

namespace nlm {

// Experiment types
enum class ExperimentType {
    Connectivity,
    Plasticity,
    STDP,
    All,
    Custom,
    Phase3,
    Phase4,
    Phase5,
    Phase6
};

// Brain state for saving/loading
struct BrainState {
    std::vector<float> neuronMembranePotentials;
    std::vector<float> neuronThresholds;
    std::vector<std::vector<float>> synapseWeights;
    std::vector<bool> synapsePlasticity;
    size_t totalNeuronCount;
    size_t totalSynapseCount;
    uint64_t totalSpikeCount;
    double simulationTime;
};

// Experiment configuration
struct ExperimentConfig {
    std::string name;
    ExperimentType type;
    uint64_t seed;
    SimulationStep maxSteps;
    std::string outputPath;
    bool enableProgressTracking;
    bool enableMetrics;
    bool saveBrainState;
    std::map<std::string, std::string> parameters;
    std::vector<std::string> additionalOptions;
};

// Experiment result
struct ExperimentResult {
    std::string experimentName;
    ExperimentType experimentType;
    bool success;
    std::string errorMessage;
    double executionTime;
    std::shared_ptr<Experiment> experimentData;
    std::map<std::string, double> metrics;
    std::string summary;
};

// Experiment manager - runs and manages different experiment types
class ExperimentManager {
public:
    ExperimentManager();
    ~ExperimentManager();
    
    // Configure the manager
    void setConfig(const Config& config);
    
    // Run specific experiment
    ExperimentResult runExperiment(const std::string& name, const ExperimentConfig& config);
    
    // Run experiment by type
    ExperimentResult runExperimentByType(ExperimentType type, const ExperimentConfig& config);
    
    // Run multiple experiments
    std::vector<ExperimentResult> runExperiments(const std::vector<ExperimentConfig>& configs);
    
    // Interactive mode
    void runInteractiveMode(Config& config);
    
    // Batch mode
    std::vector<ExperimentResult> runBatchMode(const std::string& batchConfigFile);
    
    // Save brain state to file
    bool saveBrainState(const std::string& filepath, const std::shared_ptr<Brain>& brain);
    
    // Load brain state from file
    std::shared_ptr<Brain> loadBrainState(const std::string& filepath, const Config& config);
    
    // Get available experiment types
    std::vector<ExperimentType> getAvailableExperimentTypes() const;
    
    // Get experiment type name
    std::string getExperimentTypeName(ExperimentType type) const;
    
    // Show help information
    void showHelp() const;
    
    // Progress tracking
    class ProgressTracker {
    public:
        ProgressTracker(size_t total, bool enableColors = true);
        void update(size_t current, const std::string& message = "");
        void complete();
    
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private helper methods
    ExperimentResult runConnectivityExperiment(const ExperimentConfig& config);
    ExperimentResult runPlasticityExperiment(const ExperimentConfig& config);
    ExperimentResult runStdpExperiment(const ExperimentConfig& config);
    ExperimentResult runPhase3Experiment(const ExperimentConfig& config);
    ExperimentResult runPhase4Experiment(const ExperimentConfig& config);
    ExperimentResult runPhase5Experiment(const ExperimentConfig& config);
    ExperimentResult runPhase6Experiment(const ExperimentConfig& config);
    
    BrainState brainToState(const std::shared_ptr<Brain>& brain);
    std::shared_ptr<Brain> stateToBrain(const BrainState& state, const Config& config);
    
    void recordExperimentMetrics(ExperimentResult& result, const std::shared_ptr<Brain>& brain);
    std::string generateExperimentSummary(const ExperimentResult& result) const;
};

} // namespace nlm