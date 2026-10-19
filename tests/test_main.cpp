// NLM Test Main
// Phase 6: Final Integration Tests

// NLM Test Main
// Phase 6: Final Integration Tests

#include <iostream>
#include <cassert>
#include <chrono>
#include <vector>
#include <functional>
#include <string>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"

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

namespace test_memory {
    void runAll();
}

namespace test_integration {
    void runAll();
}

namespace test_performance {
    void runAll();
}
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

namespace test_memory {
    void runAll();
}

namespace test_integration {
    void runAll();
}

namespace test_performance {
    void runAll();
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration Tests                          ║
    ║                                                               ║
    ║     Testing integrated artificial brain systems              ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp(const std::string& programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM Test Suite - Phase 6 Final Integration" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --benchmark                   Run performance benchmarks" << std::endl;
    std::cout << "  --integration                 Run integration tests" << std::endl;
    std::cout << "  --memory                      Run memory system tests" << std::endl;
    std::cout << "  --all                         Run all tests" << std::endl;
    std::cout << "  --steps N                     Number of simulation steps for tests" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --all --steps 1000" << std::endl;
    std::cout << "  " << programName << " --integration" << std::endl;
}

void printVersion() {
    std::cout << "NLM (Neural Learning Machine) Version 0.1.0" << std::endl;
    std::cout << "Phase 6: Final Integration Tests" << std::endl;
    std::cout << "Comprehensive test suite for integrated brain systems." << std::endl;
}

void runPerformanceBenchmark(size_t steps) {
    std::cout << "=== Performance Benchmark ===" << std::endl;
    std::cout << "Running performance tests with " << steps << " steps..." << std::endl;
    
    // Create a minimal brain for benchmarking
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.05);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain for benchmark!" << std::endl;
        return;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        // Log progress every 10% of steps
        if (step % (steps / 10) == 0 && step > 0) {
            std::cout << "Progress: " << step << "/" << steps << " steps (" 
                     << (double)step/steps*100 << "%)" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "=== Benchmark Results ===" << std::endl;
    std::cout << "Total steps: " << steps << std::endl;
    std::cout << "Total time: " << totalTime << "s" << std::endl;
    std::cout << "Steps per second: " << steps / totalTime << std::endl;
    std::cout << "Neurons simulated: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    
    if (totalTime > 0) {
        double spikesPerSecond = brain->getTotalSpikeCount() / totalTime;
        std::cout << "Spikes per second: " << spikesPerSecond << std::endl;
    }
}

int main(int argc, char** argv) {
    std::string programName = "nlm_test";
    if (argc > 0) {
        programName = std::string(argv[0]);
    }
    
    // Simple argument parsing
    bool help = false;
    bool benchmark = false;
    bool integration = false;
    bool memory = false;
    bool allTests = false;
    size_t steps = 1000;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            help = true;
        } else if (arg == "--benchmark") {
            benchmark = true;
        } else if (arg == "--integration") {
            integration = true;
        } else if (arg == "--memory") {
            memory = true;
        } else if (arg == "--all") {
            allTests = true;
        } else if (arg == "--steps" && i + 1 < argc) {
            steps = std::stoul(argv[++i]);
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            help = true;
        }
    }
    
    if (help) {
        printHelp(programName);
        return 0;
    }
    
    printBanner();
    
    std::cout << "=== NLM Phase 6 Test Suite ===" << std::endl;
    std::cout << "Testing integrated artificial brain systems." << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Determine which tests to run
    std::vector<std::pair<std::string, std::function<void()>>> tests;
    
    if (allTests || benchmark) {
        tests.push_back({"Performance Benchmark", [steps]() { runPerformanceBenchmark(steps); }});
    }
    
    if (allTests || integration) {
        tests.push_back({"Integration Tests", []() { test_integration::runAll(); }});
    }
    
    if (allTests || memory) {
        tests.push_back({"Memory System Tests", []() { test_memory::runAll(); }});
    }
    
    // Always run core tests unless benchmark or integration specified
    if (!benchmark && !integration && !memory) {
        tests.push_back({"Types Tests", []() { test_types::runAll(); }});
        tests.push_back({"Config Tests", []() { test_config::runAll(); }});
        tests.push_back({"Random Tests", []() { test_random::runAll(); }});
        tests.push_back({"Clock Tests", []() { test_clock::runAll(); }});
        tests.push_back({"Neuron Tests", []() { test_neuron::runAll(); }});
        tests.push_back({"Synapse Tests", []() { test_synapse::runAll(); }});
        tests.push_back({"STDP Tests", []() { test_stdp::runAll(); }});
        tests.push_back({"Brain Tests", []() { test_brain::runAll(); }});
    }
    
    // Run all tests
    for (const auto& test : tests) {
        std::cout << std::endl;
        std::cout << "=== Running " << test.first << " ===" << std::endl;
        
        try {
            test.second();
            std::cout << "  PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            allPassed = false;
        } catch (...) {
            std::cout << "  FAILED: Unknown error" << std::endl;
            allPassed = false;
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