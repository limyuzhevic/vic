#include "NeuralEpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>
#include <iomanip>

namespace nlm {

struct NeuralEpisodicMemory::Impl {
    Brain* brain;
    
    // Enhanced episode storage with neural representations
    std::deque<EpisodicMemoryItem> episodes_;
    
    // Hippocampal pattern storage (CA3)
    std::vector<NeuronId> hippocampalPatternNeurons_;
    
    // Neocortical trace storage
    std::vector<NeuronId> neocorticalPatternNeurons_;
    
    // Synaptic tagging system for consolidation
    std::vector<std::pair<NeuronId, SimulationStep>> taggedSynapses_;
    std::unordered_map<uint64_t, float> synapticTags_;
    
    // Replay queue for consolidation
    std::priority_queue<size_t> replayQueue_;
    SimulationStep nextReplayStep_;
    
    // Neuromodulation integration
    class Dopamine;
    Dopamine* dopamine_;
    
    // Working memory integration
    class NeuralWorkingMemory;
    NeuralWorkingMemory* workingMemory_;
    
    // Consolidation tracking
    std::vector<float> episodeRelevance_;
    std::vector<size_t> episodeReplayCount_;
    
    // Stats
    size_t totalEpisodesEncoded_;
    size_t totalReplaysPerformed_;
    float averageRelevanceScore_;
    
    Impl() 
        : brain(nullptr)
        , dopamine_(nullptr)
        , workingMemory_(nullptr)
        , totalEpisodesEncoded_(0)
        , totalReplaysPerformed_(0)
        , averageRelevanceScore_(0.0f)
        , nextReplayStep_(0) {
        
        // Initialize containers
        episodes_.reserve(5000);
        hippocampalPatternNeurons_.reserve(5000);
        neocorticalPatternNeurons_.reserve(20000);
        taggedSynapses_.reserve(1000);
        synapticTags_.reserve(1000);
        episodeRelevance_.reserve(5000);
        episodeReplayCount_.reserve(5000);
    }
};

NeuralEpisodicMemory::NeuralEpisodicMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , maxEpisodes_(1000)
    , replayEnabled_(true)
{
    std::ostringstream oss;
    oss << "NeuralEpisodicMemory initialized with enhanced episodic memory system";
    NLM_LOG_INFO(oss.str());
}

NeuralEpisodicMemory::~NeuralEpisodicMemory() = default;

void NeuralEpisodicMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Get neuromodulators
    if (brain) {
        pImpl->dopamine_ = brain->getDopamine();
        pImpl->workingMemory_ = brain->getWorkingMemory();
    }
    
    std::ostringstream oss;
    oss << "NeuralEpisodicMemory initialized with neural episodic memory system";
    NLM_LOG_INFO(oss.str());
}

void NeuralEpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Create episode copy with age 0
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    
    // Create hippocampal pattern (CA3 encoding)
    if (!episode.sensoryState.empty() && brain_) {
        std::vector<float> pattern = episode.sensoryState;
        
        // Add positional and contextual encoding
        size_t pos_idx = pattern.size() + 1;
        if (pos_idx < pattern.size() + 10) {
            pattern.resize(pattern.size() + 10, 0.0f);
        }
        
        pattern[0] = episode.positionX;
        pattern[1] = episode.positionY;
        pattern[2] = episode.orientation;
        pattern[3] = static_cast<float>(episode.action) / 10.0f;
        pattern[4] = std::tanh(episode.reward);
        
        NeuronId hipNeuron(30000 + pImpl->hippocampalPatternNeurons_.size());
        pImpl->hippocampalPatternNeurons_.push_back(hipNeuron);
        
        stored.activeNeurons.push_back(hipNeuron);
        stored.neuronActivations.push_back(1.0f);
    }
    
    // Create neocortical trace (slow consolidation)
    if (!episode.sensoryState.empty() && brain_) {
        std::vector<float> trace = episode.sensoryState;
        
        size_t base_idx = trace.size();
        trace.resize(base_idx + 5, 0.0f);
        
        trace[0] = episode.novelty;
        trace[1] = episode.energy;
        trace[2] = episode.resultingReward - episode.reward;
        trace[3] = std::sqrt(episode.positionX * episode.positionX + episode.positionY * episode.positionY);
        trace[4] = std::cos(episode.orientation);
        
        NeuronId neoNeuron(50000 + pImpl->neocorticalPatternNeurons_.size());
        pImpl->neocorticalPatternNeurons_.push_back(neoNeuron);
        
        stored.activeNeurons.push_back(neoNeuron);
        stored.neuronActivations.push_back(1.0f);
    }
    
    // Store in episode buffer
    episodes_.push_back(stored);
    pImpl->totalEpisodesEncoded_++;
    
    // Initialize relevance tracking
    pImpl->episodeRelevance_.push_back(0.0f);
    pImpl->episodeReplayCount_.push_back(0);
    
    // Create synaptic tag for consolidation eligibility
    createSynapticTagForEpisode(stored);
    
    // Update working memory with episode context
    if (pImpl->workingMemory_) {
        updateWorkingMemoryWithEpisode(stored);
    }
    
    // Enforce capacity limits
    while (episodes_.size() > maxEpisodes_) {
        size_t erased_index = 0;
        episodes_.erase(episodes_.begin());
        erased_index = 0;
        
        if (!pImpl->hippocampalPatternNeurons_.empty()) {
            pImpl->hippocampalPatternNeurons_.erase(pImpl->hippocampalPatternNeurons_.begin());
        }
        
        if (!pImpl->neocorticalPatternNeurons_.empty()) {
            pImpl->neocorticalPatternNeurons_.erase(pImpl->neocorticalPatternNeurons_.begin());
        }
        
        if (!pImpl->episodeRelevance_.empty()) {
            pImpl->episodeRelevance_.erase(pImpl->episodeRelevance_.begin());
        }
        
        if (!pImpl->episodeReplayCount_.empty()) {
            pImpl->episodeReplayCount_.erase(pImpl->episodeReplayCount_.begin());
        }
    }
    
    // Age all episodes and update relevance
    for (auto& ep : episodes_) {
        ep.age++;
        updateEpisodeRelevance(ep);
    }
    
    std::ostringstream oss;
    oss << "Stored episode: timestamp=" << stored.timestamp << ", reward=" << std::fixed << std::setprecision(3) << stored.reward << ", sensory_dim=" << stored.sensoryState.size();
    NLM_LOG_DEBUG(oss.str());
}

void NeuralEpisodicMemory::createSynapticTagForEpisode(const EpisodicMemoryItem& episode) {
    if (!brain_ || episode.activeNeurons.empty()) return;
    
    // Calculate tag strength based on episode properties
    float tagStrength = episode.reward * 0.5f + episode.novelty * 0.3f + episode.energy * 0.2f;
    
    // Create tags for episode-related neurons
    for (const NeuronId& neuronId : episode.activeNeurons) {
        uint64_t synapseId = neuronId.value * 1000 + episode.timestamp;
        
        pImpl->synapticTags_[synapseId] = tagStrength;
        pImpl->taggedSynapses_.push_back({INVALID_SYNAPSE_ID, episode.timestamp});
    }
    
    // Apply neuromodulatory effects during encoding
    if (pImpl->dopamine_ && episode.reward > 0) {
        pImpl->dopamine_->setLevel(std::min(1.0f, pImpl->dopamine_->getLevel() + 0.1f * episode.reward));
    }
}

void NeuralEpisodicMemory::updateWorkingMemoryWithEpisode(const EpisodicMemoryItem& episode) {
    if (!pImpl->workingMemory_) return;
    
    float integrationWeight = episode.novelty * 0.5f + episode.reward * 0.5f;
    
    if (integrationWeight > 0.3f && !episode.sensoryState.empty()) {
        pImpl->workingMemory_->store(episode.sensoryState, integrationWeight);
        
        if (episode.novelty > 0.7f || episode.reward > 0.5f) {
            std::ostringstream oss;
            oss << "Episode triggered attentional focus: reward=" << std::fixed << std::setprecision(3) << episode.reward << ", novelty=" << std::fixed << std::setprecision(3) << episode.novelty;
            NLM_LOG_DEBUG(oss.str());
        }
    }
}

void NeuralEpisodicMemory::updateEpisodeRelevance(const EpisodicMemoryItem& episode) {
    float relevance = 1.0f;
    
    // Recency factor - older episodes decay in relevance
    relevance *= std::exp(-0.01f * episode.age);
    
    // Replay count factor - more replay increases relevance
    size_t replayIndex = episode.timestamp % pImpl->episodeReplayCount_.size();
    if (replayIndex < pImpl->episodeReplayCount_.size()) {
        relevance *= std::min(2.0f, 1.0f + 0.1f * pImpl->episodeReplayCount_[replayIndex]);
    }
    
    // Neuromodulatory influence
    if (pImpl->dopamine_) {
        float rewardFactor = pImpl->dopamine_->getLevel();
        relevance *= (1.0f + rewardFactor * 0.5f);
    }
    
    // Clamp relevance
    relevance = std::min(1.0f, std::max(0.0f, relevance));
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveSimilar(
    const std::vector<float>& sensoryPattern,
    size_t maxResults) const {
    
    std::vector<std::pair<float, size_t>> scores;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float sim = computeSimilarity(sensoryPattern, episodes_[i]);
        if (sim > 0.1f) {
            scores.emplace_back(sim, i);
        }
    }
    
    std::sort(scores.begin(), scores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    std::vector<const EpisodicMemoryItem*> results;
    for (size_t i = 0; i < std::min(maxResults, scores.size()); ++i) {
        if (scores[i].first > 0.3f) {
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
    
    std::sort(results.begin(), results.end(),
        [this](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            return getEpisodeRelevance(a) > getEpisodeRelevance(b);
        });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::retrieveByLocation(
    float x, float y, float radius, size_t maxResults) const {
    
    std::vector<std::pair<float, size_t>> distances;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float dx = episodes_[i].positionX - x;
        float dy = episodes_[i].positionY - y;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist <= radius) {
            distances.emplace_back(dist, i);
        }
    }
    
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
    
    std::sort(results.begin(), results.end(),
        [this](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            return getEpisodeRelevance(a) > getEpisodeRelevance(b);
        });
    
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

void NeuralEpisodicMemory::replayEpisode(const EpisodicMemoryItem* episode) {
    if (!episode || !brain_ || !replayEnabled_) return;
    
    for (size_t i = 0; i < episode->activeNeurons.size(); ++i) {
        NeuronId neuron = episode->activeNeurons[i];
        float activation = i < episode->neuronActivations.size() ? 
                          episode->neuronActivations[i] : 0.5f;
        
        float replayStrength = activation * 2.0f;
        
        brain_->injectCurrent(neuron, replayStrength);
    }
    
    size_t episodeIndex = getEpisodeIndex(episode);
    if (episodeIndex < pImpl->episodeReplayCount_.size()) {
        pImpl->episodeReplayCount_[episodeIndex]++;
        pImpl->totalReplaysPerformed_++;
    }
    
    std::ostringstream oss;
    oss << "Replaying episode: timestamp=" << episode->timestamp << ", neurons_activated=" << episode->activeNeurons.size();
    NLM_LOG_DEBUG(oss.str());
}

void NeuralEpisodicMemory::updateRelevance(SimulationStep episodeId, float relevanceDelta) {
    if (episodeId < episodes_.size()) {
        size_t index = episodeId % pImpl->episodeRelevance_.size();
        if (index < pImpl->episodeRelevance_.size()) {
            pImpl->episodeRelevance_[index] = 
                std::min(1.0f, pImpl->episodeRelevance_[index] + relevanceDelta);
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
    
    std::sort(results.begin(), results.end(),
        [](const EpisodicMemoryItem* a, const EpisodicMemoryItem* b) {
            return a->timestamp > b->timestamp;
        });
    
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
    consolidateSynapticTags(relevanceThreshold);
    
    std::deque<EpisodicMemoryItem> newEpisodes;
    std::vector<float> newRelevance;
    std::vector<size_t> newReplayCounts;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float relevance = getEpisodeRelevance(&episodes_[i]);
        
        if (relevance >= relevanceThreshold || 
            episodes_[i].age < 100 || 
            episodes_[i].reward > 0.5f) {
            
            newEpisodes.push_back(episodes_[i]);
            newRelevance.push_back(pImpl->episodeRelevance_[i]);
            newReplayCounts.push_back(pImpl->episodeReplayCount_[i]);
        } else {
            removeHippocampalTrace(i);
        }
    }
    
    episodes_ = std::move(newEpisodes);
    pImpl->episodeRelevance_ = std::move(newRelevance);
    pImpl->episodeReplayCount_ = std::move(newReplayCounts);
    
    pImpl->hippocampalPatternNeurons_.resize(episodes_.size());
    pImpl->neocorticalPatternNeurons_.resize(episodes_.size());
    
    std::ostringstream oss;
    oss << "Consolidated episodic memory: " << episodes_.size() << " episodes remain (threshold=" << std::fixed << std::setprecision(3) << relevanceThreshold << ")";
    NLM_LOG_INFO(oss.str());
}

void NeuralEpisodicMemory::consolidateSynapticTags(float relevanceThreshold) {
    std::vector<std::pair<NeuronId, SimulationStep>> activeTags;
    
    for (const auto& tag : pImpl->taggedSynapses_) {
        SimulationStep age = tag.second;
        
        if (age < 1000) {
            activeTags.push_back(tag);
            
            auto it = pImpl->synapticTags_.find(tag.first.value);
            if (it != pImpl->synapticTags_.end()) {
                float tagStrength = it->second;
                
                if (tagStrength > relevanceThreshold) {
                    markForConsolidation(tag.first.value, tagStrength);
                }
            }
        }
    }
    
    pImpl->taggedSynapses_ = std::move(activeTags);
}

void NeuralEpisodicMemory::markForConsolidation(uint64_t synapseId, float strength) {
    if (pImpl->brain_) {
        STDP* stdp = pImpl->brain_->getSTDP();
        if (stdp) {
            SynapticWeight ltpWeight = stdp->getLTPWeight();
            stdp->applyWeightChange(nullptr, ltpWeight * strength);
        }
    }
    
    pImpl->synapticTags_.erase(synapseId);
}

void NeuralEpisodicMemory::removeHippocampalTrace(size_t episodeIndex) {
    if (episodeIndex < pImpl->hippocampalPatternNeurons_.size()) {
        pImpl->hippocampalPatternNeurons_.erase(
            pImpl->hippocampalPatternNeurons_.begin() + episodeIndex);
    }
}

void NeuralEpisodicMemory::clear() {
    episodes_.clear();
    pImpl->hippocampalPatternNeurons_.clear();
    pImpl->neocorticalPatternNeurons_.clear();
    pImpl->taggedSynapses_.clear();
    pImpl->synapticTags_.clear();
    pImpl->episodeRelevance_.clear();
    pImpl->episodeReplayCount_.clear();
    
    std::ostringstream oss;
    oss << "NeuralEpisodicMemory: Cleared all episodic memory";
    NLM_LOG_INFO(oss.str());
}

std::vector<const EpisodicMemoryItem*> NeuralEpisodicMemory::getEpisodesForReplay(
    size_t count) const {
    
    std::vector<std::tuple<float, float, size_t>> scored;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        float recencyScore = 1.0f / (episodes_[i].age + 1);
        float rewardScore = episodes_[i].reward;
        float noveltyScore = episodes_[i].novelty;
        float relevance = getEpisodeRelevance(&episodes_[i]);
        
        float score = recencyScore * 0.3f + 
                      rewardScore * 0.3f + 
                      noveltyScore * 0.2f + 
                      relevance * 0.2f;
        
        scored.emplace_back(score, rewardScore, i);
    }
    
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
    if (!replayEnabled_) return;
    
    for (size_t id : episodeIds) {
        if (id < episodes_.size()) {
            replayEpisode(&episodes_[id]);
        }
    }
    
    std::ostringstream oss;
    oss << "Performed replay sequence of " << episodeIds.size() << " episodes";
    NLM_LOG_INFO(oss.str());
}

float NeuralEpisodicMemory::computeSimilarity(const std::vector<float>& query,
                                             const EpisodicMemoryItem& episode) const {
    if (query.empty() || episode.sensoryState.empty()) return 0.0f;
    if (query.size() != episode.sensoryState.size()) return 0.0f;
    
    float dot = 0.0f, normQ = 0.0f, normE = 0.0f;
    
    for (size_t i = 0; i < query.size(); ++i) {
        float weight = 1.0f;
        
        if (i < 3) {
            weight = 2.0f;
        }
        
        dot += query[i] * episode.sensoryState[i] * weight;
        normQ += query[i] * query[i] * weight;
        normE += episode.sensoryState[i] * episode.sensoryState[i] * weight;
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

size_t NeuralEpisodicMemory::getEpisodeIndex(const EpisodicMemoryItem* episode) const {
    if (!episode) return 0;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        if (&episodes_[i] == episode) {
            return i;
        }
    }
    
    return 0;
}

float NeuralEpisodicMemory::getEpisodeRelevance(const EpisodicMemoryItem* episode) const {
    if (!episode) return 0.0f;
    
    size_t index = getEpisodeIndex(episode);
    if (index < pImpl->episodeRelevance_.size()) {
        return pImpl->episodeRelevance_[index];
    }
    
    return 0.0f;
}

} // namespace nlm

