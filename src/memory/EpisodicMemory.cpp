#include "EpisodicMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

EpisodicMemory::EpisodicMemory()
    : maxEpisodes_(1000)
    , brain_(nullptr)
{
}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::initialize(Brain* brain) {
    brain_ = brain;
    NLM_LOG_INFO("EpisodicMemory initialized");
}

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Create episode copy with age 0
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    
    episodes_.push_back(stored);
    
    // Remove old episodes if over capacity
    while (episodes_.size() > maxEpisodes_) {
        episodes_.erase(episodes_.begin());
    }
    
    // Age all episodes
    for (auto& ep : episodes_) {
        ep.age++;
    }
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    if (index < episodes_.size()) {
        return episodes_[index];
    }
    return EpisodicMemoryItem();
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<EpisodicMemoryItem> result;
    
    size_t start = count > episodes_.size() ? 0 : episodes_.size() - count;
    result.insert(result.begin(), episodes_.begin() + start, episodes_.end());
    
    return result;
}

void EpisodicMemory::clear() {
    episodes_.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Remove episodes below relevance threshold based on age and reward
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < episodes_.size(); ++i) {
        // Check if episode should be consolidated (relevance)
        float ageRelevance = 1.0f / (episodes_[i].age + 1);  // More recent = more relevant
        float rewardRelevance = episodes_[i].reward / 10.0f;  // Higher reward = more relevant
        
        if (ageRelevance + rewardRelevance < relevanceThreshold) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak episodes (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        episodes_.erase(episodes_.begin() + *it);
    }
}

float EpisodicMemory::getAverageReward() const {
    if (episodes_.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& ep : episodes_) {
        sum += ep.reward;
    }
    return sum / episodes_.size();
}

} // namespace nlm