#pragma once

#include "Config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace nlm {

using json = nlohmann::json;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration
    loadDefaultConfig();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::loadDefaultConfig() {
    // Load defaults from a predefined set
    pImpl->entries.clear();
    
    // Default configuration values based on project requirements
    set("random_seed", uint64_t(42), ConfigSource::Default, "Random seed for reproducible experiments");
    set("simulation_timestep", double(0.001), ConfigSource::Default, "Simulation timestep in seconds");
    set("neuron_count", size_t(1000), ConfigSource::Default, "Number of neurons in the brain");
    set("region_count", size_t(2), ConfigSource::Default, "Number of brain regions");
    set("connection_probability", double(0.1), ConfigSource::Default, "Connection probability for random connectivity");
    
    // Plasticity settings
    set("plasticity_learning_rate", double(0.01), ConfigSource::Default, "Global plasticity learning rate");
    set("stdp_ltp_weight", double(0.01), ConfigSource::Default, "STDP long-term potentiation weight");
    set("stdp_ltd_weight", double(0.012), ConfigSource::Default, "STDP long-term depression weight");
    set("stdp_time_constant", double(20.0), ConfigSource::Default, "STDP time constant in milliseconds");
    
    // Development settings
    set("development_synaptogenesis_rate", double(0.0001), ConfigSource::Default, "Synaptogenesis rate during development");
    set("development_pruning_rate", double(0.00001), ConfigSource::Default, "Pruning rate during development");
    
    // Neuromodulation settings
    set("dopamine_baseline", double(0.1), ConfigSource::Default, "Baseline dopamine level");
    set("curiosity_threshold", double(0.5), ConfigSource::Default, "Novelty threshold for curiosity activation");
    set("novelty_threshold", double(0.3), ConfigSource::Default, "Threshold for novelty detection");
    
    // Reward settings
    set("reward_discount_factor", double(0.99), ConfigSource::Default, "Discount factor for future rewards");
    
    // Environment settings
    set("environment_name", std::string("GridWorld"), ConfigSource::Default, "Name of the simulation environment");
    set("environment_width", int(10), ConfigSource::Default, "Width of the simulation environment");
    set("environment_height", int(10), ConfigSource::Default, "Height of the simulation environment");
    
    // Simulation settings
    set("max_simulation_steps", size_t(10000), ConfigSource::Default, "Maximum number of simulation steps");
    set("simulation_time_limit", double(0.0), ConfigSource::Default, "Simulation time limit in seconds (0 = unlimited)");
    set("replay_interval", size_t(100), ConfigSource::Default, "Interval for memory replay in steps");
    set("consolidation_interval", size_t(1000), ConfigSource::Default, "Interval for memory consolidation in steps");
    
    // Visualization settings
    set("visualization_enabled", bool(false), ConfigSource::Default, "Enable visualization updates");
    set("visualization_update_rate", double(30.0), ConfigSource::Default, "Visualization update rate in Hz");
    
    // Additional configuration options for advanced usage
    set("checkpoint_dir", std::string("./checkpoints"), ConfigSource::Default, "Directory for checkpoint files");
    set("checkpoint_interval", size_t(1000), ConfigSource::Default, "Interval for checkpointing in steps");
    set("checkpoint_max_files", size_t(10), ConfigSource::Default, "Maximum number of checkpoint files to keep");
    set("enable_development", bool(true), ConfigSource::Default, "Enable developmental processes");
    set("enable_curiosity", bool(true), ConfigSource::Default, "Enable curiosity-driven exploration");
    set("enable_structural_plasticity", bool(true), ConfigSource::Default, "Enable structural synaptic plasticity");
    set("learning_rate", double(0.01), ConfigSource::Default, "Global learning rate for plasticity");
    set("membrane_threshold", double(-50.0), ConfigSource::Default, "Neuron membrane threshold potential (mV)");
    set("membrane_resting", double(-70.0), ConfigSource::Default, "Neuron resting membrane potential (mV)");
    set("membrane_reset", double(-75.0), ConfigSource::Default, "Neuron reset potential after spike (mV)");
    set("membrane_time_constant", double(20.0), ConfigSource::Default, "Membrane time constant (ms)");
    set("refractory_period", double(2.0), ConfigSource::Default, "Refractory period after spike (ms)");
}

