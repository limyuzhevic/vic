#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations for context classes
class PlasticityContext;
class MemoryEntry;
class State;
class Experience;

// Core data structures for plugins

// Context passed to plasticity rules
struct PlasticityContext {
    NeuronId preNeuron;
    NeuronId postNeuron;
    SynapseId synapse;
    Timestamp preSpikeTime;
    Timestamp postSpikeTime;
    SynapticWeight currentWeight;
    PlasticityFlags flags;
    float reward;
    
    PlasticityContext() : preNeuron(), postNeuron(), synapse()
        , preSpikeTime(0.0), postSpikeTime(0.0), currentWeight(0.0f)
        , reward(0.0f) {}
};

// Memory entry structure
struct MemoryEntry {
    std::string key;
    std::string value;
    Timestamp timestamp;
    std::vector<NeuronId> associatedNeurons;
    float importance;
    std::string metadata;
    
    MemoryEntry() : timestamp(0.0), importance(1.0f) {}
    MemoryEntry(const std::string& k, const std::string& v) 
        : key(k), value(v), timestamp(0.0), importance(1.0f) {}
};

// State for behavior modules
struct State {
    std::vector<float> features;
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronPotentials;
    std::vector<float> neuronFiringRates;
    Timestamp timestamp;
    SimulationStep step;
    
    State() : timestamp(0.0), step(0) {}
    
    // Get feature vector size
    size_t getFeatureCount() const { return features.size(); }
    
    // Get active neuron count
    size_t getActiveNeuronCount() const { return activeNeurons.size(); }
    
    // Get max feature value
    float getMaxFeature() const {
        if (features.empty()) return 0.0f;
        return *std::max_element(features.begin(), features.end());
    }
};

// Experience for reinforcement learning
struct Experience {
    State state;
    std::unique_ptr<Action> action;
    ActionResult result;
    float returnValue;
    Timestamp timestamp;
    SimulationStep step;
    
    Experience() : returnValue(0.0f), timestamp(0.0), step(0) {}
    
    // Get reward from result
    float getReward() const { return result.reward; }
    
    // Get action value
    float getActionValue() const { return result.actionValue; }
    
    // Check if experience is valid
    bool isValid() const {
        return action != nullptr && result.isValid();
    }
};

// Integration point declarations
class AgentBrain;
class Brain;
class SimpleWorld;
class NeuralPrediction;
class WorkingMemory;
class EpisodicMemory;
class ConceptFormation;
class NeuralPlanner;

} // namespace nlm