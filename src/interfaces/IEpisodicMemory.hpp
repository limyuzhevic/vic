// IEpisodicMemory.hpp
// Abstract interface for episodic memory systems
// Stores and manages episodic memories (what, where, when, how)
// Supports replay and consolidation mechanisms

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <chrono>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;

// Episodic memory entry structure
struct EpisodicEntry {
    SimulationStep timestamp;
    std::vector<float> contextPattern;
    std::vector<float> contentPattern;
    std::string action;
    float strength;
    float predictionError;
    bool isConsolidated;
    
    EpisodicEntry() : timestamp(0), strength(1.0f), predictionError(0.0f), isConsolidated(false) {}
    EpisodicEntry(SimulationStep ts, const std::vector<float>& ctx, const std::vector<float>& cont,
                  const std::string& act, float str)
        : timestamp(ts), contextPattern(ctx), contentPattern(cont), action(act), strength(str),
          predictionError(0.0f), isConsolidated(false) {}
};

class IEpisodicMemory {
public:
    virtual ~IEpisodicMemory() = default;

    // Initialize episodic memory with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Store a new episodic memory
    virtual void storeEpisode(const std::vector<float>& context,
                             const std::vector<float>& content,
                             const std::string& action,
                             float strength = 1.0f) = 0;

    // Store memory with explicit timestamp
    virtual void storeEpisode(const std::vector<float>& context,
                             const std::vector<float>& content,
                             const std::string& action,
                             SimulationStep timestamp,
                             float strength = 1.0f) = 0;

    // Retrieve recent episodes matching context
    virtual std::vector<EpisodicEntry> retrieveEpisodes(
        const std::vector<float>& context,
        SimulationStep lookbackWindow = 100) const = 0;

    // Get most recent episode
    virtual EpisodicEntry getRecentEpisode() const = 0;

    // Update episodic memory (consolidation and replay)
    virtual void update(TimestepDuration dt) = 0;

    // Clear all episodic memories
    virtual void clear() = 0;

    // Get number of stored episodes
    virtual size_t getEpisodeCount() const = 0;

    // Get maximum capacity
    virtual size_t getMaxCapacity() const = 0;
    virtual void setMaxCapacity(size_t cap) = 0;

    // Check if episode with given context exists
    virtual bool hasEpisode(const std::vector<float>& context) const = 0;

    // Get episode strength for context
    virtual float getEpisodeStrength(const std::vector<float>& context) const = 0;

    // Get episode prediction error for context
    virtual float getEpisodePredictionError(const std::vector<float>& context) const = 0;

    // Consolidate weak memories
    virtual void consolidateWeakEpisodes() = 0;

    // Replay strong memories for rehearsal
    virtual void replayStrongEpisodes() = 0;

    // Get storage statistics
    virtual std::string getStorageStats() const = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "IEpisodicMemory"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for memory events
    using EpisodeStoreCallback = std::function<void(const EpisodicEntry& entry)>;
    virtual void setEpisodeStoreCallback(const EpisodeStoreCallback& callback) = 0;

    using EpisodeReplayCallback = std::function<void(const EpisodicEntry& entry)>;
    virtual void setEpisodeReplayCallback(const EpisodeReplayCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

protected:
    IEpisodicMemory() = default;
};

} // namespace nlm
