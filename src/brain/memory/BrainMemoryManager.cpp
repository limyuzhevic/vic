#include "brain/memory/BrainMemoryManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

BrainMemoryManager::BrainMemoryManager(std::shared_ptr<Config> config)
    : config_(config)
    , developmentalStage_(DevelopmentalStage::Initial)
    , totalSpikesThisStep_(0)
    , totalSpikesTotal_(0)
    , stepsSinceLastEpisode_(0)
    , replayInterval_(100)
    , consolidationInterval_(1000)
    , isResting_(false)
    , totalNeurons_(0)
    , totalSynapses_(0)
{
    if (!config_) {
        NLM_LOG_ERROR("Configuration is null in BrainMemoryManager constructor");
    }
    
    // Initialize random generator with seed from config
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = config_->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    rng_ = std::make_unique<RandomGenerator>(seed);
    
    NLM_LOG_INFO("BrainMemoryManager initialized");
}

BrainMemoryManager::~BrainMemoryManager() = default;

bool BrainMemoryManager::initialize(const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    NLM_LOG_INFO("Initializing brain memory systems...");
    
    // Calculate total neurons and synapses
    totalNeurons_ = 0;
    totalSynapses_ = 0;
    for (const auto& region : regions) {
        totalNeurons_ += region->getTotalNeuronCount();
        totalSynapses_ += region->getSynapseCount();
    }
    
    // Initialize memory systems
    workingMemory_ = std::make_unique<NeuralWorkingMemory>();
    episodicMemory_ = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory_ = std::make_unique<NeuralAssociativeMemory>();
    
    workingMemory_->initialize(nullptr);  // Would pass brain reference if available
    workingMemory_->setCapacity(totalNeurons_ / 10);
    
    episodicMemory_->initialize(nullptr);  // Would pass brain reference if available
    episodicMemory_->setMaxEpisodes(1000);
    
    associativeMemory_->initialize(nullptr);  // Would pass brain reference if available
    
    // Initialize prediction system
    predictionSystem_ = std::make_unique<PredictionSystem>();
    
    // Initialize cognition systems
    planner_ = std::make_unique<NeuralPlanner>();
    conceptFormation_ = std::make_unique<ConceptFormation>();
    attention_ = std::make_unique<AttentionalSelection>();
    
    planner_->initialize(nullptr);  // Would pass brain reference if available
    planner_->setPlanningDepth(5);
    
    attention_->initialize(nullptr);  // Would pass brain reference if available
    attention_->setInhibitionStrength(0.5f);
    attention_->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation systems
    dopamine_ = std::make_unique<Dopamine>();
    curiosity_ = std::make_unique<Curiosity>();
    predictionError_ = std::make_unique<PredictionError>();
    novelty_ = std::make_unique<Novelty>();
    
    dopamine_->initialize(nullptr);  // Would pass brain reference if available
    curiosity_->initialize(nullptr);  // Would pass brain reference if available
    
    // Initialize development system
    developmentSystem_ = std::make_unique<DevelopmentSystem>();
    developmentSystem_->initialize(nullptr);  // Would pass brain reference if available
    
    // Initialize plasticity systems
    spikeSystem_ = std::make_unique<SpikeSystem>();
    stdp_ = std::make_unique<STDP>();
    hebbian_ = std::make_unique<Hebbian>();
    structuralPlasticity_ = std::make_unique<StructuralPlasticity>();
    
    // Configure STDP parameters
    float ltpWeight = config_->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config_->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config_->getOr<float>("stdp_tau", 20.0f);
    stdp_->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = config_->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config_->getOr<float>("pruning_rate", 0.00001f);
    structuralPlasticity_->setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity_->setPruningRate(pruningRate);
    
    // Configure checkpoint manager
    checkpointManager_ = std::make_unique<CheckpointManager>();
    std::string checkpointDir = config_->getOr<std::string>("checkpoint_dir", "./checkpoints");
    checkpointManager_->configure(checkpointDir, 10000, 5, true);
    
    // Get timestep from config
    timestep_ = config_->getOr<double>("simulation_timestep", 0.001);
    
    // Get integration intervals from config
    replayInterval_ = config_->getOr<size_t>("replay_interval", 100);
    consolidationInterval_ = config_->getOr<size_t>("consolidation_interval", 1000);
    
    // Register spike handlers
    spikeSystem_->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++totalSpikesThisStep_;
        ++totalSpikesTotal_;
    });
    
    spikeSystem_->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Spike delivery will be handled by brain implementation
    });
    
    NLM_LOG_INFO("Brain memory systems initialized: " + 
                 std::to_string(totalNeurons_) + " neurons, " +
                 std::to_string(totalSynapses_) + " synapses");
    
    return true;
}

