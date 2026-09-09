// Brain Integration Header
#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <memory>

namespace nlm {
namespace brain {

class BrainInitializer {
public:
    static void initializeMemorySystems(std::shared_ptr<Config> config, Brain::Impl& impl);
    static void initializePredictionSystem(std::shared_ptr<Config> config, Brain::Impl& impl);
    static void initializeCognitionSystems(std::shared_ptr<Config> config, Brain::Impl& impl);
    static void initializeNeuromodulationSystems(std::shared_ptr<Config> config, Brain::Impl& impl);
    static void initializeDevelopmentSystem(std::shared_ptr<Config> config, Brain::Impl& impl);
    static void initializePlasticitySystems(std::shared_ptr<Config> config, Brain::Impl& impl);
    static void integrateAllSystems(Brain::Impl& impl);
};

void BrainInitializer::initializeMemorySystems(std::shared_ptr<Config> config, Brain::Impl& impl) {
    impl.workingMemory = std::make_unique<NeuralWorkingMemory>();
    impl.episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    impl.associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    impl.workingMemory->initialize(nullptr);
    impl.workingMemory->setCapacity(config->getOr<size_t>("working_memory_capacity", 1000));
    
    impl.episodicMemory->initialize(nullptr);
    impl.episodicMemory->setMaxEpisodes(config->getOr<size_t>("max_episodes", 1000));
}

void BrainInitializer::initializePredictionSystem(std::shared_ptr<Config> config, Brain::Impl& impl) {
    impl.predictionSystem = std::make_unique<PredictionSystem>();
}

void BrainInitializer::initializeCognitionSystems(std::shared_ptr<Config> config, Brain::Impl& impl) {
    impl.planner = std::make_unique<NeuralPlanner>();
    impl.conceptFormation = std::make_unique<ConceptFormation>();
    impl.attention = std::make_unique<AttentionalSelection>();
    
    impl.planner->initialize(nullptr);
    impl.conceptFormation->initialize(nullptr);
    impl.attention->initialize(nullptr);
    
    impl.attention->setInhibitionStrength(0.5f);
    impl.attention->setExcitationStrength(1.5f);
}

void BrainInitializer::initializeNeuromodulationSystems(std::shared_ptr<Config> config, Brain::Impl& impl) {
    impl.dopamine = std::make_unique<Dopamine>();
    impl.curiosity = std::make_unique<Curiosity>();
    impl.predictionError = std::make_unique<PredictionError>();
    impl.novelty = std::make_unique<Novelty>();
}

void BrainInitializer::initializeDevelopmentSystem(std::shared_ptr<Config> config, Brain::Impl& impl) {
    impl.developmentSystem = std::make_unique<DevelopmentSystem>();
}

void BrainInitializer::initializePlasticitySystems(std::shared_ptr<Config> config, Brain::Impl& impl) {
    impl.spikeSystem = std::make_unique<SpikeSystem>();
    impl.stdp = std::make_unique<STDP>();
    impl.hebbian = std::make_unique<Hebbian>();
    impl.structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    impl.stdp->configure(ltpWeight, ltdWeight, tau);
    
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    impl.structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    impl.structuralPlasticity->setPruningRate(pruningRate);
}

void BrainInitializer::integrateAllSystems(Brain::Impl& impl) {
    integrateMemorySystems(impl);
    integratePredictionSystem(impl);
    integrateCognitionSystems(impl);
    integrateNeuromodulationSystems(impl);
    integrateDevelopmentSystem(impl);
    integratePlasticitySystems(impl);
}

void integrateMemorySystems(Brain::Impl& impl) {
    BrainInitializer::initializeMemorySystems(impl.config, impl);
}

void integratePredictionSystem(Brain::Impl& impl) {
    BrainInitializer::initializePredictionSystem(impl.config, impl);
}

void integrateCognitionSystems(Brain::Impl& impl) {
    BrainInitializer::initializeCognitionSystems(impl.config, impl);
}

void integrateNeuromodulationSystems(Brain::Impl& impl) {
    BrainInitializer::initializeNeuromodulationSystems(impl.config, impl);
}

void integrateDevelopmentSystem(Brain::Impl& impl) {
    BrainInitializer::initializeDevelopmentSystem(impl.config, impl);
}

void integratePlasticitySystems(Brain::Impl& impl) {
    BrainInitializer::initializePlasticitySystems(impl.config, impl);
}

} // namespace brain
} // namespace nlm
