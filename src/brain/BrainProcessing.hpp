// Brain Processing Header

#pragma once

#include "Brain.hpp"
#include <vector>

namespace nlm {

class BrainProcessing {
public:
    static void processStep(Brain::Impl& impl, SimulationStep currentStep, Timestamp currentTime);
    static void updateNeurons(Brain::Impl& impl, Timestamp currentTime);
    static void detectAndProcessSpikes(Brain::Impl& impl, SimulationStep currentStep, Timestamp currentTime);
    static void updateWorkingMemory(Brain::Impl& impl);
    static void applyNeuromodulationEffects(Brain::Impl& impl);
    static void applyPlasticityRules(Brain::Impl& impl);
    static void applyHomeostaticScaling(Brain::Impl& impl, SimulationStep currentStep);
    static void updateEpisodicMemory(Brain::Impl& impl);
    static void updatePredictionSystem(Brain::Impl& impl);
    static void updateAttentionSystem(Brain::Impl& impl);
    static void updateConceptFormation(Brain::Impl& impl);
    static void applyStructuralPlasticity(Brain::Impl& impl, SimulationStep currentStep);
    static void replayMemories(Brain::Impl& impl, SimulationStep currentStep);
    static void applyDevelopmentEffects(Brain::Impl& impl, SimulationStep currentStep);
    static void consolidateMemories(Brain::Impl& impl, SimulationStep currentStep);
    static void checkpointManagerUpdate(Brain::Impl& impl, SimulationStep currentStep, Timestamp currentTime);
};

} // namespace nlm
