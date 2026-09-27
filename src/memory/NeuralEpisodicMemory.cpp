#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

// Helper for episodic memory allocations
void* allocateEpisodicMemory(size_t size) {
    return nlm::MemoryManager::MemoryTracker::getInstance().allocate(
        size, __FILE__, __LINE__, "EpisodicMemory");
}

void freeEpisodicMemory(void* ptr, size_t size) {
    nlm::MemoryManager::MemoryTracker::getInstance().free(ptr, __FILE__, __LINE__, "EpisodicMemory");
}

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Pattern neurons for episodic recall
    std::vector<NeuronId> episodeNeurons;
    
    // Memory tracking
    size_t totalMemoryAllocated;
    size_t allocationCount;
    
    Impl() : brain(nullptr), totalMemoryAllocated(0), allocationCount(0) {
        // Start memory profiling
        nlm::MemoryManager::getProfiler().takeSnapshot("NeuralEpisodicMemory startup");
    }
    
    ~Impl() {
        // Take memory snapshot before destruction
        nlm::MemoryManager::getProfiler().takeSnapshot("NeuralEpisodicMemory shutdown");
        nlm::MemoryMonitor::getStatusReport();
    }
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(1000)
    , replayEnabled_(true)
{
    NLM_LOG_INFO("NeuralEpisodicMemory created with new memory management");
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() {
    // Take memory snapshot before destruction
    nlm::MemoryManager::getProfiler().takeSnapshot("NeuralEpisodicMemory destruction");
    NLM_LOG_INFO("NeuralEpisodicMemory destroyed");
}

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Register with memory monitor
    nlm::MemoryMonitor::registerComponent("NeuralEpisodicMemory", 0);
    
    // Enable memory profiling
    nlm::MemoryManager::getProfiler().takeSnapshot("NeuralEpisodicMemory initialize");
    
    NLM_LOG_INFO("NeuralEpisodicMemory initialized with enhanced memory management");
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Track memory allocation
    size_t episodeSize = sizeof(EpisodicMemoryItem) + 
                        episode.sensoryState.size() * sizeof(float) +
                        episode.activeNeurons.size() * sizeof(NeuronId) +
                        episode.neuronActivations.size() * sizeof(float);
    
    // Allocate memory using tracked allocator
    void* mem = allocateEpisodicMemory(episodeSize);
    
    // Create episode copy with age 0
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    
    // Store using episodic memory storage
    episodes_.push_back(std::move(stored));
    
    // Create episode neuron for pattern completion using custom allocator
    if (brain_ && !episode.sensoryState.empty()) {
        NeuronId epNeuron(episodes_.size() + 20000);
        pImpl->episodeNeurons.push_back(epNeuron);
        
        // Use CSR matrix for efficient episodic memory indexing
        nlm::MemoryStructures::CSRMatrix& csr = nlm::MemoryStructures::getGlobalCSR();
        size_t row = episodeNeurons_.size();
        size_t col = row;
        float weight = 1.0f / (row + 1);
        csr.addConnection(row, col, weight);
        
        // Compress for efficiency
        csr.compress();
        
        // Update memory monitoring
        pImpl->totalMemoryAllocated += episodeSize;
        pImpl->allocationCount++;
        
        // Update memory monitor
        nlm::MemoryMonitor::updateComponentMemory("NeuralEpisodicMemory", 
                                                 pImpl->totalMemoryAllocated);
        
        // Take memory snapshot periodically
        static uint64_t lastSnapshot = 0;
        uint64_t currentSnapshot = nlm::MemoryManager::getProfiler().getCurrentTimestamp();
        if (currentSnapshot - lastSnapshot > 10000) {  // Every 10 seconds
            nlm::MemoryManager::getProfiler().takeSnapshot("NeuralEpisodicMemory episodic");
            lastSnapshot = currentSnapshot;
        }
    }
    
    // Remove old episodes if over capacity
    while (episodes_.size() > maxEpisodes_) {
        size_t oldSize = sizeof(episodes_.front()) + 
                        episodes_.front().sensoryState.size() * sizeof(float) +
                        episodes_.front().activeNeurons.size() * sizeof(NeuronId) +
                        episodes_.front().neuronActivations.size() * sizeof(float);
        
        episodes_.erase(episodes_.begin());
        if (!pImpl->episodeNeurons.empty()) {
            pImpl->episodeNeurons.erase(pImpl->episodeNeurons.begin());
        }
        
        // Update memory usage
        pImpl->totalMemoryAllocated -= oldSize;
        
        // Take snapshot after cleanup
        nlm::MemoryManager::getProfiler().takeSnapshot("NeuralEpisodicMemory cleanup");
    }
    
    // Age all episodes
    for (auto& ep : episodes_) {
        ep.age++;
    }
    
    // Free temporary allocation
    freeEpisodicMemory(mem, episodeSize);
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
    
    return results;
}

void NeuralEpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!episode || !brain_ || !replayEnabled_) return;
    
    // Decode episode to neural activity patterns
    // In a full implementation, this would:
    // 1. Convert sensory state to neural activity
    // 2. Inject activity into appropriate brain regions
    // 3. Use the neural working memory to reinforce patterns
    
    // For now, we just log the replay
    NLM_LOG_INFO("Replaying episode from " + std::to_string(episode->timestamp));
    
    // Update memory monitoring
    pImpl->allocationCount++;
    nlm::MemoryManager::getProfiler().takeSnapshot("NeuralEpisodicMemory replay");
}

void NeuralEpisodicMemory::updateRelevance(SimulationStep episodeId, float relevanceDelta) {
    if (episodeId >= episodes_.size()) return;
    
    // Update episode relevance (in a full implementation)
    // Episodes would store relevance scores that decay over time
    // Higher relevance means more likely to be replayed
    
    NLM_LOG_INFO("Updating relevance for episode " + std::to_string(episodeId) +
                " with delta " + std::to_string(relevanceDelta));
    
    // Update memory monitoring
    pImpl->allocationCount++;
}

const EpisodicMemoryItem* NeuralEpisodicMemory::getEpisode(size_t index) const {
    if (index >= episodes_.size()) return nullptr;
    return &episodes_[index];
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
    // Take memory snapshot before clearing
    nlm::MemoryManager::getProfiler().takeSnapshot("NeuralEpisodicMemory clear");
    
    episodes_.clear();
    pImpl->episodeNeurons.clear();
    
    // Reset memory tracking
    pImpl->totalMemoryAllocated = 0;
    pImpl->allocationCount = 0;
    
    // Update memory monitor
    nlm::MemoryMonitor::updateComponentMemory("NeuralEpisodicMemory", 0);
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
        bool exists = false;
        for (const auto& assoc : associations_) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                std::get<2>(assoc) = strength;
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            associations_.emplace_back(neuronA, neuronB, strength);
        }
        
        NLM_LOG_INFO("Created association between neurons " + 
                    std::to_string(neuronA.index()) + " and " + 
                    std::to_string(neuronB.index()) + 
                    " with strength " + std::to_string(strength));
    }
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (episode.activeNeurons.size() < 2) return;
    
    // Create associations between active neurons in this episode
    // Weaker associations between neurons that co-occur less frequently
    for (size_t i = 0; i < episode.activeNeurons.size(); ++i) {
        for (size_t j = i + 1; j < episode.activeNeurons.size(); ++j) {
            NeuronId neuronA = episode.activeNeurons[i];
            NeuronId neuronB = episode.activeNeurons[j];
            
            float strength = episode.neuronActivations[i] * episode.neuronActivations[j];
            strength *= (1.0f - episode.age / 100.0f);  // Decay with episode age
            
            associate(std::vector<float>(), std::vector<float>(), strength);
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults) const {
    
    std::vector<std::pair<float, std::vector<float>>> results;
    
    for (const auto& assoc : associations_) {
        NeuronId neuronA = std::get<0>(assoc);
        NeuronId neuronB = std::get<1>(assoc);
        float strength = std::get<2>(assoc);
        
        // Find associated patterns
        std::vector<float> patternA;
        std::vector<float> patternB;
        
        auto itA = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                               [neuronA](const auto& p) { return p.first == neuronA; });
        if (itA != patternNeurons_.end()) {
            patternA = itA->second;
        }
        
        auto itB = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                               [neuronB](const auto& p) { return p.first == neuronB; });
        if (itB != patternNeurons_.end()) {
            patternB = itB->second;
        }
        
        // Compute similarity with query
        float similarity = 0.0f;
        if (!patternA.empty() && !queryPattern.empty() && patternA.size() == queryPattern.size()) {
            float dot = 0.0f, normP = 0.0f, normQ = 0.0f;
            for (size_t i = 0; i < patternA.size(); ++i) {
                dot += patternA[i] * queryPattern[i];
                normP += patternA[i] * patternA[i];
                normQ += queryPattern[i] * queryPattern[i];
            }
            if (normP > 0.0001f && normQ > 0.0001f) {
                similarity = dot / (std::sqrt(normP) * std::sqrt(normQ));
            }
        }
        
        if (similarity > 0.3f) {
            results.emplace_back(similarity, patternA);
        }
    }
    
    // Sort by similarity
    std::sort(results.begin(), results.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    std::vector<std::vector<float>> finalResults;
    for (size_t i = 0; i < std::min(maxResults, results.size()); ++i) {
        finalResults.push_back(results[i].second);
    }
    
    return finalResults;
}

