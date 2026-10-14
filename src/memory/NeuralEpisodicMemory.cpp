#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Pattern neurons for episodic recall
    std::vector<NeuronId> episodeNeurons;
    
    Impl() : brain(nullptr) {}
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(1000)
    , replayEnabled_(true)
{
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralEpisodicMemory initialized");
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Create episode copy with age 0
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    
    episodes_.push_back(stored);
    
    // Create episode neuron for pattern completion
    if (brain_ && !episode.sensoryState.empty()) {
        NeuronId epNeuron(episodes_.size() + 20000);
        pImpl->episodeNeurons.push_back(epNeuron);
        
        // Store episode index in the neuron
        // (This is a simplified approach - real implementation would use more distributed encoding)
    }
    
    // Remove old episodes if over capacity
    while (episodes_.size() > maxEpisodes_) {
        episodes_.erase(episodes_.begin());
        if (!pImpl->episodeNeurons.empty()) {
            pImpl->episodeNeurons.erase(pImpl->episodeNeurons.begin());
        }
    }
    
    // Age all episodes
    for (auto& ep : episodes_) {
        ep.age++;
    }
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    size_t maxResults) const {
    
    std::vector<std::pair<float, size_t>> scores;  // (similarity, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float sim = computeSimilarity(sensoryPattern, episodes_[i]);
        scores.emplace_back(sim, i);
    }
    
    // Sort by similarity (descending)
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        if (scores[i].first > 0.3f) {  // Threshold
            results.push_back(&episodes_[scores[i].second]);
        }
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveTemporal(
    SimulationStep startTime,
    SimulationStep endTime,
    size_t maxResults) const {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        if (ep.timestamp >= startTime && ep.timestamp <= endTime) {
            results.push_back(&ep);
        }
    }
    
    // Return most recent first
    std::sort(results.begin(), results.end(),
        [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            return a->timestamp > b->timestamp;
        });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveByLocation(
    float x, float y, float radius, size_t maxResults) const {
    
    std::vector<std::pair<float, size_t>> distances;  // (distance, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float dx = episodes_[i].positionX - x;
        float dy = episodes_[i].positionY - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist <= radius) {
            distances.emplace_back(dist, i);
        }
    }
    
    // Sort by distance
    std::sort(distances.begin(), distances.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });
    
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, distances.size()); ++i) {
        results.push_back(&episodes_[distances[i].second]);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveAfterAction(
    ActionType action, size_t maxResults) const {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        if (ep.action == action) {
            results.push_back(&ep);
        }
    }
    
    // Most recent first
    std::sort(results.begin(), results.end(),
        [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            return a->timestamp > b->timestamp;
        });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

void NeuralEpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!episode || !brain_) return;
    
    // Reactivate neurons that were active during this episode
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = i < episode->neuronActivations.size() ? 
                          episode->neuronActivations[i] : 0.5f;
        
        // Inject current to reawaken this pattern
        brain_->injectCurrent(neuron, activation * 3.0f);
    }
    
    // Also inject sensory pattern if available
    if (!episode->sensoryState.empty()) {
        // This would go through sensory channels - simplified here
    }
}

void NeuralEpisodicMemory::updateRelevance(SimulationStep episodeId, float relevanceDelta) {
    if (episodeId < episodes_.size()) {
        // In a full implementation, episodes would have a relevance field
        // For now, this would age the episode differently
    }
}

const EpisodicMemoryItem* NeuralEpisodicMemory::getEpisode(size_t index) const {
    if (index < episodes_.size()) {
        return &episodes_[index];
    }
    return nullptr;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    size_t start = count > episodes_.size() ? 0 : episodes_.size() - count;
    for (size_t i = start; i < episodes_.size(); ++i) {
        results.push_back(&episodes_[i]);
    }
    
    return results;
}

float NeuralEpisodicMemory::getAverageReward() const {
    if (episodes_.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& ep : episodes_) {
        sum += ep.reward;
    }
    return sum / episodes_.size();
}

