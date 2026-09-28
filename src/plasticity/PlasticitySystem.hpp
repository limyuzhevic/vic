#include "PlasticitySystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct PlasticitySystem::Impl {
    Brain* brain = nullptr;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Configuration
    float stdpLTPWeight = 0.01f;
    float stdpLTDWeight = 0.012f;
    float stdpTau = 20.0f;
    float hebbianLearningRate = 0.01f;
    float structuralPlasticityRate = 0.0001f;
    float pruningRate = 0.00001f;
    
    // Neuromodulation effects
    float dopamineModulation = 1.0f;
    float noveltyModulation = 1.0f;
    float developmentPlasticityModifier = 1.0f;
    
    // Statistics
    size_t totalSTDPUpdates = 0;
    size_t totalHebbianUpdates = 0;
    size_t totalStructuralUpdates = 0;
    float averageWeightChange = 0.0f;
    
    // Spike event counts
    size_t preSynapticSpikeCount = 0;
    size_t postSynapticSpikeCount = 0;
    
    // Spike history
    std::vector<DetailedSpikeEvent> recentSpikeHistory;
    std::vector<DelayedSpikeEvent> recentDelayedSpikeHistory;
    
    // Eligibility traces
    std::vector<float> stdpEligibilityTraces;
    std::vector<float> hebbianTraces;
    
    // Timing
    double lastSTDPUpdate = 0.0;
    double lastHebbianUpdate = 0.0;
    double lastStructuralUpdate = 0.0;
    
    // Structural plasticity scheduling
    double nextStructuralPlasticityTime = 0.0;
    double structuralPlasticityInterval = 100.0; // Steps
    
    // State
    bool initialized = false;
};

PlasticitySystem::PlasticitySystem() : pImpl(std::make_unique<Impl>()) {}

PlasticitySystem::~PlasticitySystem() = default;

void PlasticitySystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize plasticity systems
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Configure with default parameters
    pImpl->stdp->configure(pImpl->stdpLTPWeight, pImpl->stdpLTDWeight, pImpl->stdpTau);
    pImpl->hebbian->setLearningRate(pImpl->hebbianLearningRate);
    pImpl->structuralPlasticity->setSynaptogenesisRate(pImpl->structuralPlasticityRate);
    pImpl->structuralPlasticity->setPruningRate(pImpl->pruningRate);
    
    // Reserve memory for history
    pImpl->stdpEligibilityTraces.reserve(10000);
    pImpl->hebbianTraces.reserve(10000);
    
    pImpl->initialized = true;
    
    NLM_LOG_INFO("PlasticitySystem initialized");
}

void PlasticitySystem::update(float timestep) {
    if (!pImpl->initialized || !pImpl->brain) return;
    
    double currentStep = pImpl->brain->getCurrentStep();
    
    // Process recent spike history
    processSpikeHistory(currentStep);
    
    // Apply STDP learning
    applySTDP(pImpl->brain->getSTDPNeurons());
    
    // Apply Hebbian learning
    applyHebbianLearning(pImpl->brain->getHebbianNeurons());
    
    // Apply structural plasticity periodically
    if (currentStep >= pImpl->nextStructuralPlasticityTime) {
        applyStructuralPlasticity();
        pImpl->nextStructuralPlasticityTime = currentStep + pImpl->structuralPlasticityInterval;
    }
    
    // Update plasticity system state
    pImpl->lastSTDPUpdate = currentStep;
    pImpl->lastHebbianUpdate = currentStep;
    
    NLM_LOG_DEBUG("PlasticitySystem updated");
}