void BrainMemoryManager::reset() {
    NLM_LOG_INFO("Resetting brain memory systems...");
    
    totalSpikesThisStep_ = 0;
    stepsSinceLastEpisode_ = 0;
    isResting_ = false;
    developmentalStage_ = DevelopmentalStage::Initial;
    
    if (workingMemory_) workingMemory_->clear();
    if (episodicMemory_) episodicMemory_->clear();
    if (associativeMemory_) associativeMemory_->clear();
    if (attention_) attention_->reset();
    if (spikeSystem_) spikeSystem_->reset();
    
    NLM_LOG_INFO("Brain memory systems reset");
}

size_t BrainMemoryManager::getTotalNeuronCount() const {
    return totalNeurons_;
}

size_t BrainMemoryManager::getTotalSynapseCount() const {
    return totalSynapses_;
}

size_t BrainMemoryManager::getActiveNeuronCount() const {
    size_t total = 0;
    // This would be implemented by querying all regions
    // For now, return a placeholder
    return total;
}

size_t BrainMemoryManager::getTotalSpikeCount() const {
    return totalSpikesTotal_;
}

void BrainMemoryManager::updateMemorySystems(double timestep) {
    if (workingMemory_) {
        workingMemory_->update(timestep);
    }
    
    if (episodicMemory_) {
        // Episodic memory is updated at intervals, not every step
        // This is handled by the brain implementation
    }
}

void BrainMemoryManager::updatePredictionSystem() {
    if (predictionSystem_) {
        // Prediction system updates would go here
    }
}

void BrainMemoryManager::updateCognitionSystems(double timestep) {
    if (planner_) {
        // Planner updates would go here
    }
    
    if (conceptFormation_) {
        // Concept formation updates would go here
    }
    
    if (attention_) {
        attention_->update(timestep);
    }
}

void BrainMemoryManager::updateNeuromodulationSystems(double timestep) {
    if (dopamine_) {
        dopamine_->update(timestep);
    }
    
    if (curiosity_) {
        curiosity_->update(timestep);
    }
    
    if (novelty_) {
        novelty_->update(timestep);
    }
    
    if (predictionError_) {
        // Prediction error updates would go here
    }
}

void BrainMemoryManager::updateDevelopmentSystem(double timestep) {
    if (developmentSystem_) {
        developmentSystem_->update(nullptr, *rng_, timestep);  // Would pass brain reference
    }
}

void BrainMemoryManager::updatePlasticitySystems(double timestep) {
    if (spikeSystem_) {
        // Spike system updates would go here
    }
    
    if (stdp_) {
        // STDP updates would go here
    }
    
    if (hebbian_) {
        // Hebbian updates would go here
    }
    
    if (structuralPlasticity_) {
        // Structural plasticity updates
        structuralPlasticity_->update(nullptr, *rng_);  // Would pass brain reference
    }
}

void BrainMemoryManager::storeSensoryInput(const std::vector<float>& sensoryData) {
    if (workingMemory_ && !sensoryData.empty()) {
        // Store sensory data in working memory
        // This is a simplified implementation
        for (size_t i = 0; i < sensoryData.size() && i < 100; ++i) {  // Limit to first 100 values
            float normalizedValue = sensoryData[i] * 10.0f;  // Scale factor
            workingMemory_->storeToNeuron(i, normalizedValue / 10.0f);
        }
    }
}

void BrainMemoryManager::applyNeuromodulation(double timestep) {
    // Apply neuromodulation effects
    float dopamineLevel = dopamine_ ? dopamine_->getLevel() : 0.0f;
    
    // Apply dopamine effects (example implementation)
    // This would be integrated with the brain's neural population updates
    
    // Update prediction error based on learning
    if (predictionError_) {
        // Prediction error calculation would go here
    }
}

float BrainMemoryManager::getDopamineLevel() const {
    return dopamine_ ? dopamine_->getLevel() : 0.0f;
}

float BrainMemoryManager::getCuriosityLevel() const {
    return curiosity_ ? curiosity_->getLevel() : 0.0f;
}

float BrainMemoryManager::getNoveltyLevel() const {
    return novelty_ ? novelty_->getLevel() : 0.0f;
}

float BrainMemoryManager::getPredictionError() const {
    return predictionError_ ? predictionError_->getLevel() : 0.0f;
}

} // namespace nlm
