#include "BrainDevelopment.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// ===========================================
// DEVELOPMENT SYSTEM
// ===========================================

void BrainDevelopment::initializeDevelopmentSystem(DevelopmentSystem* developmentSystem,
                                                 Brain* brain) {
    if (!developmentSystem || !brain) return;

    developmentSystem->initialize(brain);
}

void BrainDevelopment::updateDevelopment(DevelopmentSystem* developmentSystem,
                                        Brain* brain, RandomGenerator* rng,
                                        TimestepDuration timestep) {
    if (!developmentSystem || !brain || !rng) return;

    developmentSystem->update(brain, *rng, timestep);
}

void BrainDevelopment::applyDevelopmentEffects(Brain* brain, DevelopmentalStage stage) {
    if (!brain) return;

    // Development affects plasticity rates based on stage
    auto* sp = brain->getStructuralPlasticity();
    if (sp) {
        float plasticityMod = 1.0f;

        switch (stage) {
            case DevelopmentalStage::Initial:
                plasticityMod = 1.0f;  // High plasticity
                break;
            case DevelopmentalStage::CriticalPeriod:
                plasticityMod = 0.8f;
                break;
            case DevelopmentalStage::Maturation:
                plasticityMod = 0.5f;
                break;
            case DevelopmentalStage::Adult:
                plasticityMod = 0.2f;  // Stable
                break;
        }

        // Apply developmental modulation to structural plasticity
        sp->setSynaptogenesisRate(0.0001f * plasticityMod);
        sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }
}

// ===========================================
// SYNAPTOGENESIS DEVELOPMENT
// ===========================================

void BrainDevelopment::updateSynaptogenesis(Synaptogenesis* synaptogenesis,
                                           Brain* brain, RandomGenerator* rng) {
    if (!synaptogenesis || !brain || !rng) return;

    // Implement real synaptogenesis (currently placeholder)
    synaptogenesis->update(brain, *rng);
}

// ===========================================
// PRUNING DEVELOPMENT
// ===========================================

void BrainDevelopment::updatePruning(Pruning* pruning,
                                    Brain* brain, RandomGenerator* rng) {
    if (!pruning || !brain || !rng) return;

    // Implement real pruning (currently placeholder)
    pruning->update(brain, *rng);
}

// ===========================================
// MATURATION DEVELOPMENT
// ===========================================

void BrainDevelopment::updateMaturation(Maturation* maturation,
                                       Brain* brain, RandomGenerator* rng) {
    if (!maturation || !brain || !rng) return;

    // Implement real maturation (currently placeholder)
    maturation->update(brain, *rng);
}

// ===========================================
// DEVELOPMENT STATISTICS AND ACCESSORS
// ===========================================

void BrainDevelopment::logDevelopmentStatus(DevelopmentSystem* developmentSystem,
                                           Brain* brain) {
    if (!brain) return;

    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));

    // Additional development metrics can be logged here
}

} // namespace nlm
