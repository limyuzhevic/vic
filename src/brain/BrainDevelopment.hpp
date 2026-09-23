#pragma once

#include "../core/Types/Types.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../development/Synaptogenesis.hpp"
#include "../development/Pruning.hpp"
#include "../development/Maturation.hpp"

namespace nlm {

// Forward declarations
class DevelopmentSystem;
class Synaptogenesis;
class Pruning;
class Maturation;
class Brain;
class RandomGenerator;

class BrainDevelopment {
public:
    // ===========================================
    // DEVELOPMENT SYSTEM
    // ===========================================
    
    static void initializeDevelopmentSystem(DevelopmentSystem* developmentSystem, Brain* brain);
    static void updateDevelopment(DevelopmentSystem* developmentSystem, Brain* brain,
                                  RandomGenerator* rng, TimestepDuration timestep);
    static void applyDevelopmentEffects(Brain* brain, DevelopmentalStage stage);
    
    // ===========================================
    // SYNAPTOGENESIS DEVELOPMENT
    // ===========================================
    
    static void updateSynaptogenesis(Synaptogenesis* synaptogenesis,
                                     Brain* brain, RandomGenerator* rng);
    
    // ===========================================
    // PRUNING DEVELOPMENT
    // ===========================================
    
    static void updatePruning(Pruning* pruning,
                              Brain* brain, RandomGenerator* rng);
    
    // ===========================================
    // MATURATION DEVELOPMENT
    // ===========================================
    
    static void updateMaturation(Maturation* maturation,
                                 Brain* brain, RandomGenerator* rng);
    
    // ===========================================
    // DEVELOPMENT STATISTICS AND ACCESSORS
    // ===========================================
    
    static void logDevelopmentStatus(DevelopmentSystem* developmentSystem, Brain* brain);
};

} // namespace nlm