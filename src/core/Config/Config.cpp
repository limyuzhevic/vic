#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <type_traits>
#include <ctime>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

namespace {

// Helper function to parse JSON value to ConfigValue
ConfigValue jsonToConfigValue(const json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_unsigned()) {
        return j.get<uint64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Check if array contains strings
        bool allStrings = std::all_of(j.begin(), j.end(), [](const json& item) {
            return item.is_string();
        });
        if (allStrings) {
            std::vector<std::string> vec;
            vec.reserve(j.size());
            for (const auto& item : j) {
                vec.push_back(item.get<std::string>());
            }
            return vec;
        }
        
        // Check if array contains integers
        bool allInts = std::all_of(j.begin(), j.end(), [](const json& item) {
            return item.is_number_integer();
        });
        if (allInts) {
            std::vector<int> vec;
            vec.reserve(j.size());
            for (const auto& item : j) {
                vec.push_back(item.get<int>());
            }
            return vec;
        }
        
        // Check if array contains floats
        bool allFloats = std::all_of(j.begin(), j.end(), [](const json& item) {
            return item.is_number_float();
        });
        if (allFloats) {
            std::vector<double> vec;
            vec.reserve(j.size());
            for (const auto& item : j) {
                vec.push_back(item.get<double>());
            }
            return vec;
        }
        
        // Default to vector<double> for mixed arrays
        std::vector<double> vec;
        vec.reserve(j.size());
        for (const auto& item : j) {
            vec.push_back(item.get<double>());
        }
        return vec;
    } else {
        // Default to string for other types
        return j.get<std::string>();
    }
}

// Helper function to parse YAML value to ConfigValue
ConfigValue yamlToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        // Check type of scalar
        if (node.IsString()) {
            return node.as<std::string>();
        } else if (node.IsInt()) {
            return node.as<int64_t>();
        } else if (node.IsDouble()) {
            return node.as<double>();
        } else if (node.IsBool()) {
            return node.as<bool>();
        } else {
            return node.as<std::string>(); // Default to string
        }
    } else if (node.IsSequence()) {
        // Check if sequence contains strings
        bool allStrings = std::all_of(node.begin(), node.end(), [](const YAML::Node& item) {
            return item.IsScalar() && item.IsString();
        });
        if (allStrings) {
            std::vector<std::string> vec;
            vec.reserve(node.size());
            for (const auto& item : node) {
                vec.push_back(item.as<std::string>());
            }
            return vec;
        }
        
        // Check if sequence contains integers
        bool allInts = std::all_of(node.begin(), node.end(), [](const YAML::Node& item) {
            return item.IsScalar() && item.IsInt();
        });
        if (allInts) {
            std::vector<int> vec;
            vec.reserve(node.size());
            for (const auto& item : node) {
                vec.push_back(item.as<int>());
            }
            return vec;
        }
        
        // Check if sequence contains floats
        bool allFloats = std::all_of(node.begin(), node.end(), [](const YAML::Node& item) {
            return item.IsScalar() && item.IsDouble();
        });
        if (allFloats) {
            std::vector<double> vec;
            vec.reserve(node.size());
            for (const auto& item : node) {
                vec.push_back(item.as<double>());
            }
            return vec;
        }
        
        // Default to vector<double> for mixed sequences
        std::vector<double> vec;
        vec.reserve(node.size());
        for (const auto& item : node) {
            if (item.IsScalar()) {
                if (item.IsInt()) {
                    vec.push_back(item.as<int>());
                } else if (item.IsDouble()) {
                    vec.push_back(item.as<double>());
                } else {
                    vec.push_back(static_cast<double>(std::stod(item.as<std::string>())));
                }
            }
        }
        return vec;
    } else {
        // Default to string for other types
        return node.as<std::string>();
    }
}

// Helper function to convert ConfigValue to JSON
void configValueToJson(const ConfigValue& value, json& j) {
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            j = val;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            j = val;
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            j = val;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = val;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = val;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = val;
        }
    }, value);
}

