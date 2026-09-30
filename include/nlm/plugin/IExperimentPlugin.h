#ifndef NLM_PLUGIN_IEXPERIMENTPLUGIN_H
#define NLM_PLUGIN_IEXPERIMENTPLUGIN_H

#include "IPlugin.h"
#include "src/experiments/Experiment.hpp"
#include <string>
#include <vector>

namespace nlm {
namespace plugin {

class IExperimentPlugin : public virtual IPlugin {
public:
    virtual ~IExperimentPlugin() = default;

    // Experiment type
    virtual std::string getExperimentType() const = 0;

    // Experiment management
    virtual std::shared_ptr<Experiment> createExperiment() = 0;
    virtual std::vector<double> runExperiment(const std::map<std::string, double>& parameters) = 0;
    virtual bool isExperimentComplete() const = 0;

    // Experiment configuration
    virtual void configureExperiment() = 0;
    virtual size_t getMaxTrials() const = 0;
    virtual double getTimeLimit() const = 0;
    virtual std::string getMeasurementType() const = 0;

    // Data collection
    virtual std::vector<std::vector<double>> getCollectedData() const = 0;
    virtual std::vector<std::string> getMetricNames() const = 0;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_IEXPERIMENTPLUGIN_H
