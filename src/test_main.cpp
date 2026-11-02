#include "core/TestRunner.hpp"

int main(int argc, char** argv) {
    auto& testRunner = nlm::testing::TestRunner::getInstance();
    
    // Parse command line arguments
    std::string testName;
    std::string outputFile;
    bool listTests = false;
    bool runAll = false;
    bool performanceMode = false;
    bool verbose = false;
    double timeout = 30.0; // seconds
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            testRunner.printUsage();
            return 0;
        } else if (arg == "--list") {
            listTests = true;
        } else if (arg == "--run-all") {
            runAll = true;
        } else if (arg == "--performance") {
            performanceMode = true;
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg.find("--test=") == 0) {
            testName = arg.substr(7);
        } else if (arg.find("--output=") == 0) {
            outputFile = arg.substr(9);
        } else if (arg.find("--timeout=") == 0) {
            try {
                timeout = std::stod(arg.substr(10));
            } catch (...) {
                std::cerr << "Warning: Invalid timeout, using default" << std::endl;
            }
        } else {
            std::cerr << "Warning: Unknown argument ignored: " << arg << std::endl;
        }
    }
    
    // Set verbose mode
    if (verbose) {
        std::cout << "Verbose mode enabled" << std::endl;
    }
    
    // Execute tests
    bool success = true;
    
    if (listTests) {
        testRunner.listTests();
    } else if (performanceMode) {
        testRunner.runPerformanceTests();
    } else if (!testName.empty()) {
        success = testRunner.runTestByName(testName);
    } else if (runAll) {
        testRunner.runAllTests();
    } else {
        std::cout << "No tests specified. Use --list, --test=name, or --run-all" << std::endl;
        std::cout << "Run " << argv[0] << " --help for more information." << std::endl;
        return 1;
    }
    
    if (outputFile.empty()) {
        outputFile = "test_results.txt";
    }
    
    return success ? 0 : 1;
}
