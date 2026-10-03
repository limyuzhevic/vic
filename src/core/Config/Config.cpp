#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "NLMErrors.hpp"
#include "ErrorUtils.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "NLMErrors.hpp"
#include "ErrorUtils.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ErrorHandler errorHandler;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Validate file path
        NLM_VALIDATE(!filepath.empty(), "File path cannot be empty");
        
        // Check if file exists
        if (!std::filesystem::exists(filepath)) {
            NLM_THROW_IO_ERROR("Configuration file does not exist: " + filepath);
        }
        
        if (!std::filesystem::is_regular_file(filepath)) {
            NLM_THROW_IO_ERROR("Path is not a regular file: " + filepath);
        }
        
        // Open file safely
        std::ifstream file(filepath, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            NLM_THROW_IO_ERROR("Cannot open configuration file: " + filepath);
        }
        
        pImpl->errorHandler.clear();
        
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            ++lineNumber;
            pImpl->errorHandler.clear();
            
            try {
                // Skip empty lines and comments
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                // Parse simple key=value pairs with validation
                NLM_VALIDATE(line.find('=') != std::string::npos, 
                           "Invalid config line " + std::to_string(lineNumber) + ": no '=' found");
                
                size_t pos = line.find('=');
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Validate key
                NLM_VALIDATE(!key.empty(), "Config key cannot be empty (line " + std::to_string(lineNumber) + ")");
                NLM_VALIDATE(isalpha(key[0]) || key[0] == '_', 
                           "Config key must start with letter or underscore (line " + std::to_string(lineNumber) + ")");
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Validate value
                NLM_VALIDATE(!value.empty(), "Config value cannot be empty (line " + std::to_string(lineNumber) + ")");
                
                set(key, value, ConfigSource::File);
                
                NLM_THROW_ON_ERROR();
                
            } catch (const std::exception& e) {
                pImpl->errorHandler.addError(std::make_shared<RuntimeError>(
                    "Error processing line " + std::to_string(lineNumber) + ": " + std::string(e.what()), "Config"));
                continue; // Skip invalid lines but continue processing
            }
        }
        
        return true;
        
    } catch (const NLMError& e) {
        return false;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Config"));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        NLM_VALIDATE(argc > 0, "Argument count must be positive");
        
        pImpl->errorHandler.clear();
        
        for (int i = 1; i < argc; ++i) {
            NLM_VALIDATE(argv[i] != nullptr, "Argument pointer cannot be null");
            
            try {
                std::string arg(argv[i]);
                
                // Handle --key=value format
                if (arg.substr(0, 2) == "--") {
                    size_t pos = arg.find('=');
                    if (pos != std::string::npos) {
                        std::string key = arg.substr(2, pos - 2);
                        std::string value = arg.substr(pos + 1);
                        set(key, value, ConfigSource::CommandLine);
                    } else {
                        NLM_THROW_VALIDATION_ERROR("Invalid argument format (missing '='): " + arg);
                    }
                }
                // Handle -key value format
                else if (arg[0] == '-' && i + 1 < argc) {
                    std::string key = arg.substr(1);
                    std::string value = argv[++i];
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_THROW_VALIDATION_ERROR("Invalid argument format: " + arg);
                }
                
                NLM_THROW_ON_ERROR();
                
            } catch (const std::exception& e) {
                pImpl->errorHandler.addError(std::make_shared<RuntimeError>(
                    "Error processing argument " + std::to_string(i) + ": " + std::string(e.what()), "Config"));
                continue;
            }
        }
        
        return true;
        
    } catch (const NLMError& e) {
        return false;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Config"));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Validate file path
        NLM_VALIDATE(!filepath.empty(), "File path cannot be empty");
        
        // Create directory if it doesn't exist
        std::filesystem::path path(filepath);
        std::filesystem::path parentDir = path.parent_path();
        if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
            if (!std::filesystem::create_directories(parentDir)) {
                NLM_THROW_IO_ERROR("Cannot create directory for config file: " + parentDir.string());
            }
        }
        
        // Open file safely
        std::ofstream file(filepath, std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            NLM_THROW_IO_ERROR("Cannot create configuration file: " + filepath);
        }
        
        // Write header comment
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated on " << std::chrono::system_clock::now() << std::endl;
        file << std::endl;
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << std::endl;
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            file << std::endl << std::endl;
        }
        
        file.flush();
        if (!file.good()) {
            NLM_THROW_IO_ERROR("Error writing to configuration file: " + filepath);
        }
        
        return true;
        
    } catch (const NLMError& e) {
        return false;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Config"));
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
        // Validate key
        NLM_VALIDATE(!key.empty(), "Key cannot be empty");
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            pImpl->errorHandler.addError(std::make_shared<RuntimeError>(
                "Type mismatch for key: " + key, "Config"));
            return std::nullopt;
        }
        
    } catch (const NLMError& e) {
        return std::nullopt;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Config"));
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    try {
        auto val = get<T>(key);
        if (val.has_value()) {
            return val.value();
        }
        return defaultValue;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Config"));
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        // Validate inputs
        NLM_VALIDATE(!key.empty(), "Key cannot be empty");
        NLM_VALIDATE(!value.index() || !std::get<std::monostate>(value), "Value cannot be monostate");
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
        
    } catch (const std::bad_alloc&) {
        pImpl->errorHandler.addError(std::make_shared<ResourceError>(
            "Memory allocation failed while setting key: " + key, "Config"));
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Config"));
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
    try {
        return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Config"));
        return false;
    }
}

void Config::remove(const std::string& key) {
    try {
        NLM_VALIDATE(!key.empty(), "Key cannot be empty");
        
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
        
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Config"));
    }
}

std::vector<std::string> Config::getKeys() const {
    try {
        std::vector<std::string> keys;
        keys.reserve(pImpl->entries.size());
        for (const auto& entry : pImpl->entries) {
            keys.push_back(entry.key);
        }
        return keys;
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Config"));
        return std::vector<std::string>();
    }
}

void Config::clear() {
    try {
        pImpl->entries.clear();
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Config"));
    }
}

std::string Config::summary() const {
    try {
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
    } catch (const std::exception& e) {
        pImpl->errorHandler.addError(std::make_shared<RuntimeError>(e.what(), "Config"));
        return "Error generating config summary";
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
namespace {
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
}

} // namespace nlm
