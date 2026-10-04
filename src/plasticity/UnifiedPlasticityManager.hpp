#pragma once

#include "../../plasticity/PlasticityRule.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include "../../memory/MemorySystem.hpp"
#include "../../development/DevelopmentSystem.hpp"
#include "../../brain/Brain.hpp"
#include "../../prediction/PredictionSystem.hpp"
#include <cmath>
#include <algorithm>
#include <map>
#include <vector>

namespace nlm {

// Unified Plasticity Management System
class UnifiedPlasticityManager {
public:
    UnifiedPlasticityManager();
    ~UnifiedPlasticityManager();
    
    // Initialize with brain system
    void initialize(class Brain* brain);
    
    // Update all plasticity rules
    void update(TimestepDuration dt);
    
    // Apply plasticity based on neural activity
    void applyPlasticity(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt);
    
    // Configuration and parameters
    void setGlobalLearningRate(float rate);
    float getGlobalLearningRate() const;
    void setPlasticityEnabled(bool enabled);
    bool isPlasticityEnabled() const;
    
    // Integration with neuromodulation
    void setNeuromodulator(Neuromodulator* neuromodulator);
    Neuromodulator* getNeuromodulator() const;
    
    // Integration with memory system
    void setMemorySystem(MemorySystem* memory);
    MemorySystem* getMemorySystem() const;
    
    // Integration with development system
    void setDevelopmentSystem(DevelopmentSystem* development);
    DevelopmentSystem* getDevelopmentSystem() const;
    
    // Integration with prediction system
    void setPredictionSystem(PredictionSystem* prediction);
    PredictionSystem* getPredictionSystem() const;
    
    // Context-dependent plasticity
    void setContext(class Context* context);
    Context* getContext() const;
    
    // Plasticity rule management
    void registerPlasticityRule(PlasticityRule* rule);
    void unregisterPlasticityRule(const char* name);
    PlasticityRule* getPlasticityRule(const char* name) const;
    
    // Statistics and monitoring
    float getTotalPlasticityWeight() const;
    size_t getActivePlasticityRules() const;
    float getAveragePlasticityStrength() const;
    
    // Stability and homeostasis
    void enforceHomeostasis();
    float calculateHomeostaticPressure() const;
    
    // Adaptation and learning
    void adaptToExperience();
    void adjustPlasticityParameters();
    
    // Development stage adaptation
    void updateDevelopmentalAdaptation();
    
    // Reward and prediction error integration
    void integrateRewardPredictionError();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
