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
        // Log error for debugging
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue; // Skip lines without '='
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key is not empty
        if (key.empty()) {
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::File);
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        return true; // No arguments to process
    }
    
    // Initialize with advanced power-user options
    set("profile.enable", false, ConfigSource::Default);
    set("profile.mode", std::string("basic"), ConfigSource::Default);
    set("memory_report.enabled", false, ConfigSource::Default);
    set("memory_report.level", std::string("detailed"), ConfigSource::Default);
    set("checkpoint_interval", 0.0, ConfigSource::Default);
    set("log_level", std::string("info"), ConfigSource::Default);
    set("config_template.enabled", false, ConfigSource::Default);
    set("config_template.format", std::string("json"), ConfigSource::Default);
    
    // Initialize simulation control
    set("simulation.pause", false, ConfigSource::Default);
    set("simulation.speed_modulation", 1.0, ConfigSource::Default);
    set("simulation.step_mode", false, ConfigSource::Default);
    set("simulation.debug_neuron_tracking", false, ConfigSource::Default);
    
    // Initialize memory management
    set("memory_pools.enabled", false, ConfigSource::Default);
    set("memory_pools.debug", false, ConfigSource::Default);
    set("checkpoint_info.enabled", false, ConfigSource::Default);
    set("cleanup_memory.enabled", false, ConfigSource::Default);
    
    // Initialize performance monitoring
    set("performance_report.enabled", false, ConfigSource::Default);
    set("performance_report.interval", 5000.0, ConfigSource::Default);
    set("profile_neural.enabled", false, ConfigSource::Default);
    set("benchmark.enabled", false, ConfigSource::Default);
    set("metrics.enabled", false, ConfigSource::Default);
    
    // Initialize configuration tools
    set("config_validate.enabled", false, ConfigSource::Default);
    set("config_export.enabled", false, ConfigSource::Default);
    set("config_import.enabled", false, ConfigSource::Default);
    set("config_diff.enabled", false, ConfigSource::Default);
    
    // Initialize debugging tools
    set("debug_trace.enabled", false, ConfigSource::Default);
    set("debug_state.enabled", false, ConfigSource::Default);
    set("debug_plasticity.enabled", false, ConfigSource::Default);
    set("debug_memory.enabled", false, ConfigSource::Default);
    
    // Initialize scripting and automation
    set("python_script.enabled", false, ConfigSource::Default);
    set("json_config.enabled", false, ConfigSource::Default);
    set("batch_processing.enabled", false, ConfigSource::Default);
    set("api.enabled", false, ConfigSource::Default);
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            continue; // Skip null pointers
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.size() >= 2 && arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg.size() > 1 && arg[0] == '-' && i + 1 < argc && argv[i + 1]) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated automatically\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        return std::nullopt;
    }
    
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
    if (key.empty()) {
        return defaultValue;
    }
    
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        return;
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
    if (!key.empty()) {
        set(key, ConfigValue(value), source);
    }
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    if (!key.empty()) {
        set(key, ConfigValue(value), source);
    }
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    if (!key.empty()) {
        set(key, ConfigValue(value), source);
    }
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    if (!key.empty()) {
        set(key, ConfigValue(value), source);
    }
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
        if (!entry.key.empty()) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}
void Config::clear() {
    pImpl->entries.clear();
}
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    if (pImpl->entries.empty()) {
        oss << "  No configuration entries\n";
        return oss.str();
    }
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            continue;
        }
        
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
