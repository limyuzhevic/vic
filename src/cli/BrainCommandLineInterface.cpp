#include "BrainCommandLineInterface.hpp"
#include <iostream>
#include <sstream>

namespace nlm {
namespace cli {

BrainCommandLineInterface::BrainCommandLineInterface() : running(true) {
    configPath = "config.json";
    outputPath = "./output";
    logLevel = "INFO";
}

BrainCommandLineInterface::~BrainCommandLineInterface() {
    Shutdown();
}

bool BrainCommandLineInterface::Initialize(int argc, char* argv[]) {
    try {
        if (!OnInitialize()) {
            return false;
        }
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--help") {
                DisplayHelp();
                return false;
            } else if (arg == "--version") {
                DisplayVersion();
                return false;
            } else if (arg == "--config" && i + 1 < argc) {
                configPath = argv[++i];
            } else if (arg == "--output" && i + 1 < argc) {
                outputPath = argv[++i];
            } else if (arg == "--log-level" && i + 1 < argc) {
                logLevel = argv[++i];
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error initializing CLI: " << e.what() << std::endl;
        return false;
    }
}

bool BrainCommandLineInterface::Run() {
    try {
        return OnRun();
    } catch (const std::exception& e) {
        std::cerr << "Error running CLI: " << e.what() << std::endl;
        return false;
    }
}

void BrainCommandLineInterface::Shutdown() {
    try {
        OnShutdown();
    } catch (const std::exception& e) {
        std::cerr << "Error shutting down CLI: " << e.what() << std::endl;
    }
}

void BrainCommandLineInterface::RegisterCommand(const std::string& name, 
                                                const std::string& description,
                                                std::function<bool(const std::vector<std::string>&)> handler) {
    commandMap[name] = handler;
}

bool BrainCommandLineInterface::ExecuteCommand(const std::string& command, const std::vector<std::string>& args) {
    auto it = commandMap.find(command);
    if (it != commandMap.end()) {
        return it->second(args);
    }
    return false;
}

void BrainCommandLineInterface::DisplayHelp() const {
    std::cout << "Brain Command Line Interface Help:" << std::endl;
    std::cout << "  --help: Display this help message" << std::endl;
    std::cout << "  --version: Display version information" << std::endl;
    std::cout << "  --config <path>: Specify configuration file path" << std::endl;
    std::cout << "  --output <path>: Specify output directory" << std::endl;
    std::cout << "  --log-level <level>: Set logging level" << std::endl;
    std::cout << std::endl;
    std::cout << "Available commands:" << std::endl;
    for (const auto& pair : commandMap) {
        std::cout << "  " << pair.first << ": " << "[Command description]" << std::endl;
    }
}

void BrainCommandLineInterface::DisplayVersion() const {
    std::cout << "Brain Command Line Interface v1.0.0" << std::endl;
}

} // namespace cli
} // namespace nlm