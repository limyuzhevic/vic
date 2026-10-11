#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <string>
#include <regex>

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

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file type based on extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAML(filepath);
    } else {
        // Try to detect format by reading first few lines
        file.clear();
        file.seekg(0, std::ios::beg);
        
        std::string firstLine, secondLine;
        std::getline(file, firstLine);
        std::getline(file, secondLine);
        
        file.clear();
        file.seekg(0, std::ios::beg);
        
        // Simple detection
        if (firstLine.find("{") != std::string::npos || 
            firstLine.find("[") != std::string::npos ||
            secondLine.find("{") != std::string::npos ||
            secondLine.find("[") != std::string::npos) {
            return loadFromJSON(filepath);
        } else {
            return loadFromPlain(filepath);
        }
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        if (!j.is_object()) {
            return false;
        }
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            const json& value = it.value();
            
            ConfigValue configValue;
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int>();
            } else if (value.is_number_unsigned()) {
                configValue = value.get<uint64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Handle arrays of different types
                if (!value.empty()) {
                    if (std::all_of(value.begin(), value.end(), 
                                   [](const json& v) { return v.is_number_integer(); })) {
                        std::vector<int> arr;
                        for (const auto& v : value) arr.push_back(v.get<int>());
                        configValue = arr;
                    } else if (std::all_of(value.begin(), value.end(), 
                                          [](const json& v) { return v.is_number_float(); })) {
                        std::vector<double> arr;
                        for (const auto& v : value) arr.push_back(v.get<double>());
                        configValue = arr;
                    } else if (std::all_of(value.begin(), value.end(), 
                                          [](const json& v) { return v.is_string(); })) {
                        std::vector<std::string> arr;
                        for (const auto& v : value) arr.push_back(v.get<std::string>());
                        configValue = arr;
                    }
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        YAML::Node doc = YAML::LoadFile(filepath);
        
        if (!doc.IsMap()) {
            return false;
        }
        
        for (const auto& kv : doc) {
            std::string key = kv.first.as<std::string>();
            const YAML::Node& value = kv.second;
            
            ConfigValue configValue;
            if (value.Type() == YAML::NodeType::Scalar) {
                std::string str = value.as<std::string>();
                // Try to convert to numeric/bool
                if (str == "true" || str == "false") {
                    configValue = str == "true";
                } else {
                    try {
                        size_t pos;
                        int intVal = std::stoi(str, &pos);
                        if (pos == str.length()) {
                            configValue = intVal;
                        } else {
                            double doubleVal = std::stod(str, &pos);
                            if (pos == str.length()) {
                                configValue = doubleVal;
                            } else {
                                configValue = str;
                            }
                        }
                    } catch (...) {
                        configValue = str;
                    }
                }
            } else if (value.Type() == YAML::NodeType::Sequence) {
                if (value.size() > 0) {
                    // Try to infer array type
                    bool allInt = true, allDouble = true, allString = true;
                    for (size_t i = 0; i < value.size(); ++i) {
                        if (value[i].Type() != YAML::NodeType::Scalar) {
                            allInt = allDouble = allString = false;
                            break;
                        }
                        std::string str = value[i].as<std::string>();
                        try {
                            size_t pos;
                            std::stoi(str, &pos);
                            if (pos != str.length()) allInt = false;
                        } catch (...) {
                            allInt = false;
                        }
                        try {
                            size_t pos;
                            std::stod(str, &pos);
                            if (pos != str.length()) allDouble = false;
                        } catch (...) {
                            allDouble = false;
                        }
                        allString = false; // Scalar is string
                    }
                    
                    if (allInt) {
                        std::vector<int> arr;
                        for (size_t i = 0; i < value.size(); ++i) {
                            arr.push_back(value[i].as<int>());
                        }
                        configValue = arr;
                    } else if (allDouble) {
                        std::vector<double> arr;
                        for (size_t i = 0; i < value.size(); ++i) {
                            arr.push_back(value[i].as<double>());
                        }
                        configValue = arr;
                    } else {
                        std::vector<std::string> arr;
                        for (size_t i = 0; i < value.size(); ++i) {
                            arr.push_back(value[i].as<std::string>());
                        }
                        configValue = arr;
                    }
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromPlain(const std::string& filepath) {
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
        
        // Parse key=value pairs
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
    
    // Determine file type based on extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return saveToJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return saveToYAML(filepath);
    } else {
        return saveToPlain(filepath);
    }
}

bool Config::saveToPlain(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "["; bool first = true;
                for (const auto& val : arg) {
                    if (!first) file << ", ";
                    file << val;
                    first = false;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "["; bool first = true;
                for (const auto& val : arg) {
                    if (!first) file << ", ";
                    file << val;
                    first = false;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "["; bool first = true;
                for (const auto& val : arg) {
                    if (!first) file << ", ";
                    file << "\"" << val << "\"";
                    first = false;
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

bool Config::saveToJSON(const std::string& filepath) const {
    try {
        json j;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&j, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
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
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(2);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginDoc;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            
            std::visit([&emitter](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, int>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& val : arg) {
                        emitter << val;
                    }
                    emitter << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& val : arg) {
                        emitter << val;
                    }
                    emitter << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& val : arg) {
                        emitter << val;
                    }
                    emitter << YAML::EndSeq;
                }
            }, entry.value);
        }
        
        emitter << YAML::EndMap;
        emitter << YAML::EndDoc;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << emitter.c_str();
        return true;
    } catch (const std::exception&) {
        return false;
    }
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

// Advanced Config Commands Implementation

std::vector<std::string> Config::listFiles(const std::string& directory) const {
    std::vector<std::string> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.size() >= 4) {
                    std::string ext = filename.substr(filename.size() - 4);
                    if (ext == ".cfg" || ext == ".json" || ext == ".yaml" || ext == ".yml") {
                        files.push_back(filename);
                    }
                }
            }
        }
    } catch (const std::exception&) {
    }
    return files;
}

