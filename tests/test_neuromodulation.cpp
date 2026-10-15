// NLM Neuromodulation Tests
// Phase 2: Testing Neuromodulator Base Class and Derived Implementations

#include <iostream>
#include <cassert>
#include <cmath>

namespace nlm {
    class Neuromodulator;
    class Dopamine;
    class Acetylcholine;
    class Norepinephrine;
    class Serotonin;
    class Curiosity;
    class Novelty;
    class PredictionError;
    class Reward;
    
    // Forward declarations of test functions
    namespace test_neuromodulator {
        void runAll();
    }
    
    namespace test_dopamine {
        void runAll();
    }
    
    namespace test_curiosity {
        void runAll();
    }
    
    namespace test_plasticity_integration {
        void runAll();
    }
}

int main() {
    std::cout << "=== NLM Neuromodulation Tests ===" << std::endl;
    std::cout << "Testing Real Neuromodulatory Computation:" << std::endl;
    std::cout << "  - Neuromodulator Base Class" << std::endl;
    std::cout << "  - Dopamine Reward Prediction Error" << std::endl;
    std::cout << "  - Curiosity Exploration Motivation" << std::endl;
    std::cout << "  - Neuromodulation-Plasticity Integration" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Test neuromodulator base class
    std::cout << "Testing Neuromodulator base class..." << std::endl;
    try {
        nlm::test_neuromodulator::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test dopamine implementation
    std::cout << "Testing Dopamine implementation..." << std::endl;
    try {
        nlm::test_dopamine::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test curiosity implementation
    std::cout << "Testing Curiosity implementation..." << std::endl;
    try {
        nlm::test_curiosity::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Test plasticity integration
    std::cout << "Testing Neuromodulation-Plasticity Integration..." << std::endl;
    try {
        nlm::test_plasticity_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Neuromodulation Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Neuromodulation Tests FAILED ===" << std::endl;
        return 1;
    }
}
