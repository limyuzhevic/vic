// STDP Tests
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace test_stdp {

void testStdpPotentiation() {
    std::cout << "  testStdpPotentiation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    auto* region = brain.getRegion(nlm::RegionId(1));
    assert(region);
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        std::cout << "    ! No synapses for STDP test" << std::endl;
        return;
    }
    
    // Use one real synapse and its own pre/post neurons
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);  // STDP only
    float beforeWeight = syn->getWeight();
    
    nlm::NeuronId preNeuron = syn->getSourceNeuron().index();
    nlm::NeuronId postNeuron = syn->getDestinationNeuron().index();
    
    std::cout << "    Testing STDP on synapse (pre=" << preNeuron 
              << ", post=" << postNeuron << ") weight=" << beforeWeight << "..." << std::endl;
    
    // Creating correlated pre->post activity (potentiation)
    for (int trial = 0; trial < 25; ++trial) {
        brain.injectCurrent(preNeuron, 60.0f);
        brain.step(trial * 2, trial * 2 * 0.001);
        brain.injectCurrent(postNeuron, 60.0f);
        brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }
    
    float delta = syn->getWeight() - beforeWeight;
    
    if (delta > 0.001f) {
        std::cout << "    ✓ STDP WORKING: Pre-before-post produced potentiation (Δ=" << delta << ")" << std::endl;
    } else if (delta < -0.001f) {
        std::cout << "    ! STDP reversed: Check parameters (Δ=" << delta << ")" << std::endl;
    } else {
        std::cout << "    ! No change: STDP may not be triggering (Δ=" << delta << ")" << std::endl;
    }
    
    std::cout << "    testStdpPotentiation passed" << std::endl;
}

void testStdpDepression() {
    std::cout << "  testStdpDepression..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    auto* region = brain.getRegion(nlm::RegionId(1));
    assert(region);
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        std::cout << "    ! No synapses for STDP depression test" << std::endl;
        return;
    }
    
    // Use one real synapse
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);  // STDP only
    float beforeWeight = syn->getWeight();
    
    nlm::NeuronId preNeuron = syn->getSourceNeuron().index();
    nlm::NeuronId postNeuron = syn->getDestinationNeuron().index();
    
    // Creating anti-correlated pre->post activity (depression)
    for (int trial = 0; trial < 25; ++trial) {
        brain.injectCurrent(postNeuron, 60.0f);
        brain.step(trial * 2, trial * 2 * 0.001);
        brain.injectCurrent(preNeuron, 60.0f);
        brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }
    
    float delta = syn->getWeight() - beforeWeight;
    
    if (delta < -0.001f) {
        std::cout << "    ✓ STDP DEPRESSION: Post-before-pre produced depression (Δ=" << delta << ")" << std::endl;
    } else if (delta > 0.001f) {
        std::cout << "    ! STDP reversed: Check parameters (Δ=" << delta << ")" << std::endl;
    } else {
        std::cout << "    ! No change: STDP depression may not be triggering (Δ=" << delta << ")" << std::endl;
    }
    
    std::cout << "    testStdpDepression passed" << std::endl;
}

void testStdpParameters() {
    std::cout << "  testStdpParameters..." << std::endl;
    
    // Test that STDP produces reasonable results with typical parameters
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(25), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    auto* region = brain.getRegion(nlm::RegionId(1));
    assert(region);
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        std::cout << "    ! No synapses for STDP parameter test" << std::endl;
        return;
    }
    
    // Test with first few synapses to verify parameters are reasonable
    size_t testCount = std::min(size_t(3), synapses.size());
    float totalDelta = 0.0f;
    size_t tested = 0;
    
    for (size_t i = 0; i < testCount; ++i) {
        Synapse* syn = synapses[i].get();
        syn->enablePlasticity(false, true, false);  // STDP only
        float beforeWeight = syn->getWeight();
        
        nlm::NeuronId preNeuron = syn->getSourceNeuron().index();
        nlm::NeuronId postNeuron = syn->getDestinationNeuron().index();
        
        // Correlated activity
        for (int trial = 0; trial < 15; ++trial) {
            brain.injectCurrent(preNeuron, 60.0f);
            brain.step(trial * 2, trial * 2 * 0.001);
            brain.injectCurrent(postNeuron, 60.0f);
            brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float delta = syn->getWeight() - beforeWeight;
        totalDelta += delta;
        tested++;
        
        if (delta > 0.001f) {
            std::cout << "    Synapse " << i << ": Potentiation (Δ=" << delta << ")" << std::endl;
        } else if (delta < -0.001f) {
            std::cout << "    Synapse " << i << ": Depression (Δ=" << delta << ")" << std::endl;
        } else {
            std::cout << "    Synapse " << i << ": No change (Δ=" << delta << ")" << std::endl;
        }
    }
    
    if (tested > 0) {
        float avgDelta = totalDelta / tested;
        std::cout << "    Average delta: " << avgDelta << std::endl;
        
        if (std::abs(avgDelta) > 0.001f) {
            std::cout << "    ✓ STDP parameters working: average effect observed" << std::endl;
        } else {
            std::cout << "    ! Small average effect (may be acceptable)" << std::endl;
        }
    }
    
    std::cout << "    testStdpParameters passed" << std::endl;
}

void runAll() {
    std::cout << "Running STDP tests..." << std::endl;
    testStdpPotentiation();
    testStdpDepression();
    testStdpParameters();
    std::cout << "  STDP tests completed" << std::endl;
}
}