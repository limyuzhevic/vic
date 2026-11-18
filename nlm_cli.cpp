// NLM Command Line Interface
// Provides advanced command-line tools for NLM configuration and control

#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/agent/AgentBrain.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace nlm;

class NLMCommandLine {
public:
    enum class Command {
        HELP,
        CREATE_CONFIG,
        RUN_BRAIN,
        CHECKPOINT_SAVE,
        CHECKPOINT_LOAD,
        LIST_CHECKPOINTS,
        SET_PARAM,
        GET_PARAM,
        RUN_DEMO,
        VALIDATE_CONFIG,
        EXPORT_CONFIG,
        IMPORT_CONFIG
    };

    struct CommandArgs {
        Command command;
        std::string configFile;
        std::string checkpointPath;
        std::string outputFile;
        std::map<std::string, std::string> parameters;
        std::vector<std::string> extraArgs;
    };

    NLMCommandLine() = default;
    ~NLMCommandLine() = default;

    bool parseArguments(int argc, char* argv[]) {
        if (argc < 2) {
            printHelp();
            return false;
        }

        std::string commandStr = argv[1];
        
        if (commandStr == "help" || commandStr == "--help" || commandStr == "-h") {
            args_.command = Command::HELP;
        } else if (commandStr == "create-config") {
            args_.command = Command::CREATE_CONFIG;
            parseConfigArgs(argc, argv);
        } else if (commandStr == "run-brain") {
            args_.command = Command::RUN_BRAIN;
            parseRunBrainArgs(argc, argv);
        } else if (commandStr == "checkpoint-save") {
            args_.command = Command::CHECKPOINT_SAVE;
            parseCheckpointArgs(argc, argv);
        } else if (commandStr == "checkpoint-load") {
            args_.command = Command::CHECKPOINT_LOAD;
            parseCheckpointArgs(argc, argv);
        } else if (commandStr == "list-checkpoints") {
            args_.command = Command::LIST_CHECKPOINTS;
            parseCheckpointArgs(argc, argv);
        } else if (commandStr == "set-param") {
            args_.command = Command::SET_PARAM;
            parseParamArgs(argc, argv);
        } else if (commandStr == "get-param") {
            args_.command = Command::GET_PARAM;
            parseParamArgs(argc, argv);
        } else if (commandStr == "run-demo") {
            args_.command = Command::RUN_DEMO;
            parseDemoArgs(argc, argv);
        } else if (commandStr == "validate-config") {
            args_.command = Command::VALIDATE_CONFIG;
            parseConfigArgs(argc, argv);
        } else if (commandStr == "export-config") {
            args_.command = Command::EXPORT_CONFIG;
            parseConfigArgs(argc, argv);
        } else if (commandStr == "import-config") {
            args_.command = Command::IMPORT_CONFIG;
            parseConfigArgs(argc, argv);
        } else {
            std::cerr << "Unknown command: " << commandStr << std::endl;
            printHelp();
            return false;
        }

        return true;
    }

    bool execute() {
        switch (args_.command) {
            case Command::HELP:
                printHelp();
                break;
            case Command::CREATE_CONFIG:
                return createConfigFile();
            case Command::RUN_BRAIN:
                return runBrainSimulation();
            case Command::CHECKPOINT_SAVE:
                return saveCheckpoint();
            case Command::CHECKPOINT_LOAD:
                return loadCheckpoint();
            case Command::LIST_CHECKPOINTS:
                return listCheckpoints();
            case Command::SET_PARAM:
                return setParameter();
            case Command::GET_PARAM:
                return getParameter();
            case Command::RUN_DEMO:
                return runDemo();
            case Command::VALIDATE_CONFIG:
                return validateConfig();
            case Command::EXPORT_CONFIG:
                return exportConfig();
            case Command::IMPORT_CONFIG:
                return importConfig();
            default:
                std::cerr << "Error: Command not implemented" << std::endl;
                return false;
        }
        return true;
    }

private:
    void printHelp() {
        std::cout << "=== NLM Command Line Interface ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  nlm-cli <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  help                    Show this help message" << std::endl;
        std::cout << "  create-config <file>    Create a new configuration file" << std::endl;
        std::cout << "  run-brain <config> [steps]  Run brain simulation" << std::endl;
        std::cout << "  checkpoint-save <path> [name]  Save checkpoint" << std::endl;
        std::cout << "  checkpoint-load <path> [name]  Load checkpoint" << std::endl;
        std::cout << "  list-checkpoints <dir>   List available checkpoints" << std::endl;
        std::cout << "  set-param <param> <value> Set configuration parameter" << std::endl;
        std::cout << "  get-param <param>        Get configuration parameter" << std::endl;
        std::cout << "  run-demo [type]          Run built-in demo" << std::endl;
        std::cout << "  validate-config <file>   Validate configuration file" << std::endl;
        std::cout << "  export-config <file> <format>  Export configuration" << std::endl;
        std::cout << "  import-config <file> <format>  Import configuration" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm-cli create-config my_config.json" << std::endl;
        std::cout << "  nlm-cli run-brain config.json 1000" << std::endl;
        std::cout << "  nlm-cli checkpoint-save ./checkpoints brain_state" << std::endl;
        std::cout << "  nlm-cli get-param neuron_count" << std::endl;
    }

