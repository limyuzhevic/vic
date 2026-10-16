#include "InteractiveMode.hpp"
#include "ProgressIndicator.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <termios.h>
#include <unistd.h>

namespace nlm {

struct InteractiveMode::Impl {
    std::shared_ptr<Brain> brain;
    Settings settings;
    std::atomic<bool> running{false};
    std::vector<std::string> commandHistory;
    std::map<std::string, std::function<void(const std::vector<std::string>&)>> commandHandlers;
    std::string status;
    bool progressEnabled = false;
    ProgressIndicator progressIndicator;
    std::chrono::system_clock::time_point sessionStartTime;
    struct termios originalTerminalSettings;
    bool rawInputEnabled = false;
};

InteractiveMode::InteractiveMode(std::shared_ptr<Brain> brain, const Settings& settings) : pImpl(std::make_unique<Impl>()) {
    pImpl->brain = brain;
    pImpl->settings = settings;
    pImpl->sessionStartTime = std::chrono::system_clock::now();
    
    if (pImpl->settings.useRawInput && pImpl->settings.enableColor) {
        // Save original terminal settings
        tcgetattr(STDIN_FILENO, &pImpl->originalTerminalSettings);
        
        // Set raw mode
        struct termios rawSettings;
        tcgetattr(STDIN_FILENO, &rawSettings);
        
        // Disable canonical mode and echo
        rawSettings.c_lflag &= ~(ICANON | ECHO);
        rawSettings.c_cc[VMIN] = 1;
        rawSettings.c_cc[VTIME] = 0;
        
        tcsetattr(STDIN_FILENO, TCSANOW, &rawSettings);
        pImpl->rawInputEnabled = true;
    }
}

InteractiveMode::~InteractiveMode() {
    stop();
    
    if (pImpl->rawInputEnabled) {
        // Restore original terminal settings
        tcsetattr(STDIN_FILENO, TCSANOW, &pImpl->originalTerminalSettings);
    }
}

bool InteractiveMode::start() {
    if (pImpl->running) return false;
    
    pImpl->running = true;
    pImpl->sessionStartTime = std::chrono::system_clock::now();
    
    displayLogo();
    
    std::cout << "\nNLM Interactive Mode Started\n";
    std::cout << "Type 'help' for available commands or 'quit' to exit.\n\n";
    
    // Add default commands
    addCommand("help", "Show help information", 
               [this](const std::vector<std::string>&) { showHelp(); });
    
    addCommand("quit", "Exit interactive mode", 
               [this](const std::vector<std::string>&) { stop(); });
    
    addCommand("clear", "Clear the screen", 
               [this](const std::vector<std::string>&) { clearScreen(); });
    
    addCommand("history", "Show command history", 
               [this](const std::vector<std::string>&) { showHistory(); });
    
    addCommand("status", "Show current brain status", 
               [this](const std::vector<std::string>&) { displayStatus(); });
    
    addCommand("reset", "Reset the brain to initial state",
               [this](const std::vector<std::string>&) {
                   if (pImpl->brain) {
                       pImpl->brain->reset();
                       pImpl->brain->initialize();
                       setStatus("Brain reset and reinitialized.");
                   }
               });
    
    addCommand("step", "Advance simulation by one step",
               [this](const std::vector<std::string>& args) {
                   if (pImpl->brain && args.size() > 0) {
                       int steps = std::stoi(args[0]);
                       ProgressIndicator progress;
                       progress.start("Simulation Steps");
                       
                       for (int i = 0; i < steps; ++i) {
                           pImpl->brain->step(i, i * 0.001);
                           progress.update(static_cast<double>(i + 1) / steps,
                                         "Step " + std::to_string(i + 1) + "/" + std::to_string(steps));
                       }
                       
                       progress.finish("Simulation completed.");
                       setStatus("Brain simulation completed.");
                   }
               });
    
    addCommand("inject", "Inject current into neurons",
               [this](const std::vector<std::string>& args) {
                   if (pImpl->brain && args.size() > 0) {
                       NeuronType type = NeuronType::Excitatory;
                       if (args.size() > 1 && args[1] == "inhibitory") {
                           type = NeuronType::Inhibitory;
                       }
                       
                       double current = 0.0;
                       if (args.size() > 2) {
                           current = std::stod(args[2]);
                       } else {
                           current = 50.0;
                       }
                       
                       int count = 1;
                       if (args.size() > 0) {
                           count = std::stoi(args[0]);
                       }
                       
                       pImpl->brain->injectCurrentToNeurons(type, static_cast<MembranePotential>(current));
                       setStatus("Injected current into " + std::to_string(count) + " " 
                               + (type == NeuronType::Excitatory ? "excitatory" : "inhibitory") 
                               + " neurons.");
                   }
               });
    
    addCommand("save", "Save brain state to file",
               [this](const std::vector<std::string>& args) {
                   if (pImpl->brain && args.size() > 0) {
                       std::string filename = args[0];
                       if (pImpl->brain->save(filename)) {
                           setStatus("Brain state saved to " + filename);
                       } else {
                           setStatus("Failed to save brain state to " + filename);
                       }
                   }
               });
    
    addCommand("load", "Load brain state from file",
               [this](const std::vector<std::string>& args) {
                   if (args.size() > 0) {
                       std::string filename = args[0];
                       if (pImpl->brain && pImpl->brain->load(filename)) {
                           setStatus("Brain state loaded from " + filename);
                       } else {
                           setStatus("Failed to load brain state from " + filename);
                       }
                   }
               });
    
    addCommand("plasticity", "Enable or disable plasticity",
               [this](const std::vector<std::string>& args) {
                   if (!pImpl->brain) return;
                   
                   bool enable = true;
                   if (args.size() > 0 && args[0] == "off") {
                       enable = false;
                   }
                   
                   auto* region = pImpl->brain->getRegion(RegionId(1));
                   if (region) {
                       for (auto& syn : region->getSynapses()) {
                           syn->enablePlasticity(enable, enable, enable);
                       }
                       setStatus("Plasticity " + std::string(enable ? "enabled" : "disabled"));
                   }
               });
    
    // Add command completions for default commands
    if (pImpl->settings.enableCommandAutoComplete) {
        pImpl->commandCompletions.clear();
        for (const auto& pair : pImpl->commandHandlers) {
            pImpl->commandCompletions.push_back(pair.first);
        }
    }
    
    // Main interactive loop
    while (pImpl->running) {
        std::string line = readLine(pImpl->settings.prompt);
        if (line.empty()) continue;
        
        if (pImpl->settings.enableHistory) {
            pImpl->commandHistory.push_back(line);
        }
        
        runCommand(line);
    }
    
    return true;
}

void InteractiveMode::stop() {
    pImpl->running = false;
}

bool InteractiveMode::isRunning() const {
    return pImpl->running;
}

void InteractiveMode::addCommand(const std::string& name, const std::string& description,
                                std::function<void(const std::vector<std::string>&)> handler) {
    pImpl->commandHandlers[name] = handler;
}

void InteractiveMode::removeCommand(const std::string& name) {
    pImpl->commandHandlers.erase(name);
}

void InteractiveMode::listCommands() const {
    std::cout << "Available Commands:\n";
    int maxNameLength = 0;
    for (const auto& pair : pImpl->commandHandlers) {
        maxNameLength = std::max(maxNameLength, static_cast<int>(pair.first.length()));
    }
    
    for (const auto& pair : pImpl->commandHandlers) {
        std::cout << "  " << std::left << std::setw(maxNameLength + 4) << pair.first
                  << " - " << pair.second << "\n";
    }
}

void InteractiveMode::setStatus(const std::string& status) {
    pImpl->status = status;
}

void InteractiveMode::displayStatus() const {
    if (!pImpl->brain) return;
    
    std::cout << "\n=== Brain Status ===\n";
    pImpl->brain->logStatus();
    
    if (!pImpl->status.empty()) {
        std::cout << "\nCurrent Status: " << pImpl->status << "\n";
    }
    
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->sessionStartTime);
    
    std::cout << "\nSession Info:\n";
    std::cout << "  Uptime: " << elapsed.count() << " seconds\n";
    std::cout << "  Neurons: " << pImpl->brain->getTotalNeuronCount() << "\n";
    std::cout << "  Synapses: " << pImpl->brain->getTotalSynapseCount() << "\n";
    std::cout << "  Total Spikes: " << pImpl->brain->getTotalSpikeCount() << "\n";
    std::cout << "=================\n";
}

void InteractiveMode::showHelp() const {
    std::cout << "\n=== NLM Interactive Mode Help ===\n\n";
    
    std::cout << "General Commands:\n";
    std::cout << "  help                    - Show this help message\n";
    std::cout << "  quit                    - Exit interactive mode\n";
    std::cout << "  clear                   - Clear the screen\n";
    std::cout << "  history                 - Show command history\n";
    std::cout << "  status                  - Show brain status\n";
    std::cout << "  reset                   - Reset brain to initial state\n\n";
    
    std::cout << "Simulation Commands:\n";
    std::cout << "  step [n]                - Advance simulation by n steps (default: 1)\n";
    std::cout << "  inject [count] [type] [current] - Inject current into neurons\n";
    std::cout << "    count: Number of neurons to inject into (default: 1)\n";
    std::cout << "    type: 'excitatory' or 'inhibitory' (default: excitatory)\n";
    std::cout << "    current: Current amount in nA (default: 50)\n\n";
    
    std::cout << "Save/Load Commands:\n";
    std::cout << "  save <filename>         - Save brain state to file\n";
    std::cout << "  load <filename>         - Load brain state from file\n\n";
    
    std::cout << "Plasticity Commands:\n";
    std::cout << "  plasticity on/off       - Enable or disable plasticity\n\n";
    
    std::cout << "Configuration Commands:\n";
    std::cout << "  set <key> <value>       - Set configuration value\n";
    std::cout << "  get <key>               - Get configuration value\n";
    std::cout << "  config <file>           - Load configuration from file\n\n";
    
    std::cout << "Other Commands:\n";
    std::cout << "  clear history           - Clear command history\n";
    std::cout << "  enable progress         - Enable progress display\n";
    std::cout << "  disable progress        - Disable progress display\n\n";
    
    std::cout << "Press 'Tab' for command completion.\n";
    std::cout << "==============================\n\n";
}

