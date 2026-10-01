#include "CLIFactory.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <fstream>
#include <sstream>

namespace nlm {

// CommandHandler implementation
class DefaultCommandHandler : public CommandHandler {
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    
public:
    DefaultCommandHandler(std::shared_ptr<Config> cfg, std::shared_ptr<Brain> brn = nullptr)
        : config(cfg), brain(brn) {}
    
    CommandResult execute(const std::string& commandName,
                         const std::map<std::string, std::string>& options,
                         std::shared_ptr<Config> config) override {
        // Find command by name
        auto commands = getAvailableCommands();
        std::shared_ptr<Command> selectedCommand = nullptr;
        
        for (auto& cmd : commands) {
            if (cmd->getName() == commandName) {
                selectedCommand = cmd;
                break;
            }
        }
        
        if (!selectedCommand) {
            return CommandResult(CommandStatus::InvalidArguments, "Unknown command: " + commandName, config);
        }
        
        // Validate options
        auto cmdOptions = selectedCommand->getOptions();
        CommandLineParser parser;
        CommandResult validationResult = parser.validateOptions(commandName, options, cmdOptions);
        if (validationResult.status != CommandStatus::Success) {
            return validationResult;
        }
        
        // Execute command
        bool success = selectedCommand->execute(options, config);
        if (success) {
            return CommandResult(CommandStatus::Success, "Command executed successfully", config);
        } else {
            return CommandResult(CommandStatus::Failed, "Command execution failed", config);
        }
    }
    
    std::vector<std::shared_ptr<Command>> getAvailableCommands() const override {
        return CLIFactory::createDefaultCommands();
    }
    
