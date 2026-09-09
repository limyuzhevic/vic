#include "PlasticitySystemManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

struct PlasticitySystemManager::Impl {
    Brain* brain;
};

PlasticitySystemManager::PlasticitySystemManager() {
    pImpl = std::make_unique<Impl>();
    stdp_ = std::make_unique<STDP>();
    hebbian_ = std::make_unique<Hebbian>();
    structuralPlasticity_ = std::make_unique<StructuralPlasticity>();
}

PlasticitySystemManager::~PlasticitySystemManager() = default;

void PlasticitySystemManager::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize PlasticitySystemManager: invalid brain pointer");
        return;
    }
    
    pImpl->brain = brain;
    
    NLM_LOG_INFO("Plasticity systems initialized");
}

void PlasticitySystemManager::update() {
    // Plasticity is now applied during each brain step
    // This method is kept for API compatibility
    NLM_LOG_DEBUG("Plasticity systems updated");
}

void PlasticitySystemManager::configureFromConfig(const Config& config) {
    // Configure plasticity system parameters
    // TODO: Add plasticity system specific configuration options
    
    NLM_LOG_INFO("Plasticity systems configured from config");
}

void PlasticitySystemManager::reset() {
    if (stdp_) {
        stdp_->reset();
    }
    if (hebbian_) {
        hebbian_->reset();
    }
    if (structuralPlasticity_) {
        structuralPlasticity_->reset();
    }
    NLM_LOG_INFO("Plasticity systems reset");
}

} // namespace nlm