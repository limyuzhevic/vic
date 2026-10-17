#include "ArgsParser.hpp"
#include <fstream>
#include <sstream>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <stdexcept>

namespace nlm {

struct ArgsParser::Impl {
    std::map<std::string, ArgsValue> parsedConfig;
    std::vector<std::string> errors;
    bool parseSuccess;
    std::shared_ptr<ArgsValidator> validator;
    std::shared_ptr<PresetManager> presetManager;
    std::map<std::string, ArgMetadata> argumentRegistry;
    std::map<std::string, Preset> presets;
    std::map<std::string, std::function<ArgsValue()>> presetFactories;
    std::vector<ConfigSchema> configSchema;
    std::chrono::system_clock::time_point lastModified;
    std::string lastLoadedFile;
    std::map<std::string, std::string> categoryDisplayNames;
    
    Impl() : parseSuccess(false), lastModified(std::chrono::system_clock::now()) {
        initializeCategoryDisplayNames();
    }
    
    void initializeCategoryDisplayNames() {
        categoryDisplayNames["Core"] = "Core Parameters";
        categoryDisplayNames["Brain"] = "Brain Configuration";
        categoryDisplayNames["Plasticity"] = "Plasticity Settings";
        categoryDisplayNames["Neuromodulation"] = "Neuromodulation Settings";
        categoryDisplayNames["Simulation"] = "Simulation Parameters";
        categoryDisplayNames["Output"] = "Output Configuration";
        categoryDisplayNames["Debugging"] = "Debugging Options";
    }
};

ArgsParser::ArgsParser() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default components
    pImpl->validator = std::make_shared<ArgsValidator>();
    pImpl->presetManager = std::make_shared<PresetManager>();
    
    // Setup presets and validation
    setupPresets();
    setupValidation();
}

ArgsParser::~ArgsParser() = default;

ArgsParser::ArgsParser(ArgsParser&&) noexcept = default;

ArgsParser& ArgsParser::operator=(ArgsParser&&) noexcept = default;

bool ArgsParser::parse(int argc, char** argv) {
    pImpl->errors.clear();
    pImpl->parseSuccess = false;
    
    try {
        // Parse command line arguments
        if (!parseArgs(argc, argv)) {
            return false;
        }
        
        // Apply defaults
        applyDefaults();
        
        // Validate configuration
        if (!validate(pImpl->errors)) {
            return false;
        }
        
        pImpl->parseSuccess = true;
        return true;
        
    } catch (const std::exception& e) {
        pImpl->errors.push_back("Parse error: " + std::string(e.what()));
        return false;
    }
}

bool ArgsParser::parseConfigFile(const std::string& filepath) {
    pImpl->errors.clear();
    
    try {
        return parseFile(filepath);
    } catch (const std::exception& e) {
        pImpl->errors.push_back("File parse error: " + std::string(e.what()));
        return false;
    }
}

bool ArgsParser::loadConfig(const std::string& source, bool isFile) {
    pImpl->errors.clear();
    
    if (isFile) {
        // Detect file format and load
        FileFormatInfo formatInfo = FileFormatDetector::detectFormat(source);
        pImpl->lastLoadedFile = source;
        
        if (!formatInfo.isValid) {
            pImpl->errors.push_back("Cannot detect format for file: " + source);
            return false;
        }
        
        // Load based on detected format
        if (formatInfo.detectedFormat == "JSON") {
            // TODO: Implement JSON parser
            pImpl->errors.push_back("JSON format not yet implemented");
            return false;
        } else if (formatInfo.detectedFormat == "YAML") {
            // TODO: Implement YAML parser
            pImpl->errors.push_back("YAML format not yet implemented");
            return false;
        } else if (formatInfo.detectedFormat == "TOML") {
            // TODO: Implement TOML parser
            pImpl->errors.push_back("TOML format not yet implemented");
            return false;
        } else {
            // Use plain text parser
            return parseFile(source);
        }
    } else {
        // Load from command line or other source
        detectAndLoadConfig(source);
    }
    
    return isValid();
}

