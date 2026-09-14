// NLM Test Main
// Phase 6: Comprehensive Integrated Brain Test

#include <iostream>
#include <cassert>
#include <chrono>

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

namespace test_memory_systems {
    void runAll();
}

namespace test_neuromodulation {
    void runAll();
}

namespace test_prediction_system {
    void runAll();
}

namespace test_cognition_systems {
    void runAll();
}

namespace test_development_system {
    void runAll();
}

namespace test_checkpoints {
    void runAll();
}

namespace test_phase6_integration {
    void runAll();
}

void printPhase6Banner() {
    std::cout << R"(
╔════════════════════════════════════════════════════════════════════════╗
║                                                                ║
║    NLM — 熙然                                                       ║
║    Neural Learning Machine                                      ║
║                                                                ║
║    Phase 6: FINAL INTEGRATION DEMO                               ║
║                                                                ║
║    Testing: Memory Systems + Neuromodulation + Prediction +      ║
║    Cognition + Development + Checkpointing                      ║
║                                                                ║
║    Testing if all systems work together as a coherent artificial   ║
║    brain with coordinated learning and development.             ║
║                                                                ║
╚════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

int main() {
    printPhase6Banner();
    
    std::cout << "\n=== NLM Phase 6: Integrated Brain Test ===" << std::endl;
    std::cout << "Testing real artificial brain with all systems integrated:" << std::endl;
    std::cout << "  - Memory Systems (Working + Episodic + Associative)" << std::endl;
    std::cout << "  - Neuromodulation (Dopamine, Curiosity, Novelty)" << std::endl;
    std::cout << "  - Prediction System" << std::endl;
    std::cout << "  - Cognition Systems (Planner, Concept Formation, Attention)" << std::endl;
    std::cout << "  - Development System (critical periods, maturation)" << std::endl;
    std::cout << "  - Checkpointing and Persistence" << std::endl;
    std::cout << "  - Coordinated Brain Loop Integration" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Phase 1: Core System Tests (Phase 2 baseline)
    std::cout << "=== Running Phase 2: Core Neural Computation Tests ===" << std::endl;
    
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
    
    std::cout << std::endl;
    
    // Phase 2: Memory System Integration Tests
    std::cout << "=== Running Phase 2: Memory System Integration Tests ===" << std::endl;
    
    std::cout << "Running Memory Systems tests..." << std::endl;
    try {
        test_memory_systems::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Phase 2: Neuromodulation Integration Tests
    std::cout << "=== Running Phase 2: Neuromodulation Integration Tests ===" << std::endl;
    
    std::cout << "Running Neuromodulation tests..." << std::endl;
    try {
        test_neuromodulation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Phase 2: Prediction System Integration Tests
    std::cout << "=== Running Phase 2: Prediction System Integration Tests ===" << std::endl;
    
    std::cout << "Running Prediction System tests..." << std::endl;
    try {
        test_prediction_system::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Phase 2: Cognition System Integration Tests
    std::cout << "=== Running Phase 2: Cognition System Integration Tests ===" << std::endl;
    
    std::cout << "Running Cognition Systems tests..." << std::endl;
    try {
        test_cognition_systems::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Phase 2: Development System Integration Tests
    std::cout << "=== Running Phase 2: Development System Integration Tests ===" << std::endl;
    
    std::cout << "Running Development System tests..." << std::endl;
    try {
        test_development_system::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Phase 3: Checkpoint and Persistence Tests
    std::cout << "=== Running Phase 3: Checkpoint and Persistence Tests ===" << std::endl;
    
    std::cout << "Running Checkpoint tests..." << std::endl;
    try {
        test_checkpoints::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Phase 4: Phase 6 Integration Tests
    std::cout << "=== Running Phase 4: Phase 6 Integration Tests ===" << std::endl;
    
    std::cout << "Running Phase 6 Integration tests..." << std::endl;
    try {
        test_phase6_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    std::cout << "=== Phase 6 Integration Summary ===" << std::endl;
    if (allPassed) {
        std::cout << "✓ All Phase 6 integration tests PASSED" << std::endl;
        std::cout << "✓ NLM Brain successfully implements an integrated artificial brain" << std::endl;
        std::cout << "✓ All systems (memory, neuromodulation, prediction, cognition) work together" << std::endl;
        std::cout << "✓ Phase 6 objectives completed: coherent brain loop integration" << std::endl;
        std::cout << "✓ The NLM brain is now a functioning artificial neural substrate" << std::endl;
        std::cout << "  capable of changing its own synaptic connections through experience." << std::endl;
    } else {
        std::cout << "✗ Some Phase 6 integration tests FAILED" << std::endl;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== SOME TESTS FAILED ===" << std::endl;
        return 1;
    }
}
