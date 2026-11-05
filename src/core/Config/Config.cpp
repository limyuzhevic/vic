// Implementation of Config.cpp with JSON/YAML support

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == \'\' && value.back() == \'\'))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

// JSON/YAML implementation helper functions
namespace {
    nlohmann::json configValueToJson(const ConfigValue& value) {
        return std::visit([](const auto& v) -> nlohmann::json {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, int>) {
                return v;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return v;
            } else if constexpr (std::is_same_v<T, double>) {
                return v;
            } else if constexpr (std::is_same_v<T, bool>) {
                return v;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return v;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return v;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return v;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                return v;
            }
            return "";
        }, value);
    }

    ConfigValue jsonToConfigValue(const nlohmann::json& value) {
        if (value.is_number_integer()) {
            return value.get<int64_t>();
        } else if (value.is_number_float()) {
            return value.get<double>();
        } else if (value.is_boolean()) {
            return value.get<bool>();
        } else if (value.is_string()) {
            return value.get<std::string>();
        } else if (value.is_array()) {
            if (!value.empty() && value[0].is_number_integer()) {
                return value.get<std::vector<int>>();
            } else if (!value.empty() && value[0].is_number_float()) {
                return value.get<std::vector<double>>();
            } else {
                return value.get<std::vector<std::string>>();
            }
        }
        return std::string();
    }

    nlohmann::json configToJson(const Config& config) {
        nlohmann::json json;
        for (const auto& entry : config.pImpl->entries) {
            json[entry.key] = configValueToJson(entry.value);
        }
        return json;
    }

    void jsonToConfig(const nlohmann::json& json, Config& config) {
        config.clear();
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigValue value = jsonToConfigValue(*it);
            config.set(key, value, ConfigSource::File);
        }
    }

    void yamlToEmitter(const nlohmann::json& json, YAML::Emitter& emitter) {
        if (json.is_object()) {
            emitter << YAML::BeginMap;
            for (auto it = json.begin(); it != json.end(); ++it) {
                emitter << YAML::Key << it.key();
                yamlToEmitter(*it, emitter);
            }
            emitter << YAML::EndMap;
        } else if (json.is_array()) {
            emitter << YAML::BeginSeq;
            for (const auto& item : json) {
                yamlToEmitter(item, emitter);
            }
            emitter << YAML::EndSeq;
        } else if (json.is_string()) {
            emitter << json.get<std::string>();
        } else if (json.is_number_integer()) {
            emitter << json.get<int64_t>();
        } else if (json.is_number_float()) {
            emitter << json.get<double>();
        } else if (json.is_boolean()) {
            emitter << json.get<bool>();
        } else {
            emitter << "null";
        }
    }
} // namespace

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open JSON file: " + filepath);
            return false;
        }
        
        nlohmann::json json;
        file >> json;
        
        jsonToConfig(json, *this);
        NLM_LOG_INFO("Successfully loaded configuration from JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error loading JSON configuration: ") + e.what());
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node yaml = YAML::LoadFile(filepath);
        
        if (!yaml.IsMap()) {
            NLM_LOG_ERROR("Invalid YAML format: root is not a map");
            return false;
        }
        
        nlohmann::json json;
        for (const auto& pair : yaml) {
            json[pair.first.as<std::string>()] = pair.second;
        }
        
        jsonToConfig(json, *this);
        NLM_LOG_INFO("Successfully loaded configuration from YAML file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error loading YAML configuration: ") + e.what());
        return false;
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open JSON file for writing: " + filepath);
            return false;
        }
        
        nlohmann::json json = configToJson(*this);
        file << std::setw(2) << json;
        
        NLM_LOG_INFO("Successfully saved configuration to JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error saving JSON configuration: ") + e.what());
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    try {
        nlohmann::json json = configToJson(*this);
        
        YAML::Emitter emitter;
        yamlToEmitter(json, emitter);
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open YAML file for writing: " + filepath);
            return false;
        }
        
        file << emitter.c_str();
        
        NLM_LOG_INFO("Successfully saved configuration to YAML file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error saving YAML configuration: ") + e.what());
        return false;
    }
}

std::string Config::toJSON() const {
    nlohmann::json json = configToJson(*this);
    return json.dump(2);
}

bool Config::fromJSON(const std::string& jsonString) {
    try {
        nlohmann::json json = nlohmann::json::parse(jsonString);
        jsonToConfig(json, *this);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error parsing JSON: ") + e.what());
        return false;
    }
}

std::string Config::toYAML() const {
    nlohmann::json json = configToJson(*this);
    
    YAML::Emitter emitter;
    yamlToEmitter(json, emitter);
    
    return emitter.c_str();
}

