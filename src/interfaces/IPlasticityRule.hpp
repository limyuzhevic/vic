// IPlasticityRule.hpp
// Abstract interface for plasticity rules
// Defines how synaptic weights change based on neural activity
// Supports various learning algorithms and neuromodulatory influences

#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <string>

namespace nlm {

// Forward declarations
class Synapse;
class Brain;

class IPlasticityRule {
public:
    virtual ~IPlasticityRule() = default;

    // Initialize plasticity rule with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Apply plasticity change to synapse based on activity
    virtual void applyPlasticity(SynapseHandle synapse,
                                 const std::vector<float>& preTrace,
                                 const std::vector<float>& postTrace,
                                 float predictionError = 0.0f,
                                 float neuromodulatorLevel = 1.0f) = 0;

    // Compute weight change for given traces
    virtual float computeWeightChange(const std::vector<float>& preTrace,
                                      const std::vector<float>& postTrace,
                                      float predictionError = 0.0f,
                                      float neuromodulatorLevel = 1.0f) const = 0;

    // Get learning rate for this plasticity rule
    virtual float getLearningRate() const = 0;
    virtual void setLearningRate(float rate) = 0;

    // Get rule-specific parameters
    virtual float getLTPStrength() const = 0;
    virtual float getLTDStrength() const = 0;
    virtual float getTimeConstant() const = 0;

    // Update rule parameters (for learning rate adaptation)
    virtual void updateParameters(TimestepDuration dt) = 0;

    // Get rule name and description
    virtual std::string getRuleName() const = 0;
    virtual std::string getDescription() const = 0;

    // Reset rule state
    virtual void reset() = 0;

    // Get statistics
    virtual std::string getStats() const = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "IPlasticityRule"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for plasticity events
    using PlasticityAppliedCallback = std::function<void(SynapseHandle synapse,
                                                         float weightChange,
                                                         float predictionError)>;
    virtual void setPlasticityAppliedCallback(const PlasticityAppliedCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // Rule capabilities
    virtual bool supportsSTDP() const = 0;
    virtual bool supportsHebbian() const = 0;
    virtual bool supportsRewardModulation() const = 0;
    virtual bool supportsStructuralPlasticity() const = 0;

    // Get rule configuration
    virtual bool isConfigurable() const = 0;
    virtual std::map<std::string, float> getConfiguration() const = 0;
    virtual void setConfiguration(const std::map<std::string, float>& config) = 0;

protected:
    IPlasticityRule() = default;
};

} // namespace nlm
