#include "Config.hpp"
#include "Schema.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    int version = CONFIG_VERSION;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Read file
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ParseException("Cannot open configuration file: " + filepath);
        }
        
        // Parse JSON
        nlohmann::json jsonData;
        try {
            file >> jsonData;
        } catch (const nlohmann::json::parse_error& e) {
            throw ParseException("Invalid JSON in configuration file " + filepath + ": " + e.what());
        }
        
        // Check version
        if (jsonData.contains("__config_version")) {
            int fileVersion = jsonData["__config_version"];
            if (fileVersion != CONFIG_VERSION) {
                throw ValidationException("Configuration version mismatch: file version " + 
                                        std::to_string(fileVersion) + ", expected " + 
                                        std::to_string(CONFIG_VERSION));
            }
        }
        
        // Clear existing entries
        clear();
        
        // Convert JSON to Config entries
        if (jsonData.contains("entries")) {
            for (const auto& entry : jsonData["entries"]) {
                std::string key = entry.value("key", "");
                std::string description = entry.value("description", "");
                std::string sourceStr = entry.value("source", "File");
                
                ConfigSource source = ConfigSource::File;
                if (sourceStr == "CommandLine") source = ConfigSource::CommandLine;
                else if (sourceStr == "Runtime") source = ConfigSource::Runtime;
                
                // Parse value based on type
                ConfigValue value;
                if (entry.contains("int_value")) {
                    value = entry["int_value"];
                } else if (entry.contains("int64_value")) {
                    value = entry["int64_value"];
                } else if (entry.contains("double_value")) {
                    value = entry["double_value"];
                } else if (entry.contains("bool_value")) {
                    value = entry["bool_value"];
                } else if (entry.contains("string_value")) {
                    value = entry["string_value"];
                } else if (entry.contains("vector_int_value")) {
                    value = entry["vector_int_value"].get<std::vector<int>>();
                } else if (entry.contains("vector_double_value")) {
                    value = entry["vector_double_value"].get<std::vector<double>>();
                } else if (entry.contains("vector_string_value")) {
                    value = entry["vector_string_value"].get<std::vector<std::string>>();
                }
                
                set(key, value, source);
            }
        }
        
        // Validate configuration using schema
        auto schemaValidation = validateAllParametersFromSchema(jsonData);
        if (!schemaValidation.first) {
            std::string errorMsg = "Configuration schema validation failed:\n";
            for (const auto& error : schemaValidation.second) {
                errorMsg += "  - " + error + "\n";
            }
            throw ValidationException(errorMsg);
        }
        
        return true;
        
    } catch (const ConfigException& e) {
        // Re-throw configuration-specific exceptions
        throw;
    } catch (const std::exception& e) {
        throw ParseException("Error loading configuration: " + std::string(e.what()));
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Create JSON structure
        nlohmann::json jsonData;
        jsonData["__config_version"] = CONFIG_VERSION;
        
        // Add entries
        nlohmann::json entries = nlohmann::json::array();
        for (const auto& entry : pImpl->entries) {
            nlohmann::json jsonEntry;
            jsonEntry["key"] = entry.key;
            jsonEntry["description"] = entry.description;
            
            // Convert source to string
            std::string sourceStr;
            switch (entry.source) {
                case ConfigSource::Default: sourceStr = "Default"; break;
                case ConfigSource::File: sourceStr = "File"; break;
                case ConfigSource::CommandLine: sourceStr = "CommandLine"; break;
                case ConfigSource::Runtime: sourceStr = "Runtime"; break;
            }
            jsonEntry["source"] = sourceStr;
            
            // Convert ConfigValue to JSON
            if (std::holds_alternative<int>(entry.value)) {
                jsonEntry["int_value"] = std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                jsonEntry["int64_value"] = std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                jsonEntry["double_value"] = std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                jsonEntry["bool_value"] = std::get<bool>(entry.value);
            } else if (std::holds_alternative<std::string>(entry.value)) {
                jsonEntry["string_value"] = std::get<std::string>(entry.value);
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                jsonEntry["vector_int_value"] = std::get<std::vector<int>>(entry.value);
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                jsonEntry["vector_double_value"] = std::get<std::vector<double>>(entry.value);
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                jsonEntry["vector_string_value"] = std::get<std::vector<std::string>>(entry.value);
            }
            
            entries.push_back(jsonEntry);
        }
        
        jsonData["entries"] = entries;
        
        // Write JSON to file
        file << std::setw(2) << jsonData;
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
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

// Schema-based validation implementation
std::pair<bool, std::vector<std::string>> Config::validateAllParametersFromSchema(const nlohmann::json& jsonData) {
    std::vector<std::string> errors;
    
    // Basic structure validation
    if (!jsonData.contains("neuron_count")) {
        errors.push_back("Missing required parameter: neuron_count");
    }
    
    if (!jsonData.contains("region_count")) {
        errors.push_back("Missing required parameter: region_count");
    }
    
    if (!jsonData.contains("population_per_region")) {
        errors.push_back("Missing required parameter: population_per_region");
    }
    
    // Parse basic parameters
    BasicConfig basicConfig;
    if (jsonData.contains("neuron_count")) {
        basicConfig.neuron_count = jsonData["neuron_count"];
    }
    
    if (jsonData.contains("region_count")) {
        basicConfig.region_count = jsonData["region_count"];
    }
    
    if (jsonData.contains("population_per_region")) {
        basicConfig.population_per_region = jsonData["population_per_region"];
    }
    
    // Parse optional sections
    if (jsonData.contains("basic")) {
        const auto& basicJson = jsonData["basic"];
        if (basicJson.contains("neuron_size_min")) {
            basicConfig.neuron_size_min = basicJson["neuron_size_min"];
        }
        if (basicJson.contains("neuron_size_max")) {
            basicConfig.neuron_size_max = basicJson["neuron_size_max"];
        }
        if (basicJson.contains("connection_probability")) {
            basicConfig.connection_probability = basicJson["connection_probability"];
        }
        if (basicJson.contains("connection_delay_min")) {
            basicConfig.connection_delay_min = basicJson["connection_delay_min"];
        }
        if (basicJson.contains("connection_delay_max")) {
            basicConfig.connection_delay_max = basicJson["connection_delay_max"];
        }
        if (basicJson.contains("simulation_timestep")) {
            basicConfig.simulation_timestep = basicJson["simulation_timestep"];
        }
        if (basicJson.contains("simulation_steps")) {
            basicConfig.simulation_steps = basicJson["simulation_steps"];
        }
        if (basicJson.contains("simulation_time")) {
            basicConfig.simulation_time = basicJson["simulation_time"];
        }
        if (basicJson.contains("random_seed")) {
            basicConfig.random_seed = basicJson["random_seed"];
        }
    }
    
    // Validate basic parameters
    auto basicResult = validate_neuron_parameters(basicConfig);
    if (!basicResult.first) {
        errors.insert(errors.end(), basicResult.second.begin(), basicResult.second.end());
    }
    
    // Validate plasticity parameters
    if (jsonData.contains("plasticity")) {
        PlasticityConfig plasticConfig;
        const auto& plasticJson = jsonData["plasticity"];
        if (plasticJson.contains("stdp_ltp_weight")) {
            plasticConfig.stdp_ltp_weight = plasticJson["stdp_ltp_weight"];
        }
        if (plasticJson.contains("stdp_ltd_weight")) {
            plasticConfig.stdp_ltd_weight = plasticJson["stdp_ltd_weight"];
        }
        if (plasticJson.contains("stdp_tau")) {
            plasticConfig.stdp_tau = plasticJson["stdp_tau"];
        }
        if (plasticJson.contains("stdp_max_weight_change")) {
            plasticConfig.stdp_max_weight_change = plasticJson["stdp_max_weight_change"];
        }
        if (plasticJson.contains("hebbian_learning_rate")) {
            plasticConfig.hebbian_learning_rate = plasticJson["hebbian_learning_rate"];
        }
        if (plasticJson.contains("hebbian_threshold")) {
            plasticConfig.hebbian_threshold = plasticJson["hebbian_threshold"];
        }
        if (plasticJson.contains("synaptogenesis_rate")) {
            plasticConfig.synaptogenesis_rate = plasticJson["synaptogenesis_rate"];
        }
        if (plasticJson.contains("pruning_rate")) {
            plasticConfig.pruning_rate = plasticJson["pruning_rate"];
        }
        if (plasticJson.contains("max_synaptic_weight")) {
            plasticConfig.max_synaptic_weight = plasticJson["max_synaptic_weight"];
        }
        if (plasticJson.contains("min_synaptic_weight")) {
            plasticConfig.min_synaptic_weight = plasticJson["min_synaptic_weight"];
        }
        
        auto plasticResult = validate_plasticity_parameters(plasticConfig);
        if (!plasticResult.first) {
            errors.insert(errors.end(), plasticResult.second.begin(), plasticResult.second.end());
        }
    }
    
    // Validate synaptic dynamics parameters
    if (jsonData.contains("synaptic_dynamics")) {
        SynapticDynamicsConfig synapticConfig;
        const auto& synapticJson = jsonData["synaptic_dynamics"];
        if (synapticJson.contains("excitatory_reversal_potential")) {
            synapticConfig.excitatory_reversal_potential = synapticJson["excitatory_reversal_potential"];
        }
        if (synapticJson.contains("inhibitory_reversal_potential")) {
            synapticConfig.inhibitory_reversal_potential = synapticJson["inhibitory_reversal_potential"];
        }
        if (synapticJson.contains("membrane_capacitance")) {
            synapticConfig.membrane_capacitance = synapticJson["membrane_capacitance"];
        }
        if (synapticJson.contains("time_constant")) {
            synapticConfig.time_constant = synapticJson["time_constant"];
        }
        if (synapticJson.contains("spike_threshold")) {
            synapticConfig.spike_threshold = synapticJson["spike_threshold"];
        }
        if (synapticJson.contains("spike_reset_potential")) {
            synapticConfig.spike_reset_potential = synapticJson["spike_reset_potential"];
        }
        if (synapticJson.contains("refractory_period")) {
            synapticConfig.refractory_period = synapticJson["refractory_period"];
        }
        if (synapticJson.contains("adaptation_time_constant")) {
            synapticConfig.adaptation_time_constant = synapticJson["adaptation_time_constant"];
        }
        if (synapticJson.contains("adaptation_strength")) {
            synapticConfig.adaptation_strength = synapticJson["adaptation_strength"];
        }
        
        auto synapticResult = validate_synaptic_parameters(synapticConfig);
        if (!synapticResult.first) {
            errors.insert(errors.end(), synapticResult.second.begin(), synapticResult.second.end());
        }
    }
    
    // Validate neuromodulation parameters
    if (jsonData.contains("neuromodulation")) {
        NeuromodulationConfig neuromodConfig;
        const auto& neuromodJson = jsonData["neuromodulation"];
        if (neuromodJson.contains("dopamine_baseline")) {
            neuromodConfig.dopamine_baseline = neuromodJson["dopamine_baseline"];
        }
        if (neuromodJson.contains("dopamine_peak")) {
            neuromodConfig.dopamine_peak = neuromodJson["dopamine_peak"];
        }
        if (neuromodJson.contains("dopamine_decay_rate")) {
            neuromodConfig.dopamine_decay_rate = neuromodJson["dopamine_decay_rate"];
        }
        if (neuromodJson.contains("acetylcholine_baseline")) {
            neuromodConfig.acetylcholine_baseline = neuromodJson["acetylcholine_baseline"];
        }
        if (neuromodJson.contains("norepinephrine_sharpening")) {
            neuromodConfig.norepinephrine_sharpening = neuromodJson["norepinephrine_sharpening"];
        }
        if (neuromodJson.contains("serotonin_inhibition")) {
            neuromodConfig.serotonin_inhibition = neuromodJson["serotonin_inhibition"];
        }
        if (neuromodJson.contains("histamine_wake")) {
            neuromodConfig.histamine_wake = neuromodJson["histamine_wake"];
        }
        
        auto neuromodResult = validate_neuromodulation_parameters(neuromodConfig);
        if (!neuromodResult.first) {
            errors.insert(errors.end(), neuromodResult.second.begin(), neuromodResult.second.end());
        }
    }
    
    // Validate development parameters
    if (jsonData.contains("development")) {
        DevelopmentConfig devConfig;
        const auto& devJson = jsonData["development"];
        if (devJson.contains("critical_period_start")) {
            devConfig.critical_period_start = devJson["critical_period_start"];
        }
        if (devJson.contains("critical_period_end")) {
            devConfig.critical_period_end = devJson["critical_period_end"];
        }
        if (devJson.contains("maturation_rate")) {
            devConfig.maturation_rate = devJson["maturation_rate"];
        }
        if (devJson.contains("pruning_rate")) {
            devConfig.pruning_rate = devJson["pruning_rate"];
        }
        if (devJson.contains("myelination_rate")) {
            devConfig.myelination_rate = devJson["myelination_rate"];
        }
        if (devJson.contains("refinement_rate")) {
            devConfig.refinement_rate = devJson["refinement_rate"];
        }
        
        auto devResult = validate_development_parameters(devConfig);
        if (!devResult.first) {
            errors.insert(errors.end(), devResult.second.begin(), devResult.second.end());
        }
    }
    
    // Validate memory parameters
    if (jsonData.contains("memory")) {
        MemoryConfig memoryConfig;
        const auto& memoryJson = jsonData["memory"];
        if (memoryJson.contains("working_memory_capacity")) {
            memoryConfig.working_memory_capacity = memoryJson["working_memory_capacity"];
        }
        if (memoryJson.contains("working_memory_decay_rate")) {
            memoryConfig.working_memory_decay_rate = memoryJson["working_memory_decay_rate"];
        }
        if (memoryJson.contains("max_episodes")) {
            memoryConfig.max_episodes = memoryJson["max_episodes"];
        }
        if (memoryJson.contains("episode_decay_rate")) {
            memoryConfig.episode_decay_rate = memoryJson["episode_decay_rate"];
        }
        if (memoryJson.contains("semantic_memory_capacity")) {
            memoryConfig.semantic_memory_capacity = memoryJson["semantic_memory_capacity"];
        }
        if (memoryJson.contains("procedural_memory_capacity")) {
            memoryConfig.procedural_memory_capacity = memoryJson["procedural_memory_capacity"];
        }
        
        auto memoryResult = validate_memory_parameters(memoryConfig);
        if (!memoryResult.first) {
            errors.insert(errors.end(), memoryResult.second.begin(), memoryResult.second.end());
        }
    }
    
    // Validate integration parameters
    if (jsonData.contains("integration")) {
        IntegrationConfig integrationConfig;
        const auto& integrationJson = jsonData["integration"];
        if (integrationJson.contains("sensory_weight")) {
            integrationConfig.sensory_weight = integrationJson["sensory_weight"];
        }
        if (integrationJson.contains("sensory_threshold")) {
            integrationConfig.sensory_threshold = integrationJson["sensory_threshold"];
        }
        if (integrationJson.contains("motor_gain")) {
            integrationConfig.motor_gain = integrationJson["motor_gain"];
        }
        if (integrationJson.contains("motor_threshold")) {
            integrationConfig.motor_threshold = integrationJson["motor_threshold"];
        }
        if (integrationJson.contains("attentional_focus_width")) {
            integrationConfig.attentional_focus_width = integrationJson["attentional_focus_width"];
        }
        if (integrationJson.contains("attentional_gain")) {
            integrationConfig.attentional_gain = integrationJson["attentional_gain"];
        }
        if (integrationJson.contains("prediction_error_threshold")) {
            integrationConfig.prediction_error_threshold = integrationJson["prediction_error_threshold"];
        }
        if (integrationJson.contains("prediction_error_decay")) {
            integrationConfig.prediction_error_decay = integrationJson["prediction_error_decay"];
        }
        
        auto integrationResult = validate_integration_parameters(integrationConfig);
        if (!integrationResult.first) {
            errors.insert(errors.end(), integrationResult.second.begin(), integrationResult.second.end());
        }
    }
    
    return std::make_pair(errors.empty(), errors);
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (version " << pImpl->version << ") (" << pImpl->entries.size() << " entries):\n";
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
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
