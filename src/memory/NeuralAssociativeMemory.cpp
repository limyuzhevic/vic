// NLM NeuralAssociativeMemory Implementation
// Connects associative memory to working memory and episodic memory for integrated pattern learning

#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    class Brain* brain;
    class NeuralWorkingMemory* workingMemory;
    class NeuralEpisodicMemory* episodicMemory;
    
    // Temporal associations (pattern A -> pattern B)
    std::vector<std::tuple<std::vector<float>, std::vector<float>, float>> temporalAssociations;
    
    // Contextual associations (pattern -> context -> strength)
    std::unordered_map<std::vector<float>, std::unordered_map<std::vector<float>, float>> contextualAssociations;
    
    // Hebbian associations between neuron patterns
    std::unordered_map<NeuronId, std::unordered_map<NeuronId, float>> hebbianAssociations;
    
    // Novelty-triggered associations
    std::vector<std::pair<std::vector<float>, std::vector<float>>> noveltyAssociations;
    
    Impl() : brain(nullptr), workingMemory(nullptr), episodicMemory(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , workingMemory_(nullptr)
    , episodicMemory_(nullptr)
{
    actionEffects_.resize(10);  // 10 action types
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::setWorkingMemory(class NeuralWorkingMemory* wm) {
    pImpl->workingMemory = wm;
    workingMemory_ = wm;
    NLM_LOG_INFO("NeuralAssociativeMemory connected to Working Memory");
}

void NeuralAssociativeMemory::setEpisodicMemory(class NeuralEpisodicMemory* em) {
    pImpl->episodicMemory = em;
    episodicMemory_ = em;
    NLM_LOG_INFO("NeuralAssociativeMemory connected to Episodic Memory");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                      const std::vector<float>& patternB,
                                      float strength) {
    // Check if association already exists
    for (auto& assoc : pImpl->temporalAssociations) {
        if (computeSimilarity(patternA, std::get<0>(assoc)) > 0.9f &&
            computeSimilarity(patternB, std::get<1>(assoc)) > 0.9f) {
            // Update existing association
            std::get<2>(assoc) = strength;
            return;
        }
    }
    
    // Create new association
    pImpl->temporalAssociations.emplace_back(patternA, patternB, strength);
    
    // Bidirectional association
    pImpl->temporalAssociations.emplace_back(patternB, patternA, strength);
}

void NeuralAssociativeMemory::createFromWorkingMemory(const std::vector<float>& wmPattern,
                                                     float novelty) {
    if (!workingMemory_) return;
    
    // Create associations based on novelty
    if (novelty > 0.7f) {
        // High novelty creates strong associations with working memory traces
        for (const auto& trace : workingMemory_->getMemoryTraces()) {
            float similarity = computeSimilarity(wmPattern, trace.pattern);
            if (similarity > 0.5f) {
                associate(wmPattern, trace.pattern, similarity * 2.0f);
            }
        }
    }
    
    // Create context associations with working memory capacity
    std::vector<float> context(wmPattern.size(), 0.0f);
    if (!wmPattern.empty()) {
        // Use WM pattern as context
        context = wmPattern;
    }
    
    for (const auto& pattern : workingMemory_->getActivePatterns()) {
        float strength = computeSimilarity(wmPattern, pattern) * novelty;
        if (strength > 0.3f) {
            createContextualAssociation(wmPattern, pattern, strength);
        }
    }
}

void NeuralAssociativeMemory::createFromEpisodicMemory(const class NeuralEpisodicMemory* episode) {
    if (!episodicMemory_ || !episode) return;
    
    auto episodes = episode->getEpisodesForLearning();
    for (const auto* ep : episodes) {
        // Extract pattern from episode
        std::vector<float> epPattern = extractPatternFromEpisode(ep);
        
        // Create associations based on temporal proximity
        if (ep->getTemporalDistance() < 1000.0f) {
            // Create associations with recent working memory content
            if (workingMemory_) {
                auto wmTraces = workingMemory_->getRelevantTraces(epPattern, 0.6f);
                for (const auto& trace : wmTraces) {
                    associate(epPattern, trace.pattern, 1.0f - ep->getTemporalDistance() / 1000.0f);
                }
            }
        }
        
        // Create episodic context associations
        std::vector<float> episodeContext = getEpisodeContext(ep);
        createContextualAssociation(epPattern, episodeContext, 0.8f);
    }
}

void NeuralAssociativeMemory::createContextualAssociation(const std::vector<float>& pattern,
                                                         const std::vector<float>& context,
                                                         float strength) {
    // Store contextual association
    pImpl->contextualAssociations[pattern][context] = strength;
    
    // Create Hebbian connections between neurons
    createHebbianConnections(pattern, context, strength);
}

void NeuralAssociativeMemory::createHebbianConnections(const std::vector<float>& patternA,
                                                     const std::vector<float>& patternB,
                                                     float strength) {
    // Find matching neurons from patterns
    std::vector<NeuronId> neuronsA = findNeuronsFromPattern(patternA);
    std::vector<NeuronId> neuronsB = findNeuronsFromPattern(patternB);
    
    for (const auto& nA : neuronsA) {
        for (const auto& nB : neuronsB) {
            // Create bidirectional association
            float currentStrength = getAssociationStrength(nA, nB);
            float newStrength = currentStrength + strength * 0.1f;
            newStrength = std::clamp(newStrength, 0.0f, 1.0f);
            
            // Update association
            if (pImpl->brain) {
                pImpl->brain->getSynapseSystem()->createConnection(nA, nB, newStrength);
            }
            
            pImpl->hebbianAssociations[nA][nB] = newStrength;
        }
    }
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                     const std::vector<float>& patternB) const {
    // Check temporal associations first
    for (const auto& assoc : pImpl->temporalAssociations) {
        if (computeSimilarity(patternA, std::get<0>(assoc)) > 0.9f &&
            computeSimilarity(patternB, std::get<1>(assoc)) > 0.9f) {
            return std::get<2>(assoc);
        }
    }
    
    // Check contextual associations
    auto itA = pImpl->contextualAssociations.find(patternA);
    if (itA != pImpl->contextualAssociations.end()) {
        auto itB = itA->second.find(patternB);
        if (itB != itA->second.end()) {
            return itB->second;
        }
    }
    
    return 0.0f;
}

std::vector<float> NeuralAssociativeMemory::retrieveAssociatedPattern(const std::vector<float>& pattern,
                                                                        size_t maxResults) const {
    std::vector<std::pair<float, std::vector<float>>> candidates;
    
    // Search through all associations
    for (const auto& assoc : pImpl->temporalAssociations) {
        float similarity = computeSimilarity(pattern, std::get<0>(assoc));
        if (similarity > 0.5f) {
            candidates.emplace_back(similarity, std::get<1>(assoc));
        }
    }
    
    // Sort by similarity
    std::sort(candidates.begin(), candidates.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<float> result;
    for (size_t i = 0; i < std::min(maxResults, candidates.size()); ++i) {
        result.insert(result.end(), candidates[i].second.begin(), candidates[i].second.end());
    }
    
    return result;
}

void NeuralAssociativeMemory::updateFromWorkingMemory(const std::vector<float>& newPattern,
                                                     const std::vector<float>& oldPattern) {
    // Update associations when working memory pattern changes
    float similarity = computeSimilarity(newPattern, oldPattern);
    if (similarity < 0.7f) {
        // Significant change - update temporal associations
        for (auto& assoc : pImpl->temporalAssociations) {
            if (computeSimilarity(oldPattern, std::get<0>(assoc)) > 0.8f) {
                // Update associated pattern with some strength
                std::vector<float> updatedPattern = std::get<1>(assoc);
                updatedPattern = interpolateVectors(oldPattern, newPattern, 0.1f);
                std::get<0>(assoc) = updatedPattern;
            }
        }
    }
}

void NeuralAssociativeMemory::updateFromEpisodicMemory() {
    if (!episodicMemory_) return;
    
    // Gradually strengthen associations based on episodic memory consolidation
    for (auto& assoc : pImpl->temporalAssociations) {
        float& strength = std::get<2>(assoc);
        strength = std::min(1.0f, strength * 1.01f);  // Gradual strengthening
    }
}

std::vector<NeuronId> NeuralAssociativeMemory::findNeuronsFromPattern(const std::vector<float>& pattern) const {
    std::vector<NeuronId> result;
    if (!brain_) return result;
    
    // Search through all neurons for pattern matches
    for (const auto& region : brain_->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isActive() && pattern.size() > 0) {
                    result.push_back(neuron->getId());
                }
            }
        }
    }
    
    return result;
}