void NeuralEpisodicMemory::consolidate(float relevanceThreshold) {
    // Remove episodes below relevance threshold
    // In a full implementation, episodes would have relevance scores
    // For now, just enforce max capacity
    while (episodes_.size() > maxEpisodes_ * 0.8f) {
        episodes_.erase(episodes_.begin());
    }
}

void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    pImpl->episodeNeurons.clear();
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(
    size_t count) const {
    
    // Select episodes for replay based on:
    // 1. Recency (more recent episodes)
    // 2. Reward (higher reward episodes)
    // 3. Unpredictability (higher prediction error episodes)
    
    std::vector<std::tuple<float, float, size_t>> scored;  // (score, reward, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float recencyScore = 1.0f / (episodes_[i].age + 1);
        float rewardScore = episodes_[i].reward;
        float score = recencyScore * 0.5f + rewardScore * 0.5f;
        scored.emplace_back(score, rewardScore, i);
    }
    
    // Sort by score
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) {
            return std::get<0>(a) > std::get<0>(b);
        });
    
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(count, scored.size()); ++i) {
        results.push_back(&episodes_[std::get<2>(scored[i])]);
    }
    
    return results;
}

void NeuralEpisodicMemory::replaySequence(const std::vector<size_t>& episodeIds) {
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            replayEpisode(&episodes_[id]);
            
            // Small delay between replays
            // (In simulation, this would be steps)
        }
    }
}

// NeuralAssociativeMemory Implementation
struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID) {
        // Check if association exists
        for (auto& assoc : associations_) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                // Update strength
                std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + strength * 0.1f, 0.0f, 1.0f);
                return;
            }
        }
        
        // Create new association
        associations_.emplace_back(neuronA, neuronB, strength);
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty()) {
        associate(episode.sensoryState, episode.resultingSensoryState, 0.8f);
    }
    
    // Associate action with resulting state
    if (!episode.sensoryState.empty()) {
        // Create action-specific pattern
        std::vector<float> actionPattern(episode.sensoryState.size(), 0.0f);
        actionPattern[0] = static_cast<float>(episode.action) / 10.0f;
        
        associate(actionPattern, episode.resultingSensoryState, 0.7f);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    
    NeuronId queryNeuron;
    float bestSim = 0.0f;
    
    // Find best matching pattern neuron
    for (size_t i = 0; i < patternNeurons_.size(); ++i) {
        float sim = computeSimilarity(queryPattern, patternNeurons_[i].second);
        if (sim > bestSim) {
            bestSim = sim;
            queryNeuron = patternNeurons_[i].first;
        }
    }
    
    if (bestSim < 0.3f) {
        return {};
    }
    
    // Find associated patterns
    std::vector<std::vector<float>> results;
    std::vector<std::pair<float, std::vector<float>>> scored;
    
    for (const auto& assoc : associations_) {
        NeuronId otherNeuron;
        
        if (std::get<0>(assoc) == queryNeuron) {
            otherNeuron = std::get<1>(assoc);
        } else if (std::get<1>(assoc) == queryNeuron) {
            otherNeuron = std::get<0>(assoc);
        }
        
        if (otherNeuron != INVALID_NEURON_ID) {
            for (const auto& pn : patternNeurons_) {
                if (pn.first == otherNeuron) {
                    float strength = std::get<2>(assoc);
                    scored.emplace_back(strength, pn.second);
                    break;
                }
            }
        }
    }
    
    // Sort by strength
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    for (size_t i = 0; i < std::min(maxResults, scored.size()); ++i) {
        results.push_back(scored[i].second);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    
    NeuronId neuronA, neuronB;
    float bestSimA = 0.0f, bestSimB = 0.0f;
    
    for (const auto& pn : patternNeurons_) {
        float simA = computeSimilarity(patternA, pn.second);
        float simB = computeSimilarity(patternB, pn.second);
        
        if (simA > bestSimA) {
            bestSimA = simA;
            neuronA = pn.first;
        }
        if (simB > bestSimB) {
            bestSimB = simB;
            neuronB = pn.first;
        }
    }
    
    if (bestSimA < 0.3f || bestSimB < 0.3f) {
        return 0.0f;
    }
    
    for (const auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                                const std::vector<float>& patternB,
                                                float delta) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    for (auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {
    
    std::vector<std::vector<float>> activated;
    std::set<NeuronId> visited;
    
    // Find initial pattern neuron
    NeuronId currentNeuron;
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(cuePattern, pn.second) > 0.5f) {
            currentNeuron = pn.first;
            activated.push_back(pn.second);
            visited.insert(currentNeuron);
            break;
        }
    }
    
    if (currentNeuron == INVALID_NEURON_ID) {
        return activated;
    }
    
    // Spread through association network
    for (size_t step = 0; step < steps; ++step) {
        std::vector<NeuronId> nextNeurons;
        
        for (const auto& assoc : associations_) {
            NeuronId otherNeuron = INVALID_NEURON_ID;
            
            if (std::get<0>(assoc) == currentNeuron && visited.find(std::get<1>(assoc)) == visited.end()) {
                otherNeuron = std::get<1>(assoc);
            } else if (std::get<1>(assoc) == currentNeuron && visited.find(std::get<0>(assoc)) == visited.end()) {
                otherNeuron = std::get<0>(assoc);
            }
            
            if (otherNeuron != INVALID_NEURON_ID && std::get<2>(assoc) > 0.3f) {
                nextNeurons.push_back(otherNeuron);
                visited.insert(otherNeuron);
                
                // Find pattern for this neuron
                for (const auto& pn : patternNeurons_) {
                    if (pn.first == otherNeuron) {
                        activated.push_back(pn.second);
                        break;
                    }
                }
            }
        }
        
        if (nextNeurons.empty()) break;
        
        // Pick highest associated next neuron
        currentNeuron = nextNeurons[0];
    }
    
    return activated;
}

void NeuralAssociativeMemory::clear() {
    patternNeurons_.clear();
    associations_.clear();
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Check if we already have a similar pattern
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(pattern, pn.second) > 0.8f) {
            return pn.first;
        }
    }
    
    // Create new pattern neuron
    NeuronId newId(patternNeurons_.size() + 30000);
    patternNeurons_.push_back({newId, pattern});
    return newId;
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

}

