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
    
    // Enhanced replay selection based on multiple criteria:
    // 1. Recency (more recent episodes)
    // 2. Reward (higher reward episodes)
    // 3. Unpredictability (higher prediction error episodes)
    // 4. Novelty (higher novelty episodes)
    // 5. Coherence (higher coherence with current state)
    // 6. Importance (higher importance scores)
    
    std::vector<std::tuple<float, float, size_t, float>> scored;  // (score, reward, index, importance)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        // Recency score - more recent episodes are more important for replay
        float recencyScore = 1.0f / (episodes_[i].age + 1);
        
        // Reward score - reward learning benefits from replay
        float rewardScore = episodes_[i].reward;
        
        // Novelty score - novel episodes need consolidation
        float noveltyScore = episodes_[i].novelty;
        
        // Coherence score - episodes that cohere with current state
        float coherenceScore = 0.5f;  // Placeholder - would compute coherence with current neural state
        
        // Importance score - intrinsic importance of the episode
        float importanceScore = coherenceScore * 0.5f + 
                              recencyScore * 0.2f + 
                              noveltyScore * 0.2f + 
                              rewardScore * 0.1f;
        
        // Combined replay score using multiple criteria
        float replayScore = recencyScore * 0.25f + 
                           rewardScore * 0.25f + 
                           noveltyScore * 0.20f + 
                           coherenceScore * 0.15f + 
                           importanceScore * 0.15f;
        
        scored.emplace_back(replayScore, rewardScore, i, importanceScore);
    }
    
    // Sort by replay score (highest first)
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

void NeuralEpisodicMemory::integrateIntoSchema(const EpisodicMemoryItem* episode) {
    if (!episode || !brain_) return;
    
    // Schema integration involves:
    // 1. Identifying patterns in the episode
    // 2. Integrating with existing schemas in semantic memory
    // 3. Abstracting general knowledge from specific experience
    
    // For now, this is a placeholder - in full implementation this would
    // integrate with semantic memory and create generalized knowledge
    
    NLM_LOG_INFO("Integrating episode into schema: timestamp " + 
                std::to_string(episode->timestamp));
}

void NeuralEpisodicMemory::recordReplayRepetition(const EpisodicMemoryItem* episode) {
    if (!episode) return;
    
    // Track replay repetitions for consolidation
    // In a full implementation, this would maintain replay counters
    // and adjust consolidation based on repetition count
    
    // For now, just track that replay occurred
    NLM_LOG_INFO("Recording replay repetition for episode: " + 
                std::to_string(episode->timestamp));
}

void NeuralEpisodicMemory::enhancedReplayEpisode(const EpisodicMemoryItem* episode, 
                                               float replayStrength,
                                               bool enablePatternCompletion,
                                               bool enableSchemaIntegration) {
    if (!episode || !brain_) return;
    
    // Enhanced replay with multiple mechanisms:
    // 1. Synaptic strengthening through spike timing-dependent plasticity
    // 2. Pattern completion through attractor network dynamics
    // 3. Schema integration for abstract knowledge formation
    // 4. Cross-modal integration
    
    // Reactivate neurons that were active during this episode with enhanced strength
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = i < episode->neuronActivations.size() ? 
                          episode->neuronActivations[i] : 0.5f;
        
        // Enhanced current injection based on replay strength
        float enhancedCurrent = activation * 3.0f * replayStrength;
        brain_->injectCurrent(neuron, enhancedCurrent);
        
        // Apply spike timing-dependent plasticity if enabled
        // This would involve timing spikes with existing activity
    }
    
    // Pattern completion - if enabled, use partial activation to complete full pattern
    if (enablePatternCompletion) {
        // Use working memory or current activation patterns to complete episode pattern
        if (brain_->getWorkingMemory()) {
            std::vector<float> currentPattern = brain_->getWorkingMemory()->retrieve();
            
            // Pattern completion dynamics
            for (size_t i = 0; i < std::min(currentPattern.size(), 
                                           episode->sensoryState.size()); ++i) {
                float target = episode->sensoryState[i];
                float current = currentPattern[i];
                float completion = (target - current) * 0.5f * replayStrength;
                // Would apply completion to relevant neural populations
            }
        }
    }
    
    // Schema integration - integrate episode into abstract knowledge
    if (enableSchemaIntegration) {
        integrateIntoSchema(episode);
    }
    
    // Record replay for consolidation tracking
    recordReplayRepetition(episode);
}

// Implement the new methods from the header
std::vector<std::vector<const EpisodicMemoryItem*>> NeuralEpisodicMemory::getGroupedEpisodesForReplay(
    size_t maxTotalEpisodes, size_t maxGroupSize) const {
    
    std::vector<std::vector<const EpisodicMemoryItem*>> groups;
    
    // Score all episodes for importance
    std::vector<std::pair<float, size_t>> scoredEpisodes;
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float recencyScore = 1.0f / (episodes_[i].age + 1);
        float rewardScore = episodes_[i].reward;
        float noveltyScore = episodes_[i].novelty;
        
        float totalScore = recencyScore * 0.4f + rewardScore * 0.3f + 
                          noveltyScore * 0.3f;
        
        scoredEpisodes.emplace_back(totalScore, i);
    }
    
    // Sort by score
    std::sort(scoredEpisodes.begin(), scoredEpisodes.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Group by temporal proximity (episodes with similar timestamps)
    size_t currentGroupIndex = 0;
    std::vector<const EpisodicMemoryItem*> currentGroup;
    
    for (const auto& scored : scoredEpisodes) {
        size_t episodeIndex = scored.second;
        const auto& episode = episodes_[episodeIndex];
        
        if (currentGroup.size() < maxGroupSize) {
            // Check if this episode is temporally close to others in the group
            bool canAddToGroup = true;
            if (!currentGroup.empty()) {
                // For simplicity, just check if it's one of the top recent episodes
                // In full implementation, would check temporal distance
                canAddToGroup = true;
            }
            
            if (canAddToGroup) {
                currentGroup.push_back(&episode);
            }
        }
        
        if (currentGroup.size() >= maxGroupSize) {
            groups.push_back(currentGroup);
            currentGroup.clear();
        }
        
        if (groups.size() * maxGroupSize >= maxTotalEpisodes) {
            break;
        }
    }
    
    if (!currentGroup.empty()) {
        groups.push_back(currentGroup);
    }
    
    return groups;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getImportantEpisodesForRehearsal(
    float importanceThreshold) const {
    
    std::vector<const EpisodicMemoryItem*> importantEpisodes;
    
    for (const auto& episode : episodes_) {
        // Calculate importance based on multiple factors
        float recencyScore = 1.0f / (episode.age + 1);
        float rewardScore = episode.reward;
        float noveltyScore = episode.novelty;
        
        float importance = recencyScore * 0.4f + rewardScore * 0.3f + 
                          noveltyScore * 0.3f;
        
        if (importance >= importanceThreshold) {
            importantEpisodes.push_back(&episode);
        }
    }
    
    // Sort by importance (highest first)
    std::sort(importantEpisodes.begin(), importantEpisodes.end(),
        [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            float scoreA = 1.0f / (a->age + 1) * 0.4f + a->reward * 0.3f + a->novelty * 0.3f;
            float scoreB = 1.0f / (b->age + 1) * 0.4f + b->reward * 0.3f + b->novelty * 0.3f;
            return scoreA > scoreB;
        });
    
    return importantEpisodes;
}

void NeuralEpisodicMemory::scheduleReplays(float timeUntilSleep) {
    if (!brain_ || timeUntilSleep <= 0.0f) return;
    
    // Schedule replays based on time until sleep
    // More replays if more time available
    size_t numReplays = std::min(static_cast<size_t>(timeUntilSleep / 100.0f), 
                                static_cast<size_t>(episodes_.size() / 10));
    
    if (numReplays == 0) return;
    
    // Get episodes for replay with temporal grouping
    auto groups = getGroupedEpisodesForReplay(numReplays * 3, 3);  // 3 episodes per group
    
    // Schedule replays - in full implementation, would schedule actual replay events
    for (const auto& group : groups) {
        if (group.empty()) continue;
        
        // For now, just log the scheduling
        // In full implementation, would schedule replay through brain or event system
    }
}

NeuralEpisodicMemory::ReplayStats NeuralEpisodicMemory::getReplayStats() const {
    ReplayStats stats;
    stats.totalReplays = 0;
    stats.totalRehearsals = 0;
    
    // Calculate basic statistics
    // In full implementation, would track actual replay events
    
    return stats;
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