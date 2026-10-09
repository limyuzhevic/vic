#pragma once

#include "../neural/NeuralProcessor.hpp"
#include "../plasticity/PlasticityManager.hpp"
#include "../memory/MemorySystem.hpp"
#include "../cognition/CognitionSystem.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

class BrainStepProcessor {
public:
    BrainStepProcessor(Brain* brain,
                     NeuralProcessor* neuralProcessor,
                     PlasticityManager* plasticityManager,
                     MemorySystem* memorySystem,
                     CognitionSystem* cognitionSystem,
                     DevelopmentSystem* developmentSystem);
    
    // Execute the complete brain step with all integrated systems
    void executeStep(SimulationStep currentStep, Timestamp currentTime);
    
    // Individual system execution methods
    void executeNeuronalDynamics(TimestepDuration dt);
    void executeSpikingSystem(SimulationStep currentStep, Timestamp currentTime);
    void executeMemorySystems(SimulationStep currentStep, Timestamp currentTime);
    void executePlasticity(SimulationStep currentStep, Timestamp currentTime);
    void executeNeuromodulation(TimestepDuration dt);
    void executeCognition(TimestepDuration dt);
    void executeDevelopment(SimulationStep currentStep, double timestep);
    void executeReplaySystem(SimulationStep currentStep);
    void executeCheckpointing(SimulationStep currentStep, Timestamp currentTime);
    
    // Get system statistics
    size_t getTotalSpikeCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
private:
    Brain* brain; // Add reference to Brain for access to subsystems
    NeuralProcessor* neuralProcessor;
    PlasticityManager* plasticityManager;
    MemorySystem* memorySystem;
    CognitionSystem* cognitionSystem;
    DevelopmentSystem* developmentSystem;
    
    // Neuromodulation tracking
    Neuromodulator currentNeuromodulation;
    
    // Timing and state
    size_t stepsSinceReplay;
    size_t stepsSinceConsolidation;
    bool isResting;
};
