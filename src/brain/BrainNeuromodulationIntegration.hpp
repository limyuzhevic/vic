#pragma once

// BrainNeuromodulationIntegration.hpp - Dopamine, curiosity, novelty, prediction error integration
// Contains neuromodulation system integration functionality

#include "BrainCore.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations
class BrainCore;

// Brain Neuromodulation Integration Implementation
class BrainNeuromodulationIntegration {
public:
    struct Impl;
    Impl* pImpl;
    
    // Create brain neuromodulation integration with reference to core brain
    explicit BrainNeuromodulationIntegration(BrainCore* core);
    
    ~BrainNeuromodulationIntegration();
    
    // Disable copying, enable moving
    BrainNeuromodulationIntegration(const BrainNeuromodulationIntegration&) = delete;
    BrainNeuromodulationIntegration& operator=(const BrainNeuromodulationIntegration&) = delete;
    BrainNeuromodulationIntegration(BrainNeuromodulationIntegration&&) noexcept;
    BrainNeuromodulationIntegration& operator=(BrainNeuromodulationIntegration&&) noexcept;
    
    // Initialize neuromodulation systems
    bool initialize();
    
    // Update neuromodulation systems
    void update(TimestepDuration dt, SimulationStep currentStep, Timestamp currentTime);
    
    // Get neuromodulation system references
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Apply neuromodulation effects
    void applyDopamineEffects();
    void applyCuriosityEffects();
    void applyNoveltyEffects();
    void applyPredictionErrorEffects();
    
    // Clear neuromodulation state
    void clear();
    
    // Get statistics
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionErrorLevel() const;
    
private:
    // Internal helper methods
    void updateDopamine();
    void updateCuriosity();
    void updateNovelty();
    void updatePredictionError();
    void updatePlasticityModulation();
    void applyNeuromodulationToNeurons();
    
    // Utility methods
    void calculateNeuromodulationStats() const;
    
    struct Impl {
        BrainCore* brainCore;
        
        // Neuromodulation system references
        Dopamine* dopamine;
        Curiosity* curiosity;
        Novelty* novelty;
        PredictionError* predictionError;
        
        // Neuromodulation state
        std::vector<float> neuromodulationHistory;
        
        // Statistics
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        float predictionErrorLevel;
        
        Impl(BrainCore* core);
    };
};

} // namespace nlm

