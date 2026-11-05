// BrainDevelopment.hpp - Development system management
#pragma once

#include "../development/DevelopmentSystem.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

class BrainDevelopment {
public:
    static void update(Brain& brain, SimulationStep currentStep);
    static void applyDevelopmentalStage(Brain& brain, DevelopmentalStage stage);
    static void updatePlasticityBasedOnDevelopment(Brain& brain);
    static void applySynaptogenesis(Brain& brain);
    static void applyPruning(Brain& brain);
    static void resetDevelopment(Brain& brain);
    static void calculatePlasticityModulation(Brain& brain, float& plasticityMod);
    static void scheduleDevelopmentEvents(Brain& brain);
    static void applyMaturation(Brain& brain);
    static void applyCriticalPeriod(Brain& brain);
    static void applyAgingEffects(Brain& brain);
    static void updateStructuralConnectivity(Brain& brain);

private:
    static float calculateDevelopmentProgress(Brain& brain, SimulationStep currentStep);
    static void applyStageSpecificEffects(Brain& brain, DevelopmentalStage stage);
    static void updateConnectivityPatterns(Brain& brain);
    static void applyExperienceDependentChanges(Brain& brain);
    static void updateHomeostaticSetPoints(Brain& brain);
};
