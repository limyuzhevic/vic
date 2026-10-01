#include <iostream>
#include <string>
#include <vector>

// Include all test framework components
#include "tests/property/property_tests.cpp"
#include "tests/performance/benchmark_runner.cpp"
#include "tests/integration/integration_test_runner.cpp"
#include "tests/regression/regression_detector.cpp"
#include "tests/infrastructure/test_framework_integration.cpp"

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Comprehensive Testing Framework ===" << std::endl;
    std::cout << "Initializing advanced testing infrastructure..." << std::endl;
    
    // Parse command line arguments
    std::string test_mode = "all";
    bool run_property_tests = false;
    bool run_performance_tests = false;
    bool run_integration_tests = false;
    bool run_regression_tests = false;
    bool run_coverage_tests = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--property" || arg == "-p") {
            run_property_tests = true;
            test_mode = "property";
        } else if (arg == "--performance" || arg == "-perf") {
            run_performance_tests = true;
            test_mode = "performance";
        } else if (arg == "--integration" || arg == "-i") {
            run_integration_tests = true;
            test_mode = "integration";
        } else if (arg == "--regression" || arg == "-r") {
            run_regression_tests = true;
            test_mode = "regression";
        } else if (arg == "--coverage" || arg == "-c") {
            run_coverage_tests = true;
            test_mode = "coverage";
        } else if (arg == "--help" || arg == "-h") {
            print_help();
            return 0;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            print_help();
            return 1;
        }
    }
    
    // Set default test mode if not specified
    if (test_mode == "all") {
        run_property_tests = true;
        run_performance_tests = true;
        run_integration_tests = true;
        run_regression_tests = true;
        run_coverage_tests = true;
    }
    
    int result = 0;
    
    try {
        // Initialize test framework
        infrastructure::TestFrameworkIntegration::IntegrationConfig config;
        config.test_environment = "ci";
        config.coverage_report_file = "test_coverage_report.html";
        config.regression_baseline_file = "regression_baseline.json";
        config.regression_current_file = "regression_current.json";
        config.regression_report_file = "regression_report.html";
        config.enable_cmake_integration = true;
        config.enable_cross_platform_support = true;
        config.enable_performance_monitoring = true;
        config.enable_regression_testing = true;
        
        infrastructure::TestFrameworkIntegration framework(config);
        framework.initialize();
        
        // Run selected test suites
        if (run_property_tests) {
            std::cout << std::endl;
            std::cout << "=== Running Property-Based Tests ===" << std::endl;
            run_property_tests();
        }
        
        if (run_performance_tests) {
            std::cout << std::endl;
            std::cout << "=== Running Performance Benchmarks ===" << std::endl;
            run_performance_tests();
        }
        
        if (run_integration_tests) {
            std::cout << std::endl;
            std::cout << "=== Running Integration Tests ===" << std::endl;
            infrastructure::IntegrationTestRunner integration_runner;
            integration_runner.run_all_integration_tests();
        }
        
        if (run_regression_tests) {
            std::cout << std::endl;
            std::cout << "=== Running Regression Tests ===" << std::endl;
            run_regression_tests();
        }
        
        if (run_coverage_tests) {
            std::cout << std::endl;
            std::cout << "=== Running Coverage Tests ===" << std::endl;
            infrastructure::TestFrameworkIntegration coverage_framework(config);
            coverage_framework.run_integrated_test_suite();
        }
        
        // Finalize test framework
        framework.finalize();
        
        std::cout << std::endl;
        std::cout << "=== All Testing Complete ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during testing: " << e.what() << std::endl;
        result = 1;
    } catch (...) {
        std::cerr << "Unknown error during testing" << std::endl;
        result = 1;
    }
    
    return result;
}

void print_help() {
    std::cout << "NLM Comprehensive Testing Framework" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm_test [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --property, -p        Run property-based tests" << std::endl;
    std::cout << "  --performance, -perf  Run performance benchmarks" << std::endl;
    std::cout << "  --integration, -i     Run integration tests" << std::endl;
    std::cout << "  --regression, -r      Run regression tests" << std::endl;
    std::cout << "  --coverage, -c        Run coverage tests" << std::endl;
    std::cout << "  --help, -h            Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Default (if no options specified): Run all test suites" << std::endl;
    std::cout << std::endl;
    std::cout << "Property-Based Tests:" << std::endl;
    std::cout << "  - QuickCheck-style generators" << std::endl;
    std::cout << "  - Invariant testing and validation" << std::endl;
    std::cout << "  - Random property testing for neural systems" << std::endl;
    std::cout << std::endl;
    std::cout << "Performance Benchmarks:" << std::endl;
    std::cout << "  - Standardized benchmark suites" << std::endl;
    std::cout << "  - Performance comparison across configurations" << std::endl;
    std::cout << "  - Resource usage benchmarking" << std::endl;
    std::cout << std::endl;
    std::cout << "Integration Tests:" << std::endl;
    std::cout << "  - Phase 6 system integration testing" << std::endl;
    std::cout << "  - Cross-component testing and validation" << std::endl;
    std::cout << "  - Phase 6 system connectivity verification" << std::endl;
    std::cout << std::endl;
    std::cout << "Regression Tests:" << std::endl;
    std::cout << "  - Automated regression detection" << std::endl;
    std::cout << "  - Baseline comparison and analysis" << std::endl;
    std::cout << "  - Automated regression reporting" << std::endl;
    std::cout << std::endl;
    std::cout << "Coverage Tests:" << std::endl;
    std::cout << "  - Test coverage collection" << std::endl;
    std::cout << "  - Cross-platform support" << std::endl;
    std::cout << "  - Coverage reporting" << std::endl;
}