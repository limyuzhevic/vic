#pragma once

#include "../core/Types/Types.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

class NeuralProcessor {
public:
    NeuralProcessor(std::shared_ptr<SpikeSystem> spikeSystem,
                   std::shared_ptr<STDP> stdp,
                   std::shared_ptr<Hebbian> hebbian,
                   std::shared_ptr<StructuralPlasticity> structuralPlasticity);
    
    void step(SimulationStep currentStep, Timestamp currentTime);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    void updatePlasticity();
    void develop();
    size_t getTotalSpikeCount() const;
    size_t getTotalNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    RegionId addRegion(const std::string& name = "");
    void addInterRegionConnection(RegionId source, RegionId target,
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
