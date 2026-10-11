#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <sstream>

using namespace nlm;

// Command system for modular CLI
class Command {
public:
    using CommandHandler = std::function<bool(const std::vector<std::string>& args, std::ostream& output)>;
    
    Command(const std::string& name, const std::string& description, 
            const CommandHandler& handler, bool requiresBrain = false,
            const std::string& category = "General")
        : name(name), description(description), handler(handler), 
          requiresBrain(requiresBrain), category(category) {}
    
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    std::string getCategory() const { return category; }
    bool requiresBrain() const { return requiresBrain; }
    
    bool execute(const std::vector<std::string>& args, std::ostream& output) {
        return handler(args, output);
    }
    
private:
    std::string name;
    std::string description;
    CommandHandler handler;
    bool requiresBrain;
    std::string category;
};

class CommandRegistry {
public:
    static CommandRegistry& getInstance() {
        static CommandRegistry instance;
        return instance;
    }
    
    void registerCommand(const Command& command) {
        commands[command.getName()] = command;
    }
    
    Command* getCommand(const std::string& name) {
        auto it = commands.find(name);
        if (it != commands.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    std::vector<Command*> getCommandsInCategory(const std::string& category) {
        std::vector<Command*> result;
        for (auto& pair : commands) {
            if (pair.second.getCategory() == category) {
                result.push_back(&pair.second);
            }
        }
        return result;
    }
    
    std::vector<std::string> getAllCommandNames() {
        std::vector<std::string> names;
        for (const auto& pair : commands) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    void listCommands(std::ostream& output, const std::string& category = "") {
        output << "Available commands:" << std::endl;
        
        if (category.empty()) {
            // List all commands grouped by category
            std::vector<std::string> categories;
            for (const auto& pair : commands) {
                if (std::find(categories.begin(), categories.end(), pair.second.getCategory()) == categories.end()) {
                    categories.push_back(pair.second.getCategory());
                }
            }
            
            for (const auto& categoryName : categories) {
                output << "  " << categoryName << ":" << std::endl;
                for (const auto& pair : commands) {
                    if (pair.second.getCategory() == categoryName) {
                        output << "    " << pair.first << " - " << pair.second.getDescription() << std::endl;
                    }
                }
            }
        } else {
            // List commands in specific category
            for (const auto& pair : commands) {
                if (pair.second.getCategory() == category) {
                    output << "  " << pair.first << " - " << pair.second.getDescription() << std::endl;
                }
            }
        }
    }
    
private:
    std::unordered_map<std::string, Command> commands;
    CommandRegistry() {}
    CommandRegistry(const CommandRegistry&) = delete;
    CommandRegistry& operator=(const CommandRegistry&) = delete;
};

// Context for passing shared state between modules
class ApplicationContext {
public:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    std::shared_ptr<SimulationClock> clock;
    bool brainInitialized = false;
    
    ApplicationContext() {}
    
    void initialize(std::shared_ptr<Config> cfg) {
        config = cfg;
        // Brain will be initialized on demand
    }
    
    bool ensureBrain() {
        if (!brainInitialized && config) {
            brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                std::cerr << "Failed to initialize brain!" << std::endl;
                return false;
            }
            brainInitialized = true;
        }
        return brainInitialized;
    }
    
    void resetBrain() {
        if (brain) {
            brain->reset();
            brain->initialize();
        }
    }
};

// Help command implementation
bool helpCommand(const std::vector<std::string>& args, std::ostream& output) {
    if (args.empty()) {
        output << "NLM (Neural Learning Machine) - Modular Command Line Interface" << std::endl;
        output << "=====================================================" << std::endl;
        output << std::endl;
        output << "Usage: nlm <command> [arguments]" << std::endl;
        output << std::endl;
        output << "Commands can be listed by category:" << std::endl;
        output << "  nlm help                    Show general help" << std::endl;
        output << "  nlm help demo              Show demo commands" << std::endl;
        output << "  nlm help test               Show test commands" << std::endl;
        output << "  nlm help config             Show configuration commands" << std::endl;
        output << "  nlm help info               Show information commands" << std::endl;
        output << "  nlm help advanced           Show advanced commands" << std::endl;
        output << std::endl;
        output << "Example usage:" << std::endl;
        output << "  nlm run                    Run Phase 2 demonstration" << std::endl;
        output << "  nlm test --all             Run all tests" << std::endl;
        output << "  nlm config list            List configuration values" << std::endl;
        output << "  nlm info version           Show version information" << std::endl;
    } else {
        std::string category = args[0];
        CommandRegistry::getInstance().listCommands(output, category);
    }
    return true;
}

// Version command implementation
bool versionCommand(const std::vector<std::string>& args, std::ostream& output) {
    output << "NLM (Neural Learning Machine)" << std::endl;
    output << "Version: 0.1.0" << std::endl;
    output << "Build: Modular C++20 Edition" << std::endl;
    output << "Phase: 2 - Real Neural Computation" << std::endl;
    output << std::endl;
    output << "Features:" << std::endl;
    output << "  - Modular command-line interface" << std::endl;
    output << "  - Pluggable command system" << std::endl;
    output << "  - Advanced configuration management" << std::endl;
    output << "  - Comprehensive testing framework" << std::endl;
    output << "  - Enhanced Python bindings" << std::endl;
    output << std::endl;
    output << "For more information, use 'nlm help <category>'" << std::endl;
    return true;
}

// Run demonstration command
bool runDemoCommand(const std::vector<std::string>& args, std::ostream& output, ApplicationContext& context) {
    output << "============================================" << std::endl;
    output << "NLM Phase 2: Real Neural Computation Demo" << std::endl;
    output << "============================================" << std::endl;
    output << std::endl;
    
    // Parse optional arguments
    int numEpisodes = 5;
    int stepsPerEpisode = 1000;
    
    for (size_t i = 0; i < args.size(); i += 2) {
        if (args[i] == "--episodes" && i + 1 < args.size()) {
            numEpisodes = std::stoi(args[i + 1]);
        } else if (args[i] == "--steps" && i + 1 < args.size()) {
            stepsPerEpisode = std::stoi(args[i + 1]);
        }
    }
    
    output << "Configuration:" << std::endl;
    output << "  Episodes: " << numEpisodes << std::endl;
    output << "  Steps per episode: " << stepsPerEpisode << std::endl;
    output << std::endl;
    
    // Initialize brain if needed
    if (!context.ensureBrain()) {
        return false;
    }
    
    // Run the original Phase 2 tests but in a more modular way
    output << "Running Test 1: Basic Neural Connectivity" << std::endl;
    runBasicConnectivityTest(context.brain);
    
    context.resetBrain();
    
    output << "\nRunning Test 2: Plasticity Learning Experiment" << std::endl;
    runPlasticityExperiment(context.brain);
    
    context.resetBrain();
    
    output << "\nRunning Test 3: STDP Verification" << std::endl;
    runStdpVerification(context.brain);
    
    output << "\n" << std::string(60, '=') << std::endl;
    output << "DEMONSTRATION COMPLETE" << std::endl;
    output << std::string(60, '=') << std::endl;
    
    return true;
}

// Test suite command
bool testCommand(const std::vector<std::string>& args, std::ostream& output, ApplicationContext& context) {
    output << "============================================" << std::endl;
    output << "NLM Test Suite" << std::endl;
    output << "============================================" << std::endl;
    output << std::endl;
    
    if (!context.ensureBrain()) {
        return false;
    }
    
    // Parse test options
    bool runAll = false;
    std::vector<std::string> specificTests;
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--all") {
            runAll = true;
        } else if (args[i].substr(0, 6) == "--test=") {
            specificTests.push_back(args[i].substr(6));
        }
    }
    
