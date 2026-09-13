// ============================================================================
// Brain Visualization Core Components
// ============================================================================

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "../brain/Brain.hpp"
#include "../memory/Memory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"

namespace nlm {

/**
 * @struct NeuralActivityData
 * @brief Container for neural activity metrics
 * 
 * This structure holds comprehensive neural activity data for visualization.
 */
struct NeuralActivityData {
    Timestamp timestamp;
    uint64_t step;
    
    // Neuron activity metrics
    uint32_t totalSpikes;
    uint32_t firingNeuronCount;
    double averageFiringRate;
    std::vector<NeuronId> highlyActiveNeurons;
    std::vector<float> neuronPotentials;
    
    // Synaptic activity
    uint32_t activeSynapses;
    std::vector<std::pair<NeuronId, NeuronId>> strongConnections;
    std::vector<float> synapticWeights;
    
    // Regional activity
    std::map<RegionId, double> regionalActivity;
    std::map<RegionId, uint32_t> regionalSpikeCount;
    
    // Plasticity activity
    uint32_t stdpEvents;
    uint32_t hebbianEvents;
    uint32_t structuralPlasticityEvents;
    
    // Neuromodulation levels
    double dopamineLevel;
    double curiosityLevel;
    double noveltyLevel;
    double predictionError;
};

/**
 * @class BrainStateVisualizer
 * @brief Core brain state monitoring and display
 * 
 * This visualizer provides comprehensive monitoring of brain state including:
 * - Neural activity patterns
 * - Regional specialization
 * - Spike statistics
 * - Synaptic dynamics
 * - Integration with memory systems
 */
class BrainStateVisualizer : public VisualizationBase {
public:
    BrainStateVisualizer();
    ~BrainStateVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // BrainStateVisualizer specific methods
    void updateNeuralActivity(const NeuralActivityData& activity);
    void setShowNeurons(bool show) { showNeurons_ = show; }
    void setShowSynapses(bool show) { showSynapses_ = show; }
    void setShowRegions(bool show) { showRegions_ = show; }
    void setShowActivityHeatmap(bool show) { showActivityHeatmap_ = show; }
    void setShowSpikeStatistics(bool show) { showSpikeStatistics_ = show; }
    
    // Data access
    const NeuralActivityData& getCurrentActivity() const { return currentActivity_; }
    double getActivityLevel() const { return activityLevel_; }
    uint32_t getTotalSpikeCount() const { return totalSpikeCount_; }
    uint32_t getActiveRegionCount() const { return activeRegionCount_; }
    
protected:
    bool initializeImpl(const VisualizationConfig& config) override;
    void updateImpl() override;
    void renderImpl() override;
    void closeImpl() override;
    void resetImpl() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Attached systems
    class Brain* brain_ = nullptr;
    class MemorySystem* memory_ = nullptr;
    class PlasticitySystem* plasticity_ = nullptr;
    
    // Visualization state
    NeuralActivityData currentActivity_;
    bool showNeurons_ = true;
    bool showSynapses_ = true;
    bool showRegions_ = true;
    bool showActivityHeatmap_ = true;
    bool showSpikeStatistics_ = true;
    
    // Activity metrics
    double activityLevel_ = 0.0;
    uint32_t totalSpikeCount_ = 0;
    uint32_t activeRegionCount_ = 0;
    uint32_t spikeHistorySize_ = 100;
    std::vector<uint32_t> spikeHistory_;
};

/**
 * @class NeuralNetworkVisualizer
 * @brief 3D neural network topology and activity visualization
 * 
 * This visualizer provides spatial representation of neurons and synapses,
 * supporting 3D rendering, camera controls, and interactive exploration.
 */
class NeuralNetworkVisualizer : public VisualizationBase {
public:
    NeuralNetworkVisualizer();
    ~NeuralNetworkVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // NeuralNetworkVisualizer specific methods
    void setShow3D(bool show) { show3D_ = show; }
    void setShowConnections(bool show) { showConnections_ = show; }
    void setShowSynapticWeights(bool show) { showSynapticWeights_ = show; }
    void setShowNeuronsByType(bool show) { showNeuronsByType_ = show; }
    void setShowPlasticityConnections(bool show) { showPlasticityConnections_ = show; }
    
    void setCameraPosition(float x, float y, float z);
    void setCameraRotation(float angleX, float angleY);
    void setZoomLevel(float zoom);
    void setRegionVisualization(const std::map<RegionId, std::string>& regionLabels);
    
    // Data access
    const std::map<NeuronId, float>& getNeuronPositions() const { return neuronPositions_; }
    const std::map<std::pair<NeuronId, NeuronId>, float>& getConnectionWeights() const { return connectionWeights_; }
    RegionId getRegionUnderMouse() const { return regionUnderMouse_; }
    
protected:
    bool initializeImpl(const VisualizationConfig& config) override;
    void updateImpl() override;
    void renderImpl() override;
    void closeImpl() override;
    void resetImpl() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Attached systems
    class Brain* brain_ = nullptr;
    
    // Visualization state
    bool show3D_ = true;
    bool showConnections_ = true;
    bool showSynapticWeights_ = true;
    bool showNeuronsByType_ = true;
    bool showPlasticityConnections_ = false;
    
    // Network data
    std::map<NeuronId, float> neuronPositions_;
    std::map<RegionId, std::pair<float, float>> regionCenters_;
    std::map<std::pair<NeuronId, NeuronId>, float> connectionWeights_;
    std::map<NeuronId, NeuronType> neuronTypes_;
    std::map<RegionId, float> regionActivities_;
    
