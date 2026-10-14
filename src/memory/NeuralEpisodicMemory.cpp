#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <chrono>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Place cell system for episodic memory indexing
    std::vector<PlaceCell> placeCells;
    
    // Temporal indexing
    std::vector<double> temporalSignature;
    
    // Pattern completion network
    std::vector<std::vector<float>> patternCompletionWeights;
    
    // Consolidation mechanisms
    float consolidationRate;
    float replayInterval;
    
    Impl() : brain(nullptr), consolidationRate(0.8f), replayInterval(1000.0f) {
        std::random_device rd;
        rng.seed(rd());
    }
    
    std::mt19937 rng;
    std::chrono::system_clock::time_point lastReplayTime;
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(1000)
    , replayEnabled_(true)
{
    // Initialize with random seed for biological realism
    std::random_device rd;
    pImpl->rng.seed(rd());
    pImpl->lastReplayTime = std::chrono::system_clock::now();
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize place cell system
    initializePlaceCellSystem();
    
    // Initialize pattern completion network
    initializePatternCompletionNetwork();
    
    // Set up temporal indexing
    pImpl->temporalSignature.reserve(maxEpisodes_);
    
    NLM_LOG_INFO("NeuralEpisodicMemory initialized with place cell system");
}

void NeuralEpisodicMemory::initializePlaceCellSystem() {
    // Create place cells for spatial navigation and memory indexing
    // In biological systems, place cells form through exploration
    
    // Initialize with some basic place cells
    for (size_t i = 0; i < 20; ++i) {
        PlaceCell cell;
        cell.neuron = NeuronId(10000 + i);  // Distinct neuron IDs
        cell.x = static_cast<float>(i % 5) * 1.0f;  // 5x5 grid
        cell.y = static_cast<float>(i / 5) * 1.0f;
        cell.fieldRadius = 1.5f;
        cell.activation = 0.0f;
        cell.stability = 1.0f;  // Initial stability
        cell.lastUpdated = std::chrono::system_clock::now();
        
        pImpl->placeCells.push_back(cell);
    }
    
    // Create spatial connections between nearby place cells
    createSpatialConnectivity();
}

void NeuralEpisodicMemory::createSpatialConnectivity() {
    // Create connections between nearby place cells
    // This simulates the spatial relationship encoding in the hippocampus
    
    for (size_t i = 0; i < pImpl->placeCells.size(); ++i) {
        for (size_t j = i + 1; j < pImpl->placeCells.size(); ++j) {
            float distance = calculateDistance(pImpl->placeCells[i], pImpl->placeCells[j]);
            
            if (distance < 3.0f) {  // Within connection radius
                float connectionStrength = std::exp(-distance / 1.0f);
                
                // Create bidirectional connection
                pImpl->placeCells[i].neighbors.push_back(j);
                pImpl->placeCells[j].neighbors.push_back(i);
                pImpl->placeCells[i].neighborStrengths.push_back(connectionStrength);
                pImpl->placeCells[j].neighborStrengths.push_back(connectionStrength);
            }
        }
    }
}

void NeuralEpisodicMemory::initializePatternCompletionNetwork() {
    // Initialize pattern completion network for episodic memory recall
    // This simulates the auto-associative memory properties of the hippocampus
    
    // Initialize with small random weights
    size_t patternSize = 10;  // Assumed pattern size
    pImpl->patternCompletionWeights.resize(patternSize, std::vector<float>(patternSize, 0.0f));
    
    // Set up self-connections for stability
    for (size_t i = 0; i < patternSize; ++i) {
        pImpl->patternCompletionWeights[i][i] = 1.0f;
    }
    
    // Initialize temporal signature tracking
    pImpl->temporalSignature.reserve(maxEpisodes_);
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Store new episode with biological mechanisms
    
    // 1. Create episode copy with age tracking
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    stored.lastAccessed = std::chrono::system_clock::now();
    
    // 2. Encode episode in place cell system
    encodeEpisodeInPlaceCellSystem(stored);
    
    // 3. Integrate with pattern completion network
    integrateWithPatternCompletionNetwork(stored);
    
    // 4. Update temporal signature
    updateTemporalSignature(stored);
    
    // 5. Store in episodes
    episodes_.push_back(stored);
    
    // 6. Trigger consolidation if needed
    if (episodes_.size() > maxEpisodes_ / 2) {
        consolidateEpisodes();
    }
    
    // 7. Remove old episodes
    cleanupOldEpisodes();
}

