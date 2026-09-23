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
    // Try to parse as JSON first
    {
        std::ifstream file(filepath);
        if (file.is_open()) {
            try {
                nlohmann::json j;
                file >> j;
                // Convert JSON to config entries
                for (auto it = j.begin(); it != j.end(); ++it) {
                    std::string key = it.key();
                    ConfigValue value;
                    
                    if (it->is_number_integer()) {
                        value = it->get<int64_t>();
                    } else if (it->is_number_unsigned()) {
                        value = static_cast<int64_t>(it->get<uint64_t>());
                    } else if (it->is_number_float()) {
                        value = it->get<double>();
                    } else if (it->is_boolean()) {
                        value = it->get<bool>();
                    } else if (it->is_string()) {
                        value = it->get<std::string>();
                    } else if (it->is_array()) {
                        // Check array element type
                        if (!it->empty()) {
                            if (it->front().is_number_integer()) {
                                std::vector<int64_t> arr;
                                for (const auto& elem : *it) {
                                    arr.push_back(elem.get<int64_t>());
                                }
                                value = arr;
                            } else if (it->front().is_number_float()) {
                                std::vector<double> arr;
                                for (const auto& elem : *it) {
                                    arr.push_back(elem.get<double>());
                                }
                                value = arr;
                            } else if (it->front().is_string()) {
                                std::vector<std::string> arr;
                                for (const auto& elem : *it) {
                                    arr.push_back(elem.get<std::string>());
                                }
                                value = arr;
                            }
                        }
                    }
                    
                    set(key, value, ConfigSource::File);
                }
                return true;
            } catch (const std::exception&) {
                // Fall back to simple format if JSON parsing fails
            }
        }
    }
    
    // Try to parse as YAML
    {
        std::ifstream file(filepath);
        if (file.is_open()) {
            try {
                YAML::Node doc = YAML::LoadFile(filepath);
                // Convert YAML to config entries
                for (const auto& node : doc) {
                    std::string key = node.first.as<std::string>();
                    ConfigValue value;
                    
                    if (node.second.IsScalar()) {
                        if (node.second.IsInteger()) {
                            value = node.second.as<int64_t>();
                        } else if (node.second.IsFloat()) {
                            value = node.second.as<double>();
                        } else if (node.second.IsBool()) {
                            value = node.second.as<bool>();
                        } else if (node.second.IsString()) {
                            value = node.second.as<std::string>();
                        }
                    } else if (node.second.IsSequence()) {
                        // Check sequence element type
                        if (!node.second.empty()) {
                            if (node.second[0].IsInteger()) {
                                std::vector<int64_t> arr;
                                for (const auto& elem : node.second) {
                                    arr.push_back(elem.as<int64_t>());
                                }
                                value = arr;
                            } else if (node.second[0].IsFloat()) {
                                std::vector<double> arr;
                                for (const auto& elem : node.second) {
                                    arr.push_back(elem.as<double>());
                                }
                                value = arr;
                            } else if (node.second[0].IsString()) {
                                std::vector<std::string> arr;
                                for (const auto& elem : node.second) {
                                    arr.push_back(elem.as<std::string>());
                                }
                                value = arr;
                            }
                        }
                    }
                    
                    set(key, value, ConfigSource::File);
                }
                return true;
            } catch (const std::exception&) {
                // Fall back to simple format if YAML parsing fails
            }
        }
    }
    
    // Fallback to simple key=value format
    return loadFromFileSimple(filepath);
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
        // Handle -key value format (without equals)
        else if (arg[0] == '-' && i + 1 < argc && 
                 argv[i + 1][0] != '-') {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle boolean flags like --enable_feature
        else if (arg.substr(0, 2) == "--" && arg.find('=') == std::string::npos) {
            std::string key = arg.substr(2);
            // Convert to boolean true
            set(key, true, ConfigSource::CommandLine);
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
        file << entry.key << " = ";
        
        // Write the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << std::setprecision(6) << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            }
        }, entry.value);
        file << "\n\n";
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
