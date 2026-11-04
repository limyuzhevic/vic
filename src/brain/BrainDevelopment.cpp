#include "BrainDevelopment.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

BrainDevelopment::BrainDevelopment()
    : developmentalStage(DevelopmentalStage::Initial)
    , developmentRate(0.001f)
    , plasticityModulation(1.0f) {}

BrainDevelopment::~BrainDevelopment() = default;

bool BrainDevelopment::initialize(BrainCore* core) {
    this->core = core;
    
    if (!core) {
        NLM_LOG_ERROR("Cannot initialize development system without core brain");
        return false;
    }
    
    // Initialize development system
    developmentSystem = std::make_unique<DevelopmentSystem>();
    developmentSystem->initialize(core);
    
    NLM_LOG_INFO("Development system initialized");
    return true;
}

void BrainDevelopment::update(BrainCore* core, Timestamp elapsedTime) {
    if (!core || !developmentSystem) return;
    
    // Update development system
    developmentSystem->update(core, elapsedTime);
    
    // Apply development effects
    applyDevelopmentEffects(core);
}

void BrainDevelopment::applyDevelopmentEffects(BrainCore* core) {
    if (!core || !developmentSystem) return;
    
    // Get current developmental stage
    DevelopmentalStage stage = developmentalStage;
    switch (stage) {
        case DevelopmentalStage::Initial:
            // Initial stage: high plasticity, rapid development
            developmentRate = 0.001f;
            plasticityModulation = 1.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Critical period: moderate plasticity, selective development
            developmentRate = 0.0005f;
            plasticityModulation = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            // Maturation: decreasing plasticity, refinement
            developmentRate = 0.0002f;
            plasticityModulation = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            // Adult: stable plasticity, maintenance
            developmentRate = 0.0001f;
            plasticityModulation = 0.2f;
            break;
    }
    
    // Apply developmental effects to synaptic plasticity
    applyPlasticityModulation(core);
}

void BrainDevelopment::updateSynaptogenesis(BrainCore* core, Timestamp elapsedTime) {
    if (!core || !developmentSystem) return;
    
    // Update synaptogenesis based on developmental stage
    float synaptogenesisRate = developmentRate * plasticityModulation;
    core->getStructuralPlasticity()->setSynaptogenesisRate(synaptogenesisRate);
}

void BrainDevelopment::updatePruning(BrainCore* core, Timestamp elapsedTime) {
    if (!core || !developmentSystem) return;
    
    // Update pruning based on developmental stage
    float pruningRate = 0.00001f * (2.0f - plasticityModulation);
    core->getStructuralPlasticity()->setPruningRate(pruningRate);
}

void BrainDevelopment::updateMaturation(BrainCore* core, Timestamp elapsedTime) {
    if (!core || !developmentSystem) return;
    
    // Update maturation based on elapsed time
    static Timestamp lastMaturationTime = 0.0;
    Timestamp currentMaturationTime = core->currentTime();
    
    if (currentMaturationTime - lastMaturationTime > 10000.0) {  // 10 seconds
        // Maturation effects
        developmentRate *= 0.95f;  // Decrease development rate
        lastMaturationTime = currentMaturationTime;
        
        NLM_LOG_INFO("Development maturation: rate decreased to " + std::to_string(developmentRate));
    }
}

void BrainDevelopment::applyPlasticityModulation(BrainCore* core) {
    if (!core) return;
    
    // Apply plasticity modulation to neurons
    for (auto* neuron : core->getSensoryNeurons()) {
        // Development affects neural excitability
        float excitabilityMod = plasticityModulation * 0.1f;
        if (excitabilityMod > 0.0f) {
            neuron->injectCurrent(excitabilityMod);
        }
    }
}

} // namespace nlm
