// Step 7: Update episodic memory
// This module updates episodic memory with current experience
// Stores episodes of neural activity and reward signals

#pragma once

#include <memory>
#include <vector>
#include "../brain/NeuralRegion.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

namespace Step7 {

// Helper function to capture current brain state as an episode
void CaptureBrainStateForEpisode(Brain::Impl* impl,
                                 SimulationStep currentStep,
                                 Timestamp currentTime,
                                 EpisodicMemoryItem& episode);

// Helper function to extract active neurons from brain regions
void ExtractActiveNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                          std::vector<NeuronId>& activeNeurons,
                          std::vector<float>& activations);

// Helper function to update episode reward
void UpdateEpisodeReward(EpisodicMemoryItem& episode, Dopamine* dopamine);

// Store current brain state as an episode
void StoreCurrentEpisode(Brain::Impl* impl,
                         SimulationStep currentStep,
                         Timestamp currentTime);

// Update episodic memory with periodic episodes
void UpdateEpisodicMemory(Brain::Impl* impl,
                          SimulationStep currentStep,
                          Timestamp currentTime);

} // namespace Step7

} // namespace nlm