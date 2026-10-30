namespace nlm {

void ConfigSchema::addField(const std::string& key, const std::type_index& type, 
                           const Validator& validator, const ConfigValue& defaultValue,
                           const std::string& description, bool required) {
    Field field;
    field.key = key;
    field.type = type;
    field.validator = validator;
    field.defaultValue = defaultValue;
    field.description = description;
    field.required = required;
    fields_[key] = field;
}

bool ConfigSchema::validate(const Config& config) const {
    bool isValid = true;
    
    for (const auto& [key, field] : fields_) {
        if (field.required && !config.has(key)) {
            isValid = false;
        } else if (config.has(key)) {
            auto value = config.get(field.type); // Note: get would need to be templated with type_index
            if (value.has_value()) {
                if (!field.validator(*value)) {
                    isValid = false;
                }
            }
        }
    }
    
    return isValid;
}

std::string ConfigSchema::getValidationErrors(const Config& config) const {
    std::ostringstream oss;
    bool firstError = true;
    
    for (const auto& [key, field] : fields_) {
        if (field.required && !config.has(key)) {
            if (!firstError) oss << "; ";
            oss << "Missing required field: " << key;
            firstError = false;
        } else if (config.has(key)) {
            auto value = config.get(field.type); // Note: get would need to be templated with type_index
            if (value.has_value()) {
                if (!field.validator(*value)) {
                    if (!firstError) oss << "; ";
                    oss << "Invalid value for field " << key << ": " << value;
                    firstError = false;
                }
            }
        }
    }
    
    return oss.str();
}

bool ConfigSchema::hasField(const std::string& key) const {
    return fields_.find(key) != fields_.end();
}

std::string ConfigSchema::getFieldDescription(const std::string& key) const {
    auto it = fields_.find(key);
    if (it != fields_.end()) {
        return it->second.description;
    }
    return "";
}

bool JsonConfigLoader::jsonAvailable_() = false;
std::string JsonConfigLoader::lastError_ = "";

JsonConfigLoader::JsonConfig JsonConfigLoader::loadFromJson(const std::string& filepath) {
    JsonConfig config;
    config.filename = filepath;
    config.valid = false;
    
    if (!isJsonAvailable()) {
        config.errorMessage = "JSON library not available";
        return config;
    }
    
    // TODO: Implement actual JSON parsing using a library like nlohmann::json
    // For now, return a placeholder error
    config.errorMessage = "JSON parsing not yet implemented";
    return config;
}

std::string JsonConfigLoader::getJsonError() const {
    return lastError_;
}

bool JsonConfigLoader::isJsonAvailable() const {
    return jsonAvailable_;
}

void ConfigPresets::applyPreset(Config& config, const std::string& presetName) {
    if (presetName == "phase2_basic") {
        Config phase2Config = createPhase2Config();
        for (const auto& key : phase2Config.getKeys()) {
            config.set(key, phase2Config.getOr(key, ConfigValue()));
        }
    } else if (presetName == "phase3_world") {
        Config phase3Config = createPhase3Config();
        for (const auto& key : phase3Config.getKeys()) {
            config.set(key, phase3Config.getOr(key, ConfigValue()));
        }
    } else if (presetName == "phase4_cognition") {
        Config phase4Config = createPhase4Config();
        for (const auto& key : phase4Config.getKeys()) {
            config.set(key, phase4Config.getOr(key, ConfigValue()));
        }
    } else if (presetName == "phase6_integrated") {
        Config phase6Config = createPhase6Config();
        for (const auto& key : phase6Config.getKeys()) {
            config.set(key, phase6Config.getOr(key, ConfigValue()));
        }
    }
}

std::vector<std::string> ConfigPresets::getAvailablePresets() {
    return {
        "phase2_basic",
        "phase3_world",
        "phase4_cognition",
        "phase6_integrated"
    };
}

Config ConfigPresets::createConfigForPhase(int phase) {
    switch (phase) {
        case 2: return createPhase2Config();
        case 3: return createPhase3Config();
        case 4: return createPhase4Config();
        case 6: return createPhase6Config();
        default: return Config();
    }
}

Config ConfigPresets::createConfigForExperiment(const std::string& experimentName) {
    if (experimentName == "connectivity") {
        Config config;
        config.set("neuron_count", int64_t(500));
        config.set("region_count", int64_t(1));
        config.set("connection_probability", 0.15f);
        config.set("simulation_timestep", 0.001);
        config.set("random_seed", int64_t(42));
        return config;
    } else if (experimentName == "plasticity") {
        Config config;
        config.set("neuron_count", int64_t(500));
        config.set("region_count", int64_t(1));
        config.set("connection_probability", 0.15f);
        config.set("simulation_timestep", 0.001);
        config.set("random_seed", int64_t(42));
        config.set("stdp_ltp_weight", 0.02f);
        config.set("stdp_ltd_weight", 0.015f);
        config.set("stdp_tau", 20.0f);
        config.set("synaptogenesis_rate", 0.0001f);
        config.set("pruning_rate", 0.00001f);
        return config;
    } else if (experimentName == "stdp") {
        Config config;
        config.set("neuron_count", int64_t(500));
        config.set("region_count", int64_t(1));
        config.set("connection_probability", 0.15f);
        config.set("simulation_timestep", 0.001);
        config.set("random_seed", int64_t(42));
        config.set("stdp_ltp_weight", 0.02f);
        config.set("stdp_ltd_weight", 0.015f);
        config.set("stdp_tau", 20.0f);
        config.set("synaptogenesis_rate", 0.0001f);
        config.set("pruning_rate", 0.00001f);
        return config;
    }
    return Config();
}

Config ConfigPresets::createPhase2Config() {
    Config config;
    config.set("neuron_count", int64_t(500));
    config.set("region_count", int64_t(1));
    config.set("connection_probability", 0.15f);
    config.set("simulation_timestep", 0.001);
    config.set("random_seed", int64_t(42));
    config.set("stdp_ltp_weight", 0.02f);
    config.set("stdp_ltd_weight", 0.015f);
    config.set("stdp_tau", 20.0f);
    config.set("synaptogenesis_rate", 0.0001f);
    config.set("pruning_rate", 0.00001f);
    return config;
}

Config ConfigPresets::createPhase3Config() {
    Config config;
    config.set("neuron_count", int64_t(500));
    config.set("region_count", int64_t(2));
    config.set("connection_probability", 0.1f);
    config.set("random_seed", int64_t(42));
    config.set("simulation_timestep", 0.001);
    config.set("stdp_ltp_weight", 0.01f);
    config.set("stdp_ltd_weight", 0.012f);
    config.set("stdp_tau", 20.0f);
    config.set("synaptogenesis_rate", 0.0001f);
    config.set("pruning_rate", 0.00001f);
    config.set("environment_width", 20);
    config.set("environment_height", 20);
    config.set("vision_width", 16);
    config.set("vision_height", 16);
    return config;
}

Config ConfigPresets::createPhase4Config() {
    Config config;
    config.set("neuron_count", int64_t(1000));
    config.set("region_count", int64_t(2));
    config.set("connection_probability", 0.1f);
    config.set("random_seed", int64_t(42));
    config.set("simulation_timestep", 0.001);
    config.set("stdp_ltp_weight", 0.01f);
    config.set("stdp_ltd_weight", 0.012f);
    config.set("stdp_tau", 20.0f);
    config.set("synaptogenesis_rate", 0.0001f);
    config.set("pruning_rate", 0.00001f);
    return config;
}

Config ConfigPresets::createPhase6Config() {
    Config config;
    config.set("neuron_count", int64_t(500));
    config.set("region_count", int64_t(1));
    config.set("connection_probability", 0.15f);
    config.set("simulation_timestep", 0.001);
    config.set("random_seed", int64_t(42));
    config.set("stdp_ltp_weight", 0.01f);
    config.set("stdp_ltd_weight", 0.012f);
    config.set("stdp_tau", 20.0f);
    config.set("synaptogenesis_rate", 0.0001f);
    config.set("pruning_rate", 0.00001f);
    config.set("max_steps", int64_t(2000));
    config.set("enable_checkpointing", true);
    config.set("enable_replay", true);
    config.set("enable_development", true);
    config.set("checkpoint_path", "/tmp/nlm_checkpoint.bin");
    return config;
}

EnhancedCommandLineParser::CommandLineOptions EnhancedCommandLineParser::parse(int argc, char** argv) {
    CommandLineOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version") {
            options.version = true;
        } else if (arg == "--benchmark") {
            options.benchmarkMode = true;
        } else if (arg == "--verbose") {
            options.verboseLogging = true;
        } else if (arg.find("--config=") == 0) {
            options.configFile = arg.substr(9);
        } else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        } else if (arg.find("--experiment=") == 0) {
            options.experimentName = arg.substr(14);
        } else if (arg.find("--episodes=") == 0) {
            options.numEpisodes = std::stoi(arg.substr(11));
        } else if (arg.find("--steps=") == 0) {
            options.stepsPerEpisode = std::stoi(arg.substr(7));
        } else if (arg.find("--output=") == 0) {
            options.outputFile = arg.substr(9);
        } else if (arg.find("--timestep=") == 0) {
            options.customTimestep = std::stod(arg.substr(11));
        } else if (arg.find("--neurons=") == 0) {
            options.customNeuronCount = std::stoll(arg.substr(9));
        } else if (arg == "--validate" && i + 1 < argc) {
            // Validation would check the experiment
            i++;
        }
    }
    
    return options;
}

