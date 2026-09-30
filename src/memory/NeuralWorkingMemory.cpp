#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , episodicMemory_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
    , noveltyDecayRate_(0.05f)
    , noveltyLevel_(0.0f)
    , rehearsalStrength_(1.0f)
    , episodicSupportLevel_(0.0f)
    , rehearsalPaused_(false)
    , noveltyBoostEnabled_(true)
    , patternCompletionEnabled_(true)
    , patternCompletionThreshold_(0.3f)
    , lastUpdateStep_(0)
    , totalPatternCompletions_(0)
{
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralWorkingMemory initialized (basic)");
}

void NeuralWorkingMemory::initializeWithEpisodicMemory(
    Brain* brain, NeuralEpisodicMemory* episodicMem) {
    initialize(brain);
    pImpl->episodicMem = episodicMem;
    episodicMemory_ = episodicMem;
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with episodic memory integration");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    storeWithEpisodicSupport(pattern, nullptr, strength);
}

void NeuralWorkingMemory::storeWithEpisodicSupport(
    const std::vector<float>& pattern,
    const EnhancedEpisodicMemoryItem* episode,
    float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Apply episodic support if available
        if (episode && episode->relevanceScore > 0.0f) {
            activation *= (1.0f + episode->relevanceScore * 0.5f);
        }
        
        // Set neuron activation
        if (auto* n = brain_->getRegion(neuron.getId() / 1000)->getAllNeurons()) {
            for (auto* nn : *n) {
                if (nn->getId() == neuron) {
                    nn->injectCurrent(activation * 5.0f);
                    break;
                }
            }
        }
        
        // Update stored activation
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation;
            if (episode) {
                pImpl->episodicSupportMap[i] = episode->relevanceScore;
            }
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            rehearsalTimestamps_.push_back(0);
            rehearsalStrengths_.push_back(1.0f);
            memoryNeurons_.push_back(neuron);
            
            if (episode) {
                pImpl->episodicSupportMap[memoryActivations_.size() - 1] = episode->relevanceScore;
            }
        }
    }
    
    // Create maintenance connections if needed
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.5f);
    }
    
    // Schedule rehearsal for this new memory
    if (episode) {
        scheduleRehearsal(memoryActivations_.size() - 1, EpisodeSupportType::ActiveRehearsal);
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    storeToNeuronWithContext(neuron, activation, nullptr);
}

void NeuralWorkingMemory::storeToNeuronWithContext(
    NeuronId neuron, float activation,
    const EnhancedEpisodicMemoryItem* episode) {
    
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
        rehearsalTimestamps_[idx] = 0;
        rehearsalStrengths_[idx] = 1.0f;
        
        if (episode) {
            pImpl->episodicSupportMap[idx] = episode->relevanceScore;
            scheduleRehearsal(idx, EpisodeSupportType::ActiveRehearsal);
        }
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        rehearsalTimestamps_.push_back(0);
        rehearsalStrengths_.push_back(1.0f);
        
        if (episode) {
            pImpl->episodicSupportMap[memoryActivations_.size() - 1] = episode->relevanceScore;
            scheduleRehearsal(memoryActivations_.size() - 1, EpisodeSupportType::ActiveRehearsal);
        }
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    return retrieveWithEpisodicSupport();
}

std::vector<float> NeuralWorkingMemory::retrieveWithEpisodicSupport() const {
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        float activation = memoryActivations_[i];
        
        // Apply episodic support
        auto it = pImpl->episodicSupportMap.find(i);
        if (it != pImpl->episodicSupportMap.end()) {
            activation *= (1.0f + it->second * 0.5f);
        }
        
        result.push_back(activation);
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    return containsWithEpisodicSupport(neuron);
}

bool NeuralWorkingMemory::containsWithEpisodicSupport(NeuronId neuron) const {
    return std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    return getNeuronActivationWithEpisodicSupport(neuron);
}

float NeuralWorkingMemory::getNeuronActivationWithEpisodicSupport(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        float activation = memoryActivations_[idx];
        
        // Apply episodic support
        auto supportIt = pImpl->episodicSupportMap.find(idx);
        if (supportIt != pImpl->episodicSupportMap.end()) {
            activation *= (1.0f + supportIt->second * 0.5f);
        }
        
        return activation;
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    updateWithEpisodicRehearsal(dt);
}

