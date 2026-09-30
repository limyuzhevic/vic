#ifndef NLM_PLUGIN_IMOTORPLUGIN_H
#define NLM_PLUGIN_IMOTORPLUGIN_H

#include "IPlugin.h"
#include "src/motor/MotorSystem.hpp"
#include "src/motor/Action.hpp"
#include <string>
#include <vector>

namespace nlm {
namespace plugin {

class IMotorPlugin : public virtual IPlugin {
public:
    virtual ~IMotorPlugin() = default;

    // Motor command type
    virtual std::string getMotorCommandType() const = 0;

    // Output generation
    virtual std::vector<Action> generateMotorCommands(const std::vector<Action>& previousActions) = 0;
    virtual std::vector<Action> processMotorOutput(const std::vector<Action>& motorOutput) = 0;

    // Motor control configuration
    virtual void configureMotorControl() = 0;
    virtual double getOutputGain() const = 0;
    virtual double getExplorationRate() const = 0;

    // Motor learning parameters
    virtual std::map<std::string, double> getMotorParameters() const = 0;
    virtual void setMotorParameters(const std::map<std::string, double>& params) = 0;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_IMOTORPLUGIN_H
