#ifndef NLM_PLUGIN_ISENSORYPLUGIN_H
#define NLM_PLUGIN_ISENSORYPLUGIN_H

#include "IPlugin.h"
#include "src/sensory/SensoryInput.hpp"
#include <string>
#include <vector>

namespace nlm {
namespace plugin {

class ISensoryPlugin : public virtual IPlugin {
public:
    virtual ~ISensoryPlugin() = default;

    // Sensory modality name
    virtual std::string getSensoryModality() const = 0;

    // Input processing
    virtual std::shared_ptr<SensoryInput> createSensoryInput() = 0;
    virtual std::vector<SensoryInput> processInput(const std::vector<SensoryInput>& inputs) = 0;

    // Sensory configuration
    virtual void configureSensorySystem() = 0;
    virtual double getInputGain() const = 0;
    virtual double getNoiseLevel() const = 0;

    // Sensory processing parameters
    virtual std::map<std::string, double> getProcessingParameters() const = 0;
    virtual void setProcessingParameters(const std::map<std::string, double>& params) = 0;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_ISENSORYPLUGIN_H
