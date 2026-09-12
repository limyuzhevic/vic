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
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
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

// Configuration validation
bool Config::validate() const {
    bool isValid = true;
    
    // Check for duplicate keys
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        // Check for numeric range violations
        if (entry.key == "neuron_count") {
            if (auto val = get<size_t>(entry.key)) {
                if (*val == 0 || *val > 100000) {
                    NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(*val) + ". Must be 1-100000");
                    isValid = false;
                }
            }
        } else if (entry.key == "region_count") {
            if (auto val = get<size_t>(entry.key)) {
                if (*val == 0) {
                    NLM_LOG_ERROR("Invalid region_count: " + std::to_string(*val) + ". Must be at least 1");
                    isValid = false;
                }
            }
        } else if (entry.key == "simulation_timestep") {
            if (auto val = get<double>(entry.key)) {
                if (*val <= 0.0 || *val > 1.0) {
                    NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(*val) + ". Must be 0 < value <= 1.0");
                    isValid = false;
                }
            }
        } else if (entry.key == "connection_probability") {
            if (auto val = get<float>(entry.key)) {
                if (*val < 0.0f || *val > 1.0f) {
                    NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(*val) + ". Must be 0.0-1.0");
                    isValid = false;
                }
            }
        } else if (entry.key == "random_seed") {
            if (auto val = get<uint64_t>(entry.key)) {
                if (*val == 0) {
                    NLM_LOG_ERROR("Invalid random_seed: " + std::to_string(*val) + ". Must not be 0");
                    isValid = false;
                }
            }
        }
        
        keys.push_back(entry.key);
    }
    
    // Check for duplicate keys
    std::sort(keys.begin(), keys.end());
    for (size_t i = 1; i < keys.size(); ++i) {
        if (keys[i] == keys[i-1]) {
            NLM_LOG_ERROR("Duplicate configuration key: " + keys[i]);
            isValid = false;
        }
    }
    
    return isValid;
}

std::string Config::getValidationError() const {
    std::ostringstream oss;
    
    // Check required keys
    std::vector<std::string> requiredKeys = {"neuron_count", "region_count", "simulation_timestep"};
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            oss << "Missing required configuration key: " << key << "\n";
        }
    }
    
    // Check for duplicate keys
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    std::sort(keys.begin(), keys.end());
    for (size_t i = 1; i < keys.size(); ++i) {
        if (keys[i] == keys[i-1]) {
            oss << "Duplicate configuration key: " << keys[i] << "\n";
        }
    }
    
    if (oss.str().empty()) {
        oss << "Configuration is valid\n";
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