void PlasticitySystem::applySTDP(const std::vector<Synapse*>& synapses) {
    if (synapses.empty() || !pImpl->stdp) return;
    
    float totalWeightChange = 0.0f;
    int updateCount = 0;
    
    for (Synapse* syn : synapses) {
        if (!syn || !syn->getPlasticityFlags().stdp) continue;
        
        // Get spike history from synapse
        const auto& preSpikes = syn->getPreSpikeHistory();
        const auto& postSpikes = syn->getPostSpikeHistory();
        
        if (!preSpikes.empty() && !postSpikes.empty()) {
            // Apply STDP with neuromodulation
            float weightChange = pImpl->stdp->update(syn, preSpikes, postSpikes, 0.001f); // timestep not available in this call
            
            // Apply neuromodulation scaling
            weightChange *= (pImpl->dopamineModulation + pImpl->noveltyModulation) * 0.5f;
            
            // Apply development stage modifier
            weightChange *= pImpl->developmentPlasticityModifier;
            
            // Update synapse weight
            float oldWeight = syn->getWeight();
            float newWeight = std::max(-1.0f, std::min(1.0f, oldWeight + weightChange));
            syn->setWeight(newWeight);
            
            totalWeightChange += std::abs(newWeight - oldWeight);
            ++updateCount;
            
            pImpl->totalSTDPUpdates++;
        }
    }
    
    if (updateCount > 0) {
        pImpl->averageWeightChange = totalWeightChange / static_cast<float>(updateCount);
    }
    
    NLM_LOG_DEBUG("PlasticitySystem applied STDP: " + std::to_string(updateCount) + " updates");
}

void PlasticitySystem::applyHebbianLearning(const std::vector<Synapse*>& synapses) {
    if (synapses.empty() || !pImpl->hebbian) return;
    
    float totalWeightChange = 0.0f;
    int updateCount = 0;
    
    for (Synapse* syn : synapses) {
        if (!syn || !syn->getPlasticityFlags().hebbian) continue;
        
        // Get spike history from synapse
        const auto& preSpikes = syn->getPreSpikeHistory();
        const auto& postSpikes = syn->getPostSpikeHistory();
        
        if (!preSpikes.empty() && !postSpikes.empty()) {
            // Apply Hebbian learning with neuromodulation
            float weightChange = pImpl->hebbian->update(syn, preSpikes, postSpikes, 0.001f);
            
            // Apply neuromodulation scaling
            weightChange *= pImpl->dopamineModulation;
            
            // Apply development stage modifier
            weightChange *= pImpl->developmentPlasticityModifier;
            
            // Update synapse weight
            float oldWeight = syn->getWeight();
            float newWeight = std::max(-1.0f, std::min(1.0f, oldWeight + weightChange));
            syn->setWeight(newWeight);
            
            totalWeightChange += std::abs(newWeight - oldWeight);
            ++updateCount;
            
            pImpl->totalHebbianUpdates++;
        }
    }
    
    if (updateCount > 0) {
        float hebbianAverage = totalWeightChange / static_cast<float>(updateCount);
        pImpl->averageWeightChange = (pImpl->averageWeightChange * pImpl->totalSTDPUpdates + hebbianAverage * updateCount) /
                                    static_cast<float>(pImpl->totalSTDPUpdates + updateCount);
    }
    
    NLM_LOG_DEBUG("PlasticitySystem applied Hebbian learning: " + std::to_string(updateCount) + " updates");
}

void PlasticitySystem::applyStructuralPlasticity() {
    if (!pImpl->structuralPlasticity || !pImpl->brain) return;
    
    pImpl->totalStructuralUpdates++;
    
    // Apply structural plasticity with current modifiers
    pImpl->structuralPlasticity->update(pImpl->brain, *pImpl->brain->getRandomGenerator());
    
    NLM_LOG_DEBUG("PlasticitySystem applied structural plasticity");
}

