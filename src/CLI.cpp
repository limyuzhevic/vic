// CLI.cpp - CLI implementation
// This file is included in the main CMakeLists.txt build

#include "CLI.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {

CLI::CLI() {
    setupCommands();
}

CLI::~CLI() = default;

void CLI::setupCommands() {
    // Run command - main brain execution
    class RunCommand : public CLICommand {
    public:
        std::string name() const override { return "run"; }
        std::string description() const override { 
            return "Run the NLM brain with specified configuration"; 
        }
        
        bool execute(const CLIArgs& args) override {
            std::cout << "=== NLM Run Command ===\n";
            
            // Load configuration
            auto config = std::make_shared<Config>();
            
            // Try to load from config file if specified
            if (!args.configFile.empty()) {
                if (config->loadFromFile(args.configFile)) {
                    std::cout << "Loaded configuration from: " << args.configFile << "\n";
                } else {
                    std::cerr << "Warning: Could not load config file: " << args.configFile << "\n";
                }
            }
            
            // Set defaults for Phase 2
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            
            // Parse options
            for (const auto& [key, value] : args.options) {
                if (key == "neurons") {
                    config->set("neuron_count", std::stoi(value), ConfigSource::CommandLine);
                } else if (key == "steps") {
                    config->set("max_steps", std::stoi(value), ConfigSource::CommandLine);
                } else if (key == "timestep") {
                    config->set("simulation_timestep", std::stod(value), ConfigSource::CommandLine);
                }
            }
            
            // TODO: Create and run brain
            std::cout << "Configuration loaded successfully.\n";
            std::cout << "Note: Brain execution would be implemented here.\n";
            
            return true;
        }
        
        void printHelp() const override {
            std::cout << "Usage: nlm run [OPTIONS]\n";
            std::cout << "Run the NLM brain simulation.\n\n";
            std::cout << "Options:\n";
            std::cout << "  --config FILE        Load configuration from JSON file\n";
            std::cout << "  --neurons N          Number of neurons (default: 500)\n";
            std::cout << "  --steps N            Number of simulation steps (default: 1000)\n";
            std::cout << "  --timestep T         Simulation timestep (default: 0.001)\n";
            std::cout << "  --seed N             Random seed (default: 42)\n";
        }
    };
    
    // Demo command - run various demonstrations
    class DemoCommand : public CLICommand {
    public:
        std::string name() const override { return "demo"; }
        std::string description() const override { 
            return "Run demonstrations of different phases"; 
        }
        
        bool execute(const CLIArgs& args) override {
            std::string demoType = "phase2";
            if (!args.args.empty()) {
                demoType = args.args[0];
            }
            
            std::cout << "=== NLM Demo Command: " << demoType << " ===\n";
            
            if (demoType == "phase2") {
                std::cout << "Running Phase 2: Real Neural Computation Demo\n";
                std::cout << "Note: Phase 2 demo would be executed here.\n";
            } else if (demoType == "phase3") {
                std::cout << "Running Phase 3: World Interaction Demo\n";
                std::cout << "Note: Phase 3 demo would be executed here.\n";
            } else if (demoType == "phase4") {
                std::cout << "Running Phase 4: Emerging Cognition Demo\n";
                std::cout << "Note: Phase 4 demo would be executed here.\n";
            } else if (demoType == "phase6") {
                std::cout << "Running Phase 6: Integration Demo\n";
                std::cout << "Note: Phase 6 demo would be executed here.\n";
            } else {
                std::cerr << "Error: Unknown demo type: " << demoType << "\n";
                return false;
            }
            
            return true;
        }
        
        void printHelp() const override {
            std::cout << "Usage: nlm demo [DEMO_TYPE]\n";
            std::cout << "Run demonstrations of NLM functionality.\n\n";
            std::cout << "DEMO_TYPE options:\n";
            std::cout << "  phase2              Phase 2: Real Neural Computation\n";
            std::cout << "  phase3              Phase 3: World Interaction\n";
            std::cout << "  phase4              Phase 4: Emerging Cognition\n";
            std::cout << "  phase6              Phase 6: Integration Demo\n";
            std::cout << "\nIf no demo type specified, runs Phase 2 by default.\n";
        }
    };
    
    // Test command - run verification tests
    class TestCommand : public CLICommand {
    public:
        std::string name() const override { return "test"; }
        std::string description() const override { 
            return "Run verification and integration tests"; 
        }
        
        bool execute(const CLIArgs& args) override {
            std::string testType = "integration";
            if (!args.args.empty()) {
                testType = args.args[0];
            }
            
            std::cout << "=== NLM Test Command: " << testType << " ===\n";
            
            if (testType == "integration") {
                std::cout << "Running Integration Tests...\n";
                std::cout << "Note: Integration tests would be executed here.\n";
            } else if (testType == "performance") {
                std::cout << "Running Performance Tests...\n";
                std::cout << "Note: Performance tests would be executed here.\n";
            } else if (testType == "memory") {
                std::cout << "Running Memory Tests...\n";
                std::cout << "Note: Memory tests would be executed here.\n";
            } else {
                std::cerr << "Error: Unknown test type: " << testType << "\n";
                return false;
            }
            
            return true;
        }
        
        void printHelp() const override {
            std::cout << "Usage: nlm test [TEST_TYPE]\n";
            std::cout << "Run verification and integration tests.\n\n";
            std::cout << "TEST_TYPE options:\n";
            std::cout << "  integration          Integration and connectivity tests\n";
            std::cout << "  performance         Performance and benchmark tests\n";
            std::cout << "  memory              Memory and resource tests\n";
            std::cout << "\nIf no test type specified, runs integration tests by default.\n";
        }
    };
    
    // Benchmark command - performance benchmarking
    class BenchmarkCommand : public CLICommand {
    public:
        std::string name() const override { return "benchmark"; }
        std::string description() const override { 
            return "Run performance benchmarks"; 
        }
        
        bool execute(const CLIArgs& args) override {
            std::cout << "=== NLM Benchmark Command ===\n";
            std::cout << "Running performance benchmarks...\n";
            std::cout << "Note: Benchmark execution would be implemented here.\n";
            return true;
        }
        
        void printHelp() const override {
            std::cout << "Usage: nlm benchmark [OPTIONS]\n";
            std::cout << "Run performance benchmarks.\n\n";
            std::cout << "Options:\n";
            std::cout << "  --neurons N          Number of neurons to test (default: 500)\n";
            std::cout << "  --steps N            Number of steps to simulate (default: 1000)\n";
            std::cout << "  --runs N             Number of benchmark runs (default: 3)\n";
            std::cout << "  --detailed          Enable detailed performance reporting\n";
        }
    };
    
    // Config command - configuration management
    class ConfigCommand : public CLICommand {
    public:
        std::string name() const override { return "config"; }
        std::string description() const override { 
            return "Manage NLM configuration"; 
        }
        
        bool execute(const CLIArgs& args) override {
            std::cout << "=== NLM Config Command ===\n";
            
            if (args.args.empty()) {
                std::cout << "Available config operations:\n";
                std::cout << "  show              Show current configuration\n";
                std::cout << "  create            Create new configuration file\n";
                std::cout << "  validate          Validate configuration\n";
                return true;
            }
            
            std::string operation = args.args[0];
            
            if (operation == "show") {
                std::cout << "Configuration show would be implemented here.\n";
            } else if (operation == "create") {
                std::cout << "Configuration create would be implemented here.\n";
            } else if (operation == "validate") {
                std::cout << "Configuration validation would be implemented here.\n";
            } else {
                std::cerr << "Error: Unknown config operation: " << operation << "\n";
                return false;
            }
            
            return true;
        }
        
        void printHelp() const override {
            std::cout << "Usage: nlm config [OPERATION]\n";
            std::cout << "Manage NLM configuration.\n\n";
            std::cout << "OPERATIONS:\n";
            std::cout << "  show              Show current configuration\n";
            std::cout << "  create            Create new configuration file\n";
            std::cout << "  validate          Validate configuration\n";
        }
    };
    
    addCommand(std::make_unique<RunCommand>());
    addCommand(std::make_unique<DemoCommand>());
    addCommand(std::make_unique<TestCommand>());
    addCommand(std::make_unique<BenchmarkCommand>());
    addCommand(std::make_unique<ConfigCommand>());
}

void CLI::addCommand(std::unique_ptr<CLICommand> command) {
    commands_.push_back(std::move(command));
}

bool CLI::run(int argc, char** argv) {
    CLIArgs args = parseArgs(argc, argv);
    
    if (args.help) {
        printHelp(args.command.empty() ? "" : args.command);
        return true;
    }
    
    if (args.version) {
        printVersion();
        return true;
    }
    
    // Find and execute command
    for (const auto& command : commands_) {
        if (command->name() == args.command) {
            return command->execute(args);
        }
    }
    
    // Command not found
    std::cerr << "Error: Unknown command: " << args.command << "\n";
    std::cerr << "Run 'nlm --help' for available commands.\n";
    return false;
}

void CLI::printHelp(const std::string& command) const {
    if (command.empty()) {
        std::cout << "NLM (熙然) - Neural Learning Machine\n";
        std::cout << "An experimental artificial developmental brain\n\n";
        std::cout << "Usage: nlm <COMMAND> [OPTIONS]\n\n";
        std::cout << "Available commands:\n";
        
        for (const auto& cmd : commands_) {
            std::cout << "  " << cmd->name() << " - " << cmd->description() << "\n";
        }
        
        std::cout << "\nUse 'nlm <COMMAND> --help' for command-specific help.\n";
    } else {
        for (const auto& cmd : commands_) {
            if (cmd->name() == command) {
                cmd->printHelp();
                return;
            }
        }
        std::cerr << "Error: Unknown command: " << command << "\n";
    }
}

void CLI::printVersion() const {
    std::cout << "NLM (熙然) v0.1.0\n";
    std::cout << "An experimental artificial developmental brain\n";
    std::cout << "Phase 6: Final Integration\n";
}

CLIArgs CLI::parseArgs(int argc, char** argv) {
    CLIArgs args;
    
    // Parse command line
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--version") {
            args.version = true;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg.find("--") == 0) {
            // Option: --option value
            size_t equals = arg.find('=');
            if (equals != std::string::npos) {
                // --option=value
                std::string key = arg.substr(2, equals - 2);
                std::string value = arg.substr(equals + 1);
                args.options[key] = value;
            } else {
                // --option (flag)
                std::string key = arg.substr(2);
                args.flags[key] = "true";
                
                // Next argument might be value
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    i++;
                    args.options[key] = argv[i];
                }
            }
        } else if (arg.find("-") == 0) {
            // Short option
            std::string key = arg.substr(1);
            args.flags[key] = "true";
        } else {
            // Positional argument
            args.args.push_back(arg);
            
            // First positional argument is the command
            if (args.command.empty()) {
                args.command = arg;
            }
        }
    }
    
    return args;
}

} // namespace nlm
