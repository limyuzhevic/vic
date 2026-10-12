#include "CognitiveEngine.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include <algorithm>

namespace nlm {

struct CognitiveEngine::Impl {
    std::shared_ptr<Config> config;
    Brain* brain = nullptr;
    
    // Cognitive system components
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg) {
    }
};

CognitiveEngine::CognitiveEngine(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)), brain_(nullptr) {
}

CognitiveEngine::~CognitiveEngine() = default;

CognitiveEngine::CognitiveEngine(CognitiveEngine&& other) noexcept : pImpl(std::move(other.pImpl)), brain_(other.brain_) {
    other.brain_ = nullptr;
}

CognitiveEngine& CognitiveEngine::operator=(CognitiveEngine&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        brain_ = other.brain_;
        other.brain_ = nullptr;
    }
    return *this;
}

bool CognitiveEngine::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("CognitiveEngine: Cannot initialize with null brain");
        return false;
    }
    
    brain_ = brain;
    pImpl->brain = brain;
    
    // Initialize cognitive components
    pImpl->planner = std::make_unique<NeuralPlanner>();
    pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    pImpl->attention = std::make_unique<AttentionalSelection>();
    
    // Initialize cognitive components
    pImpl->planner->initialize(brain);
    pImpl->conceptFormation->initialize(brain);
    pImpl->attention->initialize(brain);
    
    // Configure cognitive parameters
    if (auto planningDepth = config_->getOr<size_t>("planning_depth", 5)) {
        pImpl->planner->setPlanningDepth(*planningDepth);
    }
    
    if (auto inhibition = config_->getOr<float>("attention_inhibition", 0.5f)) {
        pImpl->attention->setInhibitionStrength(*inhibition);
    }
    if (auto excitation = config_->getOr<float>("attention_excitation", 1.5f)) {
        pImpl->attention->setExcitationStrength(*excitation);
    }
    
    NLM_LOG_INFO("CognitiveEngine initialized successfully");
    return true;
}

void CognitiveEngine::update(TimestepDuration dt) {
    // Update attention system
    if (pImpl->attention) {
        pImpl->attention->update(dt);
    }
    
    // Update concept formation
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->update(dt);
    }
}

void CognitiveEngine::planAction() {
    if (pImpl->planner) {
        pImpl->planner->planAction();
    }
}

void CognitiveEngine::formConcepts() {
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->update(0.001f);  // Use default timestep
    }
}

void CognitiveEngine::updateAttention(const std::vector<NeuronId>& competitors) {
    if (pImpl->attention && !competitors.empty()) {
        pImpl->attention->processCompetition(competitors);
    }
}

size_t CognitiveEngine::getPlanningDepth() const {
    if (pImpl->planner) {
        return pImpl->planner->getPlanningDepth();
    }
    return 0;
}

float CognitiveEngine::getAttentionFocus() const {
    if (pImpl->attention) {
        const auto& winners = pImpl->attention->getWinners();
        return winners.empty() ? 0.0f : 1.0f;
    }
    return 0.0f;
}

void CognitiveEngine::setPlanningDepth(size_t depth) {
    if (pImpl->planner) {
        pImpl->planner->setPlanningDepth(depth);
    }
}

void CognitiveEngine::setAttentionParameters(float inhibition, float excitation) {
    if (pImpl->attention) {
        pImpl->attention->setInhibitionStrength(inhibition);
        pImpl->attention->setExcitationStrength(excitation);
    }
}

void CognitiveEngine::reset() {
    NLM_LOG_INFO("CognitiveEngine reset");
    if (pImpl->planner) pImpl->planner->reset();
    if (pImpl->conceptFormation) pImpl->conceptFormation->reset();
    if (pImpl->attention) pImpl->attention->reset();
}

} // namespace nlm