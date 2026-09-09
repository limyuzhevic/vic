#include "DevelopmentManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

struct DevelopmentManager::Impl {
    Brain* brain;
};

DevelopmentManager::DevelopmentManager() {
    pImpl = std::make_unique<Impl>();
    developmentSystem_ = std::make_unique<DevelopmentSystem>();
}

DevelopmentManager::~DevelopmentManager() = default;

void DevelopmentManager::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize DevelopmentManager: invalid brain pointer");
        return;
    }
    
    pImpl->brain = brain;
    developmentSystem_->initialize(pImpl->brain);
    
    NLM_LOG_INFO("Development system initialized");
}

void DevelopmentManager::update() {
    // Update development system
    // TODO: Integrate with simulation step and other systems
    NLM_LOG_DEBUG("Development system updated");
}

void DevelopmentManager::configureFromConfig(const Config& config) {
    // Configure development system parameters
    // TODO: Add development system specific configuration options
    
    NLM_LOG_INFO("Development system configured from config");
}

void DevelopmentManager::reset() {
    if (developmentSystem_) {
        developmentSystem_->reset();
    }
    NLM_LOG_INFO("Development system reset");
}

} // namespace nlm