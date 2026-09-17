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
    
    // Track replay timestamps for consolidation timing
    std::vector<SimulationStep> replayTimestamps;
    
    Impl() : brain(nullptr) {}
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(1000)
    , replayEnabled_(true)
{
    // Initialize replay-related state
    replayTimestamps_.clear();
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
    replayTimestamps_.push_back(pImpl->brain ? pImpl->brain->getCurrentStep() : 0);
    
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
        replayTimestamps_.erase(replayTimestamps_.begin());
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
    
    // Get the STDP plasticity rule for synaptic strengthening during replay
    auto* stdp = brain_->getSTDP();
    
    // Reactivate neurons that were active during this episode with enhanced activation
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = i < episode->neuronActivations.size() ? 
                          episode->neuronActivations[i] : 0.5f;
        
        // Inject current to reawaken this pattern with boosted activation
        // Replay uses stronger activation to promote synaptic consolidation
        brain_->injectCurrent(neuron, activation * 5.0f);
        
        // Record that this neuron was reactivated during replay
        // (In full implementation, would update spike history for STDP)
    }
    
    // Apply STDP-like synaptic strengthening for replay events
    // This mimics the biological process where reactivating patterns
    // strengthens connections between co-activated neurons
    if (stdp) {
        // Get associated neurons from the episode's resulting state
        // These represent the neural patterns after the episode
        if (!episode->activeNeurons.empty() && !episode->resultingSensoryState.empty()) {
            // Find resulting pattern neurons (simplified approach)
            // In full implementation, would have resulting neuron IDs
            // For now, we rely on the reactivated patterns above
        }
        
        // Trigger STDP update for replay consolidation
        // This strengthens synapses that were active during the original experience
        for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
            // Get the neuron that was active
            for (auto& region : brain_->getAllRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->getId() == episode->activeNeurons[i]) {
                            // Update spike history for STDP to use in next timestep
                            // In a real implementation, this would record pre-synaptic spike timing
                            neuron->recordReplaySpike();
                        }
                    }
                }
            }
        }
    }
    
    // Update associative memory with replayed patterns
    // This links current state with recalled memories
    if (brain_->getAssociativeMemory()) {
        brain_->getAssociativeMemory()->updateAssociationFromReplay(
            episode->sensoryState, 
            episode->resultingSensoryState,
            0.5f  // Moderate strengthening for replay
        );
    }
    
    // Update neuromodulatory signals based on replay content
    // Replay of salient experiences increases curiosity and dopamine
    if (brain_->getCuriosity()) {
        // Calculate prediction error proxy from episode characteristics
        float predictionErrorProxy = 0.0f;
        if (episode->reward == 0 && episode->novelty > 0.5f) {
            predictionErrorProxy = episode->novelty * 2.0f;
        } else if (episode->reward > 0.5f) {
            predictionErrorProxy = std::min(1.0f, episode->reward * 0.5f);
        }
        
        if (predictionErrorProxy > 0.1f) {
            brain_->getCuriosity()->update(predictionErrorProxy, predictionErrorProxy, 0.001f);
        }
    }
    
    // Integrate replayed patterns into working memory
    if (brain_->getWorkingMemory()) {
        // Store key features from replayed episode in working memory
        std::vector<float> episodeFeatures;
        episodeFeatures.reserve(episode->activeNeurons.size());
        
        for (size_t i = 0; i < std::min(size_t(10), episode->activeNeurons.size()); ++i) {
            // Sample some neuron activations
            float activation = i < episode->neuronActivations.size() ? 
                              episode->neuronActivations[i] : 0.5f;
            episodeFeatures.push_back(activation);
        }
        
        brain_->getWorkingMemory()->integrateReplayPattern(episodeFeatures);
    }
    
    // Trigger concept formation update with replayed patterns
    if (brain_->getConceptFormation()) {
        // Replay provides material for concept refinement
        // In full implementation, would analyze patterns and suggest concept updates
        // For now, we rely on the STDP effects to update synaptic weights
    }
    
    // Update neural planner with replayed behavioral sequences
    if (brain_->getNeuralPlanner()) {
        // Replay of action-episode sequences helps planning
        if (episode->action != ActionType::Wait) {
            // Add to planner's behavioral repertoire
            brain_->getNeuralPlanner()->recordReplayedAction(
                episode->action,
                episode->sensoryState,
                episode->reward
            );
        }
    }
    
    // Update development system with replay effects
    if (brain_->getDevelopmentSystem()) {
        // Replay of experiences can trigger developmental changes
        // Stronger replay during critical periods promotes development
        if (episode->novelty > 0.7f || episode->reward > 0.5f) {
            brain_->getDevelopmentSystem()->updateFromReplay(
                episode->activeNeurons,
                episode->reward,
                episode->novelty
            );
        }
    }
    
    // Apply attention system modulation based on replay
    if (brain_->getAttention()) {
        // Replay of salient experiences biases attention toward similar patterns
        if (episode->reward > 0.5f) {
            brain_->getAttention()->boostAttention(0.3f, episode->reward);
        }
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

std::vector<size_t> NeuralEpisodicMemory::selectImportantEpisodes(size_t count, bool sleepMode) const {
    std::vector<size_t> importantIndices;
    
    if (episodes_.empty()) return importantIndices;
    
    // Compute importance scores for each episode
    std::vector<std::pair<float, size_t>> importanceScores;  // (score, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        const auto& ep = episodes_[i];
        float importance = 0.0f;
        
        // Recency (more recent = higher importance)
        float recency = 1.0f / (ep.age + 1);
        importance += recency * 0.3f;
        
        // Reward salience
        importance += ep.reward * 0.3f;
        
        // Novelty (unexpected experiences)
        importance += ep.novelty * 0.2f;
        
        // Prediction error proxy (low reward + high novelty)
        float predictionErrorProxy = 0.0f;
        if (ep.reward < 0.3f && ep.novelty > 0.5f) {
            predictionErrorProxy = (0.5f - ep.reward) * ep.novelty;
        }
        importance += predictionErrorProxy * 0.2f;
        
        // Sleep mode: prioritize consolidation of important experiences
        if (sleepMode) {
            // During sleep, prioritize experiences that need consolidation
            // Experiences with moderate reward but high novelty need consolidation
            float consolidationPriority = (ep.reward * 0.4f + ep.novelty * 0.6f);
            importance += consolidationPriority * 0.5f;
            
            // Also prioritize recent experiences for temporal sequence
            importance += recency * 0.2f;
        }
        
        importanceScores.emplace_back(importance, i);
    }
    
    // Sort by importance (descending)
    std::sort(importanceScores.begin(), importanceScores.end(),
        [](const auto& a, const auto& b) {
            return a.first > b.first;
        });
    
    // Select top episodes
    for (size_t i = 0; i < std::min(count, importanceScores.size()); ++i) {
        importantIndices.push_back(importanceScores[i].second);
    }
    
    return importantIndices;
}

