#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class Config;

// NeuromodulationSystem: Modulatory signals coordination
// Manages dopamine, curiosity, novelty, and prediction error
class NeuromodulationSystem {
public:
    NeuromodulationSystem(std::shared_ptr<Config> config);
    ~NeuromodulationSystem();
    
    // Initialize neuromodulation systems
    bool initialize(Brain* brain);
    
    // Neuromodulator access
    class Dopamine* getDopamine() { return dopamine_.get(); }
    class Curiosity* getCuriosity() { return curiosity_.get(); }
    class Novelty* getNovelty() { return novelty_.get(); }
    class PredictionError* getPredictionError() { return predictionError_.get(); }
    
    // Update neuromodulation
    void update(double timestep);
    
    // Neuromodulation access
    float getDopamineLevel() const { return dopamineLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionError_; }
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
    // Apply neuromodulatory signal
    void applySignal(const class Neuromodulator& signal);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Neuromodulation state
    float dopamineLevel_;
    float curiosityLevel_;
    float noveltyLevel_;
    float predictionError_;
    float expectedReward_;
};

} // namespace nlm
