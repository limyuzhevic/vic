#include "BrainCognitiveManager.hpp"
#include "BrainNeuralCore.hpp"
#include "BrainMemoryManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct BrainCognitiveManager::Impl {
    std::shared_ptr<Config> config;
    
    // Cognitive system components
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg) {}
};

BrainCognitiveManager::BrainCognitiveManager(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

BrainCognitiveManager::~BrainCognitiveManager() = default;

bool BrainCognitiveManager::initialize(BrainNeuralCore* neuralCore) {
    NLM_LOG_INFO("Initializing NLM Brain Cognitive Manager...");
    
    // Initialize cognitive systems
    planner = std::make_unique<NeuralPlanner>();
    conceptFormation = std::make_unique<ConceptFormation>();
    attention = std::make_unique<AttentionalSelection>();
    
    // Initialize with neural core reference
    planner->initialize(neuralCore);
    conceptFormation->initialize(neuralCore);
    attention->initialize(neuralCore);
    
    // Configure from config
    setPlanningDepth(pImpl->config->getOr<size_t>("planning_depth", 5));
    setInhibitionStrength(pImpl->config->getOr<float>("attention_inhibition", 0.5f));
    setExcitationStrength(pImpl->config->getOr<float>("attention_excitation", 1.5f));
    
    NLM_LOG_INFO("NLM Brain Cognitive Manager initialization complete");
    return true;
}

void BrainCognitiveManager::update(TimestepDuration timestep) {
    // Update attention system
    if (attention) {
        attention->update(timestep);
    }
    
    // Note: Concept formation and planning are triggered by specific events
    // rather than continuous update
}

void BrainCognitiveManager::processCompetition(const std::vector<NeuronId>& competitors) {
    if (attention && !competitors.empty()) {
        attention->processCompetition(competitors);
    }
}

void BrainCognitiveManager::processCurrentActivity(const std::vector<NeuronId>& activeNeurons) {
    if (attention) {
        attention->processCurrentActivity(activeNeurons);
    }
    
    if (conceptFormation) {
        conceptFormation->processCurrentActivity(activeNeurons);
    }
}

void BrainCognitiveManager::formConcepts(const std::vector<NeuronId>& activeNeurons) {
    if (conceptFormation) {
        conceptFormation->processCurrentActivity(activeNeurons);
    }
}

NeuralPlanner* BrainCognitiveManager::getPlanner() {
    return planner.get();
}

const NeuralPlanner* BrainCognitiveManager::getPlanner() const {
    return planner.get();
}

ConceptFormation* BrainCognitiveManager::getConceptFormation() {
    return conceptFormation.get();
}

const ConceptFormation* BrainCognitiveManager::getConceptFormation() const {
    return conceptFormation.get();
}

AttentionalSelection* BrainCognitiveManager::getAttention() {
    return attention.get();
}

const AttentionalSelection* BrainCognitiveManager::getAttention() const {
    return attention.get();
}

void BrainCognitiveManager::setPlanningDepth(size_t depth) {
    if (planner) {
        planner->setPlanningDepth(depth);
    }
}

void BrainCognitiveManager::setInhibitionStrength(float strength) {
    if (attention) {
        attention->setInhibitionStrength(strength);
    }
}

void BrainCognitiveManager::setExcitationStrength(float strength) {
    if (attention) {
        attention->setExcitationStrength(strength);
    }
}

void BrainCognitiveManager::reset() {
    NLM_LOG_INFO("Resetting NLM Brain Cognitive Manager...");
    
    if (planner) planner->reset();
    if (conceptFormation) conceptFormation->reset();
    if (attention) attention->reset();
    
    NLM_LOG_INFO("NLM Brain Cognitive Manager reset complete");
}

std::shared_ptr<const Config> BrainCognitiveManager::getConfig() const {
    return pImpl->config;
}

void BrainCognitiveManager::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Cognitive Manager Status ===");
    // Note: Individual cognitive systems may have their own logging
}

} // namespace nlm