bool Config::loadFromFile(const std::string& filepath) {
    // Validate input
    if (filepath.empty()) {
        NLM_LOG_ERROR("Attempted to load config from empty filepath");
        return false;
    }
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_ERROR("Config file does not exist: " + filepath);
        return false;
    }
    
    // Check if it's a file (not a directory)
    if (!std::filesystem::is_regular_file(filepath)) {
        NLM_LOG_ERROR("Config path is not a file: " + filepath);
        return false;
    }
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file: " + filepath);
            return false;
        }
        
        // Parse JSON
        json j;
        file >> j;
        
        // Clear existing entries and load from JSON
        pImpl->entries.clear();
        
        // Parse JSON with validation
        parseJSON(j);
        
        NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
        NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries");
        
        return true;
        
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in config file " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        // Clear existing entries
        pImpl->entries.clear();
        
        // Load defaults first
        loadDefaultConfig();
        
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    setFromString(key, value, ConfigSource::CommandLine);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                setFromString(key, value, ConfigSource::CommandLine);
            }
            // Handle boolean flags (--enable-feature)
            else if (arg.substr(0, 2) == "--" && arg.find('=') == std::string::npos) {
                std::string flag = arg.substr(2);
                if (flag.find("enable_") == 0) {
                    std::string key = "enable_" + flag.substr(8);
                    set(key, bool(true), ConfigSource::CommandLine);
                }
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from command line arguments");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config from arguments: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    // Validate input
    if (filepath.empty()) {
        NLM_LOG_ERROR("Attempted to save config to empty filepath");
        return false;
    }
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
            return false;
        }
        
        // Convert to JSON
        json j = toJSON();
        
        // Write JSON with formatting
        file << j.dump(2);
        
        NLM_LOG_INFO("Successfully saved configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void Config::parseJSON(const json& j) {
    if (!j.is_object()) {
        throw std::runtime_error("JSON config must be an object");
    }
    
    for (const auto& [key, value] : j.items()) {
        setFromJSONValue(key, value, ConfigSource::File);
    }
}

json Config::toJSON() const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = configValueToJSON(entry.value);
    }
    
    return j;
}

void Config::setFromJSONValue(const std::string& key, const json& value, ConfigSource source) {
    try {
        ConfigValue configValue;
        
        if (value.is_null()) {
            throw std::runtime_error("JSON null value cannot be stored in config");
        } else if (value.is_boolean()) {
            configValue = value.get<bool>();
        } else if (value.is_number_integer()) {
            // Try different integer types
            if (value.get<int64_t>() <= std::numeric_limits<int>::max() && 
                value.get<int64_t>() >= std::numeric_limits<int>::min()) {
                configValue = value.get<int>();
            } else {
                configValue = value.get<int64_t>();
            }
        } else if (value.is_number_unsigned()) {
            configValue = value.get<size_t>();
        } else if (value.is_number()) {
            // Handle floating point numbers
            if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else {
                configValue = value.get<double>();
            }
        } else if (value.is_string()) {
            configValue = value.get<std::string>();
        } else if (value.is_array()) {
            // Handle arrays
            if (value.empty()) {
                throw std::runtime_error("Empty arrays are not supported in config");
            }
            
            const auto& first = value[0];
            if (first.is_boolean()) {
                std::vector<bool> vec;
                for (const auto& item : value) {
                    vec.push_back(item.get<bool>());
                }
                configValue = vec;
            } else if (first.is_number_integer()) {
                std::vector<int> vec;
                for (const auto& item : value) {
                    vec.push_back(item.get<int>());
                }
                configValue = vec;
            } else if (first.is_number()) {
                std::vector<double> vec;
                for (const auto& item : value) {
                    vec.push_back(item.get<double>());
                }
                configValue = vec;
            } else if (first.is_string()) {
                std::vector<std::string> vec;
                for (const auto& item : value) {
                    vec.push_back(item.get<std::string>());
                }
                configValue = vec;
            } else {
                throw std::runtime_error("Unsupported array type in config");
            }
        } else {
            throw std::runtime_error("Unsupported JSON type in config");
        }
        
        set(key, configValue, source);
        
    } catch (const json::type_error& e) {
        throw std::runtime_error("Type error parsing config value for key '" + key + "': " + std::string(e.what()));
    }
}

json Config::configValueToJSON(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            return json(arg);
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return json(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return json(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            json arr = json::array();
            for (const auto& item : arg) {
                arr.push_back(item);
            }
            return arr;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json arr = json::array();
            for (const auto& item : arg) {
                arr.push_back(item);
            }
            return arr;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json arr = json::array();
            for (const auto& item : arg) {
                arr.push_back(item);
            }
            return arr;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json arr = json::array();
            for (const auto& item : arg) {
                arr.push_back(item);
            }
            return arr;
        } else {
            throw std::runtime_error("Unsupported config value type");
        }
    }, value);
}

void Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Try to parse the value as different types
    try {
        // Try bool
        if (valueStr == "true" || valueStr == "false") {
            bool b = (valueStr == "true");
            set(key, b, source);
            return;
        }
        
        // Try integer
        size_t pos;
        try {
            long long intVal = std::stoll(valueStr);
            set(key, intVal, source);
            return;
        } catch (...) {}
        
        // Try unsigned integer
        try {
            unsigned long long uintVal = std::stoull(valueStr);
            set(key, static_cast<size_t>(uintVal), source);
            return;
        } catch (...) {}
        
        // Try double
        try {
            double doubleVal = std::stod(valueStr);
            set(key, doubleVal, source);
            return;
        } catch (...) {}
        
        // Fall back to string
        set(key, valueStr, source);
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse config value for key '" + key + "': " + std::string(e.what()));
    }
}

std::string Config::getValueDescription(const ConfigEntry& entry) const {
    std::ostringstream oss;
    oss << entry.key << " = ";
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> ||
                           std::is_same_v<T, size_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << (arg[i] ? "true" : "false");
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                           std::is_same_v<T, std::vector<size_t>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i];
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i];
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << "\"" << arg[i] << "\"";
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        }
    }, entry.value);
    
    oss << " (source: " << static_cast<int>(entry.source) << ")";
    return oss.str();
}

} // namespace nlm
