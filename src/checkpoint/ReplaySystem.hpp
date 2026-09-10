#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../brain/Brain.hpp"

namespace nlm {

// Simulation replay and analysis system
class ReplaySystem {
public:
    ReplaySystem();
    ~ReplaySystem();
    
    // Replay data structure
    struct ReplayData {
        std::vector<SimulationStep> steps;
        std::vector<Timestamp> times;
        std::vector<std::shared_ptr<SensoryInput>> sensoryInputs;
        std::vector<std::shared_ptr<Action>> actions;
        std::vector<float> rewards;
        std::vector<float> brainStates; // Flattened brain state vector
        std::map<std::string, std::vector<float>> additionalData;
        
        // Metadata
        uint64_t brainSize;        // Size of brain state vector
        uint64_t maxNeurons;       // Maximum neuron count
        uint64_t maxSynapses;      // Maximum synapse count
        std::string simulationConfig; // Configuration used
        std::chrono::system_clock::time_point timestamp; // Creation time
    };
    
    // Load replay data from file
    bool loadReplayData(const std::string& filepath);
    
    // Play replay in brain
    bool playReplay(std::shared_ptr<Brain> brain);
    
    // Set replay speed
    void setSpeed(double speed); // 1.0 = real-time, 2.0 = 2x speed, 0.5 = 0.5x speed
    double getSpeed() const;
    
    // Get replay information
    ReplayData getReplayData() const;
    size_t getReplayStepCount() const;
    Timestamp getReplayDuration() const;
    
    // Analyze replay
    struct ReplayAnalysis {
        double averageFiringRate;
        double firingRateVariability;
        double rewardDistributionMean;
        double rewardDistributionStdDev;
        size_t totalSpikes;
        float energyEfficiency;
        float behavioralConsistency;
        std::vector<float> actionFrequency;
        std::map<std::string, double> statisticalMetrics;
    };
    
    ReplayAnalysis analyzeReplay() const;
    
    // Extract features for machine learning
    std::vector<std::vector<float>> extractFeatures(const std::string& featureType = "basic") const;
    std::vector<float> extractTimeSeriesFeatures(size_t windowSize = 100) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
