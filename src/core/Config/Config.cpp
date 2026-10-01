#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <numeric>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::isFileReadable(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

bool Config::isFileWritable(const std::string& filepath) {
    std::ofstream file(filepath);
    return file.good();
}

std::string Config::getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) return "";
    return filename.substr(pos + 1);
}

bool Config::loadFromFile(const std::string& filepath) {
    if (filepath.empty()) {
        throw InvalidArgumentException("Filepath cannot be empty");
    }
    
    if (!isFileReadable(filepath)) {
        throw InvalidConfigFileException(filepath, "file cannot be read or does not exist");
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw InvalidConfigFileException(filepath, "failed to open file");
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key.empty()) {
                throw InvalidConfigFileException(filepath, "line " + std::to_string(lineNumber) + ": empty key");
            }
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File, true);
            } catch (const ConfigException&) {
                throw InvalidConfigFileException(filepath, "line " + std::to_string(lineNumber) + ": " + 
                    std::string(std::current_exception().__cxa_what()) + " for key '" + key + "'");
            }
        } else {
            if (!line.empty() && line.find('#') != 0 && line.find('/') != 0) {
                throw InvalidConfigFileException(filepath, "line " + std::to_string(lineNumber) + ": invalid format, expected key=value");
            }
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc < 1 || argv == nullptr) {
        throw InvalidArgumentException("argc or argv cannot be null or negative");
    }
    
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == nullptr) {
            throw InvalidArgumentException("argument at position " + std::to_string(i) + " is null");
        }
        
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key.empty()) {
                    throw InvalidArgumentException("empty key in command line argument: '" + arg + "'");
                }
                
                try {
                    set(key, value, ConfigSource::CommandLine, true);
                } catch (const ConfigException&) {
                    throw InvalidArgumentException("failed to parse command line argument '" + arg + "': " + 
                        std::string(std::current_exception().__cxa_what()));
                }
            } else {
                throw InvalidArgumentException("invalid command line argument format: '" + arg + "'");
            }
        }
        else if (arg[0] == '-' && i + 1 < argc) {
            if (argv[i + 1] == nullptr) {
                throw InvalidArgumentException("null value for key '" + arg.substr(1) + "'");
            }
            
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key.empty()) {
                throw InvalidArgumentException("empty key in command line argument: '" + arg + "'");
            }
            
            try {
                set(key, value, ConfigSource::CommandLine, true);
            } catch (const ConfigException&) {
                throw InvalidArgumentException("failed to parse command line argument key-value pair '" + arg + " " + value + "': " + 
                    std::string(std::current_exception().__cxa_what()));
            }
        } else {
            throw InvalidArgumentException("invalid command line argument format: '" + arg + "'");
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        throw InvalidArgumentException("filepath cannot be empty");
    }
    
    if (!isFileWritable(filepath)) {
        throw InvalidConfigFileException(filepath, "file cannot be written to");
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw InvalidConfigFileException(filepath, "failed to open file for writing");
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
    }
    
    return true;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue, bool validate) {
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });

        if (it == pImpl->entries.end()) {
            return defaultValue;
        }
        
        try {
            if (validate) {
                validateConfigValue(key, it->value);
            }
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            throw InvalidConfigValueException(key, getTypeName(T()), getTypeName(it->value));
        }
    } catch (const ConfigException&) {
        throw;
    } catch (const std::exception& e) {
        throw InvalidConfigValueException(key, "valid value", e.what());
    }
}

template<typename T>
T Config::getRequired(const std::string& key, const std::string& expectedType) {
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            throw MissingConfigValueException(key);
        }
        
        try {
            if (expectedType.empty()) {
                validateConfigValue(key, it->value);
            } else {
                std::string actualType = getTypeName(it->value);
                if (expectedType != actualType && 
                    !(expectedType == "string" && actualType == "string") &&
                    !(expectedType == "int" && actualType == "int64_t") &&
                    !(expectedType == "int64_t" && actualType == "int")) {
                    throw InvalidConfigValueException(key, expectedType, actualType);
                }
            }
            
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            throw InvalidConfigValueException(key, getTypeName(T()), getTypeName(it->value));
        }
    } catch (const ConfigException&) {
        throw;
    } catch (const std::exception& e) {
        throw InvalidConfigValueException(key, "valid value", e.what());
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, bool validate) {
    if (key.empty()) {
        throw InvalidArgumentException("key cannot be empty");
    }
    
    if (validate) {
        validateConfigValue(key, value);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, "", true);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, int value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, double value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, bool value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

bool Config::has(const std::string& key) const {
    if (key.empty()) {
        return false;
    }
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    if (key.empty()) {
        return;
    }
    
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
        oss << "] (" << sourceToString(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::sourceToString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        default: return "Unknown";
    }
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

template int Config::getOr<int>(const std::string&, const int&, bool) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&, bool) const;
template double Config::getOr<double>(const std::string&, const double&, bool) const;
template bool Config::getOr<bool>(const std::string&, const bool&, bool) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&, bool) const;

} // namespace nlm
