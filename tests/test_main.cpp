// NLM Test Main
// Phase 2: Comprehensive Advanced Testing and Validation

#include <iostream>
#include <cassert>
#include <chrono>
#include <string>

// Forward declarations of test suites
namespace test_plugin_system {
    void runAll();
}

namespace test_plugin_interfaces {
    void runAll();
}

namespace test_extensible_brain {
    void runAll();
}

namespace test_configuration_system {
    void runAll();
}

namespace test_architecture_compiler {
    void runAll();
}

namespace test_optimization_hooks {
    void runAll();
}

namespace test_advanced_experiment {
    void runAll();
}

namespace test_brain_plugin_integration {
    void runAll();
}

namespace test_memory_system_integration {
    void runAll();
}

namespace test_architecture_integration {
    void runAll();
}

namespace test_experiment_integration {
    void runAll();
}

namespace test_performance_integration {
    void runAll();
}

namespace test_cli_modes {
    void runAll();
}

namespace test_cli_configuration {
    void runAll();
}

namespace test_cli_interactive {
    void runAll();
}

namespace test_cli_batch {
    void runAll();
}

namespace test_cli_benchmark {
    void runAll();
}

namespace test_cli_experimental {
    void runAll();
}

namespace test_cli_validation {
    void runAll();
}

namespace test_stress_memory {
    void runAll();
}

namespace test_stress_plasticity {
    void runAll();
}

namespace test_stress_integration {
    void runAll();
}

namespace test_stress_performance {
    void runAll();
}

namespace test_stress_reliability {
    void runAll();
}

namespace test_arch_validation {
    void runAll();
}

namespace test_config_validation {
    void runAll();
}

namespace test_plugin_validation {
    void runAll();
}

namespace test_performance_validation {
    void runAll();
}

namespace test_reliability_validation {
    void runAll();
}

namespace test_benchmark_baseline {
    void runAll();
}

namespace test_benchmark_plugin_loading {
    void runAll();
}

namespace test_benchmark_simulation {
    void runAll();
}

namespace test_benchmark_memory {
    void runAll();
}

namespace test_benchmark_network {
    void runAll();
}

namespace tests {
    bool runAdvancedTestFramework();
    bool runTestResultReporter();
    bool runTestPerformanceProfiler();
    bool runTestMemoryLeakDetector();
    bool runTestConfigurationValidator();
}

