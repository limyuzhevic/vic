#include "AdvancedCommands.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <ctime>

namespace nlm {

AdvancedCommands::AdvancedCommands(std::shared_ptr<Brain> brain)
    : brain_(brain)
    , verbose_(false)
    , sessionStart_(std::chrono::steady_clock::now()) {
    
    // Register built-in commands
    registerCommand("save", std::bind(&AdvancedCommands::handleSaveCommand, this, std::placeholders::_1));
    registerCommand("load", std::bind(&AdvancedCommands::handleLoadCommand, this, std::placeholders::_1));
    registerCommand("reset", std::bind(&AdvancedCommands::handleResetCommand, this, std::placeholders::_1));
    registerCommand("debug", std::bind(&AdvancedCommands::handleDebugCommand, this, std::placeholders::_1));
    registerCommand("profile", std::bind(&AdvancedCommands::handleProfileCommand, this, std::placeholders::_1));
    registerCommand("perf", std::bind(&AdvancedCommands::handlePerfCommand, this, std::placeholders::_1));
    registerCommand("stats", std::bind(&AdvancedCommands::handleStatsCommand, this, std::placeholders::_1));
    registerCommand("checkpoint", std::bind(&AdvancedCommands::handleCheckpointCommand, this, std::placeholders::_1));
    registerCommand("experiment", std::bind(&AdvancedCommands::handleExperimentCommand, this, std::placeholders::_1));
    registerCommand("config", std::bind(&AdvancedCommands::handleConfigCommand, this, std::placeholders::_1));
    registerCommand("exit", std::bind(&AdvancedCommands::handleExitCommand, this, std::placeholders::_1));
    registerCommand("help", std::bind(&AdvancedCommands::handleHelpCommand, this, std::placeholders::_1));
    registerCommand("quit", std::bind(&AdvancedCommands::handleExitCommand, this, std::placeholders::_1));
    
    NLM_LOG_INFO("AdvancedCommands initialized with " + std::to_string(commands_.size()) + " commands");
}

AdvancedCommands::~AdvancedCommands() {
    NLM_LOG_INFO("AdvancedCommands shutdown completed");
}

bool AdvancedCommands::executeCommand(const std::string& commandLine) {
    if (commandLine.empty() || commandLine.find_first_not_of(' ') == std::string::npos) {
        return true; // Empty command or just whitespace
    }
    
    // Add to history
    commandHistory_.push_back(commandLine);
    
    // Parse command
    std::vector<std::string> tokens = parseCommandLine(commandLine);
    if (tokens.empty()) {
        return true;
    }
    
    std::string commandName = tokens[0];
    std::transform(commandName.begin(), commandName.end(), commandName.begin(), ::tolower);
    
    // Check if command exists
    auto it = commands_.find(commandName);
    if (it == commands_.end()) {
        std::cerr << "Unknown command: " << commandName << std::endl;
        if (verbose_) {
            std::cerr << "Type 'help' for available commands" << std::endl;
        }
        return false;
    }
    
    // Execute command (pass remaining arguments excluding command name)
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    bool success = it->second(args);
    
    if (verbose_) {
        std::cout << "[" << getTimestamp() << "] Command '" << commandName << "' " 
                  << (success ? "succeeded" : "failed") << std::endl;
    }
    
    return success;
}

void AdvancedCommands::registerCommand(const std::string& name, 
                                     std::function<bool(const std::vector<std::string>&)> handler) {
    std::string normalizedName = name;
    std::transform(normalizedName.begin(), normalizedName.end(), normalizedName.begin(), ::tolower);
    commands_[normalizedName] = handler;
    
    if (verbose_) {
        std::cout << "Registered command: " << name << std::endl;
    }
}

void AdvancedCommands::printHelp() const {
    std::cout << "=== NLM Advanced Commands ===" << std::endl << std::endl;
    
    std::cout << "Available commands:" << std::endl;
    for (const auto& pair : commands_) {
        std::cout << "  " << std::left << std::setw(15) << pair.first << " - ";
        
        // Provide brief descriptions for common commands
        if (pair.first == "save") {
            std::cout << "Save brain state to file" << std::endl;
        } else if (pair.first == "load") {
            std::cout << "Load brain state from file" << std::endl;
        } else if (pair.first == "reset") {
            std::cout << "Reset brain (with options)" << std::endl;
        } else if (pair.first == "debug") {
            std::cout << "Debug brain state and connections" << std::endl;
        } else if (pair.first == "profile") {
            std::cout << "Profile performance and metrics" << std::endl;
        } else if (pair.first == "perf") {
            std::cout << "Monitor performance in real-time" << std::endl;
        } else if (pair.first == "stats") {
            std::cout << "Show brain statistics" << std::endl;
        } else if (pair.first == "checkpoint") {
            std::cout << "Manage checkpoints (list, create, delete)" << std::endl;
        } else if (pair.first == "experiment") {
            std::cout << "Run experiment with parameters" << std::endl;
        } else if (pair.first == "config") {
            std::cout << "Manage configuration" << std::endl;
        } else if (pair.first == "help") {
            std::cout << "Show this help message" << std::endl;
        } else if (pair.first == "exit" || pair.first == "quit") {
            std::cout << "Exit the application" << std::endl;
        } else {
            std::cout << "Unknown command" << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Usage examples:" << std::endl;
    std::cout << "  save my_brain.bin" << std::endl;
    std::cout << "  load backup.bin" << std::endl;
    std::cout << "  reset --type full --neurons 1000" << std::endl;
    std::cout << "  debug --verbose --neurons 5" << std::endl;
    std::cout << "  stats --format json" << std::endl;
    std::cout << "  checkpoint list" << std::endl;
    std::cout << "  perf --start --monitor 1000" << std::endl;
}

std::vector<std::string> AdvancedCommands::parseCommandLine(const std::string& commandLine) {
    std::vector<std::string> tokens;
    std::stringstream ss(commandLine);
    std::string token;
    
    while (ss >> token) {
        // Remove quotes and handle escaped characters
        if (token.front() == '"' && token.back() == '"') {
            token = token.substr(1, token.size() - 2);
        }
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string AdvancedCommands::getCurrentWorkingDirectory() const {
    return std::filesystem::current_path().string();
}

std::string AdvancedCommands::getTimestamp() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - sessionStart_);
    return std::to_string(duration.count()) + "s";
}

std::string AdvancedCommands::formatBytes(uint64_t bytes) const {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double value = static_cast<double>(bytes);
    
    while (value >= 1024.0 && unitIndex < 4) {
        value /= 1024.0;
        unitIndex++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << value << " " << units[unitIndex];
    return ss.str();
}

std::string AdvancedCommands::formatTime(double seconds) const {
    if (seconds < 60.0) {
        return std::to_string(seconds) + "s";
    } else if (seconds < 3600.0) {
        int minutes = static_cast<int>(seconds / 60.0);
        int secs = static_cast<int>(seconds) % 60;
        return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    } else {
        int hours = static_cast<int>(seconds / 3600.0);
        int minutes = static_cast<int>((seconds - hours * 3600.0) / 60.0);
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
    }
}

std::string AdvancedCommands::formatFloat(float value, int precision) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

// Command implementations
bool AdvancedCommands::handleSaveCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: Save command requires filename argument" << std::endl;
        return false;
    }
    
    std::string filename = args[0];
    
    // Add timestamp if no extension
    if (filename.find('.') == std::string::npos) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        filename = "checkpoint_" + ss.str() + ".bin";
    }
    
    if (verbose_) {
        std::cout << "Saving brain to: " << filename << std::endl;
    }
    
    if (brain_->save(filename)) {
        std::cout << "Brain saved successfully to: " << filename << std::endl;
        return true;
    } else {
        std::cerr << "Error: Failed to save brain to: " << filename << std::endl;
        return false;
    }
}

bool AdvancedCommands::handleLoadCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: Load command requires filename argument" << std::endl;
        return false;
    }
    
    std::string filename = args[0];
    
    if (verbose_) {
        std::cout << "Loading brain from: " << filename << std::endl;
    }
    
    // First reset current brain
    if (verbose_) {
        std::cout << "Resetting current brain..." << std::endl;
    }
    
    if (!brain_->reset()) {
        std::cerr << "Error: Failed to reset brain before loading" << std::endl;
        return false;
    }
    
    // Re-initialize with default config
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto newBrain = std::make_shared<Brain>(config);
    if (!newBrain->initialize()) {
        std::cerr << "Error: Failed to initialize new brain" << std::endl;
        return false;
    }
    
    // Replace current brain
    brain_ = newBrain;
    
    // Try to load from file
    if (brain_->load(filename)) {
        std::cout << "Brain loaded successfully from: " << filename << std::endl;
        return true;
    } else {
        std::cerr << "Error: Failed to load brain from: " << filename << std::endl;
        return false;
    }
}

