#ifndef NLM_BRAIN_NEURALREGION_HPP
#define NLM_BRAIN_NEURALREGION_HPP

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include <string>
#include <memory>
#include <vector>

namespace nlm {

class NeuralRegion {
public:
    NeuralRegion(RegionId id, const std::string& name);
    ~NeuralRegion();
    
    // Non-copyable
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    
    // Movable
    NeuralRegion(NeuralRegion&& other) noexcept;
    NeuralRegion& operator=(NeuralRegion&& other) noexcept;
    
    // Identification
    RegionId getId() const { return id; }
    const std::string& getName() const { return name; }
    
    // Neuron management
    void addNeuron(std::unique_ptr<Neuron> neuron);
    size_t getTotalNeuronCount() const;
    size_t getActiveNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Population management
    struct Population {
        PopulationId id;
        NeuronType type;
        std::vector<Neuron*> neurons;
    };
    
    PopulationId addPopulation(size_t neuronCount, NeuronType type);
    Population* getPopulation(PopulationId populationId);
    const Population* getPopulation(PopulationId populationId) const;
    size_t getPopulationCount() const { return populations.size(); }
    
    // Synapse management
    void addSynapse(std::unique_ptr<Synapse> synapse);
    size_t getSynapseCount() const;
    SynapticWeight getAverageSynapticWeight() const;
    
    // Connectivity
    void initializeRandomConnectivity(RandomGenerator& rng, float connectionProbability, 
                                      SynapticWeight minWeight, SynapticWeight maxWeight);
    
    // Neural operations
    void step(Timestamp currentTime, Timestamp timestep);
    void reset();
    
    // Access neurons by ID
    Neuron* getNeuron(NeuronId id);
    const Neuron* getNeuron(NeuronId id) const;
    
    // Get neurons by population
    const std::vector<Neuron*>& getPopulationNeurons(PopulationId populationId) const;
    
    // Get neurons by type
    const std::vector<Neuron*>& getNeuronsByType(NeuronType type) const;
    
    // Utility
    std::string toString() const;
    
    // Statistics
    size_t getExcitatorySynapseCount() const;
    size_t getInhibitorySynapseCount() const;

private:
    RegionId id;
    std::string name;
    std::vector<std::unique_ptr<Neuron>> neurons;
    std::vector<std::unique_ptr<Synapse>> synapses;
    std::vector<Population> populations;
};

} // namespace nlm

#endif // NLM_BRAIN_NEURALREGION_HPP
