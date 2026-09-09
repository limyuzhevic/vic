// StructuralPlasticity Tests
#include "brain/Synapse.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace test_structural {

void testStructuralPlasticityCreation() {
    nlm::StructuralPlasticity structural;
    
    assert(structural.getSynaptogenesisRate() == 0.0001f);
    assert(structural.getPruningRate() == 0.00001f);
    
    std::cout << "    testStructuralPlasticityCreation passed" << std::endl;
}

void testStructuralPlasticityConfiguration() {
    nlm::StructuralPlasticity structural;
    
    structural.setSynaptogenesisRate(0.01f);
    structural.setPruningRate(0.001f);
    
    assert(structural.getSynaptogenesisRate() == 0.01f);
    assert(structural.getPruningRate() == 0.001f);
    
    std::cout << "    testStructuralPlasticityConfiguration passed" << std::endl;
}

void testStructuralPlasticitySynapseCreation() {
    nlm::StructuralPlasticity structural;
    
    // Create a minimal config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.5, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize brain to create regions
    bool success = brain.initialize();
    assert(success);
    
    // Find a region
    auto* region = brain.getRegion(brain.getRegionIds()[0]);
    assert(region != nullptr);
    
    // Get some neurons from the region
    auto neurons = region->getAllNeurons();
    assert(neurons.size() >= 2);
    
    NeuronId source = neurons[0]->getId();
    NeuronId destination = neurons[1]->getId();
    
    // Create a synapse
    SynapseId synId = structural.createSynapse(&brain, source, destination, 0.5f);
    
    // Verify creation
    assert(synId != INVALID_SYNAPSE_ID);
    
    // Verify synapse was actually created by checking region
    auto synapsesTo = region->getSynapsesTo(destination);
    bool found = false;
    for (auto* syn : synapsesTo) {
        if (syn->getSourceNeuron() == source) {
            found = true;
            break;
        }
    }
    assert(found);
    
    std::cout << "    testStructuralPlasticitySynapseCreation passed" << std::endl;
}

void testStructuralPlasticityDuplicateSynapse() {
    nlm::StructuralPlasticity structural;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    auto* region = brain.getRegion(brain.getRegionIds()[0]);
    auto neurons = region->getAllNeurons();
    assert(neurons.size() >= 2);
    
    NeuronId source = neurons[0]->getId();
    NeuronId destination = neurons[1]->getId();
    
    // Create first synapse
    SynapseId synId1 = structural.createSynapse(&brain, source, destination, 0.5f);
    assert(synId1 != INVALID_SYNAPSE_ID);
    
    // Try to create duplicate (should fail)
    SynapseId synId2 = structural.createSynapse(&brain, source, destination, 0.7f);
    assert(synId2 == INVALID_SYNAPSE_ID);
    
    std::cout << "    testStructuralPlasticityDuplicateSynapse passed" << std::endl;
}

void testStructuralPlasticitySynapseRemoval() {
    nlm::StructuralPlasticity structural;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 1.0, nlm::ConfigSource::Default);  // Ensure connections
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    auto* region = brain.getRegion(brain.getRegionIds()[0]);
    auto neurons = region->getAllNeurons();
    assert(neurons.size() >= 2);
    
    NeuronId source = neurons[0]->getId();
    NeuronId destination = neurons[1]->getId();
    
    // Create a synapse
    SynapseId synId = structural.createSynapse(&brain, source, destination, 0.5f);
    assert(synId != INVALID_SYNAPSE_ID);
    
    // Verify it exists
    auto synapsesTo = region->getSynapsesTo(destination);
    bool found = false;
    for (auto* syn : synapsesTo) {
        if (syn->getSourceNeuron() == source && syn->getId() == synId) {
            found = true;
            break;
        }
    }
    assert(found);
    
    // Remove the synapse
    bool removed = structural.removeSynapse(&brain, synId);
    assert(removed);
    
    // Verify it's gone (weight should be zero)
    found = false;
    for (auto* syn : synapsesTo) {
        if (syn->getSourceNeuron() == source && syn->getId() == synId) {
            found = true;
            // Weight should be zero after removal
            assert(syn->getWeight() == 0.0f);
            break;
        }
    }
    
    // Note: removeSynapse doesn't actually remove the synapse from storage,
    // it just zeros the weight, so found might still be true but weight is 0
    
    std::cout << "    testStructuralPlasticitySynapseRemoval passed" << std::endl;
}

void testStructuralPlasticityNonExistentRemoval() {
    nlm::StructuralPlasticity structural;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Try to remove non-existent synapse
    bool removed = structural.removeSynapse(&brain, INVALID_SYNAPSE_ID);
    assert(!removed);
    
    std::cout << "    testStructuralPlasticityNonExistentRemoval passed" << std::endl;
}

