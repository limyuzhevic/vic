#include "CLIParser.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cctype>

using namespace std::literals;

namespace nlm {

struct CLIParser::Impl {
    std::shared_ptr<Config> config;
    std::shared_ptr<Logger> logger;
    
    std::string lastError;
    size_t totalArgumentsProcessed;
    size_t successfulParses;
    size_t totalArgumentsConsumed;
    
    std::vector<CLIOption> options;
    std::map<std::string, std::vector<CLIOption>> optionsBySection;
    
    std::string programName;
    std::string version;
    std::string description;
    
    Impl(std::shared_ptr<Config> cfg, std::shared_ptr<Logger> logg)
        : config(cfg), logger(logg), totalArgumentsProcessed(0), successfulParses(0), totalArgumentsConsumed(0) {
        programName = "nlm";
        version = "Phase 2.0";
        description = "Neural Learning Machine - Phase 2: Real Neural Computation";
        
        registerAllOptions();
    }
};

CLIParser::CLIParser(std::shared_ptr<Config> config, std::shared_ptr<Logger> logger) 
    : pImpl(std::make_unique<Impl>(config, logger)) {
}

CLIParser::~CLIParser() = default;

bool CLIParser::parse(int argc, char** argv, CommandLineArgs& args) {
    pImpl->totalArgumentsProcessed = 0;
    pImpl->lastError.clear();
    
    // Reset args to defaults
    args = CommandLineArgs();
    
    if (!parseArguments(argc, argv, args)) {
        return false;
    }
    
    pImpl->successfulParses++;
    return true;
}

bool CLIParser::parseArguments(int argc, char** argv, CommandLineArgs& args) {
    for (int i = 1; i < argc; ++i) {
        pImpl->totalArgumentsProcessed++;
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            if (!parseOption(arg, args)) {
                pImpl->lastError = "Failed to parse option: " + arg;
                return false;
            }
        } else if (arg.substr(0, 1) == "-") {
            if (!parseOption(arg, args)) {
                pImpl->lastError = "Failed to parse short option: " + arg;
                return false;
            }
        } else if (arg.find('=') != std::string::npos) {
            // Handle --key=value format
            if (!parseOption(arg, args)) {
                pImpl->lastError = "Failed to parse key=value pair: " + arg;
                return false;
            }
        } else {
            // Positional argument (could be test name or input file)
            if (!parsePositionalArgument(arg, args)) {
                pImpl->lastError = "Unexpected argument: " + arg;
                return false;
            }
        }
    }
    
    return validateArgs(args);
}

bool CLIParser::parseOption(const std::string& arg, CommandLineArgs& args) {
    // Check if it's a --test option
    if (arg.substr(0, 6) == "--test") {
        return parseTestSelection(arg, args);
    }
    
    // Check if it's a verbosity option
    if (arg == "--verbose" || arg == "-v") {
        args.verbosityLevel++;
        return true;
    } else if (arg == "--quiet" || arg == "-q") {
        args.verbosityLevel--;
        return true;
    } else if (arg == "--progress") {
        args.progressRequested = true;
        return true;
    } else if (arg == "--no-progress") {
        args.noProgress = true;
        return true;
    }
    
    // Check if it's an output option
    if (arg == "--save-results") {
        args.saveResults = true;
        return true;
    } else if (arg.find("--results-file=") == 0) {
        args.resultsFile = arg.substr(16);
        return true;
    } else if (arg == "--plot") {
        args.enablePlotting = true;
        return true;
    } else if (arg.find("--plot-file=") == 0) {
        args.plotFile = arg.substr(13);
        return true;
    }
    
    // Check if it's a timing option
    if (arg.find("--max-time=") == 0) {
        try {
            args.maxExecutionTime = std::chrono::seconds(std::stoul(arg.substr(10)));
        } catch (...) {
            return false;
        }
        return true;
    } else if (arg.find("--step-delay=") == 0) {
        try {
            args.stepDelay = std::stod(arg.substr(13));
        } catch (...) {
            return false;
        }
        return true;
    } else if (arg.find("--simulation-timestep=") == 0) {
        try {
            double timestep = std::stod(arg.substr(22));
            // Update config with timestep
            if (pImpl->config) {
                pImpl->config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
            }
        } catch (...) {
            return false;
        }
        return true;
    }
    
    // Check if it's a config option
    if (arg.find("--config=") == 0) {
        args.configFile = arg.substr(9);
        return true;
    } else if (arg == "--config" && i + 1 < argc) {
        args.configFile = argv[++i];
        pImpl->totalArgumentsConsumed++;
        return true;
    }
    
    // Check for preset option
    if (arg.find("--preset=") == 0) {
        args.presetName = arg.substr(9);
        return true;
    }
    
    // Check for test-specific parameters
    for (const auto& testType : {"plasticity", "stdp", "basic", "structural_plasticity", "neuromodulation", "development", "memory"}) {
        if (arg.find("--" + testType + "-") == 0) {
            size_t dashPos = arg.find('-', testType.length() + 3);
            if (dashPos != std::string::npos && dashPos < arg.length() - 1) {
                std::string paramName = arg.substr(testType.length() + 3, dashPos - (testType.length() + 3));
                std::string paramValue = arg.substr(dashPos + 1);
                args.testParameters[testType][paramName] = paramValue;
                return true;
            }
        }
    }
    
    // Check for generic --key=value
    size_t eqPos = arg.find('=');
    if (eqPos != std::string::npos && eqPos > 2) {
        std::string key = arg.substr(2, eqPos - 2);
        std::string value = arg.substr(eqPos + 1);
        
        // Set in config
        if (pImpl->config) {
            pImpl->config->set(key, value, ConfigSource::CommandLine);
        }
        return true;
    }
    
    return false;
}