int main() {
    std::cout << "=== NLM Phase 2: Comprehensive Advanced Testing ===" << std::endl;
    std::cout << "Testing Advanced NLM Components and Functionality" << std::endl;
    std::cout << "====================================================" << std::endl;
    
    bool allPassed = true;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Unit Testing Suite
    std::cout << "\n=== Unit Testing Suite ===" << std::endl;
    
    std::cout << "Running Plugin System tests..." << std::endl;
    try {
        test_plugin_system::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Plugin Interface tests..." << std::endl;
    try {
        test_plugin_interfaces::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Extensible Brain tests..." << std::endl;
    try {
        test_extensible_brain::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Configuration System tests..." << std::endl;
    try {
        test_configuration_system::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Architecture Compiler tests..." << std::endl;
    try {
        test_architecture_compiler::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Optimization Hooks tests..." << std::endl;
    try {
        test_optimization_hooks::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Advanced Experiment tests..." << std::endl;
    try {
        test_advanced_experiment::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Integration Testing Suite
    std::cout << "\n=== Integration Testing Suite ===" << std::endl;
    
    std::cout << "Running Brain Plugin Integration tests..." << std::endl;
    try {
        test_brain_plugin_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Memory System Integration tests..." << std::endl;
    try {
        test_memory_system_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Architecture Integration tests..." << std::endl;
    try {
        test_architecture_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Experiment Integration tests..." << std::endl;
    try {
        test_experiment_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Performance Integration tests..." << std::endl;
    try {
        test_performance_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // CLI Testing Suite
    std::cout << "\n=== CLI Testing Suite ===" << std::endl;
    
    std::cout << "Running CLI Modes tests..." << std::endl;
    try {
        test_cli_modes::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running CLI Configuration tests..." << std::endl;
    try {
        test_cli_configuration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running CLI Interactive tests..." << std::endl;
    try {
        test_cli_interactive::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running CLI Batch tests..." << std::endl;
    try {
        test_cli_batch::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running CLI Benchmark tests..." << std::endl;
    try {
        test_cli_benchmark::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running CLI Experimental tests..." << std::endl;
    try {
        test_cli_experimental::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running CLI Validation tests..." << std::endl;
    try {
        test_cli_validation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Stress Testing Suite
    std::cout << "\n=== Stress Testing Suite ===" << std::endl;
    
    std::cout << "Running Stress Memory tests..." << std::endl;
    try {
        test_stress_memory::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Stress Plasticity tests..." << std::endl;
    try {
        test_stress_plasticity::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Stress Integration tests..." << std::endl;
    try {
        test_stress_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Stress Performance tests..." << std::endl;
    try {
        test_stress_performance::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Stress Reliability tests..." << std::endl;
    try {
        test_stress_reliability::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Validation Suite
    std::cout << "\n=== Validation Suite ===" << std::endl;
    
    std::cout << "Running Architecture Validation tests..." << std::endl;
    try {
        test_arch_validation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Configuration Validation tests..." << std::endl;
    try {
        test_config_validation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Plugin Validation tests..." << std::endl;
    try {
        test_plugin_validation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Performance Validation tests..." << std::endl;
    try {
        test_performance_validation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Reliability Validation tests..." << std::endl;
    try {
        test_reliability_validation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Benchmark Testing Suite
    std::cout << "\n=== Benchmark Testing Suite ===" << std::endl;
    
    std::cout << "Running Benchmark Baseline tests..." << std::endl;
    try {
        test_benchmark_baseline::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Benchmark Plugin Loading tests..." << std::endl;
    try {
        test_benchmark_plugin_loading::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Benchmark Simulation tests..." << std::endl;
    try {
        test_benchmark_simulation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Benchmark Memory tests..." << std::endl;
    try {
        test_benchmark_memory::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Benchmark Network tests..." << std::endl;
    try {
        test_benchmark_network::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    // Advanced Test Framework
    std::cout << "\n=== Advanced Test Framework ===" << std::endl;
    
    std::cout << "Running Advanced Test Framework..." << std::endl;
    try {
        if (tests::runAdvancedTestFramework()) {
            std::cout << "  PASSED" << std::endl;
        } else {
            std::cout << "  FAILED" << std::endl;
            allPassed = false;
        }
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Test Result Reporter..." << std::endl;
    try {
        if (tests::runTestResultReporter()) {
            std::cout << "  PASSED" << std::endl;
        } else {
            std::cout << "  FAILED" << std::endl;
            allPassed = false;
        }
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Performance Profiler..." << std::endl;
    try {
        if (tests::runTestPerformanceProfiler()) {
            std::cout << "  PASSED" << std::endl;
        } else {
            std::cout << "  FAILED" << std::endl;
            allPassed = false;
        }
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Memory Leak Detector..." << std::endl;
    try {
        if (tests::runTestMemoryLeakDetector()) {
            std::cout << "  PASSED" << std::endl;
        } else {
            std::cout << "  FAILED" << std::endl;
            allPassed = false;
        }
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Configuration Validator..." << std::endl;
    try {
        if (tests::runTestConfigurationValidator()) {
            std::cout << "  PASSED" << std::endl;
        } else {
            std::cout << "  FAILED" << std::endl;
            allPassed = false;
        }
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << "\n=== Test Execution Complete ===" << std::endl;
    std::cout << "Total execution time: " << duration.count() << " seconds" << std::endl;
    
    if (allPassed) {
        std::cout << "\n=== All Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "\n=== Some Tests FAILED ===" << std::endl;
        return 1;
    }
}