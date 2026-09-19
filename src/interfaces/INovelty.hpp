// INovelty.hpp
// Abstract interface for novelty detection neuromodulator
// Detects novel stimuli and states for exploration
// Novelty signals drive curiosity and learning about new experiences

#pragma once

#include "INeurosystem.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>
#include <unordered_set>

namespace nlm {

class INovelty : public INeurosystem {
public:
    virtual ~INovelty() = default;

    // Initialize novelty detection system with brain reference
    virtual bool initialize(Brain* brain) override = 0;

    // Get neuromodulator name
    virtual const char* getName() const override { return "Novelty"; }

    // Get current novelty level
    virtual float getLevel() const override = 0;
    virtual void setLevel(float level) override = 0;

    // Update novelty detection (called each timestep)
    virtual void update(TimestepDuration dt) override = 0;

    // Apply novelty effect to plasticity
    virtual float getPlasticityFactor() const override = 0;

    // Detect novelty of given state/features
    virtual float detectNovelty(const std::vector<float>& state) = 0;

    // Detect novelty of sensory input
    virtual float detectSensoryNovelty(const class SensoryInput& input) = 0;

    // Register state as known (to reduce novelty detection)
    virtual void registerState(const std::vector<float>& state) = 0;

    // Get novelty history
    virtual const std::vector<float>& getNoveltyHistory() const = 0;
    virtual const std::vector<std::vector<float>>& getStateHistory() const = 0;
    virtual void clearHistory() = 0;

    // Get novelty statistics
    virtual float getAverageNovelty() const = 0;
    virtual float getMaximumNovelty() const = 0;
    virtual float getNoveltyEntropy() const = 0;  // Diversity of novel experiences

    // Set/Get novelty threshold for significance
    virtual float getNoveltyThreshold() const = 0;
    virtual void setNoveltyThreshold(float threshold) = 0;

    // Set/Get novelty decay rate (how quickly novelty fades)
    virtual float getNoveltyDecayRate() const = 0;
    virtual void setNoveltyDecayRate(float rate) = 0;

    // Get/Get novelty memory capacity
    virtual size_t getNoveltyMemoryCapacity() const = 0;
    virtual void setNoveltyMemoryCapacity(size_t capacity) = 0;

    // Reset novelty detection system to initial state
    virtual void reset() override = 0;

    // Interface information override
    virtual NeuromodulatorType getType() const override { return NeuromodulatorType::Novelty; }

    // Error handling override
    virtual bool hasError() const override = 0;
    virtual std::string getErrorMessage() const override = 0;
    virtual void clearError() override = 0;

    // Callback pattern for novelty events
    using NoveltyDetectedCallback = std::function<void(const std::vector<float>& state, float noveltyLevel)>;
    virtual void setNoveltyDetectedCallback(const NoveltyDetectedCallback& callback) = 0;

    using StateRegisteredCallback = std::function<void(const std::vector<float>& state)>;
    virtual void setStateRegisteredCallback(const StateRegisteredCallback& callback) = 0;

    // Debug information override
    virtual std::string getDebugInfo() const override = 0;

    // Performance metrics
    virtual uint64_t getTotalStatesProcessed() const = 0;
    virtual uint64_t getTotalNovelStatesDetected() const = 0;
    virtual float getNoveltyDetectionAccuracy() const = 0;

    // Get set of known states
    virtual std::unordered_set<uint64_t> getKnownStateHashes() const = 0;
    virtual void clearKnownStates() = 0;

protected:
    INovelty() = default;
};

} // namespace nlm
