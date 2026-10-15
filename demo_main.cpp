// NLM (Neural Learning Machine) - Command Line Interface
// Demonstration of the CLI framework

#include <iostream>
#include <string>
#include <vector>
#include <map>

class CLIDemo {
public:
    void showHelp() const {
        std::cout << "NLM (Neural Learning Machine) CLI - Version 0.1.0" << std::endl;
        std::cout << "===============================================" << std::endl;
        std::cout << std::endl;
        
        std::cout << "BASIC COMMANDS:" << std::endl;
        std::cout << "  nlm --help                Show this help information" << std::endl;
        std::cout << "  nlm version               Display version information" << std::endl;
        std::cout << "  nlm --verbose             Enable verbose logging" << std::endl;
        std::cout << std::endl;
        
        std::cout << "CONFIGURATION MANAGEMENT:" << std::endl;
        std::cout << "  nlm --config <file>       Load configuration from file" << std::endl;
        std::cout << "  nlm --set <key> <value>   Set configuration parameter" << std::endl;
        std::cout << "  nlm --show-config         Display current configuration" << std::endl;
        std::cout << std::endl;
        
        std::cout << "SIMULATION CONTROL:" << std::endl;
        std::cout << "  nlm run <steps>           Run simulation for specified steps" << std::endl;
        std::cout << "  nlm run --with-agent      Enable agent-based simulation with world interaction" << std::endl;
        std::cout << "  nlm step <n>              Execute specified number of steps" << std::endl;
        std::cout << std::endl;
        
        std::cout << "DATA COLLECTION AND OUTPUT:" << std::endl;
        std::cout << "  nlm save <file>           Save brain state to file" << std::endl;
        std::cout << "  nlm load <file>           Load brain state from file" << std::endl;
        std::cout << "  nlm export-stats <file>   Export simulation statistics" << std::endl;
        std::cout << "  nlm print-status          Display current brain status" << std::endl;
        std::cout << std::endl;
        
        std::cout << "ADVANCED FEATURES:" << std::endl;
        std::cout << "  nlm experiment <name> <steps>  Run predefined experiment" << std::endl;
        std::cout << "  nlm benchmark              Run performance benchmarks" << std::endl;
        std::cout << "  nlm validate              Validate configuration and system integrity" << std::endl;
        std::cout << std::endl;
        
        std::cout << "AGENT-WORLD INTEGRATION:" << std::endl;
        std::cout << "  nlm simulate --world-size <w> <h> --vision <w> <h>  Create and simulate world" << std::endl;
        std::cout << "  nlm episode <steps>       Run a complete agent episode" << std::endl;
        std::cout << "  nlm train <episodes>     Train agent for specified episodes" << std::endl;
        std::cout << std::endl;
        
        std::cout << "EXAMPLES:" << std::endl;
        std::cout << "  nlm run --steps 100 --with-agent --world-size 20 20 --vision 8 8" << std::endl;
        std::cout << "  nlm experiment phase3 500" << std::endl;
        std::cout << "  nlm benchmark --name basic_connectivity" << std::endl;
    }
    
    void showVersion() const {
        std::cout << "NLM CLI Version 0.1.0" << std::endl;
        std::cout << "Built with C++20" << std::endl;
        std::cout << "Architecture: Native CLI with performance optimizations" << std::endl;
    }
    
    void showError(const std::string& message) const {
        std::cerr << "Error: " << message << std::endl;
    }
    
    void showSuccess(const std::string& message) const {
        std::cout << "Success: " << message << std::endl;
    }
};

int main(int argc, char** argv) {
    if (argc <= 1) {
        // No arguments, show help
        CLIDemo demo;
        demo.showHelp();
        return 0;
    }
    
    std::string command = argv[1];
    CLIDemo demo;
    
    if (command == "--help" || command == "-h") {
        demo.showHelp();
        return 0;
    } else if (command == "version") {
        demo.showVersion();
        return 0;
    } else if (command == "--verbose") {
        demo.showSuccess("Verbose logging enabled");
        return 0;
    } else if (command == "--config") {
        if (argc >= 3) {
            demo.showSuccess(std::string("Configuration loaded from: ") + argv[2]);
            return 0;
        } else {
            demo.showError("Missing configuration file argument for --config command");
            return 1;
        }
    } else if (command == "--set") {
        if (argc >= 4) {
            demo.showSuccess(std::string("Configuration parameter set: ") + argv[2] + " = " + argv[3]);
            return 0;
        } else {
            demo.showError("Missing arguments for --set command");
            return 1;
        }
    } else if (command == "--show-config") {
        demo.showSuccess("Configuration displayed (demo mode)");
        return 0;
    } else if (command == "run") {
        std::string steps = "100"; // Default
        bool withAgent = false;
        
        // Parse arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--with-agent") {
                withAgent = true;
            } else if (arg == "--steps") {
                if (i + 1 < argc) {
                    steps = argv[++i];
                }
            } else {
                try {
                    std::stoul(arg);
                } catch (...) {
                    // Not a number, ignore
                }
            }
        }
        
        demo.showSuccess(std::string("Starting NLM Phase 2 simulation...") + " Steps: " + steps + ", Agent: " + (withAgent ? "Enabled" : "Disabled"));
        return 0;
    } else if (command == "experiment") {
        if (argc >= 4) {
            demo.showSuccess(std::string("Running experiment: ") + argv[2] + " with " + argv[3] + " steps");
            return 0;
        } else {
            demo.showError("Missing arguments for experiment command");
            return 1;
        }
    } else if (command == "benchmark") {
        demo.showSuccess("Running benchmark (demo mode)");
        return 0;
    } else if (command == "validate") {
        demo.showSuccess("Configuration validated (demo mode)");
        return 0;
    } else {
        demo.showError(std::string("Unknown command: ") + command);
        demo.showSuccess("Use 'nlm --help' for available commands");
        return 1;
    }
}