void PlasticitySystem::recordPreSynapticSpike(NeuronId sourceNeuron, NeuronId targetNeuron,
                                            Timestamp timestamp) {
    pImpl->preSynapticSpikeCount++;
    
    // Record in recent history
    DetailedSpikeEvent spike;
    spike.spike_type = SpikeType::PreSynaptic;
    spike.source_neuron = sourceNeuron;
    spike.destination_neuron = targetNeuron;
    spike.weight = 1.0f; // Default weight
    spike.is_excitatory = true;
    spike.timestamp = timestamp;
    spike.step = static_cast<SimulationStep>(timestamp / 0.001); // Approximate
    
    pImpl->recentSpikeHistory.push_back(spike);
    
    // Keep history limited
    if (pImpl->recentSpikeHistory.size() > 1000) {
        pImpl->recentSpikeHistory.erase(pImpl->recentSpikeHistory.begin());
    }
    
    // Update eligibility trace for STDP
    if (pImpl->stdpEligibilityTraces.size() < pImpl->recentSpikeHistory.size()) {
        pImpl->stdpEligibilityTraces.push_back(1.0f);
    }
}

void PlasticitySystem::recordPostSynapticSpike(NeuronId sourceNeuron, NeuronId targetNeuron,
                                             Timestamp timestamp) {
    pImpl->postSynapticSpikeCount++;
    
    // Record in recent history
    DetailedSpikeEvent spike;
    spike.spike_type = SpikeType::PostSynaptic;
    spike.source_neuron = sourceNeuron;
    spike.destination_neuron = targetNeuron;
    spike.weight = 1.0f; // Default weight
    spike.is_excitatory = true;
    spike.timestamp = timestamp;
    spike.step = static_cast<SimulationStep>(timestamp / 0.001); // Approximate
    
    pImpl->recentSpikeHistory.push_back(spike);
    
    // Keep history limited
    if (pImpl->recentSpikeHistory.size() > 1000) {
        pImpl->recentSpikeHistory.erase(pImpl->recentSpikeHistory.begin());
    }
    
    // Update eligibility trace for Hebbian learning
    if (pImpl->hebbianTraces.size() < pImpl->recentSpikeHistory.size()) {
        pImpl->hebbianTraces.push_back(1.0f);
    }
}

void PlasticitySystem::storeSpikeHistory(const std::vector<DetailedSpikeEvent>& spikes) {
    pImpl->recentSpikeHistory.insert(pImpl->recentSpikeHistory.end(), 
                                   spikes.begin(), spikes.end());
    
    // Keep history limited
    if (pImpl->recentSpikeHistory.size() > 10000) {
        size_t excess = pImpl->recentSpikeHistory.size() - 10000;
        pImpl->recentSpikeHistory.erase(pImpl->recentSpikeHistory.begin(), 
                                       pImpl->recentSpikeHistory.begin() + excess);
    }
}

void PlasticitySystem::storeDelayedSpikeHistory(const std::vector<DelayedSpikeEvent>& spikes) {
    pImpl->recentDelayedSpikeHistory.insert(pImpl->recentDelayedSpikeHistory.end(),
                                          spikes.begin(), spikes.end());
    
    // Keep history limited
    if (pImpl->recentDelayedSpikeHistory.size() > 10000) {
        size_t excess = pImpl->recentDelayedSpikeHistory.size() - 10000;
        pImpl->recentDelayedSpikeHistory.erase(pImpl->recentDelayedSpikeHistory.begin(),
                                              pImpl->recentDelayedSpikeHistory.begin() + excess);
    }
}

void PlasticitySystem::processSpikeHistory(double currentStep) {
    // Process spike history for plasticity computation
    for (const auto& spike : pImpl->recentSpikeHistory) {
        if (currentStep - spike.timestamp > 100.0) { // 100 time units lifetime
            continue;
        }
        
        // Update eligibility traces based on spike timing
        if (!pImpl->stdpEligibilityTraces.empty()) {
            float decay = std::exp(-(currentStep - spike.timestamp) / 10.0f);
            for (float& trace : pImpl->stdpEligibilityTraces) {
                trace *= decay;
            }
        }
    }
}

