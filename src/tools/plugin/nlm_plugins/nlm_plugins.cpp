// nlm_plugins tool implementation
// List and manage installed plugins

#include "tools/plugin/nlm_plugins/nlm_plugins.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "cli/ArgumentParser.hpp"
#include "plugin/PluginFactory.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace nlm {

NLMPluginsTool::NLMPluginsTool() : parser_(std::make_shared<ArgumentParser>("nlm_plugins", "List and manage installed plugins")) {
    setupArgumentParser();
}

bool NLMPluginsTool::initialize(const std::unordered_map<std::string, std::string>& config) {
    logger_ = std::make_unique<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger_->addLogger(consoleLogger);
    Logger::setGlobal(logger_);
    
    config_ = std::make_shared<nlm::Config>();
    return true;
}

bool NLMPluginsTool::execute(const std::vector<std::string>& args) {
    if (parser_.getPositionalArgs().empty()) {
        parser_.printHelp();
        return false;
    }
    
    std::string command = parser_.getPositionalArgs()[0];
    
    if (command == "list") {
        return executeListCommand(args);
    } else if (command == "enable") {
        return executeEnableCommand(args);
    } else if (command == "disable") {
        return executeDisableCommand(args);
    } else {
        log("Unknown command: " + command, true);
        return false;
    }
}

bool NLMPluginsTool::executeListCommand(const std::vector<std::string>& args) {
    std::string output = parser_.wasProvided("--output") ? parser_.getValue("--output") : "plugins.txt";
    std::string category = parser_.wasProvided("--category") ? parser_.getValue("--category") : "all";
    bool enabled = parser_.wasProvided("--enabled") ? parser_.getValueBool("--enabled") : true;
    bool verbose = parser_.wasProvided("--verbose") ? parser_.getValueBool("--verbose") : false;
    
    log("Listing installed plugins (category: " << category << ", enabled: " << (enabled ? "yes" : "no") << ")");
    
    PluginFactory& factory = PluginFactory::getInstance();
    std::vector<std::string> pluginNames;
    
    if (category == "all") {
        pluginNames = factory.getRegisteredPluginNames();
    } else {
        // In a real implementation, this would filter by category
        pluginNames = factory.getRegisteredPluginNames();
    }
    
    // Filter by enabled status
    std::vector<std::string> filteredNames;
    for (const auto& name : pluginNames) {
        PluginMetadata metadata = factory.getPluginMetadata(name);
        if (metadata.enabled == enabled) {
            filteredNames.push_back(name);
        }
    }
    
    // Output results
    if (verbose) {
        std::cout << "Installed Plugins (" << filteredNames.size() << "):" << std::endl;
        std::cout << std::left << std::setw(30) << "Name" 
                  << std::setw(15) << "Version" 
                  << std::setw(20) << "Category" 
                  << std::setw(15) << "Priority" 
                  << std::setw(10) << "Enabled" << std::endl;
        std::cout << std::string(90, '=') << std::endl;
        
        for (const auto& name : filteredNames) {
            PluginMetadata metadata = factory.getPluginMetadata(name);
            std::cout << std::left << std::setw(30) << metadata.name
                      << std::setw(15) << metadata.version
                      << std::setw(20) << metadata.category
                      << std::setw(15) << metadata.priority
                      << std::setw(10) << (metadata.enabled ? "Yes" : "No") << std::endl;
        }
    } else {
        for (const auto& name : filteredNames) {
            PluginMetadata metadata = factory.getPluginMetadata(name);
            std::cout << metadata.name << " (v" << metadata.version << ") [" << metadata.category 
                      << "] " << (metadata.enabled ? "(enabled)" : "(disabled)") << std::endl;
        }
    }
    
    // Save to file if requested
    if (!output.empty() && !output.empty() && output != "-" && output != "stdout") {
        std::ofstream outFile(output);
        if (outFile.is_open()) {
            for (const auto& name : filteredNames) {
                PluginMetadata metadata = factory.getPluginMetadata(name);
                outFile << metadata.name << "\t" << metadata.version << "\t" 
                       << metadata.category << "\t" << metadata.priority << "\t"
                       << (metadata.enabled ? "enabled" : "disabled") << "\n";
            }
            outFile.close();
            log("Plugin list saved to: " << output);
        } else {
            log("Error: Cannot create output file: " << output, true);
        }
    }
    
    return true;
}

bool NLMPluginsTool::executeEnableCommand(const std::vector<std::string>& args) {
    std::string name = parser_.wasProvided("--name") ? parser_.getValue("--name") : "";
    
    if (name.empty()) {
        log("Error: --name argument is required for 'enable' command", true);
        return false;
    }
    
    log("Enabling plugin: " << name);
    
    PluginFactory& factory = PluginFactory::getInstance();
    
    if (!factory.isPluginRegistered(name)) {
        log("Error: Plugin not found: " + name, true);
        return false;
    }
    
    if (!factory.setPluginEnabled(name, true)) {
        log("Error: Failed to enable plugin: " + name, true);
        return false;
    }
    
    log("Plugin enabled successfully: " << name);
    return true;
}

bool NLMPluginsTool::executeDisableCommand(const std::vector<std::string>& args) {
    std::string name = parser_.wasProvided("--name") ? parser_.getValue("--name") : "";
    
    if (name.empty()) {
        log("Error: --name argument is required for 'disable' command", true);
        return false;
    }
    
    log("Disabling plugin: " << name);
    
    PluginFactory& factory = PluginFactory::getInstance();
    
    if (!factory.isPluginRegistered(name)) {
        log("Error: Plugin not found: " + name, true);
        return false;
    }
    
    if (!factory.setPluginEnabled(name, false)) {
        log("Error: Failed to disable plugin: " + name, true);
        return false;
    }
    
    log("Plugin disabled successfully: " << name);
    return true;
}

void NLMPluginsTool::setupArgumentParser() {
    parser_->addArgument("command", 0, "command", "Plugins command", "", false);
    parser_->addArgument("--name", 'n', "name", "Plugin name", "");
    parser_->addArgument("--output", 'o', "output", "Output file path", "plugins.txt");
    parser_->addArgument("--category", 0, "category", "Plugin category filter", "all");
    parser_->addArgument("--enabled", 0, "enabled", "Only show enabled plugins", "true");
    parser_->addArgument("--verbose", 'v', "verbose", "Enable verbose output", "false");
    parser_->addArgument("--help", 0, "help", "Show help information");
    parser_->addArgument("--version", 0, "version", "Show version information");
}

std::string NLMPluginsTool::getDescription() const {
    return "List and manage installed plugins for NLM. Provides commands to list, enable, and disable plugins.";
}

std::string NLMPluginsTool::getVersion() const {
    return "1.0.0";
}

std::string NLMPluginsTool::getUsage() const {
    return "nlm_plugins <command> [options]"; 
}

void NLMPluginsTool::shutdown() {
    if (logger_) {
        Logger::setGlobal(nullptr);
        logger_.reset();
    }
}

void NLMPluginsTool::log(const std::string& message, bool error) const {
    std::cout << (error ? "[ERROR] " : "[INFO] ") << message << std::endl;
}

} // namespace nlm