void InteractiveMode::showHistory() const {
    std::cout << "\n=== Command History ===\n";
    
    int count = 1;
    for (const auto& entry : pImpl->commandHistory) {
        std::cout << count << ". " << entry << "\n";
        count++;
    }
    
    std::cout << "=====================\n\n";
}

void InteractiveMode::clearScreen() const {
    system("cls");
    displayLogo();
}

void InteractiveMode::displayLogo() const {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     Interactive Mode                                          ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void InteractiveMode::runCommand(const std::string& commandLine) {
    std::stringstream ss(commandLine);
    std::string token;
    std::vector<std::string> tokens;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.empty()) return;
    
    std::string command = tokens[0];
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    
    auto it = pImpl->commandHandlers.find(command);
    if (it != pImpl->commandHandlers.end()) {
        try {
            it->second(args);
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    } else {
        std::cout << "Unknown command: " << command << ". Type 'help' for available commands." << std::endl;
    }
}

std::string InteractiveMode::readLine(const std::string& prompt) const {
    std::cout << prompt;
    std::cout.flush();
    
    if (pImpl->settings.useRawInput) {
        std::string line;
        
        // Set terminal to raw mode for Linux/macOS
        struct termios oldSettings;
        struct termios newSettings;
        
        tcgetattr(STDIN_FILENO, &oldSettings);
        newSettings = oldSettings;
        
        // Disable canonical mode and echo
        newSettings.c_lflag &= ~(ICANON | ECHO);
        newSettings.c_cc[VMIN] = 1;
        newSettings.c_cc[VTIME] = 0;
        
        tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
        
        char ch;
        while (true) {
            if (read(STDIN_FILENO, &ch, 1) < 0) {
                // Error reading from stdin
                break;
            }
            
            if (ch == 10 || ch == 13) { // Enter key (Unix: \n, Windows: \r)
                std::cout << "\r\n"; // Return to beginning of line
                break;
            } else if (ch == 8) { // Backspace
                if (!line.empty()) {
                    std::cout << "\b \b";
                    line.pop_back();
                }
            } else if (ch == 9 && pImpl->settings.enableTabCompletion) { // Tab
                // Tab completion
                std::vector<std::string> completions = getTabCompletions(line);
                if (!completions.empty()) {
                    std::string completion = completions[0];
                    std::cout << completion.substr(line.length()) << " ";
                    line = completion;
                }
            } else if (ch >= 32 && ch <= 126) { // Printable characters
                std::cout << ch;
                line += ch;
            }
        }
        
        // Restore terminal settings
        tcsetattr(STDIN_FILENO, TCSANOW, &oldSettings);
        
        return line;
    } else {
        std::string line;
        std::getline(std::cin, line);
        return line;
    }
}

void InteractiveMode::enableProgressDisplay(bool enable) {
    pImpl->progressEnabled = enable;
    if (enable) {
        ProgressIndicator::Options options;
        options.style = ProgressIndicator::Style::Bar;
        options.showPercentage = true;
        options.showElapsed = true;
        options.showRemaining = true;
        pImpl->progressIndicator = ProgressIndicator(options);
    }
}

void InteractiveMode::updateProgress(double progress, const std::string& status) {
    if (pImpl->progressEnabled) {
        pImpl->progressIndicator.update(progress, status);
    }
}

void InteractiveMode::finishProgress(const std::string& message) {
    if (pImpl->progressEnabled) {
        pImpl->progressIndicator.finish(message);
        pImpl->progressEnabled = false;
    }
}

void InteractiveMode::setBrain(std::shared_ptr<Brain> brain) {
    pImpl->brain = brain;
}

std::shared_ptr<Brain> InteractiveMode::getBrain() const {
    return pImpl->brain;
}

void InteractiveMode::saveSession(const std::string& filename) const {
    // Save interactive session state
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "# NLM Interactive Session\n";
        file << "# Saved at: " << std::chrono::system_clock::now() << "\n\n";
        
        for (const auto& entry : pImpl->commandHistory) {
            file << entry << "\n";
        }
        
        file << "\n# End of session\n";
        file.close();
    }
}

bool InteractiveMode::loadSession(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    
    std::string line;
    bool readingCommands = false;
    
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        
        if (line.find("# ") == 0) {
            readingCommands = false;
            continue;
        }
        
        if (readingCommands || line.find("# ") != 0) {
            if (line.find("# ") != 0) {
                pImpl->commandHistory.push_back(line);
            }
            readingCommands = true;
        }
    }
    
    file.close();
    return true;
}

void InteractiveMode::setConfigFile(const std::string& configFile) {
    pImpl->settings.configFile = configFile;
    
    auto config = std::make_shared<Config>();
    if (config->loadFromFile(configFile)) {
        std::cout << "Configuration loaded from " << configFile << std::endl;
    } else {
        std::cout << "Failed to load configuration from " << configFile << std::endl;
    }
}

} // namespace nlm