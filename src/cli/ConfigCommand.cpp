#include "ConfigCommand.hpp"
#include <iomanip>

namespace nlm {
namespace cli {

Result ConfigCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Error: Config subcommand required" << std::endl;
        std::cout << getUsage() << std::endl;
        std::cout << "Available subcommands: show, load, save, validate, create, profile, templates" << std::endl;
        return Result::failure(1, "Missing config subcommand");
    }
    
    return Result::success("Config command initialized - implement subcommand execution");
}

Result ConfigCommand::executeWithConfig(const std::vector<std::string>& args, 
                                       const std::string& configFile) {
    if (args.empty()) {
        std::cout << "Error: Config subcommand required" << std::endl;
        std::cout << getUsage() << std::endl;
        std::cout << "Available subcommands: show, load, save, validate, create, profile, templates" << std::endl;
        return Result::failure(1, "Missing config subcommand");
    }
    
    // Parse subcommand
    std::string subcommand = args[0];
    ConfigSubcommand subcmd = parseSubcommand(subcommand);
    
    // Pass through to the appropriate subcommand handler
    switch (subcmd) {
        case ConfigSubcommand::Show:
            return executeShow(std::vector<std::string>(args.begin() + 1, args.end()), configFile);
        case ConfigSubcommand::Load:
            return executeLoad(std::vector<std::string>(args.begin() + 1, args.end()), configFile);
        case ConfigSubcommand::Save:
            return executeSave(std::vector<std::string>(args.begin() + 1, args.end()), configFile);
        case ConfigSubcommand::Validate:
            return executeValidate(std::vector<std::string>(args.begin() + 1, args.end()), configFile);
        case ConfigSubcommand::Create:
            return executeCreate(std::vector<std::string>(args.begin() + 1, args.end()), configFile);
        case ConfigSubcommand::Profile:
            return executeProfile(std::vector<std::string>(args.begin() + 1, args.end()), configFile);
        case ConfigSubcommand::Templates:
            return executeTemplates(std::vector<std::string>(args.begin() + 1, args.end()), configFile);
    }
    
    return Result::failure(1, "Unsupported config subcommand: " + subcommand);
}

ConfigCommand::ConfigSubcommand ConfigCommand::parseSubcommand(const std::string& name) const {
    std::string lowerName = utils::toLower(name);
    if (lowerName == "show") return ConfigSubcommand::Show;
    if (lowerName == "load") return ConfigSubcommand::Load;
    if (lowerName == "save") return ConfigSubcommand::Save;
    if (lowerName == "validate") return ConfigSubcommand::Validate;
    if (lowerName == "create") return ConfigSubcommand::Create;
    if (lowerName == "profile") return ConfigSubcommand::Profile;
    if (lowerName == "templates") return ConfigSubcommand::Templates;
    return ConfigSubcommand::Show; // Default
}

Result ConfigCommand::executeShow(const std::vector<std::string>& args, 
                                 const std::string& configFile) const {
    nlm::Config config;
    
    if (!configFile.empty()) {
        Result loadResult = loadConfigFile(configFile, config);
        if (!loadResult.isSuccess()) {
            return loadResult;
        }
    } else {
        std::cout << "No configuration file specified. Use --config to load a configuration." << std::endl;
        return Result::success("Empty configuration shown");
    }
    
    std::cout << "=== NLM Configuration ===" << std::endl;
    std::cout << formatConfigSummary(config) << std::endl;
    
    return Result::success("Configuration displayed successfully");
}

Result ConfigCommand::executeLoad(const std::vector<std::string>& args, 
                                 const std::string& configFile) const {
    if (args.empty()) {
        std::cout << "Error: Output file required for save command" << std::endl;
        return Result::failure(1, "Missing output file");
    }
    
    std::string inputFile = configFile.empty() ? args[0] : configFile;
    std::string outputFile = args.empty() ? inputFile + ".loaded" : args[0];
    
    nlm::Config config;
    Result loadResult = loadConfigFile(inputFile, config);
    if (!loadResult.isSuccess()) {
        return loadResult;
    }
    
    Result saveResult = saveConfigFile(outputFile, config);
    if (!saveResult.isSuccess()) {
        return saveResult;
    }
    
    std::cout << "Configuration loaded from " << inputFile << " and saved to " << outputFile << std::endl;
    return Result::success("Configuration loaded successfully");
}

