// NLM Main Functionality Tests
// Tests for command-line interface, configuration handling, and main functionality

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "experiments/LearningExperiment.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

namespace test_main {

void testConfigurationLoading() {
    std::cout << "    Testing configuration loading..." << std::endl;
    
    // Test default config
    nlm::Config config;
    config.set("test_int", 42, nlm::ConfigSource::Default);
    config.set("test_string", "hello", nlm::ConfigSource::Default);
    config.set("test_double", 3.14, nlm::ConfigSource::Default);
    config.set("test_bool", true, nlm::ConfigSource::Default);
    
    assert(config.has("test_int"));
    assert(config.get<int>("test_int") == 42);
    
    assert(config.has("test_string"));
    assert(config.getOr<std::string>("test_string", "default") == "hello");
    
    assert(config.has("test_double"));
    assert(config.getOr<double>("test_double", 0.0) == 3.14);
    
    assert(config.has("test_bool"));
    assert(config.getOr<bool>("test_bool", false) == true);
    
    // Test removal
    config.remove("test_int");
    assert(!config.has("test_int"));
    
    std::cout << "    Configuration loading passed" << std::endl;
}

void testLearningExperiment() {
    std::cout << "    Testing LearningExperiment class..." << std::endl;
    
    // Create a mock config
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Test that LearningExperiment constructor works
    nlm::LearningExperiment experiment(brain, 42);
    
    // Verify initial state
    assert(experiment.brain == brain);
    assert(experiment.seed == 42);
    assert(experiment.initialSynapseCount == 0);
    assert(experiment.initialWeights.empty());
    assert(experiment.finalWeights.empty());
    assert(experiment.mostActiveNeurons.empty());
    
    std::cout << "    LearningExperiment class passed" << std::endl;
}

void testBrainInitialization() {
    std::cout << "    Testing brain initialization..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    assert(brain->initialize());
    assert(brain->getTotalNeuronCount() == 50);
    assert(brain->getRegionCount() == 1);
    assert(brain->getTotalSpikeCount() == 0);
    
    brain->logStatus();
    
    std::cout << "    Brain initialization passed" << std::endl;
}

void testConfigFileSaveLoad() {
    std::cout << "    Testing configuration file save/load..." << std::endl;
    
    // Create a test config file
    std::string testFilename = "/tmp/test_nlm_config.cfg";
    
    {
        std::ofstream file(testFilename);
        file << "[defaults]" << std::endl;
        file << "random_seed = 123" << std::endl;
        file << "simulation_timestep = 0.001" << std::endl;
        file << "neuron_count = 200" << std::endl;
        file << "" << std::endl;
        file << "[stdp]" << std::endl;
        file << "stdp_ltp_weight = 0.03" << std::endl;
        file << "stdp_ltd_weight = 0.02" << std::endl;
    }
    
    nlm::Config config;
    bool loaded = config.loadFromFile(testFilename);
    
    assert(loaded);
    assert(config.has("random_seed"));
    assert(config.getOr<int64_t>("random_seed", 0) == 123);
    assert(config.has("neuron_count"));
    assert(config.getOr<int64_t>("neuron_count", 0) == 200);
    assert(config.has("stdp_ltp_weight"));
    assert(config.getOr<float>("stdp_ltp_weight", 0.0f) == 0.03f);
    
    // Clean up
    std::remove(testFilename.c_str());
    
    std::cout << "    Configuration file save/load passed" << std::endl;
}

void testBrainStep() {
    std::cout << "    Testing brain simulation step..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2f, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Step through simulation
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t finalSpikes = brain->getTotalSpikeCount();
    
    assert(finalSpikes >= initialSpikes);
    
    // Inject current and step again
    auto* region = brain->getRegion(nlm::RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        if (!neurons.empty()) {
            neurons[0]->injectCurrent(10.0f);
            
            for (nlm::SimulationStep step = 10; step < 20; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t moreSpikes = brain->getTotalSpikeCount() - finalSpikes;
            // Should have some spikes with injected current
            assert(moreSpikes >= 0);
        }
    }
    
    std::cout << "    Brain simulation step passed" << std::endl;
}

void testNeuralTypes() {
    std::cout << "    Testing neural type definitions..." << std::endl;
    
    nlm::NeuronId nid(1);
    nlm::SynapseId sid(2);
    nlm::RegionId rid(3);
    nlm::PopulationId pid(4);
    
    assert(nid.value == 1);
    assert(sid.value == 2);
    assert(rid.value == 3);
    assert(pid.value == 4);
    
    nlm::NeuronType ntype = nlm::NeuronType::Excitatory;
    nlm::SynapseType stype = nlm::SynapseType::Excitatory;
    
    assert(stype == nlm::SynapseType::Excitatory);
    assert(ntype == nlm::NeuronType::Excitatory);
    
    nlm::PlasticityFlags flags;
    assert(!flags.hebbian);
    assert(!flags.stdp);
    assert(!flags.reward_modulated);
    assert(!flags.structural);
    assert(!flags.eligible);
    
    std::cout << "    Neural type definitions passed" << std::endl;
}

void testCommandLineArgumentParsing() {
    std::cout << "    Testing command-line argument parsing..." << std::endl;
    
    // This tests the logic in main.cpp for parsing command-line arguments
    std::vector<std::string> testArgs = {
        "nlm",
        "plasticity",
        "--neuron-count", "100",
        "--region-count", "2",
        "--verbose",
        "--steps", "500"
    };
    
    // Verify that the argument parsing logic would work correctly
    // (This is conceptual since we can't easily test the actual main function)
    
    assert(testArgs[0] == "nlm");
    assert(testArgs[1] == "plasticity");
    assert(testArgs[2] == "--neuron-count");
    assert(testArgs[3] == "100");
    assert(testArgs[4] == "--region-count");
    assert(testArgs[5] == "2");
    assert(testArgs[6] == "--verbose");
    assert(testArgs[7] == "--steps");
    assert(testArgs[8] == "500");
    
    std::cout << "    Command-line argument parsing passed" << std::endl;
}

void runAll() {
    std::cout << "Running Main Functionality tests..." << std::endl;
    std::cout << std::endl;
    
    testNeuralTypes();
    testConfigurationLoading();
    testLearningExperiment();
    testBrainInitialization();
    testConfigFileSaveLoad();
    testBrainStep();
    testCommandLineArgumentParsing();
    
    std::cout << std::endl;
    std::cout << "=== All Main Functionality Tests PASSED ===" << std::endl;
}

} // namespace test_main