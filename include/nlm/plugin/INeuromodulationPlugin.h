#ifndef NLM_PLUGIN_INEUROMODULATIONPLUGIN_H
#define NLM_PLUGIN_INEUROMODULATIONPLUGIN_H

#include "IPlugin.h"
#include "src/neuromodulation/Neuromodulator.hpp"
#include <string>

namespace nlm {
namespace plugin {

class INeuromodulationPlugin : public virtual IPlugin {
public:
    virtual ~INeuromodulationPlugin() = default;

    // Neuromodulator type
    virtual std::string getNeuromodulatorType() const = 0;

    // Neuromodulation
    virtual std::shared_ptr<Neuromodulator> createNeuromodulator() = 0;
    virtual double computeModulationSignal(double baseline, double predictionError) = 0;
    virtual double getModulationStrength() const = 0;

    // Neuromodulation configuration
    virtual void configureNeuromodulation() = 0;
    virtual double getTimeConstant() const = 0;
    virtual double getDecayRate() const = 0;

    // Signal processing parameters
    virtual std::map<std::string, double> getModulationParameters() const = 0;
    virtual void setModulationParameters(const std::map<std::string, double>& params) = 0;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_INEUROMODULATIONPLUGIN_H
