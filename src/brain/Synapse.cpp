#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Config/Config.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Synapse::Impl {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    SynapticWeight weight;
    Delay delay;  // Synaptic delay in simulation steps
    
    // Synapse type
    SynapseType type;
    
    // Spike history for STDP
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    
    // Plasticity state
    PlasticityFlags plasticityFlags;
    
    // Short-term plasticity state
    float shortTermDepression;  // Utilization factor (0-1)
    float shortTermFacilitation;  // Facilitation factor
    float lastPreSpikeTime;  // For short-term dynamics
    float lastPostSpikeTime;
    
    // Eligibility trace for reward-modulated learning
    float eligibilityTrace;
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    
    // Weight bounds - CONFIGURABLE
    Impl(std::shared_ptr<Config> config) : id(), sourceNeuron(), destinationNeuron(), weight(0.0f), delay(1),
                                           type(SynapseType::Excitatory), eligibilityTrace(0.0f), efficacy(1.0f),
                                           shortTermDepression(1.0f), shortTermFacilitation(0.0f),
                                           lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f), config(config) {}
    
    std::shared_ptr<Config> config;
};

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination, std::shared_ptr<Config> config) 
    : pImpl(new Impl(config)), weight(0.0f), delay(1), type(SynapseType::Excitatory),
      eligibilityTrace(0.0f), efficacy(1.0f), shortTermDepression(1.0f),
      shortTermFacilitation(0.0f), lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
}
