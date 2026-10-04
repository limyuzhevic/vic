// NeuromodulatorPluginInterface.hpp
#pragma once

#include "../PluginBase.hpp"
#include <memory>
#include <vector>
#include "../../neuromodulation/Neuromodulator.hpp"

namespace nlm {

class NeuromodulatorPluginInterface : public virtual PluginBase {
public:
    virtual ~NeuromodulatorPluginInterface() = default;

    // Neuromodulator creation
    virtual std::unique_ptr<Neuromodulator> createNeuromodulator(const Config& config) = 0;

    // Neuromodulator configuration
    virtual bool configureNeuromodulator(Neuromodulator* neuromodulator, const Config& config) = 0;
    virtual bool validateNeuromodulatorConfiguration(const Config& config) const = 0;

    // Neuromodulation properties
    virtual float getDefaultBaseLevel() const = 0;
    virtual float getMaxModulationLevel() const = 0;
    virtual float getMinModulationLevel() const = 0;
    virtual float getDecayRate() const = 0;
    virtual float getTimeConstant() const = 0;

    // Signal processing
    virtual bool supportsCustomDynamics() const = 0;
    virtual std::string getDynamicModelName() const = 0;
    virtual bool supportsTemporalFiltering() const = 0;
    virtual float getFilterCutoffFrequency() const = 0;

    // Plugin-specific configuration
    virtual std::string getDefaultConfigurationKey() const = 0;
    virtual Config getDefaultConfiguration() const = 0;

    // Signal processing methods
    virtual float processRewardSignal(float reward, const Config& config) const = 0;
    virtual float processPredictionErrorSignal(float error, const Config& config) const = 0;
    virtual float processNoveltySignal(float novelty, const Config& config) const = 0;
    virtual float processCuriositySignal(float curiosity, const Config& config) const = 0;
};

} // namespace nlm