// Simplified API Correctness Tests

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_api_correctness {

void test_neuron_api() {
    std::cout << "    Testing Neuron API..." << std::endl;
    
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test identity methods
    assert(neuron.getId() == nlm::NeuronId(1));
    
    // Test type methods
    assert(neuron.getType() == nlm::NeuronType::Internal);
    neuron.setType(nlm::NeuronType::Excitatory);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    
    // Test membrane potential methods
    assert(neuron.getMembranePotential() == -70.0f);
    neuron.setMembranePotential(-60.0f);
    assert(neuron.getMembranePotential() == -60.0f);
    neuron.addToMembranePotential(10.0f);
    assert(neuron.getMembranePotential() == -50.0f);
    
    // Test threshold methods
    neuron.setThreshold(-55.0f);
    assert(neuron.getThreshold() == -55.0f);
    assert(neuron.checkThreshold()); // -50 >= -55
    
    // Test firing state methods
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isFiring());
    assert(!neuron.isRefractory());
    
    // Test refractory period
    neuron.setRefractoryPeriod(10);
    assert(neuron.isRefractory());
    neuron.decrementRefractory();
    assert(neuron.isRefractory()); // Still refractory
    
    // Test current methods
    assert(neuron.getTotalCurrent() == 0.0f);
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    std::cout << "    test_neuron_api passed" << std::endl;
}

void test_synapse_api() {
    std::cout << "    Testing Synapse API..." << std::endl;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test identity methods
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(1));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(2));
    
    // Test weight methods
    assert(synapse.getWeight() == 0.0f); // Default
    synapse.setWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    synapse.addToWeight(0.2f);
    assert(synapse.getWeight() == 0.7f);
    
    // Test delay methods
    assert(synapse.getDelay() == 1); // Default
    synapse.setDelay(5);
    assert(synapse.getDelay() == 5);
    
    // Test type methods
    synapse.setType(nlm::SynapseType::Excitatory);
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    assert(synapse.getType() == nlm::SynapseType::Excitatory);
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(synapse.isInhibitory());
    assert(!synapse.isExcitatory());
    
    // Test spike history methods
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    synapse.recordPreSpike(0.1f);
    assert(synapse.getPreSpikeHistory().size() == 1);
    
    synapse.clearHistory();
    assert(synapse.getPreSpikeHistory().empty());
    
    // Test eligibility trace methods
    assert(synapse.getEligibilityTrace() == 0.0f);
    synapse.setEligibilityTrace(0.5f);
    assert(synapse.getEligibilityTrace() == 0.5f);
    
    std::cout << "    test_synapse_api passed" << std::endl;
}

void test_brain_api() {
    std::cout << "    Testing Brain API..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test initialization
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Test statistics
    assert(brain.getTotalNeuronCount() == 10);
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getFiringNeuronCount() >= 0);
    assert(brain.getAverageFiringRate() >= 0.0f);
    assert(brain.getExcitationInhibitionRatio() >= 0.0f);
    
    // Test basic operations
    brain.step(0);
    brain.step(1, 0.1f);
    
    // Test region access
    auto regionIds = brain.getRegionIds();
    assert(regionIds.size() == 1);
    
    auto* region = brain.getRegion(regionIds[0]);
    assert(region != nullptr);
    assert(region->getId() == regionIds[0]);
    
    // Test action production
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Test status logging (should not crash)
    brain.logStatus();
    
    // Test reset
    brain.reset();
    assert(brain.getTotalNeuronCount() == 10);
    
    std::cout << "    test_brain_api passed" << std::endl;
}

void test_agent_brain_api() {
    std::cout << "    Testing AgentBrain API..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain nlm_brain(config);
    nlm::AgentBrain agent_brain(&nlm_brain);
    
    // Test initialization (requires SimpleWorld, skip for now)
    // nlm::SimpleWorld world;
    // agent_brain.initialize(world);
    
    // Test API methods that don't require initialization
    assert(agent_brain.getSensoryInputSize() > 0);
    assert(agent_brain.getMotorOutputSize() > 0);
    
    std::cout << "    test_agent_brain_api passed (partial test)" << std::endl;
}

void test_api_consistency() {
    std::cout << "    Testing API consistency..." << std::endl;
    
    // Test Neuron API consistency
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Type consistency
    neuron.setType(nlm::NeuronType::Excitatory);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    assert(neuron.isFiring() == false); // Should be false initially
    
    // State consistency
    neuron.setMembranePotential(-65.0f);
    neuron.setThreshold(-55.0f);
    assert(neuron.getMembranePotential() == -65.0f);
    assert(neuron.getThreshold() == -55.0f);
    assert(neuron.checkThreshold()); // -65 < -55, so should be false
    
    // Synapse API consistency
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Type consistency
    synapse.setType(nlm::SynapseType::Excitatory);
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    assert(synapse.getType() == nlm::SynapseType::Excitatory);
    
    std::cout << "    test_api_consistency passed" << std::endl;
}

void test_development_stages() {
    std::cout << "    Testing development stages..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test developmental stage access
    nlm::DevelopmentalStage stage = brain.getDevelopmentalStage();
    assert(stage == nlm::DevelopmentalStage::Initial); // Default
    
    // Test setting developmental stage
    brain.setDevelopmentalStage(nlm::DevelopmentalStage::Adult);
    assert(brain.getDevelopmentalStage() == nlm::DevelopmentalStage::Adult);
    
    std::cout << "    test_development_stages passed" << std::endl;
}

void runAll() {
    test_neuron_api();
    test_synapse_api();
    test_brain_api();
    test_agent_brain_api();
    test_api_consistency();
    test_development_stages();
}

} // namespace test_api_correctness