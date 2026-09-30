#include "NeuralEpisodicMemoryWithSleep.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <numeric>

namespace nlm {

struct NeuralEpisodicMemoryWithSleep::Impl {
    Brain* brain;
    
    // Pattern neurons for episodic recall
    std::vector<NeuronId> episodeNeurons;
    
    // Sleep replay statistics
    size_t replayCount;
    float replayStartTime;
    float replayDuration;
    
    // Consolidation statistics
    float consolidationProgress;
    float schemaFormationProgress;
    float integrationProgress;
    
    // State-specific replay parameters
    std::map<SleepState, float> stateReplayIntervals;
    std::map<SleepState, float> stateReplayStrengths;
    std::map<SleepState, float> stateConsolidationIntensities;
    std::map<SleepState, float> stateSchemaFormationStrengths;
    
    // Schema formation components
    std::vector<std::vector<float>> schemas;
    std::vector<float> schemaWeights;
    
    // Memory integration vectors
    std::vector<std::pair<size_t, size_t>> memoryAssociations;
    std::vector<float> integrationWeights;
    
    Impl() : brain(nullptr)
        , replayCount(0)
        , replayStartTime(0.0f)
        , replayDuration(0.0f)
        , consolidationProgress(0.0f)
        , schemaFormationProgress(0.0f)
        , integrationProgress(0.0f) {
        
        // Initialize state-specific parameters
        stateReplayIntervals[SleepState::QuietSleep] = 2.0f;
        stateReplayIntervals[SleepState::DeepSleep] = 3.0f;
        stateReplayIntervals[SleepState::REMSleep] = 0.5f;
        stateReplayIntervals[SleepState::Wake] = 0.0f;
        
        stateReplayStrengths[SleepState::QuietSleep] = 0.3f;
        stateReplayStrengths[SleepState::DeepSleep] = 0.5f;
        stateReplayStrengths[SleepState::REMSleep] = 0.8f;
        stateReplayStrengths[SleepState::Wake] = 0.0f;
        
        stateConsolidationIntensities[SleepState::QuietSleep] = 0.4f;
        stateConsolidationIntensities[SleepState::DeepSleep] = 0.7f;
        stateConsolidationIntensities[SleepState::REMSleep] = 0.6f;
        stateConsolidationIntensities[SleepState::Wake] = 0.0f;
        
        stateSchemaFormationStrengths[SleepState::REMSleep] = 1.0f;
        stateSchemaFormationStrengths[SleepState::QuietSleep] = 0.3f;
        stateSchemaFormationStrengths[SleepState::DeepSleep] = 0.4f;
        stateSchemaFormationStrengths[SleepState::Wake] = 0.0f;
    }
};

NeuralEpisodicMemoryWithSleep::NeuralEpisodicMemoryWithSleep()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(1000)
    , replayEnabled_(true)
    , currentSleepState_(SleepState::Wake)
    , memoryReplayCount_(0)
    , replayStartTime_(0.0f)
    , replayDuration_(0.0f)
    , consolidationProgress_(0.0f)
    , schemaFormationProgress_(0.0f)
    , integrationProgress_(0.0f)
    , quietSleepReplayTimer_(0.0f)
    , deepSleepReplayTimer_(0.0f)
    , remSleepReplayTimer_(0.0f)
{
}

NeuralEpisodicMemoryWithSleep::~NeuralEpisodicMemoryWithSleep() = default;

void NeuralEpisodicMemoryWithSleep::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralEpisodicMemoryWithSleep initialized");
}

void NeuralEpisodicMemoryWithSleep::storeEpisode(const EpisodicMemoryItem& episode) {
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
        episodes_.pop_front();
        if (!pImpl->episodeNeurons.empty()) {
            pImpl->episodeNeurons.erase(pImpl->episodeNeurons.begin());
        }
    }
    
    // Age all episodes
    for (auto& ep : episodes_) {
        ep.age++;
    }
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemoryWithSleep::retrieveSimilar(
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

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemoryWithSleep::retrieveTemporal(
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

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemoryWithSleep::retrieveByLocation(
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

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemoryWithSleep::retrieveAfterAction(
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

void NeuralEpisodicMemoryWithSleep::replayEpisode(const EpisodicMemoryItem* episode) {
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

void NeuralEpisodicMemoryWithSleep::updateRelevance(SimulationStep episodeId, float relevanceDelta) {
    if (episodeId < episodes_.size()) {
        // In a full implementation, episodes would have a relevance field
        // For now, this would age the episode differently
    }
}

const EpisodicMemoryItem* NeuralEpisodicMemoryWithSleep::getEpisode(size_t index) const {
    if (index < episodes_.size()) {
        return &episodes_[index];
    }
    return nullptr;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemoryWithSleep::getRecentEpisodes(size_t count) const {
    std::vector<const EpisodicMemoryItem*> results;
    
    size_t start = count > episodes_.size() ? 0 : episodes_.size() - count;
    for (size_t i = start; i < episodes_.size(); ++i) {
        results.push_back(&episodes_[i]);
    }
    
    return results;
}

float NeuralEpisodicMemoryWithSleep::getAverageReward() const {
    if (episodes_.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& ep : episodes_) {
        sum += ep.reward;
    }
    return sum / episodes_.size();
}

void NeuralEpisodicMemoryWithSleep::consolidate(float relevanceThreshold) {
    // Remove episodes below relevance threshold
    // In a full implementation, episodes would have relevance scores
    // For now, just enforce max capacity
    while (episodes_.size() > maxEpisodes_ * 0.8f) {
        episodes_.pop_front();
        if (!pImpl->episodeNeurons.empty()) {
            pImpl->episodeNeurons.erase(pImpl->episodeNeurons.begin());
        }
    }
}

void NeuralEpisodicMemoryWithSleep::clear() {
    episodes_.clear();
    pImpl->episodeNeurons.clear();
    pImpl->replayCount = 0;
    consolidationProgress_ = 0.0f;
    schemaFormationProgress_ = 0.0f;
    integrationProgress_ = 0.0f;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemoryWithSleep::getEpisodesForReplay(
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

void NeuralEpisodicMemoryWithSleep::replaySequence(const std::vector<size_t>& episodeIds) {
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            replayEpisode(&episodes_[id]);
            
            // Small delay between replays
            // (In simulation, this would be steps)
        }
    }
}

void NeuralEpisodicMemoryWithSleep::executeSleepReplay(SleepState sleepState) {
    currentSleepState_ = sleepState;
    
    switch (sleepState) {
        case SleepState::QuietSleep:
            executeQuietSleepReplayPattern();
            break;
        case SleepState::DeepSleep:
            executeDeepSleepReplayPattern();
            break;
        case SleepState::REMSleep:
            executeREMSleepReplayPattern();
            break;
        case SleepState::Wake:
            // No replay during wake
            break;
    }
}

void NeuralEpisodicMemoryWithSleep::executeQuietSleepReplayPattern() {
    if (!brain_ || episodes_.empty()) return;
    
    // Quiet sleep: moderate replay frequency, focus on recent memories
    quietSleepReplayTimer_ += 0.1f;
    
    if (quietSleepReplayTimer_ >= pImpl->stateReplayIntervals[SleepState::QuietSleep]) {
        // Select episodes for quiet sleep replay
        auto episodesToReplay = getEpisodesForReplay(2);
        
        for (const auto* episode : episodesToReplay) {
            // Replay with quiet sleep characteristics
            float replayStrength = pImpl->stateReplayStrengths[SleepState::QuietSleep];
            replayEpisode(episode);
            
            // Update replay statistics
            memoryReplayCount_++;
        }
        
        quietSleepReplayTimer_ = 0.0f;
        pImpl->replayCount++;
    }
}

void NeuralEpisodicMemoryWithSleep::executeDeepSleepReplayPattern() {
    if (!brain_ || episodes_.empty()) return;
    
    // Deep sleep: low frequency, systems consolidation
    deepSleepReplayTimer_ += 0.1f;
    
    if (deepSleepReplayTimer_ >= pImpl->stateReplayIntervals[SleepState::DeepSleep]) {
        // Select fewer episodes for deep sleep replay (systems consolidation)
        auto episodesToReplay = getEpisodesForReplay(1);
        
        for (const auto* episode : episodesToReplay) {
            // Replay with deep sleep characteristics (higher strength, slower)
            float replayStrength = pImpl->stateReplayStrengths[SleepState::DeepSleep];
            replayEpisode(episode);
            
            // Update replay statistics
            memoryReplayCount_++;
            
            // Trigger systems consolidation
            systemsConsolidationDuringDeepSleep();
        }
        
        deepSleepReplayTimer_ = 0.0f;
        pImpl->replayCount++;
    }
}

void NeuralEpisodicMemoryWithSleep::executeREMSleepReplayPattern() {
    if (!brain_ || episodes_.empty()) return;
    
    // REM sleep: high frequency, memory integration, schema formation
    remSleepReplayTimer_ += 0.1f;
    
    if (remSleepReplayTimer_ >= pImpl->stateReplayIntervals[SleepState::REMSleep]) {
        // Select more episodes for REM sleep replay (integration)
        auto episodesToReplay = getEpisodesForReplay(3);
        
        for (const auto* episode : episodesToReplay) {
            // Replay with REM sleep characteristics (maximum integration)
            float replayStrength = pImpl->stateReplayStrengths[SleepState::REMSleep];
            replayEpisode(episode);
            
            // Update replay statistics
            memoryReplayCount_++;
            
            // Trigger schema formation
            formSchemasDuringREMSleep();
        }
        
        remSleepReplayTimer_ = 0.0f;
        pImpl->replayCount++;
    }
}

void NeuralEpisodicMemoryWithSleep::consolidateDuringSleep(SleepState sleepState, float duration) {
    // Apply sleep-dependent consolidation
    float consolidationRate = pImpl->stateConsolidationIntensities[sleepState];
    float consolidationDelta = consolidationRate * duration;
    
    consolidationProgress_ += consolidationDelta;
    
    // State-specific consolidation processes
    switch (sleepState) {
        case SleepState::QuietSleep:
            synapticConsolidationDuringQuietSleep();
            break;
        case SleepState::DeepSleep:
            systemsConsolidationDuringDeepSleep();
            break;
        case SleepState::REMSleep:
            schemaFormationAndGeneralization();
            break;
        case SleepState::Wake:
            // No consolidation during wake
            break;
    }
}

void NeuralEpisodicMemoryWithSleep::formSchemasDuringREMSleep() {
    if (!brain_ || episodes_.empty()) return;
    
    // Schema formation during REM sleep
    schemaFormationProgress_ += 0.1f * pImpl->stateSchemaFormationStrengths[SleepState::REMSleep];
    
    // Create abstract representations from related episodes
    std::vector<float> patternVector;
    for (const auto& episode : episodes_) {
        if (!episode.sensoryState.empty()) {
            for (float val : episode.sensoryState) {
                patternVector.push_back(val);
            }
        }
    }
    
    if (!patternVector.empty()) {
        // Normalize pattern
        float norm = 0.0f;
        for (float val : patternVector) {
            norm += val * val;
        }
        norm = std::sqrt(norm);
        if (norm > 0.0f) {
            for (float& val : patternVector) {
                val /= norm;
            }
            
            // Store schema
            pImpl->schemas.push_back(patternVector);
            pImpl->schemaWeights.push_back(1.0f);
        }
    }
}

void NeuralEpisodicMemoryWithSleep::integrateMemoriesDuringSleep(SleepState sleepState) {
    integrationProgress_ += 0.1f * pImpl->stateConsolidationIntensities[sleepState];
    
    // Create associations between related episodes
    for (size_t i = 0; i < episodes_.size(); ++i) {
        for (size_t j = i + 1; j < episodes_.size(); ++j) {
            float similarity = computeSimilarity(episodes_[i].sensoryState, episodes_[j].sensoryState);
            
            if (similarity > 0.7f) {  // Strong association
                pImpl->memoryAssociations.emplace_back(i, j);
                pImpl->integrationWeights.push_back(similarity);
            }
        }
    }
}

void NeuralEpisodicMemoryWithSleep::systemsConsolidationDuringDeepSleep() {
    // Systems consolidation - reorganize memory representation
    // Move from hippocampal to neocortical storage
    float consolidationStrength = pImpl->stateConsolidationIntensities[SleepState::DeepSleep];
    
    // Reorganize episode representations
    for (auto& episode : episodes_) {
        // Weaken temporal specificity
        episode.age = std::min(static_cast<SimulationStep>(1000), episode.age * 1.5f);
        
        // Enhance general pattern
        if (!episode.sensoryState.empty()) {
            for (float& val : episode.sensoryState) {
                val *= consolidationStrength * 0.1f;
            }
        }
    }
}

void NeuralEpisodicMemoryWithSleep::synapticConsolidationDuringQuietSleep() {
    // Synaptic consolidation - strengthen important connections
    float consolidationStrength = pImpl->stateConsolidationIntensities[SleepState::QuietSleep];
    
    // Strengthen rewarding/relevant episodes
    for (auto& episode : episodes_) {
        if (episode.reward > 0.5f) {
            // Boost reward-based memories
            if (!episode.sensoryState.empty()) {
                for (float& val : episode.sensoryState) {
                    val *= (1.0f + consolidationStrength * 0.2f);
                }
            }
        }
    }
}

void NeuralEpisodicMemoryWithSleep::schemaFormationAndGeneralization() {
    // Schema formation and generalization during REM sleep
    float schemaStrength = pImpl->stateSchemaFormationStrengths[SleepState::REMSleep];
    
    // Create generalized representations
    std::vector<float> generalizedPattern;
    for (const auto& episode : episodes_) {
        if (!episode.sensoryState.empty()) {
            // Average pattern across episodes
            for (size_t i = 0; i < episode.sensoryState.size(); ++i) {
                if (i < generalizedPattern.size()) {
                    generalizedPattern[i] += episode.sensoryState[i];
                } else {
                    generalizedPattern.push_back(episode.sensoryState[i]);
                }
            }
        }
    }
    
    // Normalize generalized pattern
    if (!generalizedPattern.empty()) {
        float norm = 0.0f;
        for (float val : generalizedPattern) {
            norm += val * val;
        }
        norm = std::sqrt(norm);
        if (norm > 0.0f) {
            for (float& val : generalizedPattern) {
                val /= norm;
            }
        }
    }
    
    // Update schema formation progress
    schemaFormationProgress_ += schemaStrength * 0.1f;
}

void NeuralEpisodicMemoryWithSleep::scheduleReplaySequence(SleepState sleepState) {
    // Set replay timing based on sleep state
    float replayInterval = pImpl->stateReplayIntervals[sleepState];
    float replayStrength = pImpl->stateReplayStrengths[sleepState];
    
    replayStartTime_ = 0.0f;
    replayDuration_ = replayInterval;
}

void NeuralEpisodicMemoryWithSleep::sequenceReplayTiming(SleepState sleepState, float& replayInterval, float& replayStrength) {
    replayInterval = pImpl->stateReplayIntervals[sleepState];
    replayStrength = pImpl->stateReplayStrengths[sleepState];
}

void NeuralEpisodicMemoryWithSleep::integrateConsolidationWithSleepStates(SleepState sleepState) {
    // Integrate consolidation processes with sleep states
    float consolidationIntensity = pImpl->stateConsolidationIntensities[sleepState];
    
    // Apply consolidation based on sleep state
    switch (sleepState) {
        case SleepState::QuietSleep:
            // Focus on synaptic strengthening
            consolidate(0.3f);
            break;
        case SleepState::DeepSleep:
            // Focus on systems consolidation
            consolidate(0.5f);
            break;
        case SleepState::REMSleep:
            // Focus on schema formation
            formSchemasDuringREMSleep();
            break;
        case SleepState::Wake:
            // No consolidation during wake
            break;
    }
}

float NeuralEpisodicMemoryWithSleep::computeSimilarity(const std::vector<float>& query,
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

bool NeuralEpisodicMemoryWithSleep::matchesCriteria(
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

void NeuralEpisodicMemoryWithSleep::scheduleMemoryReplay() {
    // Schedule memory replay for current step
    if (brain_) {
        executeSleepReplay(currentSleepState_);
    }
}

void NeuralEpisodicMemoryWithSleep::executeMemoryReplay() {
    // Execute memory replay - called from Brain::step()
    if (brain_) {
        executeSleepReplay(currentSleepState_);
    }
}

std::string NeuralEpisodicMemoryWithSleep::serialize() const {
    std::stringstream ss;
    
    // Serialize episode count and data
    ss << episodes_.size() << " ";
    ss << maxEpisodes_ << " ";
    ss << replayEnabled_ << " ";
    
    // Serialize all episodes
    for (const auto& episode : episodes_) {
        ss << episode.timestamp << " ";
        ss << episode.positionX << " " << episode.positionY << " " << episode.orientation << " ";
        ss << static_cast<int>(episode.action) << " ";
        ss << episode.reward << " " << episode.energy << " " << episode.novelty << " ";
        ss << episode.resultingReward << " ";
        ss << episode.age << " ";
        
        // Serialize sensory states
        ss << episode.sensoryState.size() << " ";
        for (float val : episode.sensoryState) {
            ss << val << " ";
        }
        ss << episode.resultingSensoryState.size() << " ";
        for (float val : episode.resultingSensoryState) {
            ss << val << " ";
        }
        
        // Serialize neuron activations
        ss << episode.activeNeurons.size() << " ";
        for (const auto& neuron : episode.activeNeurons) {
            ss << neuron.value << " ";
        }
        ss << episode.neuronActivations.size() << " ";
        for (float val : episode.neuronActivations) {
            ss << val << " ";
        }
    }
    
    // Serialize sleep state
    ss << static_cast<int>(currentSleepState_) << " ";
    
    // Serialize replay statistics
    ss << memoryReplayCount_ << " ";
    ss << consolidationProgress_ << " ";
    ss << schemaFormationProgress_ << " ";
    ss << integrationProgress_ << " ";
    
    // Serialize timers
    ss << quietSleepReplayTimer_ << " " << deepSleepReplayTimer_ << " " << remSleepReplayTimer_ << " ";
    
    return ss.str();
}

bool NeuralEpisodicMemoryWithSleep::deserialize(const std::string& data) {
    std::stringstream ss(data);
    
    size_t episodeCount;
    ss >> episodeCount;
    
    // Clear existing data
    episodes_.clear();
    pImpl->episodeNeurons.clear();
    
    // Deserialize basic parameters
    ss >> maxEpisodes_;
    ss >> replayEnabled_;
    
    // Deserialize all episodes
    for (size_t i = 0; i < episodeCount; ++i) {
        EpisodicMemoryItem episode;
        
        ss >> episode.timestamp;
        ss >> episode.positionX >> episode.positionY >> episode.orientation;
        
        int actionInt;
        ss >> actionInt;
        episode.action = static_cast<ActionType>(actionInt);
        
        ss >> episode.reward >> episode.energy >> episode.novelty;
        ss >> episode.resultingReward;
        ss >> episode.age;
        
        // Deserialize sensory states
        size_t sensorySize;
        ss >> sensorySize;
        episode.sensoryState.resize(sensorySize);
        for (size_t j = 0; j < sensorySize; ++j) {
            ss >> episode.sensoryState[j];
        }
        
        size_t resultingSize;
        ss >> resultingSize;
        episode.resultingSensoryState.resize(resultingSize);
        for (size_t j = 0; j < resultingSize; ++j) {
            ss >> episode.resultingSensoryState[j];
        }
        
        // Deserialize neuron activations
        size_t activeNeuronsSize;
        ss >> activeNeuronsSize;
        episode.activeNeurons.resize(activeNeuronsSize);
        for (size_t j = 0; j < activeNeuronsSize; ++j) {
            ss >> episode.activeNeurons[j].value;
        }
        
        size_t neuronActivationsSize;
        ss >> neuronActivationsSize;
        episode.neuronActivations.resize(neuronActivationsSize);
        for (size_t j = 0; j < neuronActivationsSize; ++j) {
            ss >> episode.neuronActivations[j];
        }
        
        episodes_.push_back(episode);
    }
    
    // Deserialize sleep state
    int stateInt;
    ss >> stateInt;
    currentSleepState_ = static_cast<SleepState>(stateInt);
    
    // Deserialize statistics
    ss >> memoryReplayCount_;
    ss >> consolidationProgress_;
    ss >> schemaFormationProgress_;
    ss >> integrationProgress_;
    
    // Deserialize timers
    ss >> quietSleepReplayTimer_ >> deepSleepReplayTimer_ >> remSleepReplayTimer_;
    
    return !ss.fail();
}

} // namespace nlm
