#include "DevelopmentSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../development/Maturation.hpp"
#include "../development/Pruning.hpp"
#include "../development/Synaptogenesis.hpp"
#include <algorithm>

namespace nlm {

struct DevelopmentSystem::Impl {
    std::shared_ptr<Config> config;
    Brain* brain = nullptr;
    std::unique_ptr<RandomGenerator> rng;
    
    // Development system components
    std::unique_ptr<Maturation> maturation;
    std::unique_ptr<Pruning> pruning;
    std::unique_ptr<Synaptogenesis> synaptogenesis;
    
    // Development state
    DevelopmentalStage developmentalStage;
    SimulationStep startStep;
    SimulationStep currentStep;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg), developmentalStage(DevelopmentalStage::Initial) {
        startStep = 0;
        currentStep = 0;
        rng = std::make_unique<RandomGenerator>(42);
    }
};

DevelopmentSystem::DevelopmentSystem(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)), brain_(nullptr) {
}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentSystem::DevelopmentSystem(DevelopmentSystem&& other) noexcept : pImpl(std::move(other.pImpl)), brain_(other.brain_) {
    other.brain_ = nullptr;
}

DevelopmentSystem& DevelopmentSystem::operator=(DevelopmentSystem&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        brain_ = other.brain_;
        other.brain_ = nullptr;
    }
    return *this;
}

bool DevelopmentSystem::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("DevelopmentSystem: Cannot initialize with null brain");
        return false;
    }
    
    brain_ = brain;
    pImpl->brain = brain;
    pImpl->rng = std::make_unique<RandomGenerator>(42);
    
    // Initialize development components
    pImpl->maturation = std::make_unique<Maturation>();
    pImpl->pruning = std::make_unique<Pruning>();
    pImpl->synaptogenesis = std::make_unique<Synaptogenesis>();
    
    pImpl->maturation->initialize(brain);
    pImpl->pruning->initialize(brain);
    pImpl->synaptogenesis->initialize(brain);
    
    // Set developmental stage
    setDevelopmentalStage(DevelopmentalStage::Initial);
    pImpl->startStep = 0;
    
    NLM_LOG_INFO("DevelopmentSystem initialized successfully");
    return true;
}

void DevelopmentSystem::update(SimulationStep step, Timestamp time, TimestepDuration dt) {
    pImpl->currentStep = step;
    
    // Update development components
    pImpl->maturation->update(brain_, pImpl->currentStep);
    pImpl->pruning->update(brain_, *pImpl->rng, dt);
    pImpl->synaptogenesis->update(brain_, *pImpl->rng, dt);
    
    // Update developmental stage based on step
    if (step % 1000 == 0) {
        // Update developmental stage every 1000 steps
        int stageIndex = static_cast<int>(developmentalStage_);
        if (stageIndex < static_cast<int>(DevelopmentalStage::Adult) - 1) {
            setDevelopmentalStage(static_cast<DevelopmentalStage>(stageIndex + 1));
        }
    }
}

DevelopmentalStage DevelopmentSystem::getDevelopmentalStage() const {
    return developmentalStage_;
}

void DevelopmentSystem::setDevelopmentalStage(DevelopmentalStage stage) {
    developmentalStage_ = stage;
    NLM_LOG_INFO("DevelopmentSystem: Stage changed to " + std::to_string(static_cast<int>(stage)));
}

float DevelopmentSystem::getDevelopmentProgress() const {
    if (pImpl->currentStep == 0) return 0.0f;
    // Simple linear progress from initial to adult
    return std::min(1.0f, static_cast<float>(pImpl->currentStep) / 10000.0f);
}

bool DevelopmentSystem::isCriticalPeriod(PlasticityFlags plasticityFlags) const {
    // Check if current developmental stage is a critical period for specific plasticity
    if (developmentalStage_ == DevelopmentalStage::CriticalPeriod) {
        return plasticityFlags.stdp || plasticityFlags.hebbian || plasticityFlags.rewardModulated;
    }
    return false;
}

float DevelopmentSystem::getPlasticityModulationFactor(PlasticityFlags plasticityFlags) const {
    // Return plasticity modulation factor based on developmental stage
    switch (developmentalStage_) {
        case DevelopmentalStage::Initial:
            return 1.0f;  // High plasticity
        case DevelopmentalStage::CriticalPeriod:
            return 0.8f;
        case DevelopmentalStage::Maturation:
            return 0.5f;
        case DevelopmentalStage::Adult:
            return 0.2f;  // Stable
        default:
            return 0.5f;
    }
}

void DevelopmentSystem::reset() {
    NLM_LOG_INFO("DevelopmentSystem reset");
    setDevelopmentalStage(DevelopmentalStage::Initial);
    pImpl->currentStep = 0;
}

} // namespace nlm