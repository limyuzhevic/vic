#include "DevelopmentSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    Brain* brain_;
    std::shared_ptr<Config> config_;
    DevelopmentalStage developmentalStage_;
    double age_;
    
    Impl() : brain_(nullptr), developmentalStage_(DevelopmentalStage::Initial), age_(0.0) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(std::make_unique<Impl>()) {}
DevelopmentSystem::~DevelopmentSystem() = default;

void DevelopmentSystem::initialize(Brain* brain, std::shared_ptr<Config> config) {
    pImpl->brain_ = brain;
    pImpl->config_ = config;
    pImpl->developmentalStage_ = DevelopmentalStage::Initial;
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    pImpl->age_ += dt;
    
    // Simple development progression based on age
    if (pImpl->age_ > 1000.0) {
        pImpl->developmentalStage_ = DevelopmentalStage::CriticalPeriod;
    } else if (pImpl->age_ > 5000.0) {
        pImpl->developmentalStage_ = DevelopmentalStage::Maturation;
    } else if (pImpl->age_ > 10000.0) {
        pImpl->developmentalStage_ = DevelopmentalStage::Adult;
    }
    
    // Update developmental stage
    if (brain) {
        brain->setDevelopmentalStage(pImpl->developmentalStage_);
    }
}

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->developmentalStage_;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->developmentalStage_ = stage;
}

float DevelopmentSystem::getPlasticityModifier() const {
    switch (pImpl->developmentalStage_) {
        case DevelopmentalStage::Initial:
            return 1.0f;  // High plasticity
        case DevelopmentalStage::CriticalPeriod:
            return 0.8f;
        case DevelopmentalStage::Maturation:
            return 0.5f;
        case DevelopmentalStage::Adult:
            return 0.2f;  // Stable
        default:
            return 1.0f;
    }
}

void DevelopmentSystem::reset() {
    pImpl->developmentalStage_ = DevelopmentalStage::Initial;
    pImpl->age_ = 0.0;
}

} // namespace nlm
