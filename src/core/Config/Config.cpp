#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <mutex>
#include <nlohmann/json.hpp>

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Register default validators
    registerValidator("neuron_count", [](const ConfigValue& value) {
        auto val = std::get<int64_t>(value);
        if (val <= 0) throw std::invalid_argument("neuron_count must be positive");
    });
    
    registerValidator("synapse_density", [](const ConfigValue& value) {
        auto val = std::get<double>(value);
        if (val < 0.0 || val > 1.0) throw std::invalid_argument("synapse_density must be between 0 and 1");
    });
    
    registerValidator("random_seed", [](const ConfigValue& value) {
        auto val = std::get<int64_t>(value);
        if (val < 0) throw std::invalid_argument("random_seed must be non-negative");
    });
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Register a validator for a configuration key
void Config::registerValidator(const std::string& key, std::function<void(const ConfigValue&)> validator) {
    pImpl->validators[key] = std::move(validator);
}

// Validate a configuration value
void Config::validate(const std::string& key, const ConfigValue& value) {
    auto it = pImpl->validators.find(key);
    if (it != pImpl->validators.end()) {
        try {
            it->second(value);
        } catch (const std::exception& e) {
            throw std::runtime_error("Validation failed for key '" + key + "': " + e.what());
        }
    }
}

// Register a validator for a configuration key
void Config::registerValidator(const std::string& key, std::function<void(const ConfigValue&)> validator) {
    pImpl->validators[key] = std::move(validator);
}

// Validate a configuration value
void Config::validate(const std::string& key, const ConfigValue& value) {
    auto it = pImpl->validators.find(key);
    if (it != pImpl->validators.end()) {
        try {
            it->second(value);
        } catch (const std::exception& e) {
            throw std::runtime_error("Validation failed for key '" + key + "': " + e.what());
        }
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Try to parse as JSON first
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json json_data;
        file >> json_data;
        
        // Clear existing entries
        clear();
        
        // Parse JSON and load values
        for (auto it = json_data.begin(); it != json_data.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            // Convert JSON type to ConfigValue
            if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                value = it->get<uint64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_array()) {
                // Convert array to vector
                if (it->empty()) {
                    value = std::vector<int>();
                } else if ((*it)[0].is_string()) {
                    std::vector<std::string> vec;
                    vec.reserve(it->size());
                    for (const auto& item : *it) {
                        vec.push_back(item.get<std::string>());
                    }
                    value = vec;
                } else if ((*it)[0].is_number_integer()) {
                    std::vector<int> vec;
                    vec.reserve(it->size());
                    for (const auto& item : *it) {
                        vec.push_back(item.get<int>());
                    }
                    value = vec;
                } else if ((*it)[0].is_number_float()) {
                    std::vector<double> vec;
                    vec.reserve(it->size());
                    for (const auto& item : *it) {
                        vec.push_back(item.get<double>());
                    }
                    value = vec;
                }
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_WARNING("JSON parse error: " + std::string(e.what()) + ". Falling back to simple format.");
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("Error loading config from " + filepath + ": " + e.what() + ". Falling back to simple format.");
    }
    
    // Fallback to simple key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int line_num = 0;
    while (std::getline(file, line)) {
        ++line_num;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value_str = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value_str.size() >= 2 && 
                ((value_str.front() == '"' && value_str.back() == '"') ||
                 (value_str.front() == '\'' && value_str.back() == '\''))) {
                value_str = value_str.substr(1, value_str.size() - 2);
            }
            
            // Convert value string to appropriate type
            bool parsed = false;
            
            // Try integer
            try {
                size_t pos_end;
                int64_t int_val = std::stoll(value_str, &pos_end);
                if (pos_end == value_str.size()) {
                    set(key, int_val, ConfigSource::File);
                    parsed = true;
                }
            } catch (...) {}
            
            // Try unsigned integer if signed failed
            if (!parsed) {
                try {
                    size_t pos_end;
                    uint64_t uint_val = std::stoull(value_str, &pos_end);
                    if (pos_end == value_str.size()) {
                        set(key, uint_val, ConfigSource::File);
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try float if integer failed
            if (!parsed) {
                try {
                    size_t pos_end;
                    double float_val = std::stod(value_str, &pos_end);
                    if (pos_end == value_str.size()) {
                        set(key, float_val, ConfigSource::File);
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try boolean
            if (!parsed) {
                std::string lower_val = toLower(value_str);
                if (lower_val == "true") {
                    set(key, true, ConfigSource::File);
                    parsed = true;
                } else if (lower_val == "false") {
                    set(key, false, ConfigSource::File);
                    parsed = true;
                }
            }
            
            // If still not parsed, treat as string
            if (!parsed) {
                set(key, value_str, ConfigSource::File);
            }
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(line_num) + ": " + line);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Invalid command line argument (missing value): " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            if (i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Invalid command line argument (missing value): " + arg);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Create parent directories if they don't exist
        std::filesystem::path path(filepath);
        std::filesystem::create_directories(path.parent_path());
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
            return false;
        }
        
        // Write JSON format
        nlohmann::json json_data;
        
        for (const auto& entry : pImpl->entries) {
            // Convert ConfigValue to JSON
            std::visit([&](const auto& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    json_data[entry.key] = value;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    json_data[entry.key] = value;
                } else if constexpr (std::is_same_v<T, uint64_t>) {
                    json_data[entry.key] = value;
                } else if constexpr (std::is_same_v<T, double>) {
                    json_data[entry.key] = value;
                } else if constexpr (std::is_same_v<T, bool>) {
                    json_data[entry.key] = value;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    json_data[entry.key] = value;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    json_data[entry.key] = value;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    json_data[entry.key] = value;
                }
            }, entry.value);
        }
        
        // Write JSON to file with pretty formatting
        file << std::setw(2) << json_data;
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving config to " + filepath + ": " + e.what());
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Validate the value
    validate(key, value);
}

bool Config::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
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