Result ConfigCommand::executeSave(const std::vector<std::string>& args, 
                                 const std::string& configFile) const {
    if (args.empty()) {
        std::cout << "Error: Output file required for save command" << std::endl;
        return Result::failure(1, "Missing output file");
    }
    
    std::string outputFile = args[0];
    nlm::Config config;
    
    if (!configFile.empty()) {
        Result loadResult = loadConfigFile(configFile, config);
        if (!loadResult.isSuccess()) {
            return loadResult;
        }
    }
    
    Result saveResult = saveConfigFile(outputFile, config);
    if (!saveResult.isSuccess()) {
        return saveResult;
    }
    
    std::cout << "Configuration saved to " << outputFile << std::endl;
    return Result::success("Configuration saved successfully");
}

Result ConfigCommand::executeValidate(const std::vector<std::string>& args, 
                                     const std::string& configFile) const {
    nlm::Config config;
    
    if (!configFile.empty()) {
        Result loadResult = loadConfigFile(configFile, config);
        if (!loadResult.isSuccess()) {
            return loadResult;
        }
    } else {
        std::cout << "No configuration file specified. Use --config to load a configuration." << std::endl;
        return Result::failure(1, "No configuration file to validate");
    }
    
    Result validationResult = validateConfig(config);
    if (!validationResult.isSuccess()) {
        std::cout << "Configuration validation failed:" << std::endl;
        std::cout << validationResult.getMessage() << std::endl;
    } else {
        std::cout << "Configuration validation passed!" << std::endl;
    }
    
    return validationResult;
}

Result ConfigCommand::executeCreate(const std::vector<std::string>& args, 
                                   const std::string& configFile) const {
    if (args.empty()) {
        std::cout << "Error: Output file required for create command" << std::endl;
        return Result::failure(1, "Missing output file");
    }
    
    std::string outputFile = args[0];
    std::string profile = args.size() > 1 ? args[1] : "default";
    
    Result createResult = createDefaultConfig(outputFile, profile);
    if (!createResult.isSuccess()) {
        return createResult;
    }
    
    std::cout << "Configuration created at " << outputFile << " (profile: " << profile << ")" << std::endl;
    return Result::success("Configuration created successfully");
}

Result ConfigCommand::executeProfile(const std::vector<std::string>& args, 
                                    const std::string& configFile) const {
    nlm::Config config;
    
    if (!configFile.empty()) {
        Result loadResult = loadConfigFile(configFile, config);
        if (!loadResult.isSuccess()) {
            return loadResult;
        }
    } else {
        std::cout << "No configuration file specified. Use --config to load a configuration." << std::endl;
        return Result::failure(1, "No configuration file to profile");
    }
    
    // Display configuration profile
    std::cout << "=== Configuration Profile ===" << std::endl;
    std::cout << "Configuration summary:" << std::endl;
    std::cout << formatConfigSummary(config) << std::endl;
    
    std::cout << "\nConfiguration statistics:" << std::endl;
    std::cout << "  Number of entries: " << config.getKeys().size() << std::endl;
    std::cout << "  Sources: " << std::endl;
    for (const auto& key : config.getKeys()) {
        auto sourceOpt = config.get<int>(key);
        if (sourceOpt.has_value()) {
            std::cout << "    " << key << ": " << sourceOpt.value() << std::endl;
        }
    }
    
    return Result::success("Configuration profiled successfully");
}

Result ConfigCommand::executeTemplates(const std::vector<std::string>& args, 
                                      const std::string& configFile) const {
    std::cout << "=== Configuration Templates ===" << std::endl;
    std::cout << "Available configuration templates:" << std::endl;
    std::cout << std::endl;
    std::cout << "1. default - Basic NLM configuration for standard simulations" << std::endl;
    std::cout << "   - Neuron count: 1000" << std::endl;
    std::cout << "   - Region count: 1" << std::endl;
    std::cout << "   - Connection probability: 0.1" << std::endl;
    std::cout << std::endl;
    std::cout << "2. plasticity - Configuration optimized for plasticity experiments" << std::endl;
    std::cout << "   - Enhanced STDP and Hebbian learning" << std::endl;
    std::cout << "   - Higher synaptic plasticity rates" << std::endl;
    std::cout << "   - Increased memory capacity" << std::endl;
    std::cout << std::endl;
    std::cout << "3. performance - Configuration optimized for performance" << std::endl;
    std::cout << "   - Reduced neuron count for faster execution" << std::endl;
    std::cout << "   - Simplified connectivity" << std::endl;
    std::cout << "   - Minimal memory usage" << std::endl;
    std::cout << std::endl;
    std::cout << "4. development - Configuration for developmental studies" << std::endl;
    std::cout << "   - Age-dependent plasticity" << std::endl;
    std::cout << "   - Structural development" << std::endl;
    std::cout << "   - Critical period simulation" << std::endl;
    std::cout << std::endl;
    std::cout << "5. learning - Configuration for learning experiments" << std::endl;
    std::cout << "   - Reward-modulated plasticity" << std::endl;
    std::cout << "   - Curiosity-driven exploration" << std::endl;
    std::cout << "   - Novelty detection" << std::endl;
    std::endl;
    
    std::cout << "To create a configuration from a template, use:" << std::endl;
    std::cout << "  nlm config create <filename> <template_name>" << std::endl;
    std::cout << "  Available templates: default, plasticity, performance, development, learning" << std::endl;
    
    return Result::success("Configuration templates listed");
}

