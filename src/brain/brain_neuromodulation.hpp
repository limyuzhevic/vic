#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Neuromodulation systems integration
class BrainNeuromodulation {
public:
    BrainNeuromodulation() = default;
    ~BrainNeuromodulation() = default;
    
    // Initialize neuromodulation systems
    void initialize(std::shared_ptr<class Brain> brain);
    
    // Neuromodulation updates (called from main step)
    void update(double timestep,
                std::shared_ptr<Dopamine> dopamine,
                std::shared_ptr<Curiosity> curiosity,
                std::shared_ptr<Novelty> novelty,
                std::shared_ptr<PredictionError> predictionError,
                const class SensoryInput& sensoryInput);
    
    // Apply neuromodulation effects to neural activity
    void applyNeuromodulation(double timestep,
                              std::shared_ptr<Dopamine> dopamine,
                              const std::vector<std::unique_ptr<class NeuralRegion>>& regions);
    
    // Get neuromodulation system accessors
    Dopamine* getDopamine() const;
    Curiosity* getCuriosity() const;
    Novelty* getNovelty() const;
    PredictionError* getPredictionErrorSignal() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
