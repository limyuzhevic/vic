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

float NeuralEpisodicMemory::computeSimilarity(const std::vector<float>& query,
                                             const EpisodicMemoryItem& episode) const {
    if (query.empty() || episode.sensoryState.empty()) return 0.0f;
    if (query.size() != episode.sensoryState.size()) return 0.0f;
    
    // Cosine similarity
    float dot = 0.0f, normQ = 0.0f, normE = 0.0f;
    for (size_t i = 0; i < query.size(); ++i) {
        dot += query[i] * episode.sensoryState[i];
        normQ += query[i] * query[i];
        normE += episode.sensoryState[i] * episode.sensoryState[i];
    }
    
    if (normQ < 0.0001f || normE < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normQ) * std::sqrt(normE));
}

// Implement replayEpisode with strength parameter
void NeuralEpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode, float strength) {
    if (!episode || !brain_) return;
    
    // Reactivate neurons that were active during this episode
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = i < episode->neuronActivations.size() ? 
                          episode->neuronActivations[i] * strength : 0.5f * strength;
        
        // Inject current to reawaken this pattern
        brain_->injectCurrent(neuron, activation * 3.0f);
    }
    
    // Also inject sensory pattern if available
    if (!episode->sensoryState.empty()) {
        // This would go through sensory channels - simplified here
    }
}

// Implement updateRelevance with actual relevance storage
void NeuralEpisodicMemory::updateRelevance(SimulationStep episodeId, float relevanceDelta) {
    if (episodeId < episodes_.size()) {
        // In a full implementation, episodes would have a relevance field
        // For now, this would age the episode differently
        auto& episode = episodes_[episodeId];
        episode.novelty += relevanceDelta * 0.1f;  // Store relevance in novelty field as placeholder
    }
}

// Get episode by index
const EpisodicMemoryItem* NeuralEpisodicMemory::getEpisode(size_t index) const {
    if (index < episodes_.size()) {
        return &episodes_[index];
    }
    return nullptr;
}

// Get recent episodes
std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    size_t start = count > episodes_.size() ? 0 : episodes_.size() - count;
    for (size_t i = start; i < episodes_.size(); ++i) {
        results.push_back(&episodes_[i]);
    }
    
    return results;
}

// Get average reward from episodes
float NeuralEpisodicMemory::getAverageReward() const {
    if (episodes_.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& ep : episodes_) {
        sum += ep.reward;
    }
    return sum / episodes_.size();
}

// Clear old episodes
void NeuralEpisodicMemory::consolidate(float relevanceThreshold) {
    // Remove episodes below relevance threshold
    // In a full implementation, episodes would have relevance scores
    // For now, just enforce max capacity
    while (episodes_.size() > maxEpisodes_ * 0.8f) {
        episodes_.erase(episodes_.begin());
    }
}

// Clear all episodes
void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    pImpl->episodeNeurons.clear();
}

// Enable/disable replay
void NeuralEpisodicMemory::enableReplay(bool enable) { replayEnabled_ = enable; }
bool NeuralEpisodicMemory::isReplayEnabled() const { return replayEnabled_; }

// Get episodes for replay (selection based on relevance and recency)
std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(size_t count) const {
    
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

// Replay multiple episodes (for sleep-like consolidation)
void NeuralEpisodicMemory::replaySequence(const std::vector<size_t>& episodeIds) {
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            replayEpisode(&episodes_[id]);
            
            // Small delay between replays
            // (In simulation, this would be steps)
        }
    }
}

// Implement memory replay mechanisms for biologically plausible consolidation
void NeuralEpisodicMemory::replayMemory(const std::vector<size_t>& episodeIds, ReplayMode mode) {
    if (!brain_) return;
    
    switch (mode) {
        case ReplayMode::LTP_CONSOLIDATION:
            // Strong replay for LTP consolidation during slow wave sleep
            replayConsolidationEpisodes(episodeIds, 2.0f);  // Replay strength multiplier
            break;
            
        case ReplayMode::PATTERN_COMPLETION:
            // Pattern completion during wakefulness
            replayPatternCompletion(episodeIds);
            break;
            
        case ReplayMode::SLEEP_REPLAY:
            // Coordinated replay during rest periods (hippocampal-neocortical dialogue)
            replaySleepPhases(episodeIds);
            break;
            
        case ReplayMode::WORKING_MEMORY_REPLAY:
            // Replay important memories for working memory integration
            replayWorkingMemoryIntegration(episodeIds);
            break;
            
        default:
            replaySequence(episodeIds);
            break;
    }
}

// Simulate hippocampal-neocortical dialogue with coordinated replay
void NeuralEpisodicMemory::simulateHippocampalNeocorticalDialogue(
    const std::vector<const EpisodicMemoryItem*>& hippocampalEpisodes,
    const std::vector<std::vector<float>>& neocorticalPatterns) {
    
    if (!brain_) return;
    
    // Implement coordinated replay: hippocampus drives initial replay
    // then neocortex sustains and integrates patterns
    
    for (size_t i = 0; i < hippocampalEpisodes.size(); ++i) {
        const EpisodicMemoryItem* episode = hippocampalEpisodes[i];
        
        if (!episode) continue;
        
        // Hippocampal phase: high-amplitude replay (replay strength multiplier)
        replayEpisode(episode, 3.0f);  // Strong replay for hippocampal activation
        
        // Neocortical phase: pattern completion and integration
        if (i < neocorticalPatterns.size()) {
            // Find and activate matching neocortical patterns
            for (size_t j = 0; j < episode->activeNeurons.size(); ++j) {
                NeuronId neuron = episode->activeNeurons[j];
                
                // Inject current to sustain neocortical activation
                brain_->injectCurrent(neuron, episode->neuronActivations[j] * 0.5f);
                
                // Strengthen connections to neocortical representations
                brain_->strengthenConnections(neuron, neocorticalPatterns[i], 0.02f);
            }
        }
    }
}

// Select episodes for replay based on multiple biological factors
std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::selectEpisodesForReplay(
    size_t maxCount,
    ReplaySelectionStrategy strategy) const {
    
    std::vector<std::tuple<float, float, size_t>> scored;  // (importanceScore, recencyScore, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        const auto& episode = episodes_[i];
        
        float importanceScore = calculateEpisodeImportance(episode);
        float recencyScore = 1.0f / (episode.age + 1);
        float noveltyScore = episode.novelty;
        
        float finalScore = 0.0f;
        
        switch (strategy) {
            case ReplaySelectionStrategy::RECENCY_PRIORITY:
                finalScore = recencyScore * 0.6f + importanceScore * 0.4f;
                break;
                
            case ReplaySelectionStrategy::IMPORTANCE_PRIORITY:
                finalScore = importanceScore * 0.7f + recencyScore * 0.3f;
                break;
                
            case ReplaySelectionStrategy::NOVELTY_DRIVEN:
                finalScore = noveltyScore * 0.5f + importanceScore * 0.3f + recencyScore * 0.2f;
                break;
                
            case ReplaySelectionStrategy::BALANCED:
                finalScore = (importanceScore + recencyScore + noveltyScore) / 3.0f;
                break;
        }
        
        scored.emplace_back(finalScore, importanceScore, i);
    }
    
    // Sort by score (descending)
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) {
            return std::get<0>(a) > std::get<0>(b);
        });
    
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxCount, scored.size()); ++i) {
        results.push_back(&episodes_[std::get<2>(scored[i])]);
    }
    
    return results;
}

// Calculate episode importance based on multiple biological factors
float NeuralEpisodicMemory::calculateEpisodeImportance(const EpisodicMemoryItem& episode) const {
    float importance = 0.0f;
    
    // Reward importance (dopamine-based reinforcement)
    importance += std::max(0.0f, episode.reward) * 0.4f;
    
    // Novelty importance (curiosity-driven)
    importance += episode.novelty * 0.3f;
    
    // Prediction error importance
    importance += episode.resultingReward * 0.2f;
    
    // Recency importance
    importance += 1.0f / (episode.age + 1) * 0.1f;
    
    return std::clamp(importance, 0.0f, 1.0f);
}

// Implement consolidation during simulated sleep/rest cycles
void NeuralEpisodicMemory::consolidateDuringSleep(
    float sleepStartTime,
    float sleepDuration,
    SleepPhase sleepPhase) {
    
    // Detect sleep phases based on time
    // 0.0-0.2: Deep sleep (slow oscillation dominant)
    // 0.2-0.4: Light sleep (theta waves)
    // 0.4-1.0: REM sleep (desynchronized)
    
    float phaseProgress = (sleepStartTime + sleepDuration) / sleepDuration;
    
    switch (sleepPhase) {
        case SleepPhase::DEEP_SLEEP:
            // High-amplitude replay of recent memories
            // LTP consolidation through coordinated reactivation
            replayDuringSleepPhase(0.8f, 1.5f);  // High replay strength, frequency
            break;
            
        case SleepPhase::LIGHT_SLEEP:
            // Pattern completion and integration
            replayDuringSleepPhase(0.5f, 0.8f);  // Moderate replay
            break;
            
        case SleepPhase::REM_SLEEP:
            // Emotional memory processing and integration
            replayDuringSleepPhase(0.3f, 0.6f);  // Lower frequency, associated replay
            break;
    }
}

// Replay episodes during sleep/rest with biological timing
void NeuralEpisodicMemory::replayDuringSleepPhase(float strength, float frequency) {
    // Select episodes based on importance and recency for sleep replay
    std::vector<const EpisodicMemoryItem*> sleepEpisodes = 
        selectEpisodesForReplay(static_cast<size_t>(episodes_.size() * 0.3f),  // Replay top 30%
                               ReplaySelectionStrategy::IMPORTANCE_PRIORITY);
    
    if (sleepEpisodes.empty()) return;
    
    // Coordinated hippocampal-neocortical replay
    // This simulates the replay sequences observed in biological sleep
    for (size_t replayIteration = 0; replayIteration < 3; ++replayIteration) {
        for (const auto* episode : sleepEpisodes) {
            if (!episode) continue;
            
            // Replay with strength-based activation
            replayEpisode(episode, strength);
            
            // Provide neocortical feedback for integration
            if (brain_ && !episode->resultingSensoryState.empty()) {
                // Find and activate related neocortical patterns
                std::vector<float> resultingPattern = episode->resultingSensoryState;
                brain_->activatePattern(resultingPattern, strength * 0.3f);
            }
            
            // Sleep timing constraints: staggered replay with appropriate delays
            // to match biological replay patterns
            Timestamp replayDelay = 0.01f * (1.0f + (replayIteration * frequency));
            // In real implementation, this would be handled by the sleep scheduler
        }
    }
}

// Integrate replay with working memory for enhanced cognition
void NeuralEpisodicMemory::integrateReplayWithWorkingMemory(const std::vector<const EpisodicMemoryItem*>& replayedEpisodes) {
    if (!brain_) return;
    
    // Strengthen working memory traces based on replayed episodes
    for (const auto* episode : replayedEpisodes) {
        if (!episode) continue;
        
        // Update working memory with episode information
        for (size_t i = 0; i < std::min(episode->activeNeurons.size(), 
                                        brain_->getWorkingMemory()->getCurrentSize()); ++i) {
            
            NeuronId neuron = episode->activeNeurons[i];
            float activation = episode->neuronActivations[i];
            
            // Store in working memory with enhanced activation
            brain_->getWorkingMemory()->storeToNeuron(neuron, activation * 1.5f);
            
            // Strengthen working memory connections
            brain_->getWorkingMemory()->strengthenMemory(1.2f);
        }
    }
}

// Private implementation methods

void NeuralEpisodicMemory::replayConsolidationEpisodes(const std::vector<size_t>& episodeIds, float strengthMultiplier) {
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            replayEpisode(&episodes_[id], strengthMultiplier);
        }
    }
}

void NeuralEpisodicMemory::replayPatternCompletion(const std::vector<size_t>& episodeIds) {
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            const auto* episode = &episodes_[id];
            
            // Pattern completion: reactivate partial patterns to retrieve full episodes
            if (brain_ && !episode->sensoryState.empty()) {
                // Find partial matches in episodic memory
                std::vector<const EpisodicMemoryItem*> partialMatches =
                    retrieveSimilar(episode->sensoryState, 3);
                
                // Complete patterns through spreading activation
                for (const auto* match : partialMatches) {
                    if (match && match != episode) {
                        replayEpisode(match, 0.8f);  // Pattern completion strength
                    }
                }
            }
        }
    }
}

