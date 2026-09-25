// NLM Integration Tests
// Phase 6: Final Integration Testing

#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>

// Forward declarations for new test namespaces
namespace test_memory_integration {
    void runAll();
}

namespace test_prediction_system {
    void runAll();
}

namespace test_cognition_integration {
    void runAll();
}

namespace test_agent_integration {
    void runAll();
}

namespace test_end_to_end_integration {
    void runAll();
}

int main() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Testing Complete System Integration:" << std::endl;
    std::cout << "  - Memory System Integration" << std::endl;
    std::cout << "  - Prediction System Functionality" << std::endl;
    std::cout << "  - Cognition System Integration" << std::endl;
    std::cout << "  - AgentBrain Integration" << std::endl;
    std::cout << "  - End-to-End Integration" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running Memory Integration tests..." << std::endl;
    try {
        test_memory_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Prediction System tests..." << std::endl;
    try {
        test_prediction_system::runAll();
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
    
    std::cout << "Running Agent Integration tests..." << std::endl;
    try {
        test_agent_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running End-to-End Integration tests..." << std::endl;
    try {
        test_end_to_end_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Integration Tests PASSED ===" << std::endl;
        std::cout << "Phase 6 Integration is complete and functional!" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Integration Tests FAILED ===" << std::endl;
        return 1;
    }
}