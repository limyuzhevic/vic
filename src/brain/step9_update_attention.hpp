// Step 9: Update attention system
// This module updates the attentional selection system
// Manages focus allocation and competition among memory traces

#pragma once

#include <memory>
#include <vector>
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step9 {

// Helper function to apply attention to working memory winners
void ApplyAttentionToWorkingMemory(Brain::Impl* impl,
                                   const std::vector<NeuronId>& competitors);

// Helper function to process attentional competition
void ProcessAttentionalCompetition(Brain::Impl* impl,
                                  const std::vector<NeuronId>& competitors);

// Update attention system
void UpdateAttention(Brain::Impl* impl,
                    SimulationStep currentStep,
                    Timestamp currentTime);

} // namespace Step9

} // namespace nlm