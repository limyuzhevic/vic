#pragma once
#include "Brain.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <filesystem>
#include <algorithm>

namespace nlm {

/**
 * Command-line interface to create, load, and save brain configurations
 */

class BrainConfigTool {
public:
    BrainConfigTool() {
        initializeTool(toolConfig_);
    }
    
    ~BrainConfigTool() {
        cleanupTool();
    }
    
    int run(int argc, char** argv) {
        if (argc < 2) {
            printHelp();
            return static_cast<int>(ToolExitCode::UsageError);
        }
        
        std::string command = argv[1];
        
        if (command == "create") {
            return createCommand(argc, argv) ? 0 : static_cast<int>(ToolExitCode::ConfigError);
        } else if (command == "load") {
            return loadCommand(argc, argv) ? 0 : static_cast<int>(ToolExitCode::FileError);
        } else if (command == "save") {
            return saveCommand(argc, argv) ? 0 : static_cast<int>(ToolExitCode::FileError);
        } else if (command == "list") {
            return listCommand(argc, argv) ? 0 : static_cast<int>(ToolExitCode::FileError);
        } else if (command == "set") {
            return setCommand(argc, argv) ? 0 : static_cast<int>(ToolExitCode::ConfigError);
        } else if (command == "get") {
            return getCommand(argc, argv) ? 0 : static_cast<int>(ToolExitCode::ConfigError);
        } else if (command == "delete") {
            return deleteCommand(argc, argv) ? 0 : static_cast<int>(ToolExitCode::ConfigError);
        } else if (command == "validate") {
            return validateCommand(argc, argv) ? 0 : static_cast<int>(ToolExitCode::ConfigError);
        } else if (command == "help") {
            printHelp();
            return 0;
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            printHelp();
            return static_cast<int>(ToolExitCode::UsageError);
        }
    }
    
private:
    ToolConfig toolConfig_;
    BrainConfigToolImpl toolImpl_;
    
    int createCommand(int argc, char** argv) {
        std::vector<std::pair<std::string, std::string>> args;
        
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    args.emplace_back(key, value);
                }
            }
        }
        
        if (toolImpl_.create(args)) {
            std::cout << "Brain configuration created successfully" << std::endl;
            return 0;
        }
        return 1;
    }
    
    int loadCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing config file path" << std::endl;
            return 1;
        }
        
        std::string filepath = argv[2];
        
        if (toolImpl_.load(filepath)) {
            std::cout << "Brain configuration loaded from: " << filepath << std::endl;
            return 0;
        }
        return 1;
    }
    
    int saveCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing config file path" << std::endl;
            return 1;
        }
        
        std::string filepath = argv[2];
        bool pretty = false;
        
        for (int i = 3; i < argc; ++i) {
            if (argv[i] == "--pretty") {
                pretty = true;
            }
        }
        
        if (toolImpl_.save(filepath, pretty)) {
            std::cout << "Brain configuration saved to: " << filepath << std::endl;
            return 0;
        }
        return 1;
    }
    
    int listCommand(int argc, char** argv) {
        std::string pattern = "";
        
        for (int i = 2; i < argc; ++i) {
            if (argv[i] == "--pattern" && i + 1 < argc) {
                pattern = argv[++i];
            }
        }
        
        if (toolImpl_.list(pattern)) {
            return 0;
        }
        return 1;
    }
    
    int setCommand(int argc, char** argv) {
        if (argc < 4) {
            std::cerr << "Error: Missing key or value" << std::endl;
            return 1;
        }
        
        std::string key = argv[2];
        std::string value = argv[3];
        
        if (toolImpl_.set(key, value)) {
            std::cout << "Config key set: " << key << " = " << value << std::endl;
            return 0;
        }
        return 1;
    }
    
    int getCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing key" << std::endl;
            return 1;
        }
        
        std::string key = argv[2];
        
        if (toolImpl_.get(key)) {
            return 0;
        }
        return 1;
    }
    
    int deleteCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing key" << std::endl;
            return 1;
        }
        
        std::string key = argv[2];
        
        if (toolImpl_.delete(key)) {
            std::cout << "Config key deleted: " << key << std::endl;
            return 0;
        }
        return 1;
    }
    
    int validateCommand(int argc, char** argv) {
        if (argc < 3) {
            std::cerr << "Error: Missing config file path" << std::endl;
            return 1;
        }
        
        std::string filepath = argv[2];
        
        if (toolImpl_.validate(filepath)) {
            std::cout << "Config validation passed: " << filepath << std::endl;
            return 0;
        }
        return 1;
    }
    
    void printHelp() {
        std::cout << "Brain Configuration Tool" << std::endl;
        std::cout << "=========================" << std::endl;
        std::cout << "Usage: nlm-config <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  create [--key=value ...]    Create a new brain configuration" << std::endl;
        std::cout << "  load <file>                 Load brain configuration from file" << std::endl;
        std::cout << "  save <file> [--pretty]      Save brain configuration to file" << std::endl;
        std::cout << "  list [--pattern <pattern>]   List all configuration keys" << std::endl;
        std::cout << "  set <key> <value>           Set configuration key" << std::endl;
        std::cout << "  get <key>                   Get configuration value" << std::endl;
        std::cout << "  delete <key>                Delete configuration key" << std::endl;
        std::cout << "  validate <file>             Validate configuration file" << std::endl;
        std::cout << "  help                        Show this help message" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm-config create --brain.name=MyBrain --brain.region_count=2" << std::endl;
        std::cout << "  nlm-config load config.txt" << std::endl;
        std::cout << "  nlm-config save new_config.txt --pretty" << std::endl;
        std::cout << "  nlm-config set simulation.timestep 0.01" << std::endl;
        std::cout << "  nlm-config get random.seed" << std::endl;
    }
};

int main(int argc, char** argv) {
    BrainConfigTool tool;
    return tool.run(argc, argv);
}

} // namespace nlm
