#pragma once

#include "Brain.hpp"
#include <memory>

namespace nlm {

class DevelopmentManager {
public:
    DevelopmentManager(Brain* brain);
    ~DevelopmentManager();
    
    bool initialize();
    void step(SimulationStep currentStep, double timestep);
    void reset();
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    
    // Development effects
    void applyDevelopmentEffects(SimulationStep currentStep, double timestep);
    DevelopmentalStage getCurrentDevelopmentalStage() const;
    
private:
    Brain* brain_;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
};

} // namespace nlm