bool AdvancedCommands::handleResetCommand(const std::vector<std::string>& args) {
    std::string resetType = "soft"; // soft, hard, neurons, synapses
    int neuronCount = 500;
    
    // Parse arguments
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--type" && i + 1 < args.size()) {
            resetType = args[i + 1];
            i++;
        } else if (args[i] == "--neurons" && i + 1 < args.size()) {
            try {
                neuronCount = std::stoi(args[i + 1]);
                i++;
            } catch (const std::exception& e) {
                std::cerr << "Warning: Invalid neuron count, using default" << std::endl;
            }
        }
    }
    
    if (verbose_) {
        std::cout << "Resetting brain with type: " << resetType << std::endl;
    }
    
    bool success = true;
    
    if (resetType == "hard") {
        // For hard reset, we need to recreate the brain completely
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        brain_ = std::make_shared<Brain>(config);
        if (!brain_->initialize()) {
            std::cerr << "Error: Failed to re-initialize brain" << std::endl;
            success = false;
        }
    } else if (resetType == "soft") {
        if (!brain_->reset()) {
            std::cerr << "Error: Failed to reset brain (soft reset)" << std::endl;
            success = false;
        }
        
        // Re-initialize with current config
        if (!brain_->initialize()) {
            std::cerr << "Error: Failed to re-initialize brain" << std::endl;
            success = false;
        }
    } else if (resetType == "neurons") {
        // Just reset neurons, keep synapses
        if (!brain_->reset()) {
            std::cerr << "Error: Failed to reset brain neurons" << std::endl;
            success = false;
        }
    }
    
    if (success) {
        std::cout << "Brain reset completed (type: " << resetType << ")" << std::endl;
    }
    
    return success;
}

