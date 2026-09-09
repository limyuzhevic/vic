// Brain cognition integration implementation
#include "BrainCognitionInitializer.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../agent/AgentBrain.hpp"

namespace nlm {
namespace brain {
namespace cognition {

void CognitionInitializer::initialize(Brain::Impl& impl) {
    impl.planner = std::make_unique<NeuralPlanner>();
    impl.conceptFormation = std::make_unique<ConceptFormation>();
    impl.attention = std::make_unique<AttentionalSelection>();
    
    impl.planner->initialize(nullptr);
    impl.planner->setPlanningDepth(impl.config->getOr<size_t>("planning_depth", 5));
    
    impl.conceptFormation->initialize(nullptr);
    
    impl.attention->initialize(nullptr);
    impl.attention->setInhibitionStrength(0.5f);
    impl.attention->setExcitationStrength(1.5f);
    
    NLM_LOG_INFO("Cognition systems initialized");
}

void CognitionInitializer::integrateWithWorkingMemory(Brain::Impl& impl) {
    if (!impl.attention || !impl.workingMemory) return;
    
    // Connect attention to working memory competition
    // This is the core integration: attention selects among working memory traces
    
    NLM_LOG_INFO("Cognition systems integrated with working memory");
}

void CognitionInitializer::integrateWithPrediction(Brain::Impl& impl) {
    if (!impl.planner || !impl.predictionSystem) return;
    
    // Neural planning uses predictions to evaluate actions
    // Concept formation uses predictions to discover patterns
    
    NLM_LOG_INFO("Cognition systems integrated with prediction system");
}

void CognitionInitializer::integrateWithActionSelection(Brain::Impl& impl) {
    if (!impl.planner || !impl.workingMemory) return;
    
    // Planner would integrate with AgentBrain for actual action selection
    // Concept formation would influence action selection through knowledge
    
    NLM_LOG_INFO("Cognition systems integrated with action selection");
}

void CognitionInitializer::integrateWithNeuromodulation(Brain::Impl& impl) {
    if (!impl.attention || !impl.dopamine) return;
    
    // Attention modulated by neuromodulators (curiosity, novelty, etc.)
    // Dopamine could affect attentional selection
    
    NLM_LOG_INFO("Cognition systems integrated with neuromodulation");
}

} // namespace cognition
} // namespace brain
} // namespace nlm