bool CLIParser::parseTestSelection(const std::string& arg, CommandLineArgs& args) {
    if (arg == "--test all") {
        args.runAllTests = true;
        args.testTypes = {"basic", "plasticity", "stdp", "structural_plasticity", "neuromodulation", "development", "memory"};
        return true;
    } else if (arg.find("--test=") == 0) {
        std::string testType = arg.substr(7);
        args.testTypes.insert(testType);
        return true;
    } else if (arg.find("-") != std::string::npos && arg.length() > 2) {
        // Handle -t plasticity etc.
        char shortOpt = arg[1];
        if (shortOpt == 't') {
            // Would need access to argv in the context of parseArguments
            // This is simplified - in real implementation would pass through
            return false;
        }
    }
    return false;
}

bool CLIParser::parsePositionalArgument(const std::string& arg, CommandLineArgs& args) {
    // Check if it looks like a test name
    for (const auto& test : {"basic", "plasticity", "stdp", "structural_plasticity", "neuromodulation", "development", "memory"}) {
        if (arg == test) {
            args.testTypes.insert(arg);
            return true;
        }
    }
    
    // Check if it's a configuration file
    if (arg.find(".cfg") != std::string::npos || arg.find(".json") != std::string::npos) {
        args.configFile = arg;
        return true;
    }
    
    // Check if it's an input file
    if (arg.find(".txt") != std::string::npos || arg.find(".data") != std::string::npos) {
        args.inputFile = arg;
        return true;
    }
    
    // Check for help/version
    if (arg == "--help" || arg == "-h") {
        args.helpRequested = true;
        return true;
    } else if (arg == "--version" || arg == "-V") {
        args.versionRequested = true;
        return true;
    } else if (arg == "--quiet" || arg == "-q") {
        args.quietMode = true;
        return true;
    }
    
    pImpl->lastError = "Unknown positional argument: " + arg;
    return false;
}

HelpInfo CLIParser::generateHelp() const {
    HelpInfo help;
    help.programName = pImpl->programName;
    help.version = pImpl->version;
    help.description = pImpl->description;
    
    generateBasicHelp(help);
    generateTestHelp(help);
    generateOutputHelp(help);
    generateTimingHelp(help);
    generateVerbosityHelp(help);
    
    return help;
}

void CLIParser::displayHelp() const {
    HelpInfo help = generateHelp();
    
    std::cout << help.programName << " " << help.version << std::endl;
    std::cout << help.description << std::endl;
    std::cout << std::endl;
    
    for (const auto& section : help.sections) {
        std::cout << "=== " << section << " ===" << std::endl;
        for (const auto& option : help.optionsBySection[section]) {
            std::string shortOpt = (option.shortName != 0) ? std::string("-") + option.shortName : "";
            std::string longOpt = "--" + option.longName;
            
            std::cout << "  " << longOpt;
            if (option.shortName != 0) {
                std::cout << ", " << shortOpt;
            }
            std::cout << std::endl;
            std::cout << "    " << option.description << std::endl;
            if (option.valueType != "flag") {
                std::cout << "    Accepts: " << option.valueType << std::endl;
            }
            std::cout << std::endl;
        }
    }
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --test basic --verbose" << std::endl;
    std::cout << "  nlm --test plasticity --plasticity-steps 2000 --learning-rate 0.02" << std::endl;
    std::cout << "  nlm --test all --max-time 3600 --save-results --verbose" << std::endl;
    std::cout << "  nlm --test stdp --stdp-trials 100 --connectivity-threshold 0.5" << std::endl;
    std::cout << "  nlm --test structural_plasticity --synaptogenesis-rate 0.001 --pruning-rate 0.0001" << std::endl;
    std::cout << "  nlm --test neuromodulation --reward-trials 200 --prediction-error-trials 200" << std::endl;
    std::cout << "  nlm --preset research --test development --maturation-steps 1000" << std::endl;
}

void CLIParser::setConfig(std::shared_ptr<Config> config) {
    pImpl->config = config;
}

std::shared_ptr<Config> CLIParser::getConfig() const {
    return pImpl->config;
}

std::string CLIParser::getLastError() const {
    return pImpl->lastError;
}

bool CLIParser::hasError() const {
    return !pImpl->lastError.empty();
}

bool CLIParser::validateArgs(const CommandLineArgs& args) const {
    // Validate test types
    for (const auto& testType : args.testTypes) {
        if (std::find({"basic", "plasticity", "stdp", "structural_plasticity", 
                       "neuromodulation", "development", "memory"}.begin(),
                      {"basic", "plasticity", "stdp", "structural_plasticity", 
                       "neuromodulation", "development", "memory"}.end(), testType) 
            == {"basic", "plasticity", "stdp", "structural_plasticity", 
                 "neuromodulation", "development", "memory"}.end()) {
            pImpl->lastError = "Unknown test type: " + testType;
            return false;
        }
    }
    
    // Validate test-specific parameters
    for (const auto& [testType, params] : args.testParameters) {
        for (const auto& [param, value] : params) {
            // Basic validation - in real implementation would check parameter types
            if (value.empty()) {
                pImpl->lastError = "Empty value for parameter " + param + " in test " + testType;
                return false;
            }
        }
    }
    
    // Validate results file path
    if (!args.resultsFile.empty() && args.resultsFile.find('/') == std::string::npos) {
        // Simple path validation
    }
    
    return true;
}

size_t CLIParser::getTotalArgumentsProcessed() const {
    return pImpl->totalArgumentsProcessed;
}

size_t CLIParser::getSuccessfulParses() const {
    return pImpl->successfulParses;
}

// Register all options
void CLIParser::registerAllOptions() {
    registerBasicOptions();
    registerTestOptions();
    registerOutputOptions();
    registerTimingOptions();
    registerVerbosityOptions();
}

void CLIParser::registerBasicOptions() {
    // Test selection
    CLIOption testOption;
    testOption.longName = "test";
    testOption.shortName = 't';
    testOption.description = "Test to run (basic, plasticity, stdp, structural_plasticity, neuromodulation, development, memory, or all)";
    testOption.valueType = "string";
    testOption.defaultValue = "all";
    options.push_back(testOption);
    
    // Config file
    CLIOption configOption;
    configOption.longName = "config";
    configOption.shortName = 'c';
    configOption.description = "Configuration file path";
    configOption.valueType = "filepath";
    configOption.defaultValue = "configs/default.cfg";
    options.push_back(configOption);
}

void CLIParser::registerTestOptions() {
    // Plasticity test parameters
    CLIOption plasticitySteps;
    plasticitySteps.longName = "plasticity-steps";
    plasticitySteps.description = "Number of steps for plasticity experiment";
    plasticitySteps.valueType = "integer";
    plasticitySteps.defaultValue = "1000";
    options.push_back(plasticitySteps);
    
    CLIOption learningRate;
    learningRate.longName = "learning-rate";
    learningRate.description = "Learning rate for plasticity experiments";
    learningRate.valueType = "float";
    learningRate.defaultValue = "0.01";
    options.push_back(learningRate);
    
    CLIOption enablePlasticity;
    enablePlasticity.longName = "enable-plasticity";
    enablePlasticity.description = "Enable or disable plasticity";
    enablePlasticity.valueType = "boolean";
    enablePlasticity.defaultValue = "true";
    options.push_back(enablePlasticity);
    
    // STDP test parameters
    CLIOption stdpTrials;
    stdpTrials.longName = "stdp-trials";
    stdpTrials.description = "Number of STDP trials";
    stdpTrials.valueType = "integer";
    stdpTrials.defaultValue = "50";
    options.push_back(stdpTrials);
    
    CLIOption connectivityThreshold;
    connectivityThreshold.longName = "connectivity-threshold";
    connectivityThreshold.description = "Connectivity threshold for network";
    connectivityThreshold.valueType = "float";
    connectivityThreshold.defaultValue = "0.15";
    options.push_back(connectivityThreshold);
}

void CLIParser::registerOutputOptions() {
    // Results output
    CLIOption saveResults;
    saveResults.longName = "save-results";
    saveResults.shortName = 's';
    saveResults.description = "Save test results to file";
    saveResults.valueType = "flag";
    saveResults.defaultValue = "false";
    options.push_back(saveResults);
    
    CLIOption resultsFile;
    resultsFile.longName = "results-file";
    resultsFile.description = "File path for saving results";
    resultsFile.valueType = "filepath";
    resultsFile.defaultValue = "results.json";
    options.push_back(resultsFile);
    
    CLIOption plot;
    plot.longName = "plot";
    plot.description = "Generate plot of results";
    plot.valueType = "flag";
    plot.defaultValue = "false";
    options.push_back(plot);
}

void CLIParser::registerTimingOptions() {
    CLIOption maxTime;
    maxTime.longName = "max-time";
    maxTime.description = "Maximum execution time in seconds";
    maxTime.valueType = "integer";
    maxTime.defaultValue = "3600";
    options.push_back(maxTime);
    
    CLIOption stepDelay;
    stepDelay.longName = "step-delay";
    stepDelay.description = "Delay between simulation steps in seconds";
    stepDelay.valueType = "float";
    stepDelay.defaultValue = "0.001";
    options.push_back(stepDelay);
}

void CLIParser::registerVerbosityOptions() {
    CLIOption verbose;
    verbose.longName = "verbose";
    verbose.shortName = 'v';
    verbose.description = "Increase verbosity level";
    verbose.valueType = "flag";
    verbose.defaultValue = "false";
    options.push_back(verbose);
    
    CLIOption quiet;
    quiet.longName = "quiet";
    quiet.shortName = 'q';
    quiet.description = "Decrease verbosity level";
    quiet.valueType = "flag";
    quiet.defaultValue = "false";
    options.push_back(quiet);
    
    CLIOption progress;
    progress.longName = "progress";
    progress.description = "Enable progress reporting";
    progress.valueType = "flag";
    progress.defaultValue = "false";
    options.push_back(progress);
    
    CLIOption noProgress;
    noProgress.longName = "no-progress";
    noProgress.description = "Disable progress reporting";
    noProgress.valueType = "flag";
    noProgress.defaultValue = "false";
    options.push_back(noProgress);
}

void CLIParser::generateBasicHelp(HelpInfo& help) const {
    help.sections.push_back("Basic Options");
    help.optionsBySection["Basic Options"] = {
        options[0],  // --test
        options[1]   // --config
    };
}

void CLIParser::generateTestHelp(HelpInfo& help) const {
    help.sections.push_back("Test Parameters");
    auto testOptions = std::vector<CLIOption>{
        options[2],  // --plasticity-steps
        options[3],  // --learning-rate
        options[4],  // --enable-plasticity
        options[5],  // --stdp-trials
        options[6]   // --connectivity-threshold
    };
    help.optionsBySection["Test Parameters"] = testOptions;
}

void CLIParser::generateOutputHelp(HelpInfo& help) const {
    help.sections.push_back("Output Options");
    auto outputOptions = std::vector<CLIOption>{
        options[7],  // --save-results
        options[8],  // --results-file
        options[9]   // --plot
    };
    help.optionsBySection["Output Options"] = outputOptions;
}

void CLIParser::generateTimingHelp(HelpInfo& help) const {
    help.sections.push_back("Timing Options");
    auto timingOptions = std::vector<CLIOption>{
        options[10],  // --max-time
        options[11]   // --step-delay
    };
    help.optionsBySection["Timing Options"] = timingOptions;
}

void CLIParser::generateVerbosityHelp(HelpInfo& help) const {
    help.sections.push_back("Verbosity Options");
    auto verbosityOptions = std::vector<CLIOption>{
        options[12],  // --verbose
        options[13],  // --quiet
        options[14],  // --progress
        options[15]   // --no-progress
    };
    help.optionsBySection["Verbosity Options"] = verbosityOptions;
}

// Utility functions
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool stringToBool(const std::string& str) {
    std::string value = toLower(trim(str));
    return value == "true" || value == "1" || value == "yes" || value == "on";
}

double stringToDouble(const std::string& str) {
    return std::stod(trim(str));
}

int stringToInt(const std::string& str) {
    return std::stoi(trim(str));
}

} // namespace nlm
