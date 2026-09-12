#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <map>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Hopfield network parameters
    float beta;  // Temperature parameter
    float gamma; // Decay rate
    
    // Neural dynamics state
    std::vector<float> energyHistory;
    std::vector<float> patternHistory;
    
    // Replay mechanism
    bool isReplaying;
    SimulationStep replayStep;
    
    Impl() : brain(nullptr), beta(0.5f), gamma(0.01f), isReplaying(false), replayStep(0) {}
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(1000)
{
    std::random_device rd;
    pImpl->rng.seed(rd());
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    clear();
    
    // Initialize neural dynamics
    pImpl->energyHistory.clear();
    pImpl->patternHistory.clear();
    
    NLM_LOG_INFO("NeuralEpisodicMemory initialized");
}

void NeuralEpisodicMemory::setMaxEpisodes(size_t max) {
    maxEpisodes_ = max;
    
    // Trim episodes if needed
    while (episodes_.size() > maxEpisodes_) {
        episodes_.pop_front();
    }
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    if (!brain_) return;
    
    // Encode episode as neural pattern
    std::vector<float> neuralPattern;
    
    // Convert active neurons to pattern
    size_t totalNeurons = 0;
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            totalNeurons += pop->getNeurons().size();
        }
    }
    
    neuralPattern.resize(totalNeurons, 0.0f);
    
    // Fill pattern with activation levels
    size_t idx = 0;
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (idx < neuralPattern.size()) {
                    // Convert membrane potential to normalized activation
                    float v = neuron->getMembranePotential();
                    float vRest = neuron->getRestingPotential();
                    neuralPattern[idx] = std::max(0.0f, (v - vRest) / 50.0f);
                    idx++;
                }
            }
        }
    }
    
    // Add reward information
    neuralPattern.push_back(episode.reward);
    
    // Create Hopfield-like representation
    EpisodicMemoryItem neuralEpisode = episode;
    neuralEpisode.pattern = neuralPattern;
    
    // Store with temporal context
    neuralEpisode.timestamp = currentStep_;
    
    episodes_.push_back(neuralEpisode);
    
    // Update energy function
    float energy = computePatternEnergy(neuralPattern);
    pImpl->energyHistory.push_back(energy);
    pImpl->patternHistory.push_back(neuralPattern);
    
    // Trim if necessary
    if (episodes_.size() > maxEpisodes_) {
        episodes_.pop_front();
        pImpl->energyHistory.erase(pImpl->energyHistory.begin());
        pImpl->patternHistory.erase(pImpl->patternHistory.begin());
    }
    
    NLM_LOG_INFO("Stored episode at step " + std::to_string(episode.timestamp) + 
                ", energy = " + std::to_string(energy));
}

void NeuralEpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!brain_ || !episode) return;
    
    // Replay mechanism - reconstruct pattern and replay activity
    pImpl->isReplaying = true;
    pImpl->replayStep = episode->timestamp;
    
    // Convert pattern back to neural activity
    std::vector<NeuronId> activeNeurons;
    float reward = 0.0f;
    
    // Decode pattern into neural activations
    size_t patternSize = episode->pattern.size();
    size_t idx = 0;
    for (auto& region : brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (idx < patternSize) {
                    float activation = episode->pattern[idx];
                    
                    if (activation > 0.5f) {
                        // Reactivate neuron
                        activeNeurons.push_back(neuron->getId());
                        brain_->injectCurrent(neuron->getId(), activation * 8.0f);
                    }
                    idx++;
                }
            }
        }
    }
    
    if (patternSize < episode->pattern.size()) {
        reward = episode->pattern[patternSize];
    }
    
    // Replay the experience with strengthened plasticity
    for (NeuronId neuronId : activeNeurons) {
        // Inject strong current for replay
        brain_->injectCurrent(neuronId, 15.0f);
    }
    
    // Apply reward modulation during replay
    if (reward > 0.0f) {
        // Create a neuromodulation signal for replay
        float replayEffect = reward * 0.5f;
        
        // Enhance synaptic strengthening during replay
        for (NeuronId neuronId : activeNeurons) {
            brain_->injectCurrent(neuronId, replayEffect * 3.0f);
        }
    }
    
    pImpl->isReplaying = false;
    
    NLM_LOG_INFO("Replayed episode from step " + std::to_string(episode->timestamp) + 
                ", activated " + std::to_string(activeNeurons.size()) + " neurons");
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(size_t maxEpisodes) const {
    std::vector<const EpisodicMemoryItem*> result;
    
    if (episodes_.empty()) {
        return result;
    }
    
    // Select episodes based on similarity and importance
    std::vector<std::pair<float, size_t>> episodeScores;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        const auto& episode = episodes_[i];
        
        // Score based on reward and recency
        float score = episode.reward * 10.0f;
        
        // Boost recent episodes
        float age = currentStep_ - episode.timestamp;
        if (age < 100) {
            score *= (1.0f + (100.0f - age) / 100.0f);
        }
        
        episodeScores.push_back(std::make_pair(score, i));
    }
    
    // Sort by score (descending)
    std::sort(episodeScores.begin(), episodeScores.end(),
              [](const std::pair<float, size_t>& a, const std::pair<float, size_t>& b) {
                  return a.first > b.first;
              });
    
    // Select top episodes
    size_t count = std::min(maxEpisodes, episodeScores.size());
    for (size_t i = 0; i < count; ++i) {
        result.push_back(&episodes_[episodeScores[i].second]);
    }
    
    return result;
}

void NeuralEpisodicMemory::consolidate(float threshold) {
    if (!brain_ || episodes_.empty()) return;
    
    // Memory consolidation - strengthen important episodes, prune weak ones
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        const auto& episode = episodes_[i];
        
        // Calculate episode importance
        float importance = episode.reward * 10.0f;
        
        // Also consider neural pattern energy
        if (i < pImpl->energyHistory.size()) {
            importance += (1.0f - pImpl->energyHistory[i] / 10.0f) * 5.0f;
        }
        
        // Weak episodes get pruned
        if (importance < threshold) {
            toRemove.push_back(i);
        } else {
            // Strengthen episode representation
            strengthenEpisode(i, importance * 0.01f);
        }
    }
    
    // Remove weak episodes (in reverse order)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        episodes_.erase(episodes_.begin() + *it);
        
        if (*it < pImpl->energyHistory.size()) {
            pImpl->energyHistory.erase(pImpl->energyHistory.begin() + *it);
        }
        
        if (*it < pImpl->patternHistory.size()) {
            pImpl->patternHistory.erase(pImpl->patternHistory.begin() + *it);
        }
    }
    
    NLM_LOG_INFO("Consolidated episodic memory, removed " + std::to_string(toRemove.size()) + 
                " weak episodes, kept " + std::to_string(episodes_.size()) + " strong episodes");
}

void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    pImpl->energyHistory.clear();
    pImpl->patternHistory.clear();
    pImpl->isReplaying = false;
}

size_t NeuralEpisodicMemory::getEpisodeCount() const {
    return episodes_.size();
}

float NeuralEpisodicMemory::computePatternEnergy(const std::vector<float>& pattern) const {
    // Simple energy function for Hopfield-like dynamics
    float energy = 0.0f;
    
    // Pattern stability term
    float patternStability = 0.0f;
    for (float val : pattern) {
        patternStability += val * val;
    }
    energy += patternStability * 0.1f;
    
    // Temporal coherence term (simplified)
    if (!patternHistory_.empty()) {
        const auto& lastPattern = patternHistory_.back();
        if (pattern.size() == lastPattern.size()) {
            float similarity = 0.0f;
            for (size_t i = 0; i < pattern.size(); ++i) {
                similarity += pattern[i] * lastPattern[i];
            }
            energy -= similarity * 0.5f;
        }
    }
    
    return std::max(0.0f, energy);
}

void NeuralEpisodicMemory::strengthenEpisode(size_t episodeIndex, float factor) {
    if (episodeIndex >= episodes_.size()) return;
    
    auto& episode = episodes_[episodeIndex];
    
    // Strengthen pattern representation
    for (float& val : episode.pattern) {
        val = std::min(1.0f, val + factor);
    }
    
    // Boost episode reward
    episode.reward = std::min(1.0f, episode.reward + factor * 0.1f);
}

} // namespace nlm