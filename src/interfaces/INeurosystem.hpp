// INeurosystem.hpp
// Abstract base interface for neuromodulation systems
// Defines the contract for neuromodulatory signal management
// Serves as base class for specific neuromodulators

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>

namespace nlm {

// Forward declarations
class Brain;

class INeurosystem {
public:
    virtual ~INeurosystem() = default;

    // Initialize neuromodulation system with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Get neuromodulator name
    virtual const char* getName() const = 0;

    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;

    // Update neuromodulator state (typically called each timestep)
    virtual void update(TimestepDuration dt) = 0;

    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;

    // Get target neurons/populations affected by this neuromodulator
    virtual std::vector<NeuronId> getTargetNeurons() const = 0;
    virtual std::vector<RegionId> getTargetRegions() const = 0;

    // Set target neurons/populations
    virtual void setTargetNeurons(const std::vector<NeuronId>& neurons) = 0;
    virtual void setTargetRegions(const std::vector<RegionId>& regions) = 0;

    // Get neuromodulator effect on synaptic transmission
    virtual float getSynapticModulationFactor() const = 0;
    virtual void setSynapticModulationFactor(float factor) = 0;

    // Reset neuromodulator to baseline
    virtual void reset() = 0;

    // Get statistics
    virtual std::string getStats() const = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "INeurosystem"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for neuromodulation events
    using NeuromodulationUpdateCallback = std::function<void(float newLevel, float plasticityFactor)>;
    virtual void setUpdateCallback(const NeuromodulationUpdateCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // neuromodulator-specific parameters
    virtual bool isConfigurable() const = 0;
    virtual std::map<std::string, float> getConfiguration() const = 0;
    virtual void setConfiguration(const std::map<std::string, float>& config) = 0;

    // Get neuromodulator type for classification
    enum class NeuromodulatorType {
        Reward,
        Arousal,
        Attention,
        Novelty,
        PredictionError,
        Curiosity,
        Unknown
    };
    virtual NeuromodulatorType getType() const = 0;

protected:
    INeurosystem() = default;
};

} // namespace nlm
