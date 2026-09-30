#ifndef NLM_PLUGIN_INEURALPLUGIN_H
#define NLM_PLUGIN_INEURALPLUGIN_H

#include "IPlugin.h"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/plasticity/PlasticityRule.hpp"
#include "src/brain/NeuralPopulation.hpp"
#include "src/dynamics/NeuralDynamics.hpp"
#include <string>

namespace nlm {
namespace plugin {

class INeuralPlugin : public virtual IPlugin {
public:
    virtual ~INeuralPlugin() = default;

    // Neuronal type creation
    virtual std::string getNeuronTypeName() const = 0;
    virtual std::shared_ptr<Neuron> createNeuron(double x, double y, double z) = 0;

    // Plasticity rules
    virtual std::string getPlasticityRuleName() const = 0;
    virtual std::shared_ptr<PlasticityRule> createPlasticityRule() = 0;

    // Neural population configuration
    virtual void configureNeuralPopulation(NeuralPopulation& population) const = 0;

    // Custom neural dynamics
    virtual bool configureNeuralDynamics(NeuralDynamics& dynamics) const = 0;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_INEURALPLUGIN_H