bool Config::validateFile(const std::string& filepath) const {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;
        
        std::string ext = getFileExtension(filepath);
        if (ext == "json") {
            json j;
            file >> j;
            return j.is_object();
        } else if (ext == "yaml" || ext == "yml") {
            YAML::Node doc = YAML::LoadFile(filepath);
            return doc.IsMap();
        }
        return false; // Plain format needs validation
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::convertFile(const std::string& inputPath, const std::string& outputPath) const {
    try {
        std::string ext1 = getFileExtension(inputPath);
        std::string ext2 = getFileExtension(outputPath);
        
        if (ext1 == "json" && ext2 == "yaml") {
            // JSON to YAML
            std::ifstream in(inputPath);
            json j;
            in >> j;
            
            YAML::Emitter emitter;
            emitter << YAML::BeginDoc;
            emitter << j.get<YAML::Node>();
            emitter << YAML::EndDoc;
            
            std::ofstream out(outputPath);
            out << emitter.c_str();
            return true;
        } else if (ext1 == "yaml" && ext2 == "json") {
            // YAML to JSON
            YAML::Node doc = YAML::LoadFile(inputPath);
            json j = doc;
            
            std::ofstream out(outputPath);
            out << j.dump(2);
            return true;
        }
    } catch (const std::exception&) {
    }
    return false;
}

bool Config::mergeFiles(const std::vector<std::string>& filePaths, const std::string& outputPath) const {
    nlm::Config mergedConfig;
    
    for (const auto& filepath : filePaths) {
        nlm::Config tempConfig;
        if (tempConfig.loadFromFile(filepath)) {
            auto keys = tempConfig.getKeys();
            for (const auto& key : keys) {
                mergedConfig.set(key, tempConfig.getOr<std::string>(key, ""));
            }
        }
    }
    
    return mergedConfig.saveToFile(outputPath);
}

std::string Config::diffFiles(const std::string& path1, const std::string& path2) const {
    nlm::Config config1, config2;
    config1.loadFromFile(path1);
    config2.loadFromFile(path2);
    
    std::ostringstream diff;
    
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    // Keys in config1 but not in config2
    for (const auto& key : keys1) {
        if (!config2.has(key)) {
            diff << "- " << key << " (removed in " << path2 << ")\n";
        } else {
            // Compare values
            auto val1 = config1.get<std::string>(key);
            auto val2 = config2.get<std::string>(key);
            if (val1 != val2) {
                diff << "* " << key << "\n";
                diff << "  " << path1 << ": " << (val1 ? *val1 : "") << "\n";
                diff << "  " << path2 << ": " << (val2 ? *val2 : "") << "\n";
            }
        }
    }
    
    // Keys in config2 but not in config1
    for (const auto& key : keys2) {
        if (!config1.has(key)) {
            diff << "+ " << key << " (added in " << path2 << ")\n";
        }
    }
    
    return diff.str();
}

std::string Config::backupFile(const std::string& filepath) const {
    if (!validateFile(filepath)) return "";
    
    std::filesystem::path path(filepath);
    std::string timestamp = std::to_string(std::time(nullptr));
    std::string backupPath = path.parent_path().string() + "/backup_" + 
                           path.stem().string() + "_" + timestamp + path.extension().string();
    
    std::filesystem::copy_file(path, backupPath);
    return backupPath;
}

nlm::Config Config::extractKeys(const std::vector<std::string>& keys) const {
    nlm::Config extracted;
    for (const auto& key : keys) {
        if (has(key)) {
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            if (it != pImpl->entries.end()) {
                extracted.set(it->key, it->value, it->source);
            }
        }
    }
    return extracted;
}

nlm::Config Config::generateTemplate(const std::string& templateType) const {
    nlm::Config templateConfig;
    
    if (templateType == "neural") {
        templateConfig.set("neuron_count", "1000");
        templateConfig.set("region_count", "10");
        templateConfig.set("stdp_ltp_weight", "0.02");
        templateConfig.set("stdp_ltd_weight", "0.015");
        templateConfig.set("connection_probability", "0.15");
        templateConfig.set("simulation_timestep", "0.001");
    } else if (templateType == "simple") {
        templateConfig.set("neuron_count", "100");
        templateConfig.set("region_count", "1");
        templateConfig.set("connection_probability", "0.1");
    }
    
    return templateConfig;
}

std::string Config::getStatistics() const {
    std::ostringstream stats;
    stats << "Configuration Statistics:\n";
    stats << "  Total entries: " << pImpl->entries.size() << "\n";
    
    std::map<std::string, int> typeCounts;
    std::map<ConfigSource, int> sourceCounts;
    
    for (const auto& entry : pImpl->entries) {
        // Type count
        std::string type = "unknown";
        std::visit([&type](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) type = "int";
            else if constexpr (std::is_same_v<T, int64_t>) type = "int64_t";
            else if constexpr (std::is_same_v<T, double>) type = "double";
            else if constexpr (std::is_same_v<T, bool>) type = "bool";
            else if constexpr (std::is_same_v<T, std::string>) type = "string";
            else if constexpr (std::is_same_v<T, std::vector<int>>) type = "int[]";
            else if constexpr (std::is_same_v<T, std::vector<double>>) type = "double[]";
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) type = "string[]";
        }, entry.value);
        typeCounts[type]++;
        
        // Source count
        sourceCounts[entry.source]++;
    }
    
    stats << "  By type: ";
    bool first = true;
    for (const auto& pair : typeCounts) {
        if (!first) stats << ", ";
        stats << pair.first << "=" << pair.second;
        first = false;
    }
    stats << "\n";
    
    stats << "  By source: ";
    first = true;
    for (const auto& pair : sourceCounts) {
        if (!first) stats << ", ";
        stats << static_cast<int>(pair.first) << "=" << pair.second;
        first = false;
    }
    stats << "\n";
    
    return stats.str();
}

