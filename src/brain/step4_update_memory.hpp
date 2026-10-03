// Step 4: Update working memory
// This module updates working memory with current neural activity patterns
// Implements memory maintenance, competition, and stabilization

#pragma once

#include <memory>
#include <vector>
#include "../brain/NeuralRegion.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step4 {

// Helper function to capture neural activity for working memory
void CaptureNeuralActivityForWorkingMemory(Brain::Impl* impl,
                                           std::vector<NeuronId>& activeNeurons,
                                           std::vector<float>& activations);

// Helper function to update working memory traces
void UpdateWorkingMemoryTraces(NeuralWorkingMemory* workingMemory,
                               const std::vector<NeuronId>& activeNeurons,
                               const std::vector<float>& activations,
                               TimestepDuration timestep);

// Update working memory with current neural state
void UpdateWorkingMemory(Brain::Impl* impl,
                         SimulationStep currentStep,
                         Timestamp currentTime);

// Optimize working memory update with efficient data structures
void OptimizedWorkingMemoryUpdate(Brain::Impl* impl,
                                  SimulationStep currentStep,
                                  Timestamp currentTime);

} // namespace Step4

} // namespace nlm