    if (runAll) {
        // Run all available tests
        output << "Running all tests..." << std::endl;
        
        // Test 1: Basic connectivity
        if (!runBasicConnectivityTest(context.brain)) {
            output << "FAILED: Basic connectivity test" << std::endl;
            return false;
        }
        output << "PASSED: Basic connectivity test" << std::endl;
        
        context.resetBrain();
        
        // Test 2: Plasticity
        if (!runPlasticityExperiment(context.brain)) {
            output << "FAILED: Plasticity test" << std::endl;
            return false;
        }
        output << "PASSED: Plasticity test" << std::endl;
        
        context.resetBrain();
        
        // Test 3: STDP
        if (!runStdpVerification(context.brain)) {
            output << "FAILED: STDP test" << std::endl;
            return false;
        }
        output << "PASSED: STDP test" << std::endl;
        
    } else if (!specificTests.empty()) {
        // Run specific tests
        for (const auto& testName : specificTests) {
            output << "Running test: " << testName << std::endl;
            // Implement specific test execution based on testName
            // For now, just acknowledge the test
            output << "Test " << testName << " execution not yet implemented" << std::endl;
        }
    } else {
        // Run basic test suite
        output << "Running basic test suite..." << std::endl;
        
        if (!runBasicConnectivityTest(context.brain)) {
            output << "FAILED: Basic connectivity test" << std::endl;
            return false;
        }
        output << "PASSED: Basic connectivity test" << std::endl;
        
        context.resetBrain();
        
        if (!runPlasticityExperiment(context.brain)) {
            output << "FAILED: Plasticity test" << std::endl;
            return false;
        }
        output << "PASSED: Plasticity test" << std::endl;
        
        context.resetBrain();
        
        if (!runStdpVerification(context.brain)) {
            output << "FAILED: STDP test" << std::endl;
            return false;
        }
        output << "PASSED: STDP test" << std::endl;
    }
    
