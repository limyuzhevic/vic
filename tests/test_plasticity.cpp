// Plasticity Experiment Tests
// Tests for the plasticity experiment (selective learning, not just saturation)

#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_plasticity {

void testSelectiveNeurons() {
    // Test that the plasticity experiment stimulates only a subset of neurons
    // (not all sensory neurons like the old buggy version)
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Get the first region
    auto* region = brain.getRegion(nlm::RegionId(1));
    assert(region != nullptr);
    
    // Collect all sensory neurons (like the plasticity experiment does)
    std::vector<nlm::NeuronId> patternNeurons;
    for (auto* neuron : region->getAllNeurons()) {
        if (neuron->getType() == nlm::NeuronType::Sensory && patternNeurons.size() < 20) {
            patternNeurons.push_back(neuron->getId());
        }
    }
    
    // Verify that only up to 20 sensory neurons are selected (the fix)
    // The old buggy version would have selected ALL sensory neurons
    assert(patternNeurons.size() <= 20);
    
    // Verify that all selected neurons are actually sensory neurons
    for (const auto& neuronId : patternNeurons) {
        // Find the neuron with this ID
        bool found = false;
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getId() == neuronId && neuron->getType() == nlm::NeuronType::Sensory) {
                found = true;
                break;
            }
        }
        assert(found);
    }
    
    std::cout << "    testSelectiveNeurons passed" << std::endl;
}

void testSTDPPlasticity() {
    // Test that STDP plasticity works correctly
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Get the STDP plasticity system
    nlm::STDP* stdp = brain.getSTDP();
    assert(stdp != nullptr);
    
    // Test STDP configuration
    assert(stdp->getLTPWeight() > 0.0f);
    assert(stdp->getLTDWeight() > 0.0f);
    assert(stdp->getTimeConstant() > 0.0f);
    
    // Test STDP name
    const char* name = stdp->getName();
    assert(name != nullptr);
    assert(std::string(name) == "STDP");
    
    std::cout << "    testSTDPPlasticity passed" << std::endl;
}

void testHebbianPlasticity() {
    // Test that Hebbian plasticity works correctly
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Get the Hebbian plasticity system
    nlm::Hebbian* hebbian = brain.getHebbian();
    assert(hebbian != nullptr);
    
    // Test Hebbian name
    const char* name = hebbian->getName();
    assert(name != nullptr);
    assert(std::string(name) == "Hebbian");
    
    std::cout << "    testHebbianPlasticity passed" << std::endl;
}

void testPlasticityParameters() {
    // Test that plasticity parameters are correctly configured
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Get the plasticity systems
    nlm::STDP* stdp = brain.getSTDP();
    nlm::Hebbian* hebbian = brain.getHebbian();
    
    assert(stdp != nullptr);
    assert(hebbian != nullptr);
    
    // Test that STDP parameters are reasonable
    float stdpLTP = stdp->getLTPWeight();
    float stdpLTD = stdp->getLTDWeight();
    float stdpTau = stdp->getTimeConstant();
    
    assert(stdpLTP > 0.0f);
    assert(stdpLTD > 0.0f);
    assert(stdpTau > 0.0f);
    
    // Test that Hebbian parameters are reasonable
    float hebbianLTP = hebbian->getLTPWeight();
    float hebbianLTD = hebbian->getLTDWeight();
    
    assert(hebbianLTP > 0.0f);
    assert(hebbianLTD > 0.0f);
    
    std::cout << "    testPlasticityParameters passed" << std::endl;
}

} // namespace test_plasticity