template<typename T>
std::optional<T> ArgsParser::get(const std::string& key) const {
    auto it = pImpl->parsedConfig.find(key);
    if (it == pImpl->parsedConfig.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->second);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T ArgsParser::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool ArgsParser::has(const std::string& key) const {
    return pImpl->parsedConfig.find(key) != pImpl->parsedConfig.end();
}

std::vector<std::string> ArgsParser::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->parsedConfig.size());
    for (const auto& pair : pImpl->parsedConfig) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::string ArgsParser::summary() const {
    return SummaryFormatter::format(pImpl->parsedConfig);
}

std::string ArgsParser::getHelp() const {
    std::vector<std::string> categories;
    for (const auto& pair : pImpl->argumentRegistry) {
        ArgsCategory category = pair.second.category;
        std::string categoryName;
        switch (category) {
            case ArgsCategory::Core: categoryName = "Core"; break;
            case ArgsCategory::Brain: categoryName = "Brain"; break;
            case ArgsCategory::Plasticity: categoryName = "Plasticity"; break;
            case ArgsCategory::Neuromodulation: categoryName = "Neuromodulation"; break;
            case ArgsCategory::Simulation: categoryName = "Simulation"; break;
            case ArgsCategory::Output: categoryName = "Output"; break;
            case ArgsCategory::Debugging: categoryName = "Debugging"; break;
        }
        if (std::find(categories.begin(), categories.end(), categoryName) == categories.end()) {
            categories.push_back(categoryName);
        }
    }
    
    return HelpFormatter::format(pImpl->argumentRegistry, categories);
}

std::vector<std::string> ArgsParser::getErrors() const {
    return pImpl->errors;
}

void ArgsParser::addPreset(const Preset& preset) {
    pImpl->presets[preset.name] = preset;
}

void ArgsParser::registerArgument(const ArgMetadata& arg) {
    pImpl->argumentRegistry[arg.name] = arg;
}

void ArgsParser::registerPresetFactory(const std::string& name,
                                      const std::function<ArgsValue()>& factory) {
    pImpl->presetFactories[name] = factory;
}

void ArgsParser::set(const std::string& key, const ArgsValue& value,
                    ConfigSource source) {
    pImpl->parsedConfig[key] = value;
}

void ArgsParser::remove(const std::string& key) {
    pImpl->parsedConfig.erase(key);
}

std::shared_ptr<Config> ArgsParser::getConfigObject() const {
    auto config = std::make_shared<Config>();
    
    for (const auto& pair : pImpl->parsedConfig) {
        try {
            if (std::holds_alternative<int>(pair.second)) {
                config->set(pair.first, std::get<int>(pair.second), ConfigSource::CommandLine);
            } else if (std::holds_alternative<int64_t>(pair.second)) {
                config->set(pair.first, std::get<int64_t>(pair.second), ConfigSource::CommandLine);
            } else if (std::holds_alternative<double>(pair.second)) {
                config->set(pair.first, std::get<double>(pair.second), ConfigSource::CommandLine);
            } else if (std::holds_alternative<bool>(pair.second)) {
                config->set(pair.first, std::get<bool>(pair.second), ConfigSource::CommandLine);
            } else if (std::holds_alternative<std::string>(pair.second)) {
                config->set(pair.first, std::get<std::string>(pair.second), ConfigSource::CommandLine);
            } else if (std::holds_alternative<std::vector<int>>(pair.second)) {
                // For vectors, convert to string representation
                std::vector<int> vec = std::get<std::vector<int>>(pair.second);
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) ss << ",";
                    ss << vec[i];
                }
                ss << "]";
                config->set(pair.first, ss.str(), ConfigSource::CommandLine);
            } else if (std::holds_alternative<std::vector<double>>(pair.second)) {
                std::vector<double> vec = std::get<std::vector<double>>(pair.second);
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) ss << ",";
                    ss << vec[i];
                }
                ss << "]";
                config->set(pair.first, ss.str(), ConfigSource::CommandLine);
            } else if (std::holds_alternative<std::vector<std::string>>(pair.second)) {
                std::vector<std::string> vec = std::get<std::vector<std::string>>(pair.second);
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) ss << ",";
                    ss << "\"" << vec[i] << "\"";
                }
                ss << "]";
                config->set(pair.first, ss.str(), ConfigSource::CommandLine);
            }
        } catch (...) {
            // Skip unconvertible values
            continue;
        }
    }
    
    return config;
}

bool ArgsParser::validate(std::vector<std::string>& errors) const {
    if (!pImpl->validator) {
        return true;
    }
    
    return pImpl->validator->validateAll(pImpl->parsedConfig, errors);
}

bool ArgsParser::applyPreset(const std::string& presetName) {
    const Preset* preset = pImpl->presetManager->getPreset(presetName);
    if (!preset) {
        pImpl->errors.push_back("Preset not found: " + presetName);
        return false;
    }
    
    return pImpl->presetManager->applyPreset(presetName, pImpl->parsedConfig);
}

std::vector<std::string> ArgsParser::getCategories() const {
    std::vector<std::string> categories;
    for (const auto& pair : pImpl->argumentRegistry) {
        ArgsCategory category = pair.second.category;
        std::string categoryName;
        switch (category) {
            case ArgsCategory::Core: categoryName = "Core"; break;
            case ArgsCategory::Brain: categoryName = "Brain"; break;
            case ArgsCategory::Plasticity: categoryName = "Plasticity"; break;
            case ArgsCategory::Neuromodulation: categoryName = "Neuromodulation"; break;
            case ArgsCategory::Simulation: categoryName = "Simulation"; break;
            case ArgsCategory::Output: categoryName = "Output"; break;
            case ArgsCategory::Debugging: categoryName = "Debugging"; break;
        }
        if (std::find(categories.begin(), categories.end(), categoryName) == categories.end()) {
            categories.push_back(categoryName);
        }
    }
    return categories;
}

std::vector<ArgMetadata> ArgsParser::getArgumentsInCategory(ArgsCategory category) const {
    std::vector<ArgMetadata> args;
    for (const auto& pair : pImpl->argumentRegistry) {
        if (pair.second.category == category) {
            args.push_back(pair.second);
        }
    }
    return args;
}

FileFormatInfo ArgsParser::detectFileFormat(const std::string& filepath) const {
    return FileFormatDetector::detectFormat(filepath);
}

std::vector<std::string> ArgsParser::generateCompletion(const std::string& prefix) const {
    std::vector<std::string> completions;
    
    for (const auto& pair : pImpl->argumentRegistry) {
        const auto& arg = pair.second;
        
        if (arg.hasLongOption && arg.longOption.find(prefix) == 0) {
            completions.push_back("--" + arg.longOption);
        }
        
        if (arg.hasShortOption && arg.shortOption.find(prefix) == 0) {
            completions.push_back("-" + arg.shortOption);
        }
        
        if (arg.name.find(prefix) == 0) {
            completions.push_back("--" + arg.name);
        }
    }
    
    return completions;
}

void ArgsParser::logConfig() const {
    if (pImpl->parsedConfig.empty()) {
        std::cout << "No configuration loaded." << std::endl;
        return;
    }
    
    std::cout << "=== Configuration Summary ===" << std::endl;
    std::cout << "Loaded " << pImpl->parsedConfig.size() << " configuration entries:" << std::endl;
    
    for (const auto& pair : pImpl->parsedConfig) {
        std::cout << "  " << pair.first << ": " << ValueFormatter::format(pair.second) << std::endl;
    }
    
    if (!pImpl->errors.empty()) {
        std::cout << std::endl << "Errors:" << std::endl;
        for (const auto& error : pImpl->errors) {
            std::cout << "  " << error << std::endl;
        }
    }
}

// Private implementation methods

bool ArgsParser::parseArgs(int argc, char** argv) {
    std::vector<CmdArg> cmdArgs = CmdLineParser::parse(argc, argv);
    updateConfigFromArgs(cmdArgs);
    
    // Detect and load additional configs
    for (const auto& cmdArg : cmdArgs) {
        if (cmdArg.longOption == "config" || cmdArg.longOption == "config-file") {
            if (!loadConfig(cmdArg.value, true)) {
                pImpl->errors.push_back("Failed to load config file: " + cmdArg.value);
            }
        }
    }
    
    return true;
}

