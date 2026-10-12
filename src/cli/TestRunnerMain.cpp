#!/usr/bin/env g++
// NLM Test Runner
// Entry point for running NLM tests and benchmarks

#include "testing/TestRunner.hpp"
#include "testing/TestImplementation.cpp"

int main(int argc, char* argv[]) {
    try {
        nlm::testing::TestRunner runner;
        
        // Parse command line arguments
        std::string outputFile;
        bool verbose = true;
        bool runIntegration = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--output" && i + 1 < argc) {
                outputFile = argv[++i];
            } else if (arg == "--quiet") {
                verbose = false;
            } else if (arg == "--integration") {
                runIntegration = true;
            } else if (arg == "--help" || arg == "-h") {
                std::cout << "NLM Test Runner" << std::endl;
                std::cout << "==============" << std::endl;
                std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  --output FILE    Save results to file" << std::endl;
                std::cout << "  --quiet          Quiet mode" << std::endl;
                std::cout << "  --integration    Run integration tests" << std::endl;
                std::cout << "  --benchmark      Run performance benchmarks" << std::endl;
                std::cout << "  --help, -h       Show this help" << std::endl;
                return 0;
            }
        }
        
        // Configure test runner
        if (!outputFile.empty()) {
            runner.setOutputFile(outputFile);
        }
        runner.setVerbose(verbose);
        
        // Create and run test suite
        createTestSuite(runner);
        
        // Run integration tests if requested
        if (runIntegration) {
            std::cout << "\n" << std::string(50, '=') << std::endl;
            std::cout << "INTEGRATION TESTS" << std::endl;
            std::cout << std::string(50, '=') << std::endl;
            
            if (!nlm::testing::IntegrationTests::runIntegrationTests()) {
                std::cerr << "Integration tests failed!" << std::endl;
                return 1;
            }
        }
        
        // Run benchmarks
        if (argc > 1 && std::string(argv[1]) == "--benchmark") {
            std::cout << "\n" << std::string(50, '=') << std::endl;
            std::cout << "PERFORMANCE BENCHMARKS" << std::endl;
            std::cout << std::string(50, '=') << std::endl;
            nlm::testing::Benchmark::runBenchmarkSuite();
        }
        
        // Run all tests
        bool success = runner.runAll();
        
        return success ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
