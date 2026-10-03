// NLM Phase 3-6 Integration Tests
// Testing advanced cognitive capabilities
// 
// Tests for Phase 3-6: World interaction, cognitive capabilities, performance, and integration
// Based on Phase3Experiment.hpp, Phase4Experiment.hpp, Phase5Experiment.hpp, Phase6IntegratedExperiment.hpp components

#include "Phase3Experiment.hpp"
#include "Phase4Experiment.hpp"
#include "Phase5Experiment.hpp"
#include "Phase6IntegratedExperiment.hpp"
#include "test_main.cpp"
#include <iostream>
#include <cassert>
#include <memory>

// Forward declarations
namespace test_phase3 {
    void runAll();
}

namespace test_phase4 {
    void runAll();
}

namespace test_phase5 {
    void runAll();
}

namespace test_phase6 {
    void runAll();
}

namespace nlm {
namespace test_memory {
    void runAll();
}
}

int main() {
    std::cout << "=== NLM Phase 3-6 Integration Tests ===" << std::endl;
    std::cout << "Testing advanced cognitive capabilities:" << std::endl;
    std::cout << "  - World interaction and sensorimotor integration" << std::endl;
    std::cout << "  - Working memory and episodic memory" << std::endl;
    std::cout << "  - Concept formation and generalization" << std::endl;
    std::cout << "  - Planning and self-modeling" << std::endl;
    std::cout << "  - Social learning and object permanence" << std::endl;
    std::cout << "  - Development and scaling benchmarks" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running Phase 3 tests (World Interaction)..." << std::endl;
    try {
        test_phase3::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Phase 4 tests (Working Memory & Episodic Memory)..." << std::endl;
    try {
        test_phase4::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Phase 5 tests (Scaling & Performance)..." << std::endl;
    try {
        test_phase5::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Phase 6 tests (Integrated Experiment)..." << std::endl;
    try {
        test_phase6::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Memory Management tests..." << std::endl;
    try {
        nlm::test_memory::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Integration Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Integration Tests FAILED ===" << std::endl;
        return 1;
    }
}
