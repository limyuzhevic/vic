// NLM Phase 6 Integration Tests
// Tests complete brain system integration (Phase 6: Final Integration)

#include <iostream>
#include <cassert>
#include <chrono>
#include <thread>

// Forward declarations
namespace test_phase6_integration {
    void runAll();
}

namespace test_phase6_memory {
    void runAll();
}

namespace test_phase6_neuromodulation {
    void runAll();
}

namespace test_phase6_plasticity {
    void runAll();
}

namespace test_phase6_prediction {
    void runAll();
}

namespace test_phase6_cognition {
    void runAll();
}

namespace test_phase6_development {
    void runAll();
}

namespace test_phase6_checkpointing {
    void runAll();
}

namespace test_phase6_replay {
    void runAll();
}

int main() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Testing complete brain system integration:" << std::endl;
    std::cout << "  - Memory systems integration" << std::endl;
    std::cout << "  - Neuromodulation integration" << std::endl;
    std::cout << "  - Plasticity integration" << std::endl;
    std::cout << "  - Prediction system integration" << std::endl;
    std::cout << "  - Cognitive integration" << std::endl;
    std::cout << "  - Development system integration" << std::endl;
    std::cout << "  - Checkpointing and replay" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running Phase 6 Integration tests..." << std::endl;
    try {
        test_phase6_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Phase 6 Memory integration tests..." << std::endl;
    try {
        test_phase6_memory::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Phase 6 Neuromodulation integration tests..." << std::endl;
    try {
        test_phase6_neuromodulation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Phase 6 Plasticity integration tests..." << std::endl;
    try {
        test_phase6_plasticity::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Phase 6 Prediction integration tests..." << std::endl;
    try {
        test_phase6_prediction::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Phase 6 Cognition integration tests..." << std::endl;
    try {
        test_phase6_cognition::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Phase 6 Development integration tests..." << std::endl;
    try {
        test_phase6_development::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Phase 6 Checkpointing tests..." << std::endl;
    try {
        test_phase6_checkpointing::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Phase 6 Replay tests..." << std::endl;
    try {
        test_phase6_replay::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Phase 6 Integration Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Phase 6 Integration Tests FAILED ===" << std::endl;
        return 1;
    }
}