    // Camera and view
    float cameraX_ = 0.0f;
    float cameraY_ = 0.0f;
    float cameraZ_ = 50.0f;
    float cameraAngleX_ = 0.0f;
    float cameraAngleY_ = 0.0f;
    float zoomLevel_ = 1.0f;
    
    // Interaction
    RegionId regionUnderMouse_ = 0;
    NeuronId neuronUnderMouse_ = 0;
};

/**
 * @class NeuronActivityVisualizer
 * @brief Individual neuron state and firing visualization
 * 
 * This visualizer focuses on detailed monitoring of individual neuron behavior,
 * including membrane potential, firing patterns, synaptic inputs, and state transitions.
 */
class NeuronActivityVisualizer : public VisualizationBase {
public:
    NeuronActivityVisualizer();
    ~NeuronActivityVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // NeuronActivityVisualizer specific methods
    void setShowMembranePotential(bool show) { showMembranePotential_ = show; }
    void setShowFiringHistory(bool show) { showFiringHistory_ = show; }
    void setShowSynapticInputs(bool show) { showSynapticInputs_ = show; }
    void setShowRefractoryPeriod(bool show) { showRefractoryPeriod_ = show; }
    void setShowAdaptation(bool show) { showAdaptation_ = show; }
    void setFocusNeuron(NeuronId neuronId) { focusNeuronId_ = neuronId; }
    void setShowAllNeurons(bool show) { showAllNeurons_ = show; }
    
    // Time series data access
    const std::vector<double>& getMembranePotentialHistory() const { return membranePotentialHistory_; }
    const std::vector<double>& getFiringRateHistory() const { return firingRateHistory_; }
    const std::vector<uint32_t>& getSpikeTimes() const { return spikeTimes_; }
    
protected:
    bool initializeImpl(const VisualizationConfig& config) override;
    void updateImpl() override;
    void renderImpl() override;
    void closeImpl() override;
    void resetImpl() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Attached systems
    class Brain* brain_ = nullptr;
    
    // Visualization state
    bool showMembranePotential_ = true;
    bool showFiringHistory_ = true;
    bool showSynapticInputs_ = true;
    bool showRefractoryPeriod_ = true;
    bool showAdaptation_ = true;
    NeuronId focusNeuronId_ = 0;
    bool showAllNeurons_ = false;
    
    // Neuron-specific data
    NeuronId selectedNeuronId_ = 0;
    std::vector<double> membranePotentialHistory_;
    std::vector<double> firingRateHistory_;
    std::vector<uint32_t> spikeTimes_;
    std::vector<float> synapticInputs_;
    std::vector<float> adaptationVariables_;
    
    // Configuration
    uint32_t historySize_ = 1000;
    double timeScale_ = 1.0;
};

/**
 * @class SynapseDynamicsVisualizer
 * @brief Synaptic transmission and plasticity visualization
 * 
 * This visualizer provides detailed visualization of synaptic dynamics including:
 * - Real-time synaptic weight changes
 * - STDP and Hebbian learning visualization
 * - Synaptic eligibility traces
 * - Connection strength modulation
 * - Plasticity rule visualization
 */
class SynapseDynamicsVisualizer : public VisualizationBase {
public:
    SynapseDynamicsVisualizer();
    ~SynapseDynamicsVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // SynapseDynamicsVisualizer specific methods
    void setShowSTDP(bool show) { showSTDP_ = show; }
    void setShowHebbian(bool show) { showHebbian_ = show; }
    void setShowStructuralPlasticity(bool show) { showStructuralPlasticity_ = show; }
    void setShowWeightChanges(bool show) { showWeightChanges_ = show; }
    void setShowEligibilityTraces(bool show) { showEligibilityTraces_ = show; }
    void setShowSynapticHistory(bool show) { showSynapticHistory_ = show; }
    
    void setPlasticityThreshold(float threshold) { plasticityThreshold_ = threshold; }
    void setSTDPWindowVisualization(bool show) { showSTDPWindow_ = show; }
    void setHebbianRuleVisualization(bool show) { showHebbianRule_ = show; }
    
    // Data access
    const std::vector<float>& getWeightHistory() const { return weightHistory_; }
    const std::vector<float>& getEligibilityTraceHistory() const { return eligibilityTraceHistory_; }
    float getMaxSynapticWeight() const { return maxSynapticWeight_; }
    float getAverageSynapticWeight() const { return averageSynapticWeight_; }
    
protected:
    bool initializeImpl(const VisualizationConfig& config) override;
    void updateImpl() override;
    void renderImpl() override;
    void closeImpl() override;
    void resetImpl() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Attached systems
    class Brain* brain_ = nullptr;
    class PlasticitySystem* plasticity_ = nullptr;
    
    // Visualization state
    bool showSTDP_ = true;
    bool showHebbian_ = true;
    bool showStructuralPlasticity_ = true;
    bool showWeightChanges_ = true;
    bool showEligibilityTraces_ = true;
    bool showSynapticHistory_ = true;
    
    // Synaptic data
    std::vector<float> weightHistory_;
    std::vector<float> eligibilityTraceHistory_;
    std::vector<std::pair<NeuronId, NeuronId>> strengthenedConnections_;
    std::vector<std::pair<NeuronId, NeuronId>> weakenedConnections_;
    
    // Statistics
    float maxSynapticWeight_ = 0.0f;
    float averageSynapticWeight_ = 0.0f;
    float plasticityThreshold_ = 0.01f;
    
    // Visualization parameters
    bool showSTDPWindow_ = false;
    bool showHebbianRule_ = false;
    uint32_t weightHistorySize_ = 500;
};

} // namespace nlm
