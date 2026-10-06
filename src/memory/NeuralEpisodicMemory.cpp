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
    
    episodes_.push_back(stored);
    
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

bool NeuralEpisodicMemory::testMemoryExport() const {
    return !episodes_.empty();
}

std::string NeuralEpisodicMemory::getDebugInfo() const {
    std::ostringstream oss;
    oss << "NeuralEpisodicMemory Debug Info:\n";
    oss << "  Episode count: " << episodes_.size() << "\n";
    oss << "  Max episodes: " << maxEpisodes_ << "\n";
    oss << "  Replay enabled: " << (replayEnabled_ ? "yes" : "no") << "\n";
    oss << "  Average reward: " << getAverageReward() << "\n";
    oss << "  Average novelty: " << (episodes_.empty() ? 0 : episodes_.back().novelty) << "\n";
    oss << "  Average energy: " << (episodes_.empty() ? 0 : episodes_.back().energy) << "\n";
    return oss.str();
}

bool NeuralEpisodicMemory::exportMemoryTraces(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for memory export: " + filename);
        return false;
    }
    
    file << "Neural Episodic Memory Trace\n";
    file << "Timestamp: " << std::time(nullptr) << "\n";
    file << "Total episodes: " << episodes_.size() << "\n";
    file << "Max episodes: " << maxEpisodes_ << "\n";
    file << "Replay enabled: " << (replayEnabled_ ? "true" : "false") << "\n";
    file << "\nEpisodic Memory Items:\n";
    
    size_t episodeIndex = 0;
    for (const auto& episode : episodes_) {
        file << "=== Episode " << episodeIndex++ << " ===\n";
        file << "Timestamp: " << episode.timestamp << "\n";
        file << "Reward: " << episode.reward << "\n";
        file << "Novelty: " << episode.novelty << "\n";
        file << "Energy: " << episode.energy << "\n";
        file << "Age: " << episode.age << "\n";
        file << "Action: " << static_cast<int>(episode.action) << "\n";
        
        if (!episode.sensoryState.empty()) {
            file << "Sensory state (first 10 values): ";
            for (size_t i = 0; i < std::min(size_t(10), episode.sensoryState.size()); ++i) {
                file << episode.sensoryState[i];
                if (i < episode.sensoryState.size() - 1) file << ", ";
            }
            file << "\n";
        }
        
        file << "Active neurons: [";
        for (size_t i = 0; i < episode.activeNeurons.size(); ++i) {
            file << episode.activeNeurons[i].getId();
            if (i < episode.activeNeurons.size() - 1) file << ", ";
        }
        file << "]\n";
        
        if (!episode.neuronActivations.empty()) {
            file << "Neuron activations (first 10): ";
            for (size_t i = 0; i < std::min(size_t(10), episode.neuronActivations.size()); ++i) {
                file << episode.neuronActivations[i];
                if (i < episode.neuronActivations.size() - 1) file << ", ";
            }
            file << "\n";
        }
        
        file << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Memory traces exported to: " + filename);
    return true;
}

bool NeuralEpisodicMemory::exportMemoryTracesJSON(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    file << "  \"type\": \"NeuralEpisodicMemory\",\n";
    file << "  \"timestamp\": " << std::time(nullptr) << ",\n";
    file << "  \"maxEpisodes\": " << maxEpisodes_ << ",\n";
    file << "  \"replayEnabled\": " << (replayEnabled_ ? "true" : "false") << ",\n";
    file << "  \"episodeCount\": " << episodes_.size() << ",\n";
    file << "  \"episodes\": [\n";
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        const auto& episode = episodes_[i];
        file << "    {\n";
        file << "      \"timestamp\": " << episode.timestamp << ",\n";
        file << "      \"reward\": " << episode.reward << ",\n";
        file << "      \"novelty\": " << episode.novelty << ",\n";
        file << "      \"energy\": " << episode.energy << ",\n";
        file << "      \"age\": " << episode.age << ",\n";
        file << "      \"action\": " << static_cast<int>(episode.action) << ",\n";
        file << "      \"sensoryState\": [";
        
        for (size_t j = 0; j < std::min(size_t(10), episode.sensoryState.size()); ++j) {
            file << episode.sensoryState[j];
            if (j < std::min(size_t(10), episode.sensoryState.size()) - 1) file << ", ";
        }
        file << "],\n";
        
        file << "      \"activeNeurons\": [";
        for (size_t j = 0; j < episode.activeNeurons.size(); ++j) {
            file << episode.activeNeurons[j].getId();
            if (j < episode.activeNeurons.size() - 1) file << ", ";
        }
        file << "],\n";
        
        file << "      \"neuronActivations\": [";
        for (size_t j = 0; j < std::min(size_t(10), episode.neuronActivations.size()); ++j) {
            file << episode.neuronActivations[j];
            if (j < std::min(size_t(10), episode.neuronActivations.size()) - 1) file << ", ";
        }
        file << "]\n";
        
        file << "    }";
        if (i < episodes_.size() - 1) file << ",";
        file << "\n";
    }
    
    file << "  ],\n";
    file << "  \"averageReward\": " << getAverageReward() << "\n";
    file << "}\n";
    file.close();
    return true;
}

bool NeuralEpisodicMemory::exportMemoryTracesCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "EpisodeIndex,Timestamp,Reward,Novelty,Energy,Age,Action,SensoryState1,SensoryState2,SensoryState3,ActiveNeuronsCount,NeuronActivationsCount\n";
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        const auto& episode = episodes_[i];
        file << i << "," << episode.timestamp << "," << episode.reward << "," 
             << episode.novelty << "," << episode.energy << "," << episode.age << "," 
             << static_cast<int>(episode.action) << ",";
        
        // Add first few sensory values
        for (size_t j = 0; j < std::min(size_t(3), episode.sensoryState.size()); ++j) {
            file << episode.sensoryState[j];
            if (j < std::min(size_t(3), episode.sensoryState.size()) - 1) file << ",";
        }
        file << "," << episode.activeNeurons.size() << "," << episode.neuronActivations.size() << "\n";
    }
    
    file.close();
    return true;
}

bool NeuralEpisodicMemory::importMemoryTraces(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for memory import: " + filename);
        return false;
    }
    
    std::string line;
    bool inDataSection = false;
    size_t episodeIndex = 0;
    
    // Skip header line
    if (std::getline(file, line)) {
        if (line.find("EpisodeIndex,Timestamp,Reward,Novelty,Energy,Age,Action") != std::string::npos) {
            inDataSection = true;
        }
    }
    
    while (inDataSection && std::getline(file, line)) {
        if (line.empty() || line.find("===") != std::string::npos) {
            continue;
        }
        
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 7) {
            EpisodicMemoryItem episode;
            episode.timestamp = static_cast<SimulationStep>(std::stoul(tokens[1]));
            episode.reward = std::stof(tokens[2]);
            episode.novelty = std::stof(tokens[3]);
            episode.energy = std::stof(tokens[4]);
            episode.age = static_cast<SimulationStep>(std::stoul(tokens[5]));
            episode.action = static_cast<ActionType>(std::stoi(tokens[6]));
            
            // Clear neural data for imported episodes
            episode.activeNeurons.clear();
            episode.neuronActivations.clear();
            
            // Add episode
            episodes_.push_back(episode);
            episodeIndex++;
            
            // Enforce max capacity
            while (episodes_.size() > maxEpisodes_) {
                episodes_.erase(episodes_.begin());
            }
        }
    }
    
    file.close();
    NLM_LOG_INFO("Memory traces imported from: " + filename + ". Imported " + 
                std::to_string(episodes_.size()) + " episodes.");
    return true;
}