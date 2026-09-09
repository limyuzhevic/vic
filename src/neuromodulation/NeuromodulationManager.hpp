#pragma once

#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"

namespace nlm {

class Brain;

class NeuromodulationManager {
public:
    NeuromodulationManager();
    ~NeuromodulationManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update neuromodulation systems
    void update();
    
    // Get neuromodulation systems
    Dopamine* getDopamine() { return dopamine_.get(); }
    Curiosity* getCuriosity() { return curiosity_.get(); }
    PredictionError* getPredictionError() { return predictionError_.get(); }
    Novelty* getNovelty() { return novelty_.get(); }
    
    // Configure from config
    void configureFromConfig(const Config& config);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<PredictionError> predictionError_;
    std::unique_ptr<Novelty> novelty_;
};

} // namespace nlm