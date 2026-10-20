#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSchema> schemas;
    bool validationEnabled = true;
    mutable bool hasErrors = false;
    mutable std::vector<std::string> errorMessages;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultSchemas();
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {
    other.pImpl = nullptr;
}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        other.pImpl = nullptr;
    }
    return *this;
}

void Config::initializeDefaultSchemas() {
    addSchema(ConfigSchema("random_seed", "Random seed for reproducible experiments", true));
    addSchema(ConfigSchema("simulation_timestep", "Simulation timestep in seconds", true));
    addSchema(ConfigSchema("neuron_count", "Number of neurons in the brain", true));
    addSchema(ConfigSchema("region_count", "Number of brain regions", true));
    addSchema(ConfigSchema("connection_probability", "Connection probability for random connectivity", false));
    addSchema(ConfigSchema("plasticity_learning_rate", "Plasticity learning rate", false));
    addSchema(ConfigSchema("stdp_ltp_weight", "STDP LTP weight", false));
    addSchema(ConfigSchema("stdp_ltd_weight", "STDP LTD weight", false));
    addSchema(ConfigSchema("stdp_time_constant", "STDP time constant", false));
    addSchema(ConfigSchema("development_synaptogenesis_rate", "Development synaptogenesis rate", false));
    addSchema(ConfigSchema("development_pruning_rate", "Development pruning rate", false));
    addSchema(ConfigSchema("dopamine_baseline", "Dopamine baseline level", false));
    addSchema(ConfigSchema("neuropeptide_baseline", "Neuropeptide baseline level", false));
    addSchema(ConfigSchema("reward_discount_factor", "Reward discount factor", false));
    addSchema(ConfigSchema("environment_name", "Environment name", false));
    addSchema(ConfigSchema("environment_width", "Environment width", false));
    addSchema(ConfigSchema("environment_height", "Environment height", false));
    addSchema(ConfigSchema("log_level", "Log level (DEBUG, INFO, WARNING, ERROR)", false));
    addSchema(ConfigSchema("log_to_file", "Enable file logging", false));
    addSchema(ConfigSchema("log_filename", "Log file name", false));
    addSchema(ConfigSchema("max_simulation_steps", "Maximum simulation steps", false));
    addSchema(ConfigSchema("simulation_time_limit", "Simulation time limit", false));
    addSchema(ConfigSchema("visualization_enabled", "Enable visualization", false));
    addSchema(ConfigSchema("visualization_update_rate", "Visualization update rate", false));

    // Advanced schemas
    addSchema(ConfigSchema("plasticity_hebbian_learning_rate", "Hebbian learning rate", false));
    addSchema(ConfigSchema("plasticity_stdp_ltp_threshold", "STDP LTP threshold", false));
    addSchema(ConfigSchema("plasticity_stdp_ltd_threshold", "STDP LTD threshold", false));
    addSchema(ConfigSchema("plasticity_structural_max_synapses", "Maximum synapses for structural plasticity", false));
    addSchema(ConfigSchema("plasticity_structural_growth_probability", "Structural plasticity growth probability", false));
    addSchema(ConfigSchema("plasticity_structural_pruning_probability", "Structural plasticity pruning probability", false));
    addSchema(ConfigSchema("development_initial_stage", "Initial developmental stage", false));
    addSchema(ConfigSchema("development_critical_period_duration", "Critical period duration", false));
    addSchema(ConfigSchema("development_maturation_rate", "Development maturation rate", false));
    addSchema(ConfigSchema("development_aging_start_step", "Aging start step", false));
    addSchema(ConfigSchema("development_plasticity_decay_rate", "Development plasticity decay rate", false));
    addSchema(ConfigSchema("neuromod_curiosity_sensitivity", "Curiosity neuromodulation sensitivity", false));
    addSchema(ConfigSchema("neuromod_novelty_sensitivity", "Novelty neuromodulation sensitivity", false));
    addSchema(ConfigSchema("neuromod_reward_prediction_error_decay", "Reward prediction error decay", false));
    addSchema(ConfigSchema("neuromod_attentional_focus_decay", "Attentional focus decay", false));
    addSchema(ConfigSchema("neuromod_learning_modulation_strength", "Learning modulation strength", false));
    addSchema(ConfigSchema("memory_working_memory_capacity", "Working memory capacity", false));
    addSchema(ConfigSchema("memory_episodic_max_episodes", "Maximum episodic memory episodes", false));
    addSchema(ConfigSchema("memory_episodic_decay_rate", "Episodic memory decay rate", false));
    addSchema(ConfigSchema("memory_associative_pattern_threshold", "Associative memory pattern threshold", false));
    addSchema(ConfigSchema("memory_associative_forgetting_rate", "Associative memory forgetting rate", false));
    addSchema(ConfigSchema("prediction_prediction_horizon", "Prediction horizon", false));
    addSchema(ConfigSchema("prediction_prediction_error_threshold", "Prediction error threshold", false));
    addSchema(ConfigSchema("prediction_prediction_learning_rate", "Prediction learning rate", false));
    addSchema(ConfigSchema("prediction_confidence_threshold", "Prediction confidence threshold", false));
    addSchema(ConfigSchema("cognition_planner_action_horizon", "Planner action horizon", false));
    addSchema(ConfigSchema("cognition_concept_learning_rate", "Concept learning rate", false));
    addSchema(ConfigSchema("cognition_attention_selection_strength", "Attention selection strength", false));
    addSchema(ConfigSchema("cognition_planning_time_budget", "Planning time budget", false));
    addSchema(ConfigSchema("performance_enable_simd", "Enable SIMD optimization", false));
    addSchema(ConfigSchema("performance_enable_memory_pool", "Enable memory pool", false));
    addSchema(ConfigSchema("performance_enable_event_queue", "Enable event queue", false));
    addSchema(ConfigSchema("performance_thread_pool_size", "Thread pool size", false));
    addSchema(ConfigSchema("performance_max_queue_depth", "Maximum queue depth", false));
    addSchema(ConfigSchema("performance_optimization_level", "Optimization level", false));
    addSchema(ConfigSchema("simulation_enable_checkpoints", "Enable checkpointing", false));
    addSchema(ConfigSchema("checkpoint_interval_steps", "Checkpoint interval in steps", false));
    addSchema(ConfigSchema("checkpoint_max_saves", "Maximum checkpoint saves", false));
    addSchema(ConfigSchema("simulation_steady_state_detection", "Enable steady state detection", false));
    addSchema(ConfigSchema("steady_state_threshold", "Steady state threshold", false));
    addSchema(ConfigSchema("state_enable_serialization", "Enable serialization", false));
    addSchema(ConfigSchema("state_compression_enabled", "Enable compression", false));
    addSchema(ConfigSchema("state_max_memory_usage", "Maximum memory usage", false));
    addSchema(ConfigSchema("state_save_format", "Save format (binary, text)", false));
    addSchema(ConfigSchema("debug_enable_breakpoints", "Enable debug breakpoints", false));
    addSchema(ConfigSchema("debug_log_synaptic_events", "Log synaptic events", false));
    addSchema(ConfigSchema("debug_log_neuron_events", "Log neuron events", false));
    addSchema(ConfigSchema("test_performance_benchmarks", "Run performance benchmarks", false));
    addSchema(ConfigSchema("test_random_seed_validation", "Validate random seeds", false));
    addSchema(ConfigSchema("experimental_enable_replay", "Enable experimental replay feature", false));
    addSchema(ConfigSchema("experimental_replay_decay_rate", "Replay decay rate", false));
    addSchema(ConfigSchema("experimental_enable_meta_learning", "Enable meta-learning", false));
    addSchema(ConfigSchema("experimental_meta_learning_rate", "Meta-learning rate", false));
}