std::vector<float> NeuralAssociativeMemory::extractPatternFromEpisode(const class NeuralEpisodicMemory::Episode* episode) const {
    std::vector<float> pattern;
    if (!episode) return pattern;
    
    // Extract pattern from episode neurons and activations
    pattern.reserve(episode->neurons.size());
    for (const auto& neuron : episode->neurons) {
        // Get neuron activation level
        if (auto* n = brain_->getRegion(NeuronId(1))->getAllNeurons()) {
            for (auto* neuronPtr : *n) {
                if (neuronPtr->getId() == neuron) {
                    pattern.push_back(neuronPtr->getActivationLevel());
                    break;
                }
            }
        }
    }
    
    return pattern;
}

std::vector<float> NeuralAssociativeMemory::getEpisodeContext(const class NeuralEpisodicMemory::Episode* episode) const {
    std::vector<float> context;
    if (!episode) return context;
    
    // Create context vector based on episode properties
    context.resize(16, 0.0f);  // 16-dimensional context vector
    
    // Encode temporal context
    context[0] = static_cast<float>(episode->timestamp) / 10000.0f;
    
    // Encode spatial context (simplified)
    if (episode->metadata.find("position") != std::string::npos) {
        // Extract position info
        context[1] = 0.5f;  // Example: set based on position
    }
    
    // Encode reward context
    context[2] = (episode->reward + 1.0f) / 2.0f;
    
    // Encode novelty context
    context[3] = (episode->novelty + 1.0f) / 2.0f;
    
    return context;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                               const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

std::vector<float> NeuralAssociativeMemory::interpolateVectors(const std::vector<float>& a,
                                                               const std::vector<float>& b,
                                                               float weight) const {
    std::vector<float> result;
    if (a.size() != b.size()) return result;
    
    result.resize(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] * (1.0f - weight) + b[i] * weight;
    }
    
    return result;
}

