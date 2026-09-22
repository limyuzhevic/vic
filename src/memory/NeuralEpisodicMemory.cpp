// NeuralEpisodicMemory Implementation
// Phase 6: Episodic Memory System
// Stores and replays complete experience episodes

#include "NeuralEpisodicMemory.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <queue>
#include <limits>
#include <chrono>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    // Episode storage with full experience snapshots
    std::vector<std::unique_ptr<EpisodicMemoryItem>> episodes;
    
    // Memory consolidation and replay system
    std::queue<const EpisodicMemoryItem*> replayQueue;
    std::vector<const EpisodicMemoryItem*> importantEpisodes;
    
    // Replay scheduling
    static constexpr size_t MAX_REPLAY_QUEUE = 10;
    static constexpr size_t MAX_EPISODES = 10000;
    static constexpr size_t EPISODE_STORAGE_LIMIT = 1000;  // Hard limit
    
    // Consolidation parameters
    static constexpr float CONSOLIDATION_THRESHOLD = 0.8f;
    static constexpr float REPLAY_PRIORITY = 0.9f;
    static constexpr float ACTIVATION_THRESHOLD = 0.3f;
    
    // Sleep/rest cycle parameters
    static constexpr size_t REST_INTERVAL = 100;  // Steps between rest periods
    static constexpr float CONSOLIDATION_DURING_REST = 0.8f;
    static constexpr float REPLAY_DURING_REST = 1.0f;
    
    // Temporal compression and replay
    static constexpr float COMPRESSION_FACTOR = 0.5f;  // Time compression during replay
    static constexpr float EVENT_SAMPLE_RATE = 0.2f;  // Sample rate for replay
    
    // Statistics
    size_t totalEpisodesStored = 0;
    size_t totalEpisodesReplayed = 0;
    size_t totalConsolidationOperations = 0;
    
    // Current state
    size_t currentRestStep = 0;
    bool isResting = false;
    float timeScale = 1.0f;  // 1.0 = normal, <1.0 = replay speed
    
    // Pattern extraction for replay
    struct ReplayPattern {
        Timestamp timestamp;
        std::vector<NeuronId> activeNeurons;
        float activationStrength;
        std::vector<float> sensoryInput;
        std::string actionTaken;
        float rewardReceived;
    };
    
    Impl() : totalEpisodesStored(0), totalEpisodesReplayed(0), 
             totalConsolidationOperations(0), currentRestStep(0), 
             isResting(false), timeScale(1.0f) {
    }
};

NeuralEpisodicMemory::NeuralEpisodicMemory(std::shared_ptr<Brain> brain)
    : pImpl(std::make_unique<Impl>()), brain(brain) {
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

NeuralEpisodicMemory::NeuralEpisodicMemory(NeuralEpisodicMemory&& other) noexcept
    : pImpl(std::move(other.pImpl)), brain(std::move(other.brain)) {
}

NeuralEpisodicMemory& NeuralEpisodicMemory::operator=(NeuralEpisodicMemory&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        brain = std::move(other.brain);
    }
    return *this;
}

void NeuralEpisodicMemory::initialize(std::shared_ptr<Brain> brain) {
    this->brain = brain;
    
    // Initialize with empty state
    pImpl->episodes.clear();
    pImpl->replayQueue = std::queue<const EpisodicMemoryItem*>();
    pImpl->importantEpisodes.clear();
    
    pImpl->totalEpisodesStored = 0;
    pImpl->totalEpisodesReplayed = 0;
    pImpl->totalConsolidationOperations = 0;
    pImpl->currentRestStep = 0;
    pImpl->isResting = false;
    pImpl->timeScale = 1.0f;
    
    NLM_LOG_INFO("NeuralEpisodicMemory initialized");
}

