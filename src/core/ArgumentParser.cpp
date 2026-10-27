#include "ArgumentParser.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

ArgumentParser::ArgumentParser() 
    : testMode_(TestMode::Normal), verbose_(false), seed_(42), resetBetweenTests_(true),
      persistState_(false), saveConfig_(false), loadState_(false), logToFile_(false),
      logFileName_("nlm.log"), simulationSteps_(10000), energyDecayRate_(0.1f),
      maxEnergy_(100.0f), environmentDimensions_(10, 10), visualizationEnabled_(false),
      visualizationUpdateRate_(30.0f), worldRandomSeed_(42) {}

void ArgumentParser::addArgument(const std::string& name, char shortName, 
                                const std::string& description, const std::string& type,
                                const std::string& defaultValue, bool required,
                                const std::vector<std::string>& validValues) {
    ArgumentConfig config(name, shortName, description, type, defaultValue, required, validValues);
    argumentConfigs_.push_back(config);
    
    shortNameToArgIndex_[std::string(1, shortName)] = argumentConfigs_.size() - 1;
}

bool ArgumentParser::parse(int argc, char** argv, ParsedArgs& parsedArgs, 
                           const std::string& configFile) {
    // Parse configuration file first if provided
    if (!configFile.empty()) {
        // Load configuration from file (simplified implementation)
        configFile_ = configFile;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printHelp();
            return false; // Exit after showing help
        } else if (arg == "--version") {
            printVersion();
            return false; // Exit after showing version
        } else if (arg == "--verbose") {
            verbose_ = true;
            logLevel_ = "DEBUG";
        } else if (arg == "--no-reset") {
            resetBetweenTests_ = false;
        } else if (arg == "--persist-state") {
            persistState_ = true;
        } else if (arg == "--save-config") {
            saveConfig_ = true;
        } else if (arg == "--load-state") {
            loadState_ = true;
        } else if (arg == "--log-to-file") {
            logToFile_ = true;
        } else if (arg.substr(0, 4) == "--log=") {
            logLevel_ = arg.substr(4);
            std::transform(logLevel_.begin(), logLevel_.end(), logLevel_.begin(), ::toupper);
        } else if (arg.substr(0, 12) == "--config-file=") {
            configFile_ = arg.substr(12);
        } else if (arg.substr(0, 11) == "--test-mode=") {
            std::string mode = arg.substr(11);
            if (mode == "fast") testMode_ = TestMode::Fast;
            else if (mode == "normal") testMode_ = TestMode::Normal;
            else if (mode == "detailed") testMode_ = TestMode::Detailed;
            else if (mode == "debug") testMode_ = TestMode::Debug;
        } else if (arg.substr(0, 6) == "--test=") {
            testNames_ = splitString(arg.substr(6), ',');
        } else if (arg.substr(0, 12) == "--timestep=") {
            timestep_ = std::stod(arg.substr(12));
        } else if (arg.substr(0, 10) == "--seed=") {
            seed_ = std::stoull(arg.substr(10));
        } else if (arg.substr(0, 15) == "--log-filename=") {
            logFileName_ = arg.substr(15);
        } else if (arg.substr(0, 21) == "--max-simulation-steps=") {
            simulationSteps_ = std::stoi(arg.substr(21));
        } else if (arg.substr(0, 21) == "--energy-decay-rate=") {
            energyDecayRate_ = std::stof(arg.substr(21));
        } else if (arg.substr(0, 13) == "--max-energy=") {
            maxEnergy_ = std::stof(arg.substr(13));
        } else if (arg.substr(0, 21) == "--env-width=") {
            environmentDimensions_.first = std::stoi(arg.substr(12));
        } else if (arg.substr(0, 22) == "--env-height=") {
            environmentDimensions_.second = std::stoi(arg.substr(13));
        } else if (arg.substr(0, 24) == "--visualization-rate=") {
            visualizationUpdateRate_ = std::stof(arg.substr(24));
        } else if (arg.substr(0, 27) == "--world-random-seed=") {
            worldRandomSeed_ = std::stoull(arg.substr(27));
        } else if (arg.substr(0, 24) == "--enable-visualization") {
            visualizationEnabled_ = true;
        } else {
            // Check if it's a short option
            if (isShortOption(arg)) {
                if (arg == "-v" || arg == "-V") {
                    verbose_ = true;
                    logLevel_ = (arg == "-V") ? "VERBOSE" : "DEBUG";
                } else if (arg == "-h" || arg == "-H") {
                    printHelp();
                    return false;
                } else if (arg == "-q") {
                    logLevel_ = "ERROR";
                } else if (arg == "-r") {
                    resetBetweenTests_ = false;
                } else if (arg == "-p") {
                    persistState_ = true;
                } else if (arg == "-s") {
                    saveConfig_ = true;
                } else if (arg == "-l") {
                    logToFile_ = true;
                } else if (arg.size() > 2 && arg[1] == '=') {
                    // Handle -k=value format for short options
                    char keyChar = arg[0];
                    std::string value = arg.substr(2);
                    if (keyChar == 't') {
                        testNames_ = splitString(value, ',');
                    } else if (keyChar == 'm') {
                        timestep_ = std::stod(value);
                    } else if (keyChar == 'S') {
                        seed_ = std::stoull(value);
                    }
                } else if (arg.size() > 1 && isalpha(arg[1])) {
                    // Handle -k value format for short options
                    char keyChar = arg[0];
                    if (i + 1 < argc) {
                        std::string nextArg = argv[++i];
                        if (keyChar == 't') {
                            testNames_ = splitString(nextArg, ',');
                        } else if (keyChar == 'm') {
                            timestep_ = std::stod(nextArg);
                        } else if (keyChar == 'S') {
                            seed_ = std::stoull(nextArg);
                        }
                    }
                } else {
                    // Unknown option
                    std::cerr << "Unknown option: " << arg << std::endl;
                    return false;
                }
            } else if (isLongOption(arg)) {
                // Long option with = format already handled above
                // Check if it's a positional argument
                if (isPositionalArg(arg)) {
                    parsedArgs.positionalArgs.push_back(arg);
                } else {
                    std::cerr << "Unknown option: " << arg << std::endl;
                    return false;
                }
            } else {
                // Positional argument
                parsedArgs.positionalArgs.push_back(arg);
            }
        }
    }
    
    return true;
}

std::string ArgumentParser::getHelpText() const {
    std::ostringstream oss;
    
    oss << "NLM Phase 2 Enhanced Command-Line Interface" << std::endl;
    oss << "=============================================" << std::endl << std::endl;
    
    oss << "Usage: nlm [options]" << std::endl << std::endl;
    
    oss << "Core Options:" << std::endl;
    oss << "  --help, -h, -H                Show this help message" << std::endl;
    oss << "  --version                     Show version information" << std::endl;
    oss << "  --verbose, -v                 Enable verbose output" << std::endl;
    oss << "  --quiet, -q                   Quiet mode (errors only)" << std::endl << std::endl;
    
    oss << "Test Selection:" << std::endl;
    oss << "  --test=<test1,test2,...>, -t <tests>     Select tests to run (default: all)" << std::endl;
    oss << "  --test-mode=<mode>          Test execution mode (fast, normal, detailed, debug)" << std::endl;
    oss << "  --no-reset, -r               Don't reset brain between tests" << std::endl << std::endl;
    
    oss << "Configuration:" << std::endl;
    oss << "  --config-file=<file>, -c <file>    Load configuration from file" << std::endl;
    oss << "  --save-config, -s             Save current configuration" << std::endl;
    oss << "  --load-state                 Load persistent state" << std::endl;
    oss << "  --persist-state, -p          Persist state across runs" << std::endl << std::endl;
    
    oss << "Logging:" << std::endl;
    oss << "  --log-to-file, -l             Enable logging to file" << std::endl;
    oss << "  --log=<level>                Set logging level (debug, info, warning, error)" << std::endl;
    oss << "  --log-filename=<file>        Set log file name" << std::endl << std::endl;
    
    oss << "Simulation Parameters:" << std::endl;
    oss << "  --timestep=<value>, -m <value>     Simulation timestep (default: 0.001)" << std::endl;
    oss << "  --seed=<value>, -S <value>        Random seed (default: 42)" << std::endl;
    oss << "  --max-simulation-steps=<n>         Maximum simulation steps (default: 10000)" << std::endl;
    oss << "  --energy-decay-rate=<r>            Energy decay rate (default: 0.1)" << std::endl;
    oss << "  --max-energy=<e>                   Maximum energy (default: 100.0)" << std::endl << std::endl;
    
    oss << "Environment:" << std::endl;
    oss << "  --env-width=<w>, --width=<w>      Environment width (default: 10)" << std::endl;
    oss << "  --env-height=<h>, --height=<h>    Environment height (default: 10)" << std::endl << std::endl;
    
    oss << "Visualization:" << std::endl;
    oss << "  --enable-visualization            Enable visualization" << std::endl;
    oss << "  --visualization-rate=<r>         Visualization update rate (default: 30.0)" << std::endl;
    oss << "  --world-random-seed=<seed>        World random seed (default: 42)" << std::endl << std::endl;
    
    oss << "Available Tests:" << std::endl;
    oss << "  connectivity                   Basic neural connectivity test" << std::endl;
    oss << "  plasticity                     Plasticity learning experiment" << std::endl;
    oss << "  stdp                           STDP verification test" << std::endl << std::endl;
    
    oss << "Test Modes:" << std::endl;
    oss << "  fast      : Quick execution with minimal logging" << std::endl;
    oss << "  normal    : Standard execution with balanced logging" << std::endl;
    oss << "  detailed  : Detailed execution with extensive logging" << std::endl;
    oss << "  debug     : Debug execution with maximum logging" << std::endl << std::endl;
    
    oss << "Examples:" << std::endl;
    oss << "  nlm --help                                     Show help" << std::endl;
    oss << "  nlm --test=connectivity,plasticity --verbose   Run connectivity and plasticity tests" << std::endl;
    oss << "  nlm -t connectivity,plasticity -v              Same as above (short form)" << std::endl;
    oss << "  nlm --test-mode=fast --test=stdp               Run STDP test in fast mode" << std::endl;
    oss << "  nlm --config-file=experiments.cfg --save-config Load config and save current settings" << std::endl;
    oss << "  nlm --log=debug --verbose                        Enable debug logging" << std::endl;
    
    return oss.str();
}

