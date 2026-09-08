// Command line argument parsing and command execution
class CommandLineParser {
private:
    int argc_;
    char** argv_;
    
public:
    CommandLineParser(int argc, char** argv) : argc_(argc), argv_(argv) {}
    
    std::string parseCommand() {
        if (argc_ < 2) {
            return "help"; // Default to help if no command provided
        }
        
        std::string arg(argv_[1]);
        
        // Handle help, version, and config options
        if (arg == "--help" || arg == "-h") {
            return "help";
        } else if (arg == "--version" || arg == "-v") {
            return "version";
        } else if (arg == "--config" || arg == "-c") {
            if (argc_ >= 3) {
                return "config";
            }
        }
        
        return arg;
    }
    
    std::string parseConfigFile() {
        std::string configFile = "configs/default.cfg";
        for (int i = 1; i < argc_; ++i) {
            std::string arg(argv_[i]);
            if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc_) {
                    configFile = argv_[++i];
                }
            }
        }
        return configFile;
    }
    
    bool hasArgument(const std::string& arg) {
        for (int i = 1; i < argc_; ++i) {
            if (argv_[i] == arg) {
                return true;
            }
        }
        return false;
    }
    
    std::string getArgument(const std::string& arg) {
        for (int i = 1; i < argc_ - 1; ++i) {
            if (argv_[i] == arg) {
                return argv_[i + 1];
            }
        }
        return "";
    }
};

// Command executor
class CommandExecutor {
private:
    CommandRegistry& registry_;
    
public:
    CommandExecutor(CommandRegistry& registry) : registry_(registry) {}
    
    int execute(const std::string& commandName) {
        if (!registry_.hasCommand(commandName)) {
            std::cerr << "Error: Unknown command: " << commandName << std::endl;
            std::cerr << "Use '--help' or '-h' for available commands" << std::endl;
            return 1;
        }
        
        auto command = registry_.createCommand(commandName);
        if (!command) {
            std::cerr << "Error: Failed to create command: " << commandName << std::endl;
            return 1;
        }
        
        try {
            command->execute();
        } catch (const std::exception& e) {
            std::cerr << "Error executing command " << commandName << ": " << e.what() << std::endl;
            return 1;
        }
        
        return 0;
    }
};