    void parseConfigArgs(int argc, char* argv[]) {
        if (argc >= 3) {
            args_.configFile = argv[2];
        }
        if (argc >= 4) {
            args_.outputFile = argv[3];
        }
        for (int i = 4; i < argc; ++i) {
            if (argv[i][0] == '-') {
                std::string param(argv[i] + 1);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    args_.parameters[param] = argv[++i];
                }
            }
        }
    }

    void parseRunBrainArgs(int argc, char* argv[]) {
        if (argc >= 3) {
            args_.configFile = argv[2];
        }
        if (argc >= 4) {
            args_.extraArgs.push_back(argv[3]);
        }
    }

    void parseCheckpointArgs(int argc, char* argv[]) {
        if (argc >= 3) {
            args_.checkpointPath = argv[2];
        }
        if (argc >= 4) {
            args_.extraArgs.push_back(argv[3]);
        }
    }

    void parseParamArgs(int argc, char* argv[]) {
        if (argc >= 3) {
            std::string param(argv[2]);
            if (argc >= 4) {
                args_.parameters[param] = argv[3];
            }
        }
    }

    void parseDemoArgs(int argc, char* argv[]) {
        if (argc >= 3) {
            args_.extraArgs.push_back(argv[2]);
        }
    }

    bool createConfigFile() {
        std::cout << "Creating configuration file..." << std::endl;
        
        auto config = std::make_shared<Config>();
        
        // Apply default values for Phase 6
        config->set("neuron_count", static_cast<int64_t>(1000));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        config->set("enable_plasticity", true);
        config->set("enable_development", true);
        config->set("enable_curiosity", true);
        config->set("enable_novelty", true);
        config->set("checkpoint_dir", "./checkpoints");
        
        // Override with user parameters
        for (const auto& param : args_.parameters) {
            setConfigParameter(config, param.first, param.second);
        }
        
        if (!args_.outputFile.empty()) {
            bool saved = config->saveToFile(args_.outputFile);
            if (saved) {
                std::cout << "Configuration saved to: " << args_.outputFile << std::endl;
            } else {
                std::cerr << "Error: Failed to save configuration" << std::endl;
                return false;
            }
        } else {
            std::cout << "Configuration:" << std::endl;
            std::cout << config->summary() << std::endl;
        }
        
        return true;
    }

    bool runBrainSimulation() {
        std::cout << "Running brain simulation..." << std::endl;
        
        auto config = std::make_shared<Config>();
        
        if (args_.configFile.empty() || !config->loadFromFile(args_.configFile)) {
            std::cerr << "Error: Could not load configuration file" << std::endl;
            return false;
        }
        
        // Parse simulation steps from extra args
        uint64_t steps = 1000;
        if (!args_.extraArgs.empty()) {
            try {
                steps = std::stoull(args_.extraArgs[0]);
            } catch (...) {
                std::cerr << "Warning: Invalid step count, using default (1000)" << std::endl;
            }
        }
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Run simulation
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (uint64_t step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << "/" << steps 
                         << " | Spikes: " << brain->getTotalSpikeCount()
                         << " | Firing: " << brain->getFiringNeuronCount()
                         << " | Reward: Not available" << std::endl;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << std::endl;
        std::cout << "Simulation completed in " << duration.count() / 1000.0f << " seconds" << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Final firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "Neurons firing: " << brain->getFiringNeuronCount() << std::endl;
        
        return true;
    }

    bool saveCheckpoint() {
        std::cout << "Saving checkpoint..." << std::endl;
        
        auto config = std::make_shared<Config>();
        if (args_.configFile.empty() || !config->loadFromFile(args_.configFile)) {
            std::cerr << "Error: Could not load configuration file" << std::endl;
            return false;
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Run a few steps to generate some activity
        for (uint64_t step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        std::string checkpointPath = args_.checkpointPath;
        if (!args_.extraArgs.empty()) {
            checkpointPath += "/" + args_.extraArgs[0];
        }
        
        bool saved = brain->save(checkpointPath);
        if (saved) {
            std::cout << "Checkpoint saved to: " << checkpointPath << std::endl;
        } else {
            std::cerr << "Error: Failed to save checkpoint" << std::endl;
            return false;
        }
        
        return true;
    }

    bool loadCheckpoint() {
        std::cout << "Loading checkpoint..." << std::endl;
        
        auto config = std::make_shared<Config>();
        if (args_.configFile.empty() || !config->loadFromFile(args_.configFile)) {
            std::cerr << "Error: Could not load configuration file" << std::endl;
            return false;
        }
        
        std::string checkpointPath = args_.checkpointPath;
        if (!args_.extraArgs.empty()) {
            checkpointPath += "/" + args_.extraArgs[0];
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        bool loaded = brain->load(checkpointPath);
        if (loaded) {
            std::cout << "Checkpoint loaded from: " << checkpointPath << std::endl;
            std::cout << "Loaded spikes: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "Loaded neurons: " << brain->getTotalNeuronCount() << std::endl;
        } else {
            std::cerr << "Error: Failed to load checkpoint" << std::endl;
            return false;
        }
        
        return true;
    }

    bool listCheckpoints() {
        std::cout << "Listing checkpoints in: " << args_.checkpointPath << std::endl;
        
        // Note: This is a simplified implementation
        // The real implementation would use CheckpointManager
        std::cout << "Note: Full checkpoint listing requires CheckpointManager integration" << std::endl;
        
        return true;
    }

    bool setParameter() {
        auto config = std::make_shared<Config>();
        if (!args_.configFile.empty()) {
            config->loadFromFile(args_.configFile);
        }
        
        for (const auto& param : args_.parameters) {
            if (setConfigParameter(config, param.first, param.second)) {
                std::cout << "Parameter " << param.first << " set to " << param.second << std::endl;
            } else {
                std::cerr << "Error: Failed to set parameter " << param.first << std::endl;
            }
        }
        
        return true;
    }

    bool getParameter() {
        auto config = std::make_shared<Config>();
        if (!args_.configFile.empty()) {
            config->loadFromFile(args_.configFile);
        }
        
        for (const auto& param : args_.parameters) {
            if (config->has(param.first)) {
                std::cout << param.first << " = " << config->getOr<std::string>(param.first, "") << std::endl;
            } else {
                std::cerr << "Error: Parameter " << param.first << " not found" << std::endl;
            }
        }
        
        return true;
    }

    bool runDemo() {
        std::cout << "Running built-in demo..." << std::endl;
        
        std::string demoType = args_.extraArgs.empty() ? "phase6" : args_.extraArgs[0];
        
        if (demoType == "phase2") {
            std::cout << "Phase 2 demo (basic neural computation)" << std::endl;
            // Would run the nlm executable
        } else if (demoType == "phase6") {
            std::cout << "Phase 6 demo (integration test)" << std::endl;
            // Would run the nlm_phase6_demo executable
        } else {
            std::cout << "Available demos: phase2, phase6" << std::endl;
        }
        
        return true;
    }

    bool validateConfig() {
        std::cout << "Validating configuration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        if (args_.configFile.empty() || !config->loadFromFile(args_.configFile)) {
            std::cerr << "Error: Could not load configuration file" << std::endl;
            return false;
        }
        
        std::cout << "Configuration summary:" << std::endl;
        std::cout << config->summary() << std::endl;
        
        // Basic validation
        bool valid = true;
        if (config->getOr<int64_t>("neuron_count", 0) <= 0) {
            std::cerr << "Error: Invalid neuron count" << std::endl;
            valid = false;
        }
        
        if (config->getOr<double>("simulation_timestep", 0.0) <= 0.0) {
            std::cerr << "Error: Invalid simulation timestep" << std::endl;
            valid = false;
        }
        
        if (valid) {
            std::cout << "Configuration is valid" << std::endl;
        } else {
            std::cout << "Configuration has errors" << std::endl;
        }
        
        return valid;
    }

    bool exportConfig() {
        std::cout << "Exporting configuration..." << std::endl;
        
        auto config = std::make_shared<Config>();
        if (args_.configFile.empty() || !config->loadFromFile(args_.configFile)) {
            std::cerr << "Error: Could not load configuration file" << std::endl;
            return false;
        }
        
        if (args_.extraArgs.empty()) {
            std::cerr << "Error: Output format not specified" << std::endl;
            return false;
        }
        
        std::string format = args_.extraArgs[0];
        if (format == "json") {
            // JSON export
            std::ofstream file(args_.outputFile);
            if (file.is_open()) {
                file << config->summary();
                file.close();
                std::cout << "Configuration exported to JSON: " << args_.outputFile << std::endl;
            } else {
                std::cerr << "Error: Could not create output file" << std::endl;
                return false;
            }
        } else {
            std::cerr << "Error: Unsupported export format: " << format << std::endl;
            return false;
        }
        
        return true;
    }

    bool importConfig() {
        std::cout << "Importing configuration..." << std::endl;
        
        if (args_.outputFile.empty()) {
            std::cerr << "Error: Output file not specified" << std::endl;
            return false;
        }
        
        // Note: This is a simplified implementation
        // The real implementation would parse the input file format
        std::cout << "Note: Full import functionality requires additional parsing logic" << std::endl;
        
        return true;
    }

    bool setConfigParameter(std::shared_ptr<Config> config, const std::string& paramName, const std::string& paramValue) {
        try {
            if (paramName == "neuron_count" || paramName == "region_count") {
                config->set(paramName, static_cast<int64_t>(std::stoll(paramValue)));
            } else if (paramName == "connection_probability" || 
                      paramName == "stdp_ltp_weight" || 
                      paramName == "stdp_ltd_weight" || 
                      paramName == "synaptogenesis_rate" || 
                      paramName == "pruning_rate") {
                config->set(paramName, static_cast<float>(std::stof(paramValue)));
            } else if (paramName == "random_seed") {
                config->set(paramName, static_cast<int64_t>(std::stoll(paramValue)));
            } else if (paramName == "simulation_timestep") {
                config->set(paramName, std::stod(paramValue));
            } else if (paramName == "enable_plasticity" || 
                      paramName == "enable_development" || 
                      paramName == "enable_curiosity" || 
                      paramName == "enable_novelty") {
                config->set(paramName, paramValue == "true" || paramValue == "1" || paramValue == "yes");
            } else {
                return false;
            }
            return true;
        } catch (...) {
            return false;
        }
    }

    std::string getConfigParameter(std::shared_ptr<Config> config, const std::string& paramName) {
        if (!config->has(paramName)) {
            return "";
        }
        
        if (paramName == "neuron_count" || paramName == "region_count") {
            return std::to_string(config->getOr<int64_t>(paramName, 0));
        } else if (paramName == "connection_probability" || 
                  paramName == "stdp_ltp_weight" || 
                  paramName == "stdp_ltd_weight" || 
                  paramName == "synaptogenesis_rate" || 
                  paramName == "pruning_rate") {
            return std::to_string(config->getOr<float>(paramName, 0.0f));
        } else if (paramName == "random_seed") {
            return std::to_string(config->getOr<int64_t>(paramName, 0));
        } else if (paramName == "simulation_timestep") {
            return std::to_string(config->getOr<double>(paramName, 0.0));
        } else if (paramName == "enable_plasticity" || 
                  paramName == "enable_development" || 
                  paramName == "enable_curiosity" || 
                  paramName == "enable_novelty") {
            return config->getOr<bool>(paramName, false) ? "true" : "false";
        } else {
            return "";
        }
    }
};

int main(int argc, char* argv[]) {
    NLMCommandLine cli;
    
    if (!cli.parseArguments(argc, argv)) {
        return 1;
    }
    
    if (!cli.execute()) {
        return 1;
    }
    
    return 0;
}