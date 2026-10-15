// Plugin Interface Tests
// Testing INeuronModelPlugin, IPlasticityRulePlugin, ISynapticDynamicsPlugin interfaces

#include "plugins/PluginInterface.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace nlm {
namespace tests {

namespace test_plugin_interfaces {

class TestNeuronModelPlugin : public INeuronModelPlugin {
public:
    TestNeuronModelPlugin() : INeuronModelPlugin("TestNeuronModel", "1.0") {}
    
    std::string getName() const override { return name; }
    std::string getVersion() const override { return version; }
    bool initialize(const Config& config) override { return true; }
    void shutdown() override {}
    void update(double timestep) override {}
    double calculateOutput(const Neuron& neuron, double input) override { return input * 1.0; }
    void applyPlasticity(Neuron& neuron, const PlasticityEvent& event) override {}
    bool validateParameters(const Config& params) const override { return true; }
    std::string getNeuronType() const override { return "TestNeuron"; }
};

class TestPlasticityRulePlugin : public IPlasticityRulePlugin {
public:
    TestPlasticityRulePlugin() : IPlasticityRulePlugin("TestPlasticityRule", "1.0") {}
    
    std::string getName() const override { return name; }
    std::string getVersion() const override { return version; }
    bool initialize(const Config& config) override { return true; }
    void shutdown() override {}
    void update(double timestep) override {}
    bool apply(const PlasticityEvent& event, Neuron& preNeuron, Neuron& postNeuron) override { return true; }
    bool validateParameters(const Config& params) const override { return true; }
    std::string getRuleType() const override { return "TestPlasticity"; }
    double getLearningRate() const override { return 0.01; }
};

class TestSynapticDynamicsPlugin : public ISynapticDynamicsPlugin {
public:
    TestSynapticDynamicsPlugin() : ISynapticDynamicsPlugin("TestSynapticDynamics", "1.0") {}
    
    std::string getName() const override { return name; }
    std::string getVersion() const override { return version; }
    bool initialize(const Config& config) override { return true; }
    void shutdown() override {}
    void update(double timestep) override {}
    double calculateWeightChange(const Synapse& synapse, const PlasticityEvent& event) override { return 0.1; }
    bool validateParameters(const Config& params) const override { return true; }
    std::string getDynamicsType() const override { return "TestSynapticDynamics"; }
    double getConductance() const override { return 1.0; }
};

void testNeuronModelPluginInterface() {
    auto plugin = std::make_shared<TestNeuronModelPlugin>();
    
    assert(plugin->getName() == "TestNeuronModel");
    assert(plugin->getVersion() == "1.0");
    assert(plugin->initialize(Config()) == true);
    
    std::cout << "    testNeuronModelPluginInterface passed" << std::endl;
}

void testPlasticityRulePluginInterface() {
    auto plugin = std::make_shared<TestPlasticityRulePlugin>();
    
    assert(plugin->getName() == "TestPlasticityRule");
    assert(plugin->getVersion() == "1.0");
    assert(plugin->initialize(Config()) == true);
    assert(plugin->getLearningRate() == 0.01);
    
    std::cout << "    testPlasticityRulePluginInterface passed" << std::endl;
}

void testSynapticDynamicsPluginInterface() {
    auto plugin = std::make_shared<TestSynapticDynamicsPlugin>();
    
    assert(plugin->getName() == "TestSynapticDynamics");
    assert(plugin->getVersion() == "1.0");
    assert(plugin->initialize(Config()) == true);
    assert(plugin->getConductance() == 1.0);
    
    std::cout << "    testSynapticDynamicsPluginInterface passed" << std::endl;
}

void testPluginInterfaceCompliance() {
    // Test that all required methods are implemented
    TestNeuronModelPlugin neuronPlugin;
    TestPlasticityRulePlugin plasticityPlugin;
    TestSynapticDynamicsPlugin synapticPlugin;
    
    // Check that all required methods exist (compile-time test)
    assert(neuronPlugin.getName() != "");
    assert(plasticityPlugin.getRuleType() != "");
    assert(synapticPlugin.getDynamicsType() != "");
    
    std::cout << "    testPluginInterfaceCompliance passed" << std::endl;
}

void testPluginMetadata() {
    TestNeuronModelPlugin plugin1;
    TestPlasticityRulePlugin plugin2;
    TestSynapticDynamicsPlugin plugin3;
    
    // Check that metadata is properly set
    assert(!plugin1.getName().empty());
    assert(!plugin1.getVersion().empty());
    assert(!plugin2.getName().empty());
    assert(!plugin2.getVersion().empty());
    assert(!plugin3.getName().empty());
    assert(!plugin3.getVersion().empty());
    
    std::cout << "    testPluginMetadata passed" << std::endl;
}

void runAll() {
    testNeuronModelPluginInterface();
    testPlasticityRulePluginInterface();
    testSynapticDynamicsPluginInterface();
    testPluginInterfaceCompliance();
    testPluginMetadata();
}

} // namespace test_plugin_interfaces

} // namespace nlm
namespace tests {

void test_plugin_interfaces() {
    test_plugin_interfaces::runAll();
}

} // namespace tests
