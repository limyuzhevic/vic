// NLM Test Runner
// Main test runner for the NLM framework

#include <iostream>
#include <string>

// Include test namespaces
namespace test_neuron { void testNeuronTestRunner(); }
namespace test_synapse { void testSynapseTestRunner(); }
namespace test_stdp { void testSTDPTestRunner(); }
namespace test_clock { void testClockTestRunner(); }
namespace test_config { void testConfigTestRunner(); }
namespace test_random { void testRandomTestRunner(); }
namespace test_types { void testTypesTestRunner(); }
namespace test_memory { void testMemoryTestRunner(); }
namespace test_neuromodulation { void testNeuromodulationTestRunner(); }
namespace test_plasticity { void testHebbianTestRunner(); }

void runAllTests() {
    std::cout << "================================================================" << std::endl;
    std::cout << "NLM (Neural Learning Machine) Test Suite" << std::endl;
    std::cout << "Framework: Phase 6 - Comprehensive Neural Simulation" << std::endl;
    std::cout << "================================================================" << std::endl;
    std::cout << std::endl;
    
    // Run basic component tests
    std::cout << "Running basic component tests..." << std::endl;
    test_neuron::runAll();
    test_synapse::runAll();
    test_stdp::runAll();
    test_clock::runAll();
    test_config::runAll();
    test_random::runAll();
    test_types::runAll();
    
    std::cout << std::endl;
    
    // Run advanced system tests
    std::cout << "Running advanced system tests..." << std::endl;
    test_memory::runAll();
    test_neuromodulation::runAll();
    test_plasticity::runAll();
    
    std::cout << std::endl;
    
    // Summary
    std::cout << "================================================================" << std::endl;
    std::cout << "Test Suite Complete!" << std::endl;
    std::cout << "================================================================" << std::endl;
    std::cout << "Coverage Summary:" << std::endl;
    std::cout << "- Basic Components: Neuron, Synapse, STDP, Clock, Config, Random, Types" << std::endl;
    std::cout << "- Memory Systems: Working Memory, Episodic Memory, Associative Memory" << std::endl;
    std::cout << "- Neuromodulation: Dopamine, Reward, Curiosity, PredictionError, Novelty" << std::endl;
    std::cout << "- Plasticity: Hebbian Learning" << std::endl;
    std::cout << std::endl;
    std::cout << "Note: Many components still under development (Phase 2 TODOs)" << std::endl;
    std::cout << "================================================================" << std::endl;
}

int main() {
    runAllTests();
    return 0;
}