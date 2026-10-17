// NLM Comprehensive Test Suite
// Phase 3+: Advanced testing for critical issues and integration

#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include <stdexcept>
#include <cmath>

// Test namespace hierarchy
namespace nlm {
    // Forward declarations needed for tests
    class RandomGenerator;
    class SpikeSystem;
    class STDP;
    class Hebbian;
    class StructuralPlasticity;
    class Neuromodulator;
    class Dopamine;
    class Curiosity;
    class Novelty;
    class PredictionError;
    class NeuralWorkingMemory;
    class NeuralEpisodicMemory;
    class NeuralAssociativeMemory;
    class PredictionSystem;
    class NeuralPlanner;
    class ConceptFormation;
    class AttentionalSelection;
    class DevelopmentSystem;
    class DevelopmentalStage;
    class Action;
    class ActionType;
    class MotorCommand;
    class SensoryInput;
    class SensoryPercept;
    class SimpleWorld;
}

// Test categories
namespace test_memory_safety {
    void test_use_before_initialization();
    void test_move_constructor_safety();
    void test_assignment_operator_safety();
    void test_memory_leaks();
    void test_double_free_scenarios();
    void test_pointer_validity();
    void runAll() {
        test_use_before_initialization();
        test_move_constructor_safety();
        test_assignment_operator_safety();
        test_memory_leaks();
        test_double_free_scenarios();
        test_pointer_validity();
    }
}

namespace test_error_handling {
    void test_config_file_handling();
    void test_null_pointer_handling();
    void test_out_of_bounds_access();
    void test_checkpoint_error_handling();
    void test_boundary_conditions();
    void runAll() {
        test_config_file_handling();
        test_null_pointer_handling();
        test_out_of_bounds_access();
        test_checkpoint_error_handling();
        test_boundary_conditions();
    }
}

namespace test_api_correctness {
    void test_brain_api();
    void test_neuron_api();
    void test_synapse_api();
    void test_agent_brain_api();
    void test_api_consistency();
    void test_integration_between_components();
    void test_development_stages();
    void runAll() {
        test_brain_api();
        test_neuron_api();
        test_synapse_api();
        test_agent_brain_api();
        test_api_consistency();
        test_integration_between_components();
        test_development_stages();
    }
}

namespace test_performance {
    void test_neuron_stepping_efficiency();
    void test_memory_usage_patterns();
    void test_spike_propagation_performance();
    void test_plasticity_performance();
    void runAll() {
        test_neuron_stepping_efficiency();
        test_memory_usage_patterns();
        test_spike_propagation_performance();
        test_plasticity_performance();
    }
}

namespace test_integration {
    void test_complete_brain_loop();
    void test_agent_world_interaction();
    void test_learning_experiments();
    void test_checkpoint_functionality();
    void runAll() {
        test_complete_brain_loop();
        test_agent_world_interaction();
        test_learning_experiments();
        test_checkpoint_functionality();
    }
}

int main() {
    std::cout << "=== NLM Comprehensive Test Suite (Phase 3+) ===" << std::endl;
    std::cout << "Testing critical issues and integration:" << std::endl;
    std::cout << "  - Memory Management and Safety" << std::endl;
    std::cout << "  - Error Handling and Robustness" << std::endl;
    std::cout << "  - API Functionality and Correctness" << std::endl;
    std::cout << "  - Performance-Related Tests" << std::endl;
    std::cout << "  - Integration Tests" << std::endl;
    std::cout << std::endl;

    bool allPassed = true;

    // Memory Management Tests
    std::cout << "Running Memory Management Tests..." << std::endl;
    try {
        test_memory_safety::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }

    // Error Handling Tests
    std::cout << "Running Error Handling Tests..." << std::endl;
    try {
        test_error_handling::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }

    // API Correctness Tests
    std::cout << "Running API Correctness Tests..." << std::endl;
    try {
        test_api_correctness::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }

    // Performance Tests
    std::cout << "Running Performance Tests..." << std::endl;
    try {
        test_performance::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }

    // Integration Tests
    std::cout << "Running Integration Tests..." << std::endl;
    try {
        test_integration::runAll();
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