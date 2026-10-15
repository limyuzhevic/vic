// NLM (熙然) - Neural Learning Machine
// Modular Command Line Interface
// Phase 2: Real Neural Computation

#include "cli/CLI.hpp"
#include <algorithm>
#include <cstring>
#include <vector>

namespace nlm {

CLIParser::CLIParser() : runAllTests_(false), runSimulation_(false) {}

void CLIParser::addOption(const std::string& name, const std::string& shortOpt,
                         const std::string& description, bool required) {
    Option opt;
    opt.name = name;
    opt.shortOpt = shortOpt;
    opt.description = description;
    opt.required = required;
    opt.hasValue = true;
    opt.defaultValue = "";
    opt.set = false;
    options_.push_back(opt);
}

void CLIParser::addFlag(const std::string& name, const std::string& shortOpt,
                       const std::string& description) {
    Option opt;
    opt.name = name;
    opt.shortOpt = shortOpt;
    opt.description = description;
    opt.required = false;
    opt.hasValue = false;
    opt.defaultValue = "";
    opt.set = false;
    options_.push_back(opt);
}

bool CLIParser::parse(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;
        }
        if (arg == "--version" || arg == "-v") {
            printVersion();
            return false;
        }
        
        bool isLongOption = (arg.size() >= 2 && arg[0] == '-' && arg[1] == '-');
        bool isShortOption = (arg.size() >= 2 && arg[0] == '-' && arg[1] != '-');
        
        if (isLongOption) {
            std::string optName = arg.substr(2);
            auto it = std::find_if(options_.begin(), options_.end(),
                                  [&](const Option& o) { return o.name == optName; });
            if (it != options_.end()) {
                if (it->hasValue) {
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        values_[it->name] = argv[++i];
                        it->set = true;
                    } else {
                        std::cerr << "Error: Option --" << it->name << " requires a value" << std::endl;
                        return false;
                    }
                } else {
                    values_[it->name] = "true";
                    it->set = true;
                }
            } else {
                std::cerr << "Error: Unknown option --" << optName << std::endl;
                return false;
            }
        } else if (isShortOption) {
            std::string optFlags = arg.substr(1);
            for (char flag : optFlags) {
                std::string optFlag(1, flag);
                auto it = std::find_if(options_.begin(), options_.end(),
                                      [&](const Option& o) { return o.shortOpt == optFlag; });
                if (it != options_.end()) {
                    if (it->hasValue) {
                        if (i + 1 < argc && argv[i + 1][0] != '-') {
                            values_[it->name] = argv[++i];
                            it->set = true;
                        } else {
                            std::cerr << "Error: Option -" << it->shortOpt << " requires a value" << std::endl;
                            return false;
                        }
                    } else {
                        values_[it->name] = "true";
                        it->set = true;
                    }
                } else {
                    std::cerr << "Error: Unknown option -" << flag << std::endl;
                    return false;
                }
            }
        } else {
            // Positional argument
            if (testNames_.empty()) {
                testNames_.push_back(arg);
            } else {
                std::cerr << "Error: Unexpected positional argument: " << arg << std::endl;
                return false;
            }
        }
    }
    
    // Check required options
    for (const auto& opt : options_) {
        if (opt.required && !opt.set) {
            std::cerr << "Error: Required option --" << opt.name << " not provided" << std::endl;
            return false;
        }
    }
    
    // Special handling for known options
    if (values_.count("config")) configFile_ = values_["config"];
    if (values_.count("output")) outputFile_ = values_["output"];
    if (values_.count("log-level")) logLevel_ = values_["log-level"];
    if (values_.count("test")) {
        // Parse comma-separated list of tests
        std::string testList = values_["test"];
        std::stringstream ss(testList);
        std::string test;
        while (std::getline(ss, test, ',')) {
            if (!test.empty()) testNames_.push_back(test);
        }
    }
    if (values_.count("run-all")) runAllTests_ = true;
    if (values_.count("simulate")) runSimulation_ = true;
    
    return true;
}

bool CLIParser::isSet(const std::string& name) const {
    return values_.count(name) > 0;
}

std::string CLIParser::getString(const std::string& name, const std::string& defaultValue) const {
    auto it = values_.find(name);
    return (it != values_.end()) ? it->second : defaultValue;
}

int CLIParser::getInt(const std::string& name, int defaultValue) const {
    auto it = values_.find(name);
    if (it == values_.end()) return defaultValue;
    try {
        return std::stoi(it->second);
    } catch (...) {
        return defaultValue;
    }
}

double CLIParser::getDouble(const std::string& name, double defaultValue) const {
    auto it = values_.find(name);
    if (it == values_.end()) return defaultValue;
    try {
        return std::stod(it->second);
    } catch (...) {
        return defaultValue;
    }
}

bool CLIParser::getBool(const std::string& name, bool defaultValue) const {
    auto it = values_.find(name);
    if (it == values_.end()) return defaultValue;
    std::string value = it->second;
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return (value == "true" || value == "1" || value == "yes" || value == "on");
}

std::string CLIParser::getConfigFile() const {
    return configFile_;
}

std::string CLIParser::getOutputFile() const {
    return outputFile_;
}

std::string CLIParser::getLogLevel() const {
    return logLevel_;
}

std::vector<std::string> CLIParser::getTestNames() const {
    return testNames_;
}

bool CLIParser::runAllTests() const {
    return runAllTests_;
}

bool CLIParser::runSimulation() const {
    return runSimulation_;
}

bool CLIParser::runHelp() const {
    return isSet("help");
}

bool CLIParser::runVersion() const {
    return isSet("version");
}

void CLIParser::printHelp() const {
    std::cout << "=== NLM Neural Learning Machine ===" << std::endl;
    std::cout << "Usage: nlm [OPTIONS] [tests...]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -v, --version           Show version information" << std::endl;
    std::cout << "  --config <file>         Configuration file path" << std::endl;
    std::cout << "  --output <file>         Output file for results" << std::endl;
    std::cout << "  --log-level <level>     Logging level (DEBUG, INFO, WARN, ERROR)" << std::endl;
    std::cout << "  --test <tests>          Comma-separated list of tests to run" << std::endl;
    std::cout << "  --run-all               Run all available tests" << std::endl;
    std::cout << "  --simulate              Run simulation with current configuration" << std::endl;
    std::cout << std::endl;
    std::cout << "Available tests:" << std::endl;
    std::cout << "  basic-connectivity      Test basic neural connectivity" << std::endl;
    std::cout << "  plasticity              Test plasticity learning experiment" << std::endl;
    std::cout << "  stdp-verification        Test STDP verification" << std::endl;
    std::cout << "  all                     Run all tests" << std::endl;
    std::cout << std::endl;
}

void CLIParser::printVersion() const {
    std::cout << "NLM v0.1.0 - Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Built for experimental artificial developmental brain." << std::endl;
}

} // namespace nlm