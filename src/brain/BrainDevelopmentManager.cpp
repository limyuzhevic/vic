#include "BrainDevelopmentManager.hpp"
#include "BrainNeuralCore.hpp"
#include "BrainPlasticityManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct BrainDevelopmentManager::Impl {
    std::shared_ptr<Config> config;
    
    // Development system components
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Development parameters
    double age_;
    float developmentRate;
    float criticalPeriodStart;
    float criticalPeriodEnd;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , age_(0.0)
        , developmentRate(0.001f)
        , criticalPeriodStart(0.0f)
        , criticalPeriodEnd(0.0f)
    {
        developmentSystem = std::make_unique<DevelopmentSystem>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
    }
};

BrainDevelopmentManager::BrainDevelopmentManager(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

BrainDevelopmentManager::~BrainDevelopmentManager() = default;

bool BrainDevelopmentManager::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain Development Manager...");
    
    // Configure development parameters
    developmentRate = pImpl->config->getOr<float>("development_rate", 0.001f);
    criticalPeriodStart = pImpl->config->getOr<float>("critical_period_start", 0.0f);
    criticalPeriodEnd = pImpl->config->getOr<float>("critical_period_end", 1000.0f);
    
    NLM_LOG_INFO("NLM Brain Development Manager initialization complete");
    return true;
}

void BrainDevelopmentManager::update(BrainNeuralCore* neuralCore,
                                     std::unique_ptr<RandomGenerator>& rng,
                                     TimestepDuration timestep) {
    // Update development system age
    pImpl->age_ += developmentRate * timestep;
    
    // Advance developmental stage if needed
    float age = static_cast<float>(pImpl->age_);
    
    DevelopmentalStage targetStage = DevelopmentalStage::Initial;
    if (age > criticalPeriodEnd) {
        targetStage = DevelopmentalStage::Adult;
    } else if (age > (criticalPeriodStart + criticalPeriodEnd) / 2.0f) {
        targetStage = DevelopmentalStage::Maturation;
    } else if (age > criticalPeriodStart) {
        targetStage = DevelopmentalStage::CriticalPeriod;
    }
    
    pImpl->developmentSystem->setStage(targetStage);
    
    // Apply development effects
    if (neuralCore) {
        applyEffectsToNeuralCore(neuralCore);
    }
    
    // Update structural plasticity with development effects
    if (neuralCore && rng) {
        // Note: In real implementation, would update structuralPlasticity here
        // For now, just call update on development system
        pImpl->developmentSystem->update(neuralCore, *rng, timestep * 1000);
    }
}

DevelopmentalStage BrainDevelopmentManager::getStage() const {
    return pImpl->developmentSystem->getStage();
}

void BrainDevelopmentManager::setStage(DevelopmentalStage stage) {
    pImpl->developmentSystem->setStage(stage);
}

const char* BrainDevelopmentManager::getStageName() const {
    return pImpl->developmentSystem->getStageName();
}

void BrainDevelopmentManager::advanceStage() {
    pImpl->developmentSystem->advanceStage();
}

void BrainDevelopmentManager::applyEffectsToPlasticity(BrainPlasticityManager* plasticityManager) {
    if (plasticityManager) {
        float plasticityMod = pImpl->developmentSystem->getPlasticityModifier();
        plasticityManager->applyDevelopment(getStage());
    }
}

void BrainDevelopmentManager::applyEffectsToNeuralCore(BrainNeuralCore* neuralCore) {
    if (neuralCore && pImpl->developmentSystem) {
        // Development affects structural plasticity rates
        auto* sp = pImpl->structuralPlasticity.get();
        if (sp) {
            DevelopmentalStage stage = pImpl->developmentSystem->getStage();
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;  // High plasticity
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;  // Stable
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

double BrainDevelopmentManager::getDevelopmentalAge() const {
    return pImpl->age_;
}

float BrainDevelopmentManager::getPlasticityModifier() const {
    return pImpl->developmentSystem->getPlasticityModifier();
}

bool BrainDevelopmentManager::isCriticalPeriod() const {
    return pImpl->developmentSystem->isCriticalPeriod();
}

float BrainDevelopmentManager::getCriticalPeriodProgress() const {
    return pImpl->developmentSystem->getCriticalPeriodProgress();
}

void BrainDevelopmentManager::setDevelopmentRate(float rate) {
    developmentRate = rate;
}

void BrainDevelopmentManager::setCriticalPeriodStart(float step) {
    criticalPeriodStart = step;
}

void BrainDevelopmentManager::setCriticalPeriodEnd(float step) {
    criticalPeriodEnd = step;
}

void BrainDevelopmentManager::reset() {
    NLM_LOG_INFO("Resetting NLM Brain Development Manager...");
    
    pImpl->age_ = 0.0;
    pImpl->developmentSystem->setStage(DevelopmentalStage::Initial);
    
    NLM_LOG_INFO("NLM Brain Development Manager reset complete");
}

std::shared_ptr<const Config> BrainDevelopmentManager::getConfig() const {
    return pImpl->config;
}

void BrainDevelopmentManager::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Development Manager Status ===");
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(getStage())) + " - " + getStageName());
    NLM_LOG_INFO("Developmental age: " + std::to_string(getDevelopmentalAge()));
    NLM_LOG_INFO("Plasticity modifier: " + std::to_string(getPlasticityModifier()));
    NLM_LOG_INFO("Is critical period: " + std::to_string(isCriticalPeriod()));
    NLM_LOG_INFO("Critical period progress: " + std::to_string(getCriticalPeriodProgress()));
}

} // namespace nlm