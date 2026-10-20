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
    // Check file extension to determine parser
    std::string ext = "";
    size_t dot_pos = filepath.find_last_of('.');
    if (dot_pos != std::string::npos) {
        ext = filepath.substr(dot_pos + 1);
    }
    
    // Try JSON parser first (most common)
    if (ext == "json" || ext.empty()) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            nlohmann::json j;
            file >> j;
            
            // Clear existing entries
            pImpl->entries.clear();
            
            // Recursively parse JSON object
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                ConfigValue value;
                
                // Convert JSON value to ConfigValue
                if (it->is_string()) {
                    value = it->get<std::string>();
                } else if (it->is_boolean()) {
                    value = it->get<bool>();
                } else if (it->is_number_integer()) {
                    // Try to infer between int and int64_t
                    if (it->get<int64_t>() <= std::numeric_limits<int>::max() &&
                        it->get<int64_t>() >= std::numeric_limits<int>::min()) {
                        value = it->get<int>();
                    } else {
                        value = it->get<int64_t>();
                    }
                } else if (it->is_number_float()) {
                    value = it->get<double>();
                } else if (it->is_array()) {
                    // Convert array to vector
                    std::vector<double> vec;
                    for (const auto& item : *it) {
                        vec.push_back(item.get<double>());
                    }
                    value = vec;
                } else {
                    // For complex types, log warning and skip
                    continue;
                }
                
                set(key, value, ConfigSource::File);
            }
            
            return true;
            
        } catch (const nlohmann::json::parse_error& e) {
            return false;
        } catch (const std::exception& e) {
            return false;
        }
    }
#ifdef HAVE_YAML
    else if (ext == "yaml" || ext == "yml") {
        try {
            YAML::Loader loader(filepath);
            YAML::Node doc = loader.GetRoot();
            
            // Clear existing entries
            pImpl->entries.clear();
            
            // Recursively parse YAML node
            std::function<void(const YAML::Node&, std::string)> parseNode = 
                [&](const YAML::Node& node, std::string prefix) {
                    if (node.IsMap()) {
                        for (const auto& pair : node) {
                            std::string key = prefix.empty() ? pair.first.as<std::string>() : 
                                (prefix + "." + pair.first.as<std::string>());
                            parseNode(pair.second, key);
                        }
                    } else if (node.IsSequence()) {
                        // Convert sequence to vector<string> for now
                        std::vector<std::string> vec;
                        for (size_t i = 0; i < node.size(); ++i) {
                            vec.push_back(node[i].as<std::string>());
                        }
                        set(prefix, vec, ConfigSource::File);
                    } else {
                        // Scalar value
                        ConfigValue value;
                        if (node.IsScalar()) {
                            std::string str_val = node.as<std::string>();
                            // Try to infer type
                            if (str_val == "true" || str_val == "false") {
                                value = str_val == "true";
                            } else {
                                try {
                                    size_t pos;
                                    double double_val = std::stod(str_val, &pos);
                                    if (pos == str_val.length()) {
                                        value = double_val;
                                    } else {
                                        value = str_val;
                                    }
                                } catch (...) {
                                    value = str_val;
                                }
                            }
                        } else {
                            // Fallback to string
                            value = node.as<std::string>();
                        }
                        set(prefix, value, ConfigSource::File);
                    }
                };
            
            parseNode(doc, "");
            return true;
            
        } catch (const std::exception& e) {
            return false;
        }
    }
#endif
    else {
        // Fallback to simple key=value format
        return loadFromFileSimple(filepath);
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
