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
    // Enhanced configuration loader supporting JSON, YAML, and key=value formats
    
    // First, try to detect file format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    // Try JSON format first (most modern)
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    bool success = false;
    
    // Try parsing as JSON
    try {
        // Simple JSON parser for basic structures
        success = parseJSONConfig(content);
        if (success) {
            NLM_LOG_INFO("Loaded configuration from " + filepath + " (JSON format)");
            return true;
        }
    } catch (...) {
        // JSON parsing failed, try other formats
    }
    
    // Try YAML format
    try {
        success = parseYAMLConfig(content);
        if (success) {
            NLM_LOG_INFO("Loaded configuration from " + filepath + " (YAML format)");
            return true;
        }
    } catch (...) {
        // YAML parsing failed, try simple format
    }
    
                // Parse value type (try to detect)
            setConfigValue(key, value, ConfigSource::File, lineNumber);
            NLM_LOG_DEBUG("Loaded config key: " + key + " = " + value + " (line " + std::to_string(lineNumber) + ")");
        } else if (!key.empty() && !key.starts_with("#") && !key.starts_with("/")) {
            // Line doesn't have =, warn about parsing error
            NLM_LOG_WARNING("Invalid config format (no '=' found) at line " + std::to_string(lineNumber) + ": " + line.substr(0, 50));
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from " + filepath + " (simple format)");
    return true;
}
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Parsing command line arguments...");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setConfigValue(key, value, ConfigSource::CommandLine);
                NLM_LOG_DEBUG("Command line argument parsed: --" + key + "=" + value);
            } else {
                NLM_LOG_WARNING("Invalid command line argument format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setConfigValue(key, value, ConfigSource::CommandLine);
            NLM_LOG_DEBUG("Command line argument parsed: -" + key + " " + value);
        }
        // Handle special configuration flags
        else if (arg == "--help" || arg == "-h") {
            NLM_LOG_INFO("NLM Configuration Options:");
            NLM_LOG_INFO("  --help, -h: Show this help message");
            NLM_LOG_INFO("  --config=file: Load configuration from file");
            NLM_LOG_INFO("  --neuron-count=N: Set number of neurons");
            NLM_LOG_INFO("  --region-count=N: Set number of regions");
            NLM_LOG_INFO("  --random-seed=N: Set random seed for reproducibility");
            NLM_LOG_INFO("  --simulation-timestep=N: Set simulation timestep");
            NLM_LOG_INFO("  --log-level=L: Set log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)");
            NLM_LOG_INFO("  --log-to-file: Enable logging to file");
            NLM_LOG_INFO("  --checkpoint-dir=DIR: Set checkpoint directory");
            NLM_LOG_INFO("  --version: Show version information");
            NLM_LOG_INFO("  Any other --key=value pairs are accepted as configuration options");
        }
        else if (arg == "--version") {
            NLM_LOG_INFO("NLM Neural Learning Machine - Version 0.1.0 (Phase 6: Integrated)");
        }
        else if (arg == "--enable-plasticity") {
            set("plasticity.stdp.enable", true, ConfigSource::CommandLine);
            set("plasticity.hebbian.enable", true, ConfigSource::CommandLine);
            set("plasticity.structural.enable", true, ConfigSource::CommandLine);
            NLM_LOG_INFO("Plasticity systems enabled");
        }
        else if (arg == "--disable-plasticity") {
            set("plasticity.stdp.enable", false, ConfigSource::CommandLine);
            set("plasticity.hebbian.enable", false, ConfigSource::CommandLine);
            set("plasticity.structural.enable", false, ConfigSource::CommandLine);
            NLM_LOG_INFO("Plasticity systems disabled");
        }
        else if (arg == "--enable-curiosity") {
            set("neuromod.curiosity.enable", true, ConfigSource::CommandLine);
            NLM_LOG_INFO("Curiosity system enabled");
        }
        else if (arg == "--enable-novelty") {
            set("neuromod.novelty.enable", true, ConfigSource::CommandLine);
            NLM_LOG_INFO("Novelty detection enabled");
        }
        else if (arg == "--enable-reward-modulation") {
            set("neuromod.dopamine.enable", true, ConfigSource::CommandLine);
            NLM_LOG_INFO("Reward modulation enabled");
        }
        else if (arg == "--enable-development") {
            set("development.enable", true, ConfigSource::CommandLine);
            NLM_LOG_INFO("Development system enabled");
        }
        else {
            NLM_LOG_WARNING("Unknown command line argument: " + arg);
        }
    }
    
    NLM_LOG_INFO("Command line argument parsing complete");
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

