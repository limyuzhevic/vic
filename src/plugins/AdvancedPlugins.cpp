// Advanced Plugin Implementation
// This plugin provides enhanced configuration validation and processing

#include "plugins/PluginInterface.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace nlm {

class AdvancedValidationPlugin : public IPlugin {
public:
    std::string getName() const override {
        return "AdvancedValidationPlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string getDescription() const override {
        return "Provides advanced configuration validation and processing";
    }
    
    void onLoad() override {
        NLM_LOG_INFO("AdvancedValidationPlugin loaded successfully");
    }
    
    void onUnload() override {
        NLM_LOG_INFO("AdvancedValidationPlugin unloaded successfully");
    }
    
    bool canHandle(const std::string& key) const override {
        // Handle advanced configuration keys
        return key.find("validation") != std::string::npos ||
               key.find("advanced") != std::string::npos ||
               key.find("performance") != std::string::npos;
    }
    
    std::string handleConfig(const std::string& key, const std::string& value) override {
        std::stringstream result;
        
        if (key == "validation.strict_mode" && value == "true") {
            result << "strict_" << value;
            NLM_LOG_INFO("Plugin: Strict validation mode enabled for " + key);
        } else if (key == "performance.optimization.level") {
            result << value << "_enhanced";
            NLM_LOG_INFO("Plugin: Enhanced optimization level for " + key);
        } else if (key == "logging.format" && value == "json") {
            result << value << "_pretty";
            NLM_LOG_INFO("Plugin: Pretty JSON logging format for " + key);
        } else {
            result << value;
        }
        
        return result.str();
    }
};

class PerformanceEnhancementPlugin : public IPlugin {
public:
    std::string getName() const override {
        return "PerformanceEnhancementPlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string getDescription() const override {
        return "Provides performance optimization and monitoring capabilities";
    }
    
    void onLoad() override {
        NLM_LOG_INFO("PerformanceEnhancementPlugin loaded successfully");
    }
    
    void onUnload() override {
        NLM_LOG_INFO("PerformanceEnhancementPlugin unloaded successfully");
    }
    
    bool canHandle(const std::string& key) const override {
        return key.find("performance") != std::string::npos ||
               key.find("threading") != std::string::npos ||
               key.find("optimization") != std::string::npos;
    }
    
    std::string handleConfig(const std::string& key, const std::string& value) override {
        // Simulate performance enhancement
        if (key == "performance.optimization.level") {
            // Convert simple level to enhanced version
            if (value == "aggressive") {
                return "aggressive_opt_level_2";
            } else if (value == "conservative") {
                return "conservative_opt_level_2";
            }
        }
        
        // Simulate thread pool optimization
        if (key == "threading.max_threads") {
            try {
                int threads = std::stoi(value);
                if (threads > 0) {
                    int optimized_threads = std::min(threads * 2, 32); // Cap at 32 threads
                    return std::to_string(optimized_threads) + " (optimized)";
                }
            } catch (...) {
                // Keep original value if parsing fails
            }
        }
        
        return value;
    }
};

class LoggingEnhancementPlugin : public IPlugin {
public:
    std::string getName() const override {
        return "LoggingEnhancementPlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string getDescription() const override {
        return "Provides advanced logging capabilities and formatting";
    }
    
    void onLoad() override {
        NLM_LOG_INFO("LoggingEnhancementPlugin loaded successfully");
    }
    
    void onUnload() override {
        NLM_LOG_INFO("LoggingEnhancementPlugin unloaded successfully");
    }
    
    bool canHandle(const std::string& key) const override {
        return key.find("logging") != std::string::npos ||
               key.find("output") != std::string::npos;
    }
    
    std::string handleConfig(const std::string& key, const std::string& value) override {
        if (key == "logging.format") {
            if (value == "json") {
                return "json_pretty_pretty"; // Double pretty for extra formatting
            } else if (value == "text") {
                return "text_enhanced_colored";
            }
        } else if (key == "logging.level") {
            // Convert DEBUG to enhanced levels
            std::string upperValue = value;
            std::transform(upperValue.begin(), upperValue.end(), upperValue.begin(), ::toupper);
            if (upperValue == "DEBUG") {
                return "DEBUG_EXTENDED";
            } else if (upperValue == "INFO") {
                return "INFO_VERBOSE";
            }
        }
        
        return value;
    }
};

class ConfigValidationPlugin : public IPlugin {
public:
    std::string getName() const override {
        return "ConfigValidationPlugin";
    }
    
    std::string getVersion() const override {
        return "1.0.0";
    }
    
    std::string getDescription() const override {
        return "Provides configuration validation and error checking";
    }
    
    void onLoad() override {
        NLM_LOG_INFO("ConfigValidationPlugin loaded successfully");
    }
    
    void onUnload() override {
        NLM_LOG_INFO("ConfigValidationPlugin unloaded successfully");
    }
    
    bool canHandle(const std::string& key) const override {
        return key.find("validation") != std::string::npos ||
               key.find("error") != std::string::npos;
    }
    
    std::string handleConfig(const std::string& key, const std::string& value) override {
        if (key == "validation.strict_mode" && value == "true") {
            // Return enhanced validation message
            return "strict=true, enhanced=true, auto_fix=true";
        } else if (key == "error_handling.level") {
            if (value == "comprehensive") {
                return "comprehensive_enhanced_monitoring";
            }
        }
        
        return value;
    }
};

// Factory function to create plugin instances
extern "C" {
    NLM_API std::shared_ptr<IPlugin> createPlugin() {
        return std::make_shared<AdvancedValidationPlugin>();
    }
    
    NLM_API std::shared_ptr<IPlugin> createPerformancePlugin() {
        return std::make_shared<PerformanceEnhancementPlugin>();
    }
    
    NLM_API std::shared_ptr<IPlugin> createLoggingPlugin() {
        return std::make_shared<LoggingEnhancementPlugin>();
    }
    
    NLM_API std::shared_ptr<IPlugin> createValidationPlugin() {
        return std::make_shared<ConfigValidationPlugin>();
    }
}

} // namespace nlm