bool AdvancedCommands::handleDebugCommand(const std::vector<std::string>& args) {
    std::cout << "=== NLM Brain Debug Information ===" << std::endl;
    
    if (!brain_) {
        std::cout << "Brain not initialized" << std::endl;
        return false;
    }
    
    brain_->logStatus();
    
    // Additional debug info if verbose
    if (verbose_) {
        std::cout << std::endl;
        std::cout << "Debug details:" << std::endl;
        
        // Memory regions
        std::cout << "Regions: " << brain_->getRegionCount() << std::endl;
        std::cout << "Total neurons: " << brain_->getTotalNeuronCount() << std::endl;
        std::cout << "Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
        std::cout << "Active neurons: " << brain_->getFiringNeuronCount() << std::endl;
        std::cout << "Average firing rate: " << formatFloat(brain_->getAverageFiringRate()) << std::endl;
        
        // Working memory
        auto* wm = brain_->getWorkingMemory();
        if (wm) {
            std::cout << "Working memory active traces: " << wm->getActiveTraces() << std::endl;
            std::cout << "Working memory capacity: " << wm->getCapacity() << std::endl;
        } else {
            std::cout << "Working memory: Not available" << std::endl;
        }
        
        // Episodic memory
        auto* em = brain_->getEpisodicMemory();
        if (em) {
            std::cout << "Episodic memory episodes: " << em->getEpisodeCount() << std::endl;
        } else {
            std::cout << "Episodic memory: Not available" << std::endl;
        }
        
        // Neuromodulation
        auto* dopamine = brain_->getDopamine();
        if (dopamine) {
            std::cout << "Dopamine system: Available" << std::endl;
        } else {
            std::cout << "Dopamine system: Not available" << std::endl;
        }
        
        // Prediction system
        auto* prediction = brain_->getPredictionSystem();
        if (prediction) {
            std::cout << "Prediction system: Available" << std::endl;
        } else {
            std::cout << "Prediction system: Not available" << std::endl;
        }
        
        // Development system
        auto* development = brain_->getDevelopmentSystem();
        if (development) {
            std::cout << "Development system: Available" << std::endl;
        } else {
            std::cout << "Development system: Not available" << std::endl;
        }
    }
    
    return true;
}

bool AdvancedCommands::handleProfileCommand(const std::vector<std::string>& args) {
    std::cout << "=== NLM Performance Profile ===" << std::endl;
    
    if (!brain_) {
        std::cout << "Brain not initialized" << std::endl;
        return false;
    }
    
    // Simple profiling based on available metrics
    std::cout << "Current brain metrics:" << std::endl;
    std::cout << "  - Total neurons: " << brain_->getTotalNeuronCount() << std::endl;
    std::cout << "  - Total synapses: " << brain_->getTotalSynapseCount() << std::endl;
    std::cout << "  - Active neurons: " << brain_->getFiringNeuronCount() << std::endl;
    std::cout << "  - Average firing rate: " << formatFloat(brain_->getAverageFiringRate()) << std::endl;
    std::cout << "  - Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
    
    // Check for additional profiling capabilities
    if (verbose_) {
        std::cout << std::endl;
        std::cout << "Additional profiling information (verbose mode):" << std::endl;
        
        // This would integrate with actual profiling tools if available
        std::cout << "  - Performance monitoring: Not implemented in this build" << std::endl;
        std::cout << "  - Memory usage: Requires external profiling tool" << std::endl;
        std::cout << "  - CPU profiling: Not available in current build" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Note: For comprehensive profiling, consider using external tools like:
";
    std::cout << "  - Linux perf
";
    std::cout << "  - Valgrind
";
    std::cout << "  - GNU prof
";
    std::cout << "  - Custom performance instrumentation" << std::endl;
    
    return true;
}

bool AdvancedCommands::handlePerfCommand(const std::vector<std::string>& args) {
    std::cout << "=== NLM Performance Monitor ===" << std::endl;
    
    if (!brain_) {
        std::cout << "Brain not initialized" << std::endl;
        return false;
    }
    
    bool startMonitoring = false;
    uint64_t duration = 0;
    
    // Parse arguments
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--start") {
            startMonitoring = true;
        } else if (args[i] == "--duration" && i + 1 < args.size()) {
            try {
                duration = std::stoull(args[i + 1]);
                i++;
            } catch (const std::exception& e) {
                std::cerr << "Warning: Invalid duration, using default" << std::endl;
            }
        }
    }
    
    if (startMonitoring) {
        std::cout << "Starting performance monitoring for " << duration << " steps..." << std::endl;
        // In a real implementation, this would start a background monitoring thread
        std::cout << "Monitoring started (simplified monitoring in this build)" << std::endl;
    } else {
        std::cout << "To start monitoring, use: perf --start [--duration <steps>]" << std::endl;
        std::cout << "Current metrics snapshot:" << std::endl;
        std::cout << "  - Neurons: " << brain_->getTotalNeuronCount() << std::endl;
        std::cout << "  - Synapses: " << brain_->getTotalSynapseCount() << std::endl;
        std::cout << "  - Firing neurons: " << brain_->getFiringNeuronCount() << std::endl;
        std::cout << "  - Average firing rate: " << formatFloat(brain_->getAverageFiringRate()) << std::endl;
    }
    
    return true;
}

bool AdvancedCommands::handleStatsCommand(const std::vector<std::string>& args) {
    std::cout << "=== NLM Brain Statistics ===" << std::endl;
    
    if (!brain_) {
        std::cout << "Brain not initialized" << std::endl;
        return false;
    }
    
    brain_->logStatus();
    
    // Additional statistics based on available metrics
    if (verbose_) {
        std::cout << std::endl;
        std::cout << "Extended statistics:" << std::endl;
        
        // Memory usage approximation (this would need actual memory tracking)
        std::cout << "  - Memory usage: Approx. " << formatBytes(sizeof(Brain) + brain_->getTotalNeuronCount() * 100) << std::endl;
        
        // Performance metrics
        double uptime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - sessionStart_).count();
        std::cout << "  - Session uptime: " << formatTime(uptime) << std::endl;
        
        // Command count
        std::cout << "  - Commands executed: " << commandHistory_.size() << std::endl;
        
        // Memory system statistics
        auto* wm = brain_->getWorkingMemory();
        if (wm) {
            std::cout << "  - Working memory utilization: " << wm->getActiveTraces() << "/" << wm->getCapacity() << std::endl;
        }
    }
    
    return true;
}

bool AdvancedCommands::handleCheckpointCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: checkpoint <command> [options]" << std::endl;
        std::cout << "Commands: list, create, delete, info" << std::endl;
        return true;
    }
    
    std::string command = args[0];
    
    if (command == "list") {
        std::cout << "Available checkpoints:" << std::endl;
        // This would integrate with actual checkpoint management
        std::cout << "  (Checkpoint listing requires CheckpointManager integration)" << std::endl;
        return true;
    } else if (command == "create") {
        if (args.size() < 2) {
            std::cerr << "Error: Checkpoint name required" << std::endl;
            return false;
        }
        
        std::string checkpointName = args[1];
        std::cout << "Creating checkpoint: " << checkpointName << std::endl;
        // This would integrate with actual checkpoint creation
        std::cout << "Checkpoint creation requires CheckpointManager integration" << std::endl;
        return true;
    } else if (command == "delete") {
        if (args.size() < 2) {
            std::cerr << "Error: Checkpoint name required" << std::endl;
            return false;
        }
        
        std::string checkpointName = args[1];
        std::cout << "Deleting checkpoint: " << checkpointName << std::endl;
        // This would integrate with actual checkpoint deletion
        std::cout << "Checkpoint deletion requires CheckpointManager integration" << std::endl;
        return true;
    } else if (command == "info") {
        if (args.size() < 2) {
            std::cerr << "Error: Checkpoint name required" << std::endl;
            return false;
        }
        
        std::string checkpointName = args[1];
        std::cout << "Checkpoint info: " << checkpointName << std::endl;
        // This would integrate with actual checkpoint info retrieval
        std::cout << "Checkpoint info requires CheckpointManager integration" << std::endl;
        return true;
    } else {
        std::cerr << "Unknown checkpoint command: " << command << std::endl;
        return false;
    }
}