    void setBrain(std::shared_ptr<Brain> brn) override {
        brain = brn;
    }
};

// Configuration command option definitions
std::vector<CommandLineOption> ConfigFileCommand::getOptions() const {
    return {
        {"--config-file", 'f', OptionType::String, "Load configuration from file", "", false, true},
        {"--help", 'h', OptionType::Flag, "Show help for this command", "", false, false}
    };
}

bool ConfigFileCommand::execute(const std::map<std::string, std::string>& options,
                               std::shared_ptr<Config> config) {
    auto it = options.find("--config-file");
    if (it == options.end()) {
        return false;
    }
    
    try {
        if (config->loadFromFile(it->second)) {
            NLM_LOG_INFO("Loaded configuration from file: " + it->second);
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load configuration from file: " + std::string(e.what()));
    }
    return false;
}

std::vector<CommandLineOption> SetConfigCommand::getOptions() const {
    return {
        {"--set", 's', OptionType::Value, "Set configuration key-value pair (key=value)", "", false, true},
        {"--help", 'h', OptionType::Flag, "Show help for this command", "", false, false}
    };
}

bool SetConfigCommand::execute(const std::map<std::string, std::string>& options,
                              std::shared_ptr<Config> config) {
    auto it = options.find("--set");
    if (it == options.end()) {
        return false;
    }
    
    std::string pair = it->second;
    size_t pos = pair.find('=');
    if (pos == std::string::npos) {
        NLM_LOG_ERROR("Invalid configuration format: " + pair + ". Use key=value format");
        return false;
    }
    
    std::string key = pair.substr(0, pos);
    std::string valueStr = pair.substr(pos + 1);
    
    try {
        // Try to determine value type
        if (valueStr == "true" || valueStr == "false") {
            bool value = (valueStr == "true");
            config->set(key, value, ConfigSource::Runtime);
        } else if (valueStr.find('.') != std::string::npos) {
            try {
                double value = std::stod(valueStr);
                config->set(key, value, ConfigSource::Runtime);
            } catch (...) {
                config->set(key, valueStr, ConfigSource::Runtime);
            }
        } else {
            try {
                int64_t value = std::stoll(valueStr);
                config->set(key, value, ConfigSource::Runtime);
            } catch (...) {
                config->set(key, valueStr, ConfigSource::Runtime);
            }
        }
        
        NLM_LOG_INFO("Set configuration " + key + " = " + valueStr);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to set configuration " + key + ": " + std::string(e.what()));
        return false;
    }
}

// Brain control command option definitions
std::vector<CommandLineOption> SaveBrainCommand::getOptions() const {
    return {
        {"--filepath", 'f', OptionType::String, "Output file path for brain state", "brain_state.json", false, true},
        {"--format", 'F', OptionType::String, "Output format (json)", "json", false, true}
    };
}

bool SaveBrainCommand::execute(const std::map<std::string, std::string>& options,
                              std::shared_ptr<Config> config) {
    if (!brain) {
        NLM_LOG_ERROR("No brain available for saving");
        return false;
    }
    
    std::string filepath = "brain_state.json";
    auto it = options.find("--filepath");
    if (it != options.end()) {
        filepath = it->second;
    }
    
    try {
        if (brain->save(filepath)) {
            NLM_LOG_INFO("Brain state saved to: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to save brain state to: " + filepath);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving brain state: " + std::string(e.what()));
        return false;
    }
}

std::vector<CommandLineOption> LoadBrainCommand::getOptions() const {
    return {
        {"--filepath", 'f', OptionType::String, "Input file path for brain state", "brain_state.json", false, true},
        {"--format", 'F', OptionType::String, "Input format (json)", "json", false, true}
    };
}

bool LoadBrainCommand::execute(const std::map<std::string, std::string>& options,
                              std::shared_ptr<Config> config) {
    if (!brain) {
        NLM_LOG_ERROR("No brain available for loading");
        return false;
    }
    
    std::string filepath = "brain_state.json";
    auto it = options.find("--filepath");
    if (it != options.end()) {
        filepath = it->second;
    }
    
    try {
        if (brain->load(filepath)) {
            NLM_LOG_INFO("Brain state loaded from: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to load brain state from: " + filepath);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading brain state: " + std::string(e.what()));
        return false;
    }
}

std::vector<CommandLineOption> ResetBrainCommand::getOptions() const {
    return {
        {"--preserve-config", 'p', OptionType::Flag, "Preserve configuration while resetting brain state", "", false, false}
    };
}

bool ResetBrainCommand::execute(const std::map<std::string, std::string>& options,
                               std::shared_ptr<Config> config) {
    if (!brain) {
        NLM_LOG_ERROR("No brain available for resetting");
        return false;
    }
    
    try {
        brain->reset();
        if (options.find("--preserve-config") == options.end()) {
            brain->initialize();
        }
        NLM_LOG_INFO("Brain reset successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error resetting brain: " + std::string(e.what()));
        return false;
    }
}

std::vector<CommandLineOption> CheckpointBrainCommand::getOptions() const {
    return {
        {"--filepath", 'f', OptionType::String, "Output file path for checkpoint", "checkpoint.json", false, true},
        {"--description", 'd', OptionType::String, "Checkpoint description", "", false, true}
    };
}

bool CheckpointBrainCommand::execute(const std::map<std::string, std::string>& options,
                                    std::shared_ptr<Config> config) {
    if (!brain) {
        NLM_LOG_ERROR("No brain available for checkpointing");
        return false;
    }
    
    std::string filepath = "checkpoint.json";
    auto it = options.find("--filepath");
    if (it != options.end()) {
        filepath = it->second;
    }
    
    try {
        if (brain->save(filepath)) {
            NLM_LOG_INFO("Checkpoint saved to: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to save checkpoint to: " + filepath);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error creating checkpoint: " + std::string(e.what()));
        return false;
    }
}

// Simulation control command option definitions
std::vector<CommandLineOption> RunSimulationCommand::getOptions() const {
    return {
        {"--steps", 'n', OptionType::Integer, "Number of simulation steps to run", "100", false, true},
        {"--timestep", 't', OptionType::Double, "Time step duration", "0.001", false, true},
        {"--duration", 'd', OptionType::Double, "Simulation duration in seconds", "", false, true},
        {"--async", 'a', OptionType::Flag, "Run asynchronously", "", false, false}
    };
}

bool RunSimulationCommand::execute(const std::map<std::string, std::string>& options,
                                  std::shared_ptr<Config> config) {
    if (!brain) {
        NLM_LOG_ERROR("No brain available for simulation");
        return false;
    }
    
    try {
        int steps = 100;
        auto it = options.find("--steps");
        if (it != options.end()) {
            steps = std::stoi(it->second);
        }
        
        double timestep = 0.001;
        it = options.find("--timestep");
        if (it != options.end()) {
            timestep = std::stod(it->second);
        }
        
        // Update config with new values
        config->set("simulation_steps", static_cast<int64_t>(steps), ConfigSource::Runtime);
        config->set("simulation_timestep", timestep, ConfigSource::Runtime);
        
        NLM_LOG_INFO("Running simulation: " + std::to_string(steps) + " steps with timestep " + std::to_string(timestep) + "s");
        
        // Run simulation steps
        for (int step = 0; step < steps; ++step) {
            brain->step(step, timestep);
            
            if (options.find("--async") != options.end()) {
                // Async execution would be handled differently
            }
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + "/" + std::to_string(steps));
            }
        }
        
        NLM_LOG_INFO("Simulation completed successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error running simulation: " + std::string(e.what()));
        return false;
    }
}

std::vector<CommandLineOption> SetStepsCommand::getOptions() const {
    return {
        {"--steps", 'n', OptionType::Integer, "Number of simulation steps", "100", true, true}
    };
}

bool SetStepsCommand::execute(const std::map<std::string, std::string>& options,
                             std::shared_ptr<Config> config) {
    auto it = options.find("--steps");
    if (it == options.end()) {
        return false;
    }
    
    try {
        int steps = std::stoi(it->second);
        config->set("simulation_steps", static_cast<int64_t>(steps), ConfigSource::Runtime);
        NLM_LOG_INFO("Set simulation steps: " + std::to_string(steps));
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting simulation steps: " + std::string(e.what()));
        return false;
    }
}

std::vector<CommandLineOption> SetTimestepCommand::getOptions() const {
    return {
        {"--timestep", 't', OptionType::Double, "Simulation time step", "0.001", true, true}
    };
}

bool SetTimestepCommand::execute(const std::map<std::string, std::string>& options,
                                std::shared_ptr<Config> config) {
    auto it = options.find("--timestep");
    if (it == options.end()) {
        return false;
    }
    
    try {
        double timestep = std::stod(it->second);
        config->set("simulation_timestep", timestep, ConfigSource::Runtime);
        NLM_LOG_INFO("Set simulation timestep: " + std::to_string(timestep) + "s");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting simulation timestep: " + std::string(e.what()));
        return false;
    }
}

std::vector<CommandLineOption> SetDurationCommand::getOptions() const {
    return {
        {"--duration", 'd', OptionType::Double, "Simulation duration in seconds", "", true, true}
    };
}

bool SetDurationCommand::execute(const std::map<std::string, std::string>& options,
                                std::shared_ptr<Config> config) {
    auto it = options.find("--duration");
    if (it == options.end()) {
        return false;
    }
    
    try {
        double duration = std::stod(it->second);
        config->set("simulation_duration", duration, ConfigSource::Runtime);
        NLM_LOG_INFO("Set simulation duration: " + std::to_string(duration) + "s");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting simulation duration: " + std::string(e.what()));
        return false;
    }
}

// Visualization command option definitions
std::vector<CommandLineOption> PlotStatsCommand::getOptions() const {
    return {
        {"--output", 'o', OptionType::String, "Output file for plot", "stats.png", false, true},
        {"--format", 'F', OptionType::String, "Plot format (png, svg, pdf)", "png", false, true}
    };
}

bool PlotStatsCommand::execute(const std::map<std::string, std::string>& options,
                              std::shared_ptr<Config> config) {
    std::string output = "stats.png";
    auto it = options.find("--output");
    if (it != options.end()) {
        output = it->second;
    }
    
    NLM_LOG_INFO("Plot statistics would be generated and saved to: " + output);
    // In a real implementation, this would generate actual plots
    return true;
}

std::vector<CommandLineOption> ExportStatsCommand::getOptions() const {
    return {
        {"--filepath", 'f', OptionType::String, "Output file for statistics", "stats.json", false, true},
        {"--format", 'F', OptionType::String, "Export format (json, csv, txt)", "json", false, true}
    };
}

bool ExportStatsCommand::execute(const std::map<std::string, std::string>& options,
                                std::shared_ptr<Config> config) {
    std::string filepath = "stats.json";
    auto it = options.find("--filepath");
    if (it != options.end()) {
        filepath = it->second;
    }
    
    try {
        if (config->saveToFile(filepath)) {
            NLM_LOG_INFO("Statistics exported to: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to export statistics to: " + filepath);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting statistics: " + std::string(e.what()));
        return false;
    }
}

std::vector<CommandLineOption> DebugCommand::getOptions() const {
    return {
        {"--verbose", 'v', OptionType::Flag, "Enable verbose output", "", false, false},
        {"--profile", 'p', OptionType::Flag, "Enable profiling", "", false, false},
        {"--log-file", 'l', OptionType::String, "Log output file", "", false, true}
    };
}

bool DebugCommand::execute(const std::map<std::string, std::string>& options,
                          std::shared_ptr<Config> config) {
    if (options.find("--verbose") != options.end()) {
        NLM_LOG_INFO("Verbose mode enabled");
    }
    
    if (options.find("--profile") != options.end()) {
        NLM_LOG_INFO("Profiling mode enabled");
    }
    
    auto it = options.find("--log-file");
    if (it != options.end() && !it->second.empty()) {
        NLM_LOG_INFO("Logging to file: " + it->second);
        // In a real implementation, this would redirect logging
    }
    
    NLM_LOG_INFO("Debug mode enabled");
    return true;
}

// Advanced command option definitions
std::vector<CommandLineOption> ProfileCommand::getOptions() const {
    return {
        {"--output", 'o', OptionType::String, "Profile output file", "profile.json", false, true},
        {"--format", 'F', OptionType::String, "Profile format", "json", false, true},
        {"--stats", 's', OptionType::Flag, "Include statistics in profile", "", false, false}
    };
}

bool ProfileCommand::execute(const std::map<std::string, std::string>& options,
                            std::shared_ptr<Config> config) {
    std::string output = "profile.json";
    auto it = options.find("--output");
    if (it != options.end()) {
        output = it->second;
    }
    
    NLM_LOG_INFO("Profiling would be performed and saved to: " + output);
    // In a real implementation, this would run actual profiling
    return true;
}

std::vector<CommandLineOption> BenchmarkCommand::getOptions() const {
    return {
        {"--output", 'o', OptionType::String, "Benchmark output file", "benchmark.json", false, true},
        {"--duration", 'd', OptionType::Double, "Benchmark duration in seconds", "60.0", false, true},
        {"--warmup", 'w', OptionType::Double, "Warmup duration in seconds", "10.0", false, true}
    };
}

bool BenchmarkCommand::execute(const std::map<std::string, std::string>& options,
                              std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Benchmark would be performed");
    // In a real implementation, this would run actual benchmarks
    return true;
}

std::vector<CommandLineOption> ValidateCommand::getOptions() const {
    return {
        {"--config", 'c', OptionType::String, "Configuration file to validate", "", false, true},
        {"--brain", 'b', OptionType::String, "Brain file to validate", "", false, true},
        {"--strict", 's', OptionType::Flag, "Strict validation mode", "", false, false}
    };
}

bool ValidateCommand::execute(const std::map<std::string, std::string>& options,
                             std::shared_ptr<Config> config) {
    bool valid = true;
    
    auto it = options.find("--config");
    if (it != options.end() && !it->second.empty()) {
        try {
            auto testConfig = std::make_shared<Config>();
            if (!testConfig->loadFromFile(it->second)) {
                NLM_LOG_ERROR("Configuration validation failed: " + it->second);
                valid = false;
            } else {
                NLM_LOG_INFO("Configuration validation passed: " + it->second);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Configuration validation error: " + std::string(e.what()));
            valid = false;
        }
    }
    
    it = options.find("--brain");
    if (it != options.end() && !it->second.empty()) {
        // In a real implementation, this would validate brain files
        NLM_LOG_INFO("Brain validation not yet implemented for: " + it->second);
    }
    
    return valid;
}

std::vector<CommandLineOption> BatchCommand::getOptions() const {
    return {
        {"--file", 'f', OptionType::String, "Batch file containing simulations", "", true, true},
        {"--parallel", 'p', OptionType::Integer, "Number of parallel simulations", "1", false, true},
        {"--output-dir", 'o', OptionType::String, "Output directory for batch results", ".", false, true}
    };
}

bool BatchCommand::execute(const std::map<std::string, std::string>& options,
                          std::shared_ptr<Config> config) {
    auto it = options.find("--file");
    if (it == options.end()) {
        NLM_LOG_ERROR("Batch file is required for batch processing");
        return false;
    }
    
    std::string batchFile = it->second;
    NLM_LOG_INFO("Batch processing from file: " + batchFile);
    
    // In a real implementation, this would read the batch file and process each simulation
    // For now, just indicate that batch processing would start
    return true;
}

// Static factory methods
std::vector<std::shared_ptr<Command>> CLIFactory::createDefaultCommands() {
    std::vector<std::shared_ptr<Command>> commands;
    
    commands.insert(commands.end(), createConfigurationCommands().begin(), createConfigurationCommands().end());
    commands.insert(commands.end(), createBrainControlCommands().begin(), createBrainControlCommands().end());
    commands.insert(commands.end(), createSimulationControlCommands().begin(), createSimulationControlCommands().end());
    commands.insert(commands.end(), createVisualizationCommands().begin(), createVisualizationCommands().end());
    commands.insert(commands.end(), createAdvancedCommands().begin(), createAdvancedCommands().end());
    
    return commands;
}

std::shared_ptr<CommandHandler> CLIFactory::createCommandHandler(std::shared_ptr<Config> config,
                                                                std::shared_ptr<Brain> brain) {
    return std::make_shared<DefaultCommandHandler>(config, brain);
}

std::vector<std::shared_ptr<Command>> CLIFactory::createConfigurationCommands() {
    std::vector<std::shared_ptr<Command>> commands;
    
    commands.push_back(std::make_shared<ConfigFileCommand>());
    commands.push_back(std::make_shared<SetConfigCommand>());
    
    return commands;
}

std::vector<std::shared_ptr<Command>> CLIFactory::createBrainControlCommands() {
    std::vector<std::shared_ptr<Command>> commands;
    
    commands.push_back(std::make_shared<SaveBrainCommand>());
    commands.push_back(std::make_shared<LoadBrainCommand>());
    commands.push_back(std::make_shared<ResetBrainCommand>());
    commands.push_back(std::make_shared<CheckpointBrainCommand>());
    
    return commands;
}

std::vector<std::shared_ptr<Command>> CLIFactory::createSimulationControlCommands() {
    std::vector<std::shared_ptr<Command>> commands;
    
    commands.push_back(std::make_shared<RunSimulationCommand>());
    commands.push_back(std::make_shared<SetStepsCommand>());
    commands.push_back(std::make_shared<SetTimestepCommand>());
    commands.push_back(std::make_shared<SetDurationCommand>());
    
    return commands;
}

std::vector<std::shared_ptr<Command>> CLIFactory::createVisualizationCommands() {
    std::vector<std::shared_ptr<Command>> commands;
    
    commands.push_back(std::make_shared<PlotStatsCommand>());
    commands.push_back(std::make_shared<ExportStatsCommand>());
    commands.push_back(std::make_shared<DebugCommand>());
    
    return commands;
}

std::vector<std::shared_ptr<Command>> CLIFactory::createAdvancedCommands() {
    std::vector<std::shared_ptr<Command>> commands;
    
    commands.push_back(std::make_shared<ProfileCommand>());
    commands.push_back(std::make_shared<BenchmarkCommand>());
    commands.push_back(std::make_shared<ValidateCommand>());
    commands.push_back(std::make_shared<BatchCommand>());
    
    return commands;
}

} // namespace nlm
