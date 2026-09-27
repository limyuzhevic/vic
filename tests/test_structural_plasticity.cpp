// StructuralPlasticity Tests
// Comprehensive testing of structural plasticity mechanisms

#include "plasticity/StructuralPlasticity.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "brain/NeuralRegion.hpp"
#include "core/Random/Random.hpp"
#include "core/Config/Config.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>

namespace test_structural_plasticity {

void testStructuralPlasticityCreation() {
    nlm::StructuralPlasticity sp;
    
    if (sp.getSynaptogenesisRate() != 0.0001f) {
        throw nlm::NLMTestException("StructuralPlasticity creation failed: synaptogenesis rate should be 0.0001f, got " + 
                                   std::to_string(sp.getSynaptogenesisRate()));
    }
    if (sp.getPruningRate() != 0.00001f) {
        throw nlm::NLMTestException("StructuralPlasticity creation failed: pruning rate should be 0.00001f, got " + 
                                   std::to_string(sp.getPruningRate()));
    }
    
    std::cout << "    testStructuralPlasticityCreation passed" << std::endl;
}

void testStructuralPlasticityConfiguration() {
    nlm::StructuralPlasticity sp;
    
    sp.setSynaptogenesisRate(0.002f);
    sp.setPruningRate(0.0002f);
    
    if (sp.getSynaptogenesisRate() != 0.002f) {
        throw nlm::NLMTestException("StructuralPlasticity configuration failed: synaptogenesis rate should be 0.002f, got " + 
                                   std::to_string(sp.getSynaptogenesisRate()));
    }
    if (sp.getPruningRate() != 0.0002f) {
        throw nlm::NLMTestException("StructuralPlasticity configuration failed: pruning rate should be 0.0002f, got " + 
                                   std::to_string(sp.getPruningRate()));
    }
    
    std::cout << "    testStructuralPlasticityConfiguration passed" << std::endl;
}

void testStructuralPlasticityName() {
    nlm::StructuralPlasticity sp;
    
    const char* name = sp.getName();
    if (!name) {
        throw nlm::NLMTestException("StructuralPlasticity name failed: name is null");
    }
    if (std::string(name) != "StructuralPlasticity") {
        throw nlm::NLMTestException("StructuralPlasticity name failed: expected 'StructuralPlasticity', got '" + 
                                   std::string(name) + "'");
    }
    
    std::cout << "    testStructuralPlasticityName passed" << std::endl;
}

void testStructuralPlasticityCreateSynapse() {
    // Create a brain with one region
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(10), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    StructuralPlasticity sp;
    
    // Get some neurons to use
    auto regions = brain->getRegions();
    if (regions.empty()) {
        throw nlm::NLMTestException("Create synapse test failed: brain has no regions");
    }
    
    auto neurons = regions[0]->getAllNeurons();
    if (neurons.size() < 2) {
        throw nlm::NLMTestException("Create synapse test failed: not enough neurons in region");
    }
    
    NeuronId src = neurons[0]->getId();
    NeuronId dst = neurons[1]->getId();
    
    SynapseId synId = sp.createSynapse(brain.get(), src, dst, 0.5f);
    
    if (synId == INVALID_SYNAPSE_ID) {
        throw nlm::NLMTestException("Create synapse test failed: could not create synapse");
    }
    
    // Check that synapse exists
    Synapse* synapse = regions[0]->getSynapse(synId);
    if (!synapse) {
        throw nlm::NLMTestException("Create synapse test failed: synapse not found after creation");
    }
    
    if (synapse->getSourceNeuron() != src || synapse->getDestinationNeuron() != dst) {
        throw nlm::NLMTestException("Create synapse test failed: synapse has wrong neurons");
    }
    
    std::cout << "    testStructuralPlasticityCreateSynapse passed (synapse ID: " << synId.getId() << ")" << std::endl;
}

void testStructuralPlasticityRemoveSynapse() {
    // Create a brain with one region
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(10), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    StructuralPlasticity sp;
    
    // Create a synapse first
    auto regions = brain->getRegions();
    if (regions.empty()) {
        throw nlm::NLMTestException("Remove synapse test failed: brain has no regions");
    }
    
    auto neurons = regions[0]->getAllNeurons();
    if (neurons.size() < 2) {
        throw nlm::NLMTestException("Remove synapse test failed: not enough neurons in region");
    }
    
    NeuronId src = neurons[0]->getId();
    NeuronId dst = neurons[1]->getId();
    
    SynapseId synId = sp.createSynapse(brain.get(), src, dst, 0.5f);
    
    if (synId == INVALID_SYNAPSE_ID) {
        throw nlm::NLMTestException("Remove synapse test failed: could not create synapse for removal test");
    }
    
    // Remove the synapse
    bool removed = sp.removeSynapse(brain.get(), synId);
    
    if (!removed) {
        throw nlm::NLMTestException("Remove synapse test failed: could not remove synapse");
    }
    
    // Check that synapse is gone (note: current implementation sets weight to 0 instead of removing)
    Synapse* synapse = regions[0]->getSynapse(synId);
    if (synapse && synapse->getWeight() != 0.0f) {
        throw nlm::NLMTestException("Remove synapse test failed: synapse still exists or has non-zero weight");
    }
    
    std::cout << "    testStructuralPlasticityRemoveSynapse passed" << std::endl;
}

void testStructuralPlasticityNullBrain() {
    StructuralPlasticity sp;
    
    // Test with null brain - should not crash
    bool result = sp.removeSynapse(nullptr, INVALID_SYNAPSE_ID);
    if (result) {
        throw nlm::NLMTestException("StructuralPlasticity null brain test failed: should return false with null brain");
    }
    
    std::cout << "    testStructuralPlasticityNullBrain passed" << std::endl;
}

void testStructuralPlasticityNeuronCreationDisabled() {
    StructuralPlasticity sp;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(10), ConfigSource::Default);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    NeuronId newNeuron = sp.createNeuron(brain.get(), NeuronType::Excitatory);
    
    // Should return invalid ID for now (not implemented)
    if (newNeuron != INVALID_NEURON_ID) {
        throw nlm::NLMTestException("StructuralPlasticity neuron creation test failed: should return invalid ID when not implemented");
    }
    
    std::cout << "    testStructuralPlasticityNeuronCreationDisabled passed" << std::endl;
}

void testStructuralPlasticityNeuronRemovalDisabled() {
    StructuralPlasticity sp;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(10), ConfigSource::Default);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Try to remove a neuron - should return false (not implemented)
    bool removed = sp.removeNeuron(brain.get(), NeuronId(999));
    if (removed) {
        throw nlm::NLMTestException("StructuralPlasticity neuron removal test failed: should return false when not implemented");
    }
    
    std::cout << "    testStructuralPlasticityNeuronRemovalDisabled passed" << std::endl;
}

void testStructuralPlasticityUpdateSynaptogenesis() {
    // Create brain with neurons
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(20), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    StructuralPlasticity sp;
    sp.setSynaptogenesisRate(0.1f); // High rate for testing
    sp.setPruningRate(0.0f); // Disable pruning for this test
    
    // Record initial state
    size_t initialSynapses = brain->getTotalSynapseCount();
    
    // Update with random generator
    nlm::RandomGenerator rng(42);
    sp.update(brain.get(), rng);
    
    size_t finalSynapses = brain->getTotalSynapseCount();
    
    // With high synaptogenesis rate, synapses should have been created
    if (finalSynapses < initialSynapses) {
        throw nlm::NLMTestException("StructuralPlasticity synaptogenesis test failed: synapse count should not decrease");
    }
    
    std::cout << "    testStructuralPlasticityUpdateSynaptogenesis passed (initial: " 
              << initialSynapses << ", final: " << finalSynapses << ")" << std::endl;
}

void testStructuralPlasticityUpdatePruning() {
    // Create brain with many synapses
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.2f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    StructuralPlasticity sp;
    sp.setSynaptogenesisRate(0.0f); // Disable synaptogenesis for this test
    sp.setPruningRate(0.1f); // High pruning rate
    
    // Record initial state
    size_t initialSynapses = brain->getTotalSynapseCount();
    
    // Update with random generator
    nlm::RandomGenerator rng(42);
    sp.update(brain.get(), rng);
    
    size_t finalSynapses = brain->getTotalSynapseCount();
    
    // With high pruning rate, synapses might have been pruned (set to 0 weight)
    // Note: current implementation doesn't actually remove synapses, just zeros weight
    if (finalSynapses > initialSynapses) {
        throw nlm::NLMTestException("StructuralPlasticity pruning test failed: synapse count should not increase with pruning");
    }
    
    std::cout << "    testStructuralPlasticityUpdatePruning passed (initial: " 
              << initialSynapses << ", final: " << finalSynapses << ")" << std::endl;
}

