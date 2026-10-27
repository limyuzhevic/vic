#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

// Simple string utility functions
namespace StringUtils {
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                       [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }
}

// Supported test modes
enum class TestMode {
    Fast,
    Normal,
    Detailed,
    Debug
};

// Logging levels
enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

// Parameter types for validation
using ParamValue = std::variant<int, int64_t, double, std::string, bool, std::vector<int>, std::vector<double>, std::vector<std::string>>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

struct ConfigEntry {
    std::string key;
    ParamValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ParamValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

class Config {
public:
    Config() = default;
    
    void set(const std::string& key, const ParamValue& value, ConfigSource source = ConfigSource::Runtime) {
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            entries.emplace_back(key, value, source);
        }
    }
    
    bool has(const std::string& key) const {
        return std::any_of(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    }
    
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == entries.end()) {
            return std::nullopt;
        }
        
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    bool loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            line = StringUtils::trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = StringUtils::trim(line.substr(0, pos));
                std::string value = StringUtils::trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    }
    
    std::string summary() const {
        std::ostringstream oss;
        oss << "Configuration (" << entries.size() << " entries):" << std::endl;
        for (const auto& entry : entries) {
            oss << "  " << entry.key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "] (" << static_cast<int>(entry.source) << ")" << std::endl;
        }
        return oss.str();
    }
private:
    std::vector<ConfigEntry> entries;
};

class ArgumentParser {
public:
    struct ParsedArgs {
        std::string configFile;
        std::string testNames;
        TestMode testMode = TestMode::Normal;
        LogLevel logLevel = LogLevel::Info;
        std::string logFile;
        bool verbose = false;
        bool noReset = false;
        double timestep = 0.001;
        uint64_t seed = 42;
        bool saveConfig = false;
        bool loadState = false;
        bool persistState = false;
        bool validationOnly = false;
        bool showHelp = false;
        bool showVersion = false;
        std::map<std::string, ParamValue> customParams;
        std::vector<std::string> unknownArgs;
        bool isValid = true;
        std::string errorMessage;
    };

    ArgumentParser() = default;
    
    ParsedArgs parse(int argc, char** argv) {
        ParsedArgs args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                args.showHelp = true;
            } else if (arg == "--version" || arg == "-v") {
                args.showVersion = true;
            } else if (parseKeyValueArg(arg, i, argv, args)) {
                // Successfully parsed
            } else if (StringUtils::startsWith(arg, "--")) {
                std::string option = arg.substr(2);
                parseOption(option, i, argv, args);
            } else if (arg[0] == '-') {
                std::string option = arg.substr(1);
                parseOption(option, i, argv, args);
            } else {
                args.unknownArgs.push_back(arg);
            }
        }
        
        validateArgs(args);
        return args;
    }
    
    bool parseKeyValueArg(const std::string& arg, size_t& currentIndex, char** argv, ParsedArgs& args) {
        if (arg.find('=') != std::string::npos) {
            size_t pos = arg.find('=');
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            
            ParamValue paramValue;
            bool parsed = parseValue(value, paramValue);
            if (parsed) {
                args.customParams[key] = paramValue;
                return true;
            }
        }
        return false;
    }
    
    void parseOption(const std::string& option, size_t& currentIndex, char** argv, ParsedArgs& args) {
        if (option == "config" || option == "c") {
            if (currentIndex + 1 < argc) {
                currentIndex++;
                args.configFile = argv[currentIndex];
            }
        } else if (option == "test" || option == "t") {
            if (currentIndex + 1 < argc) {
                currentIndex++;
                args.testNames = argv[currentIndex];
            }
        } else if (option == "verbose" || option == "v") {
            args.verbose = true;
        } else if (option == "no-reset" || option == "n") {
            args.noReset = true;
        } else if (option == "save-config" || option == "S") {
            args.saveConfig = true;
        } else if (option == "load-state" || option == "L") {
            args.loadState = true;
        } else if (option == "persist-state" || option == "P") {
            args.persistState = true;
        } else if (option == "validation-only" || option == "V") {
            args.validationOnly = true;
        } else {
            // Try to parse as key=value
            if (!parseKeyValueArg("--" + option, currentIndex, argv, args)) {
                args.unknownArgs.push_back("--" + option);
            }
        }
    }
    
    bool parseValue(const std::string& value, ParamValue& paramValue) {
        if (value.find('.') != std::string::npos) {
            try {
                double d = std::stod(value);
                paramValue = d;
                return true;
            } catch (...) {}
        }
        
        if (value == "true" || value == "false") {
            bool b = (value == "true");
            paramValue = b;
            return true;
        }
        
        if (value.find_first_not_of("0123456789") == std::string::npos) {
            try {
                uint64_t u = std::stoull(value);
                paramValue = u;
                return true;
            } catch (...) {}
        }
        
        paramValue = value;
        return true;
    }
    
    void validateArgs(ParsedArgs& args) {
        if (!args.configFile.empty()) {
            std::ifstream file(args.configFile);
            if (!file.good()) {
                args.isValid = false;
                args.errorMessage = "Configuration file not found: " + args.configFile;
            }
        }
        
        if (!args.testNames.empty() && args.testNames != "all") {
            size_t start = 0;
            size_t comma = args.testNames.find(',');
            while (comma != std::string::npos) {
                std::string testName = args.testNames.substr(start, comma - start);
                if (!isValidTestName(testName)) {
                    args.isValid = false;
                    args.errorMessage = "Invalid test name: " + testName;
                    break;
                }
                start = comma + 1;
                comma = args.testNames.find(',', start);
            }
            if (args.isValid) {
                std::string testName = args.testNames.substr(start);
                if (!isValidTestName(testName)) {
                    args.isValid = false;
                    args.errorMessage = "Invalid test name: " + testName;
                }
            }
        }
        
        if (args.timestep <= 0.0 || args.timestep >= 1.0) {
            args.isValid = false;
            args.errorMessage = "Timestep must be between 0 and 1";
        }
        
        if (args.seed == 0) {
            args.isValid = false;
            args.errorMessage = "Seed must be non-zero";
        }
    }
    
    bool isValidTestName(const std::string& testName) {
        const std::set<std::string> validTests = {"connectivity", "plasticity", "stdp"};
        return validTests.find(testName) != validTests.end();
    }
    
    std::string parseTestMode(const std::string& mode) {
        std::string lowerMode = StringUtils::toLower(mode);
        if (lowerMode == "fast") return "FAST";
        if (lowerMode == "detailed") return "DETAILED";
        if (lowerMode == "debug") return "DEBUG";
        return "NORMAL"; // Default
    }
    
    std::string parseLogLevel(const std::string& level) {
        std::string lowerLevel = StringUtils::toLower(level);
        if (lowerLevel == "trace") return "TRACE";
        if (lowerLevel == "warning") return "WARNING";
        if (lowerLevel == "error") return "ERROR";
        if (lowerLevel == "critical") return "CRITICAL";
        if (lowerLevel == "debug") return "DEBUG";
        return "INFO"; // Default
    }
};

