#include "DevelopmentManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../plasticity/StructuralPlasticity.hpp"

namespace nlm {

struct DevelopmentManager::Impl {
    std::shared_ptr<Config> config;
    Brain* brain;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg), brain(nullptr) {
        developmentSystem = std::make_unique<DevelopmentSystem>();
    }
    
    ~Impl() = default;
};

DevelopmentManager::DevelopmentManager(std::shared_ptr<Config> config) : 
    pImpl(new Impl(config)),
    criticalPeriodTiming(100.0f),
    maturationTiming(500.0f),
    pruningEnabled(true),
    synaptogenesisEnabled(true) {}

DevelopmentManager::~DevelopmentManager() {
    delete pImpl;
}

bool DevelopmentManager::initialize(Brain* brain) {
    NLM_LOG_INFO("Initializing DevelopmentManager...");
    
    pImpl->brain = brain;
    
    NLM_LOG_INFO("DevelopmentManager initialized successfully");
    return true;
}

void DevelopmentManager::update(Timestamp time, TimestepDuration timestep) {
    // Apply developmental effects periodically
    if (pImpl->brain->getCurrentStep() % 1000 == 0) {  // Update every 1000 steps
        pImpl->developmentSystem->update(pImpl->brain, *pImpl->brain->getRandomGenerator(), timestep * 1000);
        
        // Apply development effects on plasticity rates
        if (pImpl->brain->getStructuralPlasticity()) {
            DevelopmentalStage stage = getDevelopmentalStage();
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
            
            pImpl->brain->getStructuralPlasticity()->setSynaptogenesisRate(0.0001f * plasticityMod);
            pImpl->brain->getStructuralPlasticity()->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void DevelopmentManager::reset() {
    NLM_LOG_INFO("Resetting DevelopmentManager...");
    setDevelopmentalStage(DevelopmentalStage::Initial);
    NLM_LOG_INFO("DevelopmentManager reset complete");
}

DevelopmentalStage DevelopmentManager::getDevelopmentalStage() const {
    return pImpl->brain ? pImpl->brain->getDevelopmentalStage() : DevelopmentalStage::Initial;
}

void DevelopmentManager::setDevelopmentalStage(DevelopmentalStage stage) {
    if (pImpl->brain) {
        pImpl->brain->setDevelopmentalStage(stage);
    }
}

} // namespace nlm