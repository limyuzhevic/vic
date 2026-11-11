#include "Config.hpp"
#include "../Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
        // Improved configuration file loader with better error handling and validation
        // Supports key=value format with comments and proper error reporting
        
        // Check if file exists before attempting to open
        if (!std::filesystem::exists(filepath)) {
            NLM_LOG_WARNING("Configuration file not found: " + filepath);
            return false;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("Loading configuration from: " + filepath);
        
        std::string line;
        size_t lineNumber = 0;
        size_t validEntries = 0;
        size_t invalidEntries = 0;
        
        while (std::getline(file, line)) {
            ++lineNumber;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[ cryptographic || line[0] == '/' {
                continue;
            }
            
            // Validate line format before parsing
            if (line.find('=') == std::string::npos) {
                NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": missing '=' operator: " + line);
                ++invalidEntries;
                continue;
            }
            
            // Parse key=value pairs with better validation
            std::string key = trim(line.substr(0, line.find('=')));
            std::string value = trim(line.substr(line.find('=') + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate key format (alphanumeric and underscores)
            std::regex keyPattern("^[a-zA-Z0-9_]+$");
            if (!std::regex_match(key, keyPattern)) {
                NLM_LOG_WARNING("Invalid config key " + std::to_string(lineNumber) + ": contains invalid characters: " + key);
                ++invalidEntries;
                continue;
            }
            
            // Log successful parsing
            NLM_LOG_DEBUG("Parsed config: " + key + " = " + value);
            set(key, value, ConfigSource::File);
            ++validEntries;
        }
        
        // Log summary
        NLM_LOG_INFO("Configuration loaded from " + filepath + ": " + 
                    std::to_string(validEntries) + " valid entries, " + 
                    std::to_string(invalidEntries) + " invalid entries");
        
        return true;
    }

bool Config::loadFromArgs(int argc, char** argv) {
    // Improved command-line argument parser with better error handling and help/usage
    
    // Print help if requested
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --help and -h flags
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;  // Exit after showing help
        }
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format with validation
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key format
                if (key.empty()) {
                    NLM_LOG_ERROR("Empty key in command line argument: " + arg);
                    continue;
                }
                
                NLM_LOG_DEBUG("Parsed command line arg: " + key + " = " + value);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Invalid command line argument format (missing '='): " + arg);
            }
        }
        // Handle -key value format with validation
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key format
            if (key.empty()) {
                NLM_LOG_ERROR("Empty key in command line argument: -" + key);
                continue;
            }
            
            NLM_LOG_DEBUG("Parsed command line arg: " + key + " = " + value);
            set(key, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_WARNING("Unrecognized command line argument: " + arg);
        }
    }
    
    return true;
}

return true;
}

void Config::printHelp() const {
    std::cout << "NLM Configuration Help" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  nlm [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --config=<file>" << std::endl;
    std::cout << "    Load configuration from file" << std::endl;
    std::cout << "  --key=value" << std::endl;
    std::cout << "    Set configuration key to value" << std::endl;
    std::cout << "  -k value" << std::endl;
    std::cout << "    Short form for setting configuration" << std::endl;
    std::cout << "  --help, -h" << std::endl;
    std::cout << "    Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Common configuration keys:" << std::endl;
    std::cout << "  random_seed          Random number generator seed" << std::endl;
    std::cout << "  simulation_timestep  Time step in seconds" << std::endl;
    std::cout << "  neuron_count         Number of neurons in the brain" << std::endl;
    std::cout << "  region_count         Number of brain regions" << std::endl;
    std::cout << "  connection_probability  Connection probability for random connectivity" << std::endl;
    std::cout << "  stdp_ltp_weight      STDP Long-Term Potentiation weight" << std::endl;
    std::cout << "  stdp_ltd_weight      STDP Long-Term Depression weight" << std::endl;
    std::cout << std::endl;
    std::cout << "Example:" << std::endl;
    std::cout << "  nlm --config=my.cfg --neuron_count=1000 --random_seed=42" << std::endl;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Improved configuration save with better error handling and logging
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save configuration to file: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Saving configuration to: " + filepath);
    
    for (const auto& entry : pImpl->entries) {
        // Write description as comment
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Write key=value pair with proper formatting
        file << entry.key << " = ";
        
        // Format value based on type for readability
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        file << "\"" << arg[i] << "\"";
                    } else {
                        file << arg[i];
                    }
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    file.flush();
    NLM_LOG_INFO("Configuration saved successfully to: " + filepath + 
                " (" + std::to_string(pImpl->entries.size()) + " entries)");
    
    return true;
}
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