void NeuralWorkingMemory::updateWithEpisodicRehearsal(TimestepDuration dt) {
    if (!brain_) return;
    
    lastUpdateStep_++;
    
    // Update maintenance - reinforce active memory neurons
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Inject maintenance current
            brain_->injectCurrent(neuron, activation * 2.0f);
            
            // Age the trace
            memoryTimestamps_[i]++;
            rehearsalTimestamps_[i]++;
            
            // Apply rehearsal enhancement if not paused
            if (!rehearsalPaused_ && rehearsalTimestamps_[i] < 100) {
                // Schedule rehearsal based on type
                size_t rehearsalType = rehearsalTimestamps_[i] % 3;
                if (rehearsalType == 0) {
                    strengthenMemory(1.05f);  // Mild strengthening
                }
            }
            
            // Check if trace is too old
            if (memoryTimestamps_[i] > 1000) {
                activation *= (1.0f - decayRate_);
            }
            
            memoryActivations_[i] = activation;
        }
    }
    
    // Update novelty signals based on episodic memory
    if (episodicMemory_ && noveltyBoostEnabled_) {
        detectNoveltyFromEpisodic(episodicMemory_);
    }
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competition to select winners
    runCompetition();
    
    // Update temporal sequence
    maintainTemporalSequence();
    
    // Apply pattern completion
    if (patternCompletionEnabled_) {
        completePatternsFromEpisodic();
    }
    
    // Update rehearsal strength over time
    updateRehearsalStrength();
}

void NeuralWorkingMemory::rehearse(EpisodeSupportType type) {
    // This is a simplified rehearsal mechanism
    // In a full implementation, this would involve replaying patterns
    // from episodic memory to strengthen connections
    
    // Mark for rehearsal scheduling
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        if (memoryActivations_[i] > 0.3f) {
            scheduleRehearsal(i, type);
        }
    }
}

void NeuralWorkingMemory::rehearseWithPatternCompletion() {
    // Perform pattern completion using episodic memory traces
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        if (memoryActivations_[i] > 0.2f) {
            // Simulate pattern completion
            float completionScore = memoryActivations_[i] * 0.8f;
            patternCompletionScores_.push_back(completionScore);
            totalPatternCompletions_++;
        }
    }
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
    
    for (auto& strength : rehearsalStrengths_) {
        strength = std::min(1.0f, strength * factor);
    }
}

void NeuralWorkingMemory::enhanceRehearsal(float factor) {
    for (auto& strength : rehearsalStrengths_) {
        strength = std::min(1.0f, strength * factor);
    }
    
    rehearsalStrength_ = std::min(1.0f, rehearsalStrength_ * factor);
}

void NeuralWorkingMemory::detectNoveltyFromEpisodic(
    NeuralEpisodicMemory* episodicMem) {
    
    if (!episodicMem) return;
    
    // Get recent episodes for novelty calculation
    auto recentEpisodes = episodicMem->getRecentEpisodes(5);
    
    // Compute novelty based on prediction errors and unexpected outcomes
    float totalNovelty = 0.0f;
    int episodeCount = 0;
    
    for (const auto* episode : recentEpisodes) {
        if (!episode) continue;
        
        // Novelty based on prediction error
        float predictionError = std::abs(episode->modalities.novelty);
        totalNovelty += predictionError;
        episodeCount++;
        
        // Novelty based on emotional valence
        float emotionNovelty = std::abs(episode->modalities.emotion) * 0.5f;
        totalNovelty += emotionNovelty;
        
        // Novelty based on reward prediction error
        float rewardNovelty = std::abs(episode->modalities.reward) * 0.3f;
        totalNovelty += rewardNovelty;
    }
    
    if (episodeCount > 0) {
        noveltyLevel_ = totalNovelty / episodeCount;
        
        // Apply novelty boost to working memory
        if (noveltyLevel_ > 0.3f) {
            applyNoveltyBoost(noveltyLevel_);
        }
    }
}

void NeuralWorkingMemory::applyNoveltyBoost(float noveltyLevel) {
    // Enhance working memory based on novelty detection
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] > 0.1f) {
            // Apply boost based on novelty
            memoryActivations_[i] *= (1.0f + noveltyLevel * 0.3f);
        }
    }
    
    // Update novelty signals
    for (float& signal : noveltySignals_) {
        signal = std::min(1.0f, signal + noveltyLevel * 0.1f);
    }
}

void NeuralWorkingMemory::updateNoveltySignals() {
    // Decay novelty over time
    for (float& signal : noveltySignals_) {
        signal *= (1.0f - noveltyDecayRate_);
    }
    
    // Update overall novelty level
    if (!noveltySignals_.empty()) {
        float totalNovelty = std::accumulate(noveltySignals_.begin(), 
                                           noveltySignals_.end(), 0.0f);
        noveltyLevel_ = totalNovelty / noveltySignals_.size();
    }
}

void NeuralWorkingMemory::predictFromEpisodic(
    NeuralEpisodicMemory* episodicMem) {
    
    if (!episodicMem || !brain_) return;
    
    // Use episodic memory to predict future patterns
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        // Find similar episodes
        std::vector<float> pattern(1, memoryActivations_[i]);
        auto similar = episodicMem->retrieveSimilar(pattern, 3);
        
        if (!similar.empty()) {
            // Update prediction based on similar episodes
            float predictionStrength = 0.0f;
            for (const auto* episode : similar) {
                if (episode) {
                    predictionStrength += episode->relevanceScore;
                }
            }
            
            predictionStrength /= similar.size();
            memoryActivations_[i] *= (1.0f + predictionStrength * 0.2f);
        }
    }
}

void NeuralWorkingMemory::planBasedOnPastExperience() {
    // Plan based on temporal sequence and pattern completion
    buildTemporalSequence();
    
    // Identify patterns in temporal sequence
    if (temporalSequence_.size() > 3) {
        // Simple pattern recognition: check for recurring sequences
        std::map<size_t, int> sequenceCounts;
        for (size_t i = 0; i < temporalSequence_.size() - 2; ++i) {
            size_t seq = temporalSequence_[i] * 1000 + 
                        temporalSequence_[i+1] * 100 + 
                        temporalSequence_[i+2];
            sequenceCounts[seq]++;
        }
        
        // Strengthen frequently occurring sequences
        for (const auto& pair : sequenceCounts) {
            if (pair.second > 1) {  // Appears multiple times
                strengthenMemory(1.02f);
            }
        }
    }
}

std::vector<float> NeuralWorkingMemory::predictNextAction() const {
    std::vector<float> prediction(6, 0.0f);  // 6 motor actions
    
    if (!brain_) return prediction;
    
    // Calculate predictions based on memory patterns and episodic support
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float activation = memoryActivations_[i];
        
        // Find supported motor neurons
        for (auto* neuron : brain_->getMotorNeurons()) {
            if (neuron->getId() == memoryNeurons_[i]) {
                // Calculate action probability based on activation
                prediction[i % prediction.size()] += activation * 0.2f;
                break;
            }
        }
    }
    
    // Normalize predictions
    float total = std::accumulate(prediction.begin(), prediction.end(), 0.0f);
    if (total > 0.0f) {
        for (float& value : prediction) {
            value /= total;
        }
    }
    
    return prediction;
}

void NeuralWorkingMemory::updateEpisodicRelevance(
    NeuralEpisodicMemory* episodicMem) {
    
    if (!episodicMem) return;
    
    // Update relevance scores based on working memory usage
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] > 0.3f) {
            float currentRelevance = pImpl->episodicSupportMap[i];
            float newRelevance = currentRelevance + 0.01f * memoryActivations_[i];
            newRelevance = std::min(1.0f, newRelevance);
            
            pImpl->episodicSupportMap[i] = newRelevance;
            
            // Update episodic memory if available
            if (i < episodicMem->getEpisodeCount()) {
                episodicMem->updateRelevance(static_cast<SimulationStep>(i), newRelevance);
            }
        }
    }
}

void NeuralWorkingMemory::storeToEpisodicMemory(
    NeuralEpisodicMemory* episodicMem) {
    
    if (!episodicMem || episodicMem->getEpisodeCount() == 0) return;
    
    // Store current working memory state to episodic memory
    EnhancedEpisodicMemoryItem episode;
    episode.timestamp = lastUpdateStep_;
    episode.episodeAge = lastUpdateStep_;
    
    // Copy modalities from current state
    episode.modalities.reward = episodicSupportLevel_;
    episode.modalities.emotion = noveltyLevel_;
    episode.modalities.novelty = noveltyLevel_;
    
    // Store as episodic memory item
    episodicMem->storeEpisode(episode);
}

void NeuralWorkingMemory::strengthenEpisodicTrace(
    size_t episodeId, float factor) {
    
    if (!episodicMemory_) return;
    
    // Find and strengthen episodic trace
    auto* episode = episodicMemory_->getEpisode(episodeId);
    if (episode) {
        episode->relevanceScore = std::min(1.0f, episode->relevanceScore * factor);
        episode->consolidationStrength = std::min(1.0f, episode->consolidationStrength * factor);
    }
}

void NeuralWorkingMemory::maintainTemporalSequence() {
    // Update temporal sequence based on recent activations
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        if (memoryActivations_[i] > 0.1f) {
            // Add to sequence if not already present
            bool found = false;
            for (size_t seq : temporalSequence_) {
                if (seq == i) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                // Keep only most recent 20 items
                if (temporalSequence_.size() >= 20) {
                    temporalSequence_.erase(temporalSequence_.begin());
                }
                temporalSequence_.push_back(i);
            }
        }
    }
}

void NeuralWorkingMemory::completePatternsFromEpisodic() {
    // Complete patterns using episodic memory traces
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] > patternCompletionThreshold_) {
            // Find similar episodes to complete pattern
            std::vector<float> pattern(1, memoryActivations_[i]);
            if (episodicMemory_) {
                auto similar = episodicMemory_->retrieveSimilar(pattern, 3);
                
                for (const auto* episode : similar) {
                    if (episode && episode->replayNeurons.size() > 0) {
                        // Complete pattern with episodic support
                        float completionScore = episode->relevanceScore * 0.7f;
                        memoryActivations_[i] = std::min(1.0f, 
                            memoryActivations_[i] + completionScore * 0.3f);
                    }
                }
            }
        }
    }
}

void NeuralWorkingMemory::updateTemporalContext() {
    // Update temporal context for better retrieval
    for (size_t i = 0; i < memoryTimestamps_.size(); ++i) {
        // Apply temporal decay
        if (memoryTimestamps_[i] > 0) {
            float temporalWeight = 1.0f / (1.0f + memoryTimestamps_[i] * 0.01f);
            memoryActivations_[i] *= temporalWeight;
        }
    }
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Find neurons with above-threshold activation
    float threshold = 0.3f;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] >= threshold) {
            winners_.push_back(memoryNeurons_[i]);
        }
    }
    
    // Inhibitory competition - suppress non-winners
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        bool isWinner = std::find(winners_.begin(), winners_.end(), memoryNeurons_[i]) != winners_.end();
        
        if (!isWinner && brain_) {
            // Apply strong inhibition
            brain_->injectCurrent(memoryNeurons_[i], -memoryActivations_[i] * 3.0f);
        }
    }
}

void NeuralWorkingMemory::selectWinner() {
    // Select strongest neuron as winner
    if (memoryActivations_.empty()) return;
    
    size_t bestIndex = 0;
    float bestActivation = memoryActivations_[0];
    
    for (size_t i = 1; i < memoryActivations_.size(); ++i) {
        if (memoryActivations_[i] > bestActivation) {
            bestActivation = memoryActivations_[i];
            bestIndex = i;
        }
    }
    
    // Update winner history
    winnerHistory_.push_back(memoryNeurons_[bestIndex]);
    if (winnerHistory_.size() > 100) {
        winnerHistory_.erase(winnerHistory_.begin());
    }
}

void NeuralWorkingMemory::applyInhibition() {
    // Apply global inhibition to suppress weak traces
    float inhibitionStrength = 0.5f;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        if (memoryActivations_[i] > 0.0f) {
            // Apply inhibition proportional to activation
            memoryActivations_[i] *= (1.0f - inhibitionStrength * memoryActivations_[i]);
        }
    }
}

void NeuralWorkingMemory::enhanceWinners(float factor) {
    // Enhance winner neurons
    for (NeuronId neuron : winners_) {
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            if (memoryNeurons_[i] == neuron) {
                memoryActivations_[i] = std::min(1.0f, memoryActivations_[i] * factor);
                break;
            }
        }
    }
}

void NeuralWorkingMemory::applyEpisodicInfluence() {
    // Apply influence from episodic memory
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        auto supportIt = pImpl->episodicSupportMap.find(i);
        if (supportIt != pImpl->episodicSupportMap.end()) {
            float influence = supportIt->second * 0.5f;
            memoryActivations_[i] = std::min(1.0f, memoryActivations_[i] * (1.0f + influence));
        }
    }
}

void NeuralWorkingMemory::detectNovelty() {
    // Update novelty signals based on changes in activation
    float currentNovelty = 0.0f;
    for (float activation : memoryActivations_) {
        currentNovelty += std::abs(activation - 0.5f);  // Deviation from baseline
    }
    
    noveltyLevel_ = currentNovelty / memoryActivations_.size();
    noveltySignals_.push_back(noveltyLevel_);
    
    if (noveltySignals_.size() > 100) {
        noveltySignals_.erase(noveltySignals_.begin());
    }
}

void NeuralWorkingMemory::applyTopDownBiases() {
    // Apply top-down biases (simplified)
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        // Apply bias based on episodic relevance
        auto supportIt = pImpl->episodicSupportMap.find(i);
        if (supportIt != pImpl->episodicSupportMap.end()) {
            float bias = supportIt->second * 0.3f;
            memoryActivations_[i] *= (1.0f + bias);
        }
    }
}

void NeuralWorkingMemory::captureAttentionFromNovelty() {
    // Capture attention based on novelty levels
    if (noveltyLevel_ > 0.5f) {
        // Enhance attention to novel elements
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            if (memoryActivations_[i] < 0.3f) {
                // Pay attention to previously weak traces
                memoryActivations_[i] *= (1.0f + noveltyLevel_ * 0.5f);
            }
        }
    }
}

void NeuralWorkingMemory::enforceCapacity() {
    // Ensure memory doesn't exceed capacity
    while (memoryNeurons_.size() > capacity_) {
        // Find weakest trace to remove
        size_t weakestIndex = 0;
        float weakestActivation = memoryActivations_[0];
        
        for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
            if (memoryActivations_[i] < weakestActivation) {
                weakestActivation = memoryActivations_[i];
                weakestIndex = i;
            }
        }
        
        // Remove weakest trace
        memoryNeurons_.erase(memoryNeurons_.begin() + weakestIndex);
        memoryActivations_.erase(memoryActivations_.begin() + weakestIndex);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + weakestIndex);
        rehearsalTimestamps_.erase(rehearsalTimestamps_.begin() + weakestIndex);
        rehearsalStrengths_.erase(rehearsalStrengths_.begin() + weakestIndex);
        
        // Remove from episodic support map
        auto it = pImpl->episodicSupportMap.find(weakestIndex);
        if (it != pImpl->episodicSupportMap.end()) {
            pImpl->episodicSupportMap.erase(it);
        }
        
        // Update remaining indices in episodic support map
        for (auto& pair : pImpl->episodicSupportMap) {
            if (pair.first > weakestIndex) {
                pair.first--;
            }
        }
    }
}

void NeuralWorkingMemory::evictWeakTraces() {
    // Remove traces below activation threshold
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        rehearsalTimestamps_.erase(rehearsalTimestamps_.begin() + idx);
        rehearsalStrengths_.erase(rehearsalStrengths_.begin() + idx);
        
        // Remove from episodic support map
        auto supportIt = pImpl->episodicSupportMap.find(idx);
        if (supportIt != pImpl->episodicSupportMap.end()) {
            pImpl->episodicSupportMap.erase(supportIt);
        }
        
        // Update remaining indices in episodic support map
        for (auto& pair : pImpl->episodicSupportMap) {
            if (pair.first > idx) {
                pair.first--;
            }
        }
    }
}