Result ConfigCommand::loadConfigFile(const std::string& filepath, 
                                   nlm::Config& config) const {
    std::ifstream file(filepath);
    if (!file.good()) {
        return Result::failure(1, "Failed to open configuration file: " + filepath);
    }
    
    try {
        bool loadSuccess = config.loadFromFile(filepath);
        if (!loadSuccess) {
            return Result::failure(1, "Failed to parse configuration file: " + filepath);
        }
    } catch (const std::exception& e) {
        return Result::failure(1, "Error loading configuration file " + filepath + ": " + std::string(e.what()));
    }
    
    return Result::success();
}

Result ConfigCommand::saveConfigFile(const std::string& filepath, 
                                   const nlm::Config& config) const {
    std::ofstream file(filepath);
    if (!file.good()) {
        return Result::failure(1, "Failed to create configuration file: " + filepath);
    }
    
    try {
        bool saveSuccess = config.saveToFile(filepath);
        if (!saveSuccess) {
            return Result::failure(1, "Failed to save configuration file: " + filepath);
        }
    } catch (const std::exception& e) {
        return Result::failure(1, "Error saving configuration file " + filepath + ": " + std::string(e.what()));
    }
    
    return Result::success();
}

Result ConfigCommand::validateConfig(const nlm::Config& config) const {
    std::vector<std::string> errors;
    
    // Check for required keys
    std::vector<std::string> requiredKeys = {"neuron_count", "simulation_timestep"};
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    // Validate values
    auto neuronCountOpt = config.get<size_t>("neuron_count");
    if (neuronCountOpt.has_value()) {
        size_t neuronCount = neuronCountOpt.value();
        if (neuronCount == 0) {
            errors.push_back("Invalid neuron_count: must be greater than 0");
        } else if (neuronCount > 100000) {
            errors.push_back("Warning: neuron_count is very large (" + std::to_string(neuronCount) + "). This may impact performance.");
        }
    }
    
    auto timestepOpt = config.get<double>("simulation_timestep");
    if (timestepOpt.has_value()) {
        double timestep = timestepOpt.value();
        if (timestep <= 0.0) {
            errors.push_back("Invalid simulation_timestep: must be greater than 0");
        } else if (timestep > 1.0) {
            errors.push_back("Warning: simulation_timestep is large (" + std::to_string(timestep) + "). Consider smaller timesteps for stability.");
        }
    }
    
    // Validate plasticity parameters
    auto stdpLtpOpt = config.get<float>("stdp_ltp_weight");
    if (stdpLtpOpt.has_value()) {
        float weight = stdpLtpOpt.value();
        if (weight < 0.0f || weight > 1.0f) {
            errors.push_back("Invalid stdp_ltp_weight: must be between 0.0 and 1.0");
        }
    }
    
    auto stdpLtdOpt = config.get<float>("stdp_ltd_weight");
    if (stdpLtdOpt.has_value()) {
        float weight = stdpLtdOpt.value();
        if (weight < 0.0f || weight > 1.0f) {
            errors.push_back("Invalid stdp_ltd_weight: must be between 0.0 and 1.0");
        }
    }
    
    if (!errors.empty()) {
        std::ostringstream oss;
        oss << "Configuration validation errors:" << std::endl;
        for (const auto& error : errors) {
            oss << "  - " << error << std::endl;
        }
        return Result::failure(1, oss.str());
    }
    
    return Result::success("Configuration validation passed");
}

