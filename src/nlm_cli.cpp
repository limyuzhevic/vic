namespace nlm { namespace cli {

// Command line argument parser
class CLIParser {
public:
    struct Command {
        std::string name;
        std::string description;
        std::vector<std::string> args;
        std::map<std::string, std::string> options;
        std::string help;
        std::function<bool(const std::vector<std::string>&, const std::map<std::string, std::string>&)> handler;
    };

    CLIParser() {
        setupCommands();
    }

    bool parse(int argc, char** argv) {
        if (argc < 2) {
            printHelp();
            return false;
        }

        std::string command = argv[1];
        std::vector<std::string> args;
        std::map<std::string, std::string> options;

        // Parse remaining arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.find("--") == 0) {
                // Option
                size_t pos = arg.find("=", 2);
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    options[key] = value;
                } else {
                    options[arg] = "true";
                }
            } else {
                args.push_back(arg);
            }
        }

        // Find and execute command
        auto it = commands.find(command);
        if (it != commands.end()) {
            Command& cmd = it->second;
            return cmd.handler(args, options);
        } else {
            std::cerr << "Error: Unknown command: " << command << std::endl;
            printHelp();
            return false;
        }
    }

    void printHelp() {
        std::cout << "NLM - Neural Learning Machine Command Line Interface" << std::endl;
        std::cout << "Version: 2.0.0-enhanced" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Available commands:" << std::endl;
        std::cout << "  help                    Show this help message" << std::endl;
        std::cout << "  version                 Show version information" << std::endl;
        std::cout << "  config                  Configuration operations" << std::endl;
        std::cout << "  brain                   Brain operations" << std::endl;
        std::cout << "  simulate                Run simulation" << std::endl;
        std::cout << "  benchmark               Performance benchmark" << std::endl;
        std::cout << "  experiment              Run experiments" << std::endl;
        std::cout << "  checkpoint              Checkpoint operations" << std::endl;
        std::cout << "  world                   World operations" << std::endl;
        std::cout << "  agent                   Agent operations" << std::endl;
        std::cout << "  stats                   Get system statistics" << std::endl;
        std::cout << "  validate                Validate configuration" << std::endl;
        std::cout << std::endl;
    }

    void setupCommands() {
        // Register commands
        registerCommand("help", "Show help message", {}, "", 
                       [](const std::vector<std::string>& args, const std::map<std::string, std::string>& options) {
                           printHelp();
                           return true;
                       });

        registerCommand("version", "Show version information", {}, 
                       "Show NLM version and build information", 
                       [](const std::vector<std::string>& args, const std::map<std::string, std::string>& options) {
                           printVersion();
                           return true;
                       });

        registerCommand("config", "Configuration operations", {"action", "key", "value"}, 
                       "Manage NLM configuration", 
                       [](const std::vector<std::string>& args, const std::map<std::string, std::string>& options) {
                           return handleConfigCommand(args, options);
                       });

        registerCommand("brain", "Brain operations", {"action", "file", "config"}, 
                       "Manage brain operations", 
                       [](const std::vector<std::string>& args, const std::map<std::string, std::string>& options) {
                           return handleBrainCommand(args, options);
                       });

        registerCommand("simulate", "Run simulation", {"steps", "config", "world"}, 
                       "Run NLM simulation with optional brain and world", 
                       [](const std::vector<std::string>& args, const std::map<std::string, std::string>& options) {
                           return handleSimulateCommand(args, options);
                       });

        registerCommand("benchmark", "Performance benchmark", {"neurons", "steps"}, 
                       "Run performance benchmark", 
                       [](const std::vector<std::string>& args, const std::map<std::string, std::string>& options) {
                           return handleBenchmarkCommand(args, options);
                       });

        registerCommand("experiment", "Run experiments", {"type", "config"}, 
                       "Run predefined experiments", 
                       [](const std::vector<std::string>& args, const std::map<std::string, std::string>& options) {
                           return handleExperimentCommand(args, options);
                       });

        registerCommand("checkpoint", "Checkpoint operations", {"action", "file"}, 
                       "Manage brain checkpoints", 
                       [](const std::vector<std::string>& args, const std::map<std::string, std::string>& options) {
                           return handleCheckpointCommand(args, options);
                       });

        registerCommand("world", "World operations", {"action", "config"}, 
                       "Manage world simulation", 
                       [](const std::vector<std::string>& args, const std::map<std::string, std::string>& options) {
                           return handleWorldCommand(args, options);
                       });

        registerCommand("agent", "Agent operations", {"action", "brain", "world"}, 
                       "Manage agent operations", 
                       [](const std::vector<std::string>& args, const std::map<std::string, std::string>& options) {
                           return handleAgentCommand(args, options);
                       });

        registerCommand("stats", "Get system statistics", {}, 
                       "Get system and brain statistics", 
                       [](const std::vector<std::string>& args, const std::map<std::string, std::string>& options) {
                           return handleStatsCommand(args, options);
                       });

        registerCommand("validate", "Validate configuration", {"file", "schema"}, 
                       "Validate configuration file or schema", 
                       [](const std::vector<std::string>& args, const std::map<std::string, std::string>& options) {
                           return handleValidateCommand(args, options);
                       });
    }

    void registerCommand(const std::string& name, const std::string& description, 
                        const std::vector<std::string>& args, const std::string& help,
                        std::function<bool(const std::vector<std::string>&, const std::map<std::string, std::string>&)> handler) {
        Command cmd;
        cmd.name = name;
        cmd.description = description;
        cmd.args = args;
        cmd.help = help;
        cmd.handler = handler;
        commands[name] = cmd;
    }

    void printVersion() {
        std::cout << "NLM - Neural Learning Machine" << std::endl;
        std::cout << "Version: 2.0.0-enhanced" << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "C++ Standard: C++20" << std::endl;
        std::cout << "Python Bindings: Enhanced edition with comprehensive error handling" << std::endl;
        std::cout << std::endl;
    }

    // Command handlers implementation
    bool handleConfigCommand(const std::vector<std::string>& args, const std::map<std::string, std::string>& options) {
        std::string action = options.count("action") ? options.at("action") : 
                           (args.size() > 0 ? args[0] : "");

        if (action == "create" || action.empty()) {
            return createConfig(options);
        } else if (action == "list") {
            return listConfig(options);
        } else if (action == "get") {
            std::string key = options.count("key") ? options.at("key") : 
                            (args.size() > 0 ? args[0] : "");
            return getConfigValue(key, options);
        } else if (action == "set") {
            std::string key = options.count("key") ? options.at("key") : 
                            (args.size() > 0 ? args[0] : "");
            std::string value = options.count("value") ? options.at("value") : 
                              (args.size() > 1 ? args[1] : "");
            return setConfigValue(key, value, options);
        } else if (action == "save") {
            std::string file = options.count("file") ? options.at("file") : 
                             (args.size() > 0 ? args[0] : "");
            return saveConfig(file, options);
        } else if (action == "load") {
            std::string file = options.count("file") ? options.at("file") : 
                             (args.size() > 0 ? args[0] : "");
            return loadConfig(file, options);
        }

        std::cerr << "Error: Unknown config action: " << action << std::endl;
        return false;
    }

    bool createConfig(const std::map<std::string, std::string>& options) {
        auto config = std::make_shared<Config>();
        std::string file = options.count("file") ? options.at("file") : "default.cfg";

        // Apply default configuration
        config->set("neuron_count", 500);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);

        if (saveConfigToFile(config, file)) {
            std::cout << "Created default configuration in: " << file << std::endl;
            return true;
        }
        return false;
    }

    bool listConfig(const std::map<std::string, std::string>& options) {
        auto config = std::make_shared<Config>();
        auto keys = config->getKeys();
        
        std::cout << "Configuration keys:" << std::endl;
        for (const auto& key : keys) {
            std::cout << "  " << key << std::endl;
        }
        return true;
    }

    bool getConfigValue(const std::string& key, const std::map<std::string, std::string>& options) {
        auto config = std::make_shared<Config>();
        if (config->has(key)) {
            std::cout << key << " = " << config->getOr<std::string>(key, "") << std::endl;
            return true;
        } else {
            std::cerr << "Key not found: " << key << std::endl;
            return false;
        }
    }

    bool setConfigValue(const std::string& key, const std::string& value, const std::map<std::string, std::string>& options) {
        auto config = std::make_shared<Config>();
        
        // Try to determine type
        bool is_int = true;
        for (char c : value) {
            if (!std::isdigit(c)) {
                is_int = false;
                break;
            }
        }

        if (is_int) {
            config->set(key, std::stoi(value));
        } else {
            bool is_bool = (value == "true" || value == "false");
            if (is_bool) {
                config->set(key, value == "true");
            } else {
                try {
                    double double_val = std::stod(value);
                    config->set(key, double_val);
                } catch (...) {
                    config->set(key, value);
                }
            }
        }

        std::cout << "Set " << key << " = " << value << std::endl;
        return true;
    }

    bool saveConfig(const std::string& file, const std::map<std::string, std::string>& options) {
        auto config = std::make_shared<Config>();
        return saveConfigToFile(config, file);
    }

    bool loadConfig(const std::string& file, const std::map<std::string, std::string>& options) {
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(file)) {
            std::cout << "Loaded configuration from: " << file << std::endl;
            return true;
        } else {
            std::cerr << "Failed to load configuration from: " << file << std::endl;
            return false;
        }
    }

    bool saveConfigToFile(std::shared_ptr<Config> config, const std::string& file) {
        try {
            if (config->saveToFile(file)) {
                std::cout << "Saved configuration to: " << file << std::endl;
                return true;
            } else {
                std::cerr << "Failed to save configuration to: " << file << std::endl;
                return false;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error saving configuration: " << e.what() << std::endl;
            return false;
        }
    }

    // Other command handlers would be implemented similarly...

    std::map<std::string, Command> commands;
};

// Main CLI function
int main(int argc, char** argv) {
    CLIParser parser;
    return parser.parse(argc, argv) ? 0 : 1;
}

} // namespace cli

// Entry point
int main(int argc, char** argv) {
    return cli::main(argc, argv);
}