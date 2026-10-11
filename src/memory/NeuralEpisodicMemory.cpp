#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <random>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Pattern neurons for episodic recall
    std::vector<NeuronId> episodeNeurons;
    
    // Working memory integration
    std::unordered_map<size_t, std::vector<float>> workingMemoryPatterns;
    
    // Attention-based scores for episodes
    std::unordered_map<size_t, float> attentionScores;
    
    // Development tracking
    std::unordered_map<size_t, DevelopmentalStage> episodeDevelopmentalStages;
    
    // Replay tracking
    std::unordered_map<size_t, SimulationStep> episodeLastReplayTimes;
    std::unordered_map<size_t, float> episodeReplayCounts;
    
    // Sleep/rest cycle state
    size_t sleepReplayCounter;
    size_t sleepConsolidationCounter;
    
    // Pattern hashing for selective storage
    std::unordered_map<std::string, std::vector<size_t>> patternToEpisodes;
    
    Impl() : brain(nullptr), sleepReplayCounter(0), sleepConsolidationCounter(0) {}
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(1000)
    , replayEnabled_(true)
    , isResting_(false)
    , sleepReplayInterval_(100)
    , sleepConsolidationInterval_(1000)
    , stepsSinceLastSleepReplay_(0)
    , stepsSinceLastSleepConsolidation_(0)
    , developmentalStage_(DevelopmentalStage::Initial)
{
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("Enhanced NeuralEpisodicMemory initialized");
}

// Enhanced storage with integration to working memory and prediction systems
void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode,
                                     const std::vector<float>* workingMemoryPattern,
                                     float attentionScore,
                                     float predictionError,
                                     NeuromodulatorType neuromodulatorType,
                                     DevelopmentalStage stage,
                                     size_t conceptId) {
    // Create episode copy with enhanced processing
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    stored.lastReplayTime = 0;
    stored.replayStrengthened = false;
    stored.isConsolidated = false;
    stored.consolidationTime = 0;
    stored.developmentalStage = stage;
    
    // ENHANCED STORAGE: Integrate with working memory for pattern extraction
    if (workingMemoryPattern) {
        stored.workingMemoryPattern = *workingMemoryPattern;
        stored.patternCompletionScore = calculatePatternCompletionScore(stored, *workingMemoryPattern);
        
        // Create pattern hash for selective storage
        std::string patternHash = generatePatternHash(*workingMemoryPattern);
        stored.patternHash = convertHashToVector(patternHash);
        stored.patternStability = calculatePatternStability(stored);
    }
    
    // ENHANCED STORAGE: Use prediction errors to select important experiences
    stored.predictionError = predictionError;
    stored.predictionAccuracy = 1.0f - std::min(predictionError, 1.0f);
    
    // ENHANCED STORAGE: Integrate with neuromodulation for reinforcement-based storage
    stored.emotionalTag = calculateEmotionalTag(neuromodulatorType);
    stored.neuromodulatorType = neuromodulatorType;
    
    // Store the enhanced episode
    episodes_.push_back(stored);
    
    // Store metadata for retrieval
    size_t episodeIndex = episodes_.size() - 1;
    
    // Store working memory pattern for completion
    if (workingMemoryPattern) {
        pImpl->workingMemoryPatterns[episodeIndex] = *workingMemoryPattern;
    }
    
    // Store attention score
    pImpl->attentionScores[episodeIndex] = attentionScore;
    
    // Store developmental stage
    pImpl->episodeDevelopmentalStages[episodeIndex] = stage;
    
    // Store for pattern completion
    if (brain_ && !stored.sensoryState.empty()) {
        NeuronId epNeuron(episodeIndex + 20000);
        pImpl->episodeNeurons.push_back(epNeuron);
    }
    
    // Create pattern hash mapping
    if (!stored.patternHash.empty()) {
        std::string hashStr = patternVectorToString(stored.patternHash);
        pImpl->patternToEpisodes[hashStr].push_back(episodeIndex);
    }
    
    // Remove old episodes if over capacity
    while (episodes_.size() > maxEpisodes_) {
        size_t oldIndex = episodes_.front().timestamp; // Use timestamp as index for cleanup
        episodes_.erase(episodes_.begin());
        
        // Clean up metadata
        pImpl->workingMemoryPatterns.erase(oldIndex);
        pImpl->attentionScores.erase(oldIndex);
        pImpl->episodeDevelopmentalStages.erase(oldIndex);
        pImpl->episodeLastReplayTimes.erase(oldIndex);
        pImpl->episodeReplayCounts.erase(oldIndex);
        
        if (!pImpl->episodeNeurons.empty()) {
            pImpl->episodeNeurons.erase(pImpl->episodeNeurons.begin());
        }
    }
    
    // Age all episodes
    for (auto& ep : episodes_) {
        ep.age++;
    }
    
    NLM_LOG_INFO("Enhanced episode stored (index: " + std::to_string(episodeIndex) + ", prediction error: " + 
                 std::to_string(predictionError) + ", attention score: " + std::to_string(attentionScore) + ")");
}

