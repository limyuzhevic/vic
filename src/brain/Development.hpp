// Development.hpp
// Development system interface

#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

class DevelopmentSystem {
public:
    void initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain);
    void processStep(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep);
    
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
private:
    void updateDevelopmentalStage(SimulationStep currentStep);
    void applyDevelopmentEffects(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep);
    void applyDevelopmentToMemorySystems();
    void applyDevelopmentToCognitiveSystems();
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    DevelopmentalStage developmentStage_;
};

} // namespace nlm