bool Config::loadFromFile(const std::string& filepath, bool validateSchema) {
    std::error_code ec;
    if (!std::filesystem::exists(filepath, ec) || ec) {
        errorMessages.push_back("Config file error: Cannot access file: " + filepath);
        hasErrors = true;
        return false;
    }
    
    if (!std::filesystem::is_regular_file(filepath, ec) || ec) {
        errorMessages.push_back("Config file error: Path is not a file: " + filepath);
        hasErrors = true;
        return false;
    }
    
    if (!isFilePathSafe(filepath)) {
        errorMessages.push_back("Config file error: File path is not safe: " + filepath);
        hasErrors = true;
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        errorMessages.push_back("Config file error: Cannot open file: " + filepath);
        hasErrors = true;
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            errorMessages.push_back("Config file warning: Invalid format on line " + 
                                 std::to_string(lineNumber) + ": " + line);
            continue;
        }
        
        try {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            if (!isValidKey(key)) {
                errorMessages.push_back("Config file error: Invalid key name on line " + 
                                     std::to_string(lineNumber) + ": " + key);
                hasErrors = true;
                continue;
            }
            
            set(key, value, ConfigSource::File, validateSchema && !key.empty());
            
        } catch (const std::exception& e) {
            errorMessages.push_back("Config file error on line " + std::to_string(lineNumber) + ": " + 
                                 std::string(e.what()) + ": " + line);
            hasErrors = true;
        }
    }
    
    if (validateSchema) {
        validateAllEntries();
    }
    
    return !hasErrors;
}