bool ArgsParser::parseFile(const std::string& filepath) {
    pImpl->lastLoadedFile = filepath;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->errors.push_back("Cannot open file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        ++lineNum;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to parse value based on registered arguments
            auto it = pImpl->argumentRegistry.find(key);
            if (it != pImpl->argumentRegistry.end()) {
                const auto& arg = it->second;
                ArgsValue parsedValue = parseValue(value);
                set(key, parsedValue, ConfigSource::File);
            } else {
                // Try to infer type
                ArgsValue parsedValue = parseValue(value);
                set(key, parsedValue, ConfigSource::File);
            }
        }
    }
    
    pImpl->lastModified = std::chrono::system_clock::now();
    return true;
}

void ArgsParser::detectAndLoadConfig(const std::string& source) {
    // Check if source is a file or command line argument
    std::filesystem::path path(source);
    if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path)) {
        loadConfig(source, true);
    } else {
        // Treat as command line-like argument
        // This would typically be processed during parseArgs
        std::vector<CmdArg> cmdArgs;
        // TODO: Parse source as if it were command line
    }
}

void ArgsParser::applyDefaults() {
    // Apply default values from schema
    for (const auto& schema : pImpl->configSchema) {
        if (!has(schema.key)) {
            set(schema.key, schema.defaultValue, schema.source);
        }
    }
}

void ArgsParser::updateConfigFromArgs(const std::vector<CmdArg>& cmdArgs) {
    for (const auto& cmdArg : cmdArgs) {
        std::string key;
        std::string value;
        
        if (!cmdArg.longOption.empty()) {
            key = cmdArg.longOption;
        } else if (!cmdArg.shortOption.empty()) {
            key = cmdArg.shortOption;
        }
        
        value = cmdArg.value;
        
        // Normalize key name
        key = normalizeKey(key);
        
        // Parse value
        ArgsValue parsedValue = parseValue(value);
        
        // Set in configuration
        set(key, parsedValue, ConfigSource::CommandLine);
    }
}

std::string ArgsParser::formatHelp(ArgsCategory category) const {
    std::string categoryName;
    switch (category) {
        case ArgsCategory::Core: categoryName = "Core Parameters"; break;
        case ArgsCategory::Brain: categoryName = "Brain Configuration"; break;
        case ArgsCategory::Plasticity: categoryName = "Plasticity Settings"; break;
        case ArgsCategory::Neuromodulation: categoryName = "Neuromodulation Settings"; break;
        case ArgsCategory::Simulation: categoryName = "Simulation Parameters"; break;
        case ArgsCategory::Output: categoryName = "Output Configuration"; break;
        case ArgsCategory::Debugging: categoryName = "Debugging Options"; break;
    }
    
    return "=== " + categoryName + " ===\n" + 
           HelpFormatter::formatCategoryHelp(categoryName, pImpl->argumentRegistry);
}

std::string ArgsParser::formatArgument(const ArgMetadata& arg) const {
    return HelpFormatter::formatArgumentHelp(arg);
}

