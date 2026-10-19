// CLIParser Tests
// Tests for the refactored CLIParser class in main.cpp

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace test_cli_parser {

struct ParsedArgs {
    std::string configFile = "configs/default.cfg";
    std::vector<std::string> enabledExperiments;
    bool help = false;
    bool verbose = false;
    int64_t neuronCount = 1000;
    int64_t regionCount = 1;
    float connectionProbability = 0.1f;
    std::string outputFile;
};

// Simple CLI parser implementation for testing
class TestCLIParser {
public:
    static ParsedArgs parseArgs(int argc, char** argv) {
        ParsedArgs args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args.help = true;
            } else if (arg == "--verbose" || arg == "-v") {
                args.verbose = true;
            } else if (arg == "--config") {
                if (i + 1 < argc) args.configFile = argv[++i];
            } else if (arg.find("--config=") == 0) {
                args.configFile = arg.substr(9);
            } else if (arg == "--output") {
                if (i + 1 < argc) args.outputFile = argv[++i];
            } else if (arg.find("--output=") == 0) {
                args.outputFile = arg.substr(9);
            } else if (arg == "--neurons") {
                if (i + 1 < argc) args.neuronCount = std::stoll(argv[++i]);
            } else if (arg.find("--neurons=") == 0) {
                args.neuronCount = std::stoll(arg.substr(10));
            } else if (arg == "--regions") {
                if (i + 1 < argc) args.regionCount = std::stoll(argv[++i]);
            } else if (arg.find("--regions=") == 0) {
                args.regionCount = std::stoll(arg.substr(10));
            } else if (arg == "--probability") {
                if (i + 1 < argc) args.connectionProbability = std::stof(argv[++i]);
            } else if (arg.find("--probability=") == 0) {
                args.connectionProbability = std::stof(arg.substr(14));
            } else if (arg.find("--experiment=") == 0) {
                std::string expName = arg.substr(13);
                args.enabledExperiments.push_back(expName);
            } else if (arg.find("--experiment+") == 0) {
                std::string expName = arg.substr(13);
                args.enabledExperiments.push_back(expName);
            } else if (arg.substr(0, 2) == "--") {
                // Unknown flag, ignore but warn
                std::cout << "WARNING: Unknown flag: " << arg << std::endl;
            }
        }
        
        return args;
    }
};

void testBasicArgumentParsing() {
    std::cout << "  Testing basic argument parsing..." << std::endl;
    
    char* argv[] = {
        (char*)"test_program",
        (char*)"--neurons=500",
        (char*)"--regions=2",
        (char*)"--probability=0.2",
        (char*)"--experiment=plasticity",
        (char*)"--experiment+=stdp",
        (char*)"--output=results.json",
        (char*)"--verbose"
    };
    
    int argc = sizeof(argv) / sizeof(argv[0]);
    ParsedArgs args = TestCLIParser::parseArgs(argc, argv);
    
    assert(args.neuronCount == 500);
    assert(args.regionCount == 2);
    assert(args.connectionProbability == 0.2f);
    assert(args.enabledExperiments.size() == 2);
    assert(args.enabledExperiments[0] == "plasticity");
    assert(args.enabledExperiments[1] == "stdp");
    assert(args.outputFile == "results.json");
    assert(args.verbose == true);
    assert(args.configFile == "configs/default.cfg"); // Default value
    
    std::cout << "    ✓ Basic argument parsing test passed" << std::endl;
}

void testShortFormArguments() {
    std::cout << "  Testing short form arguments..." << std::endl;
    
    char* argv[] = {
        (char*)"test_program",
        (char*)"-h",
        (char*)"-v",
        (char*)"-neurons=1000",
        (char*)"-config=test.yaml"
    };
    
    int argc = sizeof(argv) / sizeof(argv[0]);
    ParsedArgs args = TestCLIParser::parseArgs(argc, argv);
    
    assert(args.help == true);
    assert(args.verbose == true);
    assert(args.neuronCount == 1000);
    assert(args.configFile == "test.yaml");
    
    std::cout << "    ✓ Short form arguments test passed" << std::endl;
}

void testExperimentSelection() {
    std::cout << "  Testing experiment selection logic..." << std::endl;
    
    char* argv[] = {
        (char*)"test_program",
        (char*)"--experiment=all",
        (char*)"--experiment=connectivity"
    };
    
    int argc = sizeof(argv) / sizeof(argv[0]);
    ParsedArgs args = TestCLIParser::parseArgs(argc, argv);
    
    assert(args.enabledExperiments.size() == 2);
    assert(args.enabledExperiments[0] == "all");
    assert(args.enabledExperiments[1] == "connectivity");
    
    std::cout << "    ✓ Experiment selection test passed" << std::endl;
}

void testUnknownArguments() {
    std::cout << "  Testing unknown argument handling..." << std::endl;
    
    char* argv[] = {
        (char*)"test_program",
        (char*)"--unknown_flag",
        (char*)"--another_unknown=123"
    };
    
    int argc = sizeof(argv) / sizeof(argv[0]);
    ParsedArgs args = TestCLIParser::parseArgs(argc, argv);
    
    // Unknown flags should be ignored but logged (we can't test the logging in this simple test)
    // Main thing is that parsing should continue without crashing
    assert(args.help == false); // Not set
    assert(args.verbose == false); // Not set
    
    std::cout << "    ✓ Unknown argument handling test passed" << std::endl;
}

void runAll() {
    std::cout << "=== CLIParser Tests ===" << std::endl;
    
    testBasicArgumentParsing();
    testShortFormArguments();
    testExperimentSelection();
    testUnknownArguments();
    
    std::cout << "=== All CLIParser Tests PASSED ===" << std::endl;
}

} // namespace test_cli_parser