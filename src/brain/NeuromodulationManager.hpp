#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <string>

namespace nlm {

// Neuromodulation system status
struct NeuromodulationStatus {
    float dopamineLevel{0.0f};
    float curiosityLevel{0.0f};
    float predictionErrorLevel{0.0f};
    float noveltyLevel{0.0f};
    
    // Calculate total neuromodulation effect
    float getTotalModulation() const {
        return dopamineLevel + curiosityLevel + predictionErrorLevel + noveltyLevel;
    }
    
    // Check for consistency
    bool isConsistent() const {
        return dopamineLevel >= 0.0f && dopamineLevel <= 10.0f &&
               curiosityLevel >= 0.0f && curiosityLevel <= 10.0f &&
               predictionErrorLevel >= 0.0f && predictionErrorLevel <= 10.0f &&
               noveltyLevel >= 0.0f && noveltyLevel <= 10.0f;
    }
    
    std::string toString() const {
        return "Dopamine: " + std::to_string(dopamineLevel) +
               ", Curiosity: " + std::to_string(curiosityLevel) +
               ", PredictionError: " + std::to_string(predictionErrorLevel) +
               ", Novelty: " + std::to_string(noveltyLevel);
    }
};

class NeuromodulationManager {
public:
    NeuromodulationManager();
    ~NeuromodulationManager();
    
    // Initialize neuromodulation systems
    bool initialize(Brain* brain);
    
    // Update neuromodulators
    void update(float timestep);
    
    // Apply neuromodulation to neurons
    void applyToNeurons(std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    // Get neuromodulation levels
    NeuromodulationStatus getStatus() const { return status_; }
    float getDopamineLevel() const { return status_.dopamineLevel; }
    float getCuriosityLevel() const { return status_.curiosityLevel; }
    float getPredictionErrorLevel() const { return status_.predictionErrorLevel; }
    float getNoveltyLevel() const { return status_.noveltyLevel; }
    
    // Get plasticity factor
    float getPlasticityFactor() const { return dopamineLevel * 0.5f; }
    
    // Configuration validation
    bool validateConfiguration(std::shared_ptr<Config> config) const;
    
    // Get neuromodulators
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    PredictionError* getPredictionError() { return predictionError.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    
    // State management
    void setLevels(float dopamine, float curiosity, float predictionError, float novelty);
    
private:
    struct Impl;
    Impl* pImpl;
    NeuromodulationStatus status_;
};
