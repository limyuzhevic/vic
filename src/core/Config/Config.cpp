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

bool Config::loadFromArgs(int argc, char** argv) {
    return loadFromArgs(argc, argv, HelpInfo());
}

bool Config::loadFromArgsWithValidation(int argc, char** argv, 
                                        const std::map<std::string, CLIArgument>& arguments,
                                        const HelpInfo& helpInfo) {
    // Handle help/version flags first
    if (handleHelpFlag(argc, argv, helpInfo)) {
        return false;  // Should exit
    }
    
    // Process arguments with validation
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Skip if it's a help/version flag that was already handled
        if (arg == "--help" || arg == "-h" || arg == "--version") {
            continue;
        }
        
        // Handle --key=value format with validation
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate argument if we have definition
                auto it = arguments.find(key);
                if (it != arguments.end()) {
                    std::string error = validateArgument(it->second, value);
                    if (!error.empty()) {
                        std::cerr << "ERROR: " << error << "\n";
                        std::cerr << "Use --help for available options.\n";
                        return false;
                    }
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format with validation
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate argument if we have definition
            auto it = arguments.find(key);
            if (it != arguments.end()) {
                std::string error = validateArgument(it->second, value);
                if (!error.empty()) {
                    std::cerr << "ERROR: " << error << "\n";
                    std::cerr << "Use --help for available options.\n";
                    return false;
                }
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle boolean flags
        else if (arg == "--" + key) {
            set(key, true, ConfigSource::CommandLine);
        }
        else if (arg.substr(0, 2) == "--" && arg.size() > 2 && arg[2] == '-') {
            std::string key = arg.substr(2);
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

// Helper function to convert string to lowercase
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Static helper methods
bool Config::handleHelpFlag(int argc, char** argv, const HelpInfo& helpInfo) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printHelp(helpInfo);
            return true;
        } else if (arg == "--version") {
            printVersion(helpInfo);
            return true;
        }
    }
    return false;
}

void Config::printHelp(const HelpInfo& helpInfo) {
    std::cout << "Usage: " << helpInfo.usage << "\n";
    std::cout << "\n";
    std::cout << helpInfo.description << "\n";
    std::cout << "\n";
    
    if (!helpInfo.arguments.empty()) {
        std::cout << "Options:\n";
        for (const auto& arg : helpInfo.arguments) {
            std::cout << "  ";
            if (!arg.shortName.empty()) {
                std::cout << "-" << arg.shortName << ", ";
            }
            std::cout << "--" << arg.name;
            
            if (!arg.defaultValue.empty()) {
                std::cout << "=" << arg.defaultValue;
            }
            
            std::cout << "\n";
            std::cout << "    " << arg.description << "\n";
            
            if (arg.required) {
                std::cout << "    [Required]" << "\n";
            }
        }
    }
    
    if (!helpInfo.version.empty()) {
        std::cout << "\nVersion: " << helpInfo.version << "\n";
    }
}

void Config::printVersion(const HelpInfo& helpInfo) {
    std::cout << helpInfo.programName << " version " << helpInfo.version << "\n";
    if (!helpInfo.description.empty()) {
        std::cout << helpInfo.description << "\n";
    }
}

std::string Config::validateArgument(const CLIArgument& arg, const std::string& value) {
    // Check required arguments
    if (arg.required && value.empty()) {
        return "Missing required argument: --" + arg.name;
    }
    
    // Type validation
    for (const auto& type : arg.valueTypes) {
        if (type == "string") {
            return "";  // Any string is valid
        } else if (type == "int") {
            try {
                size_t pos;
                int val = std::stoi(value, &pos);
                if (pos != value.size()) {
                    continue; // Not all characters consumed, try next type
                }
                return "";  // Valid integer
            } catch (...) {
                // Invalid, try next type
                continue;
            }
        } else if (type == "double") {
            try {
                size_t pos;
                double val = std::stod(value, &pos);
                if (pos != value.size()) {
                    continue; // Not all characters consumed, try next type
                }
                return "";  // Valid double
            } catch (...) {
                // Invalid, try next type
                continue;
            }
        } else if (type == "bool") {
            if (value == "true" || value == "false" || value == "1" || value == "0") {
                return "";  // Valid boolean
            }
            // Try next type
            continue;
        }
    }
    
    // Get all supported types for error message
    std::string types = "";
    for (size_t i = 0; i < arg.valueTypes.size(); ++i) {
        if (i > 0) types += "/";
        types += arg.valueTypes[i];
    }
    
    return "Invalid value '" + value + "' for argument --" + arg.name + ". Expected one of: " + types;
}

// Explicit template instantiations (existing ones)
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

