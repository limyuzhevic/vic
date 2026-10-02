// NLM Test Main - Application Module Tests
// Testing the new refactored application structure

#include <iostream>
#include <cassert>

// Forward declarations of test functions
namespace test_commandline_parser {
    void runAll();
}

namespace test_configuration_loader {
    void runAll();
}

namespace test_experiment_runner {
    void runAll();
}

namespace test_brain_initializer {
    void runAll();
}

namespace test_user_interface {
    void runAll();
}

namespace test_performance_profiler {
    void runAll();
}

namespace test_basic_connectivity {
    void runAll();
}

namespace test_plasticity_experiment {
    void runAll();
}

namespace test_stdp_verification {
    void runAll();
}

int main() {
    std::cout << "=== NLM Phase 2 Application Module Tests ===" << std::endl;
    std::cout << "Testing the new refactored application structure:" << std::endl;
    std::cout << "  - Command Line Parser" << std::endl;
    std::cout << "  - Configuration Loader" << std::endl;
    std::cout << "  - Experiment Runner" << std::endl;
    std::cout << "  - Brain Initializer" << std::endl;
    std::cout << "  - User Interface" << std::endl;
    std::cout << "  - Performance Profiler" << std::endl;
    std::cout << "  - Basic Connectivity Test" << std::endl;
    std::cout << "  - Plasticity Experiment Test" << std::endl;
    std::cout << "  - STDP Verification Test" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running Command Line Parser tests..." << std::endl;
    try {
        test_commandline_parser::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Configuration Loader tests..." << std::endl;
    try {
        test_configuration_loader::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Experiment Runner tests..." << std::endl;
    try {
        test_experiment_runner::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Brain Initializer tests..." << std::endl;
    try {
        test_brain_initializer::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running User Interface tests..." << std::endl;
    try {
        test_user_interface::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Performance Profiler tests..." << std::endl;
    try {
        test_performance_profiler::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Basic Connectivity tests..." << std::endl;
    try {
        test_basic_connectivity::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Plasticity Experiment tests..." << std::endl;
    try {
        test_plasticity_experiment::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running STDP Verification tests..." << std::endl;
    try {
        test_stdp_verification::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Application Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Application Tests FAILED ===" << std::endl;
        return 1;
    }
}