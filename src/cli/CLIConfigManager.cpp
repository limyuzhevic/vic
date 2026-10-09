#include "cli/CLIConfigManager.hpp"

CLIConfigManager::CLIConfigManager() {
    config_ = std::make_shared<Config>();
}

bool CLIConfigManager::parseCommandLine(int argc, char** argv) {
    if (argc < 3) {
        printHelp();
        return true;
    }

    std::string command = argv[2];

    if (command == "list") {
        return handleListCommand(argc - 2, &argv[2]);
    } else if (command == "set") {
        return handleSetCommand(argc - 2, &argv[2]);
    } else if (command == "get") {
        return handleGetCommand(argc - 2, &argv[2]);
    } else if (command == "save") {
        return handleSaveCommand(argc - 2, &argv[2]);
    } else if (command == "load") {
        return handleLoadCommand(argc - 2, &argv[2]);
    } else if (command == "help") {
        printHelp();
        return true;
    } else {
        std::cerr << "Unknown config command: " << command << std::endl;
        return false;
    }
}

bool CLIConfigManager::handleListCommand(int argc, char** argv) {
    config_->printConfig();
    return true;
}

bool CLIConfigManager::handleSetCommand(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Error: Missing arguments for set command." << std::endl;
        printSetHelp();
        return false;
    }

    std::string key = argv[3];
    std::string value = argv[4];

    // Try to parse as different types
    bool success = false;
    if (value.find('.') != std::string::npos) {
        // Try float
        try {
            float floatValue = std::stof(value);
            config_->set(key, floatValue);
            success = true;
        } catch (...) {
            success = false;
        }
    }

    if (!success && (value.find('-') == 0 || std::all_of(value.begin(), value.end(), ::isdigit))) {
        // Try int
        try {
            int intValue = std::stoi(value);
            config_->set(key, intValue);
            success = true;
        } catch (...) {
            success = false;
        }
    }

    if (!success) {
        // Treat as string
        config_->set(key, value);
        success = true;
    }

    if (success) {
        std::cout << "Set config " << key << " = " << value << std::endl;
    } else {
        std::cerr << "Error: Could not parse value: " << value << std::endl;
        return false;
    }

    return true;
}

bool CLIConfigManager::handleGetCommand(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Error: Missing argument for get command." << std::endl;
        return false;
    }

    std::string key = argv[3];

    if (!config_->has(key)) {
        std::cerr << "Error: Config key not found: " << key << std::endl;
        return false;
    }

    std::string value;
    if (config_->has(key, value)) {
        std::cout << key << " = " << value << std::endl;
    }

    return true;
}

bool CLIConfigManager::handleSaveCommand(int argc, char** argv) {
    std::string filepath = "configs/default.cfg";

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) {
            filepath = argv[++i];
        }
    }

    if (config_->saveToFile(filepath)) {
        std::cout << "Configuration saved to: " << filepath << std::endl;
        return true;
    } else {
        std::cerr << "Error: Failed to save configuration to: " << filepath << std::endl;
        return false;
    }
}

bool CLIConfigManager::handleLoadCommand(int argc, char** argv) {
    std::string filepath = "configs/default.cfg";

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) {
            filepath = argv[++i];
        }
    }

    if (config_->loadFromFile(filepath)) {
        std::cout << "Configuration loaded from: " << filepath << std::endl;
        return true;
    } else {
        std::cerr << "Error: Failed to load configuration from: " << filepath << std::endl;
        return false;
    }
}

void CLIConfigManager::printHelp() {
    std::cout << "NLM Config Command" << std::endl;
    std::cout << "==================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Manage NLM configuration files." << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm config <command> [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  list              List all configuration keys and values" << std::endl;
    std::cout << "  set KEY VALUE     Set a configuration key to a value" << std::endl;
    std::cout << "  get KEY           Get the value of a configuration key" << std::endl;
    std::cout << "  save              Save current configuration to file" << std::endl;
    std::cout << "  load              Load configuration from file" << std::endl;
    std::cout << "  help              Show this help message" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm config list" << std::endl;
    std::cout << "  nlm config set random_seed 12345" << std::endl;
    std::cout << "  nlm config get neuron_count" << std::endl;
    std::cout << "  nlm config save --file configs/experiment.cfg" << std::endl;
    std::cout << "  nlm config load --file configs/experiment.cfg" << std::endl;
}

void CLIConfigManager::printSetHelp() {
    std::cout << "NLM Config Set Command" << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Set a configuration key to a value." << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm config set KEY VALUE" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Key types supported:" << std::endl;
    std::cout << "  - String keys (e.g., 'random_seed', 'neuron_count')" << std::endl;
    std::cout << "  - Values automatically detected as int, float, or string" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm config set random_seed 12345" << std::endl;
    std::cout << "  nlm config set neuron_count 2000" << std::endl;
    std::cout << "  nlm config set connection_probability 0.15" << std::endl;
}

void CLIConfigManager::printGetHelp() {
    std::cout << "NLM Config Get Command" << std::endl;
    std::cout << "======================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Get the value of a configuration key." << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm config get KEY" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm config get random_seed" << std::endl;
    std::cout << "  nlm config get neuron_count" << std::endl;
}