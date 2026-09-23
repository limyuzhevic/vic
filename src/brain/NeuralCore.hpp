#pragma once

#include "brain/NeuralRegion.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace nlm {

class NeuralCore {
public:
    NeuralCore();
    ~NeuralCore();
    
    // Disable copying, enable moving
    NeuralCore(const NeuralCore&) = delete;
    NeuralCore& operator=(const NeuralCore&) = delete;
    NeuralCore(NeuralCore&&) noexcept;
    NeuralCore& operator=(NeuralCore&&) noexcept;
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Neuron and synapse management
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // Neural computation utilities
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    std::vector<Neuron*> getNeuronsByType(NeuronType type) const;
    std::vector<Neuron*> getAllNeurons() const;
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Step methods
    void stepNeurons(Timestamp currentTime, TimestepDuration timestep);
    void detectAndProcessSpikes(SimulationStep currentStep, Timestamp currentTime);
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    void updateSynapticEligibility(SimulationStep currentStep, Timestamp currentTime);
    
    // Statistics and reset
    void resetAllRegions();
    void collectNeuralStatistics() const;
    
    // Configuration
    void setConnectionProbability(float probability);
    void setSynapticWeights(float excitation, float inhibition);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm