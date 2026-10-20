#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// JSON serialization helpers
template<typename T>
void jsonToConfigValue(const nlohmann::json& j, ConfigValue& value);

template<typename T>
void configValueToJson(const ConfigValue& value, nlohmann::json& j);

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format by extension
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json" || extension == ".jsonl" || extension == ".yaml" || extension == ".yml") {
        return loadFromJson(filepath);
    } else {
        // Fall back to simple key=value format
        return loadFromText(filepath);
    }
}

bool Config::loadFromText(const std::string& filepath) {
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
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        // Clear existing entries
        clear();
        
        // Parse JSON recursively
        parseJsonRecursive(".", j, ConfigSource::File);
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void Config::parseJsonRecursive(const std::string& prefix, const nlohmann::json& j, ConfigSource source) {
    if (j.is_object()) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = prefix.empty() ? it.key() : (prefix + "." + it.key());
            parseJsonRecursive(key, *it, source);
        }
    } else if (j.is_array()) {
        // Handle arrays as a single entry
        set(prefix, jsonToConfigValue<j>(j), source);
    } else {
        // Handle scalar values
        set(prefix, jsonToConfigValue<j>(j), source);
    }
}

template<typename T>
void jsonToConfigValue(const nlohmann::json& j, ConfigValue& value) {
    if constexpr (std::is_same_v<T, int>) {
        value = j.get<int>();
    } else if constexpr (std::is_same_v<T, int64_t>) {
        value = j.get<int64_t>();
    } else if constexpr (std::is_same_v<T, double>) {
        value = j.get<double>();
    } else if constexpr (std::is_same_v<T, bool>) {
        value = j.get<bool>();
    } else if constexpr (std::is_same_v<T, std::string>) {
        value = j.get<std::string>();
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        value = j.get<std::vector<int>>();
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        value = j.get<std::vector<double>>();
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        value = j.get<std::vector<std::string>>();
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return saveToJson(filepath);
    } else {
        return saveToText(filepath);
    }
}

bool Config::saveToText(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert value to string
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
    }
    
    return true;
}

bool Config::saveToJson(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json j;
    
    // Convert entries to JSON
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        std::string jsonKey = key;
        
        // Convert dot notation to nested structure
        size_t pos = 0;
        nlohmann::json* current = &j;
        while (true) {
            size_t dotPos = jsonKey.find('.', pos);
            if (dotPos == std::string::npos) {
                // Last component
                configValueToJson(entry.value, (*current)[jsonKey.substr(pos)]);
                break;
            }
            
            std::string component = jsonKey.substr(pos, dotPos - pos);
            if (!current->contains(component)) {
                (*current)[component] = nlohmann::json::object();
            }
            current = &((*current)[component]);
            pos = dotPos + 1;
        }
    }
    
    // Write JSON with pretty formatting
    file << j.dump(2);
    return true;
}

template<typename T>
void configValueToJson(const ConfigValue& value, nlohmann::json& j) {
    if constexpr (std::is_same_v<T, int>) {
        j = std::get<int>(value);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        j = std::get<int64_t>(value);
    } else if constexpr (std::is_same_v<T, double>) {
        j = std::get<double>(value);
    } else if constexpr (std::is_same_v<T, bool>) {
        j = std::get<bool>(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        j = std::get<std::string>(value);
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        j = std::get<std::vector<int>>(value);
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        j = std::get<std::vector<double>>(value);
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        j = std::get<std::vector<std::string>>(value);
    }
}

// Validation methods
bool Config::validate(const std::string& key, const ConfigValue& value) const {
    // Basic validation rules
    std::string keyLower = toLower(key);
    
    // Neuron count should be positive
    if (keyLower == "neuron_count" || keyLower.find("brain.neuron_count") != std::string::npos) {
        if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
            int64_t count = std::holds_alternative<int>(value) ? std::get<int>(value) : std::get<int64_t>(value);
            if (count <= 0) {
                NLM_LOG_WARNING("Invalid neuron_count value: " + std::to_string(count) + ". Should be positive.");
                return false;
            }
        }
    }
    
    // Connection probability should be between 0 and 1
    if (keyLower == "connection_probability" || keyLower.find("brain.connection_probability") != std::string::npos) {
        if (std::holds_alternative<double>(value)) {
            double prob = std::get<double>(value);
            if (prob < 0.0 || prob > 1.0) {
                NLM_LOG_WARNING("Invalid connection_probability value: " + std::to_string(prob) + ". Should be between 0.0 and 1.0.");
                return false;
            }
        }
    }
    
    return true;
}

void Config::update(const std::string& filepath) {
    std::ifstream file(filepath);
    if (file.is_open()) {
        // Reload from file, merging with existing values
        std::filesystem::path path(filepath);
        std::string extension = path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        if (extension == ".json") {
            nlohmann::json j;
            file >> j;
            parseJsonRecursive(".", j, ConfigSource::File);
        } else {
            loadFromText(filepath);
        }
        file.close();
    }
}

// Explicit template instantiations
// Fix: Implement the template functions instead of just declaring them

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