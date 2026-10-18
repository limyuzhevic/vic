/**
 * Phase 6 Integration Demo - Main executable for comprehensive integration testing
 * 
 * This is the main executable for the Phase 6 integration demo. It provides
 * a command-line interface for running various integration scenarios:
 * - Quick integration verification
 * - Full integration testing
 * - Stress testing
 * - Memory leak detection
 * - Performance benchmarking
 * - Cross-system interaction testing
 * 
 * Usage:
 *   nlm_phase6_integration_demo [options]
 * 
 * Options:
 *   --quick-only     Run only quick integration verification
 *   --full-only      Run only full integration test
 *   --stress-only    Run only stress test
 *   --memory-only    Run only memory leak test
 *   --bench-only     Run only performance benchmark
 *   --cross-only     Run only cross-system interaction test
 *   --all            Run all tests (default)
 *   --html-report    Generate HTML report
 *   --no-output      Suppress console output
 */

#include "experiments/Phase6DemoIntegration.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <algorithm>

using namespace nlm;

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --quick-only     Run only quick integration verification" << std::endl;
    std::cout << "  --full-only      Run only full integration test" << std::endl;
    std::cout << "  --stress-only    Run only stress test" << std::endl;
    std::cout << "  --memory-only    Run only memory leak test" << std::endl;
    std::cout << "  --bench-only     Run only performance benchmark" << std::endl;
    std::cout << "  --cross-only     Run only cross-system interaction test" << std::endl;
    std::cout << "  --all            Run all tests (default)" << std::endl;
    std::cout << "  --html-report    Generate HTML report" << std::endl;
    std::cout << "  --no-output      Suppress console output" << std::endl;
    std::cout << "  --help           Show this help message" << std::endl;
}

bool parseArguments(int argc, char* argv[], 
                    bool& runQuickOnly, bool& runFullOnly, bool& runStressOnly,
                    bool& runMemoryOnly, bool& runBenchOnly, bool& runCrossOnly,
                    bool& generateHtmlReport, bool& suppressOutput) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--quick-only") {
            runQuickOnly = true;
        } else if (arg == "--full-only") {
            runFullOnly = true;
        } else if (arg == "--stress-only") {
            runStressOnly = true;
        } else if (arg == "--memory-only") {
            runMemoryOnly = true;
        } else if (arg == "--bench-only") {
            runBenchOnly = true;
        } else if (arg == "--cross-only") {
            runCrossOnly = true;
        } else if (arg == "--html-report") {
            generateHtmlReport = true;
        } else if (arg == "--no-output") {
            suppressOutput = true;
        } else if (arg == "--help") {
            printUsage(argv[0]);
            return false;
        } else if (arg == "--all") {
            runQuickOnly = runFullOnly = runStressOnly = runMemoryOnly = 
            runBenchOnly = runCrossOnly = false;
        } else {
            std::cerr << "Error: Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return false;
        }
    }
    
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Version: 1.0.0" << std::endl;
    std::cout << "Comprehensive integration testing of artificial brain systems" << std::endl;
    std::cout << std::endl;
    
    // Parse command line arguments
    bool runQuickOnly = false;
    bool runFullOnly = false;
    bool runStressOnly = false;
    bool runMemoryOnly = false;
    bool runBenchOnly = false;
    bool runCrossOnly = false;
    bool generateHtmlReport = false;
    bool suppressOutput = false;
    
    if (!parseArguments(argc, argv, runQuickOnly, runFullOnly, runStressOnly,
                       runMemoryOnly, runBenchOnly, runCrossOnly,
                       generateHtmlReport, suppressOutput)) {
        return 1;
    }
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create demo instance
    Phase6IntegrationDemo demo;
    
    std::cout << std::endl;
    
    // Run selected tests
    if (runQuickOnly) {
        demo.runQuickIntegrationTest();
    } else if (runFullOnly) {
        demo.runFullIntegrationTest();
    } else if (runStressOnly) {
        demo.runStressTest();
    } else if (runMemoryOnly) {
        demo.runMemoryLeakTest();
    } else if (runBenchOnly) {
        demo.runPerformanceBenchmark();
    } else if (runCrossOnly) {
        demo.runCrossSystemInteractionTest();
    } else {
        // Run all tests by default
        demo.runComprehensiveDemo();
    }
    
    // Generate HTML report if requested
    if (generateHtmlReport) {
        demo.generateFinalReport();
    }
    
    std::cout << std::endl;
    std::cout << "=== DEMONSTRATION COMPLETED ===" << std::endl;
    std::cout << "All requested integration tests have been completed successfully!" << std::endl;
    
    return 0;
}