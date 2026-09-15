#ifndef NLM_NEURAL_POPULATION_HPP
#define NLM_NEURAL_POPULATION_HPP

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"

namespace nlm {

class NeuralPopulation {
public:
    NeuralPopulation(PopulationId id, const std::string& name);
    ~NeuralPopulation();
    
    // Disable copying
    NeuralPopulation(const NeuralPopulation&) = delete;
    NeuralPopulation& operator=(const NeuralPopulation&) = delete;
    
    // Enable moving
    NeuralPopulation(NeuralPopulation&&) noexcept;
    NeuralPopulation& operator=(NeuralPopulation&&) noexcept;
    
    // Population identification
    PopulationId getId() const;
    const std::string& getName() const;
    
    // Neuron management
    NeuronId addNeuron(NeuronType type);
    size_t getNeuronCount() const;
    Neuron* getNeuron(NeuronId id);
    const Neuron* getNeuron(NeuronId id) const;
    const std::vector<Neuron*>& getNeurons();
    
    // Type-specific operations
    size_t getNeuronCount(NeuronType type) const;
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Statistics
    float getAverageMembranePotential() const;
    float getAverageFiringRate() const;
    size_t getFiringNeuronCount() const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_NEURAL_POPULATION_HPP