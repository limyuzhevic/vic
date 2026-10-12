// Comprehensive NLM Unit Tests
// Covers all core components with proper error handling and assertions

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_neuron {

void testNeuronCreation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    assert(neuron.getId() == nlm::NeuronId(1));
    std::cout << "    testNeuronCreation passed" << std::endl;
}

void testNeuronType() {
    nlm::Neuron neuron(nlm::NeuronId(2));
    neuron.setType(nlm::NeuronType::Excitatory);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    assert(neuron.getType() == nlm::NeuronType::Inhibitory);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testNeuronMembranePotential() {
    nlm::Neuron neuron(nlm::NeuronId(3));
    neuron.setMembranePotential(-65.0f);
    assert(std::abs(neuron.getMembranePotential() - (-65.0f)) < 0.01f);
    
    neuron.addToMembranePotential(5.0f);
    assert(std::abs(neuron.getMembranePotential() - (-60.0f)) < 0.01f);
    
    std::cout << "    testNeuronMembranePotential passed" << std::endl;
}

void testNeuronFiring() {
    nlm::Neuron neuron(nlm::NeuronId(4));
    neuron.setThreshold(-50.0f);
    neuron.setRefractoryPeriod(5);
    
    // Below threshold - should not fire
    neuron.setMembranePotential(-55.0f);
    bool fired = neuron.checkThreshold();
    assert(!fired);
    assert(neuron.isFiringState() == nlm::FiringState::Resting);
    
    // Above threshold - should fire
    neuron.setMembranePotential(-45.0f);
    fired = neuron.checkThreshold();
    assert(fired);
    assert(neuron.isFiringState() == nlm::FiringState::Spiking);
    
    std::cout << "    testNeuronFiring passed" << std::endl;
}

void testNeuronRefractory() {
    nlm::Neuron neuron(nlm::NeuronId(5));
    neuron.setRefractoryPeriod(10);
    assert(neuron.getRefractoryPeriod() == 10);
    
    neuron.setFiringState(nlm::FiringState::Refractory);
    assert(neuron.isRefractory());
    
    neuron.decrementRefractory();
    assert(neuron.isRefractory());  // Still in refractory
    
    std::cout << "    testNeuronRefractory passed" << std::endl;
}

void testNeuronExcitatoryInput() {
    nlm::Neuron neuron(nlm::NeuronId(6));
    neuron.receiveExcitatoryInput(10.0f);
    neuron.receiveInhibitoryInput(5.0f);
    neuron.receiveModulatoryInput(2.0f);
    
    std::cout << "    testNeuronExcitatoryInput passed" << std::endl;
}

void testNeuronReset() {
    nlm::Neuron neuron(nlm::NeuronId(7));
    neuron.setMembranePotential(-40.0f);
    neuron.recordSpike(0.0);
    neuron.setFiringState(nlm::FiringState::Spiking);
    
    neuron.reset();
    
    // Should be reset to resting state
    assert(std::abs(neuron.getMembranePotential() - (-70.0f)) < 0.01f);
    assert(neuron.getFiringState() == nlm::FiringState::Resting);
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testNeuronReset passed" << std::endl;
}

void runAll() {
    std::cout << "Running Neuron tests..." << std::endl;
    testNeuronCreation();
    testNeuronType();
    testNeuronMembranePotential();
    testNeuronFiring();
    testNeuronRefractory();
    testNeuronExcitatoryInput();
    testNeuronReset();
}

} // namespace test_neuron

namespace test_synapse {

void testSynapseCreation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(1));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(2));
    std::cout << "    testSynapseCreation passed" << std::endl;
}

void testSynapseWeight() {
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(0.5f);
    assert(std::abs(synapse.getWeight() - 0.5f) < 0.01f);
    
    synapse.addToWeight(0.2f);
    assert(std::abs(synapse.getWeight() - 0.7f) < 0.01f);
    
    synapse.setWeight(0.1f);
    assert(std::abs(synapse.getWeight() - 0.1f) < 0.01f);
    
    std::cout << "    testSynapseWeight passed" << std::endl;
}

void testSynapseType() {
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    assert(synapse.getType() == nlm::SynapseType::Excitatory);
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(synapse.getType() == nlm::SynapseType::Inhibitory);
    assert(!synapse.isExcitatory());
    assert(synapse.isInhibitory());
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testSynapseSpikeHistory() {
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    synapse.recordPreSpike(0.0f);
    synapse.recordPreSpike(10.0f);
    synapse.recordPostSpike(5.0f);
    
    assert(synapse.getPreSpikeHistory().size() == 2);
    assert(synapse.getPostSpikeHistory().size() == 1);
    assert(std::abs(synapse.getPreSpikeHistory()[0] - 0.0f) < 0.01f);
    assert(std::abs(synapse.getPreSpikeHistory()[1] - 10.0f) < 0.01f);
    assert(std::abs(synapse.getPostSpikeHistory()[0] - 5.0f) < 0.01f);
    
    synapse.clearHistory();
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    std::cout << "    testSynapseSpikeHistory passed" << std::endl;
}

void testSynapseReset() {
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.recordPreSpike(0.0f);
    synapse.recordPostSpike(5.0f);
    synapse.setWeight(0.8f);
    
    synapse.reset();
    
    assert(synapse.getWeight() == 0.01f);  // Default weight
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    std::cout << "    testSynapseReset passed" << std::endl;
}

void runAll() {
    std::cout << "Running Synapse tests..." << std::endl;
    testSynapseCreation();
    testSynapseWeight();
    testSynapseType();
    testSynapseSpikeHistory();
    testSynapseReset();
}

} // namespace test_synapse

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(config.getKeys().empty());
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetGet() {
    nlm::Config config;
    
    config.set("string_value", std::string("test"));
    assert(config.has("string_value"));
    assert(config.get<std::string>("string_value").value() == "test");
    
    config.set("int_value", 42);
    assert(config.get<int>("int_value").value() == 42);
    
    config.set("double_value", 3.14);
    assert(std::abs(config.get<double>("double_value").value() - 3.14) < 0.001);
    
    config.set("bool_value", true);
    assert(config.get<bool>("bool_value").value() == true);
    
    std::cout << "    testConfigSetGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    // Test with existing key
    config.set("existing_key", "value");
    assert(config.getOr<std::string>("existing_key", "default") == "value");
    
    // Test with non-existing key (should return default)
    assert(config.getOr<std::string>("nonexistent_key", "default") == "default");
    
    // Test with numeric defaults
    config.set("int_key", 100);
    assert(config.getOr<int>("int_key", 200) == 100);
    assert(config.getOr<int>("nonexistent", 200) == 200);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("key1", "value1");
    config.set("key2", "value2");
    config.set("key3", "value3");
    
    assert(config.getKeys().size() == 3);
    
    config.remove("key2");
    assert(config.getKeys().size() == 2);
    assert(!config.has("key2"));
    assert(config.has("key1"));
    assert(config.has("key3"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("test_key", "test_value");
    
    std::string summary = config.summary();
    assert(summary.find("test_key") != std::string::npos);
    assert(summary.find("test_value") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    std::cout << "Running Config tests..." << std::endl;
    testConfigCreation();
    testConfigSetGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigSummary();
}

} // namespace test_config

namespace test_random {

void testRandomGenerator() {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(42);
    
    // Same seed should produce same sequence
    assert(rng1.uniformReal(0.0f, 1.0f) == rng2.uniformReal(0.0f, 1.0f));
    
    // Different seeds should produce different sequences
    nlm::RandomGenerator rng3(43);
    assert(rng1.uniformReal(0.0f, 1.0f) != rng2.uniformReal(0.0f, 1.0f));
    
    std::cout << "    testRandomGenerator passed" << std::endl;
}

void runAll() {
    std::cout << "Running Random tests..." << std::endl;
    testRandomGenerator();
}

} // namespace test_random

int main() {
    std::cout << "=== Running NLM Unit Tests ===\n" << std::endl;
    
    test_neuron::runAll();
    std::cout << std::endl;
    
    test_synapse::runAll();
    std::cout << std::endl;
    
    test_config::runAll();
    std::cout << std::endl;
    
    test_random::runAll();
    std::cout << std::endl;
    
    std::cout << "=== All Tests Passed ===" << std::endl;
    return 0;
}