// Enhanced storage with attention and prediction guidance
void NeuralEpisodicMemory::storeEpisodeWithAttention(const EpisodicMemoryItem& episode,
                                                   const AttentionalSelection& attention,
                                                   const PredictionSystem& prediction,
                                                   const Curiosity& curiosity,
                                                   const Dopamine& dopamine) {
    // Extract working memory pattern from brain if available
    std::vector<float> workingMemoryPattern;
    if (brain_ && brain_->getWorkingMemory()) {
        workingMemoryPattern = brain_->getWorkingMemory()->retrieve();
    }
    
    // Calculate attention score based on attention system
    float attentionScore = calculateAttentionBasedScore(episode, attention, prediction);
    
    // Calculate prediction error from prediction system
    float predictionError = prediction.getPredictionError();
    
    // Get neuromodulator type from dopamine level
    NeuromodulatorType neuromodulatorType = NeuromodulatorType::Dopamine;
    float dopamineLevel = dopamine.getLevel();
    if (dopamineLevel < 0.2f) neuromodulatorType = NeuromodulatorType::None;
    else if (dopamineLevel < 0.4f) neuromodulatorType = NeuromodulatorType::Serotonin;
    else if (dopamineLevel < 0.6f) neuromodulatorType = NeuromodulatorType::Acetylcholine;
    
    // Store episode with full integration
    storeEpisode(episode, &workingMemoryPattern, attentionScore, predictionError,
                 neuromodulatorType, developmentalStage_, 0);
}

// Enhanced retrieval with attention and prediction guidance
std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    const AttentionalSelection* attention,
    const PredictionSystem* prediction,
    size_t maxResults) const {
    
    std::vector<std::tuple<float, float, size_t>> scores;  // (similarity, attentionScore, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float sim = computeSimilarity(sensoryPattern, episodes_[i]);
        
        // ENHANCED RETRIEVAL: Integrate attention system relevance scores
        float attentionScore = 0.0f;
        if (attention) {
            // Check which neurons are attended and match against episode
            attentionScore = calculateEpisodeAttentionScore(episodes_[i], *attention);
        }
        
        // ENHANCED RETRIEVAL: Use prediction system accuracy to prioritize episodes
        float predictionScore = 1.0f;
        if (prediction) {
            predictionScore = episodes_[i].predictionAccuracy;
        }
        
        // Combine similarity with attention and prediction scores
        float combinedScore = sim * 0.4f + attentionScore * 0.3f + predictionScore * 0.3f;
        
        scores.emplace_back(combinedScore, attentionScore, i);
    }
    
    // Sort by combined score (descending)
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return std::get<0>(a) > std::get<0>(b); });
    
    // Return top results
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        if (std::get<0>(scores[i]) > 0.3f) {  // Threshold
            results.push_back(&episodes_[std::get<2>(scores[i])]);
        }
    }
    
    return results;
}