ArgsValue ArgsParser::parseValue(const std::string& str) const {
    if (str.empty()) {
        return std::string();
    }
    
    // Try to parse as boolean
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try to parse as int64_t
    try {
        size_t pos;
        long long intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Parse as string (check for vector format)
    if (str.front() == '[' && str.back() == ']') {
        // Parse vector
        std::string content = str.substr(1, str.size() - 2);
        if (content.empty()) {
            return std::vector<int>{};
        }
        
        // Try to parse as vector of doubles first
        std::vector<double> doubleVec;
        std::stringstream ss(content);
        std::string token;
        while (std::getline(ss, token, ',')) {
            token = trim(token);
            try {
                double val = std::stod(token);
                doubleVec.push_back(val);
            } catch (...) {
                break;
            }
        }
        
        if (!doubleVec.empty()) {
            return doubleVec;
        }
        
        // Try to parse as vector of strings
        std::vector<std::string> stringVec;
        ss.clear();
        ss.str(content);
        while (std::getline(ss, token, ',')) {
            token = trim(token);
            if (token.size() >= 2 && ((token.front() == '\"' && token.back() == '\"') ||
                                     (token.front() == '\'' && token.back() == '\''))) {
                token = token.substr(1, token.size() - 2);
            }
            stringVec.push_back(token);
        }
        
        return stringVec;
    }
    
    // Default to string
    return str;
}

bool ArgsParser::detectFileFormatAndLoad(const std::string& filepath) {
    FileFormatInfo formatInfo = FileFormatDetector::detectFormat(filepath);
    
    if (!formatInfo.isValid) {
        pImpl->errors.push_back("Cannot detect format for file: " + filepath);
        return false;
    }
    
    // For now, use plain text parser
    return parseFile(filepath);
}

void ArgsParser::processCompletionRequest(const std::vector<CmdArg>& cmdArgs) {
    // Generate command completion suggestions
    for (const auto& cmdArg : cmdArgs) {
        if (cmdArg.longOption == "complete") {
            std::string prefix = cmdArg.value;
            auto completions = generateCompletion(prefix);
            
            for (const auto& completion : completions) {
                std::cout << completion << std::endl;
            }
            
            // Exit after generating completions
            std::exit(0);
        }
    }
}

void ArgsParser::initializeDefaults() {
    // Initialize with default configuration
    // Core parameters
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.15f, ConfigSource::Default);
    set("enable_neuromodulation", true, ConfigSource::Default);
    
    // Brain parameters
    set("brain_type", std::string("standard"), ConfigSource::Default);
    set("enable_plasticity", true, ConfigSource::Default);
    
    // Plasticity parameters
    set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Neuromodulation parameters
    set("curiosity_strength", 0.5f, ConfigSource::Default);
    set("novelty_threshold", 0.3f, ConfigSource::Default);
    set("reward_decay", 0.95f, ConfigSource::Default);
    
    // Simulation parameters
    set("max_steps", static_cast<int64_t>(1000), ConfigSource::Default);
    set("enable_checkpointing", false, ConfigSource::Default);
    
    // Output parameters
    set("output_dir", std::string("outputs"), ConfigSource::Default);
    set("save_weights", true, ConfigSource::Default);
    
    // Debugging parameters
    set("log_level", std::string("info"), ConfigSource::Default);
    set("debug_neurons", std::vector<std::string>{"1"}, ConfigSource::Default);
    set("enable_profiling", false, ConfigSource::Default);
}

void ArgsParser::setupPresets() {
    // Standard preset
    Preset standardPreset("standard", "Standard configuration for general use", 1.0);
    addPreset(standardPreset);
    
    // Fast preset (optimized for speed)
    Preset fastPreset("fast", "Fast configuration optimized for speed over accuracy", 0.8);
    addPreset(fastPreset);
    
    // High precision preset (optimized for accuracy)
    Preset highPrecisionPreset("high-precision", "High precision configuration optimized for accuracy", 1.5);
    addPreset(highPrecisionPreset);
    
    // Debug preset
    Preset debugPreset("debug", "Debug configuration with detailed logging", 2.0);
    addPreset(debugPreset);
}

void ArgsParser::setupValidation() {
    // Add validation rules
    pImpl->validator->addRangeValidation("random_seed", 0, std::numeric_limits<int64_t>::max(),
                                        "Random seed must be positive");
    
    pImpl->validator->addRangeValidation("simulation_timestep", 0.0, 1.0,
                                        "Simulation timestep must be between 0 and 1");
    
    pImpl->validator->addRangeValidation("neuron_count", 1, std::numeric_limits<int64_t>::max(),
                                        "Neuron count must be positive");
    
    pImpl->validator->addRangeValidation("region_count", 1, std::numeric_limits<int64_t>::max(),
                                        "Region count must be positive");
    
    pImpl->validator->addRangeValidation("connection_probability", 0.0, 1.0,
                                        "Connection probability must be between 0 and 1");
    
    pImpl->validator->addDependencyValidation("enable_checkpointing", "output_dir",
                                             "Checkpointing requires output directory");
}

