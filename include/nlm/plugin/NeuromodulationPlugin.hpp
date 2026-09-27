#pragma once

#include "PluginBase.hpp"
#include <string>
#include <memory>
#include <concepts>
#include <vector>

namespace nlm {

// Neuromodulation plugin capability concept
template<typename T>
concept NeuromodulationPluginCapability = PluginCapability<T> && requires(T plugin) {
    { plugin.process() } -> std::same_as<double>;
    { plugin.getModulatorType() } -> std::same_as<std::string>;
};

// Neuromodulation Plugin Interface for custom neuromodulators
class NeuromodulationPlugin : public virtual PluginBase {
public:
    virtual ~NeuromodulationPlugin() = default;

    // Process neuromodulation signal
    virtual double process(double inputSignal, const std::any& context) = 0;

    // Get neuromodulator type
    virtual std::string getModulatorType() const = 0;

    // Get modulation strength (0.0 to 1.0)
    virtual double getModulationStrength() const = 0;

    // Set modulation strength
    virtual void setModulationStrength(double strength) = 0;

    // Check if neuromodulator is active
    virtual bool isActive() const = 0;

    // Activate/deactivate neuromodulator
    virtual void setActive(bool active) = 0;

    // Get neuromodulator parameters
    virtual std::unordered_map<std::string, double> getParameters() const = 0;

    // Set neuromodulator parameters
    virtual void setParameters(const std::unordered_map<std::string, double>& params) = 0;

    // Get effect duration in simulation steps
    virtual uint32_t getEffectDuration() const = 0;

    // Set effect duration
    virtual void setEffectDuration(uint32_t duration) = 0;

    // Reset neuromodulator state
    virtual void reset() = 0;

    // Check if neuromodulator can be applied
    virtual bool canApply(double input) const = 0;
};

} // namespace nlm
