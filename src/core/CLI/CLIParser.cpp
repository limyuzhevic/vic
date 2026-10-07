// NLM (熙然) - Neural Learning Machine
// CLI Parser Implementation for Phase 2

#include "core/CLI/CLIConfig.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cctype>

namespace nlm {

// CLIParser implementation
CLIParser::CLIParser(int argc, char** argv) : argc(argc), argv(argv) {}

bool CLIParser::parse(int argc, char** argv) {
    // Initialize config with defaults
    config.configFile = "configs/default.cfg";
    config.testMode = TestMode::None;
    config.verbose = false;
    config.quiet = false;
    config.logLevel = "INFO";
    config.outputFormat = "text";
    config.maxSteps = 1000;
    config.timestep = 0.001;
    config.showHelp = false;
    config.showUsage = false;
    config.showExamples = false;
    
    // Parse all arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            // Handle --key=value format
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (parseConfigFile("--" + key)) config.configFile = value;
                else if (parseTestMode("--" + key)) config.testMode = stringToTestMode(value);
                else if (parseVerbose("--" + key)) config.verbose = true;
                else if (parseQuiet("--" + key)) config.quiet = true;
                else if (parseLogLevel("--" + key)) config.logLevel = value;
                else if (parseOutputFormat("--" + key)) config.outputFormat = value;
                else if (parseRunTests("--" + key)) config.runTests = splitString(value, ',');
                else if (parseSkipTests("--" + key)) config.skipTests = splitString(value, ',');
                else if (parseSteps("--" + key)) config.maxSteps = std::stoi(value);
                else if (parseTimestep("--" + key)) config.timestep = std::stod(value);
                else if (parseHelp("--" + key)) {
                    config.showHelp = true;
                    config.showUsage = true;
                    config.showExamples = true;
                }
            } else {
                // Handle flag arguments
                if (parseConfigFile(arg)) config.configFile = "";
                else if (parseTestMode(arg)) config.showHelp = true;
                else if (parseVerbose(arg)) config.verbose = true;
                else if (parseQuiet(arg)) config.quiet = true;
                else if (parseLogLevel(arg)) config.showHelp = true;
                else if (parseOutputFormat(arg)) config.showHelp = true;
                else if (parseRunTests(arg)) config.showHelp = true;
                else if (parseSkipTests(arg)) config.showHelp = true;
                else if (parseSteps(arg)) config.showHelp = true;
                else if (parseTimestep(arg)) config.showHelp = true;
                else if (parseHelp(arg)) {
                    config.showHelp = true;
                    config.showUsage = true;
                    config.showExamples = true;
                }
            }
        } else if (arg[0] == '-') {
            // Handle -key value format
            std::string key = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                
                if (key == "config") config.configFile = value;
                else if (key == "test-mode") config.testMode = stringToTestMode(value);
                else if (key == "verbose") config.verbose = true;
                else if (key == "quiet") config.quiet = true;
                else if (key == "log-level") config.logLevel = value;
                else if (key == "format") config.outputFormat = value;
                else if (key == "run-tests") config.runTests = splitString(value, ',');
                else if (key == "skip-tests") config.skipTests = splitString(value, ',');
                else if (key == "steps") config.maxSteps = std::stoi(value);
                else if (key == "timestep") config.timestep = std::stod(value);
                else if (key == "help" || key == "usage" || key == "examples") {
                    config.showHelp = true;
                    config.showUsage = true;
                    config.showExamples = true;
                }
            }
        }
    }
    
    // Convert testMode to string
    config.testModeString = testModeToString(config.testMode);
    
    // Validate configuration
    return config.validate();
}

void CLIParser::showHelp() {
    std::cout << "NLM Phase 2 - Neural Learning Machine CLI Options\n"
              << std::endl;
    std::cout << "Required arguments:\n"
              << std::endl;
    std::cout << "  --config <file>                  Load configuration from file\n"
              << std::endl;
    std::cout << "Optional arguments:\n"
              << std::endl;
    std::cout << "  --test-mode <mode>              Set test mode\n"
              << std::endl;
    std::cout << "    Connectivity                   Run basic neural connectivity tests\n"
              << std::endl;
    std::cout << "    Plasticity                     Run plasticity learning experiments\n"
              << std::endl;
    std::cout << "    STDP                           Run STDP verification tests\n"
              << std::endl;
    std::cout << "    All                            Run all tests\n"
              << std::endl;
    std::cout << "    Quick                          Run quick connectivity test only\n"
              << std::endl;
    std::cout << std::endl;
    std::cout << "  --verbose                        Enable verbose output\n"
              << std::endl;
    std::cout << "  --quiet                         Suppress output\n"
              << std::endl;
    std::cout << "  --log-level <level>             Set logging level (DEBUG, INFO, WARNING, ERROR)\n"
              << std::endl;
    std::cout << "  --format <format>               Output format (json, text, csv)\n"
              << std::endl;
    std::cout << "  --run-tests <test1,test2,...>   Specify which tests to run\n"
              << std::endl;
    std::cout << "  --skip-tests <test1,test2,...>  Specify which tests to skip\n"
              << std::endl;
    std::cout << "  --steps <count>                 Maximum simulation steps\n"
              << std::endl;
    std::cout << "  --timestep <dt>                 Simulation timestep\n"
              << std::endl;
    std::cout << "  --help / --usage / --examples    Display help information\n"
              << std::endl;
}

void CLIParser::showUsage() {
    std::cout << "Usage: nlm [options]\n"
              << std::endl;
    std::cout << "  nlm --test-mode all --steps 1000 --timestep 0.001\n"
              << std::endl;
    std::cout << "  nlm --config configs/custom.cfg --run-tests connectivity,plasticity\n"
              << std::endl;
    std::cout << "  nlm --verbose --format json\n"
              << std::endl;
}

void CLIParser::showExamples() {
    std::cout << "Examples:\n"
              << std::endl;
    std::cout << "  # Run basic connectivity test\n"
              << std::endl;
    std::cout << "  nlm --test-mode connectivity\n"
              << std::endl;
    std::cout << std::endl;
    std::cout << "  # Run all tests in text format\n"
              << std::endl;
    std::cout << "  nlm --test-mode all --format text --steps 1000\n"
              << std::endl;
    std::cout << std::endl;
    std::cout << "  # Run specific tests with custom config\n"
              << std::endl;
    std::cout << "  nlm --config my_config.cfg --run-tests stdp,plasticity --verbose\n"
              << std::endl;
}

TestMode CLIParser::stringToTestMode(const std::string& mode) {
    std::string lowerMode = toLower(mode);
    if (lowerMode == "connectivity") return TestMode::Connectivity;
    if (lowerMode == "plasticity") return TestMode::Plasticity;
    if (lowerMode == "stdp") return TestMode::STDP;
    if (lowerMode == "all") return TestMode::All;
    if (lowerMode == "quick") return TestMode::Quick;
    return TestMode::None;
}

std::string CLIParser::testModeToString(TestMode mode) {
    switch (mode) {
        case TestMode::Connectivity: return "connectivity";
        case TestMode::Plasticity: return "plasticity";
        case TestMode::STDP: return "stdp";
        case TestMode::All: return "all";
        case TestMode::Quick: return "quick";
        case TestMode::None: return "none";
        default: return "unknown";
    }
}

LogLevel CLIParser::stringToLogLevel(const std::string& level) {
    std::string lowerLevel = toLower(level);
    if (lowerLevel == "debug") return LogLevel::Debug;
    if (lowerLevel == "info") return LogLevel::Info;
    if (lowerLevel == "warning") return LogLevel::Warning;
    if (lowerLevel == "error") return LogLevel::Error;
    if (lowerLevel == "critical") return LogLevel::Critical;
    return LogLevel::Info;
}

std::string CLIParser::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default: return "INFO";
    }
}

std::string CLIParser::stringToOutputFormat(const std::string& format) {
    std::string lowerFormat = toLower(format);
    if (lowerFormat == "json") return "json";
    if (lowerFormat == "text") return "text";
    if (lowerFormat == "csv") return "csv";
    return "text";
}

bool CLIParser::shouldRunTest(const std::string& testName) const {
    if (runTests.empty()) return true;
    for (const auto& test : runTests) {
        if (test == testName) return true;
    }
    return false;
}

bool CLIParser::shouldSkipTest(const std::string& testName) const {
    for (const auto& test : skipTests) {
        if (test == testName) return true;
    }
    return false;
}

// Helper function implementations
bool CLIParser::parseConfigFile(const std::string& arg) {
    return arg == "--config" || arg == "-config" || arg == "config";
}

bool CLIParser::parseTestMode(const std::string& arg) {
    return arg == "--test-mode" || arg == "-test-mode" || arg == "test-mode";
}

bool CLIParser::parseVerbose(const std::string& arg) {
    return arg == "--verbose" || arg == "-verbose" || arg == "verbose";
}

bool CLIParser::parseQuiet(const std::string& arg) {
    return arg == "--quiet" || arg == "-quiet" || arg == "quiet";
}

bool CLIParser::parseLogLevel(const std::string& arg) {
    return arg == "--log-level" || arg == "-log-level" || arg == "log-level";
}

bool CLIParser::parseOutputFormat(const std::string& arg) {
    return arg == "--format" || arg == "-format" || arg == "format";
}

bool CLIParser::parseRunTests(const std::string& arg) {
    return arg == "--run-tests" || arg == "-run-tests" || arg == "run-tests";
}

bool CLIParser::parseSkipTests(const std::string& arg) {
    return arg == "--skip-tests" || arg == "-skip-tests" || arg == "skip-tests";
}

bool CLIParser::parseSteps(const std::string& arg) {
    return arg == "--steps" || arg == "-steps" || arg == "steps";
}

bool CLIParser::parseTimestep(const std::string& arg) {
    return arg == "--timestep" || arg == "-timestep" || arg == "timestep";
}

bool CLIParser::parseHelp(const std::string& arg) {
    return arg == "--help" || arg == "--usage" || arg == "--examples" ||
           arg == "-help" || arg == "-usage" || arg == "-examples";
}

std::vector<std::string> CLIParser::splitString(const std::string& str, char delimiter) const {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        item = trim(item);
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

bool CLIParser::isFlag(const std::string& arg) const {
    return arg == "--verbose" || arg == "--quiet" || arg == "--help" ||
           arg == "--usage" || arg == "--examples";
}

std::string CLIParser::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string CLIParser::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // namespace nlm
