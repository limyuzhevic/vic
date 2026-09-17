#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_set>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Pattern completion (Hopfield network simulation)
    std::vector<float> patternCompletionWeights;
    std::vector<std::vector<float>> patternMemoryMatrix;
    
    // Temporal dynamics
    SimulationStep lastReplayStep;
    std::vector<float> episodeRecency;
    std::vector<float> episodeRelevance;
    
    // Consolidation state
    bool replayActive;
    std::vector<SimulationStep> lastAccessTime;
    
    // Neural activity patterns for episodes
    std::vector<std::vector<float>> activePatterns;
    
    Impl() : brain(nullptr), replayActive(false), lastReplayStep(0) {}
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(1000)
    , replayEnabled_(true)
{
    // Initialize with reasonable capacity
    episodes_.reserve(maxEpisodes_);
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralEpisodicMemory initialized with capacity: " + std::to_string(maxEpisodes_));
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Create episode copy with age 0
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    
    episodes_.push_back(stored);
    
    // Store neural activity pattern for pattern completion
    if (brain_ && !episode.activeNeurons.empty()) {
        std::vector<float> neuralPattern;
        neuralPattern.reserve(brain_->getTotalNeuronCount());
        
        // Get activation levels from active neurons
        for (NeuronId neuronId : episode.activeNeurons) {
            // Try to get neuron from any region
            bool found = false;
            for (auto& region : brain_->getRegions()) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (neuron->getId() == neuronId) {
                        neuralPattern.push_back(
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f
                        );
                        found = true;
                        break;
                    }
                }
                if (found) break;
            }
        }
        
        // If no neurons found, create placeholder pattern
        if (neuralPattern.empty() && !episode.sensoryState.empty()) {
            neuralPattern = episode.sensoryState;
        }
        
        // Store pattern in memory matrix for pattern completion
        pImpl->activePatterns.push_back(neuralPattern);
        
        // Update pattern completion weights
        if (pImpl->activePatterns.size() > 1) {
            updatePatternCompletionWeights(neuralPattern, pImpl->activePatterns.size() - 2);
        }
    }
    
    // Remove old episodes if over capacity
    while (episodes_.size() > maxEpisodes_) {
        // Remove oldest episode
        size_t removeIndex = 0; // First is oldest (deque front)
        episodes_.erase(episodes_.begin());
        
        // Remove corresponding neural pattern if available
        if (removeIndex < pImpl->activePatterns.size()) {
            pImpl->activePatterns.erase(pImpl->activePatterns.begin() + removeIndex);
        }
        
        // Update recency indices for remaining patterns
        if (removeIndex < pImpl->episodeRecency.size()) {
            pImpl->episodeRecency.erase(pImpl->episodeRecency.begin() + removeIndex);
        }
        if (removeIndex < pImpl->episodeRelevance.size()) {
            pImpl->episodeRelevance.erase(pImpl->episodeRelevance.begin() + removeIndex);
        }
        if (removeIndex < pImpl->lastAccessTime.size()) {
            pImpl->lastAccessTime.erase(pImpl->lastAccessTime.begin() + removeIndex);
        }
    }
    
    // Initialize recency and relevance for new episode
    pImpl->episodeRecency.push_back(1.0f);  // Most recent initially
    pImpl->episodeRelevance.push_back(stored.reward > 0 ? stored.reward : 0.5f);
    pImpl->lastAccessTime.push_back(0);
    
    // Age all episodes
    for (auto& ep : episodes_) {
        ep.age++;
    }
    
    NLM_LOG_DEBUG("Stored episodic memory episode " + std::to_string(episodes_.size() - 1) + 
                  " with relevance: " + std::to_string(pImpl->episodeRelevance.back()));
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    size_t maxResults) const {
    
    std::vector<std::pair<float, size_t>> scores;  // (similarity, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float sim = computeSimilarity(sensoryPattern, episodes_[i]);
        if (sim > 0.01f) {  // Only consider minimally similar episodes
            // Combine with recency and relevance weighting
            float recencyWeight = pImpl->episodeRecency[i];
            float relevanceWeight = pImpl->episodeRelevance[i];
            float score = sim * 0.5f + recencyWeight * 0.3f + relevanceWeight * 0.2f;
            scores.emplace_back(score, i);
        }
    }
    
    // Sort by combined score (descending)
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        if (scores[i].first > 0.2f) {  // Threshold for relevance
            results.push_back(&episodes_[scores[i].second]);
            
            // Mark as recently accessed
            if (pImpl) {
                pImpl->lastAccessTime[scores[i].second] = 0; // Reset to most recent
                pImpl->episodeRecency[scores[i].second] = 1.0f;
            }
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
            
            // Mark as recently accessed
            auto it = &ep - &episodes_[0];
            if (pImpl && it < pImpl->lastAccessTime.size()) {
                pImpl->lastAccessTime[it] = 0;
                pImpl->episodeRecency[it] = 1.0f;
            }
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
            float recencyWeight = pImpl->episodeRecency[i];
            float relevanceWeight = pImpl->episodeRelevance[i];
            float score = (radius - dist) / radius * 0.7f + recencyWeight * 0.2f + relevanceWeight * 0.1f;
            distances.emplace_back(score, i);
        }
    }
    
    // Sort by score (descending)
    std::sort(distances.begin(), distances.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, distances.size()); ++i) {
        if (distances[i].first > 0.1f) {
            results.push_back(&episodes_[distances[i].second]);
            
            // Mark as recently accessed
            if (pImpl && distances[i].second < pImpl->lastAccessTime.size()) {
                pImpl->lastAccessTime[distances[i].second] = 0;
                pImpl->episodeRecency[distances[i].second] = 1.0f;
            }
        }
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveAfterAction(
    ActionType action, size_t maxResults) const {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    for (const auto& ep : episodes_) {
        if (ep.action == action) {
            results.push_back(&ep);
            
            // Mark as recently accessed
            auto it = &ep - &episodes_[0];
            if (pImpl && it < pImpl->lastAccessTime.size()) {
                pImpl->lastAccessTime[it] = 0;
                pImpl->episodeRecency[it] = 1.0f;
            }
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
    if (!episode->sensoryState.empty() && replayEnabled_) {
        // Pattern completion: reconstruct missing parts of the pattern
        std::vector<float> completedPattern = patternCompletion(episode->sensoryState);
        
        // Inject completed pattern through sensory channels
        // This would go through sensory processing in full implementation
    }
    
    // Update episode recency on replay
    auto it = episode - &episodes_[0];
    if (pImpl && it >= 0 && static_cast<size_t>(it) < pImpl->lastAccessTime.size()) {
        pImpl->lastAccessTime[static_cast<size_t>(it)] = 0;
        pImpl->episodeRecency[static_cast<size_t>(it)] = 1.0f;
        
        // Increase relevance based on replay
        pImpl->episodeRelevance[static_cast<size_t>(it)] = 
            std::min(1.0f, pImpl->episodeRelevance[static_cast<size_t>(it)] * 1.1f);
    }
}

void NeuralEpisodicMemory::updateRelevance(SimulationStep episodeId, float relevanceDelta) {
    if (episodeId < episodes_.size()) {
        size_t idx = static_cast<size_t>(episodeId);
        pImpl->episodeRelevance[idx] = std::min(1.0f, pImpl->episodeRelevance[idx] + relevanceDelta);
        
        // Boost recency on relevance update
        pImpl->episodeRecency[idx] = 1.0f;
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
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        if (pImpl->episodeRelevance[i] < relevanceThreshold) {
            toRemove.push_back(i);
        }
    }
    
    // Remove episodes in reverse order
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        episodes_.erase(episodes_.begin() + *it);
        
        if (*it < pImpl->activePatterns.size()) {
            pImpl->activePatterns.erase(pImpl->activePatterns.begin() + *it);
        }
        if (*it < pImpl->episodeRecency.size()) {
            pImpl->episodeRecency.erase(pImpl->episodeRecency.begin() + *it);
        }
        if (*it < pImpl->episodeRelevance.size()) {
            pImpl->episodeRelevance.erase(pImpl->episodeRelevance.begin() + *it);
        }
        if (*it < pImpl->lastAccessTime.size()) {
            pImpl->lastAccessTime.erase(pImpl->lastAccessTime.begin() + *it);
        }
    }
    
    // Also enforce capacity limit
    while (episodes_.size() > maxEpisodes_ * 0.8f) {
        episodes_.erase(episodes_.begin());
        if (!pImpl->activePatterns.empty()) {
            pImpl->activePatterns.erase(pImpl->activePatterns.begin());
        }
        if (!pImpl->episodeRecency.empty()) {
            pImpl->episodeRecency.erase(pImpl->episodeRecency.begin());
        }
        if (!pImpl->episodeRelevance.empty()) {
            pImpl->episodeRelevance.erase(pImpl->episodeRelevance.begin());
        }
        if (!pImpl->lastAccessTime.empty()) {
            pImpl->lastAccessTime.erase(pImpl->lastAccessTime.begin());
        }
    }
}

void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    pImpl->activePatterns.clear();
    pImpl->patternCompletionWeights.clear();
    pImpl->episodeRecency.clear();
    pImpl->episodeRelevance.clear();
    pImpl->lastAccessTime.clear();
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(size_t count) const {
    
    // Select episodes for replay based on:
    // 1. Recency (more recent episodes)
    // 2. Reward (higher reward episodes)
    // 3. Unpredictability (higher novelty/prediction error)
    
    std::vector<std::tuple<float, float, size_t>> scored;  // (score, reward, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float recencyScore = 1.0f / (pImpl->lastAccessTime[i] + 1);
        float rewardScore = episodes_[i].reward;
        float noveltyScore = episodes_[i].novelty;
        
        // Combine with importance weighting
        float score = recencyScore * 0.4f + rewardScore * 0.4f + noveltyScore * 0.2f;
        scored.emplace_back(score, rewardScore, i);
    }
    
    // Sort by score (descending)
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) {
            return std::get<0>(a) > std::get<0>(b);
        });
    
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(count, scored.size()); ++i) {
        size_t idx = std::get<2>(scored[i]);
        results.push_back(&episodes_[idx]);
    }
    
    return results;
}

