#include "DevelopmentManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>

namespace nlm {

struct DevelopmentManager::Impl {
    Brain* brain{nullptr};
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    Impl() = default;
};

DevelopmentManager::DevelopmentManager() : pImpl(new Impl()) {}

DevelopmentManager::~DevelopmentManager() {
    delete pImpl;
}

bool DevelopmentManager::initialize(Brain* brain) {
    if (!brain) {
        return false;
    }
    
    pImpl->brain = brain;
    
    // Initialize development system
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    
    status_.currentStage = DevelopmentalStage::Initial;
    status_.developmentalAge = 0.0;
    status_.criticalPeriodActive = false;
    status_.criticalPeriodProgress = 0.0f;
    status_.plasticityModifier = 1.0f;
    
    return true;
}

void DevelopmentManager::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    if (!brain || !pImpl->developmentSystem) {
        return;
    }
    
    // Update development system
    pImpl->developmentSystem->update(brain, rng, dt * 1000);
    
    // Update development status
    status_.currentStage = pImpl->developmentSystem->getStage();
    status_.developmentalAge = pImpl->developmentSystem->getDevelopmentalAge();
    status_.criticalPeriodActive = pImpl->developmentSystem->isCriticalPeriod();
    status_.criticalPeriodProgress = pImpl->developmentSystem->getCriticalPeriodProgress();
    status_.plasticityModifier = pImpl->developmentSystem->getPlasticityModifier();
}

void DevelopmentManager::setStage(DevelopmentalStage stage) {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->setStage(stage);
    }
    status_.currentStage = stage;
}

void DevelopmentManager::applyDevelopmentEffects(Brain* brain, float plasticityModifier) {
    if (!brain || !pImpl->developmentSystem) {
        return;
    }
    
    // Apply development effects on structural plasticity
    auto* sp = pImpl->developmentSystem->getDevelopmentSystem();
    if (sp) {
        sp->setSynaptogenesisRate(0.0001f * plasticityModifier);
        sp->setPruningRate(0.00001f * (2.0f - plasticityModifier));
    }
}

bool DevelopmentManager::validateConfiguration(std::shared_ptr<Config> config) const {
    if (!config) {
        return false;
    }
    
    // Validate developmental stage parameters
    bool valid = true;
    
    // Check for known developmental parameters
    auto criticalPeriodAge = config->get<double>("critical_period_age");
    auto maturationAge = config->get<double>("maturation_age");
    auto adultAge = config->get<double>("adult_age");
    
    if (criticalPeriodAge && (*criticalPeriodAge <= 0.0 || *criticalPeriodAge >= *maturationAge)) {
        valid = false;
    }
    
    if (maturationAge && (*maturationAge <= *criticalPeriodAge || *maturationAge >= *adultAge)) {
        valid = false;
    }
    
    if (adultAge && (*adultAge <= *maturationAge)) {
        valid = false;
    }
    
    return valid;
}

void DevelopmentManager::advanceStage() {
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->advanceStage();
    }
}

} // namespace nlm
