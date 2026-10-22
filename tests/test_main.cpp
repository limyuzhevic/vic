// Test runner for NLM
#include "core/test/TestFramework.hpp"
#include <iostream>

namespace test_types {
    void testNeuronId();
    void testSynapseId();
    void testRegionId();
    void testPopulationId();
    void testNeuronType();
    void testSynapseType();
    void testPlasticityFlags();
    void testSpikeEvent();
    void runAll();
}

namespace test_synapse {
    void testSynapseCreation();
    void testSynapseWeight();
    void testSynapseWeightBounds();
    void testSynapseType();
    void testSynapseDelay();
    void testSynapseSpikeHistory();
    void testSynapseEligibilityTrace();
    void testSynapseEfficacy();
    void testSynapseRandomInitialization();
    void testSynapseReset();
    void testSynapsePlasticityFlags();
    void runAll();
}

namespace test_stdp {
    void testSTDPCreation();
    void testSTDPConfiguration();
    void testSTDPConfigure();
    void testSTDPName();
    void testSTDPPotentiation();
    void testSTDPDepression();
    void testSTDPEmptySpikes();
    void testSTDPWeightBounds();
    void runAll();
}

namespace test_clock {
    void testClockCreation();
    void testClockAdvance();
    void testClockReset();
    void testClockSetStep();
    void testClockSetTime();
    void testClockAdvanceBy();
    void testClockConversions();
    void testClockFirstLastStep();
    void testClockTimeAtFutureStep();
    void runAll();
}

namespace test_neuron {
    void testNeuronCreation();
    void testNeuronType();
    void testMembranePotential();
    void testFiringState();
    void testRefractoryDecrement();
    void testCurrentInjection();
    void testSynapticInput();
    void testSpikeHistory();
    void testRandomInitialization();
    void testReset();
    void runAll();
}

namespace test_config {
    void testConfigCreation();
    void testConfigSetAndGet();
    void testConfigGetOr();
    void testConfigRemove();
    void testConfigKeys();
    void runAll();
}

namespace test_random {
    void testRandomGeneratorCreation();
    void testUniformInt();
    void testUniformReal();
    void testNormal();
    void testBernoulli();
    void testRandomBool();
    void testPickRandomIndex();
    void testShuffle();
    void testSeed();
    void runAll();
}

namespace test_brain {
    void testBrainCreation();
    void testBrainInitialization();
    void testBrainStep();
    void testBrainReset();
    void testBrainRegions();
    void testBrainActionProduction();
    void testBrainStatus();
    void runAll();
}

int main() {
    std::cout << "NLM Test Framework" << std::endl;
    std::cout << "===================" << std::endl;
    
    try {
        // Run all tests
        test_config::runAll();
        test_random::runAll();
        test_clock::runAll();
        test_neuron::runAll();
        test_synapse::runAll();
        test_stdp::runAll();
        test_brain::runAll();
        test_types::runAll();
        
        std::cout << std::endl;
        std::cout << "All tests passed successfully!" << std::endl;
        return 0;
    }
    catch (const nlm::test::TestFailureException& e) {
        std::cout << std::endl;
        std::cout << "TEST FAILED: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}