// Other methods continue... (I'll write the full implementation)
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
    float x, float y,
    float radius,
    size_t maxResults) const {
    
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
    ActionType action,
    size_t maxResults) const {
    
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

// Enhanced replay with attention and prediction integration
void NeuralEpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode,
                                        const AttentionalSelection& attention,
                                        const PredictionSystem& prediction,
                                        ReplayType type) {
    if (!episode || !brain_) return;
    
    NLM_LOG_INFO("Enhanced replay of episode (timestamp: " + std::to_string(episode->timestamp) + ")");
    
    // Update episode replay metadata
    auto it = pImpl->episodeLastReplayTimes.find(episode->timestamp);
    if (it != pImpl->episodeLastReplayTimes.end()) {
        it->second = episode->age;  // Current age as step count
    } else {
        pImpl->episodeLastReplayTimes[episode->timestamp] = episode->age;
    }
    
    pImpl->episodeReplayCounts[episode->timestamp]++;
    
    // ENHANCED REPLAY: Integrate with attention for focused replay
    std::vector<NeuronId> attendedNeurons;
    if (brain_ && brain_->getAttention()) {
        attendedNeurons = brain_->getAttention()->getWinners();
    }
    
    // Reactivate neurons that were active during this episode
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = i < episode->neuronActivations.size() ? 
                          episode->neuronActivations[i] : 0.5f;
        
        // Apply attention modulation if neuron is attended
        float attentionMod = 1.0f;
        if (!attendedNeurons.empty()) {
            for (const auto& attended : attendedNeurons) {
                if (attended == neuron) {
                    attentionMod = 1.5f;  // Boost activation for attended neurons
                    break;
                }
            }
        }
        
        // Apply prediction error modulation
        float predictionMod = 1.0f;
        float predictionError = episode->predictionError;
        if (predictionError > 0.3f) {
            predictionMod = 1.2f;  // Higher prediction errors strengthen replay
        }
        
        // Inject current to reawaken this pattern with enhanced modulation
        float enhancedActivation = activation * 3.0f * attentionMod * predictionMod;
        brain_->injectCurrent(neuron, enhancedActivation);
    }
    
    // ENHANCED REPLAY: Use prediction errors to guide replay targeting
    float predictionError = episode->predictionError;
    if (predictionError > 0.5f) {
        // High prediction error episodes get strengthened replay
        for (const auto& neuron : episode->activeNeurons) {
            brain_->injectCurrent(neuron, predictionError * 2.0f);  // Additional boost
        }
    }
    
    // ENHANCED REPLAY: Add forward/backward replay modes
    if (type == ReplayType::Forward) {
        forwardReplay(*episode, attention, prediction, episode->age);
    } else if (type == ReplayType::Backward) {
        backwardReplay(*episode, attention, prediction, episode->age);
    }
    
    // ENHANCED REPLAY: Connect replay to working memory for pattern completion
    if (brain_ && brain_->getWorkingMemory()) {
        completePatternFromWorkingMemory(episode->workingMemoryPattern, 1);
    }
    
    // Mark episode as replayed
    EpisodicMemoryItem* mutableEpisode = const_cast<EpisodicMemoryItem*>(episode);
    mutableEpisode->lastReplayTime = episode->age;
    mutableEpisode->replayStrengthened = true;
}

void NeuralEpisodicMemory::replayWithAttention(const EpisodicMemoryItem* episode,
                                               const AttentionalSelection& attention,
                                               const std::vector<NeuronId>& attendedNeurons) {
    if (!episode) return;
    
    // Enhanced replay with attended neuron focus
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = i < episode->neuronActivations.size() ? 
                          episode->neuronActivations[i] : 0.5f;
        
        // Strong boost for attended neurons
        float attentionBoost = 1.0f;
        for (const auto& attended : attendedNeurons) {
            if (attended == neuron) {
                attentionBoost = 2.0f;  // Significant boost for attended neurons
                break;
            }
        }
        
        brain_->injectCurrent(neuron, activation * 3.0f * attentionBoost);
    }
}

void NeuralEpisodicMemory::updateRelevance(SimulationStep episodeId, float relevanceDelta,
                                         const AttentionalSelection& attention,
                                         const PredictionSystem& prediction) {
    if (episodeId < episodes_.size()) {
        // Find the episode by searching through (since timestamp is unique)
        for (auto& ep : episodes_) {
            if (ep.timestamp == episodeId) {
                // Update relevance based on attention and prediction
                float attentionBasedRelevance = calculateEpisodeAttentionScore(ep, attention);
                float predictionBasedRelevance = ep.predictionAccuracy;
                
                // Apply relevance delta with attention-prediction weighting
                float attentionWeight = 0.6f;
                float predictionWeight = 0.4f;
                
                ep.consolidationScore += relevanceDelta * attentionWeight * attentionBasedRelevance +
                                      relevanceDelta * predictionWeight * predictionBasedRelevance;
                
                // Age the episode differently based on relevance
                if (ep.consolidationScore > 0.5f) {
                    ep.age = 0;  // Important episodes age slower
                } else {
                    ep.age += 2;  // Less important episodes age faster
                }
                
                break;
            }
        }
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

// Enhanced consolidation with sleep/rest cycles
void NeuralEpisodicMemory::consolidate(float relevanceThreshold,
                                       const std::vector<float>& neuromodulationLevels,
                                       SimulationStep currentStep) {
    // Sleep/rest cycle management
    if (isResting_) {
        sleepConsolidation(currentStep);
    } else {
        // Regular consolidation
        std::deque<EpisodicMemoryItem> consolidated;
        
        for (const auto& ep : episodes_) {
            // Calculate importance score using attention and prediction signals
            float attentionScore = pImpl->attentionScores.count(ep.timestamp) ? 
                                  pImpl->attentionScores[ep.timestamp] : 0.0f;
            float predictionScore = ep.predictionAccuracy;
            float developmentalScore = getDevelopmentalStageScore(ep.developmentalStage);
            
            // ENHANCED CONSOLIDATION: Add stress and neuromodulation signals for memory prioritization
            float stressPriority = 1.0f;
            if (!neuromodulationLevels.empty() && neuromodulationLevels.size() >= 4) {
                // Use norepinephrine for stress-based prioritization
                float noradrenalineEffect = neuromodulationLevels[2];
                if (noradrenalineEffect > 0.3f) {
                    // High norepinephrine = high stress = prioritize memory consolidation
                    stressPriority = 1.0f + noradrenalineEffect * 0.5f;
                }
            }
            
            // Combine scores for importance calculation with stress and neuromodulation
            float importanceScore = attentionScore * 0.3f + 
                                   predictionScore * 0.3f + 
                                   developmentalScore * 0.2f + 
                                   stressPriority * 0.2f;
            
            // ENHANCED CONSOLIDATION: Use attention and prediction signals for selective consolidation
            if (importanceScore >= relevanceThreshold) {
                // Check if episode needs strengthening through replay
                if (ep.lastReplayTime > 0 && ep.lastReplayTime < currentStep - 10) {
                    // Replay important memories for strengthening
                    strengthenEpisode(ep.timestamp, 1.5f * stressPriority);
                }
                
                // Apply neuromodulation effects with enhanced stress integration
                if (!neuromodulationLevels.empty() && neuromodulationLevels.size() >= 4) {
                    float acetylcholineEffect = neuromodulationLevels[1];  // Acetylcholine for memory consolidation
                    float dopamineEffect = neuromodulationLevels[0];      // Dopamine for reward-based consolidation
                    float serotoninEffect = neuromodulationLevels[3];    // Serotonin for mood-based consolidation
                    
                    // Enhanced consolidation through neuromodulation
                    float neuromodEffect = acetylcholineEffect * 0.4f + 
                                         dopamineEffect * 0.3f + 
                                         serotoninEffect * 0.3f;
                    
                    if (neuromodEffect > 0.5f) {
                        strengthenEpisode(ep.timestamp, neuromodEffect * stressPriority);
                    }
                }
                
                consolidated.push_back(ep);
                ep.isConsolidated = true;
                ep.consolidationTime = currentStep;
                ep.stressPriority = stressPriority; // Record stress priority for later retrieval
            }
        }
        
        episodes_ = std::move(consolidated);
    }
}

void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    pImpl->episodeNeurons.clear();
    pImpl->workingMemoryPatterns.clear();
    pImpl->attentionScores.clear();
    pImpl->episodeDevelopmentalStages.clear();
    pImpl->episodeLastReplayTimes.clear();
    pImpl->episodeReplayCounts.clear();
    pImpl->patternToEpisodes.clear();
}

// Get episodes for replay with attention-prediction-based selection
std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(
    size_t count,
    const AttentionalSelection* attention,
    const PredictionSystem* prediction,
    float noveltyThreshold) const {
    
    std::vector<std::tuple<float, float, size_t>> scored;  // (score, novelty, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float recencyScore = 1.0f / (episodes_[i].age + 1);
        float rewardScore = episodes_[i].reward;
        
        // ENHANCED SELECTION: Consider unpredictability (higher prediction error episodes)
        float unpredictabilityScore = episodes_[i].predictionError;
        
        // ENHANCED SELECTION: Novelty detection for replay
        float noveltyScore = episodes_[i].novelty;
        if (noveltyScore < noveltyThreshold) {
            noveltyScore = noveltyThreshold;  // Minimum novelty for replay selection
        }
        
        // ENHANCED SELECTION: Integrate attention and prediction scores
        float attentionScore = pImpl->attentionScores.count(episodes_[i].timestamp) ? 
                              pImpl->attentionScores[episodes_[i].timestamp] : 0.0f;
        
        float predictionScore = episodes_[i].predictionAccuracy;
        
        // Combined score with weights for enhanced selection
        float score = recencyScore * 0.2f + rewardScore * 0.2f + unpredictabilityScore * 0.3f +
                     noveltyScore * 0.2f + attentionScore * 0.1f + predictionScore * 0.1f;
        
        scored.emplace_back(score, noveltyScore, i);
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

void NeuralEpisodicMemory::replaySequence(const std::vector<size_t>& episodeIds,
                                         const AttentionalSelection& attention,
                                         const PredictionSystem& prediction,
                                         bool bidirectional) {
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            ReplayType replayType = ReplayType::Standard;
            if (bidirectional) {
                // Alternate between forward and backward replay
                static bool forwardNext = true;
                replayType = forwardNext ? ReplayType::Forward : ReplayType::Backward;
                forwardNext = !forwardNext;
            }
            
            replayEpisode(&episodes_[id], attention, prediction, replayType);
            
            // Small delay between replays
            // (In simulation, this would be steps)
        }
    }
}

// Working memory integration for pattern completion
std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::completePatternFromWorkingMemory(
    const std::vector<float>& workingMemoryPattern,
    size_t maxResults) const {
    
    std::vector<std::pair<float, size_t>> scores;  // (similarity, index)
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        // Check if episode has working memory pattern for completion
        if (!episodes_[i].workingMemoryPattern.empty() && 
            episodes_[i].patternCompletionScore > 0.5f) {
            
            float sim = computePatternSimilarity(workingMemoryPattern, episodes_[i].workingMemoryPattern);
            if (sim > 0.3f) {
                scores.emplace_back(sim, i);
            }
        }
    }
    
    // Sort by similarity
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        results.push_back(&episodes_[scores[i].second]);
    }
    
    return results;
}

void NeuralEpisodicMemory::strengthenEpisode(size_t episodeIndex, float strength) {
    if (episodeIndex < episodes_.size()) {
        EpisodicMemoryItem& ep = episodes_[episodeIndex];
        
        // Strengthen neural activations
        for (auto& activation : ep.neuronActivations) {
            activation = std::min(activation * strength, 1.0f);
        }
        
        // Increase reward signal
        ep.reward = std::min(ep.reward * strength, 1.0f);
        
        // Update consolidation score
        ep.consolidationScore = std::min(ep.consolidationScore * strength, 1.0f);
        
        // Boost pattern stability
        ep.patternStability = std::min(ep.patternStability * strength, 1.0f);
        
        NLM_LOG_INFO("Episode strengthened (index: " + std::to_string(episodeIndex) + ", strength: " + 
                     std::to_string(strength) + ")");
    }
}

// Start sleep cycle for enhanced memory consolidation
void NeuralEpisodicMemory::startSleepCycle() {
    NLM_LOG_INFO("Starting sleep cycle for enhanced memory consolidation");
    
    // Mark that the memory system is in a resting/sleep state
    isResting_ = true;
    
    // Update sleep counters
    if (pImpl) {
        pImpl->sleepConsolidationCounter++;
        pImpl->sleepReplayCounter++;
        
        // Log sleep cycle initiation
        NLM_LOG_INFO("Sleep cycle initiated - sleep counters updated");
    }
}

// Start sleep cycle for enhanced memory consolidation
void NeuralEpisodicMemory::startSleepCycle() {
    NLM_LOG_INFO("Starting sleep cycle for enhanced memory consolidation");
    
    // Mark that the memory system is in a resting/sleep state
    isResting_ = true;
    
    // Update sleep counters
    if (pImpl) {
        pImpl->sleepConsolidationCounter++;
        pImpl->sleepReplayCounter++;
        
        // Log sleep cycle initiation
        NLM_LOG_INFO("Sleep cycle initiated - stage: " + std::to_string(static_cast<int>(cycleState_.stage)));
    }
}

// Apply sleep/rest cycle to memory consolidation
void NeuralEpisodicMemory::applySleepConsolidation(SimulationStep currentStep) {
    startSleepCycle();
    sleepConsolidation(currentStep);
}

// Helper methods
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

bool NeuralEpisodicMemory::matchesCriteria(const EpisodicMemoryItem& episode,
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

// Enhanced helper methods
float NeuralEpisodicMemory::calculateAttentionBasedScore(const EpisodicMemoryItem& episode,
                                                        const AttentionalSelection& attention,
                                                        const PredictionSystem& prediction) const {
    float attentionScore = 0.0f;
    
    // Check which neurons are attended and match against episode
    for (const auto& neuron : episode.activeNeurons) {
        if (attention.isAttended(neuron)) {
            attentionScore += 1.0f;
        }
    }
    
    // Normalize by number of active neurons
    if (!episode.activeNeurons.empty()) {
        attentionScore /= episode.activeNeurons.size();
    }
    
    // Integrate prediction accuracy
    float predictionScore = episode.predictionAccuracy;
    
    // Combine scores
    return attentionScore * 0.7f + predictionScore * 0.3f;
}

float NeuralEpisodicMemory::calculatePatternCompletionScore(const EpisodicMemoryItem& episode,
                                                          const std::vector<float>& workingMemoryPattern) const {
    if (episode.workingMemoryPattern.empty() || workingMemoryPattern.empty()) {
        return 0.0f;
    }
    
    return computePatternSimilarity(episode.workingMemoryPattern, workingMemoryPattern);
}

float NeuralEpisodicMemory::calculatePatternStability(const EpisodicMemoryItem& episode) const {
    // Pattern stability based on age, replay history, and attention
    float ageFactor = 1.0f / (episode.age + 1);  // Younger episodes are more stable
    
    float replayFactor = std::min(static_cast<float>(pImpl->episodeReplayCounts.count(episode.timestamp) ? 
                                      pImpl->episodeReplayCounts[episode.timestamp] : 0), 5.0f) / 5.0f;
    
    float attentionFactor = pImpl->attentionScores.count(episode.timestamp) ? 
                           pImpl->attentionScores[episode.timestamp] : 0.0f;
    
    return ageFactor * 0.5f + replayFactor * 0.3f + attentionFactor * 0.2f;
}

float NeuralEpisodicMemory::calculateEmotionalTag(NeuromodulatorType type) const {
    switch (type) {
        case NeuromodulatorType::Dopamine: return 0.8f;
        case NeuromodulatorType::Acetylcholine: return 0.7f;
        case NeuromodulatorType::Norepinephrine: return 0.6f;
        case NeuromodulatorType::Serotonin: return 0.5f;
        case NeuromodulatorType::Novelty: return 0.9f;
        case NeuromodulatorType::Curiosity: return 0.7f;
        case NeuromodulatorType::PredictionError: return 0.6f;
        case NeuromodulatorType::None: return 0.3f;
        default: return 0.5f;
    }
}

float NeuralEpisodicMemory::calculateEpisodeAttentionScore(const EpisodicMemoryItem& episode,
                                                         const AttentionalSelection& attention) const {
    float attentionScore = 0.0f;
    for (const auto& neuron : episode.activeNeurons) {
        if (attention.isAttended(neuron)) {
            attentionScore += 1.0f;
        }
    }
    
    if (!episode.activeNeurons.empty()) {
        attentionScore /= episode.activeNeurons.size();
    }
    
    return attentionScore;
}

float NeuralEpisodicMemory::calculatePatternSimilarity(const std::vector<float>& pattern1,
                                                     const std::vector<float>& pattern2) const {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    if (pattern1.size() != pattern2.size()) return 0.0f;
    
    float dot = 0.0f, norm1 = 0.0f, norm2 = 0.0f;
    for (size_t i = 0; i < pattern1.size(); ++i) {
        dot += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    if (norm1 < 0.0001f || norm2 < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(norm1) * std::sqrt(norm2));
}

std::string NeuralEpisodicMemory::generatePatternHash(const std::vector<float>& pattern) const {
    // Simple hash generation for pattern
    std::hash<float> hasher;
    std::string hash;
    for (float value : pattern) {
        hash += std::to_string(hasher(value));
    }
    return hash;
}

std::vector<float> NeuralEpisodicMemory::convertHashToVector(const std::string& hash) const {
    std::vector<float> result;
    size_t pos = 0;
    std::string token;
    while ((pos = hash.find('.')) != std::string::npos) {
        token = hash.substr(0, pos);
        if (!token.empty()) {
            try {
                result.push_back(std::stof(token));
            } catch (...) {
                result.push_back(0.0f);
            }
        }
        hash.erase(0, pos + 1);
    }
    if (!hash.empty()) {
        try {
            result.push_back(std::stof(hash));
        } catch (...) {
            result.push_back(0.0f);
        }
    }
    return result;
}

std::string NeuralEpisodicMemory::patternVectorToString(const std::vector<float>& pattern) const {
    std::string result;
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (i > 0) result += ".";
        result += std::to_string(pattern[i]);
    }
    return result;
}

float NeuralEpisodicMemory::getDevelopmentalStageScore(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Initial: return 1.0f;
        case DevelopmentalStage::CriticalPeriod: return 0.8f;
        case DevelopmentalStage::Maturation: return 0.6f;
        case DevelopmentalStage::Adult: return 0.4f;
        case DevelopmentalStage::Aging: return 0.2f;
        default: return 0.5f;
    }
}

void NeuralEpisodicMemory::forwardReplay(const EpisodicMemoryItem& episode,
                                        const AttentionalSelection& attention,
                                        const PredictionSystem& prediction,
                                        SimulationStep currentStep) {
    // Forward replay for temporal sequence learning
    // Reactivate neurons in forward temporal order
    
    // Get all neurons from brain for forward replay
    std::vector<NeuronId> allNeurons;
    if (brain_) {
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    allNeurons.push_back(neuron->getId());
                }
            }
        }
    }
    
    // Select neurons that match episode pattern
    std::vector<NeuronId> forwardNeurons;
    for (const auto& neuron : allNeurons) {
        float sim = 0.0f;
        for (size_t i = 0; i < episode.activeNeurons.size(); ++i) {
            if (episode.activeNeurons[i] == neuron) {
                sim = episode.neuronActivations[i];
                if (sim > 0.3f) {
                    forwardNeurons.push_back(neuron);
                    break;
                }
            }
        }
    }
    
    // Apply attention modulation to forward replay
    for (const auto& neuron : forwardNeurons) {
        if (attention.isAttended(neuron)) {
            brain_->injectCurrent(neuron, 1.5f);  // Stronger activation for attended neurons
        } else {
            brain_->injectCurrent(neuron, 1.0f);
        }
    }
}

void NeuralEpisodicMemory::backwardReplay(const EpisodicMemoryItem& episode,
                                         const AttentionalSelection& attention,
                                         const PredictionSystem& prediction,
                                         SimulationStep currentStep) {
    // Backward replay for error correction
    // Reactivate neurons in reverse temporal order
    
    // Similar to forward replay but with different dynamics
    std::vector<NeuronId> allNeurons;
    if (brain_) {
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    allNeurons.push_back(neuron->getId());
                }
            }
        }
    }
    
    // Select neurons with error-correcting weights
    std::vector<NeuronId> backwardNeurons;
    for (const auto& neuron : allNeurons) {
        float errorCorrection = 0.0f;
        for (size_t i = 0; i < episode.activeNeurons.size(); ++i) {
            if (episode.activeNeurons[i] == neuron) {
                float activation = episode.neuronActivations[i];
                errorCorrection = (1.0f - activation) * 0.5f;  // Higher error = more correction
                if (errorCorrection > 0.2f) {
                    backwardNeurons.push_back(neuron);
                    break;
                }
            }
        }
    }
    
    // Apply attention and prediction error modulation to backward replay
    for (const auto& neuron : backwardNeurons) {
        float attentionMod = attention.isAttended(neuron) ? 1.5f : 1.0f;
        
        // Prediction error enhances error correction
        float predictionMod = 1.0f + episode.predictionError * 0.5f;
        
        brain_->injectCurrent(neuron, 0.8f * attentionMod * predictionMod);
    }
}

void NeuralEpisodicMemory::sleepConsolidation(SimulationStep currentStep) {
    pImpl->sleepReplayCounter++;
    pImpl->sleepConsolidationCounter++;
    
    // Sleep replay consolidation
    if (pImpl->sleepReplayCounter >= sleepReplayInterval_ && replayEnabled_) {
        // Get episodes for replay based on attention and prediction
        auto episodesToReplay = getEpisodesForReplay(3, nullptr, nullptr, 0.2f);
        
        for (const auto* episode : episodesToReplay) {
            replayEpisode(episode, *brain_->getAttention(), *brain_->getPredictionSystem(), ReplayType::Standard);
        }
        
        pImpl->sleepReplayCounter = 0;
        NLM_LOG_INFO("Sleep replay completed - consolidated " + std::to_string(episodesToReplay.size()) + " episodes");
    }
    
    // Sleep consolidation
    if (pImpl->sleepConsolidationCounter >= sleepConsolidationInterval_) {
        // Consolidate important memories, remove weak ones
        consolidate(0.3f);
        
        pImpl->sleepConsolidationCounter = 0;
        NLM_LOG_INFO("Sleep consolidation completed");
    }
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