void NeuralEpisodicMemory::integrateReplayWithCurrentState() {
    if (!brain_) return;
    
    // Get current neural state
    std::vector<float> currentPattern;
    float currentReward = 0.0f;
    
    // In full implementation, would collect current neural activity
    // For now, use a simplified approach
    if (brain_->getDopamine()) {
        currentReward = brain_->getDopamine()->getLevel();
    }
    
    // Integrate replayed patterns into current state
    if (brain_->getAssociativeMemory()) {
        // Use current state to retrieve related memories
        std::vector<std::vector<float>> related = brain_->getAssociativeMemory()->
            spreadActivation(currentPattern, 2);
        
        // Combine with replay effects
        for (const auto& pattern : related) {
            // Integrate related patterns with replay
            brain_->getAssociativeMemory()->integrateWithReplay(pattern, currentPattern);
        }
    }
    
    // Update working memory with integrated patterns
    if (brain_->getWorkingMemory()) {
        brain_->getWorkingMemory()->integrateWithReplay(currentPattern, currentReward);
    }
}

void NeuralEpisodicMemory::applySynapticConsolidation(const EpisodicMemoryItem* episode) {
    if (!episode || !brain_ || !brain_->getSTDP()) return;
    
    // Apply STDP-like consolidation for replayed episodes
    // This strengthens synapses that were active during the original experience
    
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = i < episode->neuronActivations.size() ? 
                          episode->neuronActivations[i] : 0.5f;
        
        // Find and update the neuron's spike history for STDP
        for (auto& region : brain_->getAllRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* n : pop->getNeurons()) {
                    if (n->getId() == neuron) {
                        // Apply replay-specific STDP
                        n->applyReplaySTDP(activation);
                        break;
                    }
                }
            }
        }
    }
}

void NeuralEpisodicMemory::sleepReplay(const std::vector<size_t>& episodeIds) {
    // Enhanced replay during rest/sleep periods
    // Sleep replay uses stronger activation and more extensive consolidation
    
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            // Boost activation for sleep replay to promote synaptic consolidation
            const auto& episode = episodes_[id];
            
            // Reactivate with higher intensity for consolidation
            for (size_t i = 0; i < episode.activeNeurons.size(); ++i) {
                NeuronId neuron = episode.activeNeurons[i];
                float activation = i < episode.neuronActivations.size() ? 
                                  episode.neuronActivations[i] : 0.5f;
                
                // Much stronger injection during sleep for consolidation
                brain_->injectCurrent(neuron, activation * 8.0f);
                
                // Record sleep replay spike for STDP
                for (auto& region : brain_->getAllRegions()) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* n : pop->getNeurons()) {
                            if (n->getId() == neuron) {
                                n->recordSleepReplaySpike();
                            }
                        }
                    }
                }
            }
            
            // Also call regular replay for full integration
            replayEpisode(&episode);
        }
    }
    
    // Post-sleep consolidation: strengthen associations
    if (brain_->getAssociativeMemory()) {
        brain_->getAssociativeMemory()->sleepConsolidateAssociations();
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