bool ArgumentParser::validate(const ParsedArgs& parsedArgs, std::string& errorMessage) const {
    bool valid = true;
    
    // Validate timestep
    if (timestep_ <= 0.0 || timestep_ >= 1.0) {
        errorMessage += "Error: timestep must be between 0 and 1" + std::string("\n");
        valid = false;
    }
    
    // Validate simulation steps
    if (simulationSteps_ <= 0) {
        errorMessage += "Error: max-simulation-steps must be positive" + std::string("\n");
        valid = false;
    }
    
    // Validate energy parameters
    if (energyDecayRate_ < 0.0f) {
        errorMessage += "Error: energy-decay-rate must be non-negative" + std::string("\n");
        valid = false;
    }
    
    if (maxEnergy_ < 0.0f) {
        errorMessage += "Error: max-energy must be non-negative" + std::string("\n");
        valid = false;
    }
    
    // Validate environment dimensions
    if (environmentDimensions_.first <= 0 || environmentDimensions_.second <= 0) {
        errorMessage += "Error: environment dimensions must be positive" + std::string("\n");
        valid = false;
    }
    
    // Validate test names
    for (const auto& testName : testNames_) {
        if (testName != "connectivity" && testName != "plasticity" && testName != "stdp") {
            errorMessage += "Error: unknown test name: " + testName + std::string("\n");
            valid = false;
        }
    }
    
    // Validate log level
    std::string logLevel = logLevel_;
    std::transform(logLevel.begin(), logLevel.end(), logLevel.begin(), ::toupper);
    if (logLevel != "DEBUG" && logLevel != "INFO" && logLevel != "WARNING" && logLevel != "ERROR") {
        errorMessage += "Error: log level must be one of: debug, info, warning, error" + std::string("\n");
        valid = false;
    }
    
    return valid;
}

void ArgumentParser::printHelp() const {
    std::cout << getHelpText();
}

void ArgumentParser::printVersion() const {
    std::cout << getVersion() << std::endl;
}

std::vector<std::string> ArgumentParser::splitString(const std::string& str, char delimiter) const {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

std::string ArgumentParser::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ArgumentParser::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string ArgumentParser::intToString(int value) const {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string ArgumentParser::doubleToString(double value) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << value;
    return oss.str();
}

bool ArgumentParser::stringToBool(const std::string& str) const {
    std::string lower = toLower(str);
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

std::vector<std::string> ArgumentParser::stringToList(const std::string& str) const {
    return splitString(str, ',');
}

std::string ArgumentParser::getValueFromArg(const std::string& arg, const std::string& expectedKey) const {
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        return arg.substr(pos + 1);
    }
    return "";
}

bool ArgumentParser::isShortOption(const std::string& arg) const {
    return arg.size() > 1 && arg[0] == '-' && std::isalpha(arg[1]);
}

bool ArgumentParser::isLongOption(const std::string& arg) const {
    return arg.size() > 2 && arg[0] == '-' && arg[1] == '-';
}

bool ArgumentParser::isPositionalArg(const std::string& arg) const {
    // Simplified check - in a real implementation, this would be more sophisticated
    return arg.find('=') == std::string::npos && 
           !(arg.size() > 1 && arg[0] == '-' && std::isalpha(arg[1])) &&
           !(arg.size() > 2 && arg[0] == '-' && arg[1] == '-');
}

std::string ArgumentParser::getCanonicalKey(const std::string& arg) const {
    // Convert argument to canonical key format
    std::string key = arg;
    if (key.find('=') != std::string::npos) {
        key = key.substr(0, key.find('='));
    }
    
    // Remove leading dashes
    if (key.find("--") == 0) {
        key = key.substr(2);
    } else if (key.find('-') == 0) {
        key = key.substr(1);
    }
    
    return key;
}

std::string ArgumentParser::formatHelpKey(const ArgumentConfig& config) const {
    std::string helpKey = "--" + config.name;
    if (config.shortName != ' ') {
        helpKey += ", -" + std::string(1, config.shortName);
    }
    return helpKey;
}
