// PlasticityVisualization.hpp - Plasticity tracking visualization for NLM

#pragma once

#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace nlm {

// Plasticity visualization for tracking synaptic weight changes and plasticity rules
class PlasticityVisualization {
public:
    PlasticityVisualization(std::shared_ptr<Logger> logger);
    ~PlasticityVisualization();
    
    // Initialize plasticity visualization system
    bool initialize();
    
    // Render synaptic weight changes over time
    void renderWeightChanges(const Brain& brain,
                            double timeWindow = 60.0,
                            bool showSTDP = true,
                            bool showHebbian = true,
                            bool showStructural = true,
                            const std::string& title = "Synaptic Weight Changes");
    
    // Render plasticity rule activity and strength
    void renderPlasticityActivity(const Brain& brain,
                                 const std::string& rule = "stdp",
                                 bool showStrength = true,
                                 bool showTiming = true,
                                 const std::string& title = "Plasticity Activity");
    
    // Render structural plasticity (synaptogenesis and pruning)
    void renderStructuralPlasticity(const Brain& brain,
                                    bool showGrowth = true,
                                    bool showPruning = true,
                                    bool showRefinement = true,
                                    const std::string& title = "Structural Plasticity");
    
    // Render learning trajectories and performance curves
    void renderLearningTrajectories(const Brain& brain,
                                   size_t numNeurons = 10,
                                   bool showIndividual = true,
                                   bool showPopulation = true,
                                   const std::string& title = "Learning Trajectories");
    
    // Plasticity visualization modes
    enum class PlasticityMode {
        WEIGHTS,       // Show synaptic weights
        CHANGES,       // Show weight changes
        RULES,         // Show plasticity rule activity
        STRUCTURE,     // Show structural changes
        TRAJECTORIES,   // Show learning trajectories
        CONSOLIDATION,  // Show memory consolidation
        ADAPTATION     // Show adaptation and compensation
    };
    
    void setPlasticityMode(PlasticityMode mode);
    PlasticityMode getPlasticityMode() const;
    
    // Configuration methods
    void setWeightChangeThreshold(float threshold);
    void enableShowTraces(bool enable);
    void setTimeWindow(double window);
    void setMinWeightChange(float minChange);
    
    // Plasticity statistics overlay
    struct PlasticityStats {
        // Synaptic plasticity
        size_t plasticSynapseCount;
        size_t stdpSynapseCount;
        float totalWeightChange;
        float meanWeightChange;
        float stdWeightChange;
        float weightChangeVariance;
        
        // Hebbian plasticity
        size_t hebbianSynapseCount;
        float hebbianLearningRate;
        
        // STDP plasticity
        size_t stdpSynapseCount;
        float stdpLearningRate;
        float stdpTimeWindow;
        
        // Structural plasticity
        size_t synaptogenesisEvents;
        size_t pruningEvents;
        float structuralGrowthRate;
        float structuralRefinementRate;
        
        // Learning metrics
        float learningProgress;
        float retentionQuality;
        float generalizationAbility;
    };
    
    PlasticityStats computePlasticityStats(const Brain& brain);
    
    // Export capabilities
    bool exportPlasticityData(const std::string& filename, const Brain& brain);
    bool exportWeightHistory(const std::string& filename, const Brain& brain);
    bool exportRuleActivity(const std::string& filename, const Brain& brain);
    bool exportLearningMetrics(const std::string& filename, const Brain& brain);
    
    // Real-time plasticity monitoring
    void startRealTimePlasticityMonitoring(double updateInterval = 0.5);
    void stopRealTimePlasticityMonitoring();
    bool isMonitoring() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