// SelfModel Implementation (continued from NeuralPlanner.cpp)
struct SelfModel::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

SelfModel::SelfModel()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capabilityLevel_(0.5f)
{
    actionEffects_.resize(10);  // 10 action types
}

SelfModel::~SelfModel() = default;

void SelfModel::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SelfModel initialized");
}

void SelfModel::recordSelfAction(ActionType action,
                               const std::vector<float>& beforeState,
                               const std::vector<float>& afterState) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return;
    
    ActionEffect effect;
    effect.beforeState = beforeState;
    effect.afterState = afterState;
    effect.observationCount = 1;
    
    // Compute confidence as consistency with previous observations
    float consistency = 0.5f;
    for (const auto& prev : actionEffects_[actionIdx]) {
        if (computeSimilarity(beforeState, prev.beforeState) > 0.8f) {
            consistency = std::max(consistency, 
                computeSimilarity(afterState, prev.afterState));
            effect.observationCount += prev.observationCount;
        }
    }
    effect.confidence = consistency;
    
    actionEffects_[actionIdx].push_back(effect);
    
    // Update capability level
    float totalConfidence = 0.0f;
    size_t count = 0;
    for (const auto& effects : actionEffects_) {
        for (const auto& e : effects) {
            totalConfidence += e.confidence;
            ++count;
        }
    }
    if (count > 0) {
        capabilityLevel_ = totalConfidence / count;
    }
}

std::vector<float> SelfModel::predictActionConsequence(ActionType action,
                                                       const std::vector<float>& currentState) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return currentState;
    
    return findMatchingEffect(action, currentState);
}

float SelfModel::getSelfModelConfidence(ActionType action) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return 0.0f;
    
    const auto& effects = actionEffects_[actionIdx];
    if (effects.empty()) return 0.0f;
    
    float totalConf = 0.0f;
    for (const auto& e : effects) {
        totalConf += e.confidence;
    }
    return totalConf / effects.size();
}

float SelfModel::computeSelfGeneratedLikeness(const std::vector<float>& beforeState,
                                             const std::vector<float>& afterState,
                                             ActionType action) const {
    // If we have a good prediction for this action, it's likely self-generated
    auto predicted = findMatchingEffect(action, beforeState);
    
    if (predicted.empty()) return 0.0f;
    
    float similarity = computeSimilarity(predicted, afterState);
    return similarity;
}

ActionType SelfModel::getPreferredAction(const std::vector<float>& state) {
    ActionType best = ActionType::Wait;
    float bestValue = -1000.0f;
    
    for (size_t i = 0; i < actionEffects_.size(); ++i) {
        if (actionEffects_[i].empty()) continue;
        
        // Check how well this action would work in current state
        auto predicted = findMatchingEffect(static_cast<ActionType>(i), state);
        if (!predicted.empty()) {
            // Value = how much the state changes toward reward
            float value = computeSimilarity(predicted, state);
            if (value > bestValue) {
                bestValue = value;
                best = static_cast<ActionType>(i);
            }
        }
    }
    
    return best;
}

