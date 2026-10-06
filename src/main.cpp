// NLM Test Runner Entry Point
// This file now serves as a simple entry point that delegates to the TestRunner
// class defined in src/test_runner.cpp. The monolithic main.cpp has been replaced
// with a modular, organized test runner that provides:
// - Clean separation of test concerns
// - Better error handling and validation
// - Command-line interface for test selection
// - Improved user experience with banners and progress reporting

#include "test_runner.cpp"

int main(int argc, char** argv) {
    // The TestRunner constructor initializes the logger and provides
    // the main test execution logic. This simple delegation keeps the
    // codebase modular and maintainable.
    TestRunner runner;
    
    // Parse command line arguments for specific test selection
    bool runAll = true;
    std::string testName = "All Tests";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--test1" || arg == "--connectivity") {
            testName = "Basic Connectivity Test";
            runAll = false;
        } else if (arg == "--test2" || arg == "--plasticity") {
            testName = "Plasticity Experiment";
            runAll = false;
        } else if (arg == "--test3" || arg == "--stdp") {
            testName = "STDP Verification";
            runAll = false;
        } else if (arg == "--test4" || arg == "--integration") {
            testName = "Integration Verification";
            runAll = false;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: nlm_test [OPTIONS]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --test1, --connectivity    Run basic connectivity test" << std::endl;
            std::cout << "  --test2, --plasticity       Run plasticity experiment" << std::endl;
            std::cout << "  --test3, --stdp             Run STDP verification" << std::endl;
            std::cout << "  --test4, --integration      Run integration verification" << std::endl;
            std::cout << "  -h, --help                  Show this help message" << std::endl;
            std::cout << "  (no args)                   Run all tests" << std::endl;
            return 0;
        }
    }
    
    if (runAll) {
        runner.runAllTests();
    } else {
        runner.printHeader(testName);
        if (testName == "Basic Connectivity Test") {
            runner.runBasicConnectivityTest();
        } else if (testName == "Plasticity Experiment") {
            runner.runPlasticityExperiment();
        } else if (testName == "STDP Verification") {
            runner.runStdpVerification();
        } else if (testName == "Integration Verification") {
            runner.runIntegrationVerification();
        }
    }
    
    return 0;
}
