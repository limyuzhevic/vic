#include "BrainStepProcessor.hpp"

namespace nlm {

BrainStepProcessor::BrainStepProcessor(Brain* brain,
                                     NeuralProcessor* neuralProcessor,
                                     PlasticityManager* plasticityManager,
                                     MemorySystem* memorySystem,
                                     CognitionSystem* cognitionSystem,
                                     DevelopmentSystem* developmentSystem)
    : brain(brain), neuralProcessor(neuralProcessor), plasticityManager(plasticityManager),
      memorySystem(memorySystem), cognitionSystem(cognitionSystem),
      developmentSystem(developmentSystem), stepsSinceReplay(0),
      stepsSinceConsolidation(0), isResting(false) {
}

void BrainStepProcessor::executeStep(SimulationStep currentStep, Timestamp currentTime) {
    TimestepDuration dt = brain->pImpl->timestep;
    
    // Execute all integrated systems in the correct order
    executeNeuronalDynamics(dt);
    executeSpikingSystem(currentStep, currentTime);
    executeMemorySystems(currentStep, currentTime);
    executePlasticity(currentStep, currentTime);
    executeNeuromodulation(dt);
    executeCognition(dt);
    executeDevelopment(currentStep, dt);
    executeReplaySystem(currentStep);
    executeCheckpointing(currentStep, currentTime);
}

void BrainStepProcessor::executeNeuronalDynamics(TimestepDuration dt) {
    neuralProcessor->updateNeuronalDynamics(dt);
}

void BrainStepProcessor::executeSpikingSystem(SimulationStep currentStep, Timestamp currentTime) {
    // Detect spikes and schedule events
    neuralProcessor->detectAndScheduleSpikes(currentStep, currentTime);
    
    // Process spike events and deliver synaptic input
    neuralProcessor->processSpikes();
    
    // Process delayed spike events (synaptic transmission)
    neuralProcessor->processDelayedSpikes(currentTime);
}

void BrainStepProcessor::executeMemorySystems(SimulationStep currentStep, Timestamp currentTime) {
    // Update working memory
    memorySystem->updateWorkingMemory(brain->pImpl->timestep);
    
    // Update episodic memory
    memorySystem->updateEpisodicMemory(currentStep, currentTime);
}

void BrainStepProcessor::executePlasticity(SimulationStep currentStep, Timestamp currentTime) {
    // Apply plasticity rules
    if (plasticityManager) {
        plasticityManager->applySTDP(currentStep, currentTime);
        plasticityManager->applyHebbianLearning(currentStep, currentTime);
    }
}

void BrainStepProcessor::executeNeuromodulation(TimestepDuration dt) {
    // Update neuromodulation systems
    if (brain->pImpl->dopamine) {
        brain->pImpl->dopamine->update(dt);
    }
    if (brain->pImpl->curiosity) {
        brain->pImpl->curiosity->update(dt);
    }
    if (brain->pImpl->novelty) {
        brain->pImpl->novelty->update(dt);
    }
    
    // Update plasticity manager with neuromodulation
    if (plasticityManager) {
        Neuromodulator combinedModulation;
        if (brain->pImpl->dopamine) {
            combinedModulation = *brain->pImpl->dopamine;
        } else if (brain->pImpl->curiosity) {
            combinedModulation = *brain->pImpl->curiosity;
        } else if (brain->pImpl->novelty) {
            combinedModulation = *brain->pImpl->novelty;
        }
        
        if (currentNeuromodulation.getLevel() > 0.0f) {
            plasticityManager->applyNeuromodulation(combinedModulation);
        }
    }
}

void BrainStepProcessor::executeCognition(TimestepDuration dt) {
    if (cognitionSystem) {
        cognitionSystem->updateCognition(dt);
    }
}

void BrainStepProcessor::executeDevelopment(SimulationStep currentStep, double timestep) {
    if (developmentSystem) {
        developmentSystem->updateDevelopment(timestep, *brain->pImpl->rng);
    }
}

void BrainStepProcessor::executeReplaySystem(SimulationStep currentStep) {
    stepsSinceReplay++;
    if (stepsSinceReplay >= brain->pImpl->replayInterval && memorySystem) {
        memorySystem->replayMemories(3, currentStep); // Replay 3 episodes
        stepsSinceReplay = 0;
    }
}

void BrainStepProcessor::executeCheckpointing(SimulationStep currentStep, Timestamp currentTime) {
    if (brain->pImpl->checkpointManager) {
        brain->pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

size_t BrainStepProcessor::getTotalSpikeCount() const {
    return brain->getTotalSpikeCount();
}

size_t BrainStepProcessor::getFiringNeuronCount() const {
    return brain->getFiringNeuronCount();
}

float BrainStepProcessor::getAverageFiringRate() const {
    return brain->getAverageFiringRate();
}

} // namespace nlm