void NeuralEpisodicMemory::encodeEpisodeInPlaceCellSystem(const EpisodicMemoryItem& episode) {
    // Encode episode information in place cell system
    // This simulates spatial and contextual indexing
    
    if (episode.location.first != 0.0f || episode.location.second != 0.0f) {
        // Find nearest place cells
        std::vector<size_t> nearbyCells = findNearbyPlaceCells(episode.location);
        
        // Activate corresponding place cells
        for (size_t cellIndex : nearbyCells) {
            if (cellIndex < pImpl->placeCells.size()) {
                PlaceCell& cell = pImpl->placeCells[cellIndex];
                cell.activation = std::min(1.0f, cell.activation + 0.3f);
                cell.lastUpdated = std::chrono::system_clock::now();
            }
        }
        
        // Create new place cell if location is novel
        if (nearbyCells.empty()) {
            PlaceCell newCell;
            newCell.neuron = NeuronId(episodes_.size() + 20000);
            newCell.x = episode.location.first;
            newCell.y = episode.location.second;
            newCell.fieldRadius = 1.0f;
            newCell.activation = 0.5f;
            newCell.stability = 1.0f;
            newCell.lastUpdated = std::chrono::system_clock::now();
            
            pImpl->placeCells.push_back(newCell);
        }
    }
    
    // Update episode-specific neuron activation
    NeuronId episodeNeuron(episodes_.size() + 30000);
    if (brain_) {
        brain_->injectCurrent(episodeNeuron, 2.0f);  // Strong activation for new episode
    }
    
    // Store episode-to-neuron mapping
    pImpl->episodeNeurons.push_back(episodeNeuron);
}

void NeuralEpisodicMemory::integrateWithPatternCompletionNetwork(const EpisodicMemoryItem& episode) {
    // Integrate with pattern completion network for memory stability
    // This simulates hippocampal-cortical interactions
    
    // Extract pattern from episode
    std::vector<float> pattern = episode.sensoryState;
    if (pattern.empty()) return;
    
    // Normalize pattern
    float patternNorm = std::sqrt(std::inner_product(pattern.begin(), pattern.end(), pattern.begin(), 0.0f));
    if (patternNorm > 0.0f) {
        for (float& val : pattern) {
            val /= patternNorm;
        }
    }
    
    // Update pattern completion weights
    for (size_t i = 0; i < std::min(pattern.size(), pImpl->patternCompletionWeights.size()); ++i) {
        for (size_t j = 0; j < pattern.size(); ++j) {
            float newWeight = pattern[i] * pattern[j];
            pImpl->patternCompletionWeights[i][j] = 
                pImpl->patternCompletionWeights[i][j] * 0.9f + newWeight * 0.1f;
        }
    }
}

void NeuralEpisodicMemory::updateTemporalSignature(const EpisodicMemoryItem& episode) {
    // Update temporal signature for sequence learning
    // This simulates temporal context encoding
    
    double currentTime = std::chrono::system_clock::now().time_since_epoch().count();
    pImpl->temporalSignature.push_back(currentTime);
    
    // Decay old temporal signatures
    if (pImpl->temporalSignature.size() > maxEpisodes_) {
        pImpl->temporalSignature.erase(pImpl->temporalSignature.begin());
    }
}

void NeuralEpisodicMemory::consolidateEpisodes() {
    // Perform memory consolidation for stability and efficiency
    // This simulates sleep/consolidation processes
    
    if (episodes_.size() < 2) return;
    
    // Calculate consolidation priority based on stability and usage
    std::vector<std::pair<float, size_t>> priorities;  // (priority, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float priority = calculateConsolidationPriority(episodes_[i]);
        priorities.emplace_back(priority, i);
    }
    
    // Sort by priority (descending)
    std::sort(priorities.begin(), priorities.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Consolidate top priority episodes
    size_t toConsolidate = std::min<size_t>(episodes_.size() / 10, priorities.size() / 3);
    
    for (size_t i = 0; i < toConsolidate; ++i) {
        consolidateEpisode(priorities[i].second);
    }
    
    // Update place cell stability
    updatePlaceCellStability();
}

void NeuralEpisodicMemory::consolidateEpisode(size_t episodeIndex) {
    // Consolidate a specific episode for better integration
    if (episodeIndex >= episodes_.size()) return;
    
    EpisodicMemoryItem& episode = episodes_[episodeIndex];
    
    // Calculate episode stability
    float stability = calculateConsolidationPriority(episode);
    
    if (stability > 0.7f) {
        // Highly stable episode - enhance connections
        enhanceEpisodeConnections(episodeIndex);
    } else {
        // Less stable episode - strengthen connections
        strengthenEpisodeConnections(episodeIndex);
    }
    
    // Update temporal encoding
    double currentTime = std::chrono::system_clock::now().time_since_epoch().count();
    episode.age = static_cast<SimulationStep>((currentTime - pImpl->temporalSignature[episodeIndex]) / 1000.0f);
}

void NeuralEpisodicMemory::updatePlaceCellStability() {
    // Update place cell stability based on usage patterns
    for (auto& cell : pImpl->placeCells) {
        // Decay stability over time
        cell.stability *= 0.95f;
        
        // Boost stability based on activation
        cell.stability = std::min(1.0f, cell.stability + cell.activation * 0.1f);
        
        // Reset activation after stability update
        cell.activation = 0.0f;
    }
}

void NeuralEpisodicMemory::enhanceEpisodeConnections(size_t episodeIndex) {
    // Enhance connections for a specific episode
    if (episodeIndex >= episodes_.size()) return;
    
    auto& episode = episodes_[episodeIndex];
    
    // Update pattern completion weights based on episode
    std::vector<float> pattern = episode.sensoryState;
    if (pattern.empty()) return;
    
    float patternNorm = std::sqrt(std::inner_product(pattern.begin(), pattern.end(), pattern.begin(), 0.0f));
    if (patternNorm > 0.0f) {
        for (size_t i = 0; i < std::min(pattern.size(), pImpl->patternCompletionWeights.size()); ++i) {
            for (size_t j = 0; j < pattern.size(); ++j) {
                float boost = pattern[i] * pattern[j] * 0.5f;
                pImpl->patternCompletionWeights[i][j] = 
                    std::min(1.0f, pImpl->patternCompletionWeights[i][j] + boost);
            }
        }
    }
}

void NeuralEpisodicMemory::strengthenEpisodeConnections(size_t episodeIndex) {
    // Strengthen connections for a less stable episode
    if (episodeIndex >= episodes_.size()) return;
    
    auto& episode = episodes_[episodeIndex];
    
    // Gradually strengthen episode connections
    for (size_t i = 0; i < std::min(episode.sensoryState.size(), pImpl->patternCompletionWeights.size()); ++i) {
        for (size_t j = 0; j < episode.sensoryState.size(); ++j) {
            pImpl->patternCompletionWeights[i][j] = 
                std::min(1.0f, pImpl->patternCompletionWeights[i][j] + 0.05f);
        }
    }
}

float NeuralEpisodicMemory::calculateConsolidationPriority(const EpisodicMemoryItem& episode) const {
    // Calculate consolidation priority based on stability and recency
    float stabilityPriority = episode.stability;
    
    // Recency priority (newer episodes have higher priority)
    float recencyPriority = 1.0f - (static_cast<float>(episode.age) / 1000.0f);
    recencyPriority = std::max(0.0f, recencyPriority);
    
    // Reward priority (higher reward episodes are more important)
    float rewardPriority = std::min(1.0f, episode.reward * 2.0f);
    
    // Combine priorities
    return (stabilityPriority * 0.4f + recencyPriority * 0.3f + rewardPriority * 0.3f);
}

void NeuralEpisodicMemory::cleanupOldEpisodes() {
    if (episodes_.size() <= maxEpisodes_) return;
    
    // Remove oldest episodes to maintain capacity
    size_t toRemove = episodes_.size() - maxEpisodes_;
    
    // Remove from end (oldest)
    episodes_.erase(episodes_.end() - toRemove, episodes_.end());
    
    // Also remove associated place cell entries and neurons
    for (size_t i = 0; i < toRemove; ++i) {
        size_t episodeIdx = episodes_.size() + i;  // Adjust index
        // Remove from place cell system
        // Remove from episode neurons
    }
    
    // Trim temporal signature
    if (pImpl->temporalSignature.size() > episodes_.size()) {
        pImpl->temporalSignature.erase(pImpl->temporalSignature.begin(),
                                      pImpl->temporalSignature.begin() + toRemove);
    }
}