// Helper function to convert ConfigValue to YAML
void configValueToYaml(const ConfigValue& value, YAML::Emitter& out) {
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            out << val;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            out << static_cast<uint64_t>(val);
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            out << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            out << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            out << YAML::DoubleQuotedString(val);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            out << YAML::BeginSeq;
            for (const auto& item : val) {
                out << item;
            }
            out << YAML::EndSeq;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            out << YAML::BeginSeq;
            for (const auto& item : val) {
                out << item;
            }
            out << YAML::EndSeq;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            out << YAML::BeginSeq;
            for (const auto& item : val) {
                out << YAML::DoubleQuotedString(item);
            }
            out << YAML::EndSeq;
        }
    }, value);
}

} // namespace

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to detect file type based on extension
    std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
    
    try {
        if (ext == "json" || ext == "jsonc" || ext == "jsonl") {
            // Parse as JSON
            json j;
            file >> j;
            
            // Clear existing entries
            pImpl->entries.clear();
            
            // Recursively parse JSON object
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                ConfigValue value = jsonToConfigValue(*it);
                set(key, value, ConfigSource::File);
                // Store description from comments if available
                auto descIt = j.find("# " + key);
                if (descIt != j.end()) {
                    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                              [&key](const ConfigEntry& e) { return e.key == key; });
                    if (entryIt != pImpl->entries.end()) {
                        entryIt->description = descIt.value();
                    }
                }
            }
        } else if (ext == "yml" || ext == "yaml") {
            // Parse as YAML
            YAML::Node doc = YAML::Load(file);
            
            // Clear existing entries
            pImpl->entries.clear();
            
            // Recursively parse YAML document
            for (const auto& pair : doc) {
                std::string key = pair.first.as<std::string>();
                ConfigValue value = yamlToConfigValue(pair.second);
                set(key, value, ConfigSource::File);
                // Store description from comments if available
                auto descIt = doc.Find(key);
                if (descIt) {
                    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                              [&key](const ConfigEntry& e) { return e.key == key; });
                    if (entryIt != pImpl->entries.end()) {
                        entryIt->description = descIt->as<std::string>();
                    }
                }
            }
        } else {
            // Fall back to simple key=value format
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
                    
                    // Try to parse as different types
                    bool parsed = false;
                    
                    // Try boolean
                    if (!parsed && (value == "true" || value == "false")) {
                        bool boolVal = (value == "true");
                        set(key, boolVal, ConfigSource::File);
                        parsed = true;
                    }
                    
                    // Try integer
                    if (!parsed) {
                        try {
                            size_t pos;
                            int64_t intVal = std::stoll(value, &pos);
                            if (pos == value.size()) {
                                set(key, intVal, ConfigSource::File);
                                parsed = true;
                            }
                        } catch (...) {}
                    }
                    
                    // Try unsigned integer
                    if (!parsed) {
                        try {
                            size_t pos;
                            uint64_t uintVal = std::stoull(value, &pos);
                            if (pos == value.size()) {
                                set(key, uintVal, ConfigSource::File);
                                parsed = true;
                            }
                        } catch (...) {}
                    }
                    
                    // Try float
                    if (!parsed) {
                        try {
                            size_t pos;
                            double floatVal = std::stod(value, &pos);
                            if (pos == value.size()) {
                                set(key, floatVal, ConfigSource::File);
                                parsed = true;
                            }
                        } catch (...) {}
                    }
                    
                    // If not parsed as any type, store as string
                    if (!parsed) {
                        set(key, value, ConfigSource::File);
                    }
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        // If JSON/YAML parsing fails, fall back to simple format
        file.clear();
        file.seekg(0);
        return loadFromFile(filepath);
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

// Helper function to write configuration entry
    void writeConfigEntry(const ConfigEntry& entry, json& j) const {
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, uint64_t>) {
                j[entry.key] = static_cast<uint64_t>(val);
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j[entry.key] = val;
            }
        }, entry.value);
        
        // Add comment if description exists
        if (!entry.description.empty()) {
            j["# " + entry.key] = entry.description;
        }
    }

    // Helper function to write configuration entry to YAML
    void writeConfigEntry(const ConfigEntry& entry, YAML::Emitter& out) const {
        out << YAML::Key << entry.key;
        out << YAML::Value;
        
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                out << val;
            } else if constexpr (std::is_same_v<T, uint64_t>) {
                out << static_cast<uint64_t>(val);
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                out << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                out << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                out << YAML::DoubleQuotedString(val);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                out << YAML::BeginSeq;
                for (const auto& item : val) {
                    out << item;
                }
                out << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                out << YAML::BeginSeq;
                for (const auto& item : val) {
                    out << item;
                }
                out << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                out << YAML::BeginSeq;
                for (const auto& item : val) {
                    out << YAML::DoubleQuotedString(item);
                }
                out << YAML::EndSeq;
            }
        }, entry.value);
        
        // Add comment if description exists
        if (!entry.description.empty()) {
            out << YAML::Comment(entry.description);
        }
    }

    bool Config::saveToFile(const std::string& filepath) const {
        return saveToFile(filepath, "json");
    }
    
    bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
        // Determine output stream based on format
        if (format == "json" || format == "jsonc") {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            json j;
            
            // Write all configuration entries
            for (const auto& entry : pImpl->entries) {
                writeConfigEntry(entry, j);
            }
            
            // Write to file with indentation
            file << j.dump(2);
            
            return true;
        } else if (format == "yml" || format == "yaml") {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            YAML::Emitter out;
            out << YAML::BeginMap;
            
            // Write all configuration entries
            for (const auto& entry : pImpl->entries) {
                writeConfigEntry(entry, out);
            }
            
            out << YAML::EndMap;
            
            file << out.c_str();
            
            return true;
        } else {
            // Fall back to simple key=value format
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                
                std::visit([&](const auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                        file << entry.key << " = " << val << "\n";
                    } else if constexpr (std::is_same_v<T, uint64_t>) {
                        file << entry.key << " = " << static_cast<uint64_t>(val) << "\n";
                    } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                        file << entry.key << " = " << val << "\n";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << entry.key << " = " << (val ? "true" : "false") << "\n";
                    } else if constexpr (std::is_same_v<T, std::string>) {
                        file << entry.key << " = \"" << val << "\"\n";
                    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        file << entry.key << " = [";
                        for (size_t i = 0; i < val.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << val[i];
                        }
                        file << "]\n";
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        file << entry.key << " = [";
                        for (size_t i = 0; i < val.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << val[i];
                        }
                        file << "]\n";
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << entry.key << " = [";
                        for (size_t i = 0; i < val.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << "\"" << val[i] << "\"";
                        }
                        file << "]\n";
                    }
                }, entry.value);
                
                file << "\n";
            }
            
            return true;
        }
    }

    // Helper function to write configuration entry
    void Config::writeConfigEntry(const ConfigEntry& entry, json& j) const {
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, uint64_t>) {
                j[entry.key] = static_cast<uint64_t>(val);
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j[entry.key] = val;
            }
        }, entry.value);
        
        // Add comment if description exists
        if (!entry.description.empty()) {
            j["# " + entry.key] = entry.description;
        }
    }

    // Helper function to write configuration entry to YAML
    void Config::writeConfigEntry(const ConfigEntry& entry, YAML::Emitter& out) const {
        out << YAML::Key << entry.key;
        out << YAML::Value;
        
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                out << val;
            } else if constexpr (std::is_same_v<T, uint64_t>) {
                out << static_cast<uint64_t>(val);
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                out << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                out << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                out << YAML::DoubleQuotedString(val);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                out << YAML::BeginSeq;
                for (const auto& item : val) {
                    out << item;
                }
                out << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                out << YAML::BeginSeq;
                for (const auto& item : val) {
                    out << item;
                }
                out << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                out << YAML::BeginSeq;
                for (const auto& item : val) {
                    out << YAML::DoubleQuotedString(item);
                }
                out << YAML::EndSeq;
            }
        }, entry.value);
        
        // Add comment if description exists
        if (!entry.description.empty()) {
            out << YAML::Comment(entry.description);
        }
    }

    bool Config::saveToFile(const std::string& filepath) const {
        return saveToFile(filepath, "json");
    }
    
    bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
        // Determine output stream based on format
        if (format == "json" || format == "jsonc") {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            json j;
            
            // Write all configuration entries
            for (const auto& entry : pImpl->entries) {
                writeConfigEntry(entry, j);
            }
            
            // Write to file with indentation
            file << j.dump(2);
            
            return true;
        } else if (format == "yml" || format == "yaml") {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            YAML::Emitter out;
            out << YAML::BeginMap;
            
            // Write all configuration entries
            for (const auto& entry : pImpl->entries) {
                writeConfigEntry(entry, out);
            }
            
            out << YAML::EndMap;
            
            file << out.c_str();
            
            return true;
        } else {
            // Fall back to simple key=value format
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                
                std::visit([&](const auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                        file << entry.key << " = " << val << "\n";
                    } else if constexpr (std::is_same_v<T, uint64_t>) {
                        file << entry.key << " = " << static_cast<uint64_t>(val) << "\n";
                    } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                        file << entry.key << " = " << val << "\n";
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << entry.key << " = " << (val ? "true" : "false") << "\n";
                    } else if constexpr (std::is_same_v<T, std::string>) {
                        file << entry.key << " = \"" << val << "\"\n";
                    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                        file << entry.key << " = [";
                        for (size_t i = 0; i < val.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << val[i];
                        }
                        file << "]\n";
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        file << entry.key << " = [";
                        for (size_t i = 0; i < val.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << val[i];
                        }
                        file << "]\n";
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << entry.key << " = [";
                        for (size_t i = 0; i < val.size(); ++i) {
                            if (i > 0) file << ", ";
                            file << "\"" << val[i] << "\"";
                        }
                        file << "]\n";
                    }
                }, entry.value);
                
                file << "\n";
            }
            
            return true;
        }
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
