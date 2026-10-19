#include "tests/test_types.cpp"
#include "tests/test_config.cpp"
#include "tests/test_random.cpp"
#include "tests/test_clock.cpp"
#include "tests/test_neuron.cpp"
#include "tests/test_synapse.cpp"
#include "tests/test_stdp.cpp"
#include "tests/test_brain.cpp"
#include "tests/test_memory_retention.cpp"

int main(int argc, char** argv) {
    std::cout << "=== NLM Test Suite ===" << std::endl;
    std::cout << "Running comprehensive tests for NLM (Phase 6 integration)..." << std::endl;
    std::cout << std::endl;
    
    int testCount = 0;
    int passedTests = 0;
    
    // Run all tests
    try {
        test_types();
        passedTests++;
        testCount++;
    } catch (const std::exception& e) {
        std::cout << "    test_types failed: " << e.what() << std::endl;
        testCount++;
    }
    
    try {
        test_config();
        passedTests++;
        testCount++;
    } catch (const std::exception& e) {
        std::cout << "    test_config failed: " << e.what() << std::endl;
        testCount++;
    }
    
    try {
        test_random();
        passedTests++;
        testCount++;
    } catch (const std::exception& e) {
        std::cout << "    test_random failed: " << e.what() << std::endl;
        testCount++;
    }
    
    try {
        test_clock();
        passedTests++;
        testCount++;
    } catch (const std::exception& e) {
        std::cout << "    test_clock failed: " << e.what() << std::endl;
        testCount++;
    }
    
    try {
        test_neuron();
        passedTests++;
        testCount++;
    } catch (const std::exception& e) {
        std::cout << "    test_neuron failed: " << e.what() << std::endl;
        testCount++;
    }
    
    try {
        test_synapse();
        passedTests++;
        testCount++;
    } catch (const std::exception& e) {
        std::cout << "    test_synapse failed: " << e.what() << std::endl;
        testCount++;
    }
    
    try {
        test_stdp();
        passedTests++;
        testCount++;
    } catch (const std::exception& e) {
        std::cout << "    test_stdp failed: " << e.what() << std::endl;
        testCount++;
    }
    
    try {
        test_brain();
        passedTests++;
        testCount++;
    } catch (const std::exception& e) {
        std::cout << "    test_brain failed: " << e.what() << std::endl;
        testCount++;
    }
    
    try {
        test_memory_retention();
        passedTests++;
        testCount++;
    } catch (const std::exception& e) {
        std::cout << "    test_memory_retention failed: " << e.what() << std::endl;
        testCount++;
    }
    
    std::cout << std::endl;
    std::cout << "=== Test Summary ===" << std::endl;
    std::cout << "Tests run: " << testCount << std::endl;
    std::cout << "Tests passed: " << passedTests << std::endl;
    std::cout << "Tests failed: " << (testCount - passedTests) << std::endl;
    
    if (passedTests == testCount) {
        std::cout << "\n✓ ALL TESTS PASSED" << std::endl;
        std::cout << "The NLM brain system is functioning correctly with full integration of:" << std::endl;
        std::cout << "  - Neural dynamics and spike processing" << std::endl;
        std::cout << "  - Synaptic plasticity (STDP, Hebbian)" << std::endl;
        std::cout << "  - Neuromodulation (dopamine, acetylcholine, norepinephrine, serotonin)" << std::endl;
        std::cout << "  - Memory systems (working memory, episodic memory)" << std::endl;
        std::cout << "  - Cognitive systems (attention, concept formation, planning)" << std::endl;
        std::cout << "  - Prediction systems" << std::endl;
        std::cout << "  - Development and plasticity regulation" << std::endl;
        std::cout << "  - Sleep/rest cycle and memory consolidation" << std::endl;
        std::cout << "  - Replay mechanisms for memory strengthening" << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ TESTS FAILED" << std::endl;
        std::cout << "The NLM brain system requires further development in:" << std::endl;
        std::cout << "  - Components that failed to pass their tests" << std::endl;
        std::cout << "  - Integration between neural and cognitive systems" << std::endl;
        std::cout << "  - Memory retention and consolidation mechanisms" << std::endl;
        std::cout << "  - Neuromodulation effects on learning and behavior" << std::endl;
        return 1;
    }
}
