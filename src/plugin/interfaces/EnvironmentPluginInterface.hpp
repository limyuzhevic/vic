// EnvironmentPluginInterface.hpp
#pragma once

#include "../PluginBase.hpp"
#include <memory>
#include <vector>
#include "../../environment/SimpleWorld.hpp"

namespace nlm {

class EnvironmentPluginInterface : public virtual PluginBase {
public:
    virtual ~EnvironmentPluginInterface() = default;

    // Environment creation
    virtual std::unique_ptr<SimpleWorld> createEnvironment(const Config& config) = 0;

    // Environment configuration
    virtual bool configureEnvironment(SimpleWorld* environment, const Config& config) = 0;
    virtual bool validateEnvironmentConfiguration(const Config& config) const = 0;

    // Environment properties
    virtual std::string getEnvironmentType() const = 0;
    virtual std::vector<std::string> getSupportedSensors() const = 0;
    virtual std::vector<std::string> getSupportedActuators() const = 0;
    virtual float getDefaultTimeStep() const = 0;
    virtual float getDefaultMaxTime() const = 0;

    // Environment dynamics
    virtual bool supportsCustomPhysics() const = 0;
    virtual std::string getPhysicsModelName() const = 0;
    virtual bool supportsCustomRewards() const = 0;
    virtual bool supportsCustomStates() const = 0;

    // Plugin-specific configuration
    virtual std::string getDefaultConfigurationKey() const = 0;
    virtual Config getDefaultConfiguration() const = 0;

    // Environment dynamics and control
    virtual float calculateReward(const SimpleWorld& environment, const MotorCommand& action) const = 0;
    virtual bool isTerminalState(const SimpleWorld& environment) const = 0;
    virtual std::vector<float> getState(const SimpleWorld& environment) const = 0;
    virtual bool isValidAction(const MotorCommand& action) const = 0;
};

} // namespace nlm