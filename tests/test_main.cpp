// NLM Test Main
// Phase 2: Real Neural Computation Tests
// Enhanced Testing Framework with Comprehensive Coverage

#include <iostream>
#include <cassert>
#include <memory>
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

namespace test_neuromodulation {
    void runAll();
}

namespace test_memory {
    void runAll();
}

namespace test_development {
    void runAll();
}

namespace test_prediction {
    void runAll();
}

namespace test_cognition {
    void runAll();
}

namespace test_sensory {
    void runAll();
}

namespace test_motor {
    void runAll();
}

namespace test_environment {
    void runAll();
}

namespace test_world {
    void runAll();
}

namespace test_visualization {
    void runAll();
}

namespace test_performance {
    void runAll();
}

namespace test_experiment {
    void runAll();
}

namespace test_cli {
    void runAll();
}

namespace test_agent {
    void runAll();
}

namespace test_plasticity {
    void runAll();
}

int main() {
    initializeTestFramework();
    
    std::cout << "=== NLM Enhanced Test Suite ===" << std::endl;
    std::cout << "Comprehensive Testing of Neural Learning Machine Components:" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Core Components:" << std::endl;
    std::cout << "  - Basic Types and Identifiers" << std::endl;
    std::cout << "  - Configuration Management" << std::endl;
    std::cout << "  - Random Number Generation" << std::endl;
    std::cout << "  - Time Management (SimulationClock)" << std::endl;
    std::cout << "  - Neuron Dynamics (LIF, Spiking)" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - Spike-Timing-Dependent Plasticity (STDP)" << std::endl;
    std::cout << "  - Brain Integration and Control" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Neuromodulation Systems:" << std::endl;
    std::cout << "  - Dopamine, Curiosity, Novelty" << std::endl;
    std::cout << "  - Reward Prediction Error" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Memory Systems:" << std::endl;
    std::cout << "  - Working Memory" << std::endl;
    std::cout << "  - Episodic Memory" << std::endl;
    std::cout << "  - Associative Memory" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Development System:" << std::endl;
    std::cout << "  - Synaptogenesis" << std::endl;
    std::cout << "  - Maturation" << std::endl;
    std::cout << "  - Pruning" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Prediction System:" << std::endl;
    std::cout << "  - Neural Prediction" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Cognition System:" << std::endl;
    std::cout << "  - Concept Formation" << std::endl;
    std::cout << "  - Neural Planning" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Sensory Systems:" << std::endl;
    std::cout << "  - Audio, Vision, Internal Signals" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Motor System:" << std::endl;
    std::cout << "  - Actions and Control" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Environment and World:" << std::endl;
    std::cout << "  - Environment Simulation" << std::endl;
    std::cout << "  - World Dynamics" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Visualization and Performance:" << std::endl;
    std::cout << "  - Visualization Interface" << std::endl;
    std::cout << "  - Memory Pool, Event Queue, Parallel Processing" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Experiment and CLI:" << std::endl;
    std::cout << "  - Experiment Suite (Phase 3-6)" << std::endl;
    std::cout << "  - Command Line Interface" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Agent System:" << std::endl;
    std::cout << "  - Agent Brain" << std::endl;
    std::cout << "  - Sensory Perception" << std::endl;
    std::cout << "  - Agent Body" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Plasticity System:" << std::endl;
    std::cout << "  - Plasticity Rules (Hebbian, Anti-Hebbian, BCM)" << std::endl;
    std::cout << "  - Structural Plasticity" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Core Components
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
    
    // Neuromodulation Systems
    std::cout << "Running Neuromodulation tests..." << std::endl;
    try {
        test_neuromodulation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Memory Systems
    std::cout << "Running Memory tests..." << std::endl;
    try {
        test_memory::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Development System
    std::cout << "Running Development tests..." << std::endl;
    try {
        test_development::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Prediction System
    std::cout << "Running Prediction tests..." << std::endl;
    try {
        test_prediction::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Cognition System
    std::cout << "Running Cognition tests..." << std::endl;
    try {
        test_cognition::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Sensory Systems
    std::cout << "Running Sensory tests..." << std::endl;
    try {
        test_sensory::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Motor System
    std::cout << "Running Motor tests..." << std::endl;
    try {
        test_motor::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Environment and World
    std::cout << "Running Environment tests..." << std::endl;
    try {
        test_environment::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running World tests..." << std::endl;
    try {
        test_world::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Visualization and Performance
    std::cout << "Running Visualization tests..." << std::endl;
    try {
        test_visualization::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Performance tests..." << std::endl;
    try {
        test_performance::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Experiment and CLI
    std::cout << "Running Experiment tests..." << std::endl;
    try {
        test_experiment::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running CLI tests..." << std::endl;
    try {
        test_cli::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Agent System
    std::cout << "Running Agent tests..." << std::endl;
    try {
        test_agent::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Plasticity System
    std::cout << "Running Plasticity tests..." << std::endl;
    try {
        test_plasticity::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Final results
    std::cout << std::endl;
    cleanupTestFramework();
    
    if (allPassed) {
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        std::cout << "The NLM testing framework has been successfully implemented with comprehensive coverage of all neural system components." << std::endl;
        return 0;
    } else {
        std::cout << "=== SOME TESTS FAILED ===" << std::endl;
        std::cout << "Please review the failed tests above for details." << std::endl;
        return 1;
    }
}