// ================= ENHANCED NEURAL EPISODIC MEMORY IMPLEMENTATION =================

// NeuralEpisodicMemory Enhanced Implementation

void NeuralEpisodicMemory::storeEpisodeWithContext(const EpisodicMemoryItem& episode,
                                                   const std::vector<float>& systemState,
                                                   float systemEnergy,
                                                   float globalExcitability,
                                                   const std::vector<float>& bodyVelocity,
                                                   const std::vector<float>& workingMemoryPattern,
                                                   const std::vector<float>& predictionTarget,
                                                   const std::vector<float>& attentionFocus,
                                                   float predictionError,
                                                   float curiosityGain,
                                                   float salience,
                                                   float emotionalValence) {
    // Create enhanced episode with comprehensive system state
    EpisodicMemoryItem enhancedEpisode = episode;
    
    // Store system state and metadata for enhanced temporal dynamics
    enhancedEpisode.systemState = systemState;
    enhancedEpisode.systemEnergy = systemEnergy;
    enhancedEpisode.globalExcitability = globalExcitability;
    enhancedEpisode.bodyVelocity = bodyVelocity;
    enhancedEpisode.workingMemoryPattern = workingMemoryPattern;
    enhancedEpisode.predictionTarget = predictionTarget;
    enhancedEpisode.attentionFocus = attentionFocus;
    
    // Store prediction error and curiosity gain for cross-system integration
    enhancedEpisode.predictionError = predictionError;
    enhancedEpisode.curiosityGain = curiosityGain;
    
    // Store salience and emotional valence for enhanced relevance
    enhancedEpisode.salience = salience;
    enhancedEpisode.emotionalValence = emotionalValence;
    
    // Update episode with enhanced metadata
    enhancedEpisode.lastReplayStep = enhancedEpisode.timestamp;
    enhancedEpisode.replayCount++;
    
    // Store the enhanced episode
    storeEpisode(enhancedEpisode);
}

void NeuralEpisodicMemory::storeActionContext(const EpisodicMemoryItem& episode,
                                              const std::vector<float>& actionContext,
                                              const std::vector<float>& actionParameters,
                                              const std::vector<float>& actionOutcomes,
                                              const std::vector<float>& resultingSystemState,
                                              float resultingEnergyChange) {
    // Create enhanced episode with action-context binding
    EpisodicMemoryItem actionEpisode = episode;
    
    // Store action-specific context and parameters
    actionEpisode.actionContext = actionContext;
    actionEpisode.actionParameters = actionParameters;
    actionEpisode.actionOutcomes = actionOutcomes;
    actionEpisode.resultingSystemState = resultingSystemState;
    actionEpisode.resultingEnergyChange = resultingEnergyChange;
    
    // Store the action-context bound episode
    storeEpisode(actionEpisode);
}

void NeuralEpisodicMemory::updatePredictionError(SimulationStep episodeId, float predictionError) {
    if (episodeId < episodes_.size()) {
        // Update prediction error for the episode
        episodes_[episodeId].predictionError = predictionError;
        
        // Update memory score based on prediction error
        updateMemoryScores();
    }
}

void NeuralEpisodicMemory::integrateWithWorkingMemory(const std::vector<float>& workingMemoryPattern) {
    // Integrate working memory pattern with episodic memory
    // This creates associations between current working memory and stored episodes
    
    // Find similar episodes based on working memory pattern
    std::vector<const EpisodicMemoryItem*> similar = retrieveSimilar(workingMemoryPattern, 5);
    
    for (const auto* ep : similar) {
        // Strengthen association between working memory and this episode
        // This will help with pattern completion
        if (brain_ && !ep->activeNeurons.empty()) {
            for (size_t i = 0; i < std::min(ep->activeNeurons.size(), ep->neuronActivations.size()); ++i) {
                float activation = ep->neuronActivations[i];
                brain_->injectCurrent(ep->activeNeurons[i], activation * 2.0f);
            }
        }
    }
}

void NeuralEpisodicMemory::storeDuringSleep(SimulationStep step) {
    // Store episodes with sleep-specific metadata
    for (auto& ep : episodes_) {
        if (ep.timestamp == step) {
            // Mark that this episode was stored during sleep
            ep.sleepOnset = step;
            ep.wasConsolidatedDuringSleep = true;
            break;
        }
    }
}

void NeuralEpisodicMemory::consolidateDuringSleep() {
    // Enhanced consolidation during sleep periods
    for (auto& ep : episodes_) {
        if (ep.wasConsolidatedDuringSleep) {
            // Apply sleep consolidation effects
            // Increase memory score based on consolidation progress
            ep.consolidationProgress = std::min(1.0f, ep.consolidationProgress + 0.1f);
            
            // Update sleep consolidation gain
            ep.sleepConsolidationGain = ep.consolidationProgress * 0.5f;
            
            // Enhance memory stability
            ep.decayFactor = ep.decayFactor * 0.95f;
        }
    }
}

void NeuralEpisodicMemory::markConsolidatedDuringSleep(SimulationStep episodeId, float gain) {
    if (episodeId < episodes_.size()) {
        episodes_[episodeId].wasConsolidatedDuringSleep = true;
        episodes_[episodeId].sleepConsolidationGain = gain;
        episodes_[episodeId].sleepConsolidationStep = brain_ ? brain_->getCurrentStep() : 0;
        
        // Update memory scores after sleep consolidation
        updateMemoryScores();
    }
}

