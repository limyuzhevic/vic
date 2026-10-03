// Step 14: Periodic memory consolidation
// This module consolidates important memories and removes weak ones
// Maintains memory efficiency and removes noise

#pragma once

#include <memory>
#include <vector>
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step14 {

// Helper function to consolidate memories with strength threshold
void ConsolidateMemoriesWithThreshold(NeuralEpisodicMemory* episodicMemory,
                                      float strengthThreshold);

// Consolidate important memories, remove weak ones
void ConsolidateMemories(Brain::Impl* impl,
                         SimulationStep currentStep,
                         Timestamp currentTime);

} // namespace Step14

} // namespace nlm