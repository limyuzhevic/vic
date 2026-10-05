// NLM Configuration System Enhancement
// This file contains the enhanced configuration system with validation, better defaults, and improved error handling

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Default configuration values
    static constexpr int64_t DEFAULT_RANDOM_SEED = 42;
    static constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;
    static constexpr int64_t DEFAULT_NEURON_COUNT = 500;
    static constexpr int64_t DEFAULT_REGION_COUNT = 1;
    static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
    static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
    static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
    static constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;
    static constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    static constexpr float DEFAULT_NOISE_LEVEL = 0.01f;
    static constexpr float DEFAULT_TEMPERATURE = 37.0f;  // Body temperature for development
    static constexpr bool DEFAULT_ENABLE_DEVELOPMENT = true;
    static constexpr bool DEFAULT_ENABLE_CHECKPOINTING = false;
    static constexpr bool DEFAULT_ENABLE_REPLAY = false;
    static constexpr int64_t DEFAULT_MAX_STEPS = 1000;
    static constexpr int64_t DEFAULT_BRAIN_COMPLEXITY = 1;
    static constexpr int64_t DEFAULT_MAX_MEMORY_EPISODES = 100;
    static constexpr float DEFAULT_REWARD_SCALE = 1.0f;
    static constexpr float DEFAULT_PUNISHMENT_SCALE = 1.0f;
    
    // Configuration validation rules
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
};

// Enhanced constructor with default values
Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with sensible defaults
    initializeDefaults();
}

Config::~Config() {
    // Ensure cleanup
    if (pImpl != nullptr) {
        pImpl->entries.clear();
    }
}

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {
    if (pImpl != nullptr) {
        NLM_LOG_DEBUG("Moving Config object");
    }
}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        if (pImpl != nullptr) {
            pImpl->entries.clear();
        }
        pImpl = std::move(other.pImpl);
        if (pImpl != nullptr) {
            NLM_LOG_DEBUG("Assigning moved Config object");
        }
    }
    return *this;
}

void Config::initializeDefaults() {
    // Set all default configuration values
    set("random_seed", ConfigSource::Default);
    set("simulation_timestep", ConfigSource::Default);
    set("neuron_count", ConfigSource::Default);
    set("region_count", ConfigSource::Default);
    set("connection_probability", ConfigSource::Default);
    set("stdp_ltp_weight", ConfigSource::Default);
    set("stdp_ltd_weight", ConfigSource::Default);
    set("stdp_tau", ConfigSource::Default);
    set("synaptogenesis_rate", ConfigSource::Default);
    set("pruning_rate", ConfigSource::Default);
    set("noise_level", ConfigSource::Default);
    set("temperature", ConfigSource::Default);
    set("enable_development", ConfigSource::Default);
    set("enable_checkpointing", ConfigSource::Default);
    set("enable_replay", ConfigSource::Default);
    set("max_steps", ConfigSource::Default);
    set("brain_complexity", ConfigSource::Default);
    set("max_memory_episodes", ConfigSource::Default);
    set("reward_scale", ConfigSource::Default);
    set("punishment_scale", ConfigSource::Default);
}

bool Config::loadFromFile(const std::string& filepath) {
    // Validate file path
    if (filepath.empty()) {
        NLM_LOG_ERROR("Empty file path provided to loadFromFile");
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with enhanced error handling
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_WARNING("Empty key found in configuration file at line " + std::to_string(lineNumber));
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error parsing configuration line " + std::to_string(lineNumber) + ": " + std::string(e.what()));
                return false;
            }
        } else {
            NLM_LOG_WARNING("Invalid configuration line (no '=') at line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    // Validate configuration after loading
    validateConfiguration();
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 0 || argv == nullptr) {
        NLM_LOG_ERROR("Invalid command line arguments");
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error parsing command line argument --" + key + "=" + value + ": " + std::string(e.what()));
                    return false;
                }
            } else {
                NLM_LOG_ERROR("Invalid command line argument format (missing '='): " + arg);
                return false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error parsing command line argument -" + key + " " + value + ": " + std::string(e.what()));
                return false;
            }
        } else {
            NLM_LOG_WARNING("Unknown command line argument: " + arg);
        }
    }
    
    // Validate configuration after loading
    validateConfiguration();
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Validate file path
    if (filepath.empty()) {
        NLM_LOG_ERROR("Empty file path provided to saveToFile");
        return false;
    }
    
    // Ensure directory exists
    std::filesystem::path filePath(filepath);
    std::filesystem::path parentDir = filePath.parent_path();
    if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
        try {
            std::filesystem::create_directories(parentDir);
        } catch (const std::filesystem::filesystem_error& e) {
            NLM_LOG_ERROR("Failed to create directory for configuration file: " + std::string(e.what()));
            return false;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    // Write configuration with comments
    file << "# NLM Configuration File\n";
    file << "# Generated on " << getCurrentTimeString() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        file << entry.key << " = ";
        
        // Format value appropriately
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

void Config::validateConfiguration() const {
    // Validate all configuration values
    for (const auto& entry : pImpl->entries) {
        validateEntry(entry);
    }
}

void Config::validateEntry(const ConfigEntry& entry) const {
    // Validate based on key
    if (entry.key == "random_seed") {
        if (!entry.value.index() || std::get<int64_t>(entry.value) < 0) {
            throw std::invalid_argument("random_seed must be a non-negative integer");
        }
    } else if (entry.key == "simulation_timestep") {
        double value = std::get<double>(entry.value);
        if (value <= 0.0 || value > 1.0) {
            throw std::invalid_argument("simulation_timestep must be between 0 and 1.0 seconds");
        }
    } else if (entry.key == "neuron_count") {
        int64_t value = std::get<int64_t>(entry.value);
        if (value <= 0) {
            throw std::invalid_argument("neuron_count must be positive");
        } else if (value > 100000) {
            NLM_LOG_WARNING("Very large neuron_count may impact performance: " + std::to_string(value));
        }
    } else if (entry.key == "region_count") {
        int64_t value = std::get<int64_t>(entry.value);
        if (value <= 0) {
            throw std::invalid_argument("region_count must be positive");
        }
    } else if (entry.key == "connection_probability") {
        float value = std::get<float>(entry.value);
        if (value < 0.0f || value > 1.0f) {
            throw std::invalid_argument("connection_probability must be between 0.0 and 1.0");
        }
    } else if (entry.key == "stdp_ltp_weight") {
        float value = std::get<float>(entry.value);
        if (value < 0.0f) {
            throw std::invalid_argument("stdp_ltp_weight must be non-negative");
        }
    } else if (entry.key == "stdp_ltd_weight") {
        float value = std::get<float>(entry.value);
        if (value < 0.0f) {
            throw std::invalid_argument("stdp_ltd_weight must be non-negative");
        }
    } else if (entry.key == "stdp_tau") {
        float value = std::get<float>(entry.value);
        if (value <= 0.0f) {
            throw std::invalid_argument("stdp_tau must be positive");
        }
    } else if (entry.key == "synaptogenesis_rate") {
        float value = std::get<float>(entry.value);
        if (value < 0.0f || value > 1.0f) {
            throw std::invalid_argument("synaptogenesis_rate must be between 0.0 and 1.0");
        }
    } else if (entry.key == "pruning_rate") {
        float value = std::get<float>(entry.value);
        if (value < 0.0f || value > 1.0f) {
            throw std::invalid_argument("pruning_rate must be between 0.0 and 1.0");
        }
    } else if (entry.key == "noise_level") {
        float value = std::get<float>(entry.value);
        if (value < 0.0f || value > 1.0f) {
            throw std::invalid_argument("noise_level must be between 0.0 and 1.0");
        }
    } else if (entry.key == "temperature") {
        float value = std::get<float>(entry.value);
        if (value < 0.0f || value > 100.0f) {
            throw std::invalid_argument("temperature must be between 0.0 and 100.0 degrees Celsius");
        }
    } else if (entry.key == "max_steps") {
        int64_t value = std::get<int64_t>(entry.value);
        if (value <= 0) {
            throw std::invalid_argument("max_steps must be positive");
        }
    } else if (entry.key == "brain_complexity") {
        int64_t value = std::get<int64_t>(entry.value);
        if (value <= 0) {
            throw std::invalid_argument("brain_complexity must be positive");
        }
    } else if (entry.key == "max_memory_episodes") {
        int64_t value = std::get<int64_t>(entry.value);
        if (value < 0) {
            throw std::invalid_argument("max_memory_episodes must be non-negative");
        }
    } else if (entry.key == "reward_scale") {
        float value = std::get<float>(entry.value);
        if (value < 0.0f) {
            throw std::invalid_argument("reward_scale must be non-negative");
        }
    } else if (entry.key == "punishment_scale") {
        float value = std::get<float>(entry.value);
        if (value < 0.0f) {
            throw std::invalid_argument("punishment_scale must be non-negative");
        }
    }
}

std::string Config::getCurrentTimeString() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm
