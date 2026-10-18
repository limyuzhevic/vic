#include "NeuralEpisodicMemory.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <chrono>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Enhanced episode structure with neural patterns
    struct Episode {
        SimulationStep startTime;
        SimulationStep endTime;
        std::vector<float> pattern;
        std::vector<std::pair<NeuronId, float>> neuralActivation;
        float strength;
        float noveltyScore;
        bool replayed;
        std::vector<SimulationStep> replayTimes;
        
        Episode() : startTime(0), endTime(0), strength(0.0f), noveltyScore(0.0f), replayed(false) {}
    };
    
    std::deque<Episode> episodes;
    
    // Replay buffer for consolidation
    std::deque<Episode> replayBuffer;
    
    // Performance optimizations
    size_t maxEpisodes;
    size_t replayBufferSize;
    float consolidationThreshold;
    float noveltyThreshold;
    bool useReplayConsolidation;
    
    // Statistics
    size_t totalEpisodesStored;
    size_t totalEpisodesReplayed;
    size_t totalConsolidations;
    float averageEpisodeStrength;
    float averageNoveltyScore;
    
    Impl() : brain(nullptr), maxEpisodes(1000), replayBufferSize(50), 
             consolidationThreshold(0.8f), noveltyThreshold(0.5f),
             useReplayConsolidation(true), totalEpisodesStored(0),
             totalEpisodesReplayed(0), totalConsolidations(0),
             averageEpisodeStrength(0.0f), averageNoveltyScore(0.0f) {}
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralEpisodicMemory initialized");
}

void NeuralEpisodicMemory::store(SimulationStep startTime, SimulationStep endTime, 
                                const std::vector<float>& pattern) {
    if (pattern.empty() || !brain_) return;
    
    Episode episode;
    episode.startTime = startTime;
    episode.endTime = endTime;
    episode.pattern = pattern;
    episode.strength = calculateEpisodeStrength(pattern);
    episode.noveltyScore = calculateNoveltyScore(pattern);
    episode.replayed = false;
    
    // Create neural activation pattern
    episode.neuralActivation = createNeuralActivation(pattern);
    
    // Store episode
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        // Remove oldest episode
        pImpl->episodes.pop_front();
    }
    
    pImpl->episodes.push_back(episode);
    pImpl->totalEpisodesStored++;
    
    // Add to replay buffer if consolidation is enabled
    if (pImpl->useReplayConsolidation && episode.noveltyScore > pImpl->noveltyThreshold) {
        pImpl->replayBuffer.push_back(episode);
        if (pImpl->replayBuffer.size() > pImpl->replayBufferSize) {
            pImpl->replayBuffer.pop_front();
        }
    }
}

void NeuralEpisodicMemory::update(float dt) {
    // Update all episodes
    for (auto& episode : pImpl->episodes) {
        episode.strength *= (1.0f - decayRate_ * dt);
    }
    
    // Update replay buffer
    for (auto& episode : pImpl->replayBuffer) {
        episode.strength *= (1.0f - decayRate_ * dt);
    }
    
    // Consolidate episodes if needed
    consolidateEpisodes();
    
    updateStatistics();
}

std::vector<Episode> NeuralEpisodicMemory::getEpisodesForReplay(size_t count) const {
    std::vector<Episode> result;
    
    if (count == 0) return result;
    
    // Prefer recent episodes with higher strength
    std::vector<std::pair<float, size_t>> episodeScores;
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        float score = pImpl->episodes[i].strength * (1000.0f / (pImpl->episodes[i].startTime + 1));
        episodeScores.emplace_back(score, i);
    }
    
    // Sort by score (descending)
    std::sort(episodeScores.begin(), episodeScores.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Select top episodes
    size_t episodesToSelect = std::min(count, pImpl->episodes.size());
    for (size_t i = 0; i < episodesToSelect; ++i) {
        result.push_back(pImpl->episodes[episodeScores[i].second]);
    }
    
    return result;
}

float NeuralEpisodicMemory::getNoveltyLevel() const {
    if (pImpl->episodes.empty()) {
        return 0.0f;
    }
    
    float totalNovelty = 0.0f;
    for (const auto& episode : pImpl->episodes) {
        totalNovelty += episode.noveltyScore;
    }
    
    return totalNovelty / pImpl->episodes.size();
}

std::string NeuralEpisodicMemory::getMemoryStats() const {
    std::ostringstream oss;
    oss << "Neural Episodic Memory Statistics:" << std::endl;
    oss << "  Total episodes stored: " << pImpl->totalEpisodesStored << std::endl;
    oss << "  Current episodes: " << pImpl->episodes.size() << std::endl;
    oss << "  Maximum episodes: " << pImpl->maxEpisodes << std::endl;
    oss << "  Total episodes replayed: " << pImpl->totalEpisodesReplayed << std::endl;
    oss << "  Total consolidations: " << pImpl->totalConsolidations << std::endl;
    oss << "  Replay buffer size: " << pImpl->replayBuffer.size() << std::endl;
    oss << "  Average episode strength: " << pImpl->averageEpisodeStrength << std::endl;
    oss << "  Average novelty score: " << pImpl->averageNoveltyScore << std::endl;
    oss << "  Novelty threshold: " << pImpl->noveltyThreshold << std::endl;
    oss << "  Consolidation threshold: " << pImpl->consolidationThreshold << std::endl;
    
    return oss.str();
}

void NeuralEpisodicMemory::clear() {
    pImpl->episodes.clear();
    pImpl->replayBuffer.clear();
    
    pImpl->totalEpisodesStored = 0;
    pImpl->totalEpisodesReplayed = 0;
    pImpl->totalConsolidations = 0;
    pImpl->averageEpisodeStrength = 0.0f;
    pImpl->averageNoveltyScore = 0.0f;
}

void NeuralEpisodicMemory::consolidateEpisodes() {
    if (!pImpl->useReplayConsolidation || pImpl->replayBuffer.empty()) {
        return;
    }
    
    // Find episodes to consolidate
    for (auto& episode : pImpl->replayBuffer) {
        if (episode.strength >= pImpl->consolidationThreshold) {
            // Consolidate by updating the original episode
            for (auto& originalEpisode : pImpl->episodes) {
                if (originalEpisode.startTime == episode.startTime &&
                    originalEpisode.endTime == episode.endTime) {
                    originalEpisode.strength = std::max(originalEpisode.strength, episode.strength);
                    originalEpisode.neuralActivation = episode.neuralActivation;
                    break;
                }
            }
            
            pImpl->totalConsolidations++;
        }
    }
    
    // Clear replay buffer after consolidation
    pImpl->replayBuffer.clear();
}

float NeuralEpisodicMemory::calculateEpisodeStrength(const std::vector<float>& pattern) const {
    if (pattern.empty()) return 0.0f;
    
    float totalStrength = 0.0f;
    for (float value : pattern) {
        totalStrength += std::abs(value);
    }
    
    return totalStrength / pattern.size();
}

float NeuralEpisodicMemory::calculateNoveltyScore(const std::vector<float>& pattern) const {
    if (pattern.empty()) return 0.0f;
    
    // Calculate novelty based on pattern variance and deviation from mean
    float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
    float variance = 0.0f;
    
    for (float value : pattern) {
        variance += (value - mean) * (value - mean);
    }
    
    variance /= pattern.size();
    
    return std::sqrt(variance);
}

std::vector<std::pair<NeuronId, float>> NeuralEpisodicMemory::createNeuralActivation(const std::vector<float>& pattern) {
    std::vector<std::pair<NeuronId, float>> activations;
    
    if (!brain_ || pattern.empty()) return activations;
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), capacity_);
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        // Create neuron ID based on pattern position
        NeuronId neuronId(i + 1, 0, 0);  // Simplified neuron ID
        float activation = pattern[i] * 5.0f;  // Scale activation
        
        activations.emplace_back(neuronId, activation);
        
        // Inject current to neuron
        brain_->injectCurrent(neuronId, activation);
    }
    
    return activations;
}

void NeuralEpisodicMemory::updateStatistics() {
    if (pImpl->episodes.empty()) {
        pImpl->averageEpisodeStrength = 0.0f;
        pImpl->averageNoveltyScore = 0.0f;
        return;
    }
    
    float totalStrength = 0.0f;
    float totalNovelty = 0.0f;
    
    for (const auto& episode : pImpl->episodes) {
        totalStrength += episode.strength;
        totalNovelty += episode.noveltyScore;
    }
    
    pImpl->averageEpisodeStrength = totalStrength / pImpl->episodes.size();
    pImpl->averageNoveltyScore = totalNovelty / pImpl->episodes.size();
}

} // namespace nlm