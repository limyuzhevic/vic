// Neural Dynamics Tests
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "dynamics/NeuralDynamics.hpp"
#include "core/Config/Config.hpp"
#include "nlm/test_framework.h"
#include <iostream>

namespace test_neural_dynamics {

TEST_CASE(testIntegrateAndFireDynamics) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setType(nlm::NeuronType::Excitatory);
    neuron.setMembranePotential(-65.0f);
    neuron.setThreshold(-55.0f);
    
    // Test IntegrateAndFireDynamics methods exist
    nlm::IntegrateAndFireDynamics lif;
    
    // Check that dynamics has a name
    ASSERT_EQ(std::string(lif.getName()), "IntegrateAndFire");
    
    // Test configuration loading
    auto config = std::make_shared<nlm::Config>();
    config->set("LIF_TIME_CONSTANT", 20.0, nlm::ConfigSource::Runtime);
    config->set("LIF_ENABLE_NOISE", false, nlm::ConfigSource::Runtime);
    lif.loadConfig(*config);
    
    // Test that LIF parameters can be set
    lif.setMembraneTimeConstant(20.0f);
    ASSERT_EQ(lif.getMembraneTimeConstant(), 20.0f);
    
    // Test basic dynamics
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(10.0f);
    
    // Should not fire yet (below threshold)
    ASSERT_FALSE(lif.shouldFire(&neuron));
    
    // Test firing
    neuron.setMembranePotential(-54.0f);  // Above threshold
    ASSERT_TRUE(lif.shouldFire(&neuron));
}

TEST_CASE(testHodgkinHuxleyDynamics) {
    nlm::Neuron neuron(nlm::NeuronId(2));
    neuron.setType(nlm::NeuronType::Excitatory);
    neuron.setMembranePotential(-65.0f);
    
    nlm::HodgkinHuxleyDynamics hh;
    
    // Check that HH dynamics has a name
    ASSERT_EQ(std::string(hh.getName()), "HodgkinHuxley");
    
    // Test HH-specific parameters
    hh.setTemperature(6.0f);
    ASSERT_FLOAT_EQ(hh.getTemperature(), 6.0f, 0.001f);
    
    hh.setSodiumConductance(120.0f);
    ASSERT_FLOAT_EQ(hh.getSodiumConductance(), 120.0f, 0.001f);
    
    hh.setPotassiumConductance(36.0f);
    ASSERT_FLOAT_EQ(hh.getPotassiumConductance(), 36.0f, 0.001f);
    
    hh.setLeakConductance(0.3f);
    ASSERT_FLOAT_EQ(hh.getLeakConductance(), 0.3f, 0.001f);
}

TEST_CASE(testFitzHughNagumoDynamics) {
    nlm::Neuron neuron(nlm::NeuronId(3));
    neuron.setType(nlm::NeuronType::Excitatory);
    neuron.setMembranePotential(-65.0f);
    
    nlm::FitzHughNagumoDynamics fhn;
    
    // Check that FHN dynamics has a name
    ASSERT_EQ(std::string(fhn.getName()), "FitzHughNagumo");
    
    // Test FHN-specific parameters
    fhn.setThreshold(-0.5f);
    ASSERT_FLOAT_EQ(fhn.getThreshold(), -0.5f, 0.001f);
    
    fhn.setGradient(0.8f);
    ASSERT_FLOAT_EQ(fhn.getGradient(), 0.8f, 0.001f);
    
    fhn.setBaseline(0.7f);
    ASSERT_FLOAT_EQ(fhn.getBaseline(), 0.7f, 0.001f);
    
    fhn.setRecoveryTime(12.5f);
    ASSERT_FLOAT_EQ(fhn.getRecoveryTime(), 12.5f, 0.001f);
}

TEST_CASE(testConfigurationIntegration) {
    auto config = std::make_shared<nlm::Config>();
    
    // Set dynamics configuration parameters
    config->set("DYNAMICS_MODEL", "HodgkinHuxley", nlm::ConfigSource::Runtime);
    config->set("HH_TEMPERATURE", 6.0, nlm::ConfigSource::Runtime);
    config->set("HH_GNa", 120.0, nlm::ConfigSource::Runtime);
    config->set("HH_GK", 36.0, nlm::ConfigSource::Runtime);
    
    // Test that config can be loaded by HH dynamics
    nlm::HodgkinHuxleyDynamics hh;
    hh.loadConfig(*config);
    
    ASSERT_FLOAT_EQ(hh.getTemperature(), 6.0f, 0.001f);
}

std::vector<TestResult> runAllTests() {
    return test_neural_dynamics::runAllTests();
}

} // namespace test_neural_dynamics

// Simple test runner for backward compatibility
void runAll() {
    auto results = test_neural_dynamics::runAllTests();
    printTestSummary(results);
}
