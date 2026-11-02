#include "core/CommandLineInterface.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <cmath>

namespace nlm {

bool CommandLineInterface::execute(const std::string& commandLine) {
    // Add to history
    m_commandHistory.push_back(commandLine);
    
    // Tokenize command
    auto tokens = tokenize(commandLine);
    if (tokens.empty()) {
        return true;
    }
    
    std::string command = tokens[0];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);
    
    // Check if command exists
    if (!hasCommand(command)) {
        std::cerr << "Error: Unknown command '" << command << "'" << std::endl;
        std::cerr << "Type 'help' for available commands" << std::endl;
        return false;
    }
    
    // Execute command
    try {
        m_commands[command](tokens);
        logCommand("EXECUTED: " + commandLine);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error executing command '" << command << "': " << e.what() << std::endl;
        return false;
    }
}

void CommandLineInterface::printHelp() const {
    std::cout << "\n=== NLM Command Line Interface Help ===\n" << std::endl;
    std::cout << "General Commands:" << std::endl;
    std::cout << "  help                    - Show this help message" << std::endl;
    std::cout << "  exit                    - Exit the NLM interface" << std::endl;
    std::cout << "  brain                   - Brain system commands" << std::endl;
    std::cout << "  config                  - Configuration commands" << std::endl;
    std::cout << "  experiment              - Run experiments" << std::endl;
    std::cout << "  save [file]             - Save brain state to file" << std::endl;
    std::cout << "  load [file]             - Load brain state from file" << std::endl;
    std::cout << "  run [steps]             - Run simulation for specified steps" << std::endl;
    std::cout << "  status                  - Show brain system status" << std::endl;
    std::cout << "  stats                   - Show performance statistics" << std::endl;
    std::cout << "  plot [output]           - Generate visualization plots" << std::endl;
    std::cout << "  benchmark               - Run performance benchmark" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Brain Commands:" << std::endl;
    std::cout << "  brain reset             - Reset brain state" << std::endl;
    std::cout << "  brain step [count]      - Advance brain by specified steps" << std::endl;
    std::cout << "  brain inject [type] [amount] - Inject current into neuron type" << std::endl;
    std::cout << "  brain enable [type]     - Enable specific brain system" << std::endl;
    std::cout << "  brain disable [type]    - Disable specific brain system" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Config Commands:" << std::endl;
    std::cout << "  config get <key>        - Get configuration value" << std::endl;
    std::cout << "  config set <key> <value> - Set configuration value" << std::endl;
    std::cout << "  config list             - List all configuration keys" << std::endl;
    std::cout << "  config save [file]      - Save config to file" << std::endl;
    std::cout << "  config load [file]      - Load config from file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Experiment Commands:" << std::endl;
    std::cout << "  experiment list         - List available experiments" << std::endl;
    std::cout << "  experiment run <name> [steps] - Run specific experiment" << std::endl;
    std::cout << std::endl;
}

void CommandLineInterface::printBrainStatus() const {
    if (!m_brain) {
        std::cout << "No brain instance available" << std::endl;
        return;
    }
    
    std::cout << "\n=== Brain Status ===\n" << std::endl;
    m_brain->logStatus();
    
    std::cout << "\nNeural Statistics:" << std::endl;
    std::cout << "  Total Neurons: " << m_brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total Synapses: " << m_brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Active Neurons: " << m_brain->getActiveNeuronCount() << std::endl;
    std::cout << "  Firing Neurons: " << m_brain->getFiringNeuronCount() << std::endl;
    std::cout << "  Total Spikes: " << m_brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Pending Spikes: " << m_brain->getPendingSpikeEventCount() << std::endl;
    std::cout << "  Excitation/Inhibition Ratio: " << m_brain->getExcitationInhibitionRatio() << std::endl;
    std::cout << "  Average Firing Rate: " << m_brain->getAverageFiringRate() << std::endl;
}

void CommandLineInterface::printConfig() const {
    if (!m_config) {
        std::cout << "No configuration available" << std::endl;
        return;
    }
    
    std::cout << "\n=== Configuration ===\n" << std::endl;
    auto keys = m_config->getKeys();
    for (const auto& key : keys) {
        auto value = m_config->getOr<std::string>(key, "");
        if (!value.has_value()) {
            std::cout << "  " << key << ": <unknown>" << std::endl;
        } else {
            std::cout << "  " << key << ": " << value.value() << std::endl;
        }
    }
}

bool CommandLineInterface::saveBrain(const std::string& filepath) const {
    if (!m_brain) {
        std::cerr << "Error: No brain instance available" << std::endl;
        return false;
    }
    
    std::cout << "Saving brain state to: " << filepath << std::endl;
    return m_brain->save(filepath);
}

bool CommandLineInterface::loadBrain(const std::string& filepath) {
    if (!m_brain) {
        std::cerr << "Error: No brain instance available" << std::endl;
        return false;
    }
    
    std::cout << "Loading brain state from: " << filepath << std::endl;
    return m_brain->load(filepath);
}

bool CommandLineInterface::runExperiment(const std::string& experimentName, int steps) {
    if (!m_experimentRunner) {
        std::cerr << "Error: No experiment runner available" << std::endl;
        return false;
    }
    
    std::cout << "Running experiment: " << experimentName << " for " << steps << " steps" << std::endl;
    return m_experimentRunner->run(experimentName, steps);
}

void CommandLineInterface::registerCommands() {
    // General commands
    m_commands["help"] = [this](const std::vector<std::string>& args) { cmdHelp(args); };
    m_commands["exit"] = [this](const std::vector<std::string>& args) { cmdExit(args); };
    
    // Brain commands
    m_commands["brain"] = [this](const std::vector<std::string>& args) { cmdBrain(args); };
    
    // Config commands
    m_commands["config"] = [this](const std::vector<std::string>& args) { cmdConfig(args); };
    
    // Experiment commands
    m_commands["experiment"] = [this](const std::vector<std::string>& args) { cmdExperiment(args); };
    
    // File commands
    m_commands["save"] = [this](const std::vector<std::string>& args) { cmdSave(args); };
    m_commands["load"] = [this](const std::vector<std::string>& args) { cmdLoad(args); };
    
    // Execution commands
    m_commands["run"] = [this](const std::vector<std::string>& args) { cmdRun(args); };
    m_commands["status"] = [this](const std::vector<std::string>& args) { cmdStatus(args); };
    m_commands["stats"] = [this](const std::vector<std::string>& args) { cmdStats(args); };
    m_commands["plot"] = [this](const std::vector<std::string>& args) { cmdPlot(args); };
    m_commands["benchmark"] = [this](const std::vector<std::string>& args) { cmdBenchmark(args); };
}

std::vector<std::string> CommandLineInterface::tokenize(const std::string& commandLine) {
    std::vector<std::string> tokens;
    std::stringstream ss(commandLine);
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

void CommandLineInterface::logCommand(const std::string& command) const {
    NLM_LOG_INFO("CLI Command: " + command);
}

bool CommandLineInterface::hasCommand(const std::string& command) const {
    return m_commands.find(command) != m_commands.end();
}

void CommandLineInterface::cmdHelp(const std::vector<std::string>& args) {
    printHelp();
}

void CommandLineInterface::cmdExit(const std::vector<std::string>& args) {
    std::cout << "Exiting NLM... Goodbye!" << std::endl;
    // Note: This won't actually exit the main program, but can be used
    // by the REPL system to indicate exit
    exit(0);
}

void CommandLineInterface::cmdBrain(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Brain commands:" << std::endl;
        std::cout << "  reset        - Reset brain state" << std::endl;
        std::cout << "  step [count]  - Advance brain by specified steps" << std::endl;
        std::cout << "  inject [type] [amount] - Inject current into neuron type" << std::endl;
        std::cout << "  enable [type]  - Enable specific brain system" << std::endl;
        std::cout << "  disable [type] - Disable specific brain system" << std::endl;
        std::cout << "  status        - Show brain status" << std::endl;
        return;
    }
    
    std::string subcommand = args[1];
    std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(), ::tolower);
    
    if (subcommand == "reset") {
        if (m_brain) {
            m_brain->reset();
            std::cout << "Brain state reset" << std::endl;
        } else {
            std::cout << "Error: No brain instance available" << std::endl;
        }
    } else if (subcommand == "step") {
        if (m_brain) {
            uint64_t steps = 1;
            if (args.size() > 2) {
                steps = std::stoul(args[2]);
            }
            std::cout << "Advancing brain by " << steps << " steps..." << std::endl;
            // TODO: Implement brain stepping
            std::cout << "(Brain stepping not yet implemented)" << std::endl;
        }
    } else if (subcommand == "inject") {
        if (m_brain && args.size() >= 3) {
            std::string neuronTypeStr = args[2];
            NeuronType neuronType;
            if (neuronTypeStr == "sensory") {
                neuronType = NeuronType::Sensory;
            } else if (neuronTypeStr == "motor") {
                neuronType = NeuronType::Motor;
            } else if (neuronTypeStr == "interneuron") {
                neuronType = NeuronType::Interneuron;
            } else {
                std::cout << "Unknown neuron type: " << neuronTypeStr << std::endl;
                return;
            }
            
            double current = 1.0;
            if (args.size() > 3) {
                current = std::stod(args[3]);
            }
            
            m_brain->injectCurrentToNeurons(neuronType, current);
            std::cout << "Injected " << current << " current into " << neuronTypeStr << " neurons" << std::endl;
        }
    } else if (subcommand == "enable" || subcommand == "disable") {
        if (args.size() >= 3) {
            std::cout << "(Enable/disable brain systems not yet implemented)" << std::endl;
        }
    } else if (subcommand == "status") {
        printBrainStatus();
    }
}

