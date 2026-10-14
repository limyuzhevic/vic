// nlm_plugin tool implementation
// Plugin discovery, loading, and management tool for NLM

#include "tools/plugin/nlm_plugin/nlm_plugin.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "cli/ArgumentParser.hpp"
#include "plugin/PluginFactory.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <map>

namespace nlm {

NLMPluginTool::NLMPluginTool() : parser_(std::make_shared<ArgumentParser>("nlm_plugin", "Plugin discovery, loading, and management tool for NLM")) {
    setupArgumentParser();
}

bool NLMPluginTool::initialize(const std::unordered_map<std::string, std::string>& config) {
    logger_ = std::make_unique<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger_->addLogger(consoleLogger);
    Logger::setGlobal(logger_);
    
    config_ = std::make_shared<nlm::Config>();
    return true;
}

bool NLMPluginTool::execute(const std::vector<std::string>& args) {
    if (parser_.getPositionalArgs().empty()) {
        parser_.printHelp();
        return false;
    }
    
    std::string command = parser_.getPositionalArgs()[0];
    
    if (command == "discover") {
        return executeDiscoverCommand(args);
    } else if (command == "load") {
        return executeLoadCommand(args);
    } else if (command == "unload") {
        return executeUnloadCommand(args);
    } else if (command == "info") {
        return executeInfoCommand(args);
    } else {
        log("Unknown command: " + command, true);
        return false;
    }
}

bool NLMPluginTool::executeDiscoverCommand(const std::vector<std::string>& args) {
    std::string output = parser_.wasProvided("--output") ? parser_.getValue("--output") : "plugins.json";
    std::string category = parser_.wasProvided("--category") ? parser_.getValue("--category") : "all";
    bool enabled = parser_.wasProvided("--enabled") ? parser_.getValueBool("--enabled") : true;
    
    log("Discovering plugins (category: " << category << ", enabled: " << (enabled ? "yes" : "no") << ")");
    
    // Use the actual plugin factory
    PluginFactory& factory = PluginFactory::getInstance();
    std::vector<std::string> pluginNames;
    
    if (category == "all" || category == "brain") {
        std::vector<std::string> names = factory.getRegisteredPluginNames();
        pluginNames.insert(pluginNames.end(), names.begin(), names.end());
    }
    
    // Create discovery output
    std::ofstream outFile(output);
    if (!outFile.is_open()) {
        log("Error: Cannot create plugins file: " << output, true);
        return false;
    }
    
    outFile << "{\"plugins\": [";
    for (size_t i = 0; i < pluginNames.size(); ++i) {
        if (i > 0) outFile << ", ";
        PluginMetadata metadata = factory.getPluginMetadata(pluginNames[i]);
        outFile << "{\"name\": \"" << metadata.name << "\", \"version\": \"" << metadata.version 
                << "\", \"description\": \"" << metadata.description << "\", \"category\": \"" 
                << metadata.category << "\", \"enabled\": " << (metadata.enabled ? "true" : "false") 
                << "}";
    }
    outFile << "]}";
    outFile.close();
    
    log("Plugin discovery saved to: " << output);
    log("Found " << pluginNames.size() << " plugins");
    return true;
}

bool NLMPluginTool::executeLoadCommand(const std::vector<std::string>& args) {
    std::string name = parser_.wasProvided("--name") ? parser_.getValue("--name") : "";
    std::string configFile = parser_.wasProvided("--config") ? parser_.getValue("--config") : "";
    
    if (name.empty()) {
        log("Error: --name argument is required for 'load' command", true);
        return false;
    }
    
    log("Loading plugin: " << name);
    
    // Try to load the plugin (note: this is a simplified example)
    // In a real implementation, this would dynamically load shared libraries
    PluginFactory& factory = PluginFactory::getInstance();
    
    if (!factory.isPluginRegistered(name)) {
        log("Error: Plugin not found: " + name, true);
        return false;
    }
    
    // Enable the plugin
    factory.setPluginEnabled(name, true);
    
    // Create an instance if config file is provided
    if (!configFile.empty()) {
        std::ifstream configStream(configFile);
        if (!configStream.is_open()) {
            log("Error: Cannot open config file: " << configFile, true);
            return false;
        }
        
        // Parse config (simplified)
        std::unordered_map<std::string, std::string> pluginConfig;
        std::string line;
        while (std::getline(configStream, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                pluginConfig[key] = value;
            }
        }
        
        // Create and configure plugin
        auto plugin = factory.createPlugin(name, pluginConfig);
        if (plugin) {
            log("Plugin loaded and configured successfully: " << name);
        } else {
            log("Warning: Plugin created but configuration may have failed: " << name);
        }
    } else {
        log("Plugin loaded successfully: " << name);
    }
    
    return true;
}

bool NLMPluginTool::executeUnloadCommand(const std::vector<std::string>& args) {
    std::string name = parser_.wasProvided("--name") ? parser_.getValue("--name") : "";
    
    if (name.empty()) {
        log("Error: --name argument is required for 'unload' command", true);
        return false;
    }
    
    log("Unloading plugin: " << name);
    
    PluginFactory& factory = PluginFactory::getInstance();
    
    if (!factory.isPluginRegistered(name)) {
        log("Error: Plugin not found: " + name, true);
        return false;
    }
    
    // Disable the plugin
    factory.setPluginEnabled(name, false);
    
    log("Plugin unloaded successfully: " << name);
    return true;
}

bool NLMPluginTool::executeInfoCommand(const std::vector<std::string>& args) {
    std::string name = parser_.wasProvided("--name") ? parser_.getValue("--name") : "";
    std::string format = parser_.wasProvided("--format") ? parser_.getValue("--format") : "text";
    
    if (name.empty()) {
        log("Error: --name argument is required for 'info' command", true);
        return false;
    }
    
    log("Getting plugin information: " << name);
    
    PluginFactory& factory = PluginFactory::getInstance();
    
    if (!factory.isPluginRegistered(name)) {
        log("Error: Plugin not found: " + name, true);
        return false;
    }
    
    PluginMetadata metadata = factory.getPluginMetadata(name);
    
    if (format == "json") {
        std::cout << "{\"name\": \"" << metadata.name << "\", \"version\": \"" << metadata.version
                  << "\", \"description\": \"" << metadata.description << "\", \"author\": \""
                  << metadata.author << "\", \"category\": \"" << metadata.category
                  << "\", \"priority\": " << metadata.priority << ", \"enabled\": "
                  << (metadata.enabled ? "true" : "false") << ", \"dependencies\": [";
        
        for (size_t i = 0; i < metadata.dependencies.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << "\"" << metadata.dependencies[i] << "\"";
        }
        
        std::cout << "], \"config_params\": {";
        for (size_t i = 0; i < metadata.configParams.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << "\"" << metadata.configParams.begin()->first << "\": \""
                      << metadata.configParams.begin()->second << "\"";
        }
        std::cout << "}}" << std::endl;
    } else {
        std::cout << "Plugin Information:" << std::endl;
        std::cout << "  Name: " << metadata.name << std::endl;
        std::cout << "  Version: " << metadata.version << std::endl;
        std::cout << "  Description: " << metadata.description << std::endl;
        std::cout << "  Author: " << metadata.author << std::endl;
        std::cout << "  Category: " << metadata.category << std::endl;
        std::cout << "  Priority: " << metadata.priority << std::endl;
        std::cout << "  Enabled: " << (metadata.enabled ? "Yes" : "No") << std::endl;
        std::cout << "  Dependencies: ";
        for (size_t i = 0; i < metadata.dependencies.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << metadata.dependencies[i];
        }
        std::cout << std::endl;
        std::cout << "  Configuration Parameters: " << metadata.configParams.size() << std::endl;
    }
    
    return true;
}

void NLMPluginTool::setupArgumentParser() {
    parser_->addArgument("command", 0, "command", "Plugin command", "", false);
    parser_->addArgument("--name", 'n', "name", "Plugin name", "");
    parser_->addArgument("--output", 'o', "output", "Output file path", "plugins.json");
    parser_->addArgument("--category", 0, "category", "Plugin category (brain, cognition, environment, memory, plasticity, neuromodulator)", "all");
    parser_->addArgument("--enabled", 0, "enabled", "Only show enabled plugins", "true");
    parser_->addArgument("--config", 0, "config", "Configuration file path", "");
    parser_->addArgument("--format", 0, "format", "Output format (text, json)", "text");
    parser_->addArgument("--help", 0, "help", "Show help information");
    parser_->addArgument("--version", 0, "version", "Show version information");
}

std::string NLMPluginTool::getDescription() const {
    return "Plugin discovery, loading, and management tool for NLM. Manages plugin lifecycle and configuration.";
}

std::string NLMPluginTool::getVersion() const {
    return "1.0.0";
}

std::string NLMPluginTool::getUsage() const {
    return "nlm_plugin <command> [options]"; 
}

void NLMPluginTool::shutdown() {
    if (logger_) {
        Logger::setGlobal(nullptr);
        logger_.reset();
    }
}

void NLMPluginTool::log(const std::string& message, bool error) const {
    std::cout << (error ? "[ERROR] " : "[INFO] ") << message << std::endl;
}

} // namespace nlm