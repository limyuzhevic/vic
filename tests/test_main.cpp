// NLM Test Main
// Phase 2: Comprehensive Testing of Enhanced NLM Features

#include <iostream>
#include <cassert>

// Forward declarations of existing test functions
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

// Forward declarations of new Phase 2 test functions
namespace test_cli {
    void runAll();
}

namespace test_advanced_features {
    void runAll();
}

namespace test_performance_optimizations {
    void runAll();
}

namespace test_user_experience {
    void runAll();
}

namespace test_integration {
    void runAll();
}

int main() {
    std::cout << "=== NLM Phase 2 Enhanced Testing ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Testing Advanced Features:" << std::endl;
    std::cout << "  - CLI Command Suite (nlm-config, nlm-sim, nlm-analyze, nlm-viz, nlm-dev)" << std::endl;
    std::cout << "  - Enhanced Event System" << std::endl;
    std::cout << "  - Homeostatic and Reward-Modulated Plasticity" << std::endl;
    std::cout << "  - Multi-Region Coordination" << std::endl;
    std::cout << "  - Enhanced Replay System" << std::endl;
    std::cout << "  - Advanced Development System" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Testing Performance Optimizations:" << std::endl;
    std::cout << "  - Memory Pool System" << std::endl;
    std::cout << "  - SIMD Optimization System" << std::endl;
    std::cout << "  - Parallel Processing System" << std::endl;
    std::cout << "  - Cache-Friendly Data Structures" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Testing User Experience:" << std::endl;
    std::cout << "  - Interactive Debug Mode" << std::endl;
    std::cout << "  - Configuration Templates" << std::endl;
    std::cout << "  - Performance Profiling" << std::endl;
    std::cout << "  - Export/Import System" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Testing Integration:" << std::endl;
    std::cout << "  - Phase 6 Integration (Memory, Cognition, Prediction, Neuromodulation)" << std::endl;
    std::cout << "  - Backward Compatibility" << std::endl;
    std::cout << "  - System Stability" << std::endl;
    std::cout << "  - Performance Improvements" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Existing tests (Phase 1/Phase 2 core functionality)
    std::cout << "=== Running Existing Core Tests ===" << std::endl;
    
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
    
    // New tests (Phase 2 enhanced features)
    std::cout << "\n=== Running New CLI Tests ===" << std::endl;
    try {
        test_cli::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "\n=== Running New Advanced Features Tests ===" << std::endl;
    try {
        test_advanced_features::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "\n=== Running New Performance Optimizations Tests ===" << std::endl;
    try {
        test_performance_optimizations::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "\n=== Running New User Experience Tests ===" << std::endl;
    try {
        test_user_experience::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "\n=== Running New Integration Tests ===" << std::endl;
    try {
        test_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== ALL TESTS PASSED - NLM Phase 2 Enhancement Complete ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== SOME TESTS FAILED - REVIEW NEEDED ===" << std::endl;
        return 1;
    }
}
