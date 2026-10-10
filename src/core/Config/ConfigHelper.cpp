#include "ConfigHelper.hpp"
#include "ConfigJSON.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct ConfigHelper::Impl {
    std::unique_ptr<Config> config;
    std::unique_ptr<ConfigJSON> jsonConfig;
    SourceType sourceType;
    std::string filepath;
    std::string schemaFile;
    
    Impl() : sourceType(SourceType::Unknown), config(std::make_unique<Config>()), 
             jsonConfig(std::make_unique<ConfigJSON>()) {}
};

ConfigHelper::ConfigHelper() : pImpl(std::make_unique<Impl>()), sourceType_(SourceType::Unknown) {}

ConfigHelper::~ConfigHelper() = default;

ConfigHelper::ConfigHelper(ConfigHelper&& other) noexcept = default;

ConfigHelper& ConfigHelper::operator=(ConfigHelper&& other) noexcept = default;

bool ConfigHelper::loadFromFile(const std::string& filepath) {
    pImpl->filepath = filepath;
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        return loadFromString(content);
        
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigHelper::loadFromString(const std::string& content) {
    std::string detectedFormat = detectFormat(content);
    
    if (detectedFormat == "json") {
        return loadJSON(content);
    } else {
        return loadKeyValue(content);
    }
}

bool ConfigHelper::saveToFile(const std::string& filepath) const {
    pImpl->filepath = filepath;
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string json = toJSON(true); // Pretty print by default
        file << json;
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

template<typename T>
std::optional<T> ConfigHelper::get(const std::string& key) const {
    return pImpl->config->get<T>(key);
}

template<typename T>
bool ConfigHelper::set(const std::string& key, const T& value) {
    return setValue(key, ConfigValue(value));
}

bool ConfigHelper::has(const std::string& key) const {
    return pImpl->config->has(key);
}

std::vector<std::string> ConfigHelper::getKeys() const {
    return pImpl->config->getKeys();
}

std::string ConfigHelper::getFilepath() const {
    return pImpl->filepath;
}

std::string ConfigHelper::getSummary() const {
    return pImpl->config->summary();
}

void ConfigHelper::clear() {
    pImpl->config->clear();
    pImpl->jsonConfig->getConfig().clear();
    sourceType_ = SourceType::Unknown;
}

std::string ConfigHelper::toJSON(bool pretty) const {
    pImpl->jsonConfig->setPrettyPrint(pretty);
    return pImpl->jsonConfig->toString();
}

bool ConfigHelper::validateSchema(const std::string& schemaFile) const {
    pImpl->schemaFile = schemaFile;
    return pImpl->jsonConfig->setValidateSchema(true, schemaFile);
}

bool ConfigHelper::merge(const ConfigHelper& other) {
    // Merge other config into this one
    for (const auto& key : other.getKeys()) {
        auto value = other.getValue(key);
        setValue(key, value);
    }
    return true;
}

ConfigHelper ConfigHelper::createDefaultConfig() {
    ConfigHelper helper;
    
    // Set some sensible defaults for NLM
    helper.set("brain.neuron.count", 10000);
    helper.set("brain.simulation.dt", 0.001);
    helper.set("brain.simulation.maxTime", 600.0);
    helper.set("memory.working.capacity", 100);
    helper.set("memory.episodic.capacity", 1000);
    helper.set("neuromodulation.dopamine.level", 0.5);
    helper.set("neuromodulation.curiosity.threshold", 0.3);
    helper.set("development.stage.duration", 300.0);
    helper.set("environment.world.size", 50.0);
    helper.set("environment.vision.fov", 90.0);
    helper.set("motor.actions.count", 6);
    
    return helper;
}

std::string ConfigHelper::createExampleConfig() {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"brain.neuron.count\": 10000,\n";
    oss << "  \"brain.simulation.dt\": 0.001,\n";
    oss << "  \"brain.simulation.maxTime\": 600.0,\n";
    oss << "  \"memory.working.capacity\": 100,\n";
    oss << "  \"memory.episodic.capacity\": 1000,\n";
    oss << "  \"neuromodulation.dopamine.level\": 0.5,\n";
    oss << "  \"neuromodulation.curiosity.threshold\": 0.3,\n";
    oss << "  \"development.stage.duration\": 300.0,\n";
    oss << "  \"environment.world.size\": 50.0,\n";
    oss << "  \"environment.vision.fov\": 90.0,\n";
    oss << "  \"motor.actions.count\": 6\n";
    oss << "}\n";
    return oss.str();
}

bool ConfigHelper::loadKeyValue(const std::string& content) {
    pImpl->config->clear();
    sourceType_ = SourceType::KeyValue;
    
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
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
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to parse value to appropriate type
            setValue(key, parseValue(value));
        }
    }
    
    return true;
}

bool ConfigHelper::loadJSON(const std::string& content) {
    pImpl->jsonConfig->clear();
    sourceType_ = SourceType::JSON;
    
    return pImpl->jsonConfig->loadFromString(content);
}

std::string ConfigHelper::detectFormat(const std::string& content) const {
    if (isJSON(content)) {
        return "json";
    } else if (isKeyValue(content)) {
        return "keyvalue";
    }
    return "unknown";
}

bool ConfigHelper::setValue(const std::string& key, const ConfigValue& value) {
    pImpl->config->set(key, value, ConfigSource::Runtime);
    return true;
}

ConfigValue ConfigHelper::getValue(const std::string& key) const {
    auto val = pImpl->config->get<std::string>(key);
    if (val) {
        return ConfigValue(*val);
    }
    
    // Try other types
    auto intVal = pImpl->config->get<int>(key);
    if (intVal) {
        return ConfigValue(*intVal);
    }
    
    auto doubleVal = pImpl->config->get<double>(key);
    if (doubleVal) {
        return ConfigValue(*doubleVal);
    }
    
    auto boolVal = pImpl->config->get<bool>(key);
    if (boolVal) {
        return ConfigValue(*boolVal);
    }
    
    return ConfigValue();
}

ConfigValue ConfigHelper::parseValue(const std::string& str) {
    if (str == "true" || str == "false") {
        bool boolVal = (str == "true");
        return ConfigValue(boolVal);
    }
    
    try {
        // Try integer
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            return ConfigValue(intVal);
        }
        
        // Try double
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return ConfigValue(doubleVal);
        }
        
        // Fall back to string
        return ConfigValue(str);
        
    } catch (const std::exception&) {
        return ConfigValue(str);
    }
}

std::string ConfigHelper::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ConfigHelper::isJSON(const std::string& content) {
    // Simple JSON detection - check for braces and quotes
    if (content.empty()) return false;
    
    bool hasBraces = (content.find('{') != std::string::npos || content.find('[') != std::string::npos);
    bool hasQuotes = (content.find('"') != std::string::npos);
    
    return hasBraces && hasQuotes;
}

bool ConfigHelper::isKeyValue(const std::string& content) {
    // Simple key=value detection
    if (content.empty()) return false;
    
    size_t equalCount = std::count(content.begin(), content.end(), '=');
    return equalCount > 0;
}

} // namespace nlm
