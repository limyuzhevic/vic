#include "CommandLineParser.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <iomanip>

using namespace nlm;

CommandLineParser::CommandLineParser(int argc, char** argv) 
    : argc_(argc), argv_(argv) {
}

bool CommandLineParser::parse() {
    NLM_LOG_INFO("Parsing command line arguments...");
    
    for (size_t i = 1; i < static_cast<size_t>(argc_); ++i) {
        if (!parseArgument(argv_[i], i)) {
            NLM_LOG_WARNING("Failed to parse argument: " + std::string(argv_[i]));
        }
    }
    
    return true;
}

bool CommandLineParser::parseArgument(const std::string& arg, size_t& currentArg) {
    if (arg == "--help" || arg == "-h") {
        helpRequested_ = true;
        return true;
    }
    
    if (arg == "--version") {
        versionRequested_ = true;
        return true;
    }
    
    if (arg == "--batch") {
        batchMode_ = true;
        return true;
    }
    
    if (arg == "--verbose") {
        verboseMode_ = true;
        return true;
    }
    
    if (arg.find("--config") == 0) {
        return parseConfigArgument(arg);
    }
    
    if (arg.find("--log") == 0) {
        return parseLogArgument(arg);
    }
    
    // Unknown argument
    unrecognizedArgs_.push_back(arg);
    return false;
}

bool CommandLineParser::parseConfigArgument(const std::string& arg) {
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        configFilePath_ = arg.substr(pos + 1);
    } else if (currentArg + 1 < static_cast<size_t>(argc_)) {
        configFilePath_ = argv_[++currentArg];
    }
    return !configFilePath_.empty();
}

bool CommandLineParser::parseLogArgument(const std::string& arg) {
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        logLevel_ = arg.substr(pos + 1);
        std::transform(logLevel_.begin(), logLevel_.end(), logLevel_.begin(), ::tolower);
    } else if (currentArg + 1 < static_cast<size_t>(argc_)) {
        logLevel_ = argv_[++currentArg];
        std::transform(logLevel_.begin(), logLevel_.end(), logLevel_.begin(), ::tolower);
    }
    return !logLevel_.empty();
}

std::string CommandLineParser::getConfigFilePath() const {
    return configFilePath_;
}

std::string CommandLineParser::getLogLevel() const {
    return logLevel_;
}

std::string CommandLineParser::getLogFile() const {
    return logFile_;
}

bool CommandLineParser::isHelpRequested() const {
    return helpRequested_;
}

bool CommandLineParser::isVersionRequested() const {
    return versionRequested_;
}

bool CommandLineParser::isBatchMode() const {
    return batchMode_;
}

bool CommandLineParser::isVerboseMode() const {
    return verboseMode_;
}

std::vector<std::string> CommandLineParser::getUnrecognizedArgs() const {
    return unrecognizedArgs_;
}

void CommandLineParser::displayHelp() const {
    std::cout << "=== NLM Command Line Options ===" << std::endl;
    std::cout << "  --help, -h                Display this help message" << std::endl;
    std::cout << "  --version                 Display version information" << std::endl;
    std::cout << "  --batch                   Run in batch mode (non-interactive)" << std::endl;
    std::cout << "  --verbose                 Enable verbose logging" << std::endl;
    std::cout << "  --config=<file>           Load configuration from file" << std::endl;
    std::cout << "  --log=<level>             Set log level (debug, info, warning, error, critical)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm --config=myconfig.cfg --log=info" << std::endl;
    std::cout << "  ./nlm --batch --verbose" << std::endl;
}

void CommandLineParser::displayVersion() const {
    std::cout << "NLM Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Version: 2.0.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
}

std::vector<std::string> CommandLineParser::validateArguments() const {
    std::vector<std::string> errors;
    
    // Validate log level
    std::vector<std::string> validLogLevels = {"debug", "info", "warning", "error", "critical"};
    if (!logLevel_.empty()) {
        if (std::find(validLogLevels.begin(), validLogLevels.end(), logLevel_) == validLogLevels.end()) {
            errors.push_back("Invalid log level: " + logLevel_);
        }
    }
    
    // Validate config file if specified
    if (!configFilePath_.empty()) {
        std::ifstream file(configFilePath_);
        if (!file.good()) {
            errors.push_back("Configuration file not found: " + configFilePath_);
        }
    }
    
    return errors;
}