#include "TestConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <nlohmann/json.hpp>

namespace nlm {

// Static member initialization
std::map<std::string, std::function<void(TestConfig&, const std::string&)>> 
    TestConfig::paramParsers = TestConfig::initializeParamParsers();

TestConfig TestConfig::fromCLI(const CLIParser::TestOptions& options) {
    TestConfig config;
    
    // Load from config file if specified
    if (!options.configFile.empty()) {
        config.loadFromFile(options.configFile);
    }
    
    // Apply CLI overrides
    config.parameters_.outputDirectory = options.outputDirectory;
    config.parameters_.outputFormat = options.outputFormat;
    config.parameters_.logLevel = options.logLevel;
    config.parameters_.logFile = options.logFile;
    config.parameters_.verbose = options.verbose;
    config.parameters_.quiet = options.quiet;
    config.parameters_.quietProgress = options.quietProgress;
    config.parameters_.progressInterval = options.progressInterval;
    
    // Plugin settings
    config.parameters_.enabledPlugins = options.enabledPlugins;
    config.parameters_.disabledPlugins = options.disabledPlugins;
    
    // Brain settings
    config.parameters_.resetBrainBetweenTests = options.resetBrainBetweenTests;
    config.parameters_.saveBrainState = options.saveBrainState;
    config.parameters_.brainSavePath = options.brainSavePath;
    config.parameters_.restoreBrainState = options.restoreBrainState;
    config.parameters_.brainLoadPath = options.brainLoadPath;
    
    // Scenario settings
    config.parameters_.scenarioFile = options.scenarioFile;
    config.parameters_.scenarioName = options.scenarioName;
    config.parameters_.runScenarios = options.runScenarios;
    
    // Performance settings
    config.parameters_.maxParallelTests = options.maxParallelTests;
    config.parameters_.timeoutSeconds = options.timeoutSeconds;
    
    // Apply runtime config
    for (const auto& pair : options.runtimeConfig) {
        setParameter(pair.first, pair.second);
    }
    
    // Merge with defaults
    config.mergeWithDefaults();
    
    return config;
}

TestConfig TestConfig::fromFile(const std::string& filepath) {
    TestConfig config;
    config.loadFromFile(filepath);
    config.mergeWithDefaults();
    return config;
}

TestConfig TestConfig::fromDefaults() {
    TestConfig config;
    config.mergeWithDefaults();
    return config;
}

void TestConfig::mergeWithDefaults() {
    // This ensures all required fields have default values
    if (parameters_.outputDirectory.empty()) {
        parameters_.outputDirectory = "./test_results";
    }
    
    if (parameters_.logLevel.empty()) {
        parameters_.logLevel = "info";
    }
    
    if (parameters_.logFile.empty()) {
        parameters_.logFile = "";
    }
}

bool TestConfig::validate(std::vector<std::string>& errors) const {
    bool valid = true;
    
    // Validate output directory
    if (!parameters_.outputDirectory.empty()) {
        // Directory creation would happen here
    }
    
    // Validate output format
    std::vector<std::string> validFormats = {"json", "xml", "csv", "plaintext"};
    if (!parameters_.outputFormat.empty() &&
        std::find(validFormats.begin(), validFormats.end(), parameters_.outputFormat) == validFormats.end()) {
        errors.push_back("Invalid output format: " + parameters_.outputFormat);
        valid = false;
    }
    
    // Validate log level
    std::vector<std::string> validLevels = {"debug", "info", "warning", "error", "critical"};
    if (!parameters_.logLevel.empty() &&
        std::find(validLevels.begin(), validLevels.end(), parameters_.logLevel) == validLevels.end()) {
        errors.push_back("Invalid log level: " + parameters_.logLevel);
        valid = false;
    }
    
    // Validate progress interval
    if (parameters_.progressInterval <= 0) {
        errors.push_back("Progress interval must be positive");
        valid = false;
    }
    
    // Validate parallel tests
    if (parameters_.maxParallelTests < 1) {
        errors.push_back("Max parallel tests must be at least 1");
        valid = false;
    }
    
    // Validate timeout
    if (parameters_.timeoutSeconds < 0) {
        errors.push_back("Timeout must be non-negative");
        valid = false;
    }
    
    return valid;
}

std::string TestConfig::getString(const std::string& key, const std::string& defaultValue) const {
    return config_.getOr<std::string>(key, defaultValue);
}

int TestConfig::getInt(const std::string& key, int defaultValue) const {
    return config_.getOr<int>(key, defaultValue);
}

double TestConfig::getDouble(const std::string& key, double defaultValue) const {
    return config_.getOr<double>(key, defaultValue);
}

bool TestConfig::getBool(const std::string& key, bool defaultValue) const {
    return config_.getOr<bool>(key, defaultValue);
}

std::vector<std::string> TestConfig::getStringList(const std::string& key) const {
    auto value = config_.get<std::vector<std::string>>(key);
    return value ? *value : std::vector<std::string>();
}

void TestConfig::setParameter(const std::string& key, const std::string& value) {
    config_.set(key, value);
}

void TestConfig::setParameter(const std::string& key, int value) {
    config_.set(key, value);
}

void TestConfig::setParameter(const std::string& key, double value) {
    config_.set(key, value);
}

void TestConfig::setParameter(const std::string& key, bool value) {
    config_.set(key, value);
}

bool TestConfig::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Parse configuration from JSON
        if (jsonData.contains("test_config")) {
            auto testConfig = jsonData["test_config"];
            
            if (testConfig.contains("parameters")) {
                auto params = testConfig["parameters"];
                
                if (params.contains("output_directory")) {
                    parameters_.outputDirectory = params["output_directory"];
                }
                
                if (params.contains("output_format")) {
                    parameters_.outputFormat = params["output_format"];
                }
                
                if (params.contains("log_level")) {
                    parameters_.logLevel = params["log_level"];
                }
                
                if (params.contains("log_file")) {
                    parameters_.logFile = params["log_file"];
                }
                
                if (params.contains("verbose")) {
                    parameters_.verbose = params["verbose"];
                }
                
                if (params.contains("quiet")) {
                    parameters_.quiet = params["quiet"];
                }
                
                if (params.contains("progress_interval")) {
                    parameters_.progressInterval = params["progress_interval"];
                }
                
                if (params.contains("timestep")) {
                    parameters_.timestep = params["timestep"];
                }
                
                if (params.contains("steps_per_test")) {
                    parameters_.stepsPerTest = params["steps_per_test"];
                }
                
                if (params.contains("max_steps")) {
                    parameters_.maxSteps = params["max_steps"];
                }
            }
        }
        
        if (jsonData.contains("brain_config")) {
            auto brainConfig = jsonData["brain_config"];
            // Add brain configuration parsing
        }
        
        if (jsonData.contains("plugin_config")) {
            auto pluginConfig = jsonData["plugin_config"];
            // Add plugin configuration parsing
        }
        
    } catch (const std::exception& e) {
        return false;
    }
    
    return true;
}

bool TestConfig::loadFromTOML(const std::string& filepath) {
    // TODO: Implement TOML parsing
    return false;
}

bool TestConfig::loadFromINI(const std::string& filepath) {
    // TODO: Implement INI parsing
    return false;
}

std::map<std::string, std::function<void(TestConfig&, const std::string&)>> 
    TestConfig::initializeParamParsers() {
    std::map<std::string, std::function<void(TestConfig&, const std::string&)>> parsers;
    
    // Configuration parameter parsers
    parsers["output_directory"] = [](TestConfig& config, const std::string& value) {
        config.parameters_.outputDirectory = value;
    };
    
    parsers["output_format"] = [](TestConfig& config, const std::string& value) {
        config.parameters_.outputFormat = value;
    };
    
    parsers["log_level"] = [](TestConfig& config, const std::string& value) {
        config.parameters_.logLevel = value;
    };
    
    parsers["log_file"] = [](TestConfig& config, const std::string& value) {
        config.parameters_.logFile = value;
    };
    
    parsers["verbose"] = [](TestConfig& config, const std::string& value) {
        config.parameters_.verbose = (value == "true" || value == "1" || value == "yes");
    };
    
    parsers["quiet"] = [](TestConfig& config, const std::string& value) {
        config.parameters_.quiet = (value == "true" || value == "1" || value == "yes");
    };
    
    parsers["progress_interval"] = [](TestConfig& config, const std::string& value) {
        config.parameters_.progressInterval = std::stoi(value);
    };
    
    parsers["timestep"] = [](TestConfig& config, const std::string& value) {
        config.parameters_.timestep = std::stod(value);
    };
    
    parsers["steps_per_test"] = [](TestConfig& config, const std::string& value) {
        config.parameters_.stepsPerTest = std::stoi(value);
    };
    
    parsers["max_steps"] = [](TestConfig& config, const std::string& value) {
        config.parameters_.maxSteps = std::stoi(value);
    };
    
    return parsers;
}

} // namespace nlm
