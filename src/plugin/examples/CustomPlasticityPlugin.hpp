// CustomPlasticityPlugin.hpp
#pragma once

#include "../interfaces/PlasticityRulePluginInterface.hpp"
#include "../../plasticity/PlasticityRule.hpp"
#include <memory>

namespace nlm {

class CustomPlasticityPlugin : public PlasticityRulePluginInterface {
public:
    CustomPlasticityPlugin();
    ~CustomPlasticityPlugin() override = default;

    // Plugin metadata
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getAuthor() const override;

    // Plugin lifecycle
    bool initialize(const Config& config) override;
    void shutdown() override;
    bool isInitialized() const override;

    // Plugin identification
    const std::type_info& getTypeInfo() const override;

    // Plugin configuration
    void setConfiguration(const Config& config) override;
    Config getConfiguration() const override;

    // Plugin status
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;

    // Plugin validation
    bool validateConfiguration() const override;
    std::string getValidationError() const override;

    // Unique identifier for plugins
    std::string getId() const override;

    // Plasticity rule creation
    std::unique_ptr<PlasticityRule> createRule(const Config& config) override;
    std::unique_ptr<PlasticityRule> createRule(PlasticityFlags flags, const Config& config) override;

    // Rule configuration
    bool configureRule(PlasticityRule* rule, const Config& config) override;
    bool validateRuleConfiguration(const Config& config) const override;

    // Plasticity properties
    PlasticityFlags getSupportedPlasticityFlags() const override;
    std::string getPlasticityTypeName() const override;
    bool supportsCustomLearningRate() const override;
    float getDefaultLearningRate() const override;
    float getLearningRateRangeMin() const override;
    float getLearningRateRangeMax() const override;

    // Plasticity dynamics
    bool supportsCustomSTDP() const override;
    float getSTDPTimeConstant() const override;
    bool supportsCustomHebbian() const override;

    // Plugin-specific configuration
    std::string getDefaultConfigurationKey() const override;
    Config getDefaultConfiguration() const override;

    // Validation and statistics
    float calculateExpectedWeightChange(const PlasticityRule& rule, float preSpikeTime, float postSpikeTime) const override;
    float calculateLearningRate(const Config& config) const override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm