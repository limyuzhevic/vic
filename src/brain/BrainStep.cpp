// Brain Step Implementation - Contains all step-related functionality
#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// Forward declarations for step execution
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;

// Step execution functions
namespace StepExecution {

    // Execute the complete brain step loop
    void executeBrainStep(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    
    // Individual step stages
    void executeSpikeDelivery(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    void executeNeuronDynamics(Brain::Impl* impl, Timestamp currentTime, double timestep);
    void executeSpikeDetection(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    void executeWorkingMemoryUpdate(Brain::Impl* impl, double timestep);
    void executeNeuromodulationUpdate(Brain::Impl* impl, double timestep);
    void executePlasticityUpdate(Brain::Impl* impl, double timestep);
    void executeEpisodicMemoryUpdate(Brain::Impl* impl, SimulationStep currentStep);
    void executePredictionSystemUpdate(Brain::Impl* impl);
    void executeAttentionSystemUpdate(Brain::Impl* impl, double timestep);
    void executeConceptFormationUpdate(Brain::Impl* impl);
    void executeStructuralPlasticityUpdate(Brain::Impl* impl, SimulationStep currentStep, std::unique_ptr<RandomGenerator>& rng);
    void executeMemoryReplay(Brain::Impl* impl, SimulationStep currentStep);
    void executeDevelopmentUpdate(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime, double timestep);
    void executeMemoryConsolidation(Brain::Impl* impl, SimulationStep currentStep);
    void executeCheckpointManagement(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    
    // Helper functions for spike processing
    void processDelayedSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    void detectSpikesAndSchedule(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime);
    void processImmediateSpikes(Brain::Impl* impl, SimulationStep currentStep);
    
    // Integration with external systems
    void integrateWithMemorySystems(Brain::Impl* impl, SimulationStep currentStep, const TimestepDuration& timestep);
    void integrateWithPredictionSystems(Brain::Impl* impl);
    void integrateWithCognitionSystems(Brain::Impl* impl, SimulationStep currentStep, const TimestepDuration& timestep);
    void integrateWithDevelopmentSystems(Brain::Impl* impl, SimulationStep currentStep, const TimestepDuration& timestep);
    void integrateWithNeuromodulationSystems(Brain::Impl* impl, SimulationStep currentStep, const TimestepDuration& timestep);
}

} // namespace nlm