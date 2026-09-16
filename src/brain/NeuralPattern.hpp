#pragma once

#include <vector>
#include <string>
#include <memory>
#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Custom exception types for Python
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("Configuration Error: " + msg) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& msg) : NLMException("Simulation Error: " + msg) {}
};

class PatternError : public NLMException {
public:
    explicit PatternError(const std::string& msg) : NLMException("Pattern Error: " + msg) {}
};

// Neural Pattern - template for neuron/synapse configurations
class NeuralPattern {
public:
    NeuralPattern();
    ~NeuralPattern();
    
    // Add neuron to pattern
    NeuronId addNeuron(NeuronType type, float weight = 1.0f, 
                       const std::string& params = "");
    
    // Add synapse to pattern
    SynapseId addSynapse(NeuronId pre, NeuronId post, 
                        SynapseType type = SynapseType::Excitatory,
                        float weight = 1.0f);
    
    // Create population from pattern
    PopulationId createPopulation(Brain& brain, const std::string& regionName,
                                 size_t count = 1);
    
    // Get pattern neurons
    std::vector<NeuronId> getNeurons() const;
    
    // Get pattern synapses
    std::vector<SynapseId> getSynapses() const;
    
    // Get neuron properties
    std::string getNeuronParams(NeuronId id) const;
    float getNeuronWeight(NeuronId id) const;
    NeuronType getNeuronType(NeuronId id) const;
    
    // Get synapse properties
    float getSynapseWeight(SynapseId id) const;
    SynapseType getSynapseType(SynapseId id) const;
    
    // Convert to/from JSON string
    std::string toJSON() const;
    bool fromJSON(const std::string& json);
    
    // Pattern metadata
    std::string name() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    // Pattern description
    std::string description() const { return description_; }
    void setDescription(const std::string& desc) { description_ = desc; }
    
    // Generate description
    std::string generateDescription() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string name_;
    std::string description_;
};

} // namespace nlm