std::vector<std::string> Config::searchKeys(const std::string& pattern) const {
    std::vector<std::string> matches;
    std::regex regex(pattern);
    
    for (const auto& entry : pImpl->entries) {
        if (std::regex_search(entry.key, regex)) {
            matches.push_back(entry.key);
        }
    }
    
    return matches;
}

bool Config::migrateFromVersion(const std::string& oldFormat, const std::string& targetVersion) const {
    // Placeholder implementation for config migration
    // In a real implementation, this would parse the old format,
    // apply migration rules, and save to the target version
    return false;
}

bool Config::exportToJSON(const std::string& filepath) const {
    return saveToJSON(filepath);
}

bool Config::exportToYAML(const std::string& filepath) const {
    return saveToYAML(filepath);
}

bool Config::importFromJSON(const std::string& filepath) {
    return loadFromJSON(filepath);
}

bool Config::importFromYAML(const std::string& filepath) {
    return loadFromYAML(filepath);
}

bool Config::validateAgainstSchema(const std::string& schemaFile) const {
    // Placeholder for schema validation
    return validateFile(schemaFile);
}

std::string Config::getFileExtension(const std::string& filepath) const {
    size_t pos = filepath.find_last_of('.');
    if (pos == std::string::npos) return "";
    std::string ext = filepath.substr(pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

bool Config::isValidFormat(const std::string& filepath) const {
    return validateFile(filepath);
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