void NeuralEpisodicMemory::replayEpisodeWithMeta(const EpisodicMemoryItem* episode,
                                                 const std::vector<float>& replayContext) {
    if (!episode || !brain_) return;
    
    // Enhanced replay with context
    // Reactivate neurons that were active during this episode
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = i < episode->neuronActivations.size() ? 
                           episode->neuronActivations[i] : 0.5f;
        
        // Enhanced injection based on emotional valence and salience
        float enhancedActivation = activation;
        if (episode->emotionalValence > 0.5f) {
            enhancedActivation *= 1.5f;
        } else if (episode->emotionalValence < -0.5f) {
            enhancedActivation *= 0.5f;
        }
        
        // Inject current to reawaken this pattern with enhanced context
        brain_->injectCurrent(neuron, enhancedActivation * 3.0f * episode->salience);
    }
    
    // Inject sensory pattern with context
    if (!episode->sensoryState.empty() && !replayContext.empty()) {
        // Apply replay context to sensory pattern
        for (size_t i = 0; i < std::min(episode->sensoryState.size(), replayContext.size()); ++i) {
            float modifiedSensory = episode->sensoryState[i] * (1.0f + replayContext[i] * 0.2f);
            // This would go through sensory channels - simplified here
        }
    }
}

void NeuralEpisodicMemory::storeSystemState(const std::vector<float>& systemState,
                                            float systemEnergy,
                                            float globalExcitability,
                                            const std::vector<float>& bodyVelocity) {
    // Store system state for enhanced temporal dynamics
    // This can be called periodically to capture system-wide state
    
    // Create a system state episode
    EpisodicMemoryItem systemEpisode;
    systemEpisode.timestamp = brain_ ? brain_->getCurrentStep() : 0;
    systemEpisode.sensoryState = systemState; // Reuse sensoryState field for system state
    systemEpisode.systemState = systemState;
    systemEpisode.systemEnergy = systemEnergy;
    systemEpisode.globalExcitability = globalExcitability;
    systemEpisode.bodyVelocity = bodyVelocity;
    systemEpisode.action = ActionType::Wait; // System state, not an action
    
    // Store the system state episode
    storeEpisode(systemEpisode);
}

float NeuralEpisodicMemory::getPredictionError(SimulationStep episodeId) const {
    if (episodeId < episodes_.size()) {
        return episodes_[episodeId].predictionError;
    }
    return 0.0f;
}

float NeuralEpisodicMemory::getCuriosityGain(SimulationStep episodeId) const {
    if (episodeId < episodes_.size()) {
        return episodes_[episodeId].curiosityGain;
    }
    return 0.0f;
}

float NeuralEpisodicMemory::getSalience(SimulationStep episodeId) const {
    if (episodeId < episodes_.size()) {
        return episodes_[episodeId].salience;
    }
    return 0.0f;
}

float NeuralEpisodicMemory::getEmotionalValence(SimulationStep episodeId) const {
    if (episodeId < episodes_.size()) {
        return episodes_[episodeId].emotionalValence;
    }
    return 0.0f;
}

bool NeuralEpisodicMemory::wasConsolidatedDuringSleep(SimulationStep episodeId) const {
    if (episodeId < episodes_.size()) {
        return episodes_[episodeId].wasConsolidatedDuringSleep;
    }
    return false;
}

float NeuralEpisodicMemory::getSleepConsolidationGain(SimulationStep episodeId) const {
    if (episodeId < episodes_.size()) {
        return episodes_[episodeId].sleepConsolidationGain;
    }
    return 0.0f;
}

float NeuralEpisodicMemory::getMemoryScore(SimulationStep episodeId) const {
    if (episodeId < episodes_.size()) {
        return episodes_[episodeId].memoryScore;
    }
    return 0.0f;
}