void SelfModel::updateSelfModel(const std::vector<float>& predicted,
                               const std::vector<float>& actual,
                               ActionType action) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return;
    
    // Find and update matching effect
    for (auto& effect : actionEffects_[actionIdx]) {
        if (computeSimilarity(effect.beforeState, predicted) > 0.5f) {
            // Update with prediction error
            float error = 1.0f - computeSimilarity(predicted, actual);
            
            // Adjust confidence based on error
            effect.confidence *= (1.0f - error * 0.1f);
            effect.confidence = std::max(0.1f, effect.confidence);
        }
    }
}

float SelfModel::getBodyAwareness() const {
    return capabilityLevel_;
}

void SelfModel::clear() {
    for (auto& effects : actionEffects_) {
        effects.clear();
    }
    capabilityLevel_ = 0.5f;
}

std::vector<float> SelfModel::findMatchingEffect(ActionType action,
                                                const std::vector<float>& beforeState) const {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= actionEffects_.size()) return {};
    
    const auto& effects = actionEffects_[actionIdx];
    if (effects.empty()) return {};
    
    float bestSim = 0.0f;
    std::vector<float> bestPrediction;
    
    for (const auto& effect : effects) {
        float sim = computeSimilarity(beforeState, effect.beforeState);
        if (sim > bestSim) {
            bestSim = sim;
            bestPrediction = effect.afterState;
        }
    }
    
    return bestPrediction;
}

float SelfModel::computeSimilarity(const std::vector<float>& a,
                               const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

// SocialLearning Implementation (continued from NeuralPlanner.cpp)
struct SocialLearning::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

SocialLearning::SocialLearning()
    : pImpl(new Impl)
    , brain_(nullptr)
    , observationCount_(0)
    , signalMeaning_(0.0f)
{
}

SocialLearning::~SocialLearning() = default;

void SocialLearning::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SocialLearning initialized");
}

void SocialLearning::observeAgentAction(ActionType observedAction,
                                       const std::vector<float>& observerState,
                                       const std::vector<float>& resultingState) {
    ObservedEffect effect;
    effect.state = observerState;
    effect.resultingState = resultingState;
    effect.reward = 0.0f;  // Observer doesn't know reward
    
    observedActions_.push_back({observedAction, effect});
    ++observationCount_;
    
    // Keep memory bounded
    if (observedActions_.size() > 500) {
        observedActions_.erase(observedActions_.begin());
    }
}

bool SocialLearning::canImitate(ActionType observedAction) const {
    // Can imitate if we've seen this action before and know its effect
    for (const auto& pair : observedActions_) {
        if (pair.first == observedAction) {
            return true;
        }
    }
    return false;
}

ActionType SocialLearning::getImitationAction(const std::vector<float>& currentState) {
    ActionType best = ActionType::Wait;
    float bestSim = 0.0f;
    
    for (const auto& pair : observedActions_) {
        float sim = computeSimilarity(currentState, pair.second.state);
        if (sim > bestSim) {
            bestSim = sim;
            best = pair.first;
        }
    }
    
    return best;
}

void SocialLearning::learnCommunicationSignal(const std::vector<float>& signalPattern,
                                           float signalReward) {
    signalPattern_ = signalPattern;
    signalMeaning_ = signalReward;
}

bool SocialLearning::detectSignal(const std::vector<float>& neuralPattern) const {
    if (signalPattern_.empty()) return false;
    
    float similarity = computeSimilarity(neuralPattern, signalPattern_);
    return similarity > 0.7f;
}

std::vector<float> SocialLearning::getSignalPattern() const {
    return signalPattern_;
}

float SocialLearning::getSignalMeaning() const {
    return signalMeaning_;
}

void SocialLearning::updateSocialKnowledge(float interactionReward) {
    // Update value of observed actions based on whether interaction was beneficial
    for (auto& pair : observedActions_) {
        // Adjust reward estimate based on interaction outcome
        pair.second.reward = pair.second.reward * 0.9f + interactionReward * 0.1f;
    }
}

void SocialLearning::clear() {
    observedActions_.clear();
    signalPattern_.clear();
    signalMeaning_ = 0.0f;
    observationCount_ = 0;
}

float SocialLearning::computeSimilarity(const std::vector<float>& a,
                                    const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

} // namespace nlm