void CommandLineInterface::cmdConfig(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Config commands:" << std::endl;
        std::cout << "  get <key>        - Get configuration value" << std::endl;
        std::cout << "  set <key> <value> - Set configuration value" << std::endl;
        std::cout << "  list             - List all configuration keys" << std::endl;
        std::cout << "  save [file]      - Save config to file" << std::endl;
        std::cout << "  load [file]      - Load config from file" << std::endl;
        std::cout << "  print            - Show current configuration" << std::endl;
        return;
    }
    
    std::string subcommand = args[1];
    std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(), ::tolower);
    
    if (subcommand == "get") {
        if (args.size() < 3) {
            std::cout << "Error: Missing key for get command" << std::endl;
            return;
        }
        
        std::string key = args[2];
        auto value = m_config->getOr<std::string>(key, "");
        if (value.has_value()) {
            std::cout << key << " = " << value.value() << std::endl;
        } else {
            std::cout << key << " = <not found>" << std::endl;
        }
    } else if (subcommand == "set") {
        if (args.size() < 4) {
            std::cout << "Error: Missing key or value for set command" << std::endl;
            return;
        }
        
        std::string key = args[2];
        std::string value = args[3];
        
        // Try to parse numeric values
        try {
            if (value.find('.') != std::string::npos) {
                double doubleVal = std::stod(value);
                m_config->set(key, doubleVal);
            } else if (value == "true" || value == "false") {
                bool boolVal = (value == "true");
                m_config->set(key, boolVal);
            } else {
                int64_t intVal = std::stoll(value);
                m_config->set(key, intVal);
            }
        } catch (...) {
            m_config->set(key, value);
        }
        
        std::cout << "Set " << key << " = " << value << std::endl;
    } else if (subcommand == "list") {
        auto keys = m_config->getKeys();
        std::cout << "Configuration keys:" << std::endl;
        for (const auto& key : keys) {
            std::cout << "  " << key << std::endl;
        }
    } else if (subcommand == "save") {
        std::string filename = "config_" + std::to_string(time(nullptr)) + ".cfg";
        if (args.size() >= 3) {
            filename = args[2];
        }
        
        if (m_config->saveToFile(filename)) {
            std::cout << "Configuration saved to: " << filename << std::endl;
        } else {
            std::cout << "Failed to save configuration" << std::endl;
        }
    } else if (subcommand == "load") {
        std::string filename;
        if (args.size() >= 3) {
            filename = args[2];
        } else {
            std::cout << "Error: Missing filename for load command" << std::endl;
            return;
        }
        
        if (m_config->loadFromFile(filename)) {
            std::cout << "Configuration loaded from: " << filename << std::endl;
        } else {
            std::cout << "Failed to load configuration from: " << filename << std::endl;
        }
    } else if (subcommand == "print") {
        printConfig();
    }
}

