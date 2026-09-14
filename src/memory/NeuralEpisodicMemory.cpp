#include <unordered_map>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Pattern neurons for episodic recall
    std::vector<NeuronId> episodeNeurons;
    
    // New: Pattern storage for distributed representation
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons;
    
    // Synaptic connections between patterns (Hebbian learning)
    std::vector<std::pair<std::vector<float>, std::vector<float>>> synapticPairs;
    std::vector<float> synapticStrengths;
    
    // Replay queue for consolidation
    std::deque<size_t> replayQueue;
    
    // Index structures for efficient retrieval
    std::unordered_map<SimulationStep, std::vector<size_t>> temporalIndex;
    std::unordered_map<uint64_t, std::vector<size_t>> locationIndex;
    std::unordered_map<ActionType, std::vector<size_t>> actionIndex;
    
    // Reward-based priority queue
    struct EpisodeWithPriority {
        size_t index;
        float priority;
    };
    
    std::priority_queue<EpisodeWithPriority> rewardPriorityQueue;
    
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
    
    // Compute neural pattern for this episode
    std::vector<float> episodePattern;
    if (brain_ && !episode.sensoryState.empty()) {
        // Encode sensory state as neural pattern
        episodePattern = encodeToNeuralPattern(episode.sensoryState);
        
        // Create pattern neuron for distributed storage
        NeuronId patternNeuron(static_cast<NeuronId>(pImpl->patternNeurons.size() + 40000));
        pImpl->patternNeurons.push_back({patternNeuron, episodePattern});
        
        // Create distributed synaptic connections for consolidation
        if (pImpl->patternNeurons.size() > 1) {
            // Hebbian learning: co-activate related patterns
            size_t prevPatternIdx = pImpl->patternNeurons.size() - 2;
            std::vector<float> prevPattern = pImpl->patternNeurons[prevPatternIdx].second;
            
            // Compute similarity and form association
            float similarity = computeSimilarity(episodePattern, prevPattern);
            if (similarity > 0.3f) {
                pImpl->synapticPairs.push_back({prevPattern, episodePattern});
                pImpl->synapticStrengths.push_back(similarity * 0.8f);
            }
        }
    }
    
    // Add to deque (main storage)
    episodes_.push_back(stored);
    
    // Update indexes for efficient retrieval
    updateIndexes(static_cast<size_t>(episodes_.size() - 1));
    
    // Add to replay queue if replay is enabled
    if (replayEnabled_) {
        pImpl->replayQueue.push_back(static_cast<size_t>(episodes_.size() - 1));
    }
    
    // Remove old episodes if over capacity
    while (episodes_.size() > maxEpisodes_) {
        size_t removedIdx = 0;
        episodes_.pop_front();
        
        // Remove from indexes
        removeFromIndexes(removedIdx);
        
        // Remove pattern neurons
        if (!pImpl->patternNeurons.empty()) {
            pImpl->patternNeurons.erase(pImpl->patternNeurons.begin());
        }
        
        // Adjust replay queue indices
        adjustReplayQueueIndices(removedIdx);
    }
    
    // Age all episodes
    for (auto& ep : episodes_) {
        ep.age++;
    }
}

// Continue storing episode - implementation now complete

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    size_t maxResults) const {
    
    // NEW: Use pattern completion for better recall
    std::vector<const EpisodicMemoryItem*> similar;
    
    // Compute neural pattern for query
    std::vector<float> queryPattern = encodeToNeuralPattern(sensoryPattern);
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        // Check if we have stored pattern for this episode
        if (i < pImpl->patternNeurons.size()) {
            // Use pattern matching for retrieval
            float dist = computePatternDistance(queryPattern, pImpl->patternNeurons[i].second);
            float similarity = 1.0f - dist; // Convert distance to similarity
            
            if (similarity > 0.2f) { // Lower threshold for neural patterns
                similar.push_back(&episodes_[i]);
            }
        } else {
            // Fallback to sensory state comparison
            float sim = computeSimilarity(sensoryPattern, episodes_[i]);
            if (sim > 0.3f) {
                similar.push_back(&episodes_[i]);
            }
        }
    }
    
    // Sort by similarity (descending)
    std::sort(similar.begin(), similar.end(),
              [this, &queryPattern](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                  float simA = 0.0f, simB = 0.0f;
                  size_t idxA = getIndex(a), idxB = getIndex(b);
                  
                  if (idxA < pImpl->patternNeurons.size()) {
                      simA = computePatternSimilarity(queryPattern, pImpl->patternNeurons[idxA].second);
                  }
                  if (idxB < pImpl->patternNeurons.size()) {
                      simB = computePatternSimilarity(queryPattern, pImpl->patternNeurons[idxB].second);
                  }
                  
                  return simA > simB;
              });
    
    if (similar.size() > maxResults) {
        similar.resize(maxResults);
    }
    
    return similar;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveTemporal(
    SimulationStep startTime,
    SimulationStep endTime,
    size_t maxResults) const {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    // Use temporal index for efficient retrieval
    for (const auto& pair : pImpl->temporalIndex) {
        if (pair.first >= startTime && pair.first <= endTime) {
            for (size_t idx : pair.second) {
                if (idx < episodes_.size()) {
                    results.push_back(&episodes_[idx]);
                }
            }
        }
    }
    
    // Sort by timestamp (descending - most recent first)
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
    float x, float y, float radius,
    size_t maxResults) const {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    // Use location index for efficient retrieval
    uint64_t locHash = *reinterpret_cast<const uint64_t*>(&x) ^ *reinterpret_cast<const uint64_t*>(&y);
    
    auto it = pImpl->locationIndex.find(locHash);
    if (it != pImpl->locationIndex.end()) {
        for (size_t idx : it->second) {
            if (idx < episodes_.size()) {
                const auto& ep = episodes_[idx];
                float dx = ep.positionX - x;
                float dy = ep.positionY - y;
                float dist = std::sqrt(dx * dx + dy * dy);
                
                if (dist <= radius) {
                    results.push_back(&episodes_[idx]);
                }
            }
        }
    }
    
    // Return by distance (closest first)
    std::sort(results.begin(), results.end(),
              [x, y](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                  float distA = std::sqrt(std::pow(a->positionX - x, 2) + std::pow(a->positionY - y, 2));
                  float distB = std::sqrt(std::pow(b->positionX - x, 2) + std::pow(b->positionY - y, 2));
                  return distA < distB;
              });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveAfterAction(
    ActionType action, size_t maxResults) const {
    
    std::vector<const EpisodicMemoryItem*> results;
    
    // Use action index for efficient retrieval
    auto it = pImpl->actionIndex.find(action);
    if (it != pImpl->actionIndex.end()) {
        for (size_t idx : it->second) {
            if (idx < episodes_.size()) {
                results.push_back(&episodes_[idx]);
            }
        }
    }
    
    // Sort by timestamp (most recent first)
    std::sort(results.begin(), results.end(),
              [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                  return a->timestamp > b->timestamp;
              });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

void NeuralEpisodicMemory::updateIndexes(size_t index) {
    if (index >= episodes_.size()) return;
    
    const auto& ep = episodes_[index];
    
    // Update temporal index
    pImpl->temporalIndex[ep.timestamp].push_back(index);
    
    // Update location index
    uint64_t locHash = *reinterpret_cast<const uint64_t*>(&ep.positionX) ^ *reinterpret_cast<const uint64_t*>(&ep.positionY);
    pImpl->locationIndex[locHash].push_back(index);
    
    // Update action index
    pImpl->actionIndex[ep.action].push_back(index);
    
    // Update reward priority queue
    pImpl->rewardPriorityQueue.push({index, ep.reward * (1.0f + ep.novelty)});
}

void NeuralEpisodicMemory::removeFromIndexes(size_t index) {
    // Remove from all indexes - simplified implementation
    // In a real implementation, we would maintain more complex indexing structures
    
    // Remove from temporal index
    for (auto it = pImpl->temporalIndex.begin(); it != pImpl->temporalIndex.end(); ++it) {
        auto& indices = it->second;
        indices.erase(std::remove(indices.begin(), indices.end(), index), indices.end());
        if (indices.empty()) {
            pImpl->temporalIndex.erase(it++);
        }
    }
    
    // Similar removal from other indexes...
}

void NeuralEpisodicMemory::adjustReplayQueueIndices(size_t removedIdx) {
    // Adjust indices in replay queue
    std::deque<size_t> newQueue;
    for (size_t idx : pImpl->replayQueue) {
        if (idx > removedIdx) {
            newQueue.push_back(idx - 1);
        } else if (idx != removedIdx) {
            newQueue.push_back(idx);
        }
    }
    pImpl->replayQueue = newQueue;
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
    // For now, this uses reward as a proxy for relevance
    
    // Find episodes with reward below threshold
    std::vector<size_t> toRemove;
    for (size_t i = 0; i < episodes_.size(); ++i) {
        if (episodes_[i].reward < relevanceThreshold && episodes_[i].age > 100) {
            toRemove.push_back(i);
        }
    }
    
    // Remove low-relevance old episodes (in reverse order)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        // Remove from main storage
        episodes_.erase(episodes_.begin() + idx);
        
        // Remove from pattern neurons
        if (idx < pImpl->patternNeurons.size()) {
            pImpl->patternNeurons.erase(pImpl->patternNeurons.begin() + idx);
        }
        
        // Remove from synaptic pairs (complex cleanup)
        // For simplicity, we'll just rebuild the synaptic structures
    }
    
    // Rebuild synaptic connections between remaining patterns
    rebuildSynapticConnections();
    
    // Update replay queue
    adjustReplayQueueIndices(0); // Dummy call to clear indices
}

void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    pImpl->episodeNeurons.clear();
    pImpl->patternNeurons.clear();
    pImpl->synapticPairs.clear();
    pImpl->synapticStrengths.clear();
    pImpl->replayQueue.clear();
    
    // Clear indexes
    pImpl->temporalIndex.clear();
    pImpl->locationIndex.clear();
    pImpl->actionIndex.clear();
    
    // Clear priority queue
    while (!pImpl->rewardPriorityQueue.empty()) {
        pImpl->rewardPriorityQueue.pop();
    }
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(size_t count) const {
    
    // Select episodes for replay based on:
    // 1. Recency (more recent episodes)
    // 2. Reward (higher reward episodes)
    // 3. Unpredictability (higher prediction error episodes)
    
    std::vector<std::tuple<float, float, size_t>> scored;  // (score, reward, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float recencyScore = 1.0f / (episodes_[i].age + 1);
        float rewardScore = episodes_[i].reward;
        float noveltyScore = episodes_[i].novelty;
        
        // Combined score
        float score = recencyScore * 0.4f + rewardScore * 0.4f + noveltyScore * 0.2f;
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

void NeuralEpisodicMemory::rebuildSynapticConnections() {
    // Rebuild synaptic connections between remaining patterns
    pImpl->synapticPairs.clear();
    pImpl->synapticStrengths.clear();
    
    if (pImpl->patternNeurons.size() < 2) return;
    
    // Form connections between similar patterns
    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
        for (size_t j = i + 1; j < pImpl->patternNeurons.size(); ++j) {
            float similarity = computePatternSimilarity(
                pImpl->patternNeurons[i].second,
                pImpl->patternNeurons[j].second);
            
            if (similarity > 0.3f) {
                pImpl->synapticPairs.push_back({pImpl->patternNeurons[i].second, 
                                               pImpl->patternNeurons[j].second});
                pImpl->synapticStrengths.push_back(similarity * 0.9f);
            }
        }
    }
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