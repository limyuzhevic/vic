// CognitionSystem.cpp - Implementation of cognition system
#include "CognitionSystem.h"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct CognitionSystem::Impl {
    Brain* brain_ = nullptr;
    size_t updateCount = 0;
};

CognitionSystem::CognitionSystem() : pImpl(std::make_unique<Impl>()) {}

CognitionSystem::~CognitionSystem() = default;

bool CognitionSystem::initialize(Brain* brain) {
    if (!brain) return false;
    
    pImpl->brain_ = brain;
    pImpl->updateCount = 0;
    
    // Initialize cognition components
    planner_ = std::make_unique<NeuralPlanner>();
    conceptFormation_ = std::make_unique<ConceptFormation>();
    attention_ = std::make_unique<AttentionalSelection>();
    
    if (planner_) {
        planner_->initialize(brain);
        planner_->setPlanningDepth(5);
    }
    
    if (conceptFormation_) {
        conceptFormation_->initialize(brain);
    }
    
    if (attention_) {
        attention_->initialize(brain);
        attention_->setInhibitionStrength(0.5f);
        attention_->setExcitationStrength(1.5f);
    }
    
    initialized_ = true;
    NLM_LOG_INFO("CognitionSystem initialized");
    return true;
}

void CognitionSystem::update(const TimestepDuration& dt) {
    if (!initialized_ || !pImpl->brain_) return;
    
    pImpl->updateCount++;
    
    // Update cognition components
    if (attention_) {
        attention_->update(dt);
    }
    
    if (conceptFormation_) {
        // Concept formation would process current neural activity
    }
    
    NLM_LOG_TRACE("CognitionSystem updated");
}

void CognitionSystem::reset() {
    if (planner_) planner_->clearCache();
    if (conceptFormation_) {
        // Clear concept formation state
    }
    if (attention_) attention_->reset();
    
    initialized_ = false;
    pImpl->updateCount = 0;
    NLM_LOG_INFO("CognitionSystem reset");
}

void CognitionSystem::logStatus() const {
    NLM_LOG_INFO("=== Cognition System Status ===");
    if (planner_) {
        NLM_LOG_INFO("Planner depth: " + std::to_string(planner_->getPlanningDepth()) +
                     ", confidence: " + std::to_string(planner_->getPlanningConfidence()));
    }
}

ActionType CognitionSystem::planAction(const std::vector<float>& currentState, float targetReward) {
    return planner_ ? planner_->planAction(currentState, targetReward) : ActionType::Wait;
}

void CognitionSystem::setPlanningDepth(size_t depth) {
    if (planner_) {
        planner_->setPlanningDepth(depth);
    }
}

void CognitionSystem::updateConcept(const std::vector<float>& pattern) {
    if (conceptFormation_) {
        // Process pattern for concept formation
    }
}

void CognitionSystem::processCompetition(const std::vector<NeuronId>& competitors) {
    if (attention_) {
        attention_->processCompetition(competitors);
    }
}

void CognitionSystem::setInhibitionStrength(float strength) {
    if (attention_) {
        attention_->setInhibitionStrength(strength);
    }
}

void CognitionSystem::setExcitationStrength(float strength) {
    if (attention_) {
        attention_->setExcitationStrength(strength);
    }
}

} // namespace nlm
