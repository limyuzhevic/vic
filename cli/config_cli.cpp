// NLM CLI Config - Configuration Management Commands
// Handles configuration loading, saving, and management

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {
namespace cli {

class ConfigCLI {
public:
    ConfigCLI() {
        initializeLogger();
    }
    
    int run(int argc, char** argv) {
        if (argc < 2) {
            printUsage();
            return 1;
        }
        
        std::string command = argv[1];
        
        try {
            if (command == "show") {
                return handleShowCommand(argc, argv);
            } else if (command == "set") {
                return handleSetCommand(argc, argv);
            } else if (command == "load") {
                return handleLoadCommand(argc, argv);
            } else if (command == "save") {
                return handleSaveCommand(argc, argv);
            } else if (command == "validate") {
                return handleValidateCommand(argc, argv);
            } else if (command == "export") {
                return handleExportCommand(argc, argv);
            } else if (command == "import") {
                return handleImportCommand(argc, argv);
            } else if (command == "list") {
                return handleListCommand(argc, argv);
            } else {
                std::cerr << "Unknown config command: " << command << std::endl;
                printUsage();
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }
    
private:
    void initializeLogger() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    int handleShowCommand(int argc, char** argv) {
        std::string configFile = "configs/default.cfg";
        std::string format = "summary";
        
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 7) == "--file") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else if (arg.substr(0, 8) == "--format") {
                if (arg.find('=') != std::string::npos) {
                    format = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    format = argv[++i];
                }
            }
        }
        
        auto config = std::make_shared<Config>();
        
        if (configFile.empty() || !config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        if (format == "summary") {
            NLM_LOG_INFO("Configuration Summary:");
            NLM_LOG_INFO(config->summary());
        } else if (format == "json") {
            std::cout << config->toJSON() << std::endl;
        } else if (format == "text") {
            std::vector<std::string> keys = config->getKeys();
            for (const auto& key : keys) {
                std::cout << key << " = [";
                
                auto entry = getConfigEntry(config, key);
                if (entry) {
                    std::visit([&](const auto& value) {
                        if constexpr (std::is_same_v<decltype(value), std::string>) {
                            std::cout << "\"" << value << "\"";
                        } else if constexpr (std::is_same_v<decltype(value), int> || 
                                          std::is_same_v<decltype(value), int64_t>) {
                            std::cout << value;
                        } else if constexpr (std::is_same_v<decltype(value), double>) {
                            std::cout << value;
                        } else if constexpr (std::is_same_v<decltype(value), bool>) {
                            std::cout << (value ? "true" : "false");
                        }
                    }, entry->value);
                }
                
                std::cout << "] (Source: " << static_cast<int>(entry->source) << ")" << std::endl;
            }
        }
        
        return 0;
    }
    
    int handleSetCommand(int argc, char** argv) {
        if (argc < 4) {
            std::cerr << "Error: Missing key or value for config set command" << std::endl;
            return 1;
        }
        
        std::string key = argv[2];
        std::string value = argv[3];
        
        auto config = std::make_shared<Config>();
        config->set(key, value, ConfigSource::CommandLine);
        
        NLM_LOG_INFO("Set configuration key: " + key + " = " + value);
        return 0;
    }
    
    int handleLoadCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing file path for config load command" << std::endl;
            return 1;
        }
        
        std::string configFile = argv[2];
        
        auto config = std::make_shared<Config>();
        
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Configuration loaded from: " + configFile);
            NLM_LOG_INFO("Loaded " + std::to_string(config->getKeys().size()) + " entries");
            return 0;
        } else {
            NLM_LOG_ERROR("Failed to load configuration from: " + configFile);
            return 1;
        }
    }
    
    int handleSaveCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing file path for config save command" << std::endl;
            return 1;
        }
        
        std::string configFile = argv[2];
        
        auto config = std::make_shared<Config>();
        
        if (config->saveToFile(configFile)) {
            NLM_LOG_INFO("Configuration saved to: " + configFile);
            return 0;
        } else {
            NLM_LOG_ERROR("Failed to save configuration to: " + configFile);
            return 1;
        }
    }
    
    int handleValidateCommand(int argc, char** argv) {
        auto config = std::make_shared<Config>();
        
        // Create a basic schema for validation
        std::vector<ConfigSchema> schema;
        
        // Required keys
        schema.push_back({"random_seed", true, ""});
        schema.push_back({"neuron_count", true, ""});
        schema.push_back({"region_count", true, ""});
        
        // Optional keys with defaults
        schema.push_back({"learning_rate", false, "0.01"});
        schema.push_back({"enable_stdp", false, "true"});
        schema.push_back({"simulation_timestep", false, "0.001"});
        
        if (config->validate(schema)) {
            NLM_LOG_INFO("Configuration validation passed.");
            return 0;
        } else {
            NLM_LOG_ERROR("Configuration validation failed.");
            return 1;
        }
    }
    
    int handleExportCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing format for config export command" << std::endl;
            printUsage();
            return 1;
        }
        
        std::string format = argv[2];
        std::string configFile = "configs/default.cfg";
        std::string outputFile = "";
        bool includeDefaults = false;
        
        // Parse additional arguments
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 7) == "--file") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else if (arg.substr(0, 6) == "--output") {
                if (arg.find('=') != std::string::npos) {
                    outputFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    outputFile = argv[++i];
                }
            } else if (arg == "--include-defaults") {
                includeDefaults = true;
            }
        }
        
        auto config = std::make_shared<Config>();
        
        if (configFile.empty() || !config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Get all entries, optionally filtering out defaults
        auto allEntries = config->getAllEntries();
        if (!includeDefaults) {
            allEntries.erase(
                std::remove_if(allEntries.begin(), allEntries.end(),
                    [](const ConfigEntry& entry) { return entry.source == ConfigSource::Default; }),
                allEntries.end()
            );
        }
        
        // Create a temporary config with the filtered entries
        auto filteredConfig = std::make_shared<Config>();
        for (const auto& entry : allEntries) {
            filteredConfig->set(entry.key, entry.value, entry.source);
        }
        
        if (format == "json") {
            std::string jsonOutput = filteredConfig->toJSON();
            if (!outputFile.empty()) {
                std::ofstream file(outputFile);
                if (file.is_open()) {
                    file << jsonOutput << std::endl;
                    NLM_LOG_INFO("Configuration exported to JSON file: " + outputFile);
                    file.close();
                } else {
                    NLM_LOG_ERROR("Failed to open file for writing: " + outputFile);
                    return 1;
                }
            } else {
                std::cout << jsonOutput << std::endl;
            }
        } else if (format == "yaml") {
            std::string yamlOutput = filteredConfig->toYAML();
            if (!outputFile.empty()) {
                std::ofstream file(outputFile);
                if (file.is_open()) {
                    file << yamlOutput << std::endl;
                    NLM_LOG_INFO("Configuration exported to YAML file: " + outputFile);
                    file.close();
                } else {
                    NLM_LOG_ERROR("Failed to open file for writing: " + outputFile);
                    return 1;
                }
            } else {
                std::cout << yamlOutput << std::endl;
            }
        } else if (format == "ini") {
            std::string iniOutput = filteredConfig->toINI();
            if (!outputFile.empty()) {
                std::ofstream file(outputFile);
                if (file.is_open()) {
                    file << iniOutput << std::endl;
                    NLM_LOG_INFO("Configuration exported to INI file: " + outputFile);
                    file.close();
                } else {
                    NLM_LOG_ERROR("Failed to open file for writing: " + outputFile);
                    return 1;
                }
            } else {
                std::cout << iniOutput << std::endl;
            }
        } else if (format == "env") {
            std::vector<std::string> keys = filteredConfig->getKeys();
            for (const auto& key : keys) {
                std::cout << key << "=";
                
                auto entry = getConfigEntry(filteredConfig, key);
                if (entry) {
                    std::cout << filteredConfig->configValueToString(entry->value);
                }
                
                std::cout << std::endl;
            }
        } else {
            std::cerr << "Unknown export format: " << format << std::endl;
            printUsage();
            return 1;
        }
        
        return 0;
    }
    
    int handleCompareCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing file path for config compare command" << std::endl;
            printUsage();
            return 1;
        }
        
        std::string configFile1 = argv[2];
        std::string configFile2 = "configs/default.cfg";
        
        // Parse additional arguments
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 6) == "--file2") {
                if (arg.find('=') != std::string::npos) {
                    configFile2 = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile2 = argv[++i];
                }
            }
        }
        
        auto config1 = std::make_shared<Config>();
        auto config2 = std::make_shared<Config>();
        
        if (configFile1.empty() || !config1->loadFromFile(configFile1)) {
            NLM_LOG_INFO("Using default configuration for first file.");
        }
        
        if (configFile2.empty() || !config2->loadFromFile(configFile2)) {
            NLM_LOG_INFO("Using default configuration for second file.");
        }
        
        std::string diff = config1->getDiff(*config2);
        std::cout << diff << std::endl;
        
        NLM_LOG_INFO("Configuration comparison completed.");
        return 0;
    }
    
    int handleDiffCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing file paths for config diff command" << std::endl;
            printUsage();
            return 1;
        }
        
        std::string configFile1 = argv[2];
        std::string configFile2 = "configs/default.cfg";
        
        // Parse additional arguments
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 6) == "--file2") {
                if (arg.find('=') != std::string::npos) {
                    configFile2 = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile2 = argv[++i];
                }
            }
        }
        
        auto config1 = std::make_shared<Config>();
        auto config2 = std::make_shared<Config>();
        
        if (configFile1.empty() || !config1->loadFromFile(configFile1)) {
            NLM_LOG_INFO("Using default configuration for first file.");
        }
        
        if (configFile2.empty() || !config2->loadFromFile(configFile2)) {
            NLM_LOG_INFO("Using default configuration for second file.");
        }
        
        std::string diff = config1->getDiff(*config2);
        
        // Write diff to a temporary file for better formatting
        std::string diffFile = "config_diff.txt";
        std::ofstream diffOut(diffFile);
        if (diffOut.is_open()) {
            diffOut << diff;
            diffOut.close();
            NLM_LOG_INFO("Configuration diff written to: " + diffFile);
        }
        
        return 0;
    }
    
    int handleTemplateCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing template type for config template command" << std::endl;
            printUsage();
            return 1;
        }
        
        std::string templateType = argv[2];
        
        auto config = std::make_shared<Config>();
        
        if (templateType == "standard") {
            NLM_LOG_INFO("Creating standard configuration template...");
            config = Config::createStandardTemplate();
        } else if (templateType == "research") {
            NLM_LOG_INFO("Creating research configuration template...");
            config = Config::createResearchTemplate();
        } else if (templateType == "performance") {
            NLM_LOG_INFO("Creating performance configuration template...");
            config = Config::createPerformanceTemplate();
        } else {
            std::cerr << "Unknown template type: " << templateType << std::endl;
            printUsage();
            return 1;
        }
        
        // Save to file
        std::string outputFile = templateType + "_template.cfg";
        if (config->saveToFile(outputFile)) {
            NLM_LOG_INFO("Template saved to: " + outputFile);
            return 0;
        } else {
            NLM_LOG_ERROR("Failed to save template to: " + outputFile);
            return 1;
        }
    }
    
    ConfigEntry* getConfigEntry(std::shared_ptr<Config> config, const std::string& key) {
        auto entries = config->getAllEntries();
        for (auto& entry : entries) {
            if (entry.key == key) {
                return &entry;
            }
        }
        return nullptr;
    }
    
    void printUsage() {
        std::cout << "Usage: nlm-config <command> [options]" << std::endl;
        std::cout << "\nCommands:" << std::endl;
        std::cout << "  show          Show configuration" << std::endl;
        std::cout << "  set <key> <value>  Set configuration value" << std::endl;
        std::cout << "  load <file>   Load configuration from file" << std::endl;
        std::cout << "  save <file>   Save configuration to file" << std::endl;
        std::cout << "  validate      Validate configuration" << std::endl;
        std::cout << "  export <format>  Export configuration to file" << std::endl;
        std::cout << "  import <format>  Import configuration from stdin" << std::endl;
        std::cout << "  list          List configuration keys and sources" << std::endl;
    }
};

int main(int argc, char** argv) {
    ConfigCLI cli;
    return cli.run(argc, argv);
}

} // namespace cli
} // namespace nlm