bool AdvancedCommands::handleExperimentCommand(const std::vector<std::string>& args) {
    std::cout << "=== NLM Experiment Control ===" << std::endl;
    
    if (args.empty()) {
        std::cout << "Usage: experiment <command> [options]" << std::endl;
        std::cout << "Commands: run, stop, status, params" << std::endl;
        return true;
    }
    
    std::string command = args[0];
    
    if (command == "run") {
        std::cout << "Starting experiment..." << std::endl;
        // This would integrate with actual experiment running
        std::cout << "Experiment running requires Phase6IntegratedExperiment integration" << std::endl;
        return true;
    } else if (command == "stop") {
        std::cout << "Stopping experiment..." << std::endl;
        std::cout << "Experiment stopping requires integration with experiment manager" << std::endl;
        return true;
    } else if (command == "status") {
        std::cout << "Experiment status:" << std::endl;
        std::cout << "  - Running: Not implemented in this build" << std::endl;
        std::cout << "  - Experiments: Requires integration with experiment framework" << std::endl;
        return true;
    } else if (command == "params") {
        std::cout << "Current experiment parameters:" << std::endl;
        std::cout << "  - Requires integration with experiment configuration system" << std::endl;
        return true;
    } else {
        std::cerr << "Unknown experiment command: " << command << std::endl;
        return false;
    }
}

bool AdvancedCommands::handleConfigCommand(const std::vector<std::string>& args) {
    std::cout << "=== NLM Configuration Management ===" << std::endl;
    
    if (args.empty()) {
        std::cout << "Usage: config <command> [options]" << std::endl;
        std::cout << "Commands: show, set, save, load, reset" << std::endl;
        return true;
    }
    
    std::string command = args[0];
    
    if (command == "show") {
        std::cout << "Current configuration:" << std::endl;
        // Show current brain configuration
        if (brain_) {
            std::cout << "  - Neuron count: " << brain_->getTotalNeuronCount() << std::endl;
            std::cout << "  - Region count: " << brain_->getRegionCount() << std::endl;
            std::cout << "  - Synapse count: " << brain_->getTotalSynapseCount() << std::endl;
        } else {
            std::cout << "  Brain not initialized" << std::endl;
        }
        return true;
    } else if (command == "set") {
        if (args.size() < 3) {
            std::cerr << "Error: Parameter name and value required" << std::endl;
            return false;
        }
        
        std::string paramName = args[1];
        std::string paramValue = args[2];
        std::cout << "Setting configuration parameter: " << paramName << " = " << paramValue << std::endl;
        // This would integrate with actual configuration system
        std::cout << "Parameter setting requires Config class integration" << std::endl;
        return true;
    } else if (command == "save") {
        std::string filename = "config_backup_" + getTimestamp() + ".cfg";
        if (args.size() > 1) {
            filename = args[1];
        }
        std::cout << "Saving configuration to: " << filename << std::endl;
        // This would integrate with actual configuration saving
        std::cout << "Configuration saving requires Config class integration" << std::endl;
        return true;
    } else if (command == "load") {
        if (args.size() < 2) {
            std::cerr << "Error: Configuration filename required" << std::endl;
            return false;
        }
        
        std::string filename = args[1];
        std::cout << "Loading configuration from: " << filename << std::endl;
        // This would integrate with actual configuration loading
        std::cout << "Configuration loading requires Config class integration" << std::endl;
        return true;
    } else if (command == "reset") {
        std::cout << "Resetting configuration to defaults..." << std::endl;
        // This would integrate with actual configuration resetting
        std::cout << "Configuration reset requires Config class integration" << std::endl;
        return true;
    } else {
        std::cerr << "Unknown config command: " << command << std::endl;
        return false;
    }
}

bool AdvancedCommands::handleExitCommand(const std::vector<std::string>& args) {
    std::cout << "Exiting NLM Advanced Commands..." << std::endl;
    return true; // This will signal the main loop to exit
}

bool AdvancedCommands::handleHelpCommand(const std::vector<std::string>& args) {
    printHelp();
    return true;
}

} // namespace nlm