bool Config::fromYAML(const std::string& yamlString) {
    try {
        YAML::Node yaml = YAML::Load(yamlString);
        
        if (!yaml.IsMap()) {
            NLM_LOG_ERROR("Invalid YAML format: root is not a map");
            return false;
        }
        
        nlohmann::json json;
        for (const auto& pair : yaml) {
            json[pair.first.as<std::string>()] = pair.second;
        }
        
        jsonToConfig(json, *this);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error parsing YAML: ") + e.what());
        return false;
    }
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    // Basic validation: check for required keys
    // This is a simple example - real validation would be more comprehensive
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            errors.push_back("Empty key found");
        }
    }
    
    return errors;
}

bool Config::isValid() const {
    return validate().empty();
}

std::vector<std::string> Config::getMissingKeys(const std::vector<std::string>& requiredKeys) const {
    std::vector<std::string> missing;
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            missing.push_back(key);
        }
    }
    return missing;
}

std::string Config::getStats() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    oss << "  Source distribution:\n";
    
    int defaultCount = 0, fileCount = 0, commandLineCount = 0, runtimeCount = 0;
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default: defaultCount++; break;
            case ConfigSource::File: fileCount++; break;
            case ConfigSource::CommandLine: commandLineCount++; break;
            case ConfigSource::Runtime: runtimeCount++; break;
        }
    }
    
    oss << "    Default: " << defaultCount << "\n";
    oss << "    File: " << fileCount << "\n";
    oss << "    Command Line: " << commandLineCount << "\n";
    oss << "    Runtime: " << runtimeCount << "\n";
    
    return oss.str();
}

std::string Config::getHelp() {
    return "NLM Configuration System\n"
           "========================\n"
           "\n"
           "Available configuration options:\n"
           "  brain.neuron_count - Total number of neurons (int)\n"
           "  brain.synapse_density - Synapse connectivity density (float)\n"
           "  brain.connection_probability - Probability of connection between neurons (float)\n"
           "  brain.initial_weight_mean - Mean initial synaptic weight (float)\n"
           "  brain.initial_weight_std - Standard deviation of initial weights (float)\n"
           "  brain.v_thresh - Neuron threshold potential (mV) (float)\n"
           "  brain.v_rest - Resting potential (mV) (float)\n"
           "  brain.v_reset - Reset potential after spike (mV) (float)\n"
           "  brain.tau_mem - Membrane time constant (ms) (float)\n"
           "  brain.tau_ref - Refractory period (ms) (float)\n"
           "  plasticity.stdp.enable - Enable STDP (bool)\n"
           "  plasticity.stdp.learning_rate - STDP learning rate (float)\n"
           "  plasticity.stdp.tau_plus - STDP time constant for potentiation (float)\n"
           "  plasticity.stdp.tau_minus - STDP time constant for depression (float)\n"
           "  neuromod.dopamine.scale - Dopamine modulation scale (float)\n"
           "  neuromod.curiosity.enable - Enable curiosity-driven exploration (bool)\n"
           "  neuromod.novelty.enable - Enable novelty detection (bool)\n"
           "\n"
           "Configuration can be loaded from files (JSON/YAML) or command line arguments.\n";
}

std::vector<std::string> Config::getAllKeys() {
    return {
        "brain.neuron_count", "brain.synapse_density", "brain.connection_probability",
        "brain.initial_weight_mean", "brain.initial_weight_std", "brain.v_thresh",
        "brain.v_rest", "brain.v_reset", "brain.tau_mem", "brain.tau_ref",
        "plasticity.stdp.enable", "plasticity.stdp.learning_rate",
        "plasticity.stdp.tau_plus", "plasticity.stdp.tau_minus",
        "plasticity.hebbian.enable", "plasticity.structural.enable",
        "neuromod.dopamine.scale", "neuromod.curiosity.enable",
        "neuromod.novelty.enable"
    };
}

std::string Config::getExample() {
    return "# Example NLM configuration\n"
           "# brain.neuron_count = 1000\n"
           "# brain.synapse_density = 0.1\n"
           "# brain.connection_probability = 0.05\n"
           "# brain.initial_weight_mean = 0.5\n"
           "# brain.initial_weight_std = 0.1\n"
           "# brain.v_thresh = -50.0\n"
           "# brain.v_rest = -70.0\n"
           "# brain.v_reset = -75.0\n"
           "# brain.tau_mem = 20.0\n"
           "# brain.tau_ref = 2.0\n"
           "# plasticity.stdp.enable = true\n"
           "# plasticity.stdp.learning_rate = 0.001\n"
           "# plasticity.stdp.tau_plus = 20.0\n"
           "# plasticity.stdp.tau_minus = 20.0\n"
           "# neuromod.dopamine.scale = 1.0\n"
           "# neuromod.curiosity.enable = true\n"
           "# neuromod.novelty.enable = true\n";
}

} // namespace nlm