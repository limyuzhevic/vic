#pragma once

#include "brain/DevelopmentSystem.hpp"
#include "brain/Brain.hpp"
#include "brain/StructuralPlasticity.hpp"
#include "brain/DevelopmentalStage.hpp"
#include "core/Random/Random.hpp"

namespace nlm {

class BrainDevelopment {
public:
    static void updateDevelopmentSystem(DevelopmentSystem* developmentSystem,
                                       Brain* brain,
                                       RandomGenerator& rng,
                                       TimestepDuration timestep,
                                       StructuralPlasticity* structuralPlasticity,
                                       DevelopmentalStage& developmentalStage);
    
    static void advanceDevelopmentalStage(DevelopmentSystem* developmentSystem,
                                         size_t currentStep,
                                         DevelopmentalStage& developmentalStage);
};

} // namespace nlm
