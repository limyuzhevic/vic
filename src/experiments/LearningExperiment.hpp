// NLM (熙然) - Neural Learning Machine
// Learning Experiment Class
//
// This class provides a framework for running learning experiments
// that measure synaptic changes through neural activity.

#pragma once

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <vector>
#include <string>
#include <memory>
#include <numeric>
#include <algorithm>
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "dynamics/SpikeSystem.hpp"

namespace nlm {

/**
 * LearningExperiment - Framework for measuring synaptic changes through experience
 * 
 * This class provides a standardized way to:
 * 1. Record initial synaptic state
 * 2. Apply learning stimuli
 * 3. Record final synaptic state
 * 4. Compute and analyze learning metrics
 * 
 * The experiment demonstrates that neural networks can learn and change
 * their synaptic connections through repeated activity.
 */
class LearningExperiment {
public:
    /**
     * Constructor
     * 
     * @param brain Shared pointer to the brain being studied
     * @param seed Random seed for reproducible experiments
     */
    LearningExperiment(std::shared_ptr<Brain> brain, uint64_t seed);
    
    /**
     * Destructor
     */
    ~LearningExperiment();
    
    /**
     * Record the initial synaptic state before learning
     */
    void recordInitialState();
    
    /**
     * Record the final synaptic state after learning
     */
    void recordFinalState();
    
    /**
     * Run the complete learning experiment
     * 
     * @param steps Number of simulation steps to run
     * @param timestep Time step size for simulation
     */
    void runLearningExperiment(SimulationStep steps, double timestep);
    
    /**
     * Compute and display learning statistics
     */
    void computeStatistics();
    
    /**
     * Get the initial synaptic weights
     * 
     * @return Vector of initial synaptic weights
     */
    const std::vector<float>& getInitialWeights() const { return initialWeights; }
    
    /**
     * Get the final synaptic weights
     * 
     * @return Vector of final synaptic weights
     */
    const std::vector<float>& getFinalWeights() const { return finalWeights; }
    
    /**
     * Get the most active neurons during the experiment
     * 
     * @param count Number of most active neurons to return
     * @return Vector of neuron IDs for the most active neurons
     */
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    /**
     * Check if learning occurred
     * 
     * @return true if significant synaptic changes were detected
     */
    bool learningOccurred() const;
    
    /**
     * Get the total spike count during the experiment
     * 
     * @return Total number of spikes recorded
     */
    size_t getTotalSpikeCount() const { return brain ? brain->getTotalSpikeCount() : 0; }
    
private:
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    // Helper methods
    void clearWeights();
    float computeMean(const std::vector<float>& weights) const;
    size_t countSignificantChanges(const std::vector<float>& before, 
                                 const std::vector<float>& after, 
                                 float threshold = 0.01f) const;
};

} // namespace nlm

// Include implementation file
#include "experiments/LearningExperiment.cpp"