void CommandLineInterface::cmdExperiment(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Experiment commands:" << std::endl;
        std::cout << "  list             - List available experiments" << std::endl;
        std::cout << "  run <name> [steps] - Run specific experiment" << std::endl;
        return;
    }
    
    std::string subcommand = args[1];
    std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(), ::tolower);
    
    if (subcommand == "list") {
        if (m_experimentRunner) {
            auto experiments = m_experimentRunner->getAvailableExperiments();
            std::cout << "Available experiments:" << std::endl;
            for (const auto& exp : experiments) {
                std::cout << "  " << exp << std::endl;
            }
        } else {
            std::cout << "No experiment runner available" << std::endl;
        }
    } else if (subcommand == "run") {
        if (args.size() < 3) {
            std::cout << "Error: Missing experiment name for run command" << std::endl;
            return;
        }
        
        std::string experimentName = args[2];
        int steps = 1000;
        if (args.size() >= 4) {
            steps = std::stoi(args[3]);
        }
        
        runExperiment(experimentName, steps);
    }
}

void CommandLineInterface::cmdSave(const std::vector<std::string>& args) {
    std::string filename = "brain_checkpoint_" + std::to_string(time(nullptr)) + ".bin";
    if (args.size() >= 2) {
        filename = args[1];
    }
    
    saveBrain(filename);
}

void CommandLineInterface::cmdLoad(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: load <filename>" << std::endl;
        return;
    }
    
    std::string filename = args[1];
    loadBrain(filename);
}

void CommandLineInterface::cmdRun(const std::vector<std::string>& args) {
    uint64_t steps = 1000;
    if (args.size() >= 2) {
        try {
            steps = std::stoul(args[1]);
        } catch (...) {
            std::cout << "Error: Invalid step count" << std::endl;
            return;
        }
    }
    
    std::cout << "Running simulation for " << steps << " steps..." << std::endl;
    
    if (m_brain) {
        for (uint64_t step = 0; step < steps; ++step) {
            m_brain->step(step, step * 0.001);
            
            // Log progress every 10%
            if (step % (steps / 10) == 0 && step > 0) {
                std::cout << "Progress: " << (step * 100 / steps) << "% (step " << step << "/" << steps << ")" << std::endl;
            }
        }
        
        std::cout << "Simulation completed" << std::endl;
    } else {
        std::cout << "Error: No brain instance available" << std::endl;
    }
}

void CommandLineInterface::cmdStatus(const std::vector<std::string>& args) {
    printBrainStatus();
}

void CommandLineInterface::cmdStats(const std::vector<std::string>& args) {
    std::cout << "\n=== Performance Statistics ===\n" << std::endl;
    
    if (m_brain) {
        std::cout << "Neural Statistics:" << std::endl;
        std::cout << "  Total Spikes: " << m_brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Active Neurons: " << m_brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing Neurons: " << m_brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Pending Spikes: " << m_brain->getPendingSpikeEventCount() << std::endl;
        std::cout << "  Average Firing Rate: " << m_brain->getAverageFiringRate() << std::endl;
        std::cout << "  Excitation/Inhibition Ratio: " << m_brain->getExcitationInhibitionRatio() << std::endl;
    }
    
    std::cout << "\nCommand History:" << std::endl;
    for (const auto& cmd : m_commandHistory) {
        std::cout << "  " << cmd << std::endl;
    }
}

void CommandLineInterface::cmdPlot(const std::vector<std::string>& args) {
    std::string outputFile = "brain_activity_" + std::to_string(time(nullptr)) + ".png";
    if (args.size() >= 2) {
        outputFile = args[1];
    }
    
    std::cout << "Generating neural activity plot..." << std::endl;
    std::cout << "Plot saved to: " << outputFile << std::endl;
    std::cout << "(Plot generation not yet implemented)" << std::endl;
}

void CommandLineInterface::cmdBenchmark(const std::vector<std::string>& args) {
    std::cout << "Running NLM benchmark..." << std::endl;
    std::cout << "(Benchmark not yet implemented)" << std::endl;
}

} // namespace nlm