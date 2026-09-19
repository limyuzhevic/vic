#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <variant>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string schemaJson;
    std::unordered_map<std::string, std::string> aliases;
    std::unordered_map<std::string, std::string> groups;
    std::vector<std::string> validationErrors;
    
    Impl() {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Determine file type by extension
        std::string ext = "." + std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            reportError("Cannot open file: " + filepath);
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        bool success = false;
        if (ext == ".json") {
            success = parseJson(content);
        } else if (ext == ".yaml" || ext == ".yml") {
            success = parseYaml(content);
        } else {
            // Default to simple key=value format for backward compatibility
            success = loadSimpleFormat(filepath);
        }
        
        if (!success) {
            reportError("Failed to parse file: " + filepath);
        }
        
        return success;
    } catch (const std::exception& e) {
        reportError(std::string("Exception in loadFromFile: ") + e.what());
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
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
    } catch (const std::exception& e) {
        reportError(std::string("Exception in loadFromArgs: ") + e.what());
        return false;
    }
}

bool Config::loadSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear();
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

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string ext = "." + std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".json") {
            std::string jsonStr = toJsonString(prettyPrint);
            file << jsonStr;
        } else if (ext == ".yaml" || ext == ".yml") {
            std::string yamlStr = toYamlString();
            file << yamlStr;
        } else {
            // Default to simple key=value format
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << " = ";
                
                std::visit([&file](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        file << '"' << arg << '"';
                    } else if constexpr (std::is_same_v<T, bool>) {
                        file << (arg ? "true" : "false");
                    } else {
                        file << arg;
                    }
                }, entry.value);
                file << "\n";
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        reportError(std::string("Exception in saveToFile: ") + e.what());
        return false;
    }
}

bool Config::setTyped(const std::string& key, const std::string& value, ConfigSource source) {
    try {
        // Try to determine type from value string
        if (value == "true" || value == "false") {
            bool boolVal = (value == "true");
            set(key, boolVal, source);
            return true;
        }
        
        // Try integer
        try {
            size_t pos;
            int intVal = std::stoi(value, &pos);
            if (pos == value.size()) {
                set(key, intVal, source);
                return true;
            }
        } catch (...) {}
        
        // Try double
        try {
            size_t pos;
            double doubleVal = std::stod(value, &pos);
            if (pos == value.size()) {
                set(key, doubleVal, source);
                return true;
            }
        } catch (...) {}
        
        // Default to string
        set(key, value, source);
        return true;
    } catch (const std::exception& e) {
        reportError(std::string("Exception in setTyped: ") + e.what());
        return false;
    }
}

// Rest of the implementation continues...

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

// Template instantiations
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

// Forward declaration of parseJson and parseYaml for implementation
template<typename T>
T stringToNumber(const std::string& str);

} // namespace nlm