std::string EnhancedCommandLineParser::getUsage() {
    return "";
    // TODO: Implement detailed usage string
}

std::string EnhancedCommandLineParser::getVersionInfo() {
    return "NLM Phase 6 (v0.1.0)";
}

bool EnhancedCommandLineParser::validateOptions(const CommandLineOptions& options) {
    std::string errors = getValidationErrors(options);
    return errors.empty();
}

std::string EnhancedCommandLineParser::getValidationErrors(const CommandLineOptions& options) {
    std::ostringstream oss;
    bool firstError = true;
    
    if (options.customNeuronCount > 0 && !ConfigConstants::isValidNeuronCount(options.customNeuronCount)) {
        if (!firstError) oss << "; ";
        oss << "Invalid neuron count: " << options.customNeuronCount;
        firstError = false;
    }
    
    if (options.customTimestep > 0 && !ConfigConstants::isValidTimestep(options.customTimestep)) {
        if (!firstError) oss << "; ";
        oss << "Invalid timestep: " << options.customTimestep;
        firstError = false;
    }
    
    if (options.numEpisodes < 1 || options.numEpisodes > ConfigConstants::MAX_EPISODES) {
        if (!firstError) oss << "; ";
        oss << "Invalid episode count: " << options.numEpisodes;
        firstError = false;
    }
    
    if (options.stepsPerEpisode < 1 || options.stepsPerEpisode > ConfigConstants::MAX_STEPS_PER_EPISODE) {
        if (!firstError) oss << "; ";
        oss << "Invalid steps per episode: " << options.stepsPerEpisode;
        firstError = false;
    }
    
    return oss.str();
}

void EnhancedCommandLineParser::printHelp() {
    std::cout << "";
    // TODO: Implement detailed help
}

std::string EnhancedCommandLineParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string EnhancedCommandLineParser::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

const std::string ExperimentConfigManager::CONFIG_DIR = "configs";
const std::string ExperimentConfigManager::EXPERIMENTS_DIR = "experiments";

ExperimentConfigManager::ExperimentConfig ExperimentConfigManager::loadExperimentConfig(const std::string& experimentName) {
    ExperimentConfig config;
    config.name = experimentName;
    config.config = std::make_shared<Config>();
    
    std::string configPath = CONFIG_DIR + "/" + experimentName + ".cfg";
    std::ifstream file(configPath);
    
    if (file.is_open()) {
        // Parse config file
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                config.config->set(key, value, ConfigSource::File);
            }
        }
        
        config.validated = true;
    }
    
    return config;
}

bool ExperimentConfigManager::saveExperimentConfig(const std::string& experimentName, const Config& config) {
    std::string configPath = CONFIG_DIR + "/" + experimentName + ".cfg";
    std::ofstream file(configPath);
    
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& key : config.getKeys()) {
        file << "# " << config.getOr(key, std::string("")) << "\n";
        file << key << " = " << config.getOr(key, std::string("")) << "\n";
    }
    
    return true;
}

std::vector<std::string> ExperimentConfigManager::getAvailableExperiments() {
    std::vector<std::string> experiments;
    
    std::string experimentsPath = EXPERIMENTS_DIR;
    std::filesystem::path dir(experimentsPath);
    
    if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file() && 
                (entry.path().extension() == ".cpp" || entry.path().extension() == ".cfg")) {
                std::string filename = entry.path().stem().string();
                if (filename.find("demo") == std::string::npos && filename.find("experiment") != std::string::npos) {
                    experiments.push_back(filename);
                }
            }
        }
    }
    
    return experiments;
}

bool ExperimentConfigManager::validateConfig(const Config& config, const std::string& experimentName) {
    ConfigSchema schema;
    // Add schema validation fields based on experiment
    return schema.validate(config);
}

void ExperimentConfigManager::migrateConfig(const Config& oldConfig, Config& newConfig, const std::string& targetVersion) {
    // TODO: Implement configuration migration logic
}

bool ConfigConstants::isValidNeuronCount(int64_t count) {
    return count >= MIN_NEURONS && count <= MAX_NEURONS;
}

bool ConfigConstants::isValidTimestep(double timestep) {
    return timestep >= MIN_TIMESTEP && timestep <= MAX_TIMESTEP;
}

bool ConfigConstants::isValidConnectionProbability(float prob) {
    return prob >= MIN_CONNECTION_PROBABILITY && prob <= MAX_CONNECTION_PROBABILITY;
}

std::string ConfigConstants::getDefaultConfigFile(const std::string& experimentName) {
    return CONFIG_DIR + "/default_" + experimentName + ".cfg";
}

std::string ConfigConstants::getConfigBackupPath(const std::string& experimentName) {
    return "/tmp/nlm_backup_" + experimentName + ".bin";
}

} // namespace nlm