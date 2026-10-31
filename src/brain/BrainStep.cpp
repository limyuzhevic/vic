// Brain step implementation
// Extracted from Brain.cpp for better organization and maintainability

#include "BrainStep.hpp"
#include "Brain.hpp"
#include "BrainPlasticity.hpp"
#include "BrainMemory.hpp"
#include "BrainCognition.hpp"
#include "BrainNeuromodulation.hpp"
#include "BrainDevelopment.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/BrainInitialization.hpp"

namespace nlm {

void brainStep(Brain* brain, SimulationStep currentStep) {
    if (brain) {
        brain->step(currentStep, static_cast<Timestamp>(currentStep) * brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
    }
}

void brainStepWithTime(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    if (brain) {
        brain->step(currentStep, currentTime);
    }
}

} // namespace nlm
