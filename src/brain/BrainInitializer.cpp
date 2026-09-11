#include "BrainInitializer.hpp"
#include "Brain.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

void BrainInitializer::initializeRegions(Brain& brain, size_t neuronCount, size_t regionCount, 
                                       float connectionProbability) {
    brain.clearRegions();
    
    for (size_t i = 0; i < regionCount; ++i) {
        brain.addRegion("Region_" + std::to_string(i + 1));
    }
    
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain.getRegion(i + 1);
        if (region) {
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            brain.collectSensoryMotorNeurons(i + 1, sensoryPopId, motorPopId);
        }
    }
}

void BrainInitializer::collectSensoryMotorNeurons(Brain& brain, size_t regionId, 
                                              RegionId sensoryPopId, RegionId motorPopId) {
    auto* region = brain.getRegion(regionId);
    if (!region) return;
    
    auto* sensoryPop = region->getPopulation(sensoryPopId);
    auto* motorPop = region->getPopulation(motorPopId);
    
    if (sensoryPop) {
        for (auto* neuron : sensoryPop->getNeurons()) {
            brain.addSensoryNeuron(neuron);
        }
    }
    
    if (motorPop) {
        for (auto* neuron : motorPop->getNeurons()) {
            brain.addMotorNeuron(neuron);
        }
    }
}

void BrainInitializer::initializeConnectivity(Brain& brain, float connectionProbability) {
    for (auto* region : brain.getRegions()) {
        region->initializeRandomConnectivity(*brain.getRandomGenerator(), 
                                             connectionProbability, 0.2f, 0.1f);
    }
}

void BrainInitializer::initializeMemorySystems(Brain& brain) {
    if (auto* workingMemory = brain.getWorkingMemory()) {
        workingMemory->initialize(&brain);
        size_t neuronCount = brain.getTotalNeuronCount();
        workingMemory->setCapacity(neuronCount / 10);
    }
    
    if (auto* episodicMemory = brain.getEpisodicMemory()) {
        episodicMemory->initialize(&brain);
        episodicMemory->setMaxEpisodes(1000);
    }
    
    if (auto* associativeMemory = brain.getAssociativeMemory()) {
        associativeMemory->initialize(&brain);
    }
}

void BrainInitializer::initializePredictionSystem(Brain& brain) {
    // PredictionSystem initialization can be added here when needed
}

void BrainInitializer::initializeCognitionSystems(Brain& brain) {
    if (auto* planner = brain.getPlanner()) {
        planner->initialize(&brain);
        planner->setPlanningDepth(5);
    }
    
    if (auto* conceptFormation = brain.getConceptFormation()) {
        conceptFormation->initialize(&brain);
    }
    
    if (auto* attention = brain.getAttention()) {
        attention->initialize(&brain);
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
}

void BrainInitializer::initializeNeuromodulationSystems(Brain& brain) {
    if (auto* novelty = brain.getNovelty()) {
        novelty->initialize(&brain);
    }
    
    if (auto* curiosity = brain.getCuriosity()) {
        curiosity->initialize(&brain);
    }
}

void BrainInitializer::initializePlasticitySystems(Brain& brain) {
    // Plasticity systems are initialized in Brain::Impl
}

void BrainInitializer::configureCheckpointSystem(Brain& brain) {
    std::string checkpointDir = config_.getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (auto* checkpointManager = brain.getCheckpointManager()) {
        checkpointManager->configure(checkpointDir, 10000, 5, true);
    }
}

void BrainInitializer::registerEventDrivenProcessors(Brain& brain) {
    auto* spikeSystem = brain.getSpikeSystem();
    if (!spikeSystem) return;
    
    // Register spike count handler
    spikeSystem->registerHandler([&, this](const DetailedSpikeEvent& event) {
        ++brain.pImpl->totalSpikesThisStep;
        ++brain.pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler for synaptic delivery
    spikeSystem->registerDelayedHandler([&, this](const DelayedSpikeEvent& event) {
        brain.deliverDelayedSpike(event);
    });
}

void BrainInitializer::logInitializationStatus(const Brain& brain) const {
    NLM_LOG_INFO("=== NLM Brain Initialization Complete ===");
    NLM_LOG_INFO("Regions: " + std::to_string(brain.getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain.getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain.getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(brain.getSensoryNeuronCount()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(brain.getMotorNeuronCount()));
}

void BrainInitializer::configureSTDPParameters(Brain& brain) const {
    float ltpWeight = config_.getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config_.getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config_.getOr<float>("stdp_tau", 20.0f);
    
    if (auto* stdp = brain.getSTDP()) {
        stdp->configure(ltpWeight, ltdWeight, tau);
    }
}

void BrainInitializer::configureStructuralPlasticity(Brain& brain) const {
    float synaptogenesisRate = config_.getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config_.getOr<float>("pruning_rate", 0.00001f);
    
    if (auto* structuralPlasticity = brain.getStructuralPlasticity()) {
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
    }
}

void BrainInitializer::setupRegionNeurons(Brain& brain, size_t regionIndex, size_t neuronsPerRegion) {
    auto* region = brain.getRegion(regionIndex);
    if (!region) return;
    
    auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
    auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
    auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
    
    // Collect neurons for I/O
    brain.collectSensoryMotorNeurons(regionIndex, sensoryPopId, motorPopId);
}

float BrainInitializer::getCurrentTimestep(const Config& config) {
    return config.getOr<double>("simulation_timestep", 0.001);
}

void BrainInitializer::getIntegrationIntervals(size_t& replayInterval, size_t& consolidationInterval, 
                                              const Config& config) {
    replayInterval = config.getOr<size_t>("replay_interval", 100);
    consolidationInterval = config.getOr<size_t>("consolidation_interval", 1000);
}

} // namespace nlm
