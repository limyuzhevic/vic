#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include <memory>

namespace nlm {

class NeuromodulationController {
public:
    NeuromodulationController(std::shared_ptr<Config> config);
    ~NeuromodulationController();
    
    // Initialize neuromodulation systems
    bool initialize(Brain* brain);
    
    // Update neuromodulation systems
    void update(Timestamp time, TimestepDuration timestep);
    
    // Reset neuromodulation systems
    void reset();
    
    // Get component access
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    PredictionError* getPredictionError() { return predictionError.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    
    // Neuromodulation statistics
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // Configuration methods
    void setDopamineLevel(float level) { dopamineLevel = level; }
    void setCuriosityGain(float gain) { curiosityGain = gain; }
    void setNoveltyThreshold(float threshold) { noveltyThreshold = threshold; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    float dopamineLevel;
    float curiosityGain;
    float noveltyThreshold;
};

} // namespace nlm