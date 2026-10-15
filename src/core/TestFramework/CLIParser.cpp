#include "CLIParser.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <cctype>

namespace nlm {

// Static initialization
void CLIParser::registerOptions() {
    // Options will be registered with command line parsing library
    // For now, we'll implement basic parsing
}

void CLIParser::setupArgumentParser() {
    // Setup argument parsing configuration
}

CLIParser::TestOptions CLIParser::parse(int argc, char** argv) {
    TestOptions options;
    
    if (argc <= 1) {
        options.showHelp = true;
        return options;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.showHelp = true;
        } else if (arg == "--version") {
            options.showVersion = true;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            } else {
                std::cerr << "Error: --config option requires a filename" << std::endl;
            }
        } else if (arg.find("--config=") == 0) {
            options.configFile = arg.substr(8);
        } else if (arg == "--output-dir") {
            if (i + 1 < argc) {
                options.outputDirectory = argv[++i];
            } else {
                std::cerr << "Error: --output-dir option requires a directory" << std::endl;
            }
        } else if (arg.find("--output-dir=") == 0) {
            options.outputDirectory = arg.substr(12);
        } else if (arg == "--output-format") {
            if (i + 1 < argc) {
                options.outputFormat = argv[++i];
            } else {
                std::cerr << "Error: --output-format option requires a format" << std::endl;
            }
        } else if (arg.find("--output-format=") == 0) {
            options.outputFormat = arg.substr(15);
        } else if (arg == "--log-level") {
            if (i + 1 < argc) {
                options.logLevel = argv[++i];
            } else {
                std::cerr << "Error: --log-level option requires a level" << std::endl;
            }
        } else if (arg.find("--log-level=") == 0) {
            options.logLevel = arg.substr(11);
        } else if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg == "--test-category") {
            if (i + 1 < argc) {
                options.testCategories.push_back(argv[++i]);
            } else {
                std::cerr << "Error: --test-category option requires a category" << std::endl;
            }
        } else if (arg.find("--test-category=") == 0) {
            options.testCategories.push_back(arg.substr(16));
        } else if (arg == "--test-tag") {
            if (i + 1 < argc) {
                options.testTags.push_back(argv[++i]);
            } else {
                std::cerr << "Error: --test-tag option requires a tag" << std::endl;
            }
        } else if (arg.find("--test-tag=") == 0) {
            options.testTags.push_back(arg.substr(13));
        } else if (arg == "--test-phase") {
            if (i + 1 < argc) {
                options.testPhases.push_back(argv[++i]);
            } else {
                std::cerr << "Error: --test-phase option requires a phase" << std::endl;
            }
        } else if (arg.find("--test-phase=") == 0) {
            options.testPhases.push_back(arg.substr(13));
        } else if (arg == "--exclude-test") {
            if (i + 1 < argc) {
                options.excludeTests.push_back(argv[++i]);
            } else {
                std::cerr << "Error: --exclude-test option requires a test name" << std::endl;
            }
        } else if (arg.find("--exclude-test=") == 0) {
            options.excludeTests.push_back(arg.substr(15));
        } else if (arg == "--test-order") {
            if (i + 1 < argc) {
                options.testOrder = argv[++i];
            } else {
                std::cerr << "Error: --test-order option requires an order" << std::endl;
            }
        } else if (arg.find("--test-order=") == 0) {
            options.testOrder = arg.substr(13);
        } else if (arg == "--parallel-tests") {
            if (i + 1 < argc) {
                options.maxParallelTests = std::stoi(argv[++i]);
            } else {
                std::cerr << "Error: --parallel-tests option requires a number" << std::endl;
            }
        } else if (arg.find("--parallel-tests=") == 0) {
            options.maxParallelTests = std::stoi(arg.substr(17));
        } else if (arg == "--timeout") {
            if (i + 1 < argc) {
                options.timeoutSeconds = std::stoi(argv[++i]);
            } else {
                std::cerr << "Error: --timeout option requires a number of seconds" << std::endl;
            }
        } else if (arg.find("--timeout=") == 0) {
            options.timeoutSeconds = std::stoi(arg.substr(10));
        } else if (arg == "--reset-brain") {
            options.resetBrainBetweenTests = true;
        } else if (arg == "--save-brain") {
            options.saveBrainState = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                options.brainSavePath = argv[++i];
            }
        } else if (arg.find("--save-brain=") == 0) {
            options.brainSavePath = arg.substr(13);
            options.saveBrainState = true;
        } else if (arg == "--load-brain") {
            options.restoreBrainState = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                options.brainLoadPath = argv[++i];
            }
        } else if (arg.find("--load-brain=") == 0) {
            options.brainLoadPath = arg.substr(12);
            options.restoreBrainState = true;
        } else if (arg == "--plugin") {
            if (i + 1 < argc) {
                options.enabledPlugins.push_back(argv[++i]);
            } else {
                std::cerr << "Error: --plugin option requires a plugin name" << std::endl;
            }
        } else if (arg.find("--plugin=") == 0) {
            options.enabledPlugins.push_back(arg.substr(9));
        } else if (arg == "--scenario") {
            if (i + 1 < argc) {
                options.scenarioName = argv[++i];
            } else {
                std::cerr << "Error: --scenario option requires a scenario name" << std::endl;
            }
        } else if (arg.find("--scenario=") == 0) {
            options.scenarioName = arg.substr(11);
            options.runScenarios = true;
        } else {
            // Treat as test name
            if (arg.substr(0, 2) == "--" || arg.substr(0, 1) == "-") {
                std::cerr << "Unknown option: " << arg << std::endl;
            } else {
                options.testNames.push_back(arg);
            }
        }
    }
    
    return options;
}

void CLIParser::printHelp(const std::string& programName) {
    std::cout << "Usage: " << programName << " [OPTIONS] [TEST_NAMES...]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help                    Show this help message and exit" << std::endl;
    std::cout << "  --version                     Show version information and exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Test Selection:" << std::endl;
    std::cout << "  [TEST_NAMES]...              Specific test names to run (default: all tests)" << std::endl;
    std::cout << "  --test-category CATEGORY      Filter tests by category" << std::endl;
    std::cout << "  --test-tag TAG                Filter tests by tag" << std::endl;
    std::cout << "  --test-phase PHASE            Filter tests by phase" << std::endl;
    std::cout << "  --exclude-test NAME           Exclude specific test" << std::endl;
    std::cout << "  --test-order ORDER            Order of test execution (alphabetical, category, phase)" << std::endl;
    std::cout << "  --parallel-tests N            Maximum number of parallel test threads (default: 1)" << std::endl;
    std::cout << "  --timeout SECONDS             Maximum test duration in seconds (default: 0 = unlimited)" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  --config FILE                 Configuration file path (default: configs/default.cfg)" << std::endl;
    std::cout << "  --runtime-opt KEY=VALUE       Override runtime configuration" << std::endl;
    std::cout << std::endl;
    std::cout << "Output and Reporting:" << std::endl;
    std::cout << "  --output-dir DIR              Output directory for test results" << std::endl;
    std::cout << "  --output-format FORMAT        Output format (json, xml, csv, plaintext)" << std::endl;
    std::cout << "  --log-level LEVEL             Logging level (debug, info, warning, error, critical)" << std::endl;
    std::cout << "  --log-file FILE               Log file path (default: console)" << std::endl;
    std::cout << "  -v, --verbose                 Enable verbose output" << std::endl;
    std::cout << "  -q, --quiet                   Suppress non-essential output" << std::endl;
    std::cout << "  --progress-interval MSEC      Progress update interval in milliseconds (default: 100)" << std::endl;
    std::cout << std::endl;
    std::cout << "Brain Management:" << std::endl;
    std::cout << "  --reset-brain                Reset brain state between tests" << std::endl;
    std::cout << "  --save-brain [FILE]          Save brain state after tests" << std::endl;
    std::cout << "  --load-brain FILE            Load brain state before tests" << std::endl;
    std::cout << std::endl;
    std::cout << "Plugin System:" << std::endl;
    std::cout << "  --plugin NAME                 Enable plugin (can be specified multiple times)" << std::endl;
    std::cout << std::endl;
    std::cout << "Scenario and Batch Execution:" << std::endl;
    std::cout << "  --scenario NAME              Run scenario with given name" << std::endl;
    std::cout << "  --dry-run                     Show what would be executed without actually running" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << "                                    # Run all tests" << std::endl;
    std::cout << "  " << programName << " connectivity plasticity stdp                  # Run specific tests" << std::endl;
    std::cout << "  " << programName << " --test-category experimental --parallel-tests 4" << std::endl;
    std::cout << "  " << programName << " --scenario learning_experiment --output-format json" << std::endl;
    std::cout << "  " << programName << " --save-brain brain_state.checkpoint --load-brain brain_state.checkpoint" << std::endl;
}