std::string ArgsParser::normalizeKey(const std::string& key) {
    std::string normalized = key;
    
    // Convert to lowercase
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    return normalized;
}

// FileFormatDetector implementation

FileFormatInfo FileFormatDetector::detectFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return FileFormatInfo(filepath, "UNKNOWN", false, 0.0);
    }
    
    std::string line;
    std::getline(file, line);
    file.close();
    
    double jsonConfidence = tryDetectJSON(filepath) ? 0.9 : 0.0;
    double yamlConfidence = tryDetectYAML(filepath) ? 0.9 : 0.0;
    double tomlConfidence = tryDetectTOML(filepath) ? 0.9 : 0.0;
    double iniConfidence = tryDetectINI(filepath) ? 0.8 : 0.0;
    double propertiesConfidence = tryDetectProperties(filepath) ? 0.8 : 0.0;
    double plainConfidence = tryDetectPlain(filepath) ? 0.7 : 0.0;
    
    // Find format with highest confidence
    double maxConfidence = std::max({jsonConfidence, yamlConfidence, tomlConfidence, 
                                    iniConfidence, propertiesConfidence, plainConfidence});
    
    if (maxConfidence == 0.0) {
        return FileFormatInfo(filepath, "UNKNOWN", false, maxConfidence);
    }
    
    std::string format;
    if (jsonConfidence == maxConfidence) format = "JSON";
    else if (yamlConfidence == maxConfidence) format = "YAML";
    else if (tomlConfidence == maxConfidence) format = "TOML";
    else if (iniConfidence == maxConfidence) format = "INI";
    else if (propertiesConfidence == maxConfidence) format = "PROPERTIES";
    else format = "PLAIN";
    
    return FileFormatInfo(filepath, format, true, maxConfidence);
}

bool FileFormatDetector::tryDetectJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    // Read first 1KB to analyze
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Check for JSON structure
    if (content.find('{') != std::string::npos || content.find('[') != std::string::npos) {
        // Count braces/brackets
        int braces = std::count(content.begin(), content.end(), '{');
        int brackets = std::count(content.begin(), content.end(), '[');
        if (braces > 0 || brackets > 0) {
            return true;
        }
    }
    
    return false;
}

bool FileFormatDetector::tryDetectYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Check for YAML structure
    size_t colonPos = content.find(':');
    if (colonPos != std::string::npos && colonPos < 100) {
        return true;
    }
    
    return false;
}

bool FileFormatDetector::tryDetectTOML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Check for TOML structure
    if (content.find("[") != std::string::npos && content.find("]") != std::string::npos) {
        return true;
    }
    
    return false;
}

bool FileFormatDetector::tryDetectINI(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool FileFormatDetector::tryDetectProperties(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool FileFormatDetector::tryDetectPlain(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string line;
    int lineCount = 0;
    while (std::getline(file, line) && lineCount < 100) {
        line = trim(line);
        if (!line.empty() && line[0] != '#' && line[0] != '/' && line.find('=') != std::string::npos) {
            lineCount++;
        }
    }
    
    return lineCount >= 5; // At least 5 key=value pairs
}

double FileFormatDetector::calculateConfidence(bool detected, const std::string& format) {
    if (!detected) return 0.0;
    
    // Different formats have different detection confidence
    if (format == "JSON" || format == "YAML" || format == "TOML") return 0.9;
    if (format == "INI" || format == "PROPERTIES") return 0.8;
    if (format == "PLAIN") return 0.7;
    
    return 0.5;
}

// CmdLineParser implementation

std::vector<CmdArg> CmdLineParser::parse(int argc, char** argv) {
    std::vector<CmdArg> cmdArgs;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        bool hasValue = false;
        std::string value;
        
        // Check if argument is an option
        if (isOption(arg)) {
            std::string option = extractOption(arg, hasValue);
            
            if (hasValue) {
                // Option has inline value (--key=value or -k value)
                if (arg.find('=') != std::string::npos) {
                    value = extractValue(arg);
                } else if (i + 1 < argc && !isOption(argv[i + 1])) {
                    value = argv[++i];
                }
            }
            
            cmdArgs.emplace_back("", option, value, hasValue);
        } else {
            // Positional argument (not handled in current implementation)
        }
    }
    
    return cmdArgs;
}

std::vector<CmdArg> CmdLineParser::parseFile(const std::string& filepath) {
    std::vector<CmdArg> cmdArgs;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        return cmdArgs;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        bool hasValue = false;
        std::string value;
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            value = line.substr(pos + 1);
            hasValue = true;
            
            // Remove quotes from value
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            cmdArgs.emplace_back(key, "", value, hasValue);
        }
    }
    
    return cmdArgs;
}