    output << "\n" << std::string(60, '=') << std::endl;
    output << "TEST SUITE COMPLETE" << std::endl;
    output << std::string(60, '=') << std::endl;
    
    return true;
}

// Configuration commands
bool configCommand(const std::vector<std::string>& args, std::ostream& output, ApplicationContext& context) {
    if (args.empty()) {
        output << "Configuration commands require a subcommand." << std::endl;
        output << "Available config commands:" << std::endl;
        output << "  list              List all configuration values" << std::endl;
        output << "  get <key>         Get a configuration value" << std::endl;
        output << "  set <key> <value> Set a configuration value" << std::endl;
        output << "  load <file>       Load configuration from file" << std::endl;
        output << "  save <file>       Save configuration to file" << std::endl;
        output << "  clear             Clear all configuration values" << std::endl;
        output << "  help              Show configuration help" << std::endl;
        return true;
    }
    
    std::string subcommand = args[0];
    std::vector<std::string> subArgs(args.begin() + 1, args.end());
    
    if (subcommand == "list") {
        if (!context.config) {
            output << "No configuration available." << std::endl;
            return true;
        }
        output << "Configuration:" << std::endl;
        output << context.config->summary() << std::endl;
        return true;
    } else if (subcommand == "get") {
        if (subArgs.size() < 1) {
            output << "Usage: config get <key>" << std::endl;
            return false;
        }
        if (!context.config) {
            output << "No configuration available." << std::endl;
            return true;
        }
        std::string key = subArgs[0];
        if (context.config->has(key)) {
            output << key << " = ";
            // For simplicity, just show that it exists
            output << "[value]" << std::endl;
        } else {
            output << "Configuration key not found: " << key << std::endl;
        }
        return true;
    } else if (subcommand == "set") {
        if (subArgs.size() < 2) {
            output << "Usage: config set <key> <value>" << std::endl;
            return false;
        }
        if (!context.config) {
            output << "No configuration available." << std::endl;
            return true;
        }
        std::string key = subArgs[0];
        std::string value = subArgs[1];
        // Simple value parsing - try different types
        try {
            // Try boolean
            if (value == "true" || value == "false") {
                bool boolVal = (value == "true");
                context.config->set(key, boolVal);
            } 
            // Try integer
            else {
                size_t pos;
                int64_t intVal = std::stoll(value, &pos);
                if (pos == value.size()) {
                    context.config->set(key, intVal);
                } else {
                    // Try float
                    double floatVal = std::stod(value, &pos);
                    if (pos == value.size()) {
                        context.config->set(key, floatVal);
                    } else {
                        // Try string
                        context.config->set(key, value);
                    }
                }
            }
            output << "Configuration set: " << key << " = " << value << std::endl;
        } catch (...) {
            output << "Error parsing value: " << value << std::endl;
            return false;
        }
        return true;
    } else if (subcommand == "load") {
        if (subArgs.size() < 1) {
            output << "Usage: config load <file>" << std::endl;
            return false;
        }
        if (!context.config) {
            output << "No configuration available." << std::endl;
            return true;
        }
        std::string filename = subArgs[0];
        if (context.config->loadFromFile(filename)) {
            output << "Configuration loaded from: " << filename << std::endl;
        } else {
            output << "Failed to load configuration from: " << filename << std::endl;
        }
        return true;
    } else if (subcommand == "save") {
        if (subArgs.size() < 1) {
            output << "Usage: config save <file>" << std::endl;
            return false;
        }
        if (!context.config) {
            output << "No configuration available." << std::endl;
            return true;
        }
        std::string filename = subArgs[0];
        if (context.config->saveToFile(filename)) {
            output << "Configuration saved to: " << filename << std::endl;
        } else {
            output << "Failed to save configuration to: " << filename << std::endl;
        }
        return true;
    } else if (subcommand == "clear") {
        if (!context.config) {
            output << "No configuration available." << std::endl;
            return true;
        }
        context.config->clear();
        output << "Configuration cleared." << std::endl;
        return true;
    } else if (subcommand == "help") {
        output << "Configuration management commands:" << std::endl;
        output << "  list              List all configuration values" << std::endl;
        output << "  get <key>         Get a configuration value" << std::endl;
        output << "  set <key> <value> Set a configuration value" << std::endl;
        output << "  load <file>       Load configuration from file" << std::endl;
        output << "  save <file>       Save configuration to file" << std::endl;
        output << "  clear             Clear all configuration values" << std::endl;
        return true;
    }
    
    output << "Unknown config command: " << subcommand << std::endl;
    return false;
}

// Info commands
bool infoCommand(const std::vector<std::string>& args, std::ostream& output) {
    if (args.empty()) {
        output << "Info commands require a subcommand." << std::endl;
        output << "Available info commands:" << std::endl;
        output << "  version           Show version information" << std::endl;
        output << "  system           Show system information" << std::endl;
        output << "  brain            Show current brain information" << std::endl;
        return true;
    }
    
    std::string subcommand = args[0];
    
    if (subcommand == "version") {
        return versionCommand({}, output);
    } else if (subcommand == "system") {
        output << "System Information:" << std::endl;
        output << "  Platform: Linux" << std::endl;
        output << "  Compiler: C++20" << std::endl;
        output << "  Build type: Release" << std::endl;
        return true;
    } else if (subcommand == "brain") {
        output << "Brain Information:" << std::endl;
        output << "  (Brain information would be available here if a brain is loaded)" << std::endl;
        return true;
    }
    
    output << "Unknown info command: " << subcommand << std::endl;
    return false;
}

// Advanced commands
bool advancedCommand(const std::vector<std::string>& args, std::ostream& output, ApplicationContext& context) {
    if (args.empty()) {
        output << "Advanced commands:" << std::endl;
        output << "  benchmark            Run performance benchmarks" << std::endl;
        output << "  profile              Profile execution" << std::endl;
        output << "  analyze             Analyze brain state" << std::endl;
        output << "  export              Export brain state" << std::endl;
        output << "  import              Import brain state" << std::endl;
        return true;
    }
    
    std::string subcommand = args[0];
    
    if (subcommand == "benchmark") {
        output << "Benchmark functionality not yet implemented." << std::endl;
        return true;
    } else if (subcommand == "profile") {
        output << "Profile functionality not yet implemented." << std::endl;
        return true;
    } else if (subcommand == "analyze") {
        output << "Analysis functionality not yet implemented." << std::endl;
        return true;
    } else if (subcommand == "export") {
        output << "Export functionality not yet implemented." << std::endl;
        return true;
    } else if (subcommand == "import") {
        output << "Import functionality not yet implemented." << std::endl;
        return true;
    }
    
    output << "Unknown advanced command: " << subcommand << std::endl;
    return false;
}

// Command registration function
void registerCommands() {
    CommandRegistry& registry = CommandRegistry::getInstance();
    
    // Help commands
    registry.registerCommand(Command("help", "Show help information", 
                                     helpCommand, false, "General"));
    
    // Information commands
    registry.registerCommand(Command("version", "Show version information", 
                                     versionCommand, false, "Info"));
    registry.registerCommand(Command("info", "Show system and brain information", 
                                     infoCommand, false, "Info"));
    
    // Demo commands
    registry.registerCommand(Command("run", "Run Phase 2 demonstration", 
                                     [](const std::vector<std::string>& args, std::ostream& output) {
                                         static ApplicationContext context;
                                         return runDemoCommand(args, output, context);
                                     }, true, "Demo"));
    
    // Test commands
    registry.registerCommand(Command("test", "Run test suite", 
                                     [](const std::vector<std::string>& args, std::ostream& output) {
                                         static ApplicationContext context;
                                         return testCommand(args, output, context);
                                     }, true, "Test"));
    
    // Configuration commands
    registry.registerCommand(Command("config", "Manage configuration", 
                                     [](const std::vector<std::string>& args, std::ostream& output) {
                                         static ApplicationContext context;
                                         return configCommand(args, output, context);
                                     }, false, "Config"));
    
    // Advanced commands
    registry.registerCommand(Command("advanced", "Run advanced operations", 
                                     [](const std::vector<std::string>& args, std::ostream& output) {
                                         static ApplicationContext context;
                                         return advancedCommand(args, output, context);
                                     }, true, "Advanced"));
}

// Main command line parser
class CommandLineInterface {
public:
    bool parse(int argc, char** argv) {
        if (argc < 2) {
            std::cerr << "Error: No command provided." << std::endl;
            std::cerr << "Use 'nlm help' to see available commands." << std::endl;
            return false;
        }
        
        std::string commandName = argv[1];
        std::vector<std::string> args;
        
        // Collect all arguments except command name
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        Command* command = CommandRegistry::getInstance().getCommand(commandName);
        if (!command) {
            std::cerr << "Error: Unknown command: " << commandName << std::endl;
            return false;
        }
        
        currentCommand = command;
        currentArgs = args;
        
        return true;
    }
    
    bool execute() {
        if (!currentCommand) {
            std::cerr << "Error: No command to execute." << std::endl;
            return false;
        }
        
        std::cout << "Executing command: " << currentCommand->getName() << std::endl;
        std::cout << "Description: " << currentCommand->getDescription() << std::endl;
        
        // For commands that require brain, ensure it's available
        if (currentCommand->requiresBrain()) {
            ApplicationContext& context = getApplicationContext();
            if (!context.ensureBrain()) {
                std::cerr << "Error: Failed to initialize brain for command." << std::endl;
                return false;
            }
        }
        
        return currentCommand->execute(currentArgs, std::cout);
    }
    
    void showHelp() {
        CommandRegistry::getInstance().listCommands(std::cout);
    }
    
    ApplicationContext& getApplicationContext() {
        static ApplicationContext context;
        return context;
    }
    
private:
    Command* currentCommand = nullptr;
    std::vector<std::string> currentArgs;
};

int main(int argc, char** argv) {
    std::cout << "NLM (Neural Learning Machine) - Modular CLI" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;
    
    // Register all commands
    registerCommands();
    
    CommandLineInterface cli;
    
    if (!cli.parse(argc, argv)) {
        return 1;
    }
    
    // Show command help if requested
    if (argc >= 2 && (argv[1] == "help" || argv[1] == "--help")) {
        cli.showHelp();
        return 0;
    }
    
    // Initialize application context
    ApplicationContext& context = cli.getApplicationContext();
    
    // Create default configuration
    auto config = std::make_shared<Config>();
    context.initialize(config);
    
    // Parse command line arguments into config
    context.config->loadFromArgs(argc, argv);
    
    // Execute the command
    if (!cli.execute()) {
        std::cerr << "Command execution failed." << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "Command completed successfully." << std::endl;
    
    return 0;
}

} // namespace nlm

// Implementation of helper functions that were forward-declared
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printConfigurationSummary(std::shared_ptr<Config> config) {
    std::cout << "\nConfiguration Summary:" << std::endl;
    std::cout << "  random_seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
    std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
    std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 500) << std::endl;
    std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
    std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.15f) << std::endl;
    std::cout << "  stdp_ltp_weight: " << config->getOr<float>("stdp_ltp_weight", 0.02f) << std::endl;
    std::cout << "  stdp_ltd_weight: " << config->getOr<float>("stdp_ltd_weight", 0.015f) << std::endl;
    std::cout << "  synaptogenesis_rate: " << config->getOr<float>("synaptogenesis_rate", 0.0001f) << std::endl;
    std::cout << "  pruning_rate: " << config->getOr<float>("pruning_rate", 0.00001f) << std::endl;
}