bool Config::loadFromArgs(int argc, char** argv, bool validateArgs) {
    if (validateArgs) {
        if (!validateCommandLineArgs(argc, argv)) {
            return false;
        }
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool includeComments) const {
    if (!validateFilePath(filepath)) {
        errorMessages.push_back("Config save error: Invalid file path: " + filepath);
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        errorMessages.push_back("Config save error: Cannot open file for writing: " + filepath);
        return false;
    }
    
    file.precision(6);
    file << std::fixed;
    
    for (const auto& entry : pImpl->entries) {
        if (includeComments && !entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key, bool validateType) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        T value = std::get<T>(it->value);
        if (validateType) {
            auto schemaIt = pImpl->schemas.find(key);
            if (schemaIt != pImpl->schemas.end()) {
                ConfigSchema& schema = schemaIt->second;
                if (!valueInRange(value, schema.minValue, schema.maxValue)) {
                    errorMessages.push_back("Config validation error: Value for key '" + key + 
                                         "' is out of range (" + std::to_string(schema.minValue) + 
                                         ", " + std::to_string(schema.maxValue) + ")");
                    hasErrors = true;
                }
                if (!valueInAllowedValues(value, schema.allowedValues)) {
                    errorMessages.push_back("Config validation error: Value for key '" + key + 
                                         "' is not in allowed values");
                    hasErrors = true;
                }
            }
        }
        return value;
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue, bool validateType) const {
    auto val = get<T>(key, validateType);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source, bool validateValue) {
    if (!isValidKey(key)) {
        errorMessages.push_back("Config error: Invalid key name: " + key);
        hasErrors = true;
        return;
    }
    
    if (validateValue) {
        auto schemaIt = pImpl->schemas.find(key);
        if (schemaIt != pImpl->schemas.end()) {
            ConfigSchema& schema = schemaIt->second;
            
            if (!valueMatchesType(value, schema.valueType)) {
                errorMessages.push_back("Config validation error: Value for key '" + key + 
                                     "' has incorrect type (expected " + schema.valueType + ")");
                hasErrors = true;
            }
            
            if (valueMatchesType(value, "double") || valueMatchesType(value, "int") || 
                valueMatchesType(value, "int64_t")) {
                double valDouble = 0.0;
                if (auto* v = std::get_if<double>(&value)) valDouble = *v;
                else if (auto* v = std::get_if<int>(&value)) valDouble = *v;
                else if (auto* v = std::get_if<int64_t>(&value)) valDouble = *v;
                
                if (valDouble < schema.minValue || valDouble > schema.maxValue) {
                    errorMessages.push_back("Config validation error: Value for key '" + key + 
                                         "' is out of range (" + std::to_string(schema.minValue) + 
                                         ", " + std::to_string(schema.maxValue) + ")");
                    hasErrors = true;
                }
            }
            
            if (!schema.allowedValues.empty()) {
                bool valueAllowed = false;
                if (auto* v = std::get_if<std::string>(&value)) {
                    for (const auto& allowed : schema.allowedValues) {
                        if (*v == allowed) {
                            valueAllowed = true;
                            break;
                        }
                    }
                }
                if (!valueAllowed) {
                    errorMessages.push_back("Config validation error: Value for key '" + key + 
                                         "' is not in allowed values");
                    hasErrors = true;
                }
            }
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, 
                 ConfigSource source, bool validateValue) {
    set(key, ConfigValue(value), source, validateValue);
}

void Config::set(const std::string& key, int value, 
                 ConfigSource source, bool validateValue) {
    set(key, ConfigValue(value), source, validateValue);
}

void Config::set(const std::string& key, double value, 
                 ConfigSource source, bool validateValue) {
    set(key, ConfigValue(value), source, validateValue);
}

void Config::set(const std::string& key, bool value, 
                 ConfigSource source, bool validateValue) {
    set(key, ConfigValue(value), source, validateValue);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        
        if (pImpl->validationEnabled) {
            bool entryValid = isValid(entry.key);
            oss << " " << (entryValid ? "✓" : "✗");
            if (!entryValid) {
                oss << " - Errors: ";
                auto errors = getValidationErrors(entry.key);
                for (size_t i = 0; i < errors.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << errors[i];
                }
            }
        }
        oss << "\n";
    }
    
    if (!pImpl->errorMessages.empty()) {
        oss << "\nValidation Errors:\n";
        for (const auto& error : pImpl->errorMessages) {
            oss << "  ERROR: " << error << "\n";
        }
    }
    
    return oss.str();
}

bool Config::isValid(const std::string& key) const {
    if (!pImpl->validationEnabled) {
        return true;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    auto schemaIt = pImpl->schemas.find(key);
    if (schemaIt != pImpl->schemas.end()) {
        ConfigSchema& schema = schemaIt->second;
        
        if (schema.required && !has(key)) {
            return false;
        }
        
        if (!valueMatchesType(it->value, schema.valueType)) {
            return false;
        }
        
        if (schema.valueType == "double" || schema.valueType == "int" || schema.valueType == "int64_t") {
            double value = 0.0;
            if (auto* v = std::get_if<double>(&it->value)) value = *v;
            else if (auto* v = std::get_if<int>(&it->value)) value = *v;
            else if (auto* v = std::get_if<int64_t>(&it->value)) value = *v;
            
            if (value < schema.minValue || value > schema.maxValue) {
                return false;
            }
        }
        
        if (!schema.allowedValues.empty()) {
            bool valueAllowed = false;
            if (auto* v = std::get_if<std::string>(&it->value)) {
                for (const auto& allowed : schema.allowedValues) {
                    if (*v == allowed) {
                        valueAllowed = true;
                        break;
                    }
                }
            }
            if (!valueAllowed) {
                return false;
            }
        }
    }
    
    return true;
}

std::vector<std::string> Config::getValidationErrors(const std::string& key) const {
    std::vector<std::string> errors;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return errors;
    }
    
    if (!isValid(key)) {
        errors.push_back("Entry is invalid");
    }
    
    auto schemaIt = pImpl->schemas.find(key);
    if (schemaIt != pImpl->schemas.end()) {
        const ConfigSchema& schema = schemaIt->second;
        
        if (schema.required && !has(key)) {
            errors.push_back("Required field is missing");
        }
        
        for (const auto& error : pImpl->errorMessages) {
            if (error.find("'" + key + "'") != std::string::npos) {
                errors.push_back(error);
            }
        }
    }
    
    return errors;
}

void Config::addSchema(const ConfigSchema& schema) {
    if (!isValidKey(schema.key)) {
        throw ConfigValidationError("Invalid schema key: " + schema.key);
    }
    
    if (!isValidDescription(schema.description)) {
        throw ConfigValidationError("Invalid schema description: " + schema.description);
    }
    
    pImpl->schemas[schema.key] = schema;
}

void Config::removeSchema(const std::string& key) {
    pImpl->schemas.erase(key);
}

bool Config::hasSchema(const std::string& key) const {
    return pImpl->schemas.find(key) != pImpl->schemas.end();
}

ConfigSchema Config::getSchema(const std::string& key) const {
    auto it = pImpl->schemas.find(key);
    if (it == pImpl->schemas.end()) {
        return ConfigSchema();
    }
    return it->second;
}

bool Config::validateFilePath(const std::string& path) {
    if (path.empty() || path.find('\0') != std::string::npos) {
        return false;
    }
    
    std::error_code ec;
    std::filesystem::path filepath(path);
    
    if (filepath.is_absolute() && filepath.has_parent_path()) {
        return false;
    }
    
    std::string extension = filepath.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    static const std::set<std::string> dangerousExtensions = {
        ".exe", ".dll", ".so", ".dylib", ".bin", ".dat", ".ldb"
    };
    
    if (dangerousExtensions.find(extension) != dangerousExtensions.end()) {
        return false;
    }
    
    return true;
}

bool Config::validateCommandLineArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (!isValidKey(key)) {
                    return false;
                }
            } else {
                return false;
            }
        } else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            if (!isValidKey(key)) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}