std::string CmdLineParser::extractOption(const std::string& arg, bool& hasValue) {
    hasValue = false;
    std::string option;
    
    if (hasLongOption(arg)) {
        option = arg.substr(2);  // Remove "--"
        hasValue = (arg.find('=') != std::string::npos);
    } else if (hasShortOption(arg)) {
        option = arg.substr(1);  // Remove "-"
        hasValue = false; // Short options are typically followed by space, not "="
    }
    
    return option;
}

std::string CmdLineParser::extractValue(const std::string& arg) {
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        return arg.substr(pos + 1);
    }
    return "";
}

bool CmdLineParser::isOption(const std::string& arg) {
    return (!arg.empty() && arg[0] == '-') && arg != "-" && arg != "--";
}

bool CmdLineParser::hasLongOption(const std::string& arg) {
    return arg.size() >= 3 && arg.substr(0, 2) == "--";
}

bool CmdLineParser::hasShortOption(const std::string& arg) {
    return arg.size() >= 2 && arg[0] == '-' && arg[1] != '-';
}

std::string CmdLineParser::normalizeOption(const std::string& opt) {
    std::string normalized = opt;
    
    // Convert to lowercase
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    return normalized;
}

// SummaryFormatter implementation

std::string SummaryFormatter::format(const std::map<std::string, ArgsValue>& config) {
    std::ostringstream oss;
    
    oss << "=== Configuration Summary ===" << std::endl;
    oss << "Total entries: " << config.size() << std::endl;
    oss << std::endl;
    
    // Group by category (simplified - in real implementation would need category mapping)
    std::map<std::string, std::vector<std::pair<std::string, ArgsValue>>> grouped;
    
    for (const auto& pair : config) {
        std::string category = "General";
        
        // Simple category detection based on key prefix
        if (pair.first.find("neuron") != std::string::npos ||
            pair.first.find("brain") != std::string::npos) {
            category = "Brain";
        } else if (pair.first.find("plasticity") != std::string::npos) {
            category = "Plasticity";
        } else if (pair.first.find("neuromod") != std::string::npos) {
            category = "Neuromodulation";
        } else if (pair.first.find("sim") != std::string::npos) {
            category = "Simulation";
        } else if (pair.first.find("output") != std::string::npos) {
            category = "Output";
        } else if (pair.first.find("debug") != std::string::npos) {
            category = "Debugging";
        }
        
        grouped[category].push_back(pair);
    }
    
    for (const auto& categoryGroup : grouped) {
        oss << "--- " << categoryGroup.first << " ---" << std::endl;
        for (const auto& entry : categoryGroup.second) {
            oss << "  " << entry.first << ": " << formatEntry(entry.first, entry.second) << std::endl;
        }
        oss << std::endl;
    }
    
    return oss.str();
}

std::string SummaryFormatter::formatCategory(const std::string& category,
                                             const std::map<std::string, ArgsValue>& config) {
    std::ostringstream oss;
    
    oss << "=== " << category << " ===" << std::endl;
    
    for (const auto& pair : config) {
        std::string entryCategory = "General";
        
        // Simple category detection based on key prefix
        if (pair.first.find("neuron") != std::string::npos ||
            pair.first.find("brain") != std::string::npos) {
            entryCategory = "Brain";
        } else if (pair.first.find("plasticity") != std::string::npos) {
            entryCategory = "Plasticity";
        } else if (pair.first.find("neuromod") != std::string::npos) {
            entryCategory = "Neuromodulation";
        } else if (pair.first.find("sim") != std::string::npos) {
            entryCategory = "Simulation";
        } else if (pair.first.find("output") != std::string::npos) {
            entryCategory = "Output";
        } else if (pair.first.find("debug") != std::string::npos) {
            entryCategory = "Debugging";
        }
        
        if (entryCategory == category) {
            oss << "  " << pair.first << ": " << formatEntry(pair.first, pair.second) << std::endl;
        }
    }
    
    return oss.str();
}

