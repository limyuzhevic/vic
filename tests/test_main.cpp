// NLM Test Main
// Phase 6: Comprehensive Integration Tests

#include <iostream>
#include <cassert>

// Forward declarations of test functions
namespace test_types {
    void runAll();
}

namespace test_config {
    void runAll();
}

namespace test_random {
    void runAll();
}

namespace test_clock {
    void runAll();
}

namespace test_neuron {
    void runAll();
}

namespace test_synapse {
    void runAll();
}

namespace test_stdp {
    void runAll();
}

namespace test_brain {
    void runAll();
}

namespace test_memory_integration {
    void runAll();
}

namespace test_prediction_integration {
    void runAll();
}

namespace test_cognition_integration {
    void runAll();
}

namespace test_neuromodulation_integration {
    void runAll();
}

namespace test_development_integration {
    void runAll();
}

namespace test_full_brain_integration {
    void runAll();
}

int main() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Testing Complete Brain System Integration:" << std::endl;
    std::cout << "  - Memory Systems Integration" << std::endl;
    std::cout << "  - Prediction System Integration" << std::endl;
    std::cout << "  - Cognition Systems Integration" << std::endl;
    std::cout << "  - Neuromodulation Systems Integration" << std::endl;
    std::cout << "  - Development Systems Integration" << std::endl;
    std::cout << "  - Full Brain Loop Integration" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running Types tests..." << std::endl;
    try {
        test_types::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Config tests..." << std::endl;
    try {
        test_config::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Random tests..." << std::endl;
    try {
        test_random::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Clock tests..." << std::endl;
    try {
        test_clock::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Neuron tests..." << std::endl;
    try {
        test_neuron::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Synapse tests..." << std::endl;
    try {
        test_synapse::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running STDP tests..." << std::endl;
    try {
        test_stdp::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Brain tests..." << std::endl;
    try {
        test_brain::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Memory Integration tests..." << std::endl;
    try {
        test_memory_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Prediction Integration tests..." << std::endl;
    try {
        test_prediction_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Cognition Integration tests..." << std::endl;
    try {
        test_cognition_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Neuromodulation Integration tests..." << std::endl;
    try {
        test_neuromodulation_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Development Integration tests..." << std::endl;
    try {
        test_development_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Full Brain Integration tests..." << std::endl;
    try {
        test_full_brain_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Phase 6 Integration Tests PASSED ===" << std::endl;
        std::cout << "Phase 6 integration claims validated:" << std::endl;
        std::cout << "  ✓ Memory systems are connected to neural processing" << std::endl;
        std::cout << "  ✓ Neuromodulation affects plasticity and neural dynamics" << std::endl;
        std::cout << "  ✓ Prediction system is integrated with learning" << std::endl;
        std::cout << "  ✓ Development affects plasticity rates" << std::endl;
        std::cout << "  ✓ Checkpoint save/load works" << std::endl;
        std::cout << "  ✓ Replay and consolidation are functional" << std::endl;
        std::cout << "  ✓ Complete brain loop functions coherently" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Phase 6 Integration Tests FAILED ===" << std::endl;
        return 1;
    }
}
