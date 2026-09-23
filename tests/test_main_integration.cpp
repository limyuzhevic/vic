// Main Integration Tests
// Tests for the complete NLM system integration
// #include "core/Config/Config.hpp"
// #include "core/Random/Random.hpp"
// #include "core/Logger/Logger.hpp"
// #include "brain/Brain.hpp"
// #include "brain/Neuron.hpp"
// #include "brain/Synapse.hpp"
// #include <cassert>
// #include <iostream>
// #include <cmath>

namespace test_main {

void testCompleteBrainInitialization() {
    std::cout << "    Testing complete brain initialization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    bool init_success = brain.initialize();
    assert(init_success);
    
    // Verify all systems are initialized
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "      Complete brain initialization test passed" << std::endl;
}

void testBrainWithPlasticity() {
    std::cout << "    Testing brain with plasticity..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(30), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Enable plasticity
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    // Record initial weights
    std::vector<float> initialWeights;
    if (region) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    // Apply learning stimulus
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 15.0f);
        brain.step(step);
    }
    
    // Record final weights
    std::vector<float> finalWeights;
    if (region) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    // Check that weights changed (plasticity occurred)
    bool weightsChanged = false;
    for (size_t i = 0; i < std::min(initialWeights.size(), finalWeights.size()); ++i) {
        if (std::abs(initialWeights[i] - finalWeights[i]) > 0.001f) {
            weightsChanged = true;
            break;
        }
    }
    
    assert(weightsChanged);
    
    std::cout << "      Brain plasticity test passed" << std::endl;
}

void testBrainActionSelection() {
    std::cout << "    Testing brain action selection..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(40), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Inject current to motor neurons to trigger action
    brain.injectCurrentToNeurons(nlm::NeuronType::Motor, 25.0f);
    brain.step(0);
    
    // Get action
    auto action = brain.produceAction();
    assert(action != nullptr);
    
    // Action should be MoveForward (due to motor neuron activity)
    assert(action->getType() == nlm::ActionType::MoveForward);
    
    std::cout << "      Brain action selection test passed" << std::endl;
}

void testBrainCheckpointing() {
    std::cout << "    Testing brain checkpointing..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create a checkpoint file
    std::string checkpointFile = "test_checkpoint.bin";
    bool saveSuccess = brain.save(checkpointFile);
    assert(saveSuccess);
    
    // Create a new brain and load the checkpoint
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config2);
    brain2.initialize();
    
    bool loadSuccess = brain2.load(checkpointFile);
    assert(loadSuccess);
    
    // Clean up
    std::remove(checkpointFile.c_str());
    
    std::cout << "      Brain checkpointing test passed" << std::endl;
}

void testConfigJSONRoundTrip() {
    std::cout << "    Testing config JSON round trip..." << std::endl;
    
    nlm::Config config;
    
    // Set various configuration values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_value", "test_string", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("double_array", std::vector<double>({1.1, 2.2, 3.3}), nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    std::string jsonFile = "test_config.json";
    bool saveSuccess = config.saveToFile(jsonFile);
    assert(saveSuccess);
    
    // Create a new config and load from JSON
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(jsonFile);
    assert(loadSuccess);
    
    // Verify values were loaded correctly
    auto intVal = config2.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config2.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.14 && *doubleVal < 3.142);
    
    auto stringVal = config2.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "test_string");
    
    auto boolVal = config2.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Clean up
    std::remove(jsonFile.c_str());
    
    std::cout << "      Config JSON round trip test passed" << std::endl;
}

void testCommandLineInterface() {
    std::cout << "    Testing command line interface..." << std::endl;
    
    // Test argument parsing
    int argc = 5;
    char* argv[] = {
        (char*)"test",
        (char*)"--neurons",
        (char*)"100",
        (char*)"--regions",
        (char*)"2"
    };
    
    NLMConfig config = parseCommandLine(argc, argv);
    
    assert(config.neuronCount == 100);
    assert(config.regionCount == 2);
    assert(config.experimentMode == "all");  // default
    
    std::cout << "      Command line interface test passed" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "    Testing configuration validation..." << std::endl;
    
    NLMConfig config;
    
    // Test valid configuration
    config.neuronCount = 100;
    config.regionCount = 2;
    config.connectionProbability = 0.15f;
    config.simulationTimestep = 0.001;
    
    assert(config.validate());
    
    // Test invalid configurations
    NLMConfig invalidConfig;
    
    // Invalid: zero neurons
    invalidConfig.neuronCount = 0;
    assert(!invalidConfig.validate());
    
    // Invalid: negative connection probability
    invalidConfig.neuronCount = 100;
    invalidConfig.connectionProbability = -0.1f;
    assert(!invalidConfig.validate());
    
    // Invalid: zero timestep
    invalidConfig.connectionProbability = 0.15f;
    invalidConfig.simulationTimestep = 0.0;
    assert(!invalidConfig.validate());
    
    std::cout << "      Configuration validation test passed" << std::endl;
}

void testExperimentHistory() {
    std::cout << "    Testing experiment history..." << std::endl;
    
    ExperimentHistory history("test_history.json");
    
    // Add some experiment results
    history.addExperiment("Basic Connectivity Test", "PASSED", "Spikes propagated through network");
    history.addExperiment("Plasticity Test", "PASSED", "Synaptic weights changed significantly");
    history.addExperiment("STDP Test", "PASSED", "Pre-before-post produced potentiation");
    
    // Verify experiments were added
    assert(history.getExperimentCount() == 3);
    
    // Save and reload
    history.saveToFile();
    
    // Verify file exists
    assert(std::ifstream("test_history.json").good());
    
    // Clean up
    std::remove("test_history.json");
    
    std::cout << "      Experiment history test passed" << std::endl;
}

void runAll() {
    std::cout << "=== Main Integration Tests ===" << std::endl;
    
    testCompleteBrainInitialization();
    testBrainWithPlasticity();
    testBrainActionSelection();
    testBrainCheckpointing();
    testConfigJSONRoundTrip();
    testCommandLineInterface();
    testConfigurationValidation();
    testExperimentHistory();
    
    std::cout << std::endl;
    std::cout << "=== All Main Integration Tests Passed! ===" << std::endl;
}

} // namespace test_main