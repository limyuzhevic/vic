#ifndef NLM_PLUGIN_IENVIRONMENTPLUGIN_H
#define NLM_PLUGIN_IENVIRONMENTPLUGIN_H

#include "IPlugin.h"
#include "src/environment/Environment.hpp"
#include "src/environment/Observation.hpp"
#include "src/environment/Action.hpp"
#include <string>
#include <vector>

namespace nlm {
namespace plugin {

class IEnvironmentPlugin : public virtual IPlugin {
public:
    virtual ~IEnvironmentPlugin() = default;

    // Environment type
    virtual std::string getEnvironmentType() const = 0;

    // Environment simulation
    virtual std::shared_ptr<Environment> createEnvironment() = 0;
    virtual std::vector<Observation> simulatePhysics(const std::vector<Observation>& inputs) = 0;
    virtual std::vector<Action> applyForces(const std::vector<Action>& actions) = 0;

    // Environment configuration
    virtual void configureEnvironment() = 0;
    virtual double getTimeStep() const = 0;
    virtual double getGravity() const = 0;
    virtual bool isDynamic() const = 0;

    // Physics parameters
    virtual std::map<std::string, double> getPhysicsParameters() const = 0;
    virtual void setPhysicsParameters(const std::map<std::string, double>& params) = 0;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_IENVIRONMENTPLUGIN_H