MemoryId NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Validate episode
    if (!validateEpisode(episode)) {
        NLM_LOG_ERROR("Invalid episode - cannot store");
        return MemoryId(0);
    }
    
    // Create unique memory ID
    MemoryId id(static_cast<MemoryId::ValueType>(++pImpl->totalEpisodesStored));
    
    // Store deep copy of episode
    auto episodeCopy = std::make_unique<EpisodicMemoryItem>(episode);
    episodeCopy->episodeId = id;
    episodeCopy->storageTimestamp = pImpl->totalEpisodesStored;
    
    // Check storage limits
    if (pImpl->episodes.size() >= MAX_EPISODES) {
        // Remove oldest episode to make room
        pImpl->episodes.erase(pImpl->episodes.begin());
        pImpl->totalEpisodesStored--;  // Adjust counter
        id = MemoryId(static_cast<MemoryId::ValueType>(pImpl->totalEpisodesStored + 1));
        episodeCopy->episodeId = id;
        episodeCopy->storageTimestamp = pImpl->totalEpisodesStored;
    }
    
    pImpl->episodes.push_back(std::move(episodeCopy));
    
    // Evaluate if episode should be marked as important for replay
    evaluateEpisodeImportance(*pImpl->episodes.back());
    
    // Extract replay patterns
    extractReplayPatterns(*pImpl->episodes.back());
    
    NLM_LOG_INFO("Stored episode " + std::to_string(id.index()) + 
                " with " + std::to_string(episode.activeNeurons.size()) + 
                " active neurons, timestamp " + std::to_string(episode.timestamp) +
                ", reward: " + std::to_string(episode.reward));
    
    return id;
}

bool NeuralEpisodicMemory::validateEpisode(const EpisodicMemoryItem& episode) {
    // Check required fields
    if (episode.episodeId.index() == 0) {
        NLM_LOG_ERROR("Episode missing ID");
        return false;
    }
    
    if (episode.timestamp <= 0) {
        NLM_LOG_ERROR("Episode timestamp invalid");
        return false;
    }
    
    // Validate active neurons (neuron IDs should be valid in brain)
    if (!brain) {
        NLM_LOG_WARNING("Brain not available for neuron validation");
    }
    
    // Validate reward range
    if (episode.reward < -100.0f || episode.reward > 100.0f) {
        NLM_LOG_ERROR("Episode reward out of range: " + std::to_string(episode.reward));
        return false;
    }
    
    return true;
}

const EpisodicMemoryItem* NeuralEpisodicMemory::getEpisode(MemoryId id) const {
    for (const auto& episode : pImpl->episodes) {
        if (episode->episodeId == id) {
            return episode.get();
        }
    }
    return nullptr;  // Episode not found
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(size_t count) const {
    // Get episodes sorted by importance (replay priority)
    std::vector<const EpisodicMemoryItem*> replayCandidates;
    
    for (const auto& episode : pImpl->episodes) {
        bool shouldReplay = false;
        
        // Check if episode is marked as important
        if (std::find(pImpl->importantEpisodes.begin(), pImpl->importantEpisodes.end(), 
                      episode.get()) != pImpl->importantEpisodes.end()) {
            shouldReplay = true;
        }
        
        // Check if episode has high reward
        if (episode->reward > 1.0f) {
            shouldReplay = true;
        }
        
        // Check if episode contains novel events (simplified)
        if (episode->actionTaken == "Explore" || episode->actionTaken == "Discover") {
            shouldReplay = true;
        }
        
        if (shouldReplay) {
            replayCandidates.push_back(episode.get());
        }
    }
    
    // Sort by importance (high reward, novelty first)
    std::sort(replayCandidates.begin(), replayCandidates.end(),
              [this](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
                  float scoreA = getReplayPriority(a);
                  float scoreB = getReplayPriority(b);
                  return scoreA > scoreB;
              });
    
    // Limit to requested count
    if (count > 0 && replayCandidates.size() > count) {
        replayCandidates.resize(count);
    }
    
    return replayCandidates;
}

float NeuralEpisodicMemory::getReplayPriority(const EpisodicMemoryItem* episode) const {
    if (!episode) return 0.0f;
    
    // Calculate priority based on multiple factors
    float priority = 0.0f;
    
    // Reward contribution (0.4 weight)
    priority += episode->reward * 0.4f;
    
    // Recency bonus (0.2 weight) - newer episodes prioritized
    float ageScore = 1.0f - (static_cast<float>(episode->timestamp) / 
                            static_cast<float>(pImpl->totalEpisodesStored + 1));
    priority += ageScore * 0.2f;
    
    // Action type bonus (0.2 weight) - exploratory actions prioritized
    if (episode->actionTaken == "Explore" || episode->actionTaken == "Discover" ||
        episode->actionTaken == "Experiment") {
        priority += 1.0f * 0.2f;
    }
    
    // Novelty detection (simplified - based on activation strength)
    if (episode->neuronActivations.size() > 0) {
        float avgActivation = 0.0f;
        for (float activation : episode->neuronActivations) {
            avgActivation += activation;
        }
        avgActivation /= episode->neuronActivations.size();
        priority += avgActivation * 0.2f;
    }
    
    return priority;
}

void NeuralEpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!episode) {
        return;
    }
    
    NLM_LOG_INFO("Replaying episode " + std::to_string(episode->episodeId.index()) +
                " (reward: " + std::to_string(episode->reward) + ")");
    
    pImpl->totalEpisodesReplayed++;
    
    // Create replay patterns from episode
    std::vector<Impl::ReplayPattern> replayPatterns;
    extractReplayPatterns(*episode, replayPatterns);
    
    // Reconstruct neural state during replay
    reconstructNeuralState(replayPatterns);
    
    // Apply rewards during replay
    if (brain && episode->reward > 0.0f) {
        // Apply positive reinforcement
        brain->getDopamine()->addReward(episode->reward * 0.5f);
    }
    
    // Update curiosity about replayed content
    if (brain) {
        brain->getCuriosity()->updateExploration(episode->noveltyDetected);
    }
    
    // Mark episode as replayed
    const_cast<EpisodicMemoryItem*>(episode)->replayedCount++;
}