void NeuralEpisodicMemory::replaySequence(const std::vector<size_t>& episodeIds) {
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            replayEpisode(&episodes_[id]);
            
            // Small delay between replays
            // In simulation, this would be steps
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

// Pattern completion using Hopfield-like dynamics
std::vector<float> NeuralEpisodicMemory::patternCompletion(const std::vector<float>& partialPattern) const {
    if (partialPattern.empty() || pImpl->activePatterns.empty()) {
        return partialPattern;
    }
    
    // Find most similar stored pattern
    float bestSimilarity = 0.0f;
    size_t bestPatternIndex = 0;
    
    for (size_t i = 0; i < pImpl->activePatterns.size(); ++i) {
        float sim = computeSimilarity(partialPattern, pImpl->activePatterns[i]);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestPatternIndex = i;
        }
    }
    
    if (bestSimilarity < 0.3f) {
        return partialPattern;  // Not enough to complete
    }
    
    // Return completed pattern (add some noise for biological realism)
    std::vector<float> completed = pImpl->activePatterns[bestPatternIndex];
    
    // Mix with partial pattern (pattern completion)
    for (size_t i = 0; i < std::min(partialPattern.size(), completed.size()); ++i) {
        if (partialPattern[i] > 0.01f) {
            completed[i] = partialPattern[i] * 0.7f + completed[i] * 0.3f;
        }
    }
    
    // Add small noise for biological realism
    float noiseLevel = 0.05f * (1.0f - bestSimilarity);
    for (float& val : completed) {
        val += (rand() % 200 - 100) / 100.0f * noiseLevel;
        val = std::max(0.0f, std::min(1.0f, val));
    }
    
    return completed;
}

