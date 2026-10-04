// DevelopmentPluginInterface.hpp
#pragma once

#include "../PluginBase.hpp"
#include <memory>
#include <vector>
#include "../../development/DevelopmentSystem.hpp"

namespace nlm {

class DevelopmentPluginInterface : public virtual PluginBase {
public:
    virtual ~DevelopmentPluginInterface() = default;

    // Development system creation
    virtual std::unique_ptr<DevelopmentSystem> createDevelopmentSystem(const Config& config) = 0;

    // Development system configuration
    virtual bool configureDevelopmentSystem(DevelopmentSystem* system, const Config& config) = 0;
    virtual bool validateDevelopmentSystemConfiguration(const Config& config) const = 0;

    // Developmental properties
    virtual float getDefaultCriticalPeriodStart() const = 0;
    virtual float getDefaultCriticalPeriodEnd() const = 0;
    virtual float getDefaultMaturationRate() const = 0;
    virtual float getDefaultPlasticityModifier() const = 0;
    virtual float getDefaultDevelopmentThreshold() const = 0;

    // Development stages
    virtual std::vector<std::string> getSupportedDevelopmentalStages() const = 0;
    virtual bool supportsCustomStageTransitions() const = 0;

    // Plugin-specific configuration
    virtual std::string getDefaultConfigurationKey() const = 0;
    virtual Config getDefaultConfiguration() const = 0;

    // Development stage calculations
    virtual float calculatePlasticityModifier(float age, const Config& config) const = 0;
    virtual float calculateCriticalPeriodEnd(float age, const Config& config) const = 0;
    virtual std::string getCurrentDevelopmentalStage(float age, const Config& config) const = 0;
};

} // namespace nlm