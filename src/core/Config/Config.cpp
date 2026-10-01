#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <numeric>  // For std::numeric_limits
#include <cmath>     // For std::isnan, std::isinf

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
#ifdef HAS_JSON
    // Try JSON first
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        // Process JSON object
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Try to convert based on type
            if (it->is_number_integer()) {
                set(key, it->get<int>(), ConfigSource::File);
            } else if (it->is_number_unsigned()) {
                set(key, static_cast<size_t>(it->get<uint64_t>()), ConfigSource::File);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File);
            } else if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File);
            } else if (it->is_array()) {
                // Convert array to vector of strings (for now)
                std::vector<std::string> vec;
                for (const auto& item : *it) {
                    vec.push_back(item.get<std::string>());
                }
                set(key, vec, ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Config loaded from JSON file: " + filepath);
        return true;
    } catch (const std::exception&) {
        // JSON parsing failed, try YAML or fallback
    }
#endif

#ifdef HAS_YAML
    // Try YAML format
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        YAML::Node doc = YAML::Load(file);
        
        // Process YAML document
        for (const auto& pair : doc) {
            std::string key = pair.first.as<std::string>();
            
            if (pair.second.IsScalar()) {
                if (pair.second.IsInteger()) {
                    set(key, pair.second.as<int>(), ConfigSource::File);
                } else if (pair.second.IsFloat()) {
                    set(key, pair.second.as<double>(), ConfigSource::File);
                } else if (pair.second.IsBoolean()) {
                    set(key, pair.second.as<bool>(), ConfigSource::File);
                } else {
                    set(key, pair.second.as<std::string>(), ConfigSource::File);
                }
            } else if (pair.second.IsSequence()) {
                std::vector<std::string> vec;
                for (const auto& item : pair.second) {
                    vec.push_back(item.as<std::string>());
                }
                set(key, vec, ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Config loaded from YAML file: " + filepath);
        return true;
    } catch (const std::exception&) {
        // YAML parsing failed, fallback to simple format
    }
#endif

    // Fallback to simple key=value format (original implementation)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        
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
            
            // Try to convert value based on content
            if (value.find('.') != std::string::npos) {
                try {
                    double num = std::stod(value);
                    set(key, num, ConfigSource::File);
                } catch (...) {
                    set(key, value, ConfigSource::File);
                }
            } else {
                try {
                    int num = std::stoi(value);
                    set(key, num, ConfigSource::File);
                } catch (...) {
                    set(key, value, ConfigSource::File);
                }
            }
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Config loaded from file (simple format): " + filepath);
    return true;

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
    // Try JSON format first (preferred)
#ifdef HAS_JSON
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&j, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    j[entry.key] = arg;
                }
            }, entry.value);
        }
        
        file << std::setw(2) << j;
        NLM_LOG_INFO("Config saved to JSON file: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("JSON save failed, trying YAML: " + std::string(e.what()));
    }
#endif

#ifdef HAS_YAML
    // Try YAML format
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            
            std::visit([&emitter](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    emitter << static_cast<int>(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    emitter << YAML::DoubleQuoted << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : arg) {
                        emitter << YAML::DoubleQuoted << item;
                    }
                    emitter << YAML::EndSeq;
                }
            }, entry.value);
        }
        
        emitter << YAML::EndMap;
        file << emitter.c_str();
        
        NLM_LOG_INFO("Config saved to YAML file: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("YAML save failed, falling back to simple format: " + std::string(e.what()));
    }
#endif

    // Fallback to simple format
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Config saved to file (simple format): " + filepath);
    return true;

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        T value = std::get<T>(it->value);
        
        // Validate based on type
        if constexpr (std::is_same_v<T, int>) {
            if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max()) {
                NLM_LOG_WARNING("Config value out of range for int key: " + key);
                return std::nullopt;
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(value) || std::isinf(value)) {
                NLM_LOG_WARNING("Config value is NaN or infinite for double key: " + key);
                return std::nullopt;
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (value.empty()) {
                NLM_LOG_WARNING("Config value is empty string for key: " + key);
            }
        }
        
        return value;
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Type mismatch for key: " + key + " (expected " + typeid(T).name() + ")");
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set config with empty key");
        return;
    }
    
    // Sanitize key (remove leading/trailing whitespace)
    std::string sanitizedKey = trim(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&sanitizedKey](const ConfigEntry& e) { return e.key == sanitizedKey; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Config updated: " + sanitizedKey);
    } else {
        // Add new entry
        pImpl->entries.emplace_back(sanitizedKey, value, source);
        NLM_LOG_DEBUG("Config added: " + sanitizedKey);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Validate integer range
    if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max()) {
        NLM_LOG_ERROR("Config value out of range for int key: " + key);
        return;
    }
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Validate double value
    if (std::isnan(value) || std::isinf(value)) {
        NLM_LOG_ERROR("Config value is NaN or infinite for double key: " + key);
        return;
    }
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
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
