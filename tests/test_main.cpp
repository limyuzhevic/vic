// NLM Test Main
// Phase 2: Real Neural Computation Tests

#include <iostream>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <string>
#include <vector>

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

// Command line argument parsing for tests
struct TestOptions {
    bool runAllTests = true;
    std::vector<std::string> specificTests;
    bool verbose = false;
    bool benchmark = false;
    bool help = false;
};

TestOptions parseTestArgs(int argc, char** argv) {
    TestOptions options;
    
    // Print usage information
    if (argc == 1) {
        std::cout << "Usage: nlm_test [options]\n";
        std::cout << "  --all               Run all tests (default)\n";
        std::cout << "  --types             Run only types tests\n";
        std::cout << "  --config            Run only config tests\n";
        std::cout << "  --random            Run only random tests\n";
        std::cout << "  --clock             Run only clock tests\n";
        std::cout << "  --neuron            Run only neuron tests\n";
        std::cout << "  --synapse           Run only synapse tests\n";
        std::cout << "  --stdp              Run only STDP tests\n";
        std::cout << "  --brain             Run only brain tests\n";
        std::cout << "  --verbose           Enable verbose output\n";
        std::cout << "  --benchmark         Run with timing measurements\n";
        std::cout << "  --help              Show this help message\n";
        return options;
    }
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            options.help = true;
            return options;
        } 
        else if (arg == "--verbose") {
            options.verbose = true;
        } 
        else if (arg == "--benchmark") {
            options.benchmark = true;
        } 
        else if (arg == "--types") {
            options.runAllTests = false;
            options.specificTests.push_back("types");
        } 
        else if (arg == "--config") {
            options.runAllTests = false;
            options.specificTests.push_back("config");
        } 
        else if (arg == "--random") {
            options.runAllTests = false;
            options.specificTests.push_back("random");
        } 
        else if (arg == "--clock") {
            options.runAllTests = false;
            options.specificTests.push_back("clock");
        } 
        else if (arg == "--neuron") {
            options.runAllTests = false;
            options.specificTests.push_back("neuron");
        } 
        else if (arg == "--synapse") {
            options.runAllTests = false;
            options.specificTests.push_back("synapse");
        } 
        else if (arg == "--stdp") {
            options.runAllTests = false;
            options.specificTests.push_back("stdp");
        } 
        else if (arg == "--brain") {
            options.runAllTests = false;
            options.specificTests.push_back("brain");
        } 
        else {
            std::cerr << "Unknown argument: " << arg << "\n";
        }
    }
    
    return options;
}

// Helper function to check if a test should be run
bool shouldRunTest(const TestOptions& options, const std::string& testName) {
    if (options.runAllTests) {
        return true;
    }
    for (const auto& specificTest : options.specificTests) {
        if (specificTest == testName) {
            return true;
        }
    }
    return false;
}

int main(int argc, char** argv) {
    // Parse command line arguments
    TestOptions options = parseTestArgs(argc, argv);
    
    if (options.help) {
        return 0;
    }
    
    std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Run Tests
    if (shouldRunTest(options, "types")) {
        std::cout << "Running Types tests..." << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        try {
            test_types::runAll();
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        if (options.benchmark) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            std::cout << "  Execution time: " << duration.count() << " ms" << std::endl;
        }
    }
    
    if (shouldRunTest(options, "config")) {
        std::cout << "Running Config tests..." << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        try {
            test_config::runAll();
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        if (options.benchmark) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            std::cout << "  Execution time: " << duration.count() << " ms" << std::endl;
        }
    }
    
    if (shouldRunTest(options, "random")) {
        std::cout << "Running Random tests..." << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        try {
            test_random::runAll();
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        if (options.benchmark) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            std::cout << "  Execution time: " << duration.count() << " ms" << std::endl;
        }
    }
    
    if (shouldRunTest(options, "clock")) {
        std::cout << "Running Clock tests..." << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        try {
            test_clock::runAll();
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        if (options.benchmark) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            std::cout << "  Execution time: " << duration.count() << " ms" << std::endl;
        }
    }
    
    if (shouldRunTest(options, "neuron")) {
        std::cout << "Running Neuron tests..." << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        try {
            test_neuron::runAll();
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        if (options.benchmark) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            std::cout << "  Execution time: " << duration.count() << " ms" << std::endl;
        }
    }
    
    if (shouldRunTest(options, "synapse")) {
        std::cout << "Running Synapse tests..." << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        try {
            test_synapse::runAll();
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        if (options.benchmark) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            std::cout << "  Execution time: " << duration.count() << " ms" << std::endl;
        }
    }
    
    if (shouldRunTest(options, "stdp")) {
        std::cout << "Running STDP tests..." << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        try {
            test_stdp::runAll();
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        if (options.benchmark) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            std::cout << "  Execution time: " << duration.count() << " ms" << std::endl;
        }
    }
    
    if (shouldRunTest(options, "brain")) {
        std::cout << "Running Brain tests..." << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        try {
            test_brain::runAll();
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        if (options.benchmark) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            std::cout << "  Execution time: " << duration.count() << " ms" << std::endl;
        }
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