void PlasticitySystem::configure(float stdpLTPWeight, float stdpLTDWeight, float stdpTau,
                                float hebbianLearningRate, float structuralPlasticityRate,
                                float pruningRate) {
    pImpl->stdpLTPWeight = stdpLTPWeight;
    pImpl->stdpLTDWeight = stdpLTDWeight;
    pImpl->stdpTau = stdpTau;
    pImpl->hebbianLearningRate = hebbianLearningRate;
    pImpl->structuralPlasticityRate = structuralPlasticityRate;
    pImpl->pruningRate = pruningRate;
    
    // Apply configuration to plasticity systems
    if (pImpl->stdp) {
        pImpl->stdp->configure(stdpLTPWeight, stdpLTDWeight, stdpTau);
    }
    if (pImpl->hebbian) {
        pImpl->hebbian->setLearningRate(hebbianLearningRate);
    }
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->setSynaptogenesisRate(structuralPlasticityRate);
        pImpl->structuralPlasticity->setPruningRate(pruningRate);
    }
    
    NLM_LOG_INFO("PlasticitySystem configured");
}

void PlasticitySystem::setNeuromodulatedLearningRates(float dopamineLevel, float noveltyLevel) {
    pImpl->dopamineModulation = dopamineLevel;
    pImpl->noveltyModulation = noveltyLevel;
    
    NLM_LOG_INFO("PlasticitySystem neuromodulation rates updated");
}

void PlasticitySystem::applyDevelopmentModifiers(float plasticityModifier) {
    pImpl->developmentPlasticityModifier = plasticityModifier;
    
    NLM_LOG_INFO("PlasticitySystem development modifiers applied");
}

PlasticitySystem::PlasticityState PlasticitySystem::getState() const {
    PlasticityState state;
    state.stdpLTPWeight = pImpl->stdpLTPWeight;
    state.stdpLTDWeight = pImpl->stdpLTDWeight;
    state.stdpTau = pImpl->stdpTau;
    state.hebbianLearningRate = pImpl->hebbianLearningRate;
    state.structuralPlasticityRate = pImpl->structuralPlasticityRate;
    state.pruningRate = pImpl->pruningRate;
    state.currentDopamineModulation = pImpl->dopamineModulation;
    state.currentNoveltyModulation = pImpl->noveltyModulation;
    state.developmentPlasticityModifier = pImpl->developmentPlasticityModifier;
    state.stdpEligibilityTraces = pImpl->stdpEligibilityTraces;
    state.hebbianTraces = pImpl->hebbianTraces;
    state.lastSTDPUpdate = pImpl->lastSTDPUpdate;
    state.lastHebbianUpdate = pImpl->lastHebbianUpdate;
    state.lastStructuralUpdate = pImpl->lastStructuralUpdate;
    
    return state;
}

void PlasticitySystem::restoreState(const PlasticityState& state) {
    pImpl->stdpLTPWeight = state.stdpLTPWeight;
    pImpl->stdpLTDWeight = state.stdpLTDWeight;
    pImpl->stdpTau = state.stdpTau;
    pImpl->hebbianLearningRate = state.hebbianLearningRate;
    pImpl->structuralPlasticityRate = state.structuralPlasticityRate;
    pImpl->pruningRate = state.pruningRate;
    pImpl->dopamineModulation = state.currentDopamineModulation;
    pImpl->noveltyModulation = state.currentNoveltyModulation;
    pImpl->developmentPlasticityModifier = state.developmentPlasticityModifier;
    pImpl->stdpEligibilityTraces = state.stdpEligibilityTraces;
    pImpl->hebbianTraces = state.hebbianTraces;
    pImpl->lastSTDPUpdate = state.lastSTDPUpdate;
    pImpl->lastHebbianUpdate = state.lastHebbianUpdate;
    pImpl->lastStructuralUpdate = state.lastStructuralUpdate;
    
    // Reconfigure plasticity systems with restored state
    if (pImpl->stdp) {
        pImpl->stdp->configure(state.stdpLTPWeight, state.stdpLTDWeight, state.stdpTau);
    }
    if (pImpl->hebbian) {
        pImpl->hebbian->setLearningRate(state.hebbianLearningRate);
    }
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->setSynaptogenesisRate(state.structuralPlasticityRate);
        pImpl->structuralPlasticity->setPruningRate(state.pruningRate);
    }
    
    NLM_LOG_INFO("PlasticitySystem state restored");
}