void NeuralEpisodicMemory::updateMemoryScores() {
    // Update memory scores based on all factors
    for (auto& ep : episodes_) {
        // Calculate composite score based on multiple factors
        float recencyScore = 1.0f / (ep.age + 1);
        float rewardScore = ep.reward * 0.3f;
        float predictionErrorScore = ep.predictionError * 0.2f;
        float curiosityScore = ep.curiosityGain * 0.2f;
        float salienceScore = ep.salience * 0.15f;
        float emotionalScore = std::abs(ep.emotionalValence) * 0.15f;
        float consolidationScore = ep.consolidationProgress * 0.1f;
        
        ep.memoryScore = recencyScore + rewardScore + predictionErrorScore + 
                         curiosityScore + salienceScore + emotionalScore + 
                         consolidationScore;
    }
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveByPredictionError(
    float threshold, size_t maxResults) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        if (ep.predictionError >= threshold) {
            results.push_back(&ep);
        }
    }
    
    // Sort by prediction error magnitude (descending)
    std::sort(results.begin(), results.end(),
              [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                  return a->predictionError > b->predictionError;
              });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveByCuriosityGain(
    float threshold, size_t maxResults) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        if (ep.curiosityGain >= threshold) {
            results.push_back(&ep);
        }
    }
    
    // Sort by curiosity gain (descending)
    std::sort(results.begin(), results.end(),
              [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                  return a->curiosityGain > b->curiosityGain;
              });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveBySalience(
    float threshold, size_t maxResults) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        if (ep.salience >= threshold) {
            results.push_back(&ep);
        }
    }
    
    // Sort by salience (descending)
    std::sort(results.begin(), results.end(),
              [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                  return a->salience > b->salience;
              });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveByEmotionalValence(
    float threshold, size_t maxResults) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        if (std::abs(ep.emotionalValence) >= threshold) {
            results.push_back(&ep);
        }
    }
    
    // Sort by emotional valence magnitude (descending)
    std::sort(results.begin(), results.end(),
              [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                  return std::abs(a->emotionalValence) > std::abs(b->emotionalValence);
              });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesConsolidatedDuringSleep() const {
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        if (ep.wasConsolidatedDuringSleep) {
            results.push_back(&ep);
        }
    }
    
    // Sort by sleep consolidation gain (descending)
    std::sort(results.begin(), results.end(),
              [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                  return a->sleepConsolidationGain > b->sleepConsolidationGain;
              });
    
    return results;
}

size_t NeuralEpisodicMemory::getReplayCount(SimulationStep episodeId) const {
    if (episodeId < episodes_.size()) {
        return episodes_[episodeId].replayCount;
    }
    return 0;
}

SimulationStep NeuralEpisodicMemory::getLastReplayStep(SimulationStep episodeId) const {
    if (episodeId < episodes_.size()) {
        return episodes_[episodeId].lastReplayStep;
    }
    return 0;
}

void NeuralEpisodicMemory::replayConsolidatedEpisode(const EpisodicMemoryItem* episode, bool enhancedReplay) {
    if (!episode || !brain_) return;
    
    // Enhanced replay that considers sleep consolidation
    if (enhancedReplay) {
        // Enhanced replay for episodes consolidated during sleep
        // Use higher activation and apply consolidation effects
        
        float activationMultiplier = 1.0f;
        if (episode->wasConsolidatedDuringSleep) {
            activationMultiplier *= episode->sleepConsolidationGain;
        }
        
        // Reactivate neurons with enhanced activation
        for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
            NeuronId neuron = episode->activeNeurons[i];
            float activation = i < episode->neuronActivations.size() ? 
                               episode->neuronActivations[i] : 0.5f;
            
            float enhancedActivation = activation * activationMultiplier * 1.5f;
            brain_->injectCurrent(neuron, enhancedActivation);
        }
        
        // Enhanced replay for sleep-consolidated memories
        // These memories are more stable and can trigger broader network activation
        if (brain_->getPredictionSystem()) {
            // Integrate with prediction system for enhanced replay
            // This helps prediction errors and learning during sleep
        }
    } else {
        // Standard replay
        replayEpisode(episode);
    }
}

} // namespace nlm