void NeuralEpisodicMemory::replaySleepPhases(const std::vector<size_t>& episodeIds) {
    // Implement coordinated replay across hippocampal and neocortical networks
    // during simulated sleep phases
    
    // This simulates the sleep spindle and slow oscillation interactions
    // that facilitate memory consolidation in biological systems
    
    float timeProgress = 0.0f;
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            const auto* episode = &episodes_[id];
            
            // Time-dependent replay: early replay of recent memories,
            // late replay of older but important memories
            float replayTiming = 1.0f - (timeProgress / episodeIds.size());
            
            // Varying replay strength based on timing within sleep cycle
            float phaseReplayStrength = replayTiming;  // High early in sleep
            
            replayEpisode(episode, phaseReplayStrength);
            
            timeProgress += 0.1f;  // Simulate progression through sleep phases
        }
    }
}

void NeuralEpisodicMemory::replayWorkingMemoryIntegration(const std::vector<size_t>& episodeIds) {
    if (!brain_) return;
    
    // Replay specifically to integrate with working memory
    std::vector<const EpisodicMemoryItem*> episodesForIntegration;
    
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            const auto* episode = &episodes_[id];
            
            // Select episodes that match working memory content
            bool hasWorkingMemoryMatch = false;
            auto workingMemory = brain_->getWorkingMemory();
            
            if (workingMemory) {
                for (size_t i = 0; i < episode->activeNeurons.size() && !hasWorkingMemoryMatch; ++i) {
                    NeuronId neuron = episode->activeNeurons[i];
                    if (workingMemory->contains(neuron)) {
                        hasWorkingMemoryMatch = true;
                    }
                }
            }
            
            if (hasWorkingMemoryMatch) {
                episodesForIntegration.push_back(episode);
            }
        }
    }
    
    // Integrate with working memory
    integrateReplayWithWorkingMemory(episodesForIntegration);
}

bool NeuralEpisodicMemory::matchesCriteria(
    const EpisodicMemoryItem& episode,
    const std::vector<float>* sensoryQuery,
    SimulationStep* startTime,
    SimulationStep* endTime,
    ActionType* actionQuery) const {
    
    if (sensoryQuery) {
        if (computeSimilarity(*sensoryQuery, episode) < 0.3f) {
            return false;
        }
    }
    
    if (startTime && endTime) {
        if (episode.timestamp < *startTime || episode.timestamp > *endTime) {
            return false;
        }
    }
    
    if (actionQuery && episode.action != *actionQuery) {
        return false;
    }
    
    return true;
}

bool NeuralEpisodicMemory::matchesCriteria(
    const EpisodicMemoryItem& episode,
    const std::vector<float>* sensoryQuery,
    SimulationStep* startTime,
    SimulationStep* endTime,
    ActionType* actionQuery) const {
    
    if (sensoryQuery) {
        if (computeSimilarity(*sensoryQuery, episode) < 0.3f) {
            return false;
        }
    }
    
    if (startTime && endTime) {
        if (episode.timestamp < *startTime || episode.timestamp > *endTime) {
            return false;
        }
    }
    
    if (actionQuery && episode.action != *actionQuery) {
        return false;
    }
    
    return true;
}

} // namespace nlm
    const EpisodicMemoryItem& episode,
    const std::vector<float>* sensoryQuery,
    SimulationStep* startTime,
    SimulationStep* endTime,
    ActionType* actionQuery) const {
    
    if (sensoryQuery) {
        if (computeSimilarity(*sensoryQuery, episode) < 0.3f) {
            return false;
        }
    }
    
    if (startTime && endTime) {
        if (episode.timestamp < *startTime || episode.timestamp > *endTime) {
            return false;
        }
    }
    
    if (actionQuery && episode.action != *actionQuery) {
        return false;
    }
    
    return true;
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

} // namespace nlm