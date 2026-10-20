// NLM Comprehensive Subsystem Test Runner
// Phase 2: Complete Neural Subsystem Testing Infrastructure

#include <iostream>
#include <cassert>

// Forward declarations of all test functions
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

namespace test_neural_working_memory {
    void runAll();
}

namespace test_neural_episodic_memory {
    void runAll();
}

namespace test_neural_associative_memory {
    void runAll();
}

namespace test_prediction_system {
    void runAll();
}

namespace test_neural_prediction {
    void runAll();
}

namespace test_neural_planner {
    void runAll();
}

namespace test_concept_formation {
    void runAll();
}

namespace test_attention_selection {
    void runAll();
}

namespace test_development_system {
    void runAll();
}

namespace test_dopamine {
    void runAll();
}

namespace test_curiosity {
    void runAll();
}

namespace test_novelty {
    void runAll();
}

namespace test_prediction_error {
    void runAll();
}

int main() {
    std::cout << "=== NLM Phase 2 Comprehensive Subsystem Tests ===" << std::endl;
    std::cout << "Testing Complete Neural Architecture:" << std::endl;
    std::cout << "  - Basic Neural Computation (Types, Config, Random, Clock)" << std::endl;
    std::cout << "  - Neural Substrates (Neuron, Synapse, STDP)" << std::endl;
    std::cout << "  - Brain Integration and Function" << std::endl;
    std::cout << "  - Memory Systems (Working, Episodic, Associative)" << std::endl;
    std::cout << "  - Prediction Systems (High-level and Neural)" << std::endl;
    std::cout << "  - Cognitive Processing (Planning, Concept Formation, Attention)" << std::endl;
    std::cout << "  - Development and Plasticity" << std::endl;
    std::cout << "  - Neuromodulation (Dopamine, Curiosity, Novelty, Prediction Error)" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Basic Neural Computation Tests
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
    
    // Core Neural Structure Tests
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
    
    // Memory System Tests
    std::cout << "Running Neural Working Memory tests..." << std::endl;
    try {
        test_neural_working_memory::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Neural Episodic Memory tests..." << std::endl;
    try {
        test_neural_episodic_memory::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Neural Associative Memory tests..." << std::endl;
    try {
        test_neural_associative_memory::runAll();
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
    
    std::cout << "Running Neural Prediction tests..." << std::endl;
    try {
        test_neural_prediction::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Cognitive Processing Tests
    std::cout << "Running Neural Planner tests..." << std::endl;
    try {
        test_neural_planner::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Concept Formation tests..." << std::endl;
    try {
        test_concept_formation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Attentional Selection tests..." << std::endl;
    try {
        test_attention_selection::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Development and Plasticity Tests
    std::cout << "Running Development System tests..." << std::endl;
    try {
        test_development_system::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Neuromodulation Tests
    std::cout << "Running Dopamine (Reward) tests..." << std::endl;
    try {
        test_dopamine::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Curiosity tests..." << std::endl;
    try {
        test_curiosity::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Novelty tests..." << std::endl;
    try {
        test_novelty::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Prediction Error tests..." << std::endl;
    try {
        test_prediction_error::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        std::cout << "NLM Phase 2 comprehensive testing completed successfully!" << std::endl;
        std::cout << "All neural subsystems are functioning correctly." << std::endl;
        return 0;
    } else {
        std::cout << "=== SOME TESTS FAILED ===" << std::endl;
        std::cout << "NLM Phase 2 testing completed with failures." << std::endl;
        std::cout << "Some neural subsystems require attention." << std::endl;
        return 1;
    }
}