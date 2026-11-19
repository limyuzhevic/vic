// NLM Demo Systems Comprehensive Test Suite
// Phase 6: Final Integration Tests

#include <iostream>
#include <cassert>
#include <memory>
#include <string>

// Forward declarations
namespace test_demo_phase2 {
    void runAll();
}

namespace test_demo_phase3 {
    void runAll();
}

namespace test_demo_phase4 {
    void runAll();
}

namespace test_demo_phase5 {
    void runAll();
}

namespace test_demo_phase6 {
    void runAll();
}

namespace test_demo_phase6_text {
    void runAll();
}

namespace test_demo_phase6_benchmark {
    void runAll();
}

int main() {
    std::cout << "=== NLM Demo Systems Comprehensive Test Suite ===" << std::endl;
    std::cout << "Phase 6: Final Integration Tests" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Phase 2: Basic Neural Computation Tests
    std::cout << "Running Phase 2 Demo Tests..." << std::endl;
    try {
        test_demo_phase2::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    // Phase 3: World Interaction Tests
    std::cout << "Running Phase 3 Demo Tests..." << std::endl;
    try {
        test_demo_phase3::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    // Phase 4: Advanced Cognitive Tests
    std::cout << "Running Phase 4 Demo Tests..." << std::endl;
    try {
        test_demo_phase4::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    // Phase 5: Performance & Personality Tests
    std::cout << "Running Phase 5 Demo Tests..." << std::endl;
    try {
        test_demo_phase5::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    // Phase 6: Integration Tests
    std::cout << "Running Phase 6 Demo Tests..." << std::endl;
    try {
        test_demo_phase6::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    // Phase 6 Text Demo Tests
    std::cout << "Running Phase 6 Text Demo Tests..." << std::endl;
    try {
        test_demo_phase6_text::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    // Phase 6 Benchmark Tests
    std::cout << "Running Phase 6 Benchmark Tests..." << std::endl;
    try {
        test_demo_phase6_benchmark::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    std::cout << std::endl;
    
    std::cout << "=== Demo Systems Test Summary ===" << std::endl;
    std::cout << "Phase 2: Basic neural computation tests" << std::endl;
    std::cout << "Phase 3: World interaction & behavior tests" << std::endl;
    std::cout << "Phase 4: Advanced cognitive capabilities tests" << std::endl;
    std::cout << "Phase 5: Performance optimization & personality tests" << std::endl;
    std::cout << "Phase 6: Full integration & system coherence tests" << std::endl;
    std::cout << std::endl;
    
    if (allPassed) {
        std::cout << "=== All Demo System Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Demo System Tests FAILED ===" << std::endl;
        return 1;
    }
}