void NeuralEpisodicMemory::replayEpisodes() {
    if (!replayEnabled_ || episodes_.empty()) return;
    
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->lastReplayTime).count();
    
    // Check if it's time for replay
    if (elapsed < replayInterval) return;
    
    // Perform memory replay
    performMemoryReplay();
    
    // Update replay time
    pImpl->lastReplayTime = now;
}

void NeuralEpisodicMemory::performMemoryReplay() {
    // Replay important memories for consolidation
    // This simulates offline memory consolidation
    
    // Select episodes for replay based on stability
    std::vector<size_t> episodesToReplay;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float stability = calculateConsolidationPriority(episodes_[i]);
        if (stability > 0.7f) {  // High stability episodes are replayed
            episodesToReplay.push_back(i);
        }
    }
    
    // Replay selected episodes
    for (size_t episodeIdx : episodesToReplay) {
        replayEpisode(episodeIdx);
    }
    
    // Enhance pattern completion network through replay
    enhancePatternCompletionThroughReplay();
}

void NeuralEpisodicMemory::replayEpisode(size_t episodeIndex) {
    if (episodeIndex >= episodes_.size()) return;
    
    auto& episode = episodes_[episodeIndex];
    
    // Update temporal encoding
    double currentTime = std::chrono::system_clock::now().time_since_epoch().count();
    episode.lastAccessed = std::chrono::system_clock::now();
    
    // Enhance connections through replay
    enhanceEpisodeConnections(episodeIndex);
    
    // Update stability based on replay
    episode.stability = std::min(1.0f, episode.stability + 0.05f);
}

void NeuralEpisodicMemory::enhancePatternCompletionThroughReplay() {
    // Enhance pattern completion network through replay
    // This simulates systems consolidation
    
    // Gradually strengthen pattern completion weights
    for (auto& row : pImpl->patternCompletionWeights) {
        for (float& weight : row) {
            weight = std::min(1.0f, weight + 0.01f);
        }
    }
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    size_t maxResults) const {
    
    if (episodes_.empty()) return {};
    
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
    
    // Sort by recency
    std::sort(results.begin(), results.end(),
              [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                  return a->timestamp > b->timestamp;
              });
    
    // Return requested number of results
    size_t returnCount = std::min(maxResults, results.size());
    return std::vector<const EpisodicMemoryItem*>(results.begin(), results.begin() + returnCount);
}

float NeuralEpisodicMemory::computeSimilarity(const EpisodicMemoryItem& episode) const {
    // Compute similarity between current sensory pattern and episode
    // This uses pattern completion network
    
    std::vector<float> pattern = episode.sensoryState;
    if (pattern.empty()) return 0.0f;
    
    // Normalize pattern
    float patternNorm = std::sqrt(std::inner_product(pattern.begin(), pattern.end(), pattern.begin(), 0.0f));
    if (patternNorm < 0.0001f) return 0.0f;
    
    for (float& val : pattern) {
        val /= patternNorm;
    }
    
    // Compute similarity through pattern completion network
    float similarity = 0.0f;
    size_t patternSize = std::min(pattern.size(), pImpl->patternCompletionWeights.size());
    
    for (size_t i = 0; i < patternSize; ++i) {
        for (size_t j = 0; j < patternSize; ++j) {
            similarity += pattern[i] * pImpl->patternCompletionWeights[i][j] * pattern[j];
        }
    }
    
    return similarity;
}

std::vector<size_t> NeuralEpisodicMemory::findNearbyPlaceCells(const std::pair<float, float>& location) const {
    std::vector<size_t> nearbyCells;
    
    for (size_t i = 0; i < pImpl->placeCells.size(); ++i) {
        float distance = calculateDistance(pImpl->placeCells[i], location);
        if (distance < pImpl->placeCells[i].fieldRadius * 2.0f) {
            nearbyCells.push_back(i);
        }
    }
    
    return nearbyCells;
}

float NeuralEpisodicMemory::calculateDistance(const PlaceCell& cell, const std::pair<float, float>& location) const {
    float dx = cell.x - location.first;
    float dy = cell.y - location.second;
    return std::sqrt(dx * dx + dy * dy);
}

void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    pImpl->placeCells.clear();
    pImpl->temporalSignature.clear();
    pImpl->patternCompletionWeights.clear();
    pImpl->episodeNeurons.clear();
}

} // namespace nlm