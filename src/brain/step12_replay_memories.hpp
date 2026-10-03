// Step 12: Replay important memories
// This module replays important memories for consolidation
// Retransmits key experiences to strengthen connections

#pragma once

#include <memory>
#include <vector>
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step12 {

// Helper function to get episodes for replay
void GetEpisodesForReplay(Brain::Impl* impl,
                          size_t maxEpisodes,
                          std::vector<const class EpisodicMemoryItem*>& episodesToReplay);

// Helper function to replay a single episode
void ReplaySingleEpisode(Brain::Impl* impl,
                        const class EpisodicMemoryItem* episode);

// Replay important memories (during rest or periodically)
void ReplayMemories(Brain::Impl* impl,
                    SimulationStep currentStep,
                    Timestamp currentTime);

} // namespace Step12

} // namespace nlm