void NeuralEpisodicMemory::updatePatternCompletionWeights(const std::vector<float>& newPattern, size_t index) {
    // Update pattern completion weights (simplified Hopfield network)
    size_t matrixSize = pImpl->patternMemoryMatrix.size();
    
    if (matrixSize == 0) {
        pImpl->patternMemoryMatrix.resize(pImpl->activePatterns.size(), std::vector<float>(pImpl->activePatterns[0].size(), 0.0f));
    }
    
    // Update with new pattern
    for (size_t i = 0; i < newPattern.size(); ++i) {
        pImpl->patternMemoryMatrix[index][i] = newPattern[i];
    }
}

// NeuralAssociativeMemory Implementation
struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Spreading activation state
    std::vector<float> nodeActivations;
    std::vector<float> associationStrengths;
    std::vector<std::unordered_set<NeuronId>> activationSets;
    
    // Co-activation history for Hebbian learning
    std::vector<std::vector<float>> coActivationHistory;
    
    // Cue-to-pattern mapping
    std::unordered_map<NeuronId, std::vector<NeuronId>> cueToPatternMap;
    
    Impl() : brain(nullptr) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
    // Pre-allocate
    patternNeurons_.reserve(100);
    associations_.reserve(500);
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
    // Create pattern neurons if needed
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID && neuronA != neuronB) {
        // Check if association exists
        for (auto& assoc : associations_) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                // Update strength with decay and new learning
                float oldStrength = std::get<2>(assoc);
                float newStrength = std::clamp(oldStrength * 0.9f + strength * 0.1f, 0.0f, 1.0f);
                std::get<2>(assoc) = newStrength;
                
                // Record co-activation for Hebbian learning
                recordCoActivation(neuronA, neuronB, newStrength);
                return;
            }
        }
        
        // Create new association
        associations_.emplace_back(neuronA, neuronB, strength);
        
        // Record co-activation
        recordCoActivation(neuronA, neuronB, strength);
        
        // Update spreading activation
        pImpl->associationStrengths.push_back(strength);
        pImpl->activationSets.emplace_back(std::unordered_set<NeuronId>{neuronA, neuronB});
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!episode.sensoryState.empty() && !episode.resultingSensoryState.empty()) {
        // Associate input with output
        associate(episode.sensoryState, episode.resultingSensoryState, 0.8f);
    }
    
    // Associate action with resulting state
    if (!episode.sensoryState.empty()) {
        // Create action-specific pattern
        std::vector<float> actionPattern(episode.sensoryState.size(), 0.0f);
        actionPattern[0] = static_cast<float>(episode.action) / 10.0f;
        
        associate(actionPattern, episode.resultingSensoryState, 0.7f);
    }
    
    // Associate reward prediction
    float reward = episode.reward;
    if (reward != 0.0f) {
        std::vector<float> rewardPattern(episode.sensoryState.size(), 0.0f);
        rewardPattern[0] = reward;
        
        associate(rewardPattern, episode.resultingSensoryState, 0.6f * std::abs(reward));
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    
    // Find best matching pattern neuron
    NeuronId queryNeuron;
    float bestSim = 0.0f;
    
    for (size_t i = 0; i < patternNeurons_.size(); ++i) {
        float sim = computeSimilarity(queryPattern, patternNeurons_[i].second);
        if (sim > bestSim) {
            bestSim = sim;
            queryNeuron = patternNeurons_[i].first;
        }
    }
    
    if (bestSim < 0.3f) {
        return {};  // Not enough match to retrieve
    }
    
    // Find associated patterns using spreading activation
    std::vector<std::pair<float, std::vector<float>>> scored;
    std::unordered_set<NeuronId> visited;
    
    // Start with direct associations
    std::queue<NeuronId> toProcess;
    toProcess.push(queryNeuron);
    visited.insert(queryNeuron);
    
    while (!toProcess.empty()) {
        NeuronId current = toProcess.front();
        toProcess.pop();
        
        // Find all neurons associated with current
        for (const auto& assoc : associations_) {
            NeuronId otherNeuron;
            
            if (std::get<0>(assoc) == current) {
                otherNeuron = std::get<1>(assoc);
            } else if (std::get<1>(assoc) == current) {
                otherNeuron = std::get<0>(assoc);
            } else {
                continue;
            }
            
            if (visited.find(otherNeuron) == visited.end()) {
                visited.insert(otherNeuron);
                
                // Get strength of association
                float strength = std::get<2>(assoc);
                
                // Find pattern for this neuron
                for (const auto& pn : patternNeurons_) {
                    if (pn.first == otherNeuron) {
                        float score = strength * bestSim;  // Similarity influences strength
                        scored.emplace_back(score, pn.second);
                        toProcess.push(otherNeuron);
                        break;
                    }
                }
            }
        }
    }
    
    // Sort by score (descending)
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    std::vector<std::vector<float>> results;
    for (size_t i = 0; i < std::min(maxResults, scored.size()); ++i) {
        results.push_back(scored[i].second);
    }
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
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
            
            // Update co-activation history
            recordCoActivation(neuronA, neuronB, std::get<2>(assoc));
            return;
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {
    
    std::vector<std::vector<float>> activated;
    std::unordered_set<NeuronId> visited;
    
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
        
        // Pick next neuron with highest association strength
        NeuronId nextBest = nextNeurons[0];
        float maxStrength = std::get<2>(associations_[0]);
        
        for (NeuronId n : nextNeurons) {
            for (const auto& assoc : associations_) {
                if ((std::get<0>(assoc) == currentNeuron && std::get<1>(assoc) == n) ||
                    (std::get<0>(assoc) == n && std::get<1>(assoc) == currentNeuron)) {
                    if (std::get<2>(assoc) > maxStrength) {
                        maxStrength = std::get<2>(assoc);
                        nextBest = n;
                    }
                    break;
                }
            }
        }
        
        currentNeuron = nextBest;
    }
    
    return activated;
}

void NeuralAssociativeMemory::clear() {
    patternNeurons_.clear();
    associations_.clear();
    pImpl->associationStrengths.clear();
    pImpl->activationSets.clear();
    pImpl->coActivationHistory.clear();
    pImpl->cueToPatternMap.clear();
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    // Check if we already have a similar pattern
    for (const auto& pn : patternNeurons_) {
        if (computeSimilarity(pattern, pn.second) > 0.8f) {
            return pn.first;
        }
    }
    
    // Create new pattern neuron
    NeuronId newId(patternNeurons_.size() + 40000);
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

void NeuralAssociativeMemory::recordCoActivation(NeuronId neuronA, NeuronId neuronB, float strength) {
    // Record co-activation history for Hebbian learning
    pImpl->coActivationHistory.push_back({strength, strength});
    
    // Apply Hebbian-like learning to strengthen connections
    if (pImpl->brain) {
        // This would normally update synaptic weights in the brain
        // For now, we just record the co-activation
    }
}

} // namespace nlm
