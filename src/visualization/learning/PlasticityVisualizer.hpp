// ============================================================================
// Plasticity Visualization Components
// ============================================================================

#pragma once

#include <string>
#include <memory>
#include <vector>
#include "../core/Types/Types.hpp"
#include "../plasticity/PlasticitySystem.hpp"
#include "VisualizationBase.hpp"

namespace nlm {

/**
 * @class PlasticityVisualizer
 * @brief Learning and plasticity process visualization
 * 
 * This visualizer provides comprehensive visualization of plasticity processes,
 * including STDP, Hebbian learning, structural changes, and learning dynamics.
 */
class PlasticityVisualizer : public VisualizationBase {
public:
    PlasticityVisualizer();
    ~PlasticityVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // PlasticityVisualizer specific methods
    void setShowSTDP(bool show) { showSTDP_ = show; }
    void setShowHebbian(bool show) { showHebbian_ = show; }
    void setShowStructural(bool show) { showStructural_ = show; }
    void setShowLearningCurve(bool show) { showLearningCurve_ = show; }
    void setShowWeightChanges(bool show) { showWeightChanges_ = show; }
    void setShowEligibilityTraces(bool show) { showEligibilityTraces_ = show; }
    
    // Data access
    double getLearningRate() const { return learningRate_; }
    uint32_t getPlasticityEvents() const { return plasticityEvents_; }
    double getOverallPlasticity() const { return overallPlasticity_; }
    float getAverageWeightChange() const { return averageWeightChange_; }
    
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
    bool showStructural_ = true;
    bool showLearningCurve_ = true;
    bool showWeightChanges_ = true;
    bool showEligibilityTraces_ = true;
    
    // Plasticity metrics
    double learningRate_ = 0.0;
    uint32_t plasticityEvents_ = 0;
    double overallPlasticity_ = 0.0;
    float averageWeightChange_ = 0.0f;
    uint32_t stdpEvents_ = 0;
    uint32_t hebbianEvents_ = 0;
    uint32_t structuralEvents_ = 0;
    
    // Learning curve data
    uint32_t learningHistorySize_ = 100;
    std::vector<double> learningCurve_;
    std::vector<double> performanceCurve_;
};

/**
 * @class STDPVisualizer
 * @brief Spike-timing dependent plasticity visualization
 * 
 * This visualizer focuses specifically on STDP visualization, showing
 * the delicate balance between LTP and LTD, the timing windows, and
 * the impact of spike timing on synaptic strengthening/weakening.
 */
class STDPVisualizer : public VisualizationBase {
public:
    STDPVisualizer();
    ~STDPVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // STDPVisualizer specific methods
    void setShowLTP(bool show) { showLTP_ = show; }
    void setShowLTD(bool show) { showLTD_ = show; }
    void setShowTimingWindow(bool show) { showTimingWindow_ = show; }
    void setShowSpikePairs(bool show) { showSpikePairs_ = show; }
    void setShowWeightTrace(bool show) { showWeightTrace_ = show; }
    
    // Data access
    float getLTPStrength() const { return ltpStrength_; }
    float getLTDStrength() const { return ltdStrength_; }
    double getSTDPBalance() const { return stdpBalance_; }
    uint32_t getLTPEvents() const { return ltpEvents_; }
    uint32_t getLTDEvents() const { return ltdEvents_; }
    
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
    bool showLTP_ = true;
    bool showLTD_ = true;
    bool showTimingWindow_ = true;
    bool showSpikePairs_ = true;
    bool showWeightTrace_ = true;
    
    // STDP metrics
    float ltpStrength_ = 0.0f;
    float ltdStrength_ = 0.0f;
    double stdpBalance_ = 0.0;
    uint32_t ltpEvents_ = 0;
    uint32_t ltdEvents_ = 0;
    float timingWindowWidth_ = 20.0f;
    float timingWindowHeight_ = 0.01f;
    
