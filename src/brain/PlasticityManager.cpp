// PlasticityManager implementation
#include "BrainComponents.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

PlasticityManager::PlasticityManager(std::shared_ptr<Brain> brain, const BrainSystemConfig& config)
    : brain_(brain), config_(config), hebbianEnabled_(false), stdpEnabled_(false), rewardModulated_(false) {}

bool PlasticityManager::initialize() {
    NLM_LOG_INFO("Initializing PlasticityManager...");
    
    // Configure STDP parameters from config
    float ltpWeight = config_.stdpLtpWeight;
    float ltdWeight = config_.stdpLtdWeight;
    float tau = config_.stdpTau;
    
    if (brain_->getSTDP()) {
        brain_->getSTDP()->configure(ltpWeight, ltdWeight, tau);
    }
    
    // Configure structural plasticity parameters
    if (brain_->getStructuralPlasticity()) {
        brain_->getStructuralPlasticity()->setSynaptogenesisRate(config_.synaptogenesisRate);
        brain_->getStructuralPlasticity()->setPruningRate(config_.pruningRate);
    }
    
    NLM_LOG_INFO("PlasticityManager initialized successfully");
    return true;
}

void PlasticityManager::reset() {
    NLM_LOG_INFO("Resetting PlasticityManager...");
    hebbianEnabled_ = false;
    stdpEnabled_ = false;
    rewardModulated_ = false;
    NLM_LOG_INFO("PlasticityManager reset complete");
}

void PlasticityManager::update() {
    // Plasticity is applied during each simulation step
    // This method is kept for component architecture consistency
}

void PlasticityManager::logStatus() const {
    NLM_LOG_INFO("=== PlasticityManager Status ===");
    NLM_LOG_INFO("Hebbian enabled: " + std::to_string(hebbianEnabled_));
    NLM_LOG_INFO("STDP enabled: " + std::to_string(stdpEnabled_));
    NLM_LOG_INFO("Reward modulated: " + std::to_string(rewardModulated_));
    
    if (brain_->getSTDP()) {
        NLM_LOG_INFO("STDP LTP weight: " + std::to_string(brain_->getSTDP()->getLTPWeight()));
        NLM_LOG_INFO("STDP LTD weight: " + std::to_string(brain_->getSTDP()->getLTDWeight()));
    }
}

void PlasticityManager::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    hebbianEnabled_ = hebbian;
    stdpEnabled_ = stdp;
    rewardModulated_ = rewardModulated;
    
    NLM_LOG_INFO("Plasticity enabled - Hebbian: " + std::to_string(hebbian) +
                 ", STDP: " + std::to_string(stdp) +
                 ", Reward modulated: " + std::to_string(rewardModulated));
}

void PlasticityManager::applySTDP(SimulationStep step, Timestamp time) {
    if (brain_->getSTDP() && stdpEnabled_) {
        // Apply STDP based on spike history
        for (auto& region : brain_->getRegions()) {
            for (auto& syn : region->getSynapses()) {
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        brain_->getSTDP()->update(syn, preSpikes, postSpikes, time);
                    }
                }
            }
        }
    }
}

void PlasticityManager::applyHebbianLearning(SimulationStep step, Timestamp time) {
    if (hebbianEnabled_) {
        for (auto& region : brain_->getRegions()) {
            for (auto& syn : region->getSynapses()) {
                if (syn->getPlasticityFlags().hebbian) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        brain_->getHebbian()->update(syn, preSpikes, postSpikes, time);
                    }
                }
            }
        }
    }
}

void PlasticityManager::applyStructuralPlasticity(SimulationStep step, Timestamp time) {
    if (brain_->getStructuralPlasticity() && step % 100 == 0) {
        brain_->getStructuralPlasticity()->update(brain_.get(), *brain_->getRandomGenerator());
    }
}

void PlasticityManager::setSTDPParameters(float ltpWeight, float ltdWeight, float tau) {
    if (brain_->getSTDP()) {
        brain_->getSTDP()->configure(ltpWeight, ltdWeight, tau);
        NLM_LOG_INFO("STDP parameters updated: LTP=" + std::to_string(ltpWeight) +
                     ", LTD=" + std::to_string(ltdWeight) +
                     ", tau=" + std::to_string(tau));
    }
}

void PlasticityManager::setStructuralPlasticityParameters(float synaptogenesisRate, float pruningRate) {
    if (brain_->getStructuralPlasticity()) {
        brain_->getStructuralPlasticity()->setSynaptogenesisRate(synaptogenesisRate);
        brain_->getStructuralPlasticity()->setPruningRate(pruningRate);
        NLM_LOG_INFO("Structural plasticity parameters updated: synaptogenesis=" +
                     std::to_string(synaptogenesisRate) +
                     ", pruning=" + std::to_string(pruningRate));
    }
}

} // namespace nlm