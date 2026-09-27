// BrainMain.hpp - Brain main loop and system integration interface
#pragma once

#include "../brain/Types/SimulationStep.hpp"
#include "../brain/Types/Timestamp.hpp"
#include "../sensory/SensoryInput.hpp"

namespace nlm {

class Brain;

class BrainMain {
public:
    BrainMain();
    ~BrainMain();
    
    // Set reference to parent brain
    void setBrain(Brain* brain) { pImpl->brain = brain; }
    
    // Initialize sensory neuron references
    void initializeSensoryNeurons(std::vector<Neuron*>& sensoryNeurons, 
                                 std::vector<Neuron*>& motorNeurons);
    
    // Store neural activity to working memory
    void storeToWorkingMemory(NeuronId neuronId, float activation);
    
    // Update working memory with current neural activity
    void updateWorkingMemory(double timestep);
    
    // Update episodic memory with current experience
    void updateEpisodicMemory(SimulationStep step, Timestamp time);
    
    // Process memory replay for learning consolidation
    void processMemoryReplay(SimulationStep step);
    
    // Update prediction system with current sensory input
    void updatePredictionSystem(const SensoryInput& input);
    
    // Update cognition systems (planner, concept formation, attention)
    void updateCognitionSystems(double timestep);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm