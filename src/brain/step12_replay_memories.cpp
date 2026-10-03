// Step 12: Replay important memories
// This module replays important memories for consolidation
// Retransmits key experiences to strengthen connections

#include "step12_replay_memories.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

namespace Step12 {

void GetEpisodesForReplay(Brain::Impl* impl,
                          size_t maxEpisodes,
                          std::vector<const class EpisodicMemoryItem*>& episodesToReplay) {
    if (!impl || !impl->episodicMemory) {
        return;
    }
    
    episodesToReplay = impl->episodicMemory->getEpisodesForReplay(maxEpisodes);
}

void ReplaySingleEpisode(Brain::Impl* impl,
                        const class EpisodicMemoryItem* episode) {
    if (!impl || !impl->episodicMemory || !episode) {
        return;
    }
    
    impl->episodicMemory->replayEpisode(episode);
}

void ReplayMemories(Brain::Impl* impl,
                    SimulationStep currentStep,
                    Timestamp currentTime) {
    if (!impl) {
        NLM_LOG_ERROR("ReplayMemories called with null impl pointer");
        return;
    }
    
    if (!impl->episodicMemory) {
        NLM_LOG_DEBUG("Episodic memory not initialized, skipping replay");
        return;
    }
    
    NLM_LOG_DEBUG("Replaying memories (Step " + std::to_string(currentStep) + ")");
    
    // Replay important memories periodically
    if (currentStep % impl->replayInterval == 0) {
        auto episodesToReplay = impl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            ReplaySingleEpisode(impl, episode);
        }
    }
}

} // namespace Step12

} // namespace nlm