#pragma once

#include "Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <vector>
#include <unordered_map>

namespace nlm {

class BrainCore {
public:
    BrainCore() = default;
    ~BrainCore() = default;
    
    // Core neural computation methods
    void initializeCore(std::shared_ptr<Brain> brain, size_t neuronCount, size_t regionCount, 
                       float connectionProbability);
    void createNeuralArchitecture(std::shared_ptr<Brain> brain, size_t neuronCount, size_t regionCount);
    void initializeConnectivity(std::shared_ptr<Brain> brain, float connectionProbability);
    
    // Neural dynamics and plasticity
    void processSpikes(std::shared_ptr<Brain> brain, SimulationStep currentStep, Timestamp currentTime);
    void updateNeurons(std::shared_ptr<Brain> brain, Timestamp currentTime, TimestepDuration timestep);
    void applyPlasticity(std::shared_ptr<Brain> brain, SimulationStep currentStep, Timestamp currentTime);
    void updateStructuralPlasticity(std::shared_ptr<Brain> brain, size_t currentStep);
    
    // Statistics and monitoring
    size_t getTotalNeuronCount(const Brain* brain) const;
    size_t getTotalSynapseCount(const Brain* brain) const;
    size_t getActiveNeuronCount(const Brain* brain) const;
    size_t getFiringNeuronCount(const Brain* brain) const;
    float getAverageFiringRate(const Brain* brain) const;
    float getExcitationInhibitionRatio(const Brain* brain) const;
    
    // Neuron population management
    void collectSensoryNeurons(Brain* brain, std::vector<Neuron*>& sensoryNeurons);
    void collectMotorNeurons(Brain* brain, std::vector<Neuron*>& motorNeurons);
    
    // Neural region operations
    NeuralRegion* getRegion(Brain* brain, RegionId id);
    const NeuralRegion* getRegion(const Brain* brain, RegionId id) const;
    
    // Random connectivity and weight initialization
    void initializeRandomConnectivity(NeuralRegion* region, RandomGenerator& rng, 
                                     float connectionProbability, float minWeight, float maxWeight);
    
    // Spike system integration
    void setupSpikeSystemHandlers(std::shared_ptr<Brain> brain);
    
private:
    // Helper methods for efficiency
    void processSpikesForRegion(NeuralRegion* region, SimulationStep currentStep, Timestamp currentTime);
    void updateNeuronsForPopulation(NeuralPopulation* pop, Timestamp currentTime, TimestepDuration timestep);
    void applyPlasticityForSynapse(Synapse* syn, SimulationStep currentStep, Timestamp currentTime, 
                                  float plasticityMod, STDP* stdp, Hebbian* hebbian);
};

} // namespace nlm