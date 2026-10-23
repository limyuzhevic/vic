// TestRunner.hpp - Main orchestrator for NLM tests
//
// This header defines the TestRunner class that coordinates all test functions
// with command-line argument parsing, error handling, and logging. It maintains
// backward compatibility by preserving the original test sequence while adding
// modularity and configurability.

#ifndef NLM_TEST_RUNNER_HPP
#define NLM_TEST_RUNNER_HPP

#include <string>
#include <vector>
#include <memory>
#include <brain/Brain.hpp>
#include <core/Config/Config.hpp>
#include <core/SimulationClock/SimulationClock.hpp>

namespace nlm {
namespace tests {

class TestRunner {
public:
    enum class TestType {
        Unknown,
        Connectivity,
        Plasticity,
        Stdp
    };
    
    TestRunner(int argc, char** argv);
    
    void printBanner();
    void initializeLogger();
    void loadConfiguration();
    void determineTestsToRun();
    void initializeBrain();
    
    void runConnectivityTest();
    void runPlasticityTest();
    void runStdpTest();
    
    void runAllTests();
    
    void printFinalBrainStatus();
    void printPhase2Complete();
    
    int run();
    
private:
    int argc;
    char** argv;
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimulationClock> clock_;
    std::vector<TestType> testsToRun;
    
    void printHelp();
    void printVersion();
    
    TestType stringToTestType(const std::string& str);
};

} // namespace tests
} // namespace nlm

#endif // NLM_TEST_RUNNER_HPP
