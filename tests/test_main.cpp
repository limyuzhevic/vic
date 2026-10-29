// NLM Test Main
// Phase 2: Real Neural Computation Tests

#include <iostream>
#include <cassert>
#include <chrono>
#include <string>
#include <vector>
#include <functional>

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

// Test framework
struct TestResult {
    std::string name;
    bool passed;
    float duration;
    std::string message;
    int testCount;
};

class TestRunner {
private:
    std::vector<TestResult> results;
    bool verbose;
    bool quiet;
    bool benchmark;
    bool runAll;
    int numTypesTests = 1;
    int numConfigTests = 1;
    int numRandomTests = 1;
    int numClockTests = 1;
    int numNeuronTests = 1;
    int numSynapseTests = 1;
    int numStdpTests = 1;
    int numBrainTests = 1;

public:
    TestRunner() : verbose(false), quiet(false), benchmark(false), runAll(true) {}

    void parseArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "--help" || arg == "-h") {
                printHelp();
                exit(0);
            }
            else if (arg.find("--types=") == 0) {
                numTypesTests = std::stoi(arg.substr(8));
                runAll = false;
            }
            else if (arg.find("--config=") == 0) {
                numConfigTests = std::stoi(arg.substr(9));
                runAll = false;
            }
            else if (arg.find("--random=") == 0) {
                numRandomTests = std::stoi(arg.substr(9));
                runAll = false;
            }
            else if (arg.find("--clock=") == 0) {
                numClockTests = std::stoi(arg.substr(8));
                runAll = false;
            }
            else if (arg.find("--neuron=") == 0) {
                numNeuronTests = std::stoi(arg.substr(9));
                runAll = false;
            }
            else if (arg.find("--synapse=") == 0) {
                numSynapseTests = std::stoi(arg.substr(10));
                runAll = false;
            }
            else if (arg.find("--stdp=") == 0) {
                numStdpTests = std::stoi(arg.substr(7));
                runAll = false;
            }
            else if (arg.find("--brain=") == 0) {
                numBrainTests = std::stoi(arg.substr(8));
                runAll = false;
            }
            else if (arg == "--verbose" || arg == "-v") {
                verbose = true;
            }
            else if (arg == "--quiet" || arg == "-q") {
                quiet = true;
            }
            else if (arg == "--benchmark") {
                benchmark = true;
            }
            else if (arg == "--test-only") {
                runAll = true; // Just use other flags to control what runs
            }
            else if (arg.find("--") == 0) {
                std::cout << "Unknown argument: " << arg << "\n";
                std::cout << "Use --help for usage information.\n";
                exit(1);
            }
            else {
                std::cout << "Unknown argument: " << arg << "\n";
                std::cout << "Use --help for usage information.\n";
                exit(1);
            }
        }
    }

    void printHelp() {
        std::cout << "NLM Test Runner Usage:\n"
                  << "  --types=<n>           Number of types tests to run (default: 1)\n"
                  << "  --config=<n>          Number of config tests to run (default: 1)\n"
                  << "  --random=<n>          Number of random tests to run (default: 1)\n"
                  << "  --clock=<n>           Number of clock tests to run (default: 1)\n"
                  << "  --neuron=<n>          Number of neuron tests to run (default: 1)\n"
                  << "  --synapse=<n>         Number of synapse tests to run (default: 1)\n"
                  << "  --stdp=<n>            Number of STDP tests to run (default: 1)\n"
                  << "  --brain=<n>           Number of brain tests to run (default: 1)\n"
                  << "  --verbose,-v           Enable verbose output\n"
                  << "  --quiet,-q             Suppress non-essential output\n"
                  << "  --benchmark            Run performance benchmarks\n"
                  << "  --test-only            Only run the specified tests (don't skip if failed)\n"
                  << "  --version              Show version and exit\n";
    }

    void runTestSuite(const std::string& name, std::function<void()> testFunc, int count = 1) {
        if (!runAll && count == 0) return;

        auto startTime = std::chrono::high_resolution_clock::now();

        bool suitePassed = true;
        std::string suiteMessage = "";

        try {
            for (int i = 0; i < count; ++i) {
                if (verbose) {
                    std::cout << "    Running " << name << " test " << (i + 1) << "/" << count << "..." << std::endl;
                }

                testFunc();

                if (benchmark) {
                    auto endTime = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0f;
                    if (verbose) {
                        std::cout << "    Test suite completed in " << duration << "s" << std::endl;
                    }
                }
            }
        } catch (const std::exception& e) {
            suitePassed = false;
            suiteMessage = std::string("Exception: ") + e.what();
            if (verbose) {
                std::cout << "    ERROR: " << suiteMessage << std::endl;
            }
        } catch (...) {
            suitePassed = false;
            suiteMessage = "Unknown exception";
            if (verbose) {
                std::cout << "    ERROR: " << suiteMessage << std::endl;
            }
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0f;

        TestResult result;
        result.name = name;
        result.passed = suitePassed;
        result.duration = duration;
        result.message = suiteMessage;
        result.testCount = count;

        results.push_back(result);

        if (!quiet) {
            std::cout << "  " << name << ": " << (suitePassed ? "PASSED" : "FAILED") << " (" << duration << "s)";
            if (!suiteMessage.empty()) {
                std::cout << " - " << suiteMessage;
            }
            std::cout << std::endl;
        }
    }

    void printSummary() {
        int totalTests = 0;
        int passedTests = 0;
        int failedTests = 0;
        float totalDuration = 0.0f;

        std::cout << std::endl;
        std::cout << "=== Test Suite Summary ===" << std::endl;

        for (const auto& result : results) {
            totalTests += result.testCount;
            if (result.passed) {
                passedTests++;
            } else {
                failedTests++;
            }
            totalDuration += result.duration;

            std::cout << "  " << result.name << ": " << result.testCount << " tests "
                      << (result.passed ? "PASSED" : "FAILED") << " (" << result.duration << "s)";

            if (!result.message.empty()) {
                std::cout << " - " << result.message;
            }
            std::cout << std::endl;
        }

        std::cout << std::endl;
        std::cout << "Overall Results:" << std::endl;
        std::cout << "  Total test suites: " << results.size() << std::endl;
        std::cout << "  Total tests: " << totalTests << std::endl;
        std::cout << "  Passed: " << passedTests << std::endl;
        std::cout << "  Failed: " << failedTests << std::endl;
        std::cout << "  Total time: " << totalDuration << "s" << std::endl;

        if (failedTests > 0) {
            std::cout << std::endl;
            std::cout << "=== WARNING: Some tests failed ===" << std::endl;
            return false;
        }

        return true;
    }

    const std::vector<TestResult>& getResults() const {
        return results;
    }
};

int main(int argc, char** argv[]) {
    std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;

    TestRunner runner;
    runner.parseArgs(argc, argv);

    // Run test suites
    runner.runTestSuite("Types", test_types::runAll, runner.numTypesTests);
    runner.runTestSuite("Config", test_config::runAll, runner.numConfigTests);
    runner.runTestSuite("Random", test_random::runAll, runner.numRandomTests);
    runner.runTestSuite("Clock", test_clock::runAll, runner.numClockTests);
    runner.runTestSuite("Neuron", test_neuron::runAll, runner.numNeuronTests);
    runner.runTestSuite("Synapse", test_synapse::runAll, runner.numSynapseTests);
    runner.runTestSuite("STDP", test_stdp::runAll, runner.numStdpTests);
    runner.runTestSuite("Brain", test_brain::runAll, runner.numBrainTests);

    // Print summary
    bool allPassed = runner.printSummary();

    return allPassed ? 0 : 1;
}