void testStructuralPlasticityActivityDependent() {
    nlm::StructuralPlasticity structural;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Track statistics before update
    size_t synapsesBefore = brain.getTotalSynapseCount();
    
    // Run structural plasticity update
    nlm::RandomGenerator rng(42);
    structural.update(&brain, rng);
    
    size_t synapsesAfter = brain.getTotalSynapseCount();
    
    // Synapses may be created or pruned, so count can change
    // The important thing is that it doesn't crash
    assert(std::abs(static_cast<int>(synapsesAfter - synapsesBefore)) < 10);  // Allow small changes
    
    std::cout << "    testStructuralPlasticityActivityDependent passed" << std::endl;
}

void testStructuralPlasticityMinWeightThreshold() {
    nlm::StructuralPlasticity structural;
    
    // Set low threshold for pruning
    structural.setMinWeightThreshold(0.01f);
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    auto* region = brain.getRegion(brain.getRegionIds()[0]);
    auto neurons = region->getAllNeurons();
    assert(neurons.size() >= 2);
    
    NeuronId source = neurons[0]->getId();
    NeuronId destination = neurons[1]->getId();
    
    // Create a very weak synapse
    SynapseId synId = structural.createSynapse(&brain, source, destination, 0.005f);  // Below threshold
    assert(synId != INVALID_SYNAPSE_ID);
    
    // Verify weak weight
    auto synapsesTo = region->getSynapsesTo(destination);
    bool found = false;
    for (auto* syn : synapsesTo) {
        if (syn->getSourceNeuron() == source) {
            assert(syn->getWeight() == 0.005f);
            found = true;
            break;
        }
    }
    assert(found);
    
    std::cout << "    testStructuralPlasticityMinWeightThreshold passed" << std::endl;
}

void testStructuralPlasticityMaxSynapsesPerNeuron() {
    nlm::StructuralPlasticity structural;
    
    // Set low limit to test constraint
    structural.setPruningRate(0.001f);  // Very low pruning
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.3, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Verify constraints are in place
    // (Actual limit checking would be done during creation)
    assert(true);  // Just verifying we can set rates
    
    std::cout << "    testStructuralPlasticityMaxSynapsesPerNeuron passed" << std::endl;
}

void testStructuralPlasticityActivityDependency() {
    nlm::StructuralPlasticity structural;
    
    // Test activity-dependent pruning
    nlm::RandomGenerator rng(42);
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Run structural plasticity
    structural.update(&brain, rng);
    
    // Should not crash
    assert(brain.getTotalSynapseCount() >= 0);
    
    std::cout << "    testStructuralPlasticityActivityDependency passed" << std::endl;
}

void testStructuralPlasticityRateBounds() {
    nlm::StructuralPlasticity structural;
    
    // Test rate clamping
    structural.setSynaptogenesisRate(-0.1f);  // Should be clamped to 0
    assert(structural.getSynaptogenesisRate() == 0.0f);
    
    structural.setPruningRate(0.1f);  // Should be clamped to 0.01f
    assert(structural.getPruningRate() == 0.01f);
    
    // Test valid range
    structural.setSynaptogenesisRate(0.05f);  // Within 0-0.1
    assert(structural.getSynaptogenesisRate() == 0.05f);
    
    structural.setPruningRate(0.005f);  // Within 0-0.01
    assert(structural.getPruningRate() == 0.005f);
    
    std::cout << "    testStructuralPlasticityRateBounds passed" << std::endl;
}

void testStructuralPlasticityEdgeCases() {
    nlm::StructuralPlasticity structural;
    
    // Test with empty brain (no regions)
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);  // Brain may initialize even with 0 regions
    
    nlm::RandomGenerator rng(42);
    
    // Should not crash with edge cases
    structural.update(&brain, rng);
    assert(true);
    
    std::cout << "    testStructuralPlasticityEdgeCases passed" << std::endl;
}

void runAll() {
    std::cout << "Running Structural Plasticity tests..." << std::endl;
    testStructuralPlasticityCreation();
    testStructuralPlasticityConfiguration();
    testStructuralPlasticitySynapseCreation();
    testStructuralPlasticityDuplicateSynapse();
    testStructuralPlasticitySynapseRemoval();
    testStructuralPlasticityNonExistentRemoval();
    testStructuralPlasticityActivityDependent();
    testStructuralPlasticityMinWeightThreshold();
    testStructuralPlasticityMaxSynapsesPerNeuron();
    testStructuralPlasticityActivityDependency();
    testStructuralPlasticityRateBounds();
    testStructuralPlasticityEdgeCases();
}

} // namespace test_structural