void Config::validateAllEntries() const {
    hasErrors = false;
    errorMessages.clear();
    
    for (const auto& entry : pImpl->entries) {
        if (!isValid(entry.key)) {
            hasErrors = true;
            errorMessages.push_back("Invalid entry: " + entry.key);
        }
    }
}

// Helper function implementations
bool Config::isFilePathSafe(const std::string& path) {
    if (path.empty() || path.find("..") != std::string::npos) {
        return false;
    }
    
    std::error_code ec;
    std::filesystem::path filepath(path);
    
    try {
        if (std::filesystem::exists(filepath, ec)) {
            auto absolutePath = std::filesystem::absolute(filepath, ec);
            auto currentDir = std::filesystem::current_path(ec);
            
            if (ec) return false;
            
            size_t pathLen = absolutePath.string().length();
            size_t currentDirLen = currentDir.string().length();
            
            if (pathLen > currentDirLen) {
                std::string suffix = absolutePath.string().substr(currentDirLen);
                if (suffix.find("..") != std::string::npos || suffix.find("//") != std::string::npos) {
                    return false;
                }
            }
        }
    } catch (...) {
        return false;
    }
    
    return true;
}

bool Config::validateFileContent(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    int commentLines = 0;
    int emptyLines = 0;
    int invalidLines = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            if (line.empty()) emptyLines++;
            else commentLines++;
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            invalidLines++;
        } else {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (!isValidKey(key)) {
                invalidLines++;
            }
        }
    }
    
    return invalidLines == 0 && (commentLines + emptyLines + 1) < lineNumber;
}

std::vector<std::string> Config::extractValidKeysFromArgs(int argc, char** argv) {
    std::vector<std::string> validKeys;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                if (isValidKey(key)) {
                    validKeys.push_back(key);
                }
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            if (isValidKey(key)) {
                validKeys.push_back(key);
            }
        }
    }
    
    return validKeys;
}

std::string Config::getErrorMessage(const std::exception& e) {
    return std::string("Error: ") + e.what();
}

bool Config::hasErrors() const {
    return pImpl->hasErrors || !pImpl->errorMessages.empty();
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&, bool) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&, bool) const;
template std::optional<double> Config::get<double>(const std::string&, bool) const;
template std::optional<bool> Config::get<bool>(const std::string&, bool) const;
template std::optional<std::string> Config::get<std::string>(const std::string&, bool) const;

template int Config::getOr<int>(const std::string&, const int&, bool) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&, bool) const;
template double Config::getOr<double>(const std::string&, const double&, bool) const;
template bool Config::getOr<bool>(const std::string&, const bool&, bool) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&, bool) const;

} // namespace nlm