void PlasticitySystem::registerSpikeHandlers() {
    if (!pImpl->brain || !pImpl->brain->getSpikeSystem()) return;
    
    // Register spike handlers for plasticity
    pImpl->brain->getSpikeSystem()->registerHandler([this](const DetailedSpikeEvent& event) {
        // Record pre-synaptic spike for plasticity
        this->recordPreSynapticSpike(event.source_neuron, event.destination_neuron, event.timestamp);
    });
    
    pImpl->brain->getSpikeSystem()->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Record post-synaptic spike for plasticity
        this->recordPostSynapticSpike(event.source_neuron, event.destination_neuron, event.timestamp);
    });
    
    NLM_LOG_INFO("PlasticitySystem spike handlers registered");
}

size_t PlasticitySystem::getTotalSTDPUpdates() const {
    return pImpl->totalSTDPUpdates;
}

size_t PlasticitySystem::getTotalHebbianUpdates() const {
    return pImpl->totalHebbianUpdates;
}

size_t PlasticitySystem::getTotalStructuralUpdates() const {
    return pImpl->totalStructuralUpdates;
}

float PlasticitySystem::getAverageWeightChange() const {
    return pImpl->averageWeightChange;
}

float PlasticitySystem::getAverageSynapticEfficacy() const {
    if (pImpl->brain) {
        const auto& synapses = pImpl->brain->getAllSynapses();
        if (!synapses.empty()) {
            float totalEfficacy = 0.0f;
            for (const auto* syn : synapses) {
                totalEfficacy += syn->getEfficacy();
            }
            return totalEfficacy / static_cast<float>(synapses.size());
        }
    }
    return 0.0f;
}

size_t PlasticitySystem::getEffectiveSynapseCount() const {
    if (pImpl->brain) {
        const auto& synapses = pImpl->brain->getAllSynapses();
        size_t effectiveCount = 0;
        for (const auto* syn : synapses) {
            if (syn->getEfficacy() > 0.1f) { // Threshold for effective synapse
                ++effectiveCount;
            }
        }
        return effectiveCount;
    }
    return 0;
}

size_t PlasticitySystem::getPreSynapticSpikeCount() const {
    return pImpl->preSynapticSpikeCount;
}

size_t PlasticitySystem::getPostSynapticSpikeCount() const {
    return pImpl->postSynapticSpikeCount;
}

void PlasticitySystem::reset() {
    // Reset plasticity systems
    if (pImpl->stdp) pImpl->stdp->reset();
    if (pImpl->hebbian) pImpl->hebbian->reset();
    if (pImpl->structuralPlasticity) pImpl->structuralPlasticity->reset();
    
    // Reset statistics
    pImpl->totalSTDPUpdates = 0;
    pImpl->totalHebbianUpdates = 0;
    pImpl->totalStructuralUpdates = 0;
    pImpl->averageWeightChange = 0.0f;
    pImpl->preSynapticSpikeCount = 0;
    pImpl->postSynapticSpikeCount = 0;
    
    // Reset history
    pImpl->recentSpikeHistory.clear();
    pImpl->recentDelayedSpikeHistory.clear();
    pImpl->stdpEligibilityTraces.clear();
    pImpl->hebbianTraces.clear();
    
    // Reset timing
    pImpl->lastSTDPUpdate = 0.0;
    pImpl->lastHebbianUpdate = 0.0;
    pImpl->lastStructuralUpdate = 0.0;
    pImpl->nextStructuralPlasticityTime = 0.0;
    
    // Reset neuromodulation effects
    pImpl->dopamineModulation = 1.0f;
    pImpl->noveltyModulation = 1.0f;
    pImpl->developmentPlasticityModifier = 1.0f;
    
    NLM_LOG_INFO("PlasticitySystem reset");
}

} // namespace nlm
