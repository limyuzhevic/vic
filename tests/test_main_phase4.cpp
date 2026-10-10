// NLM Phase 4 Cognitive System Tests
// Tests for concept formation, neural planning, and other Phase 4 cognitive mechanisms

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

// Forward declarations
namespace test_concept_formation {
    void runAll();
}

namespace test_neural_planning {
    void runAll();
}

namespace test_working_memory {
    void runAll();
}

namespace test_prediction_system {
    void runAll();
}

namespace test_attention {
    void runAll();
}

namespace test_social_learning {
    void runAll();
}

int main() {
    std::cout << "=== NLM Phase 4 Cognitive System Tests ===" << std::endl;
    std::cout << "Testing Phase 4 cognitive capabilities:" << std::endl;
    std::cout << "  - Concept formation from experience patterns" << std::endl;
    std::cout << "  - Neural planning with action sequences" << std::endl;
    std::cout << "  - Working memory with active maintenance" << std::endl;
    std::cout << "  - Prediction system for sensory prediction" << std::endl;
    std::cout << "  - Attention with competitive selection dynamics" << std::endl;
    std::cout << "  - Social learning from other agents" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Concept Formation Tests
    std::cout << "Running Concept Formation tests..." << std::endl;
    try {
        test_concept_formation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Neural Planning Tests
    std::cout << "Running Neural Planning tests..." << std::endl;
    try {
        test_neural_planning::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Working Memory Tests
    std::cout << "Running Working Memory tests..." << std::endl;
    try {
        test_working_memory::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Prediction System Tests
    std::cout << "Running Prediction System tests..." << std::endl;
    try {
        test_prediction_system::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Attention Tests
    std::cout << "Running Attention tests..." << std::endl;
    try {
        test_attention::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Social Learning Tests
    std::cout << "Running Social Learning tests..." << std::endl;
    try {
        test_social_learning::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Phase 4 Cognitive System Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Phase 4 Cognitive System Tests FAILED ===" << std::endl;
        return 1;
    }
}
