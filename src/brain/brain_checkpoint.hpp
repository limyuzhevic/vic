#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Checkpoint and serialization
class BrainCheckpoint {
public:
    BrainCheckpoint() = default;
    ~BrainCheckpoint() = default;
    
    // Save brain state to file
    bool save(const std::string& filepath,
              const std::vector<std::unique_ptr<class NeuralRegion>>& regions,
              size_t totalSpikesTotal,
              size_t totalSpikesThisStep,
              float learnedPreference,
              SimulationStep stepsSinceLastEpisode,
              std::shared_ptr<class RandomGenerator> rng,
              std::shared_ptr<class SpikeSystem> spikeSystem);
    
    // Load brain state from file
    bool load(const std::string& filepath,
              std::vector<std::unique_ptr<class NeuralRegion>>& regions,
              size_t& totalSpikesTotal,
              size_t& totalSpikesThisStep,
              float& learnedPreference,
              SimulationStep& stepsSinceLastEpisode,
              std::shared_ptr<class RandomGenerator> rng,
              std::shared_ptr<class SpikeSystem> spikeSystem);
    
    // Get checkpoint manager status
    std::string getCheckpointStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
