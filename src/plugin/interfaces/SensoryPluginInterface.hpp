// SensoryPluginInterface.hpp
#pragma once

#include "../PluginBase.hpp"
#include <memory>
#include <vector>
#include "../../sensory/SensoryProcessor.hpp"

namespace nlm {

class SensoryPluginInterface : public virtual PluginBase {
public:
    virtual ~SensoryPluginInterface() = default;

    // Sensory processor creation
    virtual std::unique_ptr<SensoryProcessor> createSensoryProcessor(const Config& config) = 0;
    virtual std::unique_ptr<SensoryProcessor> createSensoryProcessor(SensoryModality modality, const Config& config) = 0;

    // Sensory processor configuration
    virtual bool configureSensoryProcessor(SensoryProcessor* processor, const Config& config) = 0;
    virtual bool validateSensoryProcessorConfiguration(const Config& config) const = 0;

    // Sensory properties
    virtual std::vector<std::string> getSupportedModalities() const = 0;
    virtual size_t getDefaultInputSize() const = 0;
    virtual float getDefaultNoiseLevel() const = 0;
    virtual float getDefaultGain() const = 0;
    virtual float getDefaultThreshold() const = 0;

    // Sensory processing
    virtual bool supportsCustom transduction() const = 0;
    virtual std::string getTransductionModelName() const = 0;
    virtual bool supportsCustomFiltering() const = 0;
    virtual float getFilterCutoffFrequency() const = 0;

    // Plugin-specific configuration
    virtual std::string getDefaultConfigurationKey() const = 0;
    virtual Config getDefaultConfiguration() const = 0;

    // Sensory signal processing
    virtual float processSensoryInput(float input, const Config& config) const = 0;
    virtual float calculateSensoryResponse(float stimulus, const Config& config) const = 0;
    virtual bool isSensoryActive(float input, const Config& config) const = 0;
};

} // namespace nlm