void NeuralWorkingMemory::compressWorkingMemory() {
    // Compress working memory to reduce redundancy
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        // Apply compression to weak traces
        if (memoryActivations_[i] < 0.1f) {
            memoryActivations_[i] *= 0.5f;
        }
        
        // Merge similar activations
        for (size_t j = i + 1; j < memoryNeurons_.size(); ++j) {
            if (std::abs(memoryNeurons_[i].getId() - memoryNeurons_[j].getId()) < 10) {
                // Similar neurons - merge activations
                memoryActivations_[i] = (memoryActivations_[i] + memoryActivations_[j]) * 0.5f;
                memoryActivations_[j] = 0.0f;
            }
        }
    }
    
    // Remove zero activations
    std::vector<size_t> toRemove;
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        if (memoryActivations_[i] < 0.001f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove compressed traces
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        if (idx < memoryNeurons_.size()) {
            memoryNeurons_.erase(memoryNeurons_.begin() + idx);
            memoryActivations_.erase(memoryActivations_.begin() + idx);
            memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
            rehearsalTimestamps_.erase(rehearsalTimestamps_.begin() + idx);
            rehearsalStrengths_.erase(rehearsalStrengths_.begin() + idx);
        }
    }
}

void NeuralWorkingMemory::updateRehearsalStrength() {
    // Update rehearsal strength based on usage
    rehearsalStrength_ = 1.0f;
    
    float totalRehearsal = 0.0f;
    for (float strength : rehearsalStrengths_) {
        totalRehearsal += strength;
    }
    
    if (!rehearsalStrengths_.empty()) {
        rehearsalStrength_ = totalRehearsal / rehearsalStrengths_.size();
    }
    
    // Update episodic support level
    episodicSupportLevel_ = 0.0f;
    if (!pImpl->episodicSupportMap.empty()) {
        float totalSupport = 0.0f;
        for (const auto& pair : pImpl->episodicSupportMap) {
            totalSupport += pair.second;
        }
        episodicSupportLevel_ = totalSupport / pImpl->episodicSupportMap.size();
    }
}

float NeuralWorkingMemory::getMemoryActivity() const {
    if (memoryActivations_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (float act : memoryActivations_) {
        total += act;
    }
    return total / memoryActivations_.size();
}

void NeuralWorkingMemory::createRecurrentConnection(
    NeuronId from, NeuronId to, float strength) {
    
    // Check if connection already exists
    for (const auto& conn : pImpl->maintenanceSynapses) {
        if (conn.first == from && conn.second == to) return;
    }
    
    pImpl->maintenanceSynapses.emplace_back(from, to);
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        
        // Find from neuron activation
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            fromActivation = memoryActivations_[idx];
        }
        
        if (fromActivation > 0.1f && brain_) {
            // Send maintenance signal
            brain_->injectCurrent(conn.second, fromActivation * 2.0f);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        rehearsalTimestamps_.erase(rehearsalTimestamps_.begin() + idx);
        rehearsalStrengths_.erase(rehearsalStrengths_.begin() + idx);
        
        // Remove from episodic support map
        auto supportIt = pImpl->episodicSupportMap.find(idx);
        if (supportIt != pImpl->episodicSupportMap.end()) {
            pImpl->episodicSupportMap.erase(supportIt);
        }
        
        // Update remaining indices in episodic support map
        for (auto& pair : pImpl->episodicSupportMap) {
            if (pair.first > idx) {
                pair.first--;
            }
        }
    }
}

void NeuralWorkingMemory::scheduleRehearsal(
    size_t episodeId, EpisodeSupportType type) {
    
    // Schedule rehearsal for episode
    pImpl->rehearsalSchedules.push_back({episodeId, 1});  // 1 step delay
    
    if (pImpl->rehearsalSchedules.size() > 100) {
        pImpl->rehearsalSchedules.erase(pImpl->rehearsalSchedules.begin());
    }
}

} // namespace nlm