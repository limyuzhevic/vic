#include "AbstractBrain.hpp"

namespace nlm {

std::unique_ptr<AbstractBrain> BrainFactory::createBrain(BrainType type, std::shared_ptr<Config> config) {
    switch (type) {
        case BrainType::Standard:
            return createStandardBrain(config);
        case BrainType::Developmental:
            return createDevelopmentalBrain(config);
        case BrainType::Adult:
            return createAdultBrain(config);
        default:
            return nullptr;
    }
}

std::unique_ptr<AbstractBrain> BrainFactory::createStandardBrain(std::shared_ptr<Config> config) {
    // Create a standard NLM brain with default parameters
    auto brain = std::make_unique<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize standard brain");
        return nullptr;
    }
    return brain;
}

std::unique_ptr<AbstractBrain> BrainFactory::createDevelopmentalBrain(std::shared_ptr<Config> config) {
    // Create a developmental brain with high plasticity
    auto brain = std::make_unique<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize developmental brain");
        return nullptr;
    }
    
    // Configure for developmental stage
    brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    NLM_LOG_INFO("Created developmental brain with high plasticity");
    
    return brain;
}

std::unique_ptr<AbstractBrain> BrainFactory::createAdultBrain(std::shared_ptr<Config> config) {
    // Create an adult brain with low plasticity
    auto brain = std::make_unique<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize adult brain");
        return nullptr;
    }
    
    // Configure for adult stage
    brain->setDevelopmentalStage(DevelopmentalStage::Adult);
    NLM_LOG_INFO("Created adult brain with stable plasticity");
    
    return brain;
}

} // namespace nlm