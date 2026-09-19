#include "BrainCognition.hpp"

namespace nlm {

BrainCognition::BrainCognition(std::shared_ptr<Config> cfg)
    : config(cfg)
    , planner(nullptr)
    , conceptFormation(nullptr)
    , attention(nullptr)
{
    // Initialize cognition systems
    planner = std::make_unique<NeuralPlanner>();
    conceptFormation = std::make_unique<ConceptFormation>();
    attention = std::make_unique<AttentionalSelection>();
    
    // Configure from config
    initializeFromConfig();
}

BrainCognition::~BrainCognition() = default;

void BrainCognition::initialize(Brain* brain) {
    if (!brain) return;
    
    // Initialize cognition systems with brain reference
    if (planner) planner->initialize(brain);
    if (conceptFormation) conceptFormation->initialize(brain);
    if (attention) attention->initialize(brain);
}

void BrainCognition::update(TimestepDuration timestep) {
    if (!planner || !conceptFormation || !attention) return;
    
    // Update cognition systems
    planner->update(timestep);
    conceptFormation->update(timestep);
    attention->update(timestep);
    
    // Update conceptual learning
    updateConceptualLearning();
    
    // Update attentional selection
    updateAttentionalSelection();
}

void BrainCognition::applyAttentionToWorkingMemory(Brain* brain) {
    if (!brain || !attention || !brain->getWorkingMemory()) return;
    
    // Apply attention to working memory winners
    std::vector<NeuronId> competitors = brain->getWorkingMemory()->getMemoryNeurons();
    if (!competitors.empty()) {
        attention->processCompetition(competitors);
    }
}

void BrainCognition::processMemoryCompetition(Brain* brain) {
    if (!brain || !attention || !brain->getWorkingMemory()) return;
    
    std::vector<NeuronId> competitors = brain->getWorkingMemory()->getMemoryNeurons();
    attention->processCompetition(competitors);
}

void BrainCognition::reset() {
    if (planner) planner->reset();
    if (conceptFormation) conceptFormation->reset();
    if (attention) attention->reset();
}

void BrainCognition::logStatus() const {
    // TODO: Add logging implementation
}

void BrainCognition::initializeFromConfig() {
    if (!config) return;
    
    // Get cognition parameters from config
    if (attention) {
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
    if (planner) {
        planner->setPlanningDepth(5);
    }
}

void BrainCognition::updateConceptualLearning() {
    if (!conceptFormation) return;
    
    // TODO: Implement conceptual learning
}

void BrainCognition::updateAttentionalSelection() {
    if (!attention) return;
    
    // TODO: Update attentional selection parameters
}

} // namespace nlm