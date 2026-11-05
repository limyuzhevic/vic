#include "BrainPlasticity.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

void BrainPlasticity::configureFromConfig(const Config* config) {
    if (!config) return;
    
    // Configure STDP parameters
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    stdp_.configure(ltpWeight, ltdWeight, tau);
    
    // Configure Hebbian parameters
    float hebbianRate = config->getOr<float>("hebbian_learning_rate", 0.01f);
    hebbian_.setLearningRate(hebbianRate);
    
    // Configure structural plasticity parameters
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    structuralPlasticity_.setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity_.setPruningRate(pruningRate);
    
    NLM_LOG_INFO("BrainPlasticity configured from config:");
    NLM_LOG_INFO("  STDP LTP weight: " + std::to_string(ltpWeight));
    NLM_LOG_INFO("  STDP LTD weight: " + std::to_string(ltdWeight));
    NLM_LOG_INFO("  STDP tau: " + std::to_string(tau));
    NLM_LOG_INFO("  Hebbian learning rate: " + std::to_string(hebbianRate));
    NLM_LOG_INFO("  Synaptogenesis rate: " + std::to_string(synaptogenesisRate));
    NLM_LOG_INFO("  Pruning rate: " + std::to_string(pruningRate));
}

void BrainPlasticity::setLearningRates(float stdpLtp, float stdpLtd, float hebbian) {
    stdp_.configure(stdpLtp, stdpLtd, 20.0f);
    hebbian_.setLearningRate(hebbian);
}

void BrainPlasticity::setStructuralPlasticityRates(float synaptogenesis, float pruning) {
    structuralPlasticity_.setSynaptogenesisRate(synaptogenesis);
    structuralPlasticity_.setPruningRate(pruning);
}

void BrainPlasticity::updateAllPlasticity(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    plasticityEventCount_ = 0;
    float totalWeightChange = 0.0f;
    
    // Apply neuromodulation factor
    float neuromodulationFactor = calculatePlasticityModulation(brain);
    
    // Update STDP with neuromodulation
    updateSTDP(brain, timestep);
    
    // Update Hebbian learning
    updateHebbian(brain, timestep);
    
    // Apply homeostatic scaling every 100 steps
    if (brain->getCurrentStep() > 0 && brain->getCurrentStep() % 100 == 0) {
        applyHomeostaticScaling(brain);
    }
    
    // Collect statistics
    collectPlasticityStats(brain);
    
    // Validate plasticity state
    validatePlasticityState(brain);
}

void BrainPlasticity::updateSTDP(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    float totalDelta = 0.0f;
    size_t updatedSynapses = 0;
    
    for (auto& syn : region->getSynapses()) {
        if (!syn->getPlasticityFlags().stdp) continue;
        
        const auto& preSpikes = syn->getPreSpikeHistory();
        const auto& postSpikes = syn->getPostSpikeHistory();
        
        if (!preSpikes.empty() && !postSpikes.empty()) {
            float beforeWeight = syn->getWeight();
            stdp_.update(syn.get(), preSpikes, postSpikes, timestep);
            float delta = syn->getWeight() - beforeWeight;
            totalDelta += std::abs(delta);
            ++updatedSynapses;
            
            // Apply neuromodulation factor to STDP
            float neuromodulationFactor = calculatePlasticityModulation(brain);
            float neuromodulatedDelta = delta * neuromodulationFactor;
            
            if (std::abs(neuromodulatedDelta) > 0.001f) {
                syn->setWeight(syn->getWeight() + neuromodulatedDelta);
            }
            
            ++plasticityEventCount_;
        }
    }
    
    // Log STDP activity
    if (updatedSynapses > 0) {
        NLM_LOG_INFO("  STDP updated " + std::to_string(updatedSynapses) + " synapses, total delta: " + std::to_string(totalDelta));
    }
}

void BrainPlasticity::updateHebbian(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    float totalDelta = 0.0f;
    size_t updatedSynapses = 0;
    
    for (auto& syn : region->getSynapses()) {
        if (!syn->getPlasticityFlags().hebbian) continue;
        
        const auto& preSpikes = syn->getPreSpikeHistory();
        const auto& postSpikes = syn->getPostSpikeHistory();
        
        if (!preSpikes.empty() && !postSpikes.empty()) {
            float beforeWeight = syn->getWeight();
            hebbian_.update(syn.get(), preSpikes, postSpikes, timestep);
            float delta = syn->getWeight() - beforeWeight;
            totalDelta += std::abs(delta);
            ++updatedSynapses;
            
            ++plasticityEventCount_;
        }
    }
    
    // Log Hebbian activity
    if (updatedSynapses > 0) {
        NLM_LOG_INFO("  Hebbian updated " + std::to_string(updatedSynapses) + " synapses, total delta: " + std::to_string(totalDelta));
    }
}

void BrainPlasticity::updateStructuralPlasticity(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    structuralPlasticity_.update(brain, rng);
    ++plasticityEventCount_;  // Count as one event for structural changes
    
    NLM_LOG_INFO("  Structural plasticity applied (synaptogenesis/pruning)");
}

void BrainPlasticity::applyHomeostaticScaling(Brain* brain) {
    if (!brain) return;
    
    constexpr float kIncomingExcitatoryCap = 3.0f;
    
    for (auto& region : brain->getRegions()) {
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

void BrainPlasticity::collectPlasticityStats(Brain* brain) {
    if (!brain) return;
    
    averageWeightChange_ = 0.0f;
    size_t totalSynapses = 0;
    
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            float weight = std::abs(syn->getWeight());
            averageWeightChange_ += weight;
            ++totalSynapses;
        }
    }
    
    if (totalSynapses > 0) {
        averageWeightChange_ /= static_cast<float>(totalSynapses);
    }
}

void BrainPlasticity::validatePlasticityState(Brain* brain) const {
    if (!brain) return;
    
    // Check for invalid weights
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (std::isnan(weight) || std::isinf(weight)) {
                NLM_LOG_ERROR("Invalid synaptic weight detected: " + std::to_string(weight));
            }
            
            // Check for runaway weights
            if (std::abs(weight) > 10.0f) {
                NLM_LOG_WARNING("Runaway synaptic weight: " + std::to_string(weight));
                syn->setWeight(std::clamp(weight, -1.0f, 1.0f));
            }
        }
    }
}

float BrainPlasticity::calculatePlasticityModulation(Brain* brain) const {
    if (!brain) return 1.0f;
    
    // Simple neuromodulation factor based on developmental stage
    float modulation = 1.0f;
    
    // Check for dopamine effects (if available)
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        float dopamineLevel = dopamine->getLevel();
        modulation += dopamineLevel * 0.5f;  // Dopamine enhances plasticity
    }
    
    return modulation;
}

void BrainPlasticity::enableSTDP(bool enable) {
    // This would need access to actual STDP configuration
    // For now, this is a placeholder for advanced configuration
    NLM_LOG_INFO("STDP " + std::string(enable ? "enabled" : "disabled"));
}

} // namespace nlm