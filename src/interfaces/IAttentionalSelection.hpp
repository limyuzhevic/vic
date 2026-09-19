// IAttentionalSelection.hpp
// Abstract interface for attentional selection systems
// Selects salient neural populations for processing
// Uses gain modulation and inhibition to focus processing

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>
#include <unordered_set>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;

// Attentional priority entry
struct AttentionalPriority {
    NeuronId neuronId;
    float priorityScore;
    float salience;
    float gainModulation;
    SimulationStep lastUpdated;
    AttentionalPriority boost;
    float inhibition;
    
    AttentionalPriority() : priorityScore(0), salience(0), gainModulation(1),
                           lastUpdated(0), boost(0), inhibition(1) {}
};

// Attentional focus region
struct AttentionalFocus {
    RegionId regionId;
    float focusStrength;
    float processingPriority;
    SimulationStep acquisitionTime;
    SimulationStep decayStartTime;
    
    AttentionalFocus() : regionId(), focusStrength(0), processingPriority(0),
                        acquisitionTime(0), decayStartTime(0) {}
};

class IAttentionalSelection {
public:
    virtual ~IAttentionalSelection() = default;

    // Initialize attentional selection with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Process competing inputs and select winners
    // Returns IDs of winning neural populations
    virtual std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                                    float globalInhibition = 0.5f) = 0;

    // Apply attention to a specific region (focus processing there)
    virtual void focusOnRegion(RegionId region) = 0;

    // Release attention (allow all regions equal processing)
    virtual void releaseAttention() = 0;

    // Get current attention focus
    virtual std::vector<RegionId> getAttendedRegions() const = 0;

    // Set attention parameters
    virtual void setInhibitionStrength(float strength) = 0;
    virtual void setExcitationStrength(float strength) = 0;
    virtual void setCompetitionThreshold(float threshold) = 0;

    // Get inhibition level for a neuron
    virtual float getInhibitionFor(NeuronId neuron) const = 0;

    // Get excitation level for a neuron
    virtual float getExcitationFor(NeuronId neuron) const = 0;

    // Update attention dynamics
    virtual void update(TimestepDuration dt) = 0;

    // Is this neuron currently attended?
    virtual bool isAttended(NeuronId neuron) const = 0;

    // Get attention winners
    virtual const std::vector<NeuronId>& getWinners() const = 0;

    // Top-down attention (goal-directed)
    virtual void applyTopDownBias(NeuronId neuron, float biasStrength) = 0;

    // Bottom-up attention (stimulus-driven)
    virtual void applyBottomUpSalience(NeuronId neuron, float salienceStrength) = 0;

    // Reset attention
    virtual void reset() = 0;

    // Get attentional statistics
    virtual std::string getAttentionalStats() const = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "IAttentionalSelection"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for attention events
    using AttentionSelectedCallback = std::function<void(const std::vector<NeuronId>& winners)>;
    virtual void setAttentionSelectedCallback(const AttentionSelectedCallback& callback) = 0;

    using AttentionFocusedCallback = std::function<void(RegionId region, float strength)>;
    virtual void setAttentionFocusedCallback(const AttentionFocusedCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // Performance metrics
    virtual float getSelectionAccuracy() const = 0;
    virtual float getSelectionSpeed() const = 0;  // Selections per second
    virtual uint64_t getTotalSelectionsMade() const = 0;

    // Attention parameters
    virtual float getGlobalInhibitionLevel() const = 0;
    virtual void setGlobalInhibitionLevel(float level) = 0;
    virtual float getSalienceSigma() const = 0;  // How quickly salience decays
    virtual void setSalienceSigma(float sigma) = 0;
    virtual float getBoostDecayRate() const = 0;
    virtual void setBoostDecayRate(float rate) = 0;

protected:
    IAttentionalSelection() = default;
};

} // namespace nlm
