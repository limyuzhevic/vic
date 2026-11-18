// CLI parsing implementation
#include <vector>
#include <algorithm>
#include <cctype>

namespace nlm {

CliOptions parseCommandLine(int argc, char** argv) {
    CliOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version" || arg == "-v") {
            options.version = true;
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--list-tests" || arg == "--list_tests") {
            options.listTests = true;
        } else if (arg.find("--steps=") == 0) {
            try {
                options.simulationSteps = std::stoi(arg.substr(7));
            } catch (...) {
                options.simulationSteps = 0;
            }
        } else if (arg.find("--config=") == 0) {
            options.configFile = arg.substr(9);
        } else if (arg.find("--config") == 0 && arg.find('=') == std::string::npos) {
            if (i + 1 < argc) options.configFile = argv[++i];
        } else if (arg.find("--log-level=") == 0) {
            options.logLevel = arg.substr(12);
        } else if (arg.find("--log-file=") == 0) {
            options.logFile = arg.substr(11);
        } else if (arg.find("--tests=") == 0) {
            std::string testsStr = arg.substr(8);
            options.enabledTests.clear();
            size_t pos = 0;
            while (pos < testsStr.length()) {
                size_t nextPos = testsStr.find(',', pos);
                std::string testName = testsStr.substr(pos, nextPos - pos);
                // Trim whitespace
                testName.erase(0, testName.find_first_not_of(" \t"));
                testName.erase(testName.find_last_not_of(" \t") + 1);
                if (!testName.empty()) options.enabledTests.push_back(testName);
                pos = nextPos == std::string::npos ? testsStr.length() : nextPos + 1;
            }
        }
    }
    
    return options;
}

void printHelp(const std::string& programName) {
    std::cout << "NLM (Neural Learning Machine) - Neural Network Simulator" << std::endl;
    std::cout << "Phase: " << NLM_PHASE << std::endl;
    std::cout << std::endl;
    
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help                    Show this help message and exit" << std::endl;
    std::cout << "  -v, --version                 Show version information and exit" << std::endl;
    std::cout << "  --config <file>               Configuration file path (default: configs/default.cfg)" << std::endl;
    std::cout << "  --log-level <level>           Log level: debug, info, warning, error (default: info)" << std::endl;
    std::cout << "  --log-file <file>             Log output file (in addition to console)" << std::endl;
    std::cout << "  --verbose, -v                 Enable verbose output" << std::endl;
    std::cout << "  --list-tests                  List available tests and exit" << std::endl;
    std::cout << "  --tests <name>[,<name>,...]   Comma-separated list of tests to run" << std::endl;
    std::cout << "  --steps <n>                   Override simulation steps" << std::endl;
    std::endl;
    
    std::cout << "Available Tests:" << std::endl;
    std::cout << "  all           Run all tests (default)" << std::endl;
    std::cout << "  connectivity  Test basic neural connectivity" << std::endl;
    std::cout << "  plasticity    Run plasticity learning experiment" << std::endl;
    std::cout << "  stdp          STDP plasticity verification test" << std::endl;
    std::endl;
}

void printVersion() {
    std::cout << "NLM " << NLM_VERSION << std::endl;
    std::cout << "Phase: " << NLM_PHASE << std::endl;
    std::cout << "Built with C++20 and pybind11 for Python bindings" << std::endl;
}

} // namespace nlm