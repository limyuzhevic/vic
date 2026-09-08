#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    UserLevelConfig userLevelConfig;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->userLevelConfig.setBeginnerMode();
}

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

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    UserLevelConfig userLevelConfig;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->userLevelConfig.setBeginnerMode();
}

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
                ((value.front() == '\"' && value.back() == '\"') ||
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

void Config::setUserLevel(UserLevel level) {
    pImpl->userLevelConfig.userLevel = level;
    
    // Reset settings based on user level
    switch (level) {
        case UserLevel::Beginner:
            pImpl->userLevelConfig.setBeginnerMode();
            pImpl->userLevelConfig.tutorialMode = true;
            pImpl->userLevelConfig.debugMode = false;
            break;
        case UserLevel::Intermediate:
            pImpl->userLevelConfig.setIntermediateMode();
            pImpl->userLevelConfig.tutorialMode = false;
            pImpl->userLevelConfig.debugMode = false;
            break;
        case UserLevel::Expert:
            pImpl->userLevelConfig.setExpertMode();
            pImpl->userLevelConfig.tutorialMode = false;
            pImpl->userLevelConfig.debugMode = true;
            break;
    }
}

UserLevel Config::getUserLevel() const {
    return pImpl->userLevelConfig.userLevel;
}

bool Config::isTutorialMode() const {
    return pImpl->userLevelConfig.tutorialMode;
}

bool Config::isDebugMode() const {
    return pImpl->userLevelConfig.debugMode;
}

bool Config::shouldShowGuidance() const {
    return pImpl->userLevelConfig.levelSettings.getUserLevel().showGuidance;
}

bool Config::isAdvancedMode() const {
    return pImpl->userLevelConfig.levelSettings.getUserLevel().advancedMode;
}

void Config::showHelp() const {
    // User-level specific help
    switch (pImpl->userLevelConfig.userLevel) {
        case UserLevel::Beginner:
            NLM_LOG_INFO("=== NLM Beginner Help ===");
            NLM_LOG_INFO("Available commands:");
            NLM_LOG_INFO("  help               - Show help information");
            NLM_LOG_INFO("  tutorial <topic>   - Start a tutorial");
            NLM_LOG_INFO("  run_basic_tests     - Run simple demonstration tests");
            NLM_LOG_INFO("  set beginner       - Set to beginner mode");
            NLM_LOG_INFO("Use 'tutorial getting_started' for a quick start guide.");
            break;
        case UserLevel::Intermediate:
            NLM_LOG_INFO("=== NLM Intermediate Help ===");
            NLM_LOG_INFO("Available commands:");
            NLM_LOG_INFO("  help               - Show help information");
            NLM_LOG_INFO("  tutorial <topic>   - Start a tutorial");
            NLM_LOG_INFO("  run_tests          - Run all tests");
            NLM_LOG_INFO("  explore config      - Explore configuration options");
            NLM_LOG_INFO("  set intermediate   - Set to intermediate mode");
            break;
        case UserLevel::Expert:
            NLM_LOG_INFO("=== NLM Expert Help ===");
            NLM_LOG_INFO("Available commands:");
            NLM_LOG_INFO("  help               - Show help information");
            NLM_LOG_INFO("  debug <command>    - Run debug commands");
            NLM_LOG_INFO("  explore config      - Explore configuration options");
            NLM_LOG_INFO("  run_advanced_tests - Run advanced tests");
            NLM_LOG_INFO("  set expert         - Set to expert mode");
            NLM_LOG_INFO("  raw_config output  - Enable raw configuration output");
            break;
    }
}

void Config::showTutorial(const std::string& topic) const {
    if (topic.empty() && !pImpl->userLevelConfig.currentHelpTopic.empty()) {
        return showTutorial(pImpl->userLevelConfig.currentHelpTopic);
    }
    
    NLM_LOG_INFO("=== NLM Tutorial: " + topic + " ===");
    
    switch (pImpl->userLevelConfig.userLevel) {
        case UserLevel::Beginner:
            if (topic == "getting_started") {
                NLM_LOG_INFO("Welcome to NLM! This tutorial will help you get started:");
                NLM_LOG_INFO("1. The brain consists of neurons that fire and communicate");
                NLM_LOG_INFO("2. Run 'run_basic_tests' to see basic neural activity");
                NLM_LOG_INFO("3. Use 'help' for more commands");
            }
            break;
        case UserLevel::Intermediate:
            if (topic == "intermediate") {
                NLM_LOG_INFO("Intermediate Tutorial: Understanding NLM configurations:");
                NLM_LOG_INFO("1. Configuration files control simulation parameters");
                NLM_LOG_INFO("2. You can load configurations with --config file.cfg");
                NLM_LOG_INFO("3. Use 'run_tests' to verify your setup");
            }
            break;
        case UserLevel::Expert:
            if (topic == "advanced") {
                NLM_LOG_INFO("Advanced Tutorial: NLM internal systems:");
                NLM_LOG_INFO("1. Brain systems: Working memory, Episodic memory, Neuromodulation");
                NLM_LOG_INFO("2. Plasticity: STDP, Hebbian, Structural");
                NLM_LOG_INFO("3. Development: Stages, maturation, plasticity changes");
                NLM_LOG_INFO("4. Use 'debug' commands for detailed information");
            }
            break;
    }
}

void Config::setBeginnerMode(bool enabled) {
    if (enabled) {
        setUserLevel(UserLevel::Beginner);
    }
}

void Config::setIntermediateMode(bool enabled) {
    if (enabled) {
        setUserLevel(UserLevel::Intermediate);
    }
}

void Config::setExpertMode(bool enabled) {
    if (enabled) {
        setUserLevel(UserLevel::Expert);
    }
}

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    UserLevelConfig userLevelConfig;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->userLevelConfig.setBeginnerMode();
}

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
                ((value.front() == '\"' && value.back() == '\"') ||
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

void Config::setUserLevel(UserLevel level) {
    pImpl->userLevelConfig.userLevel = level;
    
    // Reset settings based on user level
    switch (level) {
        case UserLevel::Beginner:
            pImpl->userLevelConfig.setBeginnerMode();
            pImpl->userLevelConfig.tutorialMode = true;
            pImpl->userLevelConfig.debugMode = false;
            break;
        case UserLevel::Intermediate:
            pImpl->userLevelConfig.setIntermediateMode();
            pImpl->userLevelConfig.tutorialMode = false;
            pImpl->userLevelConfig.debugMode = false;
            break;
        case UserLevel::Expert:
            pImpl->userLevelConfig.setExpertMode();
            pImpl->userLevelConfig.tutorialMode = false;
            pImpl->userLevelConfig.debugMode = true;
            break;
    }
}

UserLevel Config::getUserLevel() const {
    return pImpl->userLevelConfig.userLevel;
}

bool Config::isTutorialMode() const {
    return pImpl->userLevelConfig.tutorialMode;
}

bool Config::isDebugMode() const {
    return pImpl->userLevelConfig.debugMode;
}

bool Config::shouldShowGuidance() const {
    return pImpl->userLevelConfig.levelSettings.getBeginner().showGuidance;
}

bool Config::isAdvancedMode() const {
    return pImpl->userLevelConfig.levelSettings.getBeginner().advancedMode;
}

void Config::showHelp() const {
    // User-level specific help
    switch (pImpl->userLevelConfig.userLevel) {
        case UserLevel::Beginner:
            NLM_LOG_INFO("=== NLM Beginner Help ===");
            NLM_LOG_INFO("Available commands:");
            NLM_LOG_INFO("  help               - Show help information");
            NLM_LOG_INFO("  tutorial <topic>   - Start a tutorial");
            NLM_LOG_INFO("  run_basic_tests     - Run simple demonstration tests");
            NLM_LOG_INFO("  set beginner       - Set to beginner mode");
            NLM_LOG_INFO("Use 'tutorial getting_started' for a quick start guide.");
            break;
        case UserLevel::Intermediate:
            NLM_LOG_INFO("=== NLM Intermediate Help ===");
            NLM_LOG_INFO("Available commands:");
            NLM_LOG_INFO("  help               - Show help information");
            NLM_LOG_INFO("  tutorial <topic>   - Start a tutorial");
            NLM_LOG_INFO("  run_tests          - Run all tests");
            NLM_LOG_INFO("  explore config      - Explore configuration options");
            NLM_LOG_INFO("  set intermediate   - Set to intermediate mode");
            break;
        case UserLevel::Expert:
            NLM_LOG_INFO("=== NLM Expert Help ===");
            NLM_LOG_INFO("Available commands:");
            NLM_LOG_INFO("  help               - Show help information");
            NLM_LOG_INFO("  debug <command>    - Run debug commands");
            NLM_LOG_INFO("  explore config      - Explore configuration options");
            NLM_LOG_INFO("  run_advanced_tests - Run advanced tests");
            NLM_LOG_INFO("  set expert         - Set to expert mode");
            NLM_LOG_INFO("  raw_config output  - Enable raw configuration output");
            break;
    }
}

void Config::showTutorial(const std::string& topic) const {
    if (topic.empty() && !pImpl->userLevelConfig.currentHelpTopic.empty()) {
        return showTutorial(pImpl->userLevelConfig.currentHelpTopic);
    }
    
    NLM_LOG_INFO("=== NLM Tutorial: " + topic + " ===");
    
    switch (pImpl->userLevelConfig.userLevel) {
        case UserLevel::Beginner:
            if (topic == "getting_started") {
                NLM_LOG_INFO("Welcome to NLM! This tutorial will help you get started:");
                NLM_LOG_INFO("1. The brain consists of neurons that fire and communicate");
                NLM_LOG_INFO("2. Run 'run_basic_tests' to see basic neural activity");
                NLM_LOG_INFO("3. Use 'help' for more commands");
            }
            break;
        case UserLevel::Intermediate:
            if (topic == "intermediate") {
                NLM_LOG_INFO("Intermediate Tutorial: Understanding NLM configurations:");
                NLM_LOG_INFO("1. Configuration files control simulation parameters");
                NLM_LOG_INFO("2. You can load configurations with --config file.cfg");
                NLM_LOG_INFO("3. Use 'run_tests' to verify your setup");
            }
            break;
        case UserLevel::Expert:
            if (topic == "advanced") {
                NLM_LOG_INFO("Advanced Tutorial: NLM internal systems:");
                NLM_LOG_INFO("1. Brain systems: Working memory, Episodic memory, Neuromodulation");
                NLM_LOG_INFO("2. Plasticity: STDP, Hebbian, Structural");
                NLM_LOG_INFO("3. Development: Stages, maturation, plasticity changes");
                NLM_LOG_INFO("4. Use 'debug' commands for detailed information");
            }
            break;
    }
}

void Config::setBeginnerMode(bool enabled) {
    if (enabled) {
        setUserLevel(UserLevel::Beginner);
    }
}

void Config::setIntermediateMode(bool enabled) {
    if (enabled) {
        setUserLevel(UserLevel::Intermediate);
    }
}

void Config::setExpertMode(bool enabled) {
    if (enabled) {
        setUserLevel(UserLevel::Expert);
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

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
