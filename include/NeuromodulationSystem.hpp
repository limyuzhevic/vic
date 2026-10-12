#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <memory>

namespace nlm {

// ==========================================
// NEUROMODULATION SYSTEM - Neuromodulation Systems
// ==========================================
// 
// Manages: dopamine, curiosity, novelty, prediction error
// 
// Contains: Dopamine, Curiosity, Novelty, PredictionError
// Handles: reward, motivation, novelty detection
// Manages: neuromodulatory effects on neural activity

class NeuromodulationSystem {
public:
    NeuromodulationSystem(std::shared_ptr<Config> config);
    ~NeuromodulationSystem();
    
    // Disable copying, enable moving
    NeuromodulationSystem(const NeuromodulationSystem&) = delete;
    NeuromodulationSystem& operator=(const NeuromodulationSystem&) = delete;
    NeuromodulationSystem(NeuromodulationSystem&&) noexcept;
    NeuromodulationSystem& operator=(NeuromodulationSystem&&) noexcept;
    
    // Initialize neuromodulation system with brain reference
    bool initialize(Brain* brain);
    
    // Get neuromodulation system components
    class Dopamine* getDopamine() { return dopamine_.get(); }
    const class Dopamine* getDopamine() const { return dopamine_.get(); }
    
    class Curiosity* getCuriosity() { return curiosity_.get(); }
    const class Curiosity* getCuriosity() const { return curiosity_.get(); }
    
    class Novelty* getNovelty() { return novelty_.get(); }
    const class Novelty* getNovelty() const { return novelty_.get(); }
    
    class PredictionError* getPredictionError() { return predictionError_.get(); }
    const class PredictionError* getPredictionError() const { return predictionError_.get(); }
    
    // Get brain reference
    Brain* getBrain() { return brain_; }
    const Brain* getBrain() const { return brain_; }
    
    // Update neuromodulation systems
    void update(TimestepDuration dt);
    
    // Apply neuromodulation to brain
    void applyToBrain();
    
    // Apply neuromodulation signal
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Get neuromodulation statistics
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    
    // Set neuromodulation parameters
    void setDopamineLevel(float level);
    void setCuriosityLevel(float level);
    void setNoveltyLevel(float level);
    void setPredictionError(float error);
    
    // Reset neuromodulation system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::shared_ptr<Config> config_;
    
    std::unique_ptr<class Dopamine> dopamine_;
    std::unique_ptr<class Curiosity> curiosity_;
    std::unique_ptr<class Novelty> novelty_;
    std::unique_ptr<class PredictionError> predictionError_;
};

} // namespace nlm