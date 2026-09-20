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
    
    // Determine format based on file extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Default to simple key=value format
        return loadFromSimple(filepath);
    }
}

bool Config::loadFromSimple(const std::string& filepath) {
bool Config::loadFromFile(const std::string& filepath) {
    // Determine format based on file extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Default to simple key=value format
        return loadFromSimple(filepath);
    }
}

bool Config::loadFromSimple(const std::string& filepath) {
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

bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAVE_NLOHMANN_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        // Convert JSON to config entries
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            setFromJSONValue(key, *it);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error loading JSON config: ") + e.what());
        return false;
    }
#else
    NLM_LOG_ERROR("JSON support not compiled in - use -DHAVE_NLOHMANN_JSON during CMake configuration");
    return false;
#endif
}

bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAVE_YAML_CPP
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        YAML::Node doc = YAML::LoadFile(filepath);
        
        // Convert YAML to config entries
        for (const auto& pair : doc) {
            std::string key = pair.first.as<std::string>();
            setFromYAMLNode(key, pair.second);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error loading YAML config: ") + e.what());
        return false;
    }
#else
    NLM_LOG_ERROR("YAML support not compiled in - use -DHAVE_YAML_CPP during CMake configuration");
    return false;
#endif
}

void Config::setFromJSONValue(const std::string& key, const nlohmann::json& value) {
    if (value.is_string()) {
        set(key, value.get<std::string>(), ConfigSource::File);
    } else if (value.is_number_integer()) {
        set(key, value.get<int64_t>(), ConfigSource::File);
    } else if (value.is_number_float()) {
        set(key, value.get<double>(), ConfigSource::File);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), ConfigSource::File);
    } else if (value.is_array()) {
        // For arrays, store as string representation
        std::string arrayStr = "[";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i > 0) arrayStr += ",";
            arrayStr += value[i].dump();
        }
        arrayStr += "]";
        set(key, arrayStr, ConfigSource::File);
    } else {
        set(key, value.dump(), ConfigSource::File);
    }
}

void Config::setFromYAMLNode(const std::string& key, const YAML::Node& node) {
    if (node.IsScalar()) {
        if (node.Tag() == "!!str") {
            set(key, node.as<std::string>(), ConfigSource::File);
        } else if (node.Tag() == "!!int" || node.Tag() == "!!integer") {
            set(key, node.as<int64_t>(), ConfigSource::File);
        } else if (node.Tag() == "!!float" || node.Tag() == "!!double" || node.Tag() == "!!real") {
            set(key, node.as<double>(), ConfigSource::File);
        } else if (node.Tag() == "!!bool") {
            set(key, node.as<bool>(), ConfigSource::File);
        } else {
            set(key, node.as<std::string>(), ConfigSource::File);
        }
    } else if (node.IsSequence()) {
        // For sequences, store as string representation
        std::string arrayStr = "[";
        for (size_t i = 0; i < node.size(); ++i) {
            if (i > 0) arrayStr += ",";
            arrayStr += std::to_string(node[i].as<double>());
        }
        arrayStr += "]";
        set(key, arrayStr, ConfigSource::File);
    } else if (node.IsMap()) {
        // Nested object - store as JSON string
        std::stringstream ss;
        for (const auto& pair : node) {
            ss << "\"" << pair.first.as<std::string>() << "\":" << pair.second;
        }
        set(key, ss.str(), ConfigSource::File);
    } else {
        set(key, node.as<std::string>(), ConfigSource::File);
    }
}
}

bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAVE_NLOHMANN_JSON
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        // Convert JSON to config entries
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            setFromJSONValue(key, *it);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error loading JSON config: ") + e.what());
        return false;
    }
#else
    NLM_LOG_ERROR("JSON support not compiled in - use -DHAVE_NLOHMANN_JSON during CMake configuration");
    return false;
#endif
}

bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAVE_YAML_CPP
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        YAML::Node doc = YAML::LoadFile(filepath);
        
        // Convert YAML to config entries
        for (const auto& pair : doc) {
            std::string key = pair.first.as<std::string>();
            setFromYAMLNode(key, pair.second);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error loading YAML config: ") + e.what());
        return false;
    }
#else
    NLM_LOG_ERROR("YAML support not compiled in - use -DHAVE_YAML_CPP during CMake configuration");
    return false;
#endif
}

void Config::setFromJSONValue(const std::string& key, const nlohmann::json& value) {
    if (value.is_string()) {
        set(key, value.get<std::string>(), ConfigSource::File);
    } else if (value.is_number_integer()) {
        set(key, value.get<int64_t>(), ConfigSource::File);
    } else if (value.is_number_float()) {
        set(key, value.get<double>(), ConfigSource::File);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), ConfigSource::File);
    } else if (value.is_array()) {
        // For arrays, store as string representation
        std::string arrayStr = "[";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i > 0) arrayStr += ",";
            arrayStr += value[i].dump();
        }
        arrayStr += "]";
        set(key, arrayStr, ConfigSource::File);
    } else {
        set(key, value.dump(), ConfigSource::File);
    }
}

void Config::setFromYAMLNode(const std::string& key, const YAML::Node& node) {
    if (node.IsScalar()) {
        if (node.Tag() == "!!str") {
            set(key, node.as<std::string>(), ConfigSource::File);
        } else if (node.Tag() == "!!int" || node.Tag() == "!!integer") {
            set(key, node.as<int64_t>(), ConfigSource::File);
        } else if (node.Tag() == "!!float" || node.Tag() == "!!double" || node.Tag() == "!!real") {
            set(key, node.as<double>(), ConfigSource::File);
        } else if (node.Tag() == "!!bool") {
            set(key, node.as<bool>(), ConfigSource::File);
        } else {
            set(key, node.as<std::string>(), ConfigSource::File);
        }
    } else if (node.IsSequence()) {
        // For sequences, store as string representation
        std::string arrayStr = "[";
        for (size_t i = 0; i < node.size(); ++i) {
            if (i > 0) arrayStr += ",";
            arrayStr += std::to_string(node[i].as<double>());
        }
        arrayStr += "]";
        set(key, arrayStr, ConfigSource::File);
    } else if (node.IsMap()) {
        // Nested object - store as JSON string
        std::stringstream ss;
        for (const auto& pair : node) {
            ss << "\"" << pair.first.as<std::string>() << "\":" << pair.second;
        }
        set(key, ss.str(), ConfigSource::File);
    } else {
        set(key, node.as<std::string>(), ConfigSource::File);
    }
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