Result ConfigCommand::createDefaultConfig(const std::string& filepath, 
                                        const std::string& profile) const {
    nlm::Config config;
    
    // Set default values based on profile
    if (profile == "plasticity") {
        config.set("neuron_count", 2000ULL, nlm::ConfigSource::Default);
        config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config.set("region_count", 1ULL, nlm::ConfigSource::Default);
        config.set("connection_probability", 0.2f, nlm::ConfigSource::Default);
        config.set("stdp_ltp_weight", 0.025f, nlm::ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.02f, nlm::ConfigSource::Default);
        config.set("stdp_tau", 20.0f, nlm::ConfigSource::Default);
        config.set("synaptogenesis_rate", 0.0002f, nlm::ConfigSource::Default);
        config.set("pruning_rate", 0.00002f, nlm::ConfigSource::Default);
        config.set("checkpoint_dir", "./checkpoints", nlm::ConfigSource::Default);
        config.set("replay_interval", 100ULL, nlm::ConfigSource::Default);
        config.set("consolidation_interval", 1000ULL, nlm::ConfigSource::Default);
        
    } else if (profile == "performance") {
        config.set("neuron_count", 500ULL, nlm::ConfigSource::Default);
        config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config.set("region_count", 1ULL, nlm::ConfigSource::Default);
        config.set("connection_probability", 0.05f, nlm::ConfigSource::Default);
        config.set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
        config.set("stdp_tau", 20.0f, nlm::ConfigSource::Default);
        config.set("synaptogenesis_rate", 0.00005f, nlm::ConfigSource::Default);
        config.set("pruning_rate", 0.000005f, nlm::ConfigSource::Default);
        config.set("checkpoint_dir", "./checkpoints", nlm::ConfigSource::Default);
        config.set("replay_interval", 50ULL, nlm::ConfigSource::Default);
        config.set("consolidation_interval", 500ULL, nlm::ConfigSource::Default);
        
    } else if (profile == "development") {
        config.set("neuron_count", 1000ULL, nlm::ConfigSource::Default);
        config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config.set("region_count", 1ULL, nlm::ConfigSource::Default);
        config.set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        config.set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
        config.set("stdp_tau", 20.0f, nlm::ConfigSource::Default);
        config.set("synaptogenesis_rate", 0.00015f, nlm::ConfigSource::Default);
        config.set("pruning_rate", 0.00001f, nlm::ConfigSource::Default);
        config.set("checkpoint_dir", "./checkpoints", nlm::ConfigSource::Default);
        config.set("replay_interval", 100ULL, nlm::ConfigSource::Default);
        config.set("consolidation_interval", 1000ULL, nlm::ConfigSource::Default);
        
    } else if (profile == "learning") {
        config.set("neuron_count", 1500ULL, nlm::ConfigSource::Default);
        config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config.set("region_count", 1ULL, nlm::ConfigSource::Default);
        config.set("connection_probability", 0.15f, nlm::ConfigSource::Default);
        config.set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.018f, nlm::ConfigSource::Default);
        config.set("stdp_tau", 20.0f, nlm::ConfigSource::Default);
        config.set("synaptogenesis_rate", 0.0001f, nlm::ConfigSource::Default);
        config.set("pruning_rate", 0.00001f, nlm::ConfigSource::Default);
        config.set("checkpoint_dir", "./checkpoints", nlm::ConfigSource::Default);
        config.set("replay_interval", 150ULL, nlm::ConfigSource::Default);
        config.set("consolidation_interval", 1500ULL, nlm::ConfigSource::Default);
        
    } else { // default profile
        config.set("neuron_count", 1000ULL, nlm::ConfigSource::Default);
        config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config.set("region_count", 1ULL, nlm::ConfigSource::Default);
        config.set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        config.set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.012f, nlm::ConfigSource::Default);
        config.set("stdp_tau", 20.0f, nlm::ConfigSource::Default);
        config.set("synaptogenesis_rate", 0.0001f, nlm::ConfigSource::Default);
        config.set("pruning_rate", 0.00001f, nlm::ConfigSource::Default);
        config.set("checkpoint_dir", "./checkpoints", nlm::ConfigSource::Default);
        config.set("replay_interval", 100ULL, nlm::ConfigSource::Default);
        config.set("consolidation_interval", 1000ULL, nlm::ConfigSource::Default);
    }
    
    Result saveResult = saveConfigFile(filepath, config);
    if (!saveResult.isSuccess()) {
        return saveResult;
    }
    
    return Result::success();
}

std::string ConfigCommand::formatConfigSummary(const nlm::Config& config) const {
    std::ostringstream oss;
    oss << "  Configuration entries: " << config.getKeys().size() << std::endl;
    for (const auto& key : config.getKeys()) {
        auto sourceOpt = config.get<std::string>(key);
        if (sourceOpt.has_value()) {
            oss << "    " << key << " = " << sourceOpt.value() << std::endl;
        }
    }
    return oss.str();
}

} // namespace cli
} // namespace nlm