std::string SummaryFormatter::formatEntry(const std::string& key,
                                          const ArgsValue& value,
                                          const std::string& source) {
    std::ostringstream oss;
    
    oss << ValueFormatter::format(value);
    
    if (!source.empty()) {
        oss << " (" << source << ")";
    }
    
    return oss.str();
}

// HelpFormatter implementation

std::string HelpFormatter::format(const std::map<std::string, ArgMetadata>& args,
                                 const std::vector<std::string>& categories) {
    std::ostringstream oss;
    
    oss << "=== Argument Parser Help ===" << std::endl;
    oss << "Usage: program [options] [config-file]" << std::endl;
    oss << std::endl;
    
    for (const auto& category : categories) {
        oss << formatCategoryHelp(category, args) << std::endl;
    }
    
    oss << "=== Configuration File ===" << std::endl;
    oss << "  --config=<file>    Load configuration from file" << std::endl;
    oss << std::endl;
    
    oss << "=== Presets ===" << std::endl;
    oss << "  --preset=<name>    Apply configuration preset (standard, fast, high-precision, debug)" << std::endl;
    oss << std::endl;
    
    oss << "=== Examples ===" << std::endl;
    oss << "  nlm --neuron_count=1000 --simulation_timestep=0.01" << std::endl;
    oss << "  nlm --preset=debug --config=myconfig.cfg" << std::endl;
    oss << "  nlm --enable_plasticity=true --stdp_ltp_weight=0.05" << std::endl;
    
    return oss.str();
}

std::string HelpFormatter::formatCategoryHelp(const std::string& category,
                                             const std::map<std::string, ArgMetadata>& args) {
    std::ostringstream oss;
    
    oss << "=== " << category << " ===" << std::endl;
    
    for (const auto& pair : args) {
        const auto& arg = pair.second;
        
        // Check if argument belongs to this category
        std::string argCategory;
        if (category == "Core") argCategory = "Core";
        else if (category == "Brain") argCategory = "Brain";
        else if (category == "Plasticity") argCategory = "Plasticity";
        else if (category == "Neuromodulation") argCategory = "Neuromodulation";
        else if (category == "Simulation") argCategory = "Simulation";
        else if (category == "Output") argCategory = "Output";
        else if (category == "Debugging") argCategory = "Debugging";
        else argCategory = "General";
        
        if (argCategory != category) continue;
        
        oss << "  " << formatArgumentHelp(arg) << std::endl;
    }
    
    return oss.str();
}

std::string HelpFormatter::formatArgumentHelp(const ArgMetadata& arg) {
    std::ostringstream oss;
    
    // Build option string
    std::string options;
    if (arg.hasShortOption) {
        options += "-" + arg.shortOption;
        if (arg.hasLongOption) {
            options += ", ";
        }
    }
    
    if (arg.hasLongOption) {
        options += "--" + arg.longOption;
    }
    
    oss << "  " << std::left << std::setw(30) << options;
    oss << "  " << arg.description;
    
    if (!arg.example.empty()) {
        oss << "  Example: " << arg.example;
    }
    
    if (arg.required) {
        oss << "  [Required]";
    }
    
    oss << std::endl;
    
    return oss.str();
}

// ValueFormatter implementation

std::string ValueFormatter::format(const ArgsValue& value) {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::fixed << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        }
    }, value);
    
    return oss.str();
}

std::string ValueFormatter::format(const ArgsValue& value, const std::string& type) {
    return format(value);
}

ArgsValue ValueFormatter::parse(const std::string& str, const std::string& type) {
    return parse(str);
}

// Helper function for string trimming

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm