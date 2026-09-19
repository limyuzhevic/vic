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
            return 1;
        }
        
        std::string format = argv[2];
        std::string configFile = "configs/default.cfg";
        
        // Parse additional arguments
        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 7) == "--file") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            }
        }
        
        auto config = std::make_shared<Config>();
        
        if (configFile.empty() || !config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        if (format == "json") {
            std::cout << config->toJSON() << std::endl;
        } else if (format == "yaml") {
            NLM_LOG_WARNING("YAML export not yet implemented.");
        } else if (format == "ini") {
            NLM_LOG_WARNING("INI export not yet implemented.");
        } else if (format == "env") {
            std::vector<std::string> keys = config->getKeys();
            for (const auto& key : keys) {
                std::cout << key << "=";
                
                auto entry = getConfigEntry(config, key);
                if (entry) {
                    std::cout << config->configValueToString(entry->value);
                }
                
                std::cout << std::endl;
            }
        } else {
            std::cerr << "Unknown export format: " << format << std::endl;
            return 1;
        }
        
        return 0;
    }
    
    int handleImportCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing format for config import command" << std::endl;
            return 1;
        }
        
        std::string format = argv[2];
        
        auto config = std::make_shared<Config>();
        
        if (format == "json") {
            std::string jsonStr;
            std::string line;
            while (std::getline(std::cin, line)) {
                jsonStr += line + "\n";
            }
            config->fromJSON(jsonStr);
        } else if (format == "yaml") {
            NLM_LOG_WARNING("YAML import not yet implemented.");
            return 1;
        } else if (format == "ini") {
            NLM_LOG_WARNING("INI import not yet implemented.");
            return 1;
        } else if (format == "env") {
            std::string line;
            while (std::getline(std::cin, line)) {
                if (line.empty() || line[0] == '#') continue;
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    config->set(key, value, ConfigSource::File);
                }
            }
        } else {
            std::cerr << "Unknown import format: " << format << std::endl;
            return 1;
        }
        
        NLM_LOG_INFO("Configuration imported successfully.");
        return 0;
    }
    
    int handleListCommand(int argc, char** argv) {
        auto config = std::make_shared<Config>();
        
        NLM_LOG_INFO("Configuration keys:");
        std::vector<std::string> keys = config->getKeys();
        for (const auto& key : keys) {
            NLM_LOG_INFO("  " + key);
        }
        
        NLM_LOG_INFO("\nConfiguration sources:");
        NLM_LOG_INFO(config->summary());
        
        return 0;
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
