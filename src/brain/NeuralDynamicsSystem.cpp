#include "NeuralDynamicsSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralDynamicsSystem::Impl {
    Brain* brain_;
    std::shared_ptr<Config> config_;
    size_t totalSpikes_;
    
    std::unique_ptr<SpikeSystem> spikeSystem_;
    std::unique_ptr<STDP> stdp_;
    std::unique_ptr<Hebbian> hebbian_;
    
    float currentDopamineLevel_;
    float currentNoveltyLevel_;
    float currentPredictionError_;
    
    // Statistics
    size_t totalExcitatoryConnections_;
    size_t totalInhibitoryConnections_;
    
    Impl() : brain_(nullptr), totalSpikes_(0), currentDopamineLevel_(0.0f),
             currentNoveltyLevel_(0.0f), currentPredictionError_(0.0f),
             totalExcitatoryConnections_(0), totalInhibitoryConnections_(0) {}
};

NeuralDynamicsSystem::NeuralDynamicsSystem() : pImpl(std::make_unique<Impl>()) {}
NeuralDynamicsSystem::~NeuralDynamicsSystem() = default;

void NeuralDynamicsSystem::initialize(Brain* brain, std::shared_ptr<Config> config) {
    pImpl->brain_ = brain;
    pImpl->config_ = config;
    
    // Initialize spike system
    pImpl->spikeSystem_ = std::make_unique<SpikeSystem>();
    
    // Initialize plasticity systems
    pImpl->stdp_ = std::make_unique<STDP>();
    pImpl->hebbian_ = std::make_unique<Hebbian>();
    
    // Configure STDP parameters
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    pImpl->stdp_->configure(ltpWeight, ltdWeight, tau);
    
    // Configure Hebbian parameters
    pImpl->hebbian_->configure(config->getOr<float>("hebbian_learning_rate", 0.001f),
                             config->getOr<float>("hebbian_threshold", 0.1f));
}

void NeuralDynamicsSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->spikeSystem_->processDelayedSpikes(currentStep, currentTime);
}

void NeuralDynamicsSystem::updateNeuronDynamics(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                                TimestepDuration timestep, Timestamp currentTime) {
    // Update all neurons (LIF dynamics) - equivalent to STEP 2
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void NeuralDynamicsSystem::detectAndQueueSpikes(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                                SimulationStep currentStep, Timestamp currentTime,
                                                std::unique_ptr<SpikeSystem>& spikeSystem) {
    // STEP 3: Detect spikes and schedule spike events
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem_->queueSpike(event);
                    pImpl->totalSpikes_++;

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            currentStep,
                            deliveryStep
                        );

                        pImpl->spikeSystem_->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                }
            }
        }
    }
}

void NeuralDynamicsSystem::applyNeuromodulationEffects(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                                        const Dopamine* dopamine) {
    // Dopamine modulates neural excitability by adjusting effective current injection
    if (dopamine) {
        float dopamineLevel = dopamine->getLevel();
        float excitabilityMod = dopamineLevel * 0.5f;
        
        if (excitabilityMod > 0.0f) {
            for (auto& region : regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Dopamine modulates excitability by injecting additional current
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void NeuralDynamicsSystem::applyPlasticityRules(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                                STDP* stdp, Hebbian* hebbian,
                                                float plasticityMod, TimestepDuration timestep,
                                                Timestamp currentTime) {
    // STEP 6: Apply plasticity rules (STDP and Hebbian)
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    stdp->update(syn.get(), preSpikes, postSpikes, timestep);
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
                    hebbian->update(syn.get(), preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void NeuralDynamicsSystem::applySynapticScaling(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                                SimulationStep currentStep) {
    // STEP 6b: Homeostatic synaptic scaling (Phase 4)
    if (currentStep > 0 && currentStep % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        for (auto& region : regions) {
            for (auto* neuron : region->getAllNeurons()) {
                auto incoming = region->getSynapsesTo(neuron->getId());
                float sum = 0.0f;
                for (Synapse* syn : incoming) {
                    if (syn->getWeight() > 0.0f) sum += syn->getWeight();
                }
                if (sum > kIncomingExcitatoryCap) {
                    float scale = kIncomingExcitatoryCap / sum;
                    for (Synapse* syn : incoming) {
                        if (syn->getWeight() > 0.0f) {
                            syn->setWeight(syn->getWeight() * scale);
                        }
                    }
                }
            }
        }
    }
}

void NeuralDynamicsSystem::updateWorkingMemory(const std::vector<Neuron*>& sensoryNeurons,
                                               std::unique_ptr<NeuralWorkingMemory>& workingMemory,
                                               SimulationStep currentStep) {
    // Store to working memory - neurons that fire become part of working memory
    for (auto* neuron : sensoryNeurons) {
        if (workingMemory) {
            const auto& state = neuron->getState();
            if (state.firingState == FiringState::Refractory) {
                workingMemory->storeToNeuron(neuron->getId(), 1.0f);
            }
        }
    }
}

float NeuralDynamicsSystem::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    // Note: This would need access to regions, so we'll update this later
    return totalInhibitory > 0.0f ? totalExcitatory / totalInhibitory : (totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f);
}

void NeuralDynamicsSystem::reset(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    pImpl->totalSpikes_ = 0;
    pImpl->spikeSystem_->reset();
    pImpl->stdp_->reset();
    pImpl->hebbian_->reset();
}

void NeuralDynamicsSystem::injectCurrent(NeuronId neuron, MembranePotential current) {
    if (pImpl->brain_) {
        pImpl->brain_->injectCurrent(neuron, current);
    }
}

void NeuralDynamicsSystem::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    if (pImpl->brain_) {
        pImpl->brain_->injectCurrentToNeurons(type, current);
    }
}

} // namespace nlm