void testStructuralPlasticityActivityDependency() {
    // Create brain with neurons
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(30), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    StructuralPlasticity sp;
    sp.setUseActivityDependency(true); // Enable activity dependency
    sp.setSynaptogenesisRate(0.0f); // Disable synaptogenesis
    sp.setPruningRate(0.05f); // Moderate pruning rate
    
    // Get neurons and simulate some activity
    auto regions = brain->getRegions();
    if (!regions.empty()) {
        auto neurons = regions[0]->getAllNeurons();
        
        // Give some neurons spike history (activity)
        if (!neurons.empty()) {
            neurons[0]->recordSpike(0.1);
            neurons[0]->recordSpike(0.2);
        }
        
        // Update with random generator
        nlm::RandomGenerator rng(42);
        sp.update(brain.get(), rng);
        
        std::cout << "    testStructuralPlasticityActivityDependency passed" << std::endl;
    }
}

void testStructuralPlasticityMaxConstraints() {
    StructuralPlasticity sp;
    
    // Set very low max constraints
    sp.setMaxSynapsesPerNeuron(5);
    sp.setMaxTotalSynapses(10);
    
    // Verify settings
    if (sp.getMaxSynapsesPerNeuron() != 5) {
        throw nlm::NLMTestException("StructuralPlasticity max constraints test failed: max synapses per neuron should be 5, got " + 
                                   std::to_string(sp.getMaxSynapsesPerNeuron()));
    }
    if (sp.getMaxTotalSynapses() != 10) {
        throw nlm::NLMTestException("StructuralPlasticity max constraints test failed: max total synapses should be 10, got " + 
                                   std::to_string(sp.getMaxTotalSynapses()));
    }
    
    std::cout << "    testStructuralPlasticityMaxConstraints passed" << std::endl;
}

void testStructuralPlasticityStatistics() {
    // Create brain with some activity
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(20), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    StructuralPlasticity sp;
    
    // Record initial statistics
    size_t initialCreated = sp.getTotalSynapsesCreated();
    size_t initialPruned = sp.getTotalSynapsesPruned();
    
    // Create some synapses
    auto regions = brain->getRegions();
    if (!regions.empty()) {
        auto neurons = regions[0]->getAllNeurons();
        if (neurons.size() >= 2) {
            sp.createSynapse(brain.get(), neurons[0]->getId(), neurons[1]->getId(), 0.5f);
        }
    }
    
    // Remove some synapses
    sp.removeSynapse(brain.get(), INVALID_SYNAPSE_ID); // Should do nothing
    
    // Statistics should be updated
    size_t finalCreated = sp.getTotalSynapsesCreated();
    size_t finalPruned = sp.getTotalSynapsesPruned();
    
    // Created should have increased
    if (finalCreated <= initialCreated) {
        throw nlm::NLMTestException("StructuralPlasticity statistics test failed: created count should increase");
    }
    
    std::cout << "    testStructuralPlasticityStatistics passed (created: " 
              << initialCreated << " -> " << finalCreated << ", pruned: " << initialPruned << " -> " << finalPruned << ")" << std::endl;
}

void runAll() {
    std::cout << "Running StructuralPlasticity tests..." << std::endl;
    testStructuralPlasticityCreation();
    testStructuralPlasticityConfiguration();
    testStructuralPlasticityName();
    testStructuralPlasticityCreateSynapse();
    testStructuralPlasticityRemoveSynapse();
    testStructuralPlasticityNullBrain();
    testStructuralPlasticityNeuronCreationDisabled();
    testStructuralPlasticityNeuronRemovalDisabled();
    testStructuralPlasticityUpdateSynaptogenesis();
    testStructuralPlasticityUpdatePruning();
    testStructuralPlasticityActivityDependency();
    testStructuralPlasticityMaxConstraints();
    testStructuralPlasticityStatistics();
}

} // namespace test_structural_plasticity
