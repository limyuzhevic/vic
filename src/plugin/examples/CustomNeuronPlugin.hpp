// CustomNeuronPlugin.hpp
#pragma once

#include "../interfaces/NeuronPluginInterface.hpp"
#include "../../brain/Neuron.hpp"
#include <memory>

namespace nlm {

class CustomNeuronPlugin : public NeuronPluginInterface {
public:
    CustomNeuronPlugin();
    ~CustomNeuronPlugin() override = default;

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

    // Neuron model creation
    std::unique_ptr<Neuron> createNeuron(const Config& config) override;
    std::unique_ptr<Neuron> createNeuron(NeuronType type, const Config& config) override;

    // Neuron configuration
    bool configureNeuron(Neuron* neuron, const Config& config) override;
    bool validateNeuronConfiguration(const Config& config) const override;

    // Neuron properties
    NeuronType getDefaultNeuronType() const override;
    std::string getNeuronTypeName(NeuronType type) const override;
    bool isNeuronTypeSupported(NeuronType type) const override;

    // Neuron population management
    size_t getRecommendedPopulationSize() const override;
    size_t getMinPopulationSize() const override;
    size_t getMaxPopulationSize() const override;

    // Neuron dynamics
    bool supportsCustomDynamics() const override;
    std::string getDynamicModelName() const override;

    // Plugin-specific configuration
    std::string getDefaultConfigurationKey() const override;
    Config getDefaultConfiguration() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm