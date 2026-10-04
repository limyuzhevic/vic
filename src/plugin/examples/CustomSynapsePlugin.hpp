// CustomSynapsePlugin.hpp
#pragma once

#include "../interfaces/SynapsePluginInterface.hpp"
#include "../../brain/Synapse.hpp"
#include <memory>

namespace nlm {

class CustomSynapsePlugin : public SynapsePluginInterface {
public:
    CustomSynapsePlugin();
    ~CustomSynapsePlugin() override = default;

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

    // Synapse model creation
    std::unique_ptr<Synapse> createSynapse(const Config& config) override;
    std::unique_ptr<Synapse> createSynapse(NeuronId source, NeuronId target, const Config& config) override;

    // Synapse configuration
    bool configureSynapse(Synapse* synapse, const Config& config) override;
    bool validateSynapseConfiguration(const Config& config) const override;

    // Synapse properties
    SynapticWeight getDefaultWeight() const override;
    Delay getDefaultDelay() const override;
    float getWeightRangeMin() const override;
    float getWeightRangeMax() const override;
    Delay getDelayRangeMin() const override;
    Delay getDelayRangeMax() const override;

    // Synapse dynamics
    bool supportsCustomDynamics() const override;
    std::string getDynamicModelName() const override;
    float getTimeConstant() const override;

    // Plasticity integration
    bool supportsCustomPlasticity() const override;
    std::string getPlasticityModelName() const override;

    // Plugin-specific configuration
    std::string getDefaultConfigurationKey() const override;
    Config getDefaultConfiguration() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm