// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// Enhanced Argument Parser Implementation
//
// This implements comprehensive command-line argument parsing with support for:
// - Configuration file loading (--config, --json-config)
// - Configuration overrides (--config-value, --config-object)
// - Configuration presets (research, performance, education, debug, production)
// - Configuration validation and export
// - Advanced argument handling

#include "core/ArgumentParser.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <fstream>
#include <iomanip>

using namespace nlm;

// ArgumentParser implementation
ArgumentParser::ArgumentParser() : verbose_(false), quiet_(false) {
    initializeBuiltInOptions();
}

void ArgumentParser::initializeBuiltInOptions() {
    // Core arguments
    addOption("--help", "-h", Option::Type::FLAG, "Show help information and exit",
              "Shows the complete list of available options with descriptions",
              "None", "Usage", false);
    
    addOption("--version", Option::Type::FLAG, "Show version information and exit",
              "Displays the NLM version and build information",
              "None", "General", false);
    
    addOption("--verbose", "-v", Option::Type::FLAG, "Enable verbose logging",
              "Enables detailed logging output for debugging purposes",
              "None", "Logging", false);
    
    addOption("--quiet", "-q", Option::Type::FLAG, "Suppress all output",
              "Runs NLM with minimal output for non-interactive environments",
              "None", "Logging", false);
    
    // Configuration arguments
    addOption("--config", "-c", Option::Type::VALUE, "Configuration file path",
              "Load configuration from a traditional key=value format file",
              "configs/default.cfg", "Configuration", false);
    
    addOption("--json-config", "-j", Option::Type::VALUE, "JSON configuration file path",
              "Load configuration from a JSON file (new enhanced format)",
              "configs/default.json", "Configuration", false);
    
    addOption("--config-value", "-C", Option::Type::KEY_VALUE, "Key=Value override",
              "Override a single configuration value",
              "key=value", "Configuration", false);
    
    addOption("--config-object", "-O", Option::Type::KEY_OBJECT, "Key=Object override",
              "Override a nested configuration object",
              "key={nested:value}", "Configuration", false);
    
    addOption("--list-configs", Option::Type::FLAG, "List available configuration presets",
              "Displays all available configuration presets and their descriptions",
              "None", "Configuration", false);
    
    addOption("--show-config", Option::Type::FLAG, "Show current configuration",
              "Displays the current configuration values after parsing all inputs",
              "None", "Configuration", false);
    
    addOption("--validate-config", Option::Type::FLAG, "Validate configuration against schema",
              "Validates the current configuration against the expected schema",
              "None", "Configuration", false);
    
    addOption("--export-config", Option::Type::VALUE, "Export configuration to file",
              "Exports the current configuration to the specified file",
              "exported_config.cfg", "Configuration", false);
    
    addOption("--merge-configs", Option::Type::VALUES, "Merge multiple configuration files",
              "Merges multiple configuration files in the specified order",
              "config1.cfg,config2.cfg", "Configuration", false);
    
    addOption("--dry-run", Option::Type::FLAG, "Validate configuration without simulation",
              "Validates all configuration and exits without running the simulation",
              "None", "Simulation", false);
    
    // Preset options
    addOption("--preset", Option::Type::VALUE, "Configuration preset",
              "Apply a predefined configuration preset",
              "research", "Configuration", false);
    
    // Advanced options
    addOption("--log-level", Option::Type::VALUE, "Set log level",
              "Sets the minimum log level (DEBUG, INFO, WARN, ERROR, FATAL)",
              "INFO", "Logging", true);
    
    addOption("--simulation-steps", Option::Type::VALUE, "Set simulation steps",
              "Sets the maximum number of simulation steps",
              "10000", "Simulation", true);
}

void ArgumentParser::addOption(const std::string& longOpt, const std::string& shortOpt,
                              const Option& opt) {
    options_[longOpt] = opt;
    if (!shortOpt.empty()) {
        shortOptions_[shortOpt] = longOpt;
    }
}

void ArgumentParser::addOption(const std::string& longOpt, const std::string& shortOpt,
                              Option::Type type, const std::string& description,
                              const std::string& defaultValue, const std::string& category,
                              bool advanced) {
    Option opt;
    opt.type = type;
    opt.longOpt = longOpt;
    opt.shortOpt = shortOpt;
    opt.description = description;
    opt.defaultValue = defaultValue;
    opt.category = category;
    opt.advanced = advanced;
    
    // Set example based on type
    switch (type) {
        case Option::Type::FLAG:
            opt.example = "";
            break;
        case Option::Type::VALUE:
            opt.example = defaultValue;
            break;
        case Option::Type::KEY_VALUE:
            opt.example = "key=value";
            break;
        case Option::Type::KEY_OBJECT:
            opt.example = "key={nested:value}";
            break;
        case Option::Type::VALUES:
            opt.example = "file1.cfg,file2.cfg";
            break;
    }
    
    addOption(longOpt, shortOpt, opt);
}

bool ArgumentParser::parse(int argc, char** argv, Config& config) {
    // Initialize parsed options
    parsedOptions_.clear();
    parsedValues_.clear();
    parsedKeyValues_.clear();
    parsedKeyObjects_.clear();
    parsedValuesList_.clear();
    
    // Store original values before parsing
    originalValues_ = parsedOptions_;
    
    // Process all arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (!parseArgument(arg, i, argc, argv, config)) {
            return false;
        }
    }
    
    // Apply presets if specified
    if (parsedOptions_.count("--preset")) {
        if (!applyPreset(config)) {
            return false;
        }
    }
    
    // Load configuration files in order
    if (!loadConfigurationFiles(config)) {
        return false;
    }
    
    // Apply overrides
    if (!applyOverrides(config)) {
        return false;
    }
    
    // Validate configuration
    if (parsedOptions_.count("--validate-config")) {
        if (!config.validateAdvancedOptions(config)) {
            return false;
        }
    }
    
    return true;
}

bool ArgumentParser::parseArgument(const std::string& arg, int& i, int argc, char** argv, Config& config) {
    // Check for long option
    if (arg.substr(0, 2) == "--") {
        std::string optName = arg.substr(2);
        
        if (optName == "help" || optName == "h") {
            parsedOptions_["--help"] = true;
            return true;
        } else if (optName == "version") {
            parsedOptions_["--version"] = true;
            return true;
        } else if (optName == "verbose" || optName == "v") {
            parsedOptions_["--verbose"] = true;
            return true;
        } else if (optName == "quiet" || optName == "q") {
            parsedOptions_["--quiet"] = true;
            return true;
        } else if (optName == "list-configs") {
            parsedOptions_["--list-configs"] = true;
            return true;
        } else if (optName == "show-config") {
            parsedOptions_["--show-config"] = true;
            return true;
        } else if (optName == "validate-config") {
            parsedOptions_["--validate-config"] = true;
            return true;
        } else if (optName == "dry-run") {
            parsedOptions_["--dry-run"] = true;
            return true;
        } else if (optName == "log-level") {
            if (i + 1 < argc) {
                parsedOptions_["--log-level"] = true;
                parsedValues_["--log-level"] = argv[++i];
            }
            return true;
        } else if (optName == "simulation-steps") {
            if (i + 1 < argc) {
                parsedOptions_["--simulation-steps"] = true;
                parsedValues_["--simulation-steps"] = argv[++i];
            }
            return true;
        } else if (optName == "preset") {
            if (i + 1 < argc) {
                parsedOptions_["--preset"] = true;
                parsedValues_["--preset"] = argv[++i];
            }
            return true;
        } else if (optName == "export-config") {
            parsedOptions_["--export-config"] = true;
            if (arg.find("=") != std::string::npos) {
                parsedValues_["--export-config"] = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                parsedValues_["--export-config"] = argv[++i];
            }
            return true;
        } else if (optName == "config") {
            parsedOptions_["--config"] = true;
            if (arg.find("=") != std::string::npos) {
                parsedValues_["--config"] = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                parsedValues_["--config"] = argv[++i];
            }
            return true;
        } else if (optName == "json-config") {
            parsedOptions_["--json-config"] = true;
            if (arg.find("=") != std::string::npos) {
                parsedValues_["--json-config"] = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                parsedValues_["--json-config"] = argv[++i];
            }
            return true;
        } else if (optName == "config-value" || optName == "C") {
            parsedOptions_["--config-value"] = true;
            if (arg.find("=") != std::string::npos) {
                std::string keyValue = arg.substr(arg.find("=") + 1);
                size_t pos = keyValue.find("=");
                if (pos != std::string::npos) {
                    parsedKeyValues_[keyValue.substr(0, pos)] = keyValue.substr(pos + 1);
                }
            }
            return true;
        } else if (optName == "config-object" || optName == "O") {
            parsedOptions_["--config-object"] = true;
            if (arg.find("=") != std::string::npos) {
                std::string keyObject = arg.substr(arg.find("=") + 1);
                size_t pos = keyObject.find("=");
                if (pos != std::string::npos) {
                    parsedKeyObjects_[keyObject.substr(0, pos)] = keyObject.substr(pos + 1);
                }
            }
            return true;
        } else if (optName == "merge-configs") {
            parsedOptions_["--merge-configs"] = true;
            if (arg.find("=") != std::string::npos) {
                parsedValuesList_["--merge-configs"].push_back(arg.substr(arg.find("=") + 1));
            }
            return true;
        }
    } 
    // Check for short option
    else if (arg[0] == '-') {
        std::string optName = shortOptions_[arg.substr(1)];
        if (!optName.empty()) {
            // Handle short option like -abc
            for (char c : arg.substr(1)) {
                std::string shortOpt(1, c);
                if (shortOpt == "h") {
                    parsedOptions_["--help"] = true;
                } else if (shortOpt == "v") {
                    parsedOptions_["--verbose"] = true;
                } else if (shortOpt == "q") {
                    parsedOptions_["--quiet"] = true;
                } else if (shortOpt == "c") {
                    parsedOptions_["--config"] = true;
                    if (i + 1 < argc) {
                        parsedValues_["--config"] = argv[++i];
                    }
                } else if (shortOpt == "j") {
                    parsedOptions_["--json-config"] = true;
                    if (i + 1 < argc) {
                        parsedValues_["--json-config"] = argv[++i];
                    }
                } else if (shortOpt == "C") {
                    parsedOptions_["--config-value"] = true;
                    if (i + 1 < argc) {
                        std::string keyValue = argv[++i];
                        size_t pos = keyValue.find("=");
                        if (pos != std::string::npos) {
                            parsedKeyValues_[keyValue.substr(0, pos)] = keyValue.substr(pos + 1);
                        }
                    }
                } else if (shortOpt == "O") {
                    parsedOptions_["--config-object"] = true;
                    if (i + 1 < argc) {
                        std::string keyObject = argv[++i];
                        size_t pos = keyObject.find("=");
                        if (pos != std::string::npos) {
                            parsedKeyObjects_[keyObject.substr(0, pos)] = keyObject.substr(pos + 1);
                        }
                    }
                }
            }
            return true;
        }
    }
    
    // Unknown argument
    return false;
}

void ArgumentParser::displayHelp() const {
    std::cout << "=== NLM Argument Parser Help ===" << std::endl << std::endl;
    
    std::cout << "Core Arguments:" << std::endl;
    std::cout << "  --help, -h                    Show this help information" << std::endl;
    std::cout << "  --version                     Show version information" << std::endl;
    std::cout << "  --verbose, -v                 Enable verbose logging" << std::endl;
    std::cout << "  --quiet, -q                   Suppress all output" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration Arguments:" << std::endl;
    std::cout << "  --config, -c <file>           Load traditional configuration file" << std::endl;
    std::cout << "  --json-config, -j <file>      Load JSON configuration file" << std::endl;
    std::cout << "  --config-value, -C <key=val>  Override single configuration value" << std::endl;
    std::cout << "  --config-object, -O <key=val> Override nested configuration object" << std::endl;
    std::cout << "  --list-configs                List available configuration presets" << std::endl;
    std::cout << "  --show-config                 Show current configuration" << std::endl;
    std::cout << "  --validate-config              Validate configuration against schema" << std::endl;
    std::cout << "  --export-config <file>         Export configuration to file" << std::endl;
    std::cout << "  --merge-configs <files>       Merge multiple configuration files" << std::endl;
    std::cout << "  --dry-run                     Validate configuration without running" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Preset Configuration Arguments:" << std::endl;
    std::cout << "  --preset <name>               Apply a configuration preset" << std::endl;
    std::cout << "      Available presets:" << std::endl;
    std::cout << "      - research: Detailed logging, validation enabled" << std::endl;
    std::cout << "      - performance: Optimized for speed, reduced logging" << std::endl;
    std::cout << "      - education: Simplified configuration, verbose output" << std::endl;
    std::cout << "      - debug: Maximum verbosity, all debugging enabled" << std::endl;
    std::cout << "      - production: Minimal output, validation enabled" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Advanced Arguments:" << std::endl;
    std::cout << "  --log-level <level>            Set log level (DEBUG, INFO, WARN, ERROR, FATAL)" << std::endl;
    std::cout << "  --simulation-steps <steps>     Set maximum simulation steps" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm --config myconfig.cfg --verbose" << std::endl;
    std::cout << "  nlm --json-config config.json --preset research" << std::endl;
    std::cout << "  nlm --config-value neuron_count=2000 --config-object brain.region1=0.1" << std::endl;
    std::cout << "  nlm --preset debug --show-config" << std::endl;
    std::cout << "  nlm --validate-config --dry-run" << std::endl;
    std::cout << "  nlm --list-configs" << std::endl;
    std::cout << std::endl;
}

void ArgumentParser::displayVersion() const {
    std::cout << "NLM Version 0.1.0" << std::endl;
    std::cout << "Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Built with enhanced argument parsing support" << std::endl;
}

bool ArgumentParser::applyPreset(const Config& config) {
    if (!parsedOptions_.count("--preset")) {
        return true;
    }
    
    std::string presetName = parsedValues_["--preset"];
    nlm::ConfigPreset preset = Config::stringToPreset(presetName);
    
    if (preset == nlm::ConfigPreset::CUSTOM) {
        std::cout << "Custom preset applied" << std::endl;
        return true;
    }
    
    std::cout << "Applying preset: " << presetName << std::endl;
    return config.applyPreset(preset);
}

bool ArgumentParser::loadConfigurationFiles(Config& config) {
    std::vector<std::string> configFiles;
    
    // Add JSON config if specified
    if (parsedOptions_.count("--json-config") && parsedValues_.count("--json-config")) {
        configFiles.push_back(parsedValues_["--json-config"]);
    }
    
    // Add traditional config if specified
    if (parsedOptions_.count("--config") && parsedValues_.count("--config")) {
        configFiles.push_back(parsedValues_["--config"]);
    }
    
    // Add merged configs if specified
    if (parsedOptions_.count("--merge-configs") && !parsedValuesList_["--merge-configs"].empty()) {
        for (const auto& file : parsedValuesList_["--merge-configs"]) {
            configFiles.push_back(file);
        }
    }
    
    // Load files in order
    for (const auto& file : configFiles) {
        if (config.loadFromFile(file)) {
            std::cout << "Loaded configuration from: " << file << std::endl;
        } else {
            std::cout << "Warning: Failed to load configuration from: " << file << std::endl;
        }
    }
    
    return true;
}

bool ArgumentParser::applyOverrides(Config& config) {
    // Apply key-value overrides
    for (const auto& [key, value] : parsedKeyValues_) {
        config.set(key, value, ConfigSource::CommandLine);
        std::cout << "Applied override: " << key << " = " << value << std::endl;
    }
    
    // Apply key-object overrides (for nested structures)
    for (const auto& [key, jsonStr] : parsedKeyObjects_) {
        // Parse JSON string and apply as object
        try {
            nlohmann::json json = nlohmann::json::parse(jsonStr);
            // Convert JSON to ConfigValue and set
            ConfigValue configValue;
            if (config.jsonToConfigValue(json, configValue)) {
                config.set(key, configValue, ConfigSource::CommandLine);
                std::cout << "Applied object override: " << key << " = " << jsonStr << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Warning: Failed to parse object override for key " << key << ": " << e.what() << std::endl;
        }
    }
    
    // Apply log level if specified
    if (parsedOptions_.count("--log-level") && parsedValues_.count("--log-level")) {
        config.set("log_level", parsedValues_["--log-level"], ConfigSource::CommandLine);
    }
    
    // Apply simulation steps if specified
    if (parsedOptions_.count("--simulation-steps") && parsedValues_.count("--simulation-steps")) {
        try {
            int steps = std::stoi(parsedValues_["--simulation-steps"]);
            config.set("max_simulation_steps", steps, ConfigSource::CommandLine);
        } catch (const std::exception& e) {
            std::cout << "Warning: Invalid simulation steps value: " << parsedValues_["--simulation-steps"] << std::endl;
        }
    }
    
    return true;
}

void ArgumentParser::setVerbose(bool verbose) {
    verbose_ = verbose;
}

void ArgumentParser::setQuiet(bool quiet) {
    quiet_ = quiet;
}

bool ArgumentParser::isVerbose() const {
    return verbose_;
}

bool ArgumentParser::isQuiet() const {
    return quiet_;
}