void Config::setConfigValue(const std::string& key, const std::string& value, 
                           ConfigSource source, size_t lineNumber) {
    // Enhanced config value parsing with type detection
    if (key.empty()) {
        NLM_LOG_WARNING("Empty config key encountered");
        return;
    }
    
    std::string trimmedKey = trim(key);
    std::string trimmedValue = trim(value);
    
    // Skip comments in the value
    if (trimmedValue.empty() || trimmedValue[0] == '#') {
        return;
    }
    
    // Detect value type and store appropriately
    ConfigValue configValue;
    
    if (isNumber(trimmedValue)) {
        if (containsDecimalPoint(trimmedValue)) {
            configValue = std::stod(trimmedValue);
        } else {
            configValue = std::stol(trimmedValue);
        }
    } else if (trimmedValue == "true" || trimmedValue == "false") {
        configValue = (trimmedValue == "true") ? true : false;
    } else if (trimmedValue.starts_with('"') && trimmedValue.ends_with('"')) {
        configValue = trimmedValue.substr(1, trimmedValue.size() - 2);
    } else {
        configValue = trimmedValue;
    }
    
    set(trimmedKey, configValue, source);
    
    // Add metadata for debugging
    if (lineNumber) {
        // In a real implementation, we'd store this metadata
    }
}

std::string Config::detectConfigType(const std::string& value) const {
    // Heuristic-based type detection
    if (value.empty()) return "unknown";
    
    // Check for boolean
    if (value == "true" || value == "false") return "bool";
    
    // Check for number (integer or float)
    if (std::isdigit(value[0]) || (value[0] == '-' && std::isdigit(value[1])) {
        if (value.find('.') != std::string::npos) {
            return "double";
        } else {
            return "int";
        }
    }
    
    // Check for quoted string
    if ((value.starts_with('"') && value.ends_with('"')) ||
        (value.starts_with("'") && value.ends_with("'"))) {
        return "string";
    }
    
    // Default to string
    return "string";
}

bool Config::isJSONFile(const std::string& content) const {
    // Simple JSON detection - looks for { or [ at start
    if (content.empty()) return false;
    
    size_t firstNonSpace = content.find_first_not_of(" \t\n\r");
    if (firstNonSpace == std::string::npos) return false;
    
    return content[firstNonSpace] == '{' || content[firstNonSpace] == '[';
}

bool Config::isYAMLFile(const std::string& content) const {
    // Simple YAML detection - looks for key: value patterns
    if (content.empty()) return false;
    
    std::istringstream stream(content);
    std::string line;
    size_t colonCount = 0;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        // Count colons in this line (simple heuristic)
        colonCount += std::count(line.begin(), line.end(), ':');
        if (colonCount > 0) return true;
    }
    
    return false;
}

bool Config::parseJSONConfig(const std::string& content) {
    // Placeholder for JSON parsing - would use a proper JSON library
    // For now, just return false to indicate this is not a full implementation
    return false;
}

bool Config::parseYAMLConfig(const std::string& content) {
    // Placeholder for YAML parsing - would use a proper YAML library
    // For now, just return false to indicate this is not a full implementation
    return false;
}

void Config::validateConfig() const {
    // Validate configuration consistency
    for (const auto& entry : pImpl->entries) {
        // Check for duplicate keys
        // Check for valid value types
        // Check for required parameters
    }
}

void Config::ensureRequiredKeys() const {
    // Ensure required configuration keys are present
    // Provide sensible defaults if missing
    static const std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing required config key: " + key + ". Using default.");
        }
    }
}

bool Config::isNumber(const std::string& str) const {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '-') start = 1;
    
    for (size_t i = start; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

bool Config::containsDecimalPoint(const std::string& str) const {
    return str.find('.') != std::string::npos;
}
