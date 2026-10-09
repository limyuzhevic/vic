// Configuration system implementation

#include "Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::unordered_map<std::string, ConfigEntry> configEntries;
    std::string configFilepath;
    size_t maxEntries;
    bool validateRequired;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(new Impl) {
    pImpl->maxEntries = 1000;
    pImpl->validateRequired = true;
    pImpl->configFilepath = "";
    
    // Set default configuration
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    
    // STDP parameters
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Memory parameters
    set("working_memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    set("episodic_memory_max_episodes", static_cast<int64_t>(1000), ConfigSource::Default);
    set("replay_interval", static_cast<int64_t>(100), ConfigSource::Default);
    set("consolidation_interval", static_cast<int64_t>(1000), ConfigSource::Default);
    
    // Description for default entries
    pImpl->configEntries["random_seed"].description = "Random seed for simulation reproducibility";
    pImpl->configEntries["simulation_timestep"].description = "Simulation time step in seconds";
    pImpl->configEntries["neuron_count"].description = "Total number of neurons in the brain";
    pImpl->configEntries["region_count"].description = "Number of neural regions";
    pImpl->configEntries["connection_probability"].description = "Probability of forming synaptic connections";
    pImpl->configEntries["stdp_ltp_weight"].description = "STDP long-term potentiation weight";
    pImpl->configEntries["stdp_ltd_weight"].description = "STDP long-term depression weight";
    pImpl->configEntries["stdp_tau"].description = "STDP time constant for exponential decay";
    pImpl->configEntries["synaptogenesis_rate"].description = "Rate of new synapse formation";
    pImpl->configEntries["pruning_rate"].description = "Rate of synapse elimination";
    pImpl->configEntries["working_memory_capacity"].description = "Capacity of working memory in traces";
    pImpl->configEntries["episodic_memory_max_episodes"].description = "Maximum number of episodic memory episodes";
    pImpl->configEntries["replay_interval"].description = "Steps between memory replay events";
    pImpl->configEntries["consolidation_interval"].description = "Steps between memory consolidation events";
}

Config::~Config() {
    delete pImpl;
}

Config::Config(Config&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool Config::has(const std::string& key) const {
    return pImpl->configEntries.find(key) != pImpl->configEntries.end();
}

bool Config::get(const std::string& key, bool& value) const {
    auto it = pImpl->configEntries.find(key);
    if (it == pImpl->configEntries.end()) {
        return false;
    }
    
    try {
        value = std::stoi(it->second.value);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Config", "Failed to convert boolean value for key '" + key + "': " + e.what());
        return false;
    }
}

bool Config::get(const std::string& key, int64_t& value) const {
    auto it = pImpl->configEntries.find(key);
    if (it == pImpl->configEntries.end()) {
        return false;
    }
    
    try {
        value = std::stoll(it->second.value);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Config", "Failed to convert int64_t value for key '" + key + "': " + e.what());
        return false;
    }
}

bool Config::get(const std::string& key, uint64_t& value) const {
    auto it = pImpl->configEntries.find(key);
    if (it == pImpl->configEntries.end()) {
        return false;
    }
    
    try {
        value = std::stoull(it->second.value);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Config", "Failed to convert uint64_t value for key '" + key + "': " + e.what());
        return false;
    }
}

bool Config::get(const std::string& key, double& value) const {
    auto it = pImpl->configEntries.find(key);
    if (it == pImpl->configEntries.end()) {
        return false;
    }
    
    try {
        value = std::stod(it->second.value);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Config", "Failed to convert double value for key '" + key + "': " + e.what());
        return false;
    }
}

bool Config::get(const std::string& key, float& value) const {
    double temp;
    if (!get(key, temp)) {
        return false;
    }
    value = static_cast<float>(temp);
    return true;
}

bool Config::get(const std::string& key, std::string& value) const {
    auto it = pImpl->configEntries.find(key);
    if (it == pImpl->configEntries.end()) {
        return false;
    }
    value = it->second.value;
    return true;
}

bool Config::getOr(const std::string& key, bool defaultValue) const {
    bool value;
    if (get(key, value)) {
        return value;
    }
    return defaultValue;
}

int64_t Config::getOr(const std::string& key, int64_t defaultValue) const {
    int64_t value;
    if (get(key, value)) {
        return value;
    }
    return defaultValue;
}

uint64_t Config::getOr(const std::string& key, uint64_t defaultValue) const {
    uint64_t value;
    if (get(key, value)) {
        return value;
    }
    return defaultValue;
}

double Config::getOr(const std::string& key, double defaultValue) const {
    double value;
    if (get(key, value)) {
        return value;
    }
    return defaultValue;
}

float Config::getOr(const std::string& key, float defaultValue) const {
    float value;
    if (get(key, value)) {
        return value;
    }
    return defaultValue;
}

std::string Config::getOr(const std::string& key, const std::string& defaultValue) const {
    std::string value;
    if (get(key, value)) {
        return value;
    }
    return defaultValue;
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    ConfigEntry entry;
    entry.key = key;
    entry.value = value ? "1" : "0";
    entry.source = source;
    entry.description = pImpl->configEntries[key].description; // Keep existing description
    
    pImpl->configEntries[key] = entry;
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    ConfigEntry entry;
    entry.key = key;
    entry.value = std::to_string(value);
    entry.source = source;
    entry.description = pImpl->configEntries[key].description; // Keep existing description
    
    pImpl->configEntries[key] = entry;
}

void Config::set(const std::string& key, uint64_t value, ConfigSource source) {
    ConfigEntry entry;
    entry.key = key;
    entry.value = std::to_string(value);
    entry.source = source;
    entry.description = pImpl->configEntries[key].description; // Keep existing description
    
    pImpl->configEntries[key] = entry;
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    ConfigEntry entry;
    entry.key = key;
    entry.value = std::to_string(value);
    entry.source = source;
    entry.description = pImpl->configEntries[key].description; // Keep existing description
    
    pImpl->configEntries[key] = entry;
}

void Config::set(const std::string& key, float value, ConfigSource source) {
    set(key, static_cast<double>(value), source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigEntry entry;
    entry.key = key;
    entry.value = value;
    entry.source = source;
    entry.description = pImpl->configEntries[key].description; // Keep existing description
    
    pImpl->configEntries[key] = entry;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Config", "Failed to open config file: " + filepath);
            return false;
        }
        
        // Parse JSON-like format (simplified)
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments
            size_t commentPos = line.find("//");
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            // Trim whitespace
            size_t start = line.find_first_not_of(" \t\n\r");
            size_t end = line.find_last_not_of(" \t\n\r");
            if (start == std::string::npos || end == std::string::npos) {
                continue;
            }
            line = line.substr(start, end - start + 1);
            
            // Parse key-value pair (simple "key=value" format)
            size_t equalsPos = line.find('=');
            if (equalsPos == std::string::npos) {
                continue;
            }
            
            std::string key = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            // Trim key and value
            start = key.find_first_not_of(" \t\n\r");
            end = key.find_last_not_of(" \t\n\r");
            if (start != std::string::npos) {
                key = key.substr(start, end - start + 1);
            }
            
            start = value.find_first_not_of(" \t\n\r");
            end = value.find_last_not_of(" \t\n\r");
            if (start != std::string::npos) {
                value = value.substr(start, end - start + 1);
            }
            
            // Remove quotes from value
            if (!value.empty() && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
        
        pImpl->configFilepath = filepath;
        NLM_LOG_INFO("Config", "Loaded configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Config", "Exception loading config file: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Config", "Failed to create config file: " + filepath);
            return false;
        }
        
        // Write configuration in simple format
        for (const auto& pair : pImpl->configEntries) {
            file << pair.first << " = " << pair.second.value << " // " << pair.second.description << std::endl;
        }
        
        NLM_LOG_INFO("Config", "Saved configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Config", "Exception saving config file: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        size_t equalsPos = arg.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = arg.substr(0, equalsPos);
            std::string value = arg.substr(equalsPos + 1);
            
            // Remove -- prefix from key
            if (key.size() >= 2 && key.substr(0, 2) == "--") {
                key = key.substr(2);
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle --key value format
        else if (arg.size() >= 2 && arg.substr(0, 2) == "--" && i + 1 < argc) {
            std::string key = arg.substr(2);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    
    return success;
}

void Config::clear() {
    clear(ConfigSource::Runtime);
}

void Config::clear(ConfigSource source) {
    for (auto it = pImpl->configEntries.begin(); it != pImpl->configEntries.end();) {
        if (it->second.source == source) {
            it = pImpl->configEntries.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->configEntries.size());
    
    for (const auto& pair : pImpl->configEntries) {
        keys.push_back(pair.first);
    }
    
    return keys;
}

std::string Config::getDescription(const std::string& key) const {
    auto it = pImpl->configEntries.find(key);
    if (it != pImpl->configEntries.end()) {
        return it->second.description;
    }
    return "";
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = pImpl->configEntries.find(key);
    if (it != pImpl->configEntries.end()) {
        return it->second.source;
    }
    return ConfigSource::Default;
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "Configuration Summary:" << std::endl;
    ss << "Total entries: " << pImpl->configEntries.size() << std::endl;
    
    for (const auto& pair : pImpl->configEntries) {
        ss << "  " << pair.first << " = " << pair.second.value 
           << " (source: " << static_cast<int>(pair.second.source) << ")" << std::endl;
    }
    
    return ss.str();
}

void Config::print() const {
    NLM_LOG_INFO("Config", summary());
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    // Basic validation rules
    auto neuronCountIt = pImpl->configEntries.find("neuron_count");
    if (neuronCountIt != pImpl->configEntries.end()) {
        try {
            int64_t count = std::stoll(neuronCountIt->second.value);
            if (count <= 0) {
                pImpl->validationErrors.push_back("neuron_count must be positive");
            } else if (count > 1000000) {
                pImpl->validationErrors.push_back("neuron_count cannot exceed 1,000,000");
            }
        } catch (const std::exception&) {
            pImpl->validationErrors.push_back("neuron_count has invalid value");
        }
    }
    
    auto timestepIt = pImpl->configEntries.find("simulation_timestep");
    if (timestepIt != pImpl->configEntries.end()) {
        try {
            double timestep = std::stod(timestepIt->second.value);
            if (timestep <= 0.0) {
                pImpl->validationErrors.push_back("simulation_timestep must be positive");
            } else if (timestep > 0.1) {
                pImpl->validationErrors.push_back("simulation_timestep cannot exceed 0.1 seconds");
            }
        } catch (const std::exception&) {
            pImpl->validationErrors.push_back("simulation_timestep has invalid value");
        }
    }
    
    auto connectionProbIt = pImpl->configEntries.find("connection_probability");
    if (connectionProbIt != pImpl->configEntries.end()) {
        try {
            double prob = std::stod(connectionProbIt->second.value);
            if (prob < 0.0 || prob > 1.0) {
                pImpl->validationErrors.push_back("connection_probability must be between 0.0 and 1.0");
            }
        } catch (const std::exception&) {
            pImpl->validationErrors.push_back("connection_probability has invalid value");
        }
    }
    
    return pImpl->validationErrors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

std::string Config::serialize() const {
    std::stringstream ss;
    ss << "{\n";
    
    for (size_t i = 0; i < pImpl->configEntries.size(); ++i) {
        const auto& pair = *std::next(pImpl->configEntries.begin(), i);
        ss << "  \"" << pair.first << "\": \"" << pair.second.value << "\"";
        
        if (i < pImpl->configEntries.size() - 1) {
            ss << ",";
        }
        ss << "\n";
    }
    
    ss << "}\n";
    return ss.str();
}

bool Config::deserialize(const std::string& data) {
    try {
        clear(ConfigSource::File); // Clear existing file-based config
        
        // Simple JSON-like parsing (not full JSON)
        size_t pos = 0;
        while (pos < data.size()) {
            size_t keyStart = data.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = data.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = data.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            size_t valueStart = data.find('"', keyEnd + 1);
            if (valueStart == std::string::npos) break;
            
            size_t valueEnd = data.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            
            std::string value = data.substr(valueStart + 1, valueEnd - valueStart - 1);
            
            set(key, value, ConfigSource::File);
            
            pos = valueEnd + 1;
            while (pos < data.size() && (data[pos] == ' ' || data[pos] == ',' || data[pos] == '\n')) {
                pos++;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Config", "Exception deserializing config: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
