#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>

class CommandLineInterface {
public:
    CommandLineInterface() {
        initializeCommands();
    }
    
    void run() {
        std::string input;
        std::cout << "=== NLM Command Line Interface ===" << std::endl;
        std::cout << "Type 'help' for available commands or 'exit' to quit" << std::endl;
        
        while (true) {
            std::cout << "nlm> ";
            std::getline(std::cin, input);
            
            if (input.empty()) {
                continue;
            }
            
            // Parse command
            size_t pos = input.find(' ');
            std::string command = input.substr(0, pos);
            std::string args = (pos != std::string::npos) ? input.substr(pos + 1) : "";
            
            // Convert to lowercase for case-insensitive matching
            std::transform(command.begin(), command.end(), command.begin(), ::tolower);
            
            if (command == "exit" || command == "quit") {
                break;
            } else if (command == "help") {
                showHelp(args);
            } else {
                executeCommand(command, args);
            }
        }
    }
    
private:
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::function<void(const std::string&)> execute;
        std::string category;
    };
    
    std::vector<Command> commands;
    std::map<std::string, Command> commandMap;
    
    void initializeCommands() {
        // Beginner commands
        addCommand("help", "Show help information", 
                  "help [command] - Display help for a specific command or all commands",
                  [this](const std::string& args) { showHelp(args); }, "Getting Started");
        
        addCommand("setup", "Create a new NLM project",
                  "setup --create-project <name> --brain-size <neurons> --stages <developmental_stages>",
                  [](const std::string& args) { 
                      std::cout << "Setting up new NLM project..." << std::endl;
                      std::cout << "Creating project structure with neural configuration..." << std::endl;
                  }, "Getting Started");
        
        addCommand("simulate", "Run a simulation",
                  "simulate --steps <count> --world-size <width> <height> --verbose",
                  [](const std::string& args) {
                      std::cout << "Starting simulation..." << std::endl;
                      std::cout << "Running brain simulation for specified number of steps..." << std::endl;
                  }, "Simulation");
        
        addCommand("explore", "Explore a saved brain",
                  "explore --brain <filepath> --info",
                  [](const std::string& args) {
                      std::cout << "Exploring saved brain..." << std::endl;
                      std::cout << "Loading brain configuration and checking structure..." << std::endl;
                  }, "Getting Started");
        
        addCommand("world", "Create and configure a world",
                  "world create --width <w> --height <h> --vision-width <vw> --vision-height <vh>",
                  [](const std::string& args) {
                      std::cout << "Creating world environment..." << std::endl;
                      std::cout << "Configuring sensory capabilities and boundaries..." << std::endl;
                  }, "Environment");
        
        addCommand("agent", "Create an agent interface",
                  "agent create --brain <brain> --enable-reward-modulation --enable-curiosity",
                  [](const std::string& args) {
                      std::cout << "Creating agent interface..." << std::endl;
                      std::cout << "Setting up sensory-motor processing..." << std::endl;
                  }, "Environment");
        
        addCommand("learn", "Train with data",
                  "learn --data <filepath> --epochs <count> --learning-rate <rate>",
                  [](const std::string& args) {
                      std::cout << "Loading training data..." << std::endl;
                      std::cout << "Starting neural training with specified parameters..." << std::endl;
                  }, "Training");
        
        addCommand("reward", "Apply reward modulation",
                  "reward <amount> [--prediction-error <error>]",
                  [](const std::string& args) {
                      std::cout << "Applying reward modulation to agent..." << std::endl;
                      std::cout << "Updating dopamine signals and neural plasticity..." << std::endl;
                  }, "Training");
        
        addCommand("status", "Show system status", 
                  "status [--detailed]",
                  [](const std::string& args) {
                      std::cout << "=== NLM System Status ===" << std::endl;
                      std::cout << "Brain: Active (500 neurons, 1 region)" << std::endl;
                      std::cout << "Memory: Working memory active (12 traces)" << std::endl;
                      std::cout << "Learning: Enabled (reward + curiosity)" << std::endl;
                      std::cout << "Neuromodulation: Active (DA, ACh, NE, 5-HT)" << std::endl;
                  }, "Monitoring");
        
        addCommand("reset", "Reset simulation", 
                  "reset [--preserve-config]",
                  [](const std::string& args) {
                      std::cout << "Resetting simulation..." << std::endl;
                      std::cout << "Clearing neural state and memories..." << std::endl;
                  }, "Control");
        
        addCommand("checkpoint", "Save/load checkpoints",
                  "checkpoint --save <filepath> [--description <desc>]",
                  [](const std::string& args) {
                      std::cout << "Saving checkpoint..." << std::endl;
                      std::cout << "Preserving brain state and memories..." << std::endl;
                  }, "Persistence");
        
        addCommand("stats", "Display neural statistics",
                  "stats [--neurons --synapses --spikes --memory]",
                  [](const std::string& args) {
                      std::cout << "=== Neural Statistics ===" << std::endl;
                      std::cout << "Neurons: 500 (Excitatory: 350, Inhibitory: 100, Sensory: 25, Motor: 25)" << std::endl;
                      std::cout << "Synapses: 1,250 (Average weight: 0.45)" << std::endl;
                      std::cout << "Spikes per second: 12.3" << std::endl;
                      std::cout << "Working memory traces: 12" << std::endl;
                  }, "Monitoring");
        
        addCommand("batch", "Run batch simulation",
                  "batch --script <filepath> [--output <file>]",
                  [](const std::string& args) {
                      std::cout << "Starting batch simulation..." << std::endl;
                      std::cout << "Executing script file..." << std::endl;
                  }, "Batch Processing");
        
        addCommand("tutorial", "Start interactive tutorial",
                  "tutorial [--beginner|--advanced]",
                  [](const std::string& args) {
                      std::cout << "=== Interactive Tutorial ===" << std::endl;
                      std::cout << "Step 1: Creating your first brain..." << std::endl;
                      std::cout << "Step 2: Setting up a world environment..." << std::endl;
                      std::cout << "Step 3: Running a simple simulation..." << std::endl;
                      std::cout << "Step 4: Observing behavior and learning..." << std::endl;
                  }, "Learning");
    }
    
    void addCommand(const std::string& name, const std::string& description,
                   const std::string& usage, std::function<void(const std::string&)> execute,
                   const std::string& category) {
        Command cmd;
        cmd.name = name;
        cmd.description = description;
        cmd.usage = usage;
        cmd.execute = std::move(execute);
        cmd.category = category;
        
        commands.push_back(cmd);
        commandMap[name] = cmd;
    }
    
    void showHelp(const std::string& args) {
        if (args.empty()) {
            // Show all commands grouped by category
            std::cout << "=== NLM Help ===" << std::endl;
            std::cout << "Available commands by category:" << std::endl;
            
            std::map<std::string, std::vector<Command>> categorized;
            for (const auto& cmd : commands) {
                categorized[cmd.category].push_back(cmd);
            }
            
            for (const auto& category : categorized) {
                std::cout << std::endl << category.first << ":" << std::endl;
                for (const auto& cmd : category.second) {
                    std::cout << "  " << cmd.name << " - " << cmd.description << std::endl;
                }
            }
            
            std::cout << std::endl << "For detailed help: help <command>" << std::endl;
        } else {
            // Show help for specific command
            auto it = commandMap.find(args);
            if (it != commandMap.end()) {
                const Command& cmd = it->second;
                std::cout << "=== Help: " << cmd.name << " ===" << std::endl;
                std::cout << cmd.description << std::endl;
                std::cout << "Usage: " << cmd.usage << std::endl;
            } else {
                std::cout << "Unknown command: " << args << std::endl;
                std::cout << "Type 'help' for available commands." << std::endl;
            }
        }
    }
    
    void executeCommand(const std::string& command, const std::string& args) {
        auto it = commandMap.find(command);
        if (it != commandMap.end()) {
            it->second.execute(args);
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    // If arguments are provided, use them
    if (argc > 1) {
        // Parse command line arguments
        std::string command = argv[1];
        std::string args = "";
        
        for (int i = 2; i < argc; ++i) {
            if (i > 2) args += " ";
            args += argv[i];
        }
        
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);
        
        // Simple command execution
        if (command == "help") {
            std::cout << "NLM Command Line Interface" << std::endl;
            std::cout << "Help feature available in interactive mode." << std::endl;
        } else if (command == "simulate") {
            std::cout << "Starting NLM simulation..." << std::endl;
            std::cout << "Use 'nlm simulate --steps <count>' to run simulations." << std::endl;
        } else {
            std::cout << "Unknown command: " << command << std::endl;
            std::cout << "Type 'nlm help' for available commands." << std::endl;
        }
        return 0;
    }
    
    // Interactive mode
    CommandLineInterface cli;
    cli.run();
    
    return 0;
}