float NeuralAssociativeMemory::getAssociationStrength(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB) const {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) return 0.0f;
    
    for (const auto& assoc : associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            return std::get<2>(assoc);
        }
    }
    
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(
    const std::vector<float>& patternA,
    const std::vector<float>& patternB,
    float delta) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID) {
        for (auto& assoc : associations_) {
            if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
                (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
                std::get<2>(assoc) += delta;
                std::get<2>(assoc) = std::max(0.0f, std::min(1.0f, std::get<2>(assoc)));
                break;
            }
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps) const {
    
    std::vector<std::vector<float>> results;
    results.push_back(cuePattern);
    
    for (size_t step = 0; step < steps; ++step) {
        std::vector<std::vector<float>> newPatterns;
        
        for (const auto& pattern : results) {
            // Find patterns associated with this pattern
            for (const auto& assoc : associations_) {
                NeuronId neuronA = std::get<0>(assoc);
                NeuronId neuronB = std::get<1>(assoc);
                
                // Find which pattern matches this neuron
                std::vector<float> patternA;
                std::vector<float> patternB;
                
                auto itA = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                                       [neuronA](const auto& p) { return p.first == neuronA; });
                if (itA != patternNeurons_.end()) patternA = itA->second;
                
                auto itB = std::find_if(patternNeurons_.begin(), patternNeurons_.end(),
                                       [neuronB](const auto& p) { return p.first == neuronB; });
                if (itB != patternNeurons_.end()) patternB = itB->second;
                
                // Spread activation
                if (!patternA.empty() && patternA.size() == pattern.size()) {
                    std::vector<float> spreadPattern(pattern.size(), 0.0f);
                    for (size_t i = 0; i < pattern.size(); ++i) {
                        spreadPattern[i] = pattern[i] * 0.7f + patternA[i] * 0.3f;
                    }
                    newPatterns.push_back(spreadPattern);
                }
                
                if (!patternB.empty() && patternB.size() == pattern.size()) {
                    std::vector<float> spreadPattern(pattern.size(), 0.0f);
                    for (size_t i = 0; i < pattern.size(); ++i) {
                        spreadPattern[i] = pattern[i] * 0.7f + patternB[i] * 0.3f;
                    }
                    newPatterns.push_back(spreadPattern);
                }
            }
        }
        
        // Add new patterns to results
        for (const auto& pattern : newPatterns) {
            bool exists = false;
            for (const auto& existing : results) {
                if (existing == pattern) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                results.push_back(pattern);
            }
        }
    }
    
    return results;
}

void NeuralAssociativeMemory::clear() {
    patternNeurons_.clear();
    associations_.clear();
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

NeuronId NeuralAssociativeMemory::findPatternNeuron(
    const std::vector<float>& pattern) {
    // Find existing pattern neuron or create one
    for (const auto& existing : patternNeurons_) {
        if (existing.second == pattern) {
            return existing.first;
        }
    }
    
    // Create new pattern neuron
    NeuronId neuronId(patternNeurons_.size() + 10000);
    patternNeurons_.emplace_back(neuronId, pattern);
    
    return neuronId;
}

float NeuralAssociativeMemory::computeSimilarity(
    const std::vector<float>& a,
    const std::vector<float>& b) const {
    if (a.empty() || b.empty() || a.size() != b.size()) return 0.0f;
    
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