    // Timing data
    uint32_t timingHistorySize_ = 100;
    std::vector<float> prePostTiming_;
    std::vector<float> weightChanges_;
};

/**
 * @class HebbianVisualizer
 * @brief Hebbian learning visualization
 * 
 * This visualizer focuses on Hebbian learning patterns, showing
 * co-activation, synaptic strengthening through correlated activity,
 * and the formation of associative connections.
 */
class HebbianVisualizer : public VisualizationBase {
public:
    HebbianVisualizer();
    ~HebbianVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // HebbianVisualizer specific methods
    void setShowCoActivation(bool show) { showCoActivation_ = show; }
    void setShowAssociativeMemory(bool show) { showAssociativeMemory_ = show; }
    void setShowSynapticStrengthening(bool show) { showSynapticStrengthening_ = show; }
    void setShowLearningRules(bool show) { showLearningRules_ = show; }
    void setShowFireTogether(bool show) { showFireTogether_ = show; }
    
    // Data access
    double getCoActivationStrength() const { return coActivationStrength_; }
    uint32_t getAssociativeEvents() const { return associativeEvents_; }
    float getHebbianLearningRate() const { return hebbianLearningRate_; }
    uint32_t getCorrelatedSpikePairs() const { return correlatedSpikePairs_; }
    
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
    bool showCoActivation_ = true;
    bool showAssociativeMemory_ = true;
    bool showSynapticStrengthening_ = true;
    bool showLearningRules_ = true;
    bool showFireTogether_ = true;
    
    // Hebbian metrics
    double coActivationStrength_ = 0.0;
    uint32_t associativeEvents_ = 0;
    float hebbianLearningRate_ = 0.01f;
    uint32_t correlatedSpikePairs_ = 0;
    float correlationThreshold_ = 0.5f;
    uint32_t associationHistorySize_ = 100;
    std::vector<float> correlationHistory_;
    std::vector<uint32_t> associativeConnections_;
};

/**
 * @class StructuralPlasticityVisualizer
 * @brief Synaptogenesis and pruning visualization
 * 
 * This visualizer focuses on structural plasticity, showing the formation
 * and elimination of synaptic connections, the dynamics of neural
 * connectivity, and the balance between growth and refinement.
 */
class StructuralPlasticityVisualizer : public VisualizationBase {
public:
    StructuralPlasticityVisualizer();
    ~StructuralPlasticityVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // StructuralPlasticityVisualizer specific methods
    void setShowSynaptogenesis(bool show) { showSynaptogenesis_ = show; }
    void setShowPruning(bool show) { showPruning_ = show; }
    void setShowConnectivity(bool show) { showConnectivity_ = show; }
    void setShowGrowthDynamics(bool show) { showGrowthDynamics_ = show; }
    void setShowRefinement(bool show) { showRefinement_ = show; }
    void setShowNetworkTopology(bool show) { showNetworkTopology_ = show; }
    
    // Data access
    uint32_t getNewConnections() const { return newConnections_; }
    uint32_t getPrunedConnections() const { return prunedConnections_; }
    float getConnectivityDensity() const { return connectivityDensity_; }
    uint32_t getGrowthRate() const { return growthRate_; }
    uint32_t getPruningRate() const { return pruningRate_; }
    
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
    bool showSynaptogenesis_ = true;
    bool showPruning_ = true;
    bool showConnectivity_ = true;
    bool showGrowthDynamics_ = true;
    bool showRefinement_ = true;
    bool showNetworkTopology_ = true;
    
    // Structural plasticity metrics
    uint32_t newConnections_ = 0;
    uint32_t prunedConnections_ = 0;
    float connectivityDensity_ = 0.0f;
    uint32_t growthRate_ = 0;
    uint32_t pruningRate_ = 0;
    float synaptogenesisRate_ = 0.0001f;
    float pruningRate_ = 0.00001f;
    uint32_t topologyHistorySize_ = 100;
    std::vector<float> connectivityHistory_;
    std::vector<uint32_t> newConnectionHistory_;
    std::vector<uint32_t> prunedConnectionHistory_;
};

} // namespace nlm
