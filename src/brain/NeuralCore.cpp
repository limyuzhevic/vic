#include "NeuralCore.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Config/ConfigConstants.hpp"
#include "../core/Logger/Logger.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <algorithm>
#include <limits>

namespace nlm {

struct NeuralCore::Impl {
    std::vector<std::unique_ptr<NeuralRegion>>* regions;
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Working memory
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    Impl() : regions(nullptr), totalSpikesThisStep(0), totalSpikesTotal(0) {
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
    }
};

NeuralCore::NeuralCore() : pImpl(std::make_unique<Impl>()) {}

NeuralCore::~NeuralCore() = default;

bool NeuralCore::initialize(std::shared_ptr<Config> config, 
                           const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    pImpl->regions = const_cast<std::vector<std::unique_ptr<NeuralRegion>>*>(&regions);
    
    // Configure plasticity systems from config
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    pImpl->structuralPlasticity->setPruningRate(pruningRate);
    
    pImpl->timestep = config->getOr<double>("simulation_timestep", 0.001);
    
    // Register spike handlers
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        if (!pImpl->regions) return;
        
        for (auto& region : *pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current
                    MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;
                }
            }
        }
    });
    
    return true;
}

void NeuralCore::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Process pending delayed spikes (deliver synaptic input)
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // Update all neurons (LIF dynamics)
    processNeurons(currentStep, currentTime);
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // Store working memory traces
    storeWorkingMemoryTraces();
}

void NeuralCore::injectCurrent(NeuronId neuron, MembranePotential current) {
    if (!pImpl->regions) return;
    
    for (auto& region : *pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void NeuralCore::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    if (!pImpl->regions) return;
    
    for (auto& region : *pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

size_t NeuralCore::getTotalNeuronCount() const {
    if (!pImpl->regions) return 0;
    size_t total = 0;
    for (const auto& region : *pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t NeuralCore::getTotalSynapseCount() const {
    if (!pImpl->regions) return 0;
    size_t total = 0;
    for (const auto& region : *pImpl->regions) {
        total += region->getSynapseCount();
    }
    return total;
}

size_t NeuralCore::getActiveNeuronCount() const {
    if (!pImpl->regions) return 0;
    size_t total = 0;
    for (const auto& region : *pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t NeuralCore::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float NeuralCore::getAverageFiringRate() const {
    if (!pImpl->regions || pImpl->regions->empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : *pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions->size());
}

float NeuralCore::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    if (!pImpl->regions) return 0.0f;
    
    for (const auto& region : *pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

SpikeSystem* NeuralCore::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* NeuralCore::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* NeuralCore::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* NeuralCore::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* NeuralCore::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

void NeuralCore::processNeurons(SimulationStep currentStep, Timestamp currentTime) {
    if (!pImpl->regions) return;
    
    for (auto& region : *pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

void NeuralCore::processSpikes(SimulationStep currentStep) {
    pImpl->spikeSystem->processSpikes(currentStep);
}

void NeuralCore::applyPlasticity(float plasticityMod, Timestamp currentTime) {
    if (!pImpl->regions) return;
    
    // Apply STDP with neuromodulation
    for (auto& region : *pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void NeuralCore::reset() {
    if (!pImpl->regions) return;
    
    for (auto& region : *pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
}

const std::vector<std::unique_ptr<NeuralRegion>>& NeuralCore::getRegions() const {
    static const std::vector<std::unique_ptr<NeuralRegion>> empty;
    return pImpl->regions ? *pImpl->regions : empty;
}

void NeuralCore::storeWorkingMemoryTraces() {
    if (!pImpl->regions) return;
    
    for (auto& region : *pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring() || 
                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                    
                    float activation = std::abs(neuron->getState().membranePotential - 
                                             neuron->getState().restingPotential) / 20.0f;
                    
                    // Store in working memory if available
                    // This would integrate with the actual working memory system
                    // For now, we'll just log the active neurons
                    if (activation > 0.1f) {
                        // In a real implementation, this would call:
                        // brain->getWorkingMemory()->storeToNeuron(neuron->getId(), activation);
                    }
                }
            }
        }
    }
}

void NeuralCore::applyDevelopmentEffects(const DevelopmentSystem& devSystem, 
                                        DevelopmentalStage stage, float plasticityMod) {
    if (!pImpl->regions) return;
    
    // Apply structural plasticity based on development stage
    for (auto& region : *pImpl->regions) {
        // In a real implementation, this would call:
        // devSystem.applyToRegion(region.get(), stage, plasticityMod);
    }
}

} // namespace nlm