void NeuralEpisodicMemory::reconstructNeuralState(const std::vector<Impl::ReplayPattern>& patterns) {
    if (!brain) {
        return;
    }
    
    // Clear current state for replay
    brain->reset();
    
    // Rebuild neural states step by step
    for (const auto& pattern : patterns) {
        // Restore neural activations
        for (NeuronId neuronId : pattern.activeNeurons) {
            auto* neuron = findNeuronById(neuronId);
            if (neuron) {
                // Set neuron to active state based on activation strength
                float potential = neuron->getState().restingPotential + 
                                 pattern.activationStrength * 10.0f;
                neuron->setMembranePotential(potential);
                
                if (pattern.activationStrength > 0.5f) {
                    neuron->setFiringState(FiringState::Active);
                }
            }
        }
        
        // Apply sensory input if available
        if (!pattern.sensoryInput.empty() && brain) {
            // Convert pattern sensory input to actual sensory input
            // This would create a SensoryInput object for the replay
        }
        
        // Allow time for neural dynamics to process
        // In real implementation, this would step the brain for one timestep
    }
}

NeuralWorkingMemory::Impl* NeuralEpisodicMemory::findNeuronById(NeuronId neuronId) {
    // Helper function to find neuron by ID in brain
    if (!brain) return nullptr;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->getId() == neuronId) {
                    return neuron;
                }
            }
        }
    }
    
    return nullptr;
}

void NeuralEpisodicMemory::evaluateEpisodeImportance(const EpisodicMemoryItem& episode) {
    // Mark episodes as important based on various criteria
    bool isImportant = false;
    
    // High reward
    if (episode.reward > 5.0f) {
        isImportant = true;
    }
    
    // Novel actions
    if (episode.actionTaken == "Explore" || episode.actionTaken == "Discover" ||
        episode.actionTaken == "Learn") {
        isImportant = true;
    }
    
    // High neural activation
    float totalActivation = 0.0f;
    for (float activation : episode.neuronActivations) {
        totalActivation += activation;
    }
    if (totalActivation / episode.neuronActivations.size() > 0.7f) {
        isImportant = true;
    }
    
    // Recent episodes
    if (episode.timestamp > pImpl->totalEpisodesStored * 0.8f) {  // Top 20%
        isImportant = true;
    }
    
    if (isImportant) {
        pImpl->importantEpisodes.push_back(&episode);
    }
}

void NeuralEpisodicMemory::extractReplayPatterns(const EpisodicMemoryItem& episode,
                                                  std::vector<Impl::ReplayPattern>& patterns) {
    patterns.clear();
    
    // Convert episode to replay patterns
    Impl::ReplayPattern pattern;
    pattern.timestamp = episode.timestamp;
    pattern.activeNeurons = episode.activeNeurons;
    pattern.rewardReceived = episode.reward;
    pattern.actionTaken = episode.actionTaken;
    
    // Calculate activation strength from neuron activations
    if (episode.neuronActivations.size() > 0) {
        pattern.activationStrength = 0.0f;
        for (float activation : episode.neuronActivations) {
            pattern.activationStrength += activation;
        }
        pattern.activationStrength /= episode.neuronActivations.size();
    }
    
    // Convert sensory input to float vector (simplified)
    pattern.sensoryInput = episode.sensoryInput;
    
    patterns.push_back(pattern);
}

