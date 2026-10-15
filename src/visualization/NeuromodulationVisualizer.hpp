#pragma once

#include "VisualizationInterface.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <iomanip>
#include <sstream>

namespace nlm {

// Neuromodulator state visualization
struct NeuromodulatorState {
    NeuromodulatorType type;
    float level;              // Current concentration level (0-1)
    float targetLevel;        // Target level for homeostasis
    float rate;               // Rate of change
    float effect;             // Current effect on brain
    float threshold;          // Threshold for activation
    SimulationStep lastUpdated;
    std::string status;       // Current status (e.g., "stable", "increasing", "decreasing")
    
    NeuromodulatorState() : type(NeuromodulatorType::Dopamine), level(0.0f), targetLevel(0.5f),
                           rate(0.0f), effect(0.0f), threshold(0.1f), lastUpdated(0), 
                           status("stable") {}
    NeuromodulatorState(NeuromodulatorType t) : type(t), level(0.0f), targetLevel(0.5f),
                                              rate(0.0f), effect(0.0f), threshold(0.1f), lastUpdated(0),
                                              status("stable") {}
};

// Neuromodulation network visualization
struct NeuromodulationNetwork {
    std::vector<NeuromodulatorType> activeNeuromodulators;
    std::unordered_map<NeuromodulatorType, float> interactionWeights;
    std::unordered_map<NeuromodulatorType, float> crossEffects;
    float networkActivity;    // Overall network activity level
    float homeostasis;       // Network homeostasis level
    
    NeuromodulationNetwork() : networkActivity(0.0f), homeostasis(1.0f) {}
};

// Neuromodulation visualization
class NeuromodulationVisualizer : public VisualizationInterface {
public:
    NeuromodulationVisualizer();
    ~NeuromodulationVisualizer() override;
    
    // Initialize visualization
    bool initialize() override;
    
    // Update visualization data
    void update(double deltaTime) override;
    
    // Render current state
    void render() override;
    
    // Close visualization
    void close() override;
    
    // Set neuromodulation system reference
    void setNeuromodulationSystem(std::shared_ptr<NeuromodulationSystem> neuromodulation);
    
    // Update neuromodulator state
    void updateNeuromodulator(NeuromodulatorType type, float level, float effect);
    
    // Add neuromodulator to network
    void addNeuromodulator(NeuromodulatorType type);
    
    // Set neuromodulation interaction
    void setInteraction(NeuromodulatorType source, NeuromodulatorType target, float weight);
    
    // Get neuromodulator states
    const std::unordered_map<NeuromodulatorType, NeuromodulatorState>& getNeuromodulatorStates() const;
    
    // Get network visualization data
    const NeuromodulationNetwork& getNetwork() const;
    
    // Generate neuromodulator level visualization
    std::string generateLevelVisualization() const;
    
    // Generate interaction network visualization
    std::string generateNetworkVisualization() const;
    
    // Generate homeostasis visualization
    std::string generateHomeostasisVisualization() const;
    
    // Get neuromodulation performance metrics
    std::string getNeuromodulationMetrics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
