#include "NeuromodulationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

struct NeuromodulationManager::Impl {
    Brain* brain;
};

NeuromodulationManager::NeuromodulationManager() {
    pImpl = std::make_unique<Impl>();
    dopamine_ = std::make_unique<Dopamine>();
    curiosity_ = std::make_unique<Curiosity>();
    predictionError_ = std::make_unique<PredictionError>();
    novelty_ = std::make_unique<Novelty>();
}

NeuromodulationManager::~NeuromodulationManager() = default;

void NeuromodulationManager::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize NeuromodulationManager: invalid brain pointer");
        return;
    }
    
    pImpl->brain = brain;
    dopamine_->initialize(pImpl->brain);
    curiosity_->initialize(pImpl->brain);
    predictionError_->initialize(pImpl->brain);
    novelty_->initialize(pImpl->brain);
    
    NLM_LOG_INFO("Neuromodulation systems initialized");
}

void NeuromodulationManager::update() {
    // Update neuromodulation systems based on neural activity
    // For now, placeholder implementation
    // TODO: Integrate with spike events, prediction errors, and sensory inputs
    
    NLM_LOG_DEBUG("Neuromodulation systems updated");
}

void NeuromodulationManager::configureFromConfig(const Config& config) {
    // Configure neuromodulation system parameters
    // TODO: Add neuromodulation system specific configuration options
    
    NLM_LOG_INFO("Neuromodulation systems configured from config");
}

void NeuromodulationManager::reset() {
    if (dopamine_) {
        dopamine_->reset();
    }
    if (curiosity_) {
        curiosity_->reset();
    }
    if (predictionError_) {
        predictionError_->reset();
    }
    if (novelty_) {
        novelty_->reset();
    }
    NLM_LOG_INFO("Neuromodulation systems reset");
}

} // namespace nlm