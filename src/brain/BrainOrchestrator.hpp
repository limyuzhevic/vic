#pragma once

#include "Config.hpp"
#include "Brain.hpp"
#include <memory>

namespace nlm {

// Forward declarations for nested classes
class SensoryInput;
class Action;
class Neuromodulator;
class SimulationStep;
class Timestamp;

class BrainOrchestrator {
public:
    BrainOrchestrator(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain);
    ~BrainOrchestrator();
    
    // Core simulation methods
    bool initialize();
    void step(SimulationStep step, Timestamp time);
    void receiveSensoryInput(const SensoryInput& input);
    std::unique_ptr<Action> produceAction();
    
    // Development and neuromodulation
    void updateDevelopment(double timestep);
    void applyNeuromodulation(const Neuromodulator& signal);
    void reset();
    
    // Accessors
    std::shared_ptr<Brain> getBrain() const;
    float getExcitationInhibitionRatio() const;
    float getAverageFiringRate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

