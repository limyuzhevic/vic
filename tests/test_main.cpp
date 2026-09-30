// NLM Test Main
// Phase 6: Comprehensive Integration Tests for All Connected Systems

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

// Forward declarations of new integration test functions
namespace test_workingmemory {
    void runAll();
}

namespace test_episodicmemory {
    void runAll();
}

namespace test_predictionsystem {
    void runAll();
}

namespace test_neuralplanner {
    void runAll();
}

namespace test_conceptformation {
    void runAll();
}

namespace test_neuromodulation {
    void runAll();
}

namespace test_sleeprest {
    void runAll();
}

namespace test_replay {
    void runAll();
}

namespace test_crosssystem {
    void runAll();
}

namespace test_brainstep {
    void runAll();
}

int main() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Testing Comprehensive Integration of All Brain Systems:" << std::endl;
    std::cout << "  - Working Memory Integration" << std::endl;
    std::cout << "  - Episodic Memory Integration" << std::endl;
    std::cout << "  - Prediction System Integration" << std::endl;
    std::cout << "  - Neural Planner Integration" << std::endl;
    std::cout << "  - Concept Formation Integration" << std::endl;
    std::cout << "  - Neuromodulation Integration" << std::endl;
    std::cout << "  - Sleep/Rest Integration" << std::endl;
    std::cout << "  - Replay Mechanism Integration" << std::endl;
    std::cout << "  - Cross-System Integration" << std::endl;
    std::cout << "  - Brain::step() Integration Validation" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "=== Phase 2 Core Tests ===" << std::endl;
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
    std::cout << "=== Phase 6 Integration Tests ===" << std::endl;
    
    std::cout << "Running Working Memory Integration tests..." << std::endl;
    try {
        test_workingmemory::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Episodic Memory Integration tests..." << std::endl;
    try {
        test_episodicmemory::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Prediction System Integration tests..." << std::endl;
    try {
        test_predictionsystem::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Neural Planner Integration tests..." << std::endl;
    try {
        test_neuralplanner::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Concept Formation Integration tests..." << std::endl;
    try {
        test_conceptformation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Neuromodulation Integration tests..." << std::endl;
    try {
        test_neuromodulation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Sleep/Rest Integration tests..." << std::endl;
    try {
        test_sleeprest::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Replay Mechanism Integration tests..." << std::endl;
    try {
        test_replay::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Cross-System Integration tests..." << std::endl;
    try {
        test_crosssystem::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Brain::step() Integration Validation tests..." << std::endl;
    try {
        test_brainstep::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
        return 1;
    }
}
