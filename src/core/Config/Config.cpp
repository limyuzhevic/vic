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

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Configuration validation and type checking
bool Config::validate() const {
    // Phase 6 validation rules based on integration requirements
    for (const auto& entry : pImpl->entries) {
        // Validate that required integration settings have sensible defaults
        if (entry.key == "enable_memory_integration") {
            auto val = get<bool>(entry.key);
            if (val && !val.value()) {
                NLM_LOG_WARNING("Memory integration is disabled - this may affect Phase 6 functionality");
            }
        }
        else if (entry.key == "enable_prediction_integration") {
            auto val = get<bool>(entry.key);
            if (val && !val.value()) {
                NLM_LOG_WARNING("Prediction integration is disabled - this may affect cognitive capabilities");
            }
        }
        else if (entry.key == "enable_cognition_integration") {
            auto val = get<bool>(entry.key);
            if (val && !val.value()) {
                NLM_LOG_WARNING("Cognition integration is disabled - core intelligence features may not work");
            }
        }
        else if (entry.key == "enable_sleep_consolidation") {
            auto val = get<bool>(entry.key);
            if (val && !val.value()) {
                NLM_LOG_INFO("Sleep consolidation is disabled - memory consolidation may be less effective");
            }
        }
        else if (entry.key == "enable_replay_mechanism") {
            auto val = get<bool>(entry.key);
            if (val && !val.value()) {
                NLM_LOG_INFO("Replay mechanism is disabled - episodic memory replay will not occur");
            }
        }
        else if (entry.key == "neuron_count") {
            auto val = get<int>(entry.key);
            if (val && val.value() < 1000) {
                NLM_LOG_WARNING("Neuron count is low (" + std::to_string(val.value()) + ") - may limit cognitive capabilities");
            } else if (val && val.value() > 1000000) {
                NLM_LOG_WARNING("Neuron count is very high (" + std::to_string(val.value()) + ") - may cause performance issues");
            }
        }
        else if (entry.key == "region_count") {
            auto val = get<int>(entry.key);
            if (val && val.value() < 4) {
                NLM_LOG_WARNING("Region count is low (" + std::to_string(val.value()) + ") - limits brain specialization");
            }
        }
        else if (entry.key == "working_memory_capacity") {
            auto val = get<size_t>(entry.key);
            if (val && val.value() < 100) {
                NLM_LOG_WARNING("Working memory capacity is low (" + std::to_string(val.value()) + ") - may limit cognitive processing");
            }
        }
        else if (entry.key == "episodic_memory_max_episodes") {
            auto val = get<size_t>(entry.key);
            if (val && val.value() < 100) {
                NLM_LOG_WARNING("Episodic memory capacity is low (" + std::to_string(val.value()) + ") - may limit experience storage");
            }
        }
        else if (entry.key == "replay_interval") {
            auto val = get<size_t>(entry.key);
            if (val && val.value() > 10000) {
                NLM_LOG_WARNING("Replay interval is very long (" + std::to_string(val.value()) + ") - may reduce memory consolidation effectiveness");
            }
        }
        else if (entry.key == "consolidation_interval") {
            auto val = get<size_t>(entry.key);
            if (val && val.value() > 10000) {
                NLM_LOG_WARNING("Consolidation interval is very long (" + std::to_string(val.value()) + ") - may reduce memory consolidation effectiveness");
            }
        }
        // Check for missing required keys
        else if (entry.key == "log_level") {
            auto val = get<std::string>(entry.key);
            if (!val.has_value()) {
                NLM_LOG_WARNING("log_level not set - using default");
            }
        }
    }
    
    // Final validation checks
    bool neuronCountValid = has("neuron_count") ? (get<size_t>("neuron_count").value_or(0) >= 1000) : false;
    bool regionCountValid = has("region_count") ? (get<size_t>("region_count").value_or(0) >= 4) : false;
    bool memoryCapacityValid = (has("working_memory_capacity") ? get<size_t>("working_memory_capacity").value_or(0) : 0) >= 100;
    
    if (!neuronCountValid) {
        NLM_LOG_WARNING("Neuron count validation failed - setting to default 10000");
        set<size_t>("neuron_count", 10000);
    }
    if (!regionCountValid) {
        NLM_LOG_WARNING("Region count validation failed - setting to default 8");
        set<size_t>("region_count", 8);
    }
    if (!memoryCapacityValid) {
        NLM_LOG_WARNING("Working memory capacity validation failed - setting to default 1000");
        set<size_t>("working_memory_capacity", 1000);
    }
    
    NLM_LOG_INFO("Configuration validation completed successfully");
    return true;
}

// Configuration schema for Phase 6 integration
std::string Config::getSchema() const {
    return """# NLM Configuration Schema for Phase 6
# Required integration settings
enable_memory_integration: boolean (default: true)
enable_prediction_integration: boolean (default: true)
enable_cognition_integration: boolean (default: true)
enable_sleep_consolidation: boolean (default: true)
enable_replay_mechanism: boolean (default: true)

# System parameters
neuron_count: integer (>= 1000, default: 10000)
region_count: integer (>= 4, default: 8)

# Memory parameters
working_memory_capacity: integer (>= 100, default: 1000)
episodic_memory_max_episodes: integer (>= 100, default: 1000)
replay_interval: integer (default: 100)
consolidation_interval: integer (default: 1000)

# Performance parameters
max_simulation_steps: integer (default: 100000)
simulation_timestep: float (default: 0.001)
""";
}

// Validate configuration compatibility
bool Config::validateCompatibility() const {
    NLM_LOG_INFO("Validating Phase 6 configuration compatibility");
    
    // Check integration flags consistency
    bool memoryIntegration = get<bool>("enable_memory_integration").value_or(true);
    bool predictionIntegration = get<bool>("enable_prediction_integration").value_or(true);
    bool cognitionIntegration = get<bool>("enable_cognition_integration").value_or(true);
    
    if (memoryIntegration && !has("working_memory_capacity")) {
        NLM_LOG_WARNING("Memory integration enabled but working_memory_capacity not set");
    }
    
    if (predictionIntegration && !has("enable_prediction_integration")) {
        NLM_LOG_WARNING("Prediction integration enabled but prediction system not configured");
    }
    
    if (cognitionIntegration && !has("enable_cognition_integration")) {
        NLM_LOG_WARNING("Cognition integration enabled but cognition systems not configured");
    }
    
    // Check memory system consistency
    bool memoryEnabled = get<bool>("enable_memory_integration").value_or(true);
    if (memoryEnabled) {
        size_t workingMem = get<size_t>("working_memory_capacity").value_or(1000);
        size_t episodicMem = get<size_t>("episodic_memory_max_episodes").value_or(1000);
        
        if (workingMem < 100) {
            NLM_LOG_ERROR("Working memory capacity too low for effective cognition");
            return false;
        }
        
        if (episodicMem < 100) {
            NLM_LOG_ERROR("Episodic memory capacity too low for experience storage");
            return false;
        }
        
        // Check consistency between memory systems
        if (workingMem > 10000) {
            NLM_LOG_WARNING("Working memory capacity very high - may affect performance");
        }
        
        if (episodicMem > 10000) {
            NLM_LOG_WARNING("Episodic memory capacity very high - may affect performance");
        }
    }
    
    NLM_LOG_INFO("Configuration compatibility validation completed successfully");
    return true;
}

// Get configuration for specific Phase 6 subsystem
Config Config::getPhase6Config() const {
    Config phase6Config;
    
    // Copy only Phase 6 relevant settings
    std::vector<std::string> phase6Keys = {
        "enable_memory_integration",
        "enable_prediction_integration", 
        "enable_cognition_integration",
        "enable_sleep_consolidation",
        "enable_replay_mechanism",
        "neuron_count",
        "region_count",
        "working_memory_capacity",
        "episodic_memory_max_episodes",
        "replay_interval",
        "consolidation_interval",
        "log_level",
        "log_to_file",
        "log_filename"
    };
    
    for (const auto& key : phase6Keys) {
        if (has(key)) {
            if (has<int>(key)) phase6Config.set(key, get<int>(key).value());
            else if (has<size_t>(key)) phase6Config.set(key, get<size_t>(key).value());
            else if (has<double>(key)) phase6Config.set(key, get<double>(key).value());
            else if (has<bool>(key)) phase6Config.set(key, get<bool>(key).value());
            else if (has<std::string>(key)) phase6Config.set(key, get<std::string>(key).value());
        }
    }
    
    NLM_LOG_INFO("Phase 6 configuration extracted successfully");
    return phase6Config;
}

// Get configuration for specific Phase 6 subsystem
std::string Config::getPhase6Summary() const {
    std::ostringstream oss;
    oss << "=== NLM Phase 6 Configuration Summary ===\n\n";
    
    oss << "Integration Status:\n";
    oss << "  Memory Integration: " << (get<bool>("enable_memory_integration").value_or(true) ? "✅ ENABLED" : "❌ DISABLED") << "\n";
    oss << "  Prediction Integration: " << (get<bool>("enable_prediction_integration").value_or(true) ? "✅ ENABLED" : "❌ DISABLED") << "\n";
    oss << "  Cognition Integration: " << (get<bool>("enable_cognition_integration").value_or(true) ? "✅ ENABLED" : "❌ DISABLED") << "\n";
    oss << "  Sleep Consolidation: " << (get<bool>("enable_sleep_consolidation").value_or(true) ? "✅ ENABLED" : "❌ DISABLED") << "\n";
    oss << "  Replay Mechanism: " << (get<bool>("enable_replay_mechanism").value_or(true) ? "✅ ENABLED" : "❌ DISABLED") << "\n\n";
    
    oss << "System Parameters:\n";
    oss << "  Neuron Count: " << get<size_t>("neuron_count").value_or(10000) << "\n";
    oss << "  Region Count: " << get<size_t>("region_count").value_or(8) << "\n";
    oss << "  Working Memory Capacity: " << get<size_t>("working_memory_capacity").value_or(1000) << "\n";
    oss << "  Episodic Memory Episodes: " << get<size_t>("episodic_memory_max_episodes").value_or(1000) << "\n\n";
    
    oss << "Memory Integration:\n";
    oss << "  Replay Interval: " << get<size_t>("replay_interval").value_or(100) << " steps\n";
    oss << "  Consolidation Interval: " << get<size_t>("consolidation_interval").value_or(1000) << " steps\n\n";
    
    oss << "Logging:\n";
    oss << "  Log Level: " << get<std::string>("log_level").value_or("INFO") << "\n";
    oss << "  Log to File: " << (get<bool>("log_to_file").value_or(true) ? "✅ YES" : "❌ NO") << "\n";
    oss << "  Log Filename: " << get<std::string>("log_filename").value_or("nlm_phase6.log") << "\n";
    
    return oss.str();
}

// Get configuration summary
std::string Config::summary() const {
    return getPhase6Summary();
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
