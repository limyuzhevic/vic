#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format based on extension
    std::string format = getFileFormat(filepath);
    return loadFromFile(filepath, format);
}

// Load from file with explicit format
bool Config::loadFromFile(const std::string& filepath, const std::string& format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (format == "json" || format.empty()) {
        // Try JSON first
        if (loadFromJson(content)) {
            NLM_LOG_INFO("Loaded configuration from JSON: " + filepath);
            return true;
        }
        
        // If JSON fails, try as simple key=value
        if (content.find('{') == std::string::npos && content.find('[') == std::string::npos) {
            // Simple key=value format
            return loadFromSimpleFormat(content, filepath);
        }
    } else if (format == "yaml" || format == "yml") {
        if (loadFromYaml(content)) {
            NLM_LOG_INFO("Loaded configuration from YAML: " + filepath);
            return true;
        }
    }
    
    // Fallback to simple format for any file
    if (loadFromSimpleFormat(content, filepath)) {
        return true;
    }
    
    NLM_LOG_ERROR("Failed to parse configuration file: " + filepath + " (format: " + format + ")");
    return false;
}

// Load from JSON string
bool Config::loadFromJson(const std::string& jsonString) {
    // TODO: Implement nlohmann/json parsing when available
    // For now, implement a simple JSON parser for basic structures
    return false;
}

// Load from YAML string
bool Config::loadFromYaml(const std::string& yamlString) {
    // TODO: Implement yaml-cpp parsing when available
    // For now, fallback to simple format
    return false;
}

// Load from simple key=value format
bool Config::loadFromSimpleFormat(const std::string& content, const std::string& source) {
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
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
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    std::string sourceName = (source.find("configs/") != std::string::npos) ? 
        source.substr(source.find("configs/") + 8) : source;
    NLM_LOG_INFO("Loaded configuration from simple format: " + sourceName);
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

void Config::validate() const {
    // Validate configuration with basic checks
    std::vector<std::string> errors;
    
    // Check for essential keys
    std::vector<std::string> essentialKeys = {"random_seed", "simulation_timestep", "neuron_count"};
    for (const auto& key : essentialKeys) {
        if (!has(key)) {
            errors.push_back("Missing essential configuration key: " + key);
        }
    }
    
    // Validate value types and ranges
    if (has("random_seed")) {
        auto seed = get<int64_t>("random_seed");
        if (!seed) {
            errors.push_back("random_seed has invalid type (expected int64_t)");
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = get<double>("simulation_timestep");
        if (!timestep || timestep <= 0.0) {
            errors.push_back("simulation_timestep must be a positive number");
        } else if (timestep > 0.1) {
            errors.push_back("simulation_timestep too large (should be <= 0.1s for stability)");
        }
    }
    
    if (has("neuron_count")) {
        auto neuronCount = get<int64_t>("neuron_count");
        if (!neuronCount || neuronCount <= 0) {
            errors.push_back("neuron_count must be a positive integer");
        } else if (neuronCount > 1000000) {
            errors.push_back("neuron_count too large (should be <= 1,000,000 for memory constraints)");
        }
    }
    
    if (has("connection_probability")) {
        auto connProb = get<double>("connection_probability");
        if (!connProb || connProb < 0.0 || connProb > 1.0) {
            errors.push_back("connection_probability must be between 0.0 and 1.0");
        }
    }
    
    // Check for duplicate or conflicting keys
    auto keys = getKeys();
    std::sort(keys.begin(), keys.end());
    auto last = std::unique(keys.begin(), keys.end());
    if (last != keys.end()) {
        errors.push_back("Duplicate configuration keys found");
    }
    
    // If validation fails, log errors and throw exception
    if (!errors.empty()) {
        std::string errorMsg = "Configuration validation failed:\n";
        for (const auto& error : errors) {
            errorMsg += "  " + error + "\n";
        }
        NLM_LOG_ERROR("Configuration validation failed:\n" + errorMsg);
        throw std::invalid_argument(errorMsg);
    }
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
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::getFileFormat(const std::string& filepath) const {
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    // Convert to lowercase for comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") return "json";
    if (extension == "yaml" || extension == "yml") return "yaml";
    return ""; // Unknown or simple format
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
