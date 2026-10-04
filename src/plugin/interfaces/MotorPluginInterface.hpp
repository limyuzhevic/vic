// MotorPluginInterface.hpp
#pragma once

#include "../PluginBase.hpp"
#include <memory>
#include <vector>
#include "../../motor/MotorCommand.hpp"

namespace nlm {

class MotorPluginInterface : public virtual PluginBase {
public:
    virtual ~MotorPluginInterface() = default;

    // Motor command creation
    virtual std::unique_ptr<MotorCommand> createMotorCommand(const Config& config) = 0;
    virtual std::unique_ptr<MotorCommand> createMotorCommand(ActionType action, const Config& config) = 0;

    // Motor command configuration
    virtual bool configureMotorCommand(MotorCommand* command, const Config& config) = 0;
    virtual bool validateMotorCommandConfiguration(const Config& config) const = 0;

    // Motor properties
    virtual std::vector<std::string> getSupportedActions() const = 0;
    virtual float getDefaultActionSelectionThreshold() const = 0;
    virtual float getDefaultNoiseLevel() const = 0;
    virtual float getDefaultExplorationRate() const = 0;

    // Action selection
    virtual bool supportsCustomActionSelection() const = 0;
    virtual std::string getActionSelectionModelName() const = 0;
    virtual bool supportsTemporalAbstraction() const = 0;
    virtual float getTemporalAbstractionWindow() const = 0;

    // Plugin-specific configuration
    virtual std::string getDefaultConfigurationKey() const = 0;
    virtual Config getDefaultConfiguration() const = 0;

    // Action processing
    virtual ActionType selectAction(const std::vector<float>& motorActivity, const Config& config) const = 0;
    virtual float calculateActionSelectionConfidence(const std::vector<float>& motorActivity, const Config& config) const = 0;
    virtual float calculateExplorationRate(float timeStep, const Config& config) const = 0;
};

} // namespace nlm