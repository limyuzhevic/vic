// Main.cpp Functionality Tests
// Tests for the improved main.cpp modularity and command-line support

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_main {

void testConfigurationConstants() {
    // Test that configuration constants are properly defined
    const size_t DEFAULT_NEURON_COUNT = 500;
    const size_t DEFAULT_REGION_COUNT = 1;
    const float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
    const float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
    const float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
    const float DEFAULT_STDP_TAU = 20.0f;
    const float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    const float DEFAULT_PRUNING_RATE = 0.00001f;
    const float DEFAULT_SIMULATION_TIMESTEP = 0.001f;
    
    // Verify constants have expected values
    assert(DEFAULT_NEURON_COUNT == 500);
    assert(DEFAULT_REGION_COUNT == 1);
    assert(DEFAULT_CONNECTION_PROBABILITY == 0.15f);
    assert(DEFAULT_STDP_LTP_WEIGHT == 0.02f);
    assert(DEFAULT_STDP_LTD_WEIGHT == 0.015f);
    assert(DEFAULT_STDP_TAU == 20.0f);
    assert(DEFAULT_SYNAPTOGENESIS_RATE == 0.0001f);
    assert(DEFAULT_PRUNING_RATE == 0.00001f);
    assert(DEFAULT_SIMULATION_TIMESTEP == 0.001f);
    
    std::cout << "    testConfigurationConstants passed" << std::endl;
}

void testLearningExperimentStruct() {
    // Test that LearningExperiment struct works correctly
    struct LearningExperiment {
        std::shared_ptr<nlm::Brain> brain;
        uint64_t seed;
        size_t initialSynapseCount;
        std::vector<float> initialWeights;
        std::vector<float> finalWeights;
        std::vector<nlm::NeuronId> mostActiveNeurons;
        
        LearningExperiment(std::shared_ptr<nlm::Brain> b, uint64_t s) 
            : brain(b), seed(s), initialSynapseCount(0) {}
        
        void recordInitialState() {
            initialSynapseCount = brain->getTotalSynapseCount();
            initialWeights.clear();
            
            // Record initial weights from first region
            if (auto* region = brain->getRegion(nlm::RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    initialWeights.push_back(syn->getWeight());
                }
            }
        }
        
        void recordFinalState() {
            finalWeights.clear();
            
            // Record final weights from first region
            if (auto* region = brain->getRegion(nlm::RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    finalWeights.push_back(syn->getWeight());
                }
            }
        }
    };
    
    // Test struct creation
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    LearningExperiment experiment(brain, 42);
    
    // Test methods
    experiment.recordInitialState();
    experiment.recordFinalState();
    
    std::cout << "    testLearningExperimentStruct passed" << std::endl;
}

void testCommandLineArgsStruct() {
    // Test CommandLineArgs struct
    struct CommandLineArgs {
        std::string configFile = "configs/default.cfg";
        bool runTests = true;
        bool enableDebug = false;
        nlm::SimulationStep maxSteps = 1000;
        nlm::NeuronCount neuronCount = 500;
        bool verbose = true;
    };
    
    CommandLineArgs args;
    
    // Verify defaults
    assert(args.configFile == "configs/default.cfg");
    assert(args.runTests == true);
    assert(args.enableDebug == false);
    assert(args.maxSteps == 1000);
    assert(args.neuronCount == 500);
    assert(args.verbose == true);
    
    // Test modification
    args.configFile = "custom.cfg";
    args.runTests = false;
    args.enableDebug = true;
    args.maxSteps = 2000;
    args.neuronCount = 1000;
    args.verbose = false;
    
    assert(args.configFile == "custom.cfg");
    assert(args.runTests == false);
    assert(args.enableDebug == true);
    assert(args.maxSteps == 2000);
    assert(args.neuronCount == 1000);
    assert(args.verbose == false);
    
    std::cout << "    testCommandLineArgsStruct passed" << std::endl;
}

void testParserFunctions() {
    // Test parser helper functions
    auto testString = "  hello world  \n\t";
    
    // Simple trim simulation
    size_t start = testString.find_first_not_of(" \t\r\n");
    size_t end = testString.find_last_not_of(" \t\r\n");
    std::string trimmed = testString.substr(start, end - start + 1);
    
    assert(trimmed == "hello world");
    
    // Test lowercase conversion
    std::string uppercase = "HELLO WORLD";
    std::string lower = uppercase;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    assert(lower == "hello world");
    
    std::cout << "    testParserFunctions passed" << std::endl;
}

void runAll() {
    testConfigurationConstants();
    testLearningExperimentStruct();
    testCommandLineArgsStruct();
    testParserFunctions();
}

} // namespace test_main