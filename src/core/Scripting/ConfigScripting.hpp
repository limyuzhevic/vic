// Configuration scripting system for NLM
// Provides support for JSON, YAML, and Python-like configuration files
// Includes parameter validation and template support

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "core/Config/UnifiedConfig.hpp"

namespace nlm {

/**
 * Configuration parser base class
 */
class ConfigParser {
public:
    virtual ~ConfigParser() = default;
    
    virtual bool parse(const std::string& filename, std::shared_ptr<Config> config) = 0;
    
    virtual std::string getFormatName() const = 0;
};

/**
 * JSON configuration parser
 */
class JSONConfigParser : public ConfigParser {
public:
    bool parse(const std::string& filename, std::shared_ptr<Config> config) override {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();
        
        return parseJSON(content, config);
    }
    
    bool parseJSON(const std::string& json, std::shared_ptr<Config> config) {
        // Simplified JSON parsing - in practice, you'd use a proper JSON library
        size_t pos = 0;
        while ((pos = json.find('"', pos)) != std::string::npos) {
            size_t keyStart = pos;
            pos = json.find('"', keyStart + 1);
            if (pos == std::string::npos) break;
            
            std::string key = json.substr(keyStart + 1, pos - keyStart - 1);
            
            pos = json.find(':', pos);
            if (pos == std::string::npos) break;
            
            size_t valueStart = json.find_first_not_of(": \t", pos);
            if (valueStart == std::string::npos) break;
            
            // Find the end of the value (simplified)
            size_t valueEnd = json.find_first_of(",}", valueStart);
            if (valueEnd == std::string::npos) valueEnd = json.length();
            
            std::string value = json.substr(valueStart, valueEnd - valueStart);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Remove quotes from key if present
            if (key.front() == '"' && key.back() == '"') {
                key = key.substr(1, key.length() - 2);
            }
            
            // Parse value
            try {
                if (value == "true" || value == "false") {
                    bool boolValue = (value == "true");
                    config->set<bool>(key, boolValue, "json:" + filename);
                } else if (value.find('.') != std::string::npos) {
                    double doubleValue = std::stod(value);
                    config->set<double>(key, doubleValue, "json:" + filename);
                } else {
                    try {
                        int64_t intValue = std::stoll(value);
                        config->set<int64_t>(key, intValue, "json:" + filename);
                    } catch (...) {
                        try {
                            float floatValue = std::stof(value);
                            config->set<float>(key, floatValue, "json:" + filename);
                        } catch (...) {
                            // String value
                            config->set<std::string>(key, value, "json:" + filename);
                        }
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Warning: Failed to parse JSON value for key '" 
                         << key << "': " << e.what() << std::endl;
            }
            
            pos = valueEnd;
        }
        
        return true;
    }
    
    std::string getFormatName() const override {
        return "JSON";
    }
};

/**
 * YAML configuration parser
 */
class YAMLConfigParser : public ConfigParser {
public:
    bool parse(const std::string& filename, std::shared_ptr<Config> config) override {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();
        
        return parseYAML(content, config);
    }
    
    bool parseYAML(const std::string& yaml, std::shared_ptr<Config> config) {
        // Simplified YAML parsing - in practice, you'd use a proper YAML library
        size_t pos = 0;
        while ((pos = yaml.find('- ', pos)) != std::string::npos) {
            size_t keyStart = yaml.find_first_of(": ", pos + 2);
            if (keyStart == std::string::npos) break;
            
            std::string key = yaml.substr(pos + 2, keyStart - pos - 2);
            
            size_t valueStart = yaml.find_first_not_of(": \t", keyStart + 1);
            if (valueStart == std::string::npos) break;
            
            // Find the end of the line
            size_t valueEnd = yaml.find_first_of('\n', valueStart);
            if (valueEnd == std::string::npos) valueEnd = yaml.length();
            
            std::string value = yaml.substr(valueStart, valueEnd - valueStart);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Parse value
            try {
                if (value == "true" || value == "false") {
                    bool boolValue = (value == "true");
                    config->set<bool>(key, boolValue, "yaml:" + filename);
                } else if (value.find('.') != std::string::npos) {
                    double doubleValue = std::stod(value);
                    config->set<double>(key, doubleValue, "yaml:" + filename);
                } else {
                    try {
                        int64_t intValue = std::stoll(value);
                        config->set<int64_t>(key, intValue, "yaml:" + filename);
                    } catch (...) {
                        try {
                            float floatValue = std::stof(value);
                            config->set<float>(key, floatValue, "yaml:" + filename);
                        } catch (...) {
                            // String value
                            config->set<std::string>(key, value, "yaml:" + filename);
                        }
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Warning: Failed to parse YAML value for key '" 
                         << key << "': " << e.what() << std::endl;
            }
            
            pos = valueEnd;
        }
        
        return true;
    }
    
    std::string getFormatName() const override {
        return "YAML";
    }
};

/**
 * Configuration manager for scriptable configuration
 */
class ConfigManager {
private:
    std::vector<std::unique_ptr<ConfigParser>> parsers_;
    std::shared_ptr<Config> defaultConfig_;
    
public:
    ConfigManager() {
        // Add available parsers
        parsers_.push_back(std::make_unique<JSONConfigParser>());
        parsers_.push_back(std::make_unique<YAMLConfigParser>());
        
        defaultConfig_ = std::make_shared<Config>();
    }
    
    /**
     * Load configuration from file (auto-detect format)
     */
    std::shared_ptr<Config> loadConfig(const std::string& filename) {
        auto config = std::make_shared<Config>();
        
        // Try each parser to find the right one
        for (const auto& parser : parsers_) {
            try {
                if (parser->parse(filename, config)) {
                    return config;
                }
            } catch (const std::exception& e) {
                std::cerr << "Warning: Parser " << parser->getFormatName() 
                         << " failed to parse " << filename << ": " << e.what() << std::endl;
            }
        }
        
        // If all parsers failed, fall back to basic config file
        std::ifstream file(filename);
        if (file.is_open()) {
            file.close();
            return defaultConfig_;
        }
        
        return nullptr;
    }
    
    /**
     * Load configuration from string content
     */
    std::shared_ptr<Config> loadConfigFromString(const std::string& content, const std::string& format = "auto") {
        auto config = std::make_shared<Config>();
        
        if (format == "auto") {
            // Try to detect format
            if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
                // Try JSON
                auto jsonParser = std::make_unique<JSONConfigParser>();
                if (jsonParser->parseJSON(content, config)) {
                    return config;
                }
            }
            
            if (content.find("- ") != std::string::npos && content.find('\n') != std::string::npos) {
                // Try YAML
                auto yamlParser = std::make_unique<YAMLConfigParser>();
                if (yamlParser->parseYAML(content, config)) {
                    return config;
                }
            }
        } else if (format == "json") {
            auto jsonParser = std::make_unique<JSONConfigParser>();
            jsonParser->parseJSON(content, config);
            return config;
        } else if (format == "yaml") {
            auto yamlParser = std::make_unique<YAMLConfigParser>();
            yamlParser->parseYAML(content, config);
            return config;
        } else if (format == "nlm") {
            // NLM format (key=value)
            std::stringstream ss(content);
            std::string line;
            while (std::getline(ss, line)) {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    
                    // Trim whitespace
                    key.erase(0, key.find_first_not_of(" \t"));
                    key.erase(key.find_last_not_of(" \t") + 1);
                    
                    value.erase(0, value.find_first_not_of(" \t"));
                    value.erase(value.find_last_not_of(" \t") + 1);
                    
                    config->setFromString(key, value, "script:nlm");
                }
            }
        }
        
        return config;
    }
    
    /**
     * Get configuration parsers
     */
    const std::vector<std::unique_ptr<ConfigParser>>& getParsers() const {
        return parsers_;
    }
    
    /**
     * Add a custom configuration parser
     */
    void addParser(std::unique_ptr<ConfigParser> parser) {
        parsers_.push_back(std::move(parser));
    }
    
    /**
     * Get default configuration
     */
    std::shared_ptr<Config> getDefaultConfig() const {
        return defaultConfig_;
    }
};

/**
 * Configuration template system for reusable experiment configurations
 */
class ConfigTemplate {
private:
    std::string name_;
    std::map<std::string, std::string> parameters_;
    std::vector<std::string> dependencies_;
    
public:
    ConfigTemplate(const std::string& name, const std::map<std::string, std::string>& params,
                   const std::vector<std::string>& deps = {})
        : name_(name), parameters_(params), dependencies_(deps) {}
    
    std::string getName() const { return name_; }
    
    std::string getParameter(const std::string& key) const {
        auto it = parameters_.find(key);
        return it != parameters_.end() ? it->second : "";
    }
    
    std::map<std::string, std::string> getParameters() const { return parameters_; }
    
    std::vector<std::string> getDependencies() const { return dependencies_; }
    
    /**
     * Apply template to configuration
     */
    void applyToConfig(std::shared_ptr<Config> config) {
        for (const auto& pair : parameters_) {
            config->setFromString(pair.first, pair.second, "template:" + name_);
        }
    }
    
    /**
     * Create config from template with overrides
     */
    static std::shared_ptr<Config> createFromTemplate(
        const ConfigTemplate& template, 
        const std::map<std::string, std::string>& overrides = {}) {
        
        std::shared_ptr<Config> config = std::make_shared<Config>();
        
        // Apply template parameters
        for (const auto& pair : template.getParameters()) {
            config->setFromString(pair.first, pair.second, "template:" + template.getName());
        }
        
        // Apply overrides
        for (const auto& pair : overrides) {
            config->setFromString(pair.first, pair.second, "override:" + template.getName());
        }
        
        return config;
    }
};

/**
 * Experiment configuration builder
 */
class ExperimentConfigBuilder {
private:
    std::map<std::string, std::string> config_;
    std::shared_ptr<Config> activeConfig_;
    
public:
    ExperimentConfigBuilder() {
        activeConfig_ = std::make_shared<Config>();
    }
    
    /**
     * Set a configuration parameter
     */
    ExperimentConfigBuilder& set(const std::string& key, const std::string& value) {
        config_[key] = value;
        activeConfig_->setFromString(key, value, "builder");
        return *this;
    }
    
    /**
     * Set a configuration parameter (integer)
     */
    ExperimentConfigBuilder& setInt(const std::string& key, int64_t value) {
        config_[key] = std::to_string(value);
        activeConfig_->set<int64_t>(key, value, "builder");
        return *this;
    }
    
    /**
     * Set a configuration parameter (float)
     */
    ExperimentConfigBuilder& setFloat(const std::string& key, float value) {
        config_[key] = std::to_string(value);
        activeConfig_->set<float>(key, value, "builder");
        return *this;
    }
    
    /**
     * Set a configuration parameter (boolean)
     */
    ExperimentConfigBuilder& setBool(const std::string& key, bool value) {
        std::string strValue = value ? "true" : "false";
        config_[key] = strValue;
        activeConfig_->set<bool>(key, value, "builder");
        return *this;
    }
    
    /**
     * Get current configuration
     */
    std::shared_ptr<Config> getConfig() const {
        return activeConfig_;
    }
    
    /**
     * Get configuration as string
     */
    std::string getConfigString() const {
        std::stringstream ss;
        for (const auto& pair : config_) {
            ss << pair.first << " = " << pair.second << std::endl;
        }
        return ss.str();
    }
    
    /**
     * Save configuration to file
     */
    bool saveToFile(const std::string& filename, const std::string& format = "nlm") {
        if (format == "nlm") {
            // Save as NLM format (key=value)
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            for (const auto& pair : config_) {
                file << pair.first << " = " << pair.second << std::endl;
            }
            
            file.close();
            return true;
        } else if (format == "json") {
            // Convert to JSON string
            std::stringstream json;
            json << "{" << std::endl;
            bool first = true;
            for (const auto& pair : config_) {
                if (!first) json << "," << std::endl;
                first = false;
                json << "  \"" << pair.first << "\": \"" << pair.second << "\"";
            }
            json << std::endl << "}";
            
            return saveStringToFile(json.str(), filename);
        } else if (format == "yaml") {
            // Convert to YAML string
            std::stringstream yaml;
            for (const auto& pair : config_) {
                yaml << pair.first << ": " << pair.second << std::endl;
            }
            
            return saveStringToFile(yaml.str(), filename);
        }
        
        return false;
    }
    
private:
    bool saveStringToFile(const std::string& content, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << content;
        file.close();
        return true;
    }
};

} // namespace nlm