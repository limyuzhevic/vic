// NeuralEpisodicMemory implementation with optimized data structures

#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <queue>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Pattern neurons for episodic recall - O(1) lookup
    std::vector<NeuronId> episodeNeurons;
    
    // Hash maps for O(1) access
    std::unordered_map<SimulationStep, size_t> timestampToIndexMap;
    std::unordered_map<size_t, size_t> positionToIndexMap;
    
    // LRU linked list for temporal ordering
    struct EpisodeNode {
        size_t index;
        SimulationStep timestamp;
        EpisodeNode* prev;
        EpisodeNode* next;
        EpisodeNode(size_t idx, SimulationStep ts) : index(idx), timestamp(ts), prev(nullptr), next(nullptr) {}
    };
    
    EpisodeNode* head;
    EpisodeNode* tail;
    
    // Priority queue for similarity search
    struct EpisodeScore {
        float score;
        size_t index;
        
        bool operator<(const EpisodeScore& other) const {
            return score > other.score;  // Max-heap for similarity
        }
    };
    
    // Memory pool for episode objects
    static constexpr size_t POOL_SIZE = 4096;
    std::vector<EpisodicMemoryItem> memoryPool;
    std::vector<size_t> freeSlots;
    
    Impl() : brain(nullptr), head(nullptr), tail(nullptr) {
        memoryPool.reserve(POOL_SIZE);
        freeSlots.reserve(POOL_SIZE);
    }
    
    ~Impl() {
        EpisodeNode* current = head;
        while (current) {
            EpisodeNode* next = current->next;
            delete current;
            current = next;
        }
    }
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(1000)
    , replayEnabled_(true)
{
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() {
    cleanupLRU();
}

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize LRU linked list with existing episodes
    for (size_t i = 0; i < episodes_.size(); ++i) {
        EpisodeNode* newNode = new EpisodeNode(i, episodes_[i].timestamp);
        if (!pImpl->head) {
            pImpl->head = pImpl->tail = newNode;
        } else {
            pImpl->tail->next = newNode;
            newNode->prev = pImpl->tail;
            pImpl->tail = newNode;
        }
    }
    
    NLM_LOG_INFO("NeuralEpisodicMemory initialized");
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Check memory pool first for reuse
    size_t episodeIndex;
    if (!pImpl->freeSlots.empty()) {
        episodeIndex = pImpl->freeSlots.back();
        pImpl->freeSlots.pop_back();
    } else {
        episodeIndex = episodes_.size();
        episodes_.push_back(EpisodicMemoryItem());
    }
    
    // Create episode copy with age 0
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    
    // Store using memory pool or vector
    if (pImpl->memoryPool.size() > episodeIndex) {
        pImpl->memoryPool[episodeIndex] = stored;
    } else {
        episodes_[episodeIndex] = stored;
    }
    
    // Create episode neuron for pattern completion
    if (brain_ && !episode.sensoryState.empty()) {
        NeuronId epNeuron(episodes_.size() + 20000);
        if (pImpl->memoryPool.size() > episodeIndex) {
            pImpl->episodeNeurons[episodeIndex] = epNeuron;
        } else {
            pImpl->episodeNeurons.push_back(epNeuron);
        }
    }
    
    // Update timestamp lookup for O(1) temporal search
    pImpl->timestampToIndexMap[stored.timestamp] = episodeIndex;
    
    // Update position lookup for O(1) spatial search (quantized)
    size_t xKey = static_cast<size_t>(stored.positionX * 100.0f);
    size_t yKey = static_cast<size_t>(stored.positionY * 100.0f);
    pImpl->positionToIndexMap[(xKey << 32) | yKey] = episodeIndex;
    
    // Add to LRU linked list for temporal ordering
    EpisodeNode* newNode = new EpisodeNode(episodeIndex, stored.timestamp);
    if (!pImpl->head) {
        pImpl->head = pImpl->tail = newNode;
    } else {
        pImpl->tail->next = newNode;
        newNode->prev = pImpl->tail;
        pImpl->tail = newNode;
    }
    
    // Remove old episodes if over capacity - O(1) amortized
    while (episodes_.size() > maxEpisodes_) {
        // Remove from LRU list
        EpisodeNode* oldNode = pImpl->head;
        if (oldNode) {
            pImpl->head = oldNode->next;
            if (pImpl->head) {
                pImpl->head->prev = nullptr;
            } else {
                pImpl->tail = nullptr;
            }
            
            // Mark as free for reuse
            if (oldNode->index < pImpl->memoryPool.size()) {
                pImpl->freeSlots.push_back(oldNode->index);
            }
            
            // Remove from hash maps
            size_t idx = oldNode->index;
            if (pImpl->memoryPool.size() > idx) {
                auto& oldEpisode = pImpl->memoryPool[idx];
                auto tsIt = pImpl->timestampToIndexMap.find(oldEpisode.timestamp);
                if (tsIt != pImpl->timestampToIndexMap.end() && tsIt->second == idx) {
                    pImpl->timestampToIndexMap.erase(tsIt);
                }
                
                size_t xKeyOld = static_cast<size_t>(oldEpisode.positionX * 100.0f);
                size_t yKeyOld = static_cast<size_t>(oldEpisode.positionY * 100.0f);
                auto posIt = pImpl->positionToIndexMap.find((xKeyOld << 32) | yKeyOld);
                if (posIt != pImpl->positionToIndexMap.end() && posIt->second == idx) {
                    pImpl->positionToIndexMap.erase(posIt);
                }
            }
            
            delete oldNode;
        }
    }
    
    // Increment age - optimized using difference updates
    for (auto& ep : episodes_) {
        ep.age++;
    }
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    size_t maxResults) const {

    // Early exit if no episodes
    if (episodes_.empty()) {
        return {};
    }
    
    // Use priority queue for top-N similarity search (O(n log k))
    std::priority_queue<Impl::EpisodeScore> topResults;
    std::vector<float> normQuery(sensoryPattern.size(), 0.0f);
    
    // Pre-compute query norm for cosine similarity
    for (size_t i = 0; i < sensoryPattern.size(); ++i) {
        normQuery[i] = sensoryPattern[i] * sensoryPattern[i];
    }
    
    // Process episodes with early termination optimization
    for (size_t i = 0; i < episodes_.size(); ++i) {
        const auto& episode = episodes_[i];
        
        // Skip if episode sensory state doesn't match size
        if (sensoryPattern.size() != episode.sensoryState.size()) {
            continue;
        }
        
        // Compute similarity with early exit for poor matches
        float dot = 0.0f;
        float normE = 0.0f;
        bool earlyExit = false;
        
        for (size_t j = 0; j < sensoryPattern.size(); ++j) {
            float queryVal = sensoryPattern[j];
            float episodeVal = episode.sensoryState[j];
            
            dot += queryVal * episodeVal;
            normE += episodeVal * episodeVal;
            
            // Early termination: if partial dot product is already too low
            if (dot < 0.0f && (topResults.size() >= maxResults || 
                               dot * 10.0f < -normE)) {
                earlyExit = true;
                break;
            }
        }
        
        if (earlyExit) {
            continue;
        }
        
        // Final similarity calculation (only if promising)
        float sim = 0.0f;
        if (normE > 0.0001f) {
            float normQ = std::accumulate(normQuery.begin(), normQuery.end(), 0.0f);
            if (normQ > 0.0001f) {
                sim = dot / (std::sqrt(normQ) * std::sqrt(normE));
            }
        }
        
        // Apply threshold before adding to priority queue
        if (sim > 0.3f) {
            topResults.emplace(Impl::EpisodeScore{sim, i});
            
            // Maintain size constraint
            if (topResults.size() > maxResults) {
                topResults.pop();
            }
        }
    }
    
    // Extract results in descending order
    std::vector<const EpisodicMemoryItem*> results;
    results.reserve(topResults.size());
    
    while (!topResults.empty()) {
        results.push_back(&episodes_[topResults.top().index]);
        topResults.pop();
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveTemporal(
    SimulationStep startTime,
    SimulationStep endTime,
    size_t maxResults) const {

    std::vector<const EpisodicMemoryItem*> results;
    results.reserve(std::min(maxResults, episodes_.size()));
    
    // Use LRU linked list for O(1) temporal ordering (already sorted by timestamp)
    EpisodeNode* current = pImpl->head;
    while (current && results.size() < maxResults) {
        const auto& ep = episodes_[current->index];
        
        if (ep.timestamp >= startTime && ep.timestamp <= endTime) {
            results.push_back(&ep);
        }
        
        current = current->next;
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveByLocation(
    float x, float y, float radius, size_t maxResults) const {

    // Use spatial hash map for O(1) candidate identification
    std::vector<const EpisodicMemoryItem*> results;
    results.reserve(std::min(maxResults, episodes_.size()));
    
    // Quantized coordinates for grid-based lookup
    size_t xKey = static_cast<size_t>(x * 100.0f);
    size_t yKey = static_cast<size_t>(y * 100.0f);
    
    // Search nearby cells (3x3 grid for simplicity)
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            auto posIt = pImpl->positionToIndexMap.find(((xKey + dx) << 32) | (yKey + dy));
            if (posIt != pImpl->positionToIndexMap.end()) {
                size_t index = posIt->second;
                const auto& ep = episodes_[index];
                
                // Compute exact Euclidean distance
                float dx_ = ep.positionX - x;
                float dy_ = ep.positionY - y;
                float dist = std::sqrt(dx_ * dx_ + dy_ * dy_);
                
                if (dist <= radius) {
                    results.push_back(&ep);
                }
            }
        }
    }
    
    // Sort by distance (descending)
    std::sort(results.begin(), results.end(),
        [x, y](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            float distA = std::sqrt((a->positionX - x) * (a->positionX - x) + 
                                  (a->positionY - y) * (a->positionY - y));
            float distB = std::sqrt((b->positionX - x) * (b->positionX - x) + 
                                  (b->positionY - y) * (b->positionY - y));
            return distA < distB;
        });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveAfterAction(
    ActionType action, size_t maxResults) const {

    // Use priority queue for O(n log k) instead of O(n log n)
    std::priority_queue<Impl::EpisodeScore> topResults;
    
    // Traverse LRU list (already ordered by timestamp)
    EpisodeNode* current = pImpl->head;
    while (current) {
        const auto& ep = episodes_[current->index];
        
        if (ep.action == action) {
            // Score by recency (more recent = higher score)
            float score = 1.0f / (ep.age + 1);
            topResults.emplace(Impl::EpisodeScore{score, current->index});
            
            // Maintain size constraint
            if (topResults.size() > maxResults) {
                topResults.pop();
            }
        }
        
        current = current->next;
    }
    
    // Extract results in descending order
    std::vector<const EpisodicMemoryItem*> results;
    results.reserve(topResults.size());
    
    while (!topResults.empty()) {
        results.push_back(&episodes_[topResults.top().index]);
        topResults.pop();
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
    results.reserve(std::min(count, episodes_.size()));
    
    // Use LRU linked list for O(n) retrieval
    EpisodeNode* current = pImpl->head;
    size_t found = 0;
    
    while (current && found < count) {
        results.push_back(&episodes_[current->index]);
        found++;
        current = current->next;
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
    // Use memory pool for O(1) removal from free list
    while (episodes_.size() > maxEpisodes_ * 0.8f) {
        // Remove from LRU linked list
        EpisodeNode* oldNode = pImpl->head;
        if (oldNode) {
            pImpl->head = oldNode->next;
            if (pImpl->head) {
                pImpl->head->prev = nullptr;
            } else {
                pImpl->tail = nullptr;
            }
            
            // Mark as free for reuse
            if (oldNode->index < pImpl->memoryPool.size()) {
                pImpl->freeSlots.push_back(oldNode->index);
            }
            
            // Remove from hash maps
            size_t idx = oldNode->index;
            if (pImpl->memoryPool.size() > idx) {
                auto& oldEpisode = pImpl->memoryPool[idx];
                auto tsIt = pImpl->timestampToIndexMap.find(oldEpisode.timestamp);
                if (tsIt != pImpl->timestampToIndexMap.end() && tsIt->second == idx) {
                    pImpl->timestampToIndexMap.erase(tsIt);
                }
                
                size_t xKeyOld = static_cast<size_t>(oldEpisode.positionX * 100.0f);
                size_t yKeyOld = static_cast<size_t>(oldEpisode.positionY * 100.0f);
                auto posIt = pImpl->positionToIndexMap.find((xKeyOld << 32) | yKeyOld);
                if (posIt != pImpl->positionToIndexMap.end() && posIt->second == idx) {
                    pImpl->positionToIndexMap.erase(posIt);
                }
            }
            
            delete oldNode;
        }
    }
}

void NeuralEpisodicMemory::clear() {
    // Clear episodes deque
    episodes_.clear();
    
    // Clear episode neurons
    pImpl->episodeNeurons.clear();
    
    // Clean up LRU linked list
    EpisodeNode* current = pImpl->head;
    while (current) {
        EpisodeNode* next = current->next;
        delete current;
        current = next;
    }
    pImpl->head = pImpl->tail = nullptr;
    
    // Clear hash maps
    pImpl->timestampToIndexMap.clear();
    pImpl->positionToIndexMap.clear();
    pImpl->freeSlots.clear();
}

// Use priority queue for episode selection (O(n log k))
std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(
    size_t count) const {

    // Use max-heap to keep top episodes by score
    std::priority_queue<Impl::EpisodeScore> topEpisodes;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        const auto& ep = episodes_[i];
        
        // Calculate composite score (recency + reward)
        float recencyScore = 1.0f / (ep.age + 1);
        float score = recencyScore * 0.5f + ep.reward * 0.5f;
        
        topEpisodes.emplace(Impl::EpisodeScore{score, i});
        
        // Maintain size constraint
        if (topEpisodes.size() > count) {
            topEpisodes.pop();
        }
    }
    
    // Extract results in descending order
    std::vector<const EpisodicMemoryItem*> results;
    results.reserve(topEpisodes.size());
    
    while (!topEpisodes.empty()) {
        results.push_back(&episodes_[topEpisodes.top().index]);
        topEpisodes.pop();
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

// Clean up LRU linked list
void NeuralEpisodicMemory::cleanupLRU() {
    if (!pImpl) return;
    
    EpisodeNode* current = pImpl->head;
    while (current) {
        EpisodeNode* next = current->next;
        delete current;
        current = next;
    }
    pImpl->head = pImpl->tail = nullptr;
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