void NeuralEpisodicMemory::extractReplayPatterns(const EpisodicMemoryItem& episode) {
    // Simplified version for single episode
    std::vector<Impl::ReplayPattern> patterns;
    extractReplayPatterns(episode, patterns);
    
    // Add to replay queue
    if (patterns.size() > 0) {
        // In full implementation, would schedule patterns for replay
    }
}

void NeuralEpisodicMemory::consolidate(float consolidationRate) {
    if (consolidationRate <= 0.0f || pImpl->episodes.empty()) {
        return;
    }
    
    pImpl->totalConsolidationOperations++;
    
    // Consolidate important episodes
    for (auto& episode : pImpl->episodes) {
        // Only consolidate episodes above threshold
        float episodeStrength = calculateEpisodeStrength(*episode);
        
        if (episodeStrength >= consolidationRate) {
            // Compress and reinforce episode
            compressEpisode(*episode);
            strengthenEpisodeConnections(*episode);
        }
    }
    
    // Clear old important episodes to prevent memory bloat
    pImpl->importantEpisodes.clear();
    
    NLM_LOG_INFO("Consolidated " + std::to_string(pImpl->episodes.size()) + 
                " episodes with rate " + std::to_string(consolidationRate));
}

float NeuralEpisodicMemory::calculateEpisodeStrength(const EpisodicMemoryItem& episode) const {
    float strength = 0.0f;
    
    // Reward contribution
    strength += episode.reward * 0.3f;
    
    // Activation strength
    if (episode.neuronActivations.size() > 0) {
        float avgActivation = 0.0f;
        for (float activation : episode.neuronActivations) {
            avgActivation += activation;
        }
        avgActivation /= episode.neuronActivations.size();
        strength += avgActivation * 0.3f;
    }
    
    // Recency bonus
    float recencyScore = 1.0f - (static_cast<float>(episode.timestamp) / 
                                 static_cast<float>(pImpl->totalEpisodesStored + 1));
    strength += recencyScore * 0.2f;
    
    // Action diversity (0.2 weight)
    strength += std::min(1.0f, static_cast<float>(episode.actionTaken.size()) / 10.0f) * 0.2f;
    
    return std::min(1.0f, strength);
}

void NeuralEpisodicMemory::compressEpisode(EpisodicMemoryItem& episode) {
    // Temporal compression - remove redundant time steps
    // In full implementation, would analyze and compress timing
    
    // Pattern compression - reduce neuron precision for storage efficiency
    // Simplified: just ensure array sizes are reasonable
    if (episode.activeNeurons.size() > 1000) {
        // Downsample to maintain important neurons
        size_t targetSize = 1000;
        size_t originalSize = episode.activeNeurons.size();
        
        std::vector<NeuronId> compressed;
        compressed.reserve(targetSize);
        
        for (size_t i = 0; i < targetSize; ++i) {
            size_t sourceIdx = (i * originalSize) / targetSize;
            if (sourceIdx < episode.activeNeurons.size()) {
                compressed.push_back(episode.activeNeurons[sourceIdx]);
            }
        }
        
        episode.activeNeurons = compressed;
    }
    
    if (episode.neuronActivations.size() > 1000) {
        size_t targetSize = 1000;
        size_t originalSize = episode.neuronActivations.size();
        
        std::vector<float> compressed;
        compressed.reserve(targetSize);
        
        for (size_t i = 0; i < targetSize; ++i) {
            size_t sourceIdx = (i * originalSize) / targetSize;
            if (sourceIdx < episode.neuronActivations.size()) {
                compressed.push_back(episode.neuronActivations[sourceIdx]);
            }
        }
        
        episode.neuronActivations = compressed;
    }
}

void NeuralEpisodicMemory::strengthenEpisodeConnections(const EpisodicMemoryItem& episode) {
    // Strengthen associations between neurons that co-activated in this episode
    // In full implementation, would use spike-timing dependent plasticity
    // Simplified: log for future integration
    
    NLM_LOG_INFO("Strengthening connections from episode " + 
                std::to_string(episode.episodeId.index()));
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(size_t count) const {
    return getEpisodesForReplay(count);
}

void NeuralEpisodicMemory::update(float timestep) {
    // Update episodic memory system
    
    // Update rest/rest cycle
    pImpl->currentRestStep++;
    if (pImpl->currentRestStep >= REST_INTERVAL) {
        // Enter rest period for consolidation and replay
        enterRestPeriod();
        pImpl->currentRestStep = 0;
        pImpl->isResting = true;
    }
    
    if (!pImpl->isResting) {
        // Normal operation mode
        timeScale = 1.0f;
        
        // Check for episodes that need consolidation
        if (pImpl->totalEpisodesStored % 100 == 0) {  // Every 100 episodes
            consolidate(0.3f);
        }
    }
}

void NeuralEpisodicMemory::enterRestPeriod() {
    NLM_LOG_INFO("Entering rest period for memory consolidation and replay");
    
    // Speed up replay during rest
    timeScale = REPLAY_DURING_REST;
    
    // Enhanced consolidation during rest
    consolidate(CONSOLIDATION_DURING_REST);
    
    // Prioritize replay of important episodes
    auto importantEpisodes = getEpisodesForReplay(pImpl->importantEpisodes.size());
    for (const auto* episode : importantEpisodes) {
        if (episode) {
            replayEpisode(episode);
        }
    }
    
    // Reset rest state
    pImpl->isResting = false;
    
    NLM_LOG_INFO("Rest period complete");
}

void NeuralEpisodicMemory::enableForReplay(bool enable) {
    // Enable or disable replay functionality
    // In full implementation, would control replay scheduling
    NLM_LOG_INFO("Episodic memory replay " + std::string(enable ? "enabled" : "disabled"));
}

void NeuralEpisodicMemory::setMaxEpisodes(size_t maxEpisodes) {
    // Set maximum number of episodes to store
    pImpl->MAX_EPISODES = std::max(maxEpisodes, size_t(100));  // Minimum 100
    
    // Trim episodes if over limit
    while (pImpl->episodes.size() > pImpl->MAX_EPISODES) {
        pImpl->episodes.erase(pImpl->episodes.begin());
        if (pImpl->totalEpisodesStored > 0) {
            pImpl->totalEpisodesStored--;
        }
    }
    
    NLM_LOG_INFO("Max episodes set to " + std::to_string(pImpl->MAX_EPISODES));
}

std::string NeuralEpisodicMemory::getStats() const {
    std::ostringstream oss;
    oss << "=== NeuralEpisodicMemory Statistics ===\n";
    oss << "Total episodes stored: " << pImpl->episodes.size() << " (max: " << pImpl->MAX_EPISODES << ")\n";
    oss << "Total episodes replayed: " << pImpl->totalEpisodesReplayed << "\n";
    oss << "Consolidation operations: " << pImpl->totalConsolidationOperations << "\n";
    oss << "Important episodes: " << pImpl->importantEpisodes.size() << "\n";
    oss << "Current rest step: " << pImpl->currentRestStep << "/" << REST_INTERVAL << "\n";
    oss << "Is resting: " << (pImpl->isResting ? "yes" : "no") << "\n";
    oss << "Time scale: " << pImpl->timeScale << "x\n";
    
    if (pImpl->episodes.size() > 0) {
        oss << "Average episode reward: " << calculateAverageReward() << "\n";
        oss << "Average episode neurons: " << calculateAverageActiveNeurons() << "\n";
    }
    
    return oss.str();
}

float NeuralEpisodicMemory::calculateAverageReward() const {
    if (pImpl->episodes.empty()) return 0.0f;
    
    float totalReward = 0.0f;
    for (const auto& episode : pImpl->episodes) {
        totalReward += episode->reward;
    }
    
    return totalReward / pImpl->episodes.size();
}

float NeuralEpisodicMemory::calculateAverageActiveNeurons() const {
    if (pImpl->episodes.empty()) return 0.0f;
    
    float totalNeurons = 0.0f;
    for (const auto& episode : pImpl->episodes) {
        totalNeurons += episode->activeNeurons.size();
    }
    
    return totalNeurons / pImpl->episodes.size();
}

NeuralEpisodicMemory& NeuralEpisodicMemory::clear() {
    pImpl->episodes.clear();
    pImpl->replayQueue = std::queue<const EpisodicMemoryItem*>();
    pImpl->importantEpisodes.clear();
    
    pImpl->totalEpisodesStored = 0;
    pImpl->totalEpisodesReplayed = 0;
    pImpl->totalConsolidationOperations = 0;
    pImpl->currentRestStep = 0;
    pImpl->isResting = false;
    pImpl->timeScale = 1.0f;
    
    NLM_LOG_INFO("NeuralEpisodicMemory cleared");
    
    return *this;
}

} // namespace nlm
