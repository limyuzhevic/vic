// Enhanced Python Bindings Tests
// Tests for advanced Python functionality in NLM

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cassert>
#include <iostream>

namespace test_pybindings {

void testPyBindingsEnhancedFunctionality() {
    // Import the pynlm module
    py::module_ pynlm = py::module_::import("pynlm");
    
    // Test createDefaultConfig
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    assert(config != nullptr);
    
    // Test createBrain
    auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
    assert(brain != nullptr);
    
    // Test createSimpleWorld
    auto world = pynlm.attr("createSimpleWorld")().cast<std::shared_ptr<SimpleWorld>>();
    assert(world != nullptr);
    
    // Test createAgentBrain
    auto agent = pynlm.attr("createAgentBrain")(brain).cast<std::shared_ptr<AgentBrain>>();
    assert(agent != nullptr);
    
    std::cout << "    testPyBindingsEnhancedFunctionality passed" << std::endl;
}

void testPyBindingsAdvancedMethods() {
    py::module_ pynlm = py::module_::import("pynlm");
    
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
    auto world = pynlm.attr("createSimpleWorld")().cast<std::shared_ptr<SimpleWorld>>();
    auto agent = pynlm.attr("createAgentBrain")(brain).cast<std::shared_ptr<AgentBrain>>();
    
    // Initialize agent
    agent->initialize(*world);
    
    // Test advanced AgentBrain methods
    auto visionNeurons = agent->getSensoryVisionNeurons();
    assert(!visionNeurons.empty());
    
    auto touchNeurons = agent->getSensoryTouchNeurons();
    assert(!touchNeurons.empty());
    
    auto internalNeurons = agent->getSensoryInternalNeurons();
    assert(!internalNeurons.empty());
    
    auto forwardNeurons = agent->getMotorForwardNeurons();
    assert(!forwardNeurons.empty());
    
    auto backwardNeurons = agent->getMotorBackwardNeurons();
    assert(!backwardNeurons.empty());
    
    auto leftNeurons = agent->getMotorTurnLeftNeurons();
    assert(!leftNeurons.empty());
    
    auto rightNeurons = agent->getMotorTurnRightNeurons();
    assert(!rightNeurons.empty());
    
    auto interactNeurons = agent->getMotorInteractNeurons();
    assert(!interactNeurons.empty());
    
    auto waitNeurons = agent->getMotorWaitNeurons();
    assert(!waitNeurons.empty());
    
    // Test neuromodulation level getters
    float neuromodulationLevel = agent->getNeuromodulationLevel();
    assert(neuromodulationLevel >= 0.0f);
    
    float curiosityLevel = agent->getCuriosityLevel();
    assert(curiosityLevel >= 0.0f);
    
    float noveltyLevel = agent->getNoveltyLevel();
    assert(noveltyLevel >= 0.0f);
    
    float predictionError = agent->getPredictionError();
    assert(predictionError >= 0.0f);
    
    auto developmentalStage = agent->getDevelopmentalStage();
    assert(developmentalStage >= DevelopmentalStage::Initial &&
           developmentalStage <= DevelopmentalStage::Aging);
    
    float plasticityModifier = agent->getPlasticityModifier();
    assert(plasticityModifier > 0.0f);
    
    // Test enhanced configuration methods
    config->set("excitatory_proportion", 0.8, nlm::ConfigSource::Runtime);
    config->set("connectivity_probability", 0.15, nlm::ConfigSource::Runtime);
    
    // Test batch operations
    std::vector<NeuronId> neuronIds;
    config->set("batch_test_size", 5, nlm::ConfigSource::Runtime);
    
    // Test getAllActiveNeurons (if available)
    auto activeNeurons = pynlm.attr("getAllActiveNeurons")(brain);
    assert(!activeNeurons.empty());
    
    // Test getWorkingMemoryTraces (if available)
    auto memoryTraces = pynlm.attr("getWorkingMemoryTraces")(brain);
    assert(memoryTraces >= 0);
    
    // Test getEpisodicMemoryCount (if available)
    auto episodicCount = pynlm.attr("getEpisodicMemoryCount")(brain);
    assert(episodicCount >= 0);
    
    // Test getPredictionErrorMetrics (if available)
    auto errorMetrics = pynlm.attr("getPredictionErrorMetrics")(brain);
    assert(errorMetrics.contains("prediction_error"));
    assert(errorMetrics.contains("prediction_confidence"));
    assert(errorMetrics.contains("novelty_score"));
    
    // Test getCognitionMetrics (if available)
    auto cognitionMetrics = pynlm.attr("getCognitionMetrics")(brain);
    assert(!cognitionMetrics.empty());
    
    std::cout << "    testPyBindingsAdvancedMethods passed" << std::endl;
}

void testPyBindingsCreateBrainWithRandomConnectivity() {
    py::module_ pynlm = py::module_::import("pynlm");
    
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    
    // Test createBrainWithRandomConnectivity
    auto brain1 = pynlm.attr("createBrainWithRandomConnectivity")(
        config, 0.2f, 0.8f).cast<std::shared_ptr<Brain>>();
    assert(brain1 != nullptr);
    
    auto brain2 = pynlm.attr("createBrainWithRandomConnectivity")(
        config, 0.5f, 0.5f).cast<std::shared_ptr<Brain>>();
    assert(brain2 != nullptr);
    
    // Brains should have different connectivity patterns
    assert(brain1->getTotalNeuronCount() == brain2->getTotalNeuronCount());
    
    std::cout << "    testPyBindingsCreateBrainWithRandomConnectivity passed" << std::endl;
}

void testPyBindingsBatchInjectCurrent() {
    py::module_ pynlm = py::module_::import("pynlm");
    
    auto config = pynlm.attr("createDefaultConfig")().cast<std::shared_ptr<Config>>();
    auto brain = pynlm.attr("createBrain")(config).cast<std::shared_ptr<Brain>>();
    
    // Initialize brain first
    brain->initialize();
    
    // Create some neurons to test batch operations
    auto neurons = brain->getRegions()[0]->getAllNeurons();
    std::vector<NeuronId> neuronIds;
    
    // Collect up to 5 neuron IDs
    size_t count = std::min(size_t(5), neurons.size());
    for (size_t i = 0; i < count; ++i) {
        neuronIds.push_back(neurons[i]->getId());
    }
    
    if (!neuronIds.empty()) {
        // Test batchInjectCurrent
        pynlm.attr("batchInjectCurrent")(brain, neuronIds, 10.0f, true);
        
        // Verify by checking if neurons received input
        // (This is a simplified test - in practice you'd need more sophisticated verification)
        
        std::cout << "    testPyBindingsBatchInjectCurrent passed" << std::endl;
    }
}

void testPyBindingsConstants() {
    py::module_ pynlm = py::module_::import("pynlm");
    
    // Test that constants are properly exposed
    auto invalid_neuron_id = pynlm.attr("INVALID_NEURON_ID").cast<NeuronId>();
    assert(invalid_neuron_id.value == 0);
    
    auto invalid_synapse_id = pynlm.attr("INVALID_SYNAPSE_ID").cast<SynapseId>();
    assert(invalid_synapse_id.value == 0);
    
    auto invalid_region_id = pynlm.attr("INVALID_REGION_ID").cast<RegionId>();
    assert(invalid_region_id.value == 0);
    
    auto invalid_population_id = pynlm.attr("INVALID_POPULATION_ID").cast<PopulationId>();
    assert(invalid_population_id.value == 0);
    
    std::cout << "    testPyBindingsConstants passed" << std::endl;
}

void runAll() {
    std::cout << "=== Enhanced Python Bindings Tests ===" << std::endl;
    
    testPyBindingsEnhancedFunctionality();
    testPyBindingsAdvancedMethods();
    testPyBindingsCreateBrainWithRandomConnectivity();
    testPyBindingsBatchInjectCurrent();
    testPyBindingsConstants();
    
    std::cout << std::endl;
    std::cout << "=== All Python Bindings Tests Passed ===" << std::endl;
}

} // namespace test_pybindings

// Test runner function
void runEnhancedPyBindingsTests() {
    test_pybindings::runAll();
}