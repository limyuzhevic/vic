#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <unordered_set>
#include "brain/Brain.hpp"
#include "core/Types/Types.hpp"
#include "dynamics/SpikeSystem.hpp"

namespace nlm {

enum class StateChangeType {
    NoChange,
    GradualChange,
    SuddenChange,
    CyclicChange,
    BurstChange,
    AnomalousChange,
    RecoveryChange,
    Stabilization
};

class StateAnalyzer {
public:
    struct StateSnapshot {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::vector<double> neuronStates;
        std::vector<double> synapseStates;
        std::vector<size_t> spikeCounts;
        double globalEnergy;
        double averageFiringRate;
        std::map<NeuronId, double> neuronActivities;
        std::map<SynapseId, double> synapseWeights;
    };
    
    struct StateChange {
        std::chrono::high_resolution_clock::time_point timestamp;
        StateChangeType type;
        double magnitude;
        std::string description;
    };
    
    struct EvolutionMetrics {
        double totalChangeMagnitude;
        double stabilityScore;
        double changeFrequency;
        double volatility;
        std::vector<double> stateDeltas;
    };
    
    static std::unique_ptr<StateAnalyzer> create() {
        return std::make_unique<StateAnalyzer>();
    }
    
    // State comparison and tracking
    StateSnapshot captureState(const Brain& brain) const;
    std::vector<StateChange> compareStates(const StateSnapshot& oldState, 
                                           const StateSnapshot& newState) const;
    EvolutionMetrics trackEvolution(const std::vector<StateSnapshot>& states) const;
    
    // State anomaly detection
    std::vector<StateChange> detectAnomalies(const std::vector<StateSnapshot>& states) const;
    std::vector<size_t> findOutliers(const std::vector<StateSnapshot>& states, 
                                     double threshold) const;
    
    // State evolution analysis
    std::vector<double> analyzeStateTransitions(const std::vector<StateSnapshot>& states) const;
    double calculateStateStability(const StateSnapshot& state) const;
    double calculateStateVolatility(const std::vector<StateSnapshot>& states) const;
    
    // State visualization data
    std::vector<double> extractStateFeatures(const StateSnapshot& state) const;
    std::vector<std::vector<double>> generateStateSpaceTrajectories(const std::vector<StateSnapshot>& states) const;
    
    // State analysis export
    bool exportStateSnapshots(const std::string& filepath, 
                             const std::vector<StateSnapshot>& snapshots) const;
    bool exportStateChanges(const std::string& filepath, 
                           const std::vector<StateChange>& changes) const;
    bool exportEvolutionMetrics(const std::string& filepath, 
                                const EvolutionMetrics& metrics) const;
    
private:
    StateAnalyzer() = default;
    
    // Helper methods
    void extractNeuronStates(const Brain& brain, std::vector<double>& states) const;
    void extractSynapseStates(const Brain& brain, std::vector<double>& states) const;
    void extractSpikeCounts(const Brain& brain, std::vector<size_t>& counts) const;
    double calculateGlobalEnergy(const Brain& brain) const;
    StateChangeType determineChangeType(const StateSnapshot& oldState, 
                                       const StateSnapshot& newState) const;
};

} // namespace nlm