void CLIParser::printVersion() {
    std::cout << "NLM Test Framework v1.0.0" << std::endl;
    std::cout << "Built with C++20, CMake 3.16+" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
}

bool CLIParser::validateOptions(const TestOptions& options, std::vector<std::string>& errors) {
    bool valid = true;
    
    // Validate test names
    for (const auto& name : options.testNames) {
        if (name.empty()) {
            errors.push_back("Test name cannot be empty");
            valid = false;
        }
    }
    
    // Validate categories
    for (const auto& category : options.testCategories) {
        if (category.empty()) {
            errors.push_back("Test category cannot be empty");
            valid = false;
        }
    }
    
    // Validate phases
    for (const auto& phase : options.testPhases) {
        if (phase.empty()) {
            errors.push_back("Test phase cannot be empty");
            valid = false;
        }
    }
    
    // Validate output format
    if (!options.outputFormat.empty()) {
        std::vector<std::string> validFormats = {"json", "xml", "csv", "plaintext"};
        if (std::find(validFormats.begin(), validFormats.end(), options.outputFormat) == validFormats.end()) {
            errors.push_back("Invalid output format: " + options.outputFormat);
            valid = false;
        }
    }
    
    // Validate test order
    if (!options.testOrder.empty()) {
        std::vector<std::string> validOrders = {"alphabetical", "category", "phase", "custom"};
        if (std::find(validOrders.begin(), validOrders.end(), options.testOrder) == validOrders.end()) {
            errors.push_back("Invalid test order: " + options.testOrder);
            valid = false;
        }
    }
    
    // Validate parallel tests
    if (options.maxParallelTests < 1) {
        errors.push_back("Max parallel tests must be at least 1");
        valid = false;
    }
    
    // Validate timeout
    if (options.timeoutSeconds < 0) {
        errors.push_back("Timeout must be non-negative");
        valid = false;
    }
    
    // Validate scenario settings
    if (options.runScenarios && options.scenarioName.empty()) {
        errors.push_back("Scenario name is required when running scenarios");
        valid = false;
    }
    
    // Check for conflicts
    if (!options.testNames.empty() && (options.runScenarios || !options.scenarioName.empty())) {
        errors.push_back("Cannot specify both individual tests and scenarios");
        valid = false;
    }
    
    return valid;
}

std::map<std::string, std::string> CLIParser::getOptionDescriptions() {
    std::map<std::string, std::string> descriptions;
    
    descriptions["help"] = "Show this help message and exit";
    descriptions["version"] = "Show version information and exit";
    descriptions["config"] = "Configuration file path (default: configs/default.cfg)";
    descriptions["output_dir"] = "Output directory for test results";
    descriptions["output_format"] = "Output format (json, xml, csv, plaintext)";
    descriptions["log_level"] = "Logging level (debug, info, warning, error, critical)";
    descriptions["log_file"] = "Log file path (default: console)";
    descriptions["verbose"] = "Enable verbose output";
    descriptions["quiet"] = "Suppress non-essential output";
    descriptions["progress_interval"] = "Progress update interval in milliseconds (default: 100)";
    descriptions["test_category"] = "Filter tests by category";
    descriptions["test_tag"] = "Filter tests by tag";
    descriptions["test_phase"] = "Filter tests by phase";
    descriptions["exclude_test"] = "Exclude specific test";
    descriptions["test_order"] = "Order of test execution (alphabetical, category, phase, custom)";
    descriptions["parallel_tests"] = "Maximum number of parallel test threads (default: 1)";
    descriptions["timeout"] = "Maximum test duration in seconds (default: 0 = unlimited)";
    descriptions["reset_brain"] = "Reset brain state between tests";
    descriptions["save_brain"] = "Save brain state after tests";
    descriptions["load_brain"] = "Load brain state before tests";
    descriptions["plugin"] = "Enable plugin (can be specified multiple times)";
    descriptions["scenario"] = "Run scenario with given name";
    descriptions["dry_run"] = "Show what would be executed without actually running";
    
    return descriptions;
}

std::map<std::string, std::string> CLIParser::getTestOptionDescriptions() {
    std::map<std::string, std::string> descriptions;
    
    descriptions["test_name"] = "Specific test names to run";
    descriptions["test_category"] = "Filter tests by category";
    descriptions["test_tag"] = "Filter tests by tag";
    descriptions["test_phase"] = "Filter tests by phase";
    descriptions["exclude_test"] = "Exclude specific test";
    
    return descriptions;
}

} // namespace nlm
