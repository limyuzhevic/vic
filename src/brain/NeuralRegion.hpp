#ifndef NLM_NEURAL_REGION_HPP
#define NLM_NEURAL_REGION_HPP

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"

namespace nlm {

class NeuralRegion {
public:
    NeuralRegion(RegionId id, const std::string& name);
    ~NeuralRegion();
    
    // Disable copying
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    
    // Enable moving
    NeuralRegion(NeuralRegion&&) noexcept;
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    // Region identification
    RegionId getId() const;
    const std::string& getName() const;
    
    // Population management
    PopulationId addPopulation(size_t neuronCount, NeuronType type);
    size_t getPopulationCount() const;
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations();
    
    // Neuron access
    NeuronId addNeuron(NeuronType type);
    size_t getTotalNeuronCount() const;
    Neuron* getNeuron(NeuronId id);
    const Neuron* getNeuron(NeuronId id) const;
    const std::vector<Neuron*>& getAllNeurons();
    
    // Synapse management
    SynapseId addSynapse(NeuronId source, NeuronId target, float weight, Delay delay);
    size_t getSynapseCount() const;
    const std::vector<Synapse*>& getSynapses();
    const std::vector<Synapse*>& getSynapsesTo(NeuronId target) const;
    const std::vector<Synapse*>& getSynapsesFrom(NeuronId source) const;
    Synapse* getSynapse(SynapseId id);
    
    // Connectivity
    void initializeRandomConnectivity(RandomGenerator& rng, float probability, float excWeight, float inhWeight);
    
    // Statistics
    float getAverageSynapticWeight() const;
    float getExcitationInhibitionRatio() const;
    size_t getActiveNeuronCount() const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_NEURAL_REGION_HPP