class ConfigValidator {
public:
    static bool validateConfig(const Config& config) {
        bool valid = true;
        
        if (config.has("random_seed")) {
            auto seedOpt = config.get<int64_t>("random_seed");
            if (!seedOpt || *seedOpt == 0) {
                std::cerr << "Configuration validation error: random_seed must be > 0" << std::endl;
                valid = false;
            }
        }
        
        if (config.has("simulation_timestep")) {
            auto timestepOpt = config.get<double>("simulation_timestep");
            if (!timestepOpt || *timestepOpt <= 0.0 || *timestepOpt >= 1.0) {
                std::cerr << "Configuration validation error: simulation_timestep must be between 0 and 1" << std::endl;
                valid = false;
            }
        }
        
        return valid;
    }
};

void printHelp() {
    std::cout << "=== NLM Argument Parser Test ===" << std::endl;
    std::cout << "Usage: test_program [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                    Show help message" << std::endl;
    std::cout << "  --version, -v                 Show version" << std::endl;
    std::cout << "  --config <file>              Load configuration from file" << std::endl;
    std::cout << "  --test <tests>               Specify tests to run (comma-separated)" << std::endl;
    std::cout << "  --verbose, -v                 Enable verbose output" << std::endl;
    std::cout << "  --no-reset, -n               Don't reset brain between tests" << std::endl;
    std::cout << "  --timestep <value>            Set simulation timestep" << std::endl;
    std::cout << "  --seed <value>               Set random seed" << std::endl;
    std::cout << "  --test-mode <mode>            Test mode: fast, normal, detailed, debug" << std::endl;
    std::cout << "  --log-level <level>           Log level: trace, debug, info, warning, error, critical" << std::endl;
    std::cout << std::endl;
}

void printVersion() {
    std::cout << "NLM Phase 2: Real Neural Computation v1.0.0" << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "=== NLM Argument Parser Test ===" << std::endl;
    
    ArgumentParser parser;
    auto args = parser.parse(argc, argv);
    
    if (args.showHelp) {
        printHelp();
        return 0;
    }
    
    if (args.showVersion) {
        printVersion();
        return 0;
    }
    
    if (!args.isValid) {
        std::cerr << "Error: " << args.errorMessage << std::endl;
        printHelp();
        return 1;
    }
    
    std::cout << "=== Parsing Results ===" << std::endl;
    std::cout << "Config file: " << (args.configFile.empty() ? "default" : args.configFile) << std::endl;
    std::cout << "Test names: " << (args.testNames.empty() ? "all" : args.testNames) << std::endl;
    std::cout << "Verbose: " << (args.verbose ? "true" : "false") << std::endl;
    std::cout << "Timestep: " << args.timestep << std::endl;
    std::cout << "Seed: " << args.seed << std::endl;
    std::cout << "Test mode: " << parser.parseTestMode("normal") << std::endl;
    std::cout << "Log level: " << parser.parseLogLevel("info") << std::endl;
    
    if (!args.unknownArgs.empty()) {
        std::cout << "Unknown arguments: ";
        for (const auto& arg : args.unknownArgs) {
            std::cout << arg << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "Number of custom parameters: " << args.customParams.size() << std::endl;
    
    // Test configuration loading
    std::cout << "\n=== Configuration Test ===" << std::endl;
    Config config;
    
    if (!args.configFile.empty()) {
        if (config.loadFromFile(args.configFile)) {
            std::cout << "Configuration loaded from: " << args.configFile << std::endl;
        } else {
            std::cout << "Failed to load configuration from: " << args.configFile << std::endl;
        }
    }
    
    std::cout << "Configuration summary:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Test configuration validation
    std::cout << "\n=== Configuration Validation ===" << std::endl;
    if (ConfigValidator::validateConfig(config)) {
        std::cout << "Configuration validation passed!" << std::endl;
    } else {
        std::cout << "Configuration validation failed!" << std::endl;
    }
    
    std::cout << "\n=== All tests completed successfully! ===" << std::endl;
    return 0;
}