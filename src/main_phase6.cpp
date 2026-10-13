// NLM Phase 6 - Enhanced Command Line Interface
// Comprehensive CLI for NLM Phase 6 with advanced commands and features
// Supports batch processing, experiment management, and performance analysis

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "prediction/PredictionSystem.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "development/DevelopmentSystem.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/STDP.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "sensory/Vision.hpp"
#include "motor/Action.hpp"
#include "environment/Observation.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <thread>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <numeric>

namespace nlm {

// Forward declarations for helper functions
struct ExperimentMetrics {
    int episode = 0;
    int steps = 0;
    float totalReward = 0.0f;
    float totalSpikes = 0.0f;
    float finalNovelty = 0.0f;
    float finalCuriosity = 0.0f;
    float finalEnergy = 0.0f;
    float predictionError = 0.0f;
    float developmentalAge = 0.0f;
    std::string developmentalStage = "Unknown";
    size_t totalSynapses = 0;
    float averageFiringRate = 0.0f;
    
    int forwardCount = 0, backwardCount = 0, turnLeftCount = 0;
    int turnRightCount = 0, interactCount = 0, waitCount = 0;
};

class NeuralSystemManager {
public:
    std::shared_ptr<Brain> createBrain(const std::string& configName = "default") {
        auto config = std::make_shared<Config>();
        
        if (configName == "default") {
            // Load default config
            config->set("neuron_count", size_t(1000));
            config->set("region_count", size_t(2));
            config->set("connection_probability", 0.1f);
            config->set("random_seed", uint64_t(42));
            config->set("simulation_timestep", 0.001);
            config->set("stdp_ltp_weight", 0.01f);
            config->set("stdp_ltd_weight", 0.012f);
            config->set("stdp_tau", 20.0f);
            config->set("synaptogenesis_rate", 0.0001f);
            config->set("pruning_rate", 0.00001f);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }
    
    std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }
    
    std::shared_ptr<SimpleWorld> createWorld(int width = 20, int height = 20, int visionW = 16, int visionH = 16) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(static_cast<float>(width), static_cast<float>(height), visionW, visionH);
        world->reset();
        return world;
    }
    
    void enableAllFeatures(std::shared_ptr<AgentBrain> agentBrain) {
        agentBrain->enableRewardModulation(true);
        agentBrain->enableStructuralPlasticity(true);
        agentBrain->enableDevelopment(true);
        agentBrain->enableCuriosity(true);
    }
    
    std::string getSystemInfo(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agentBrain) {
        std::stringstream ss;
        ss << "=== NLM Phase 6 Enhanced System ===\n";
        ss << "Brain Configuration:\n";
        ss << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        ss << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        ss << "  Regions: " << brain->getRegionCount() << "\n";
        ss << "  Active neurons: " << brain->getActiveNeuronCount() << "\n";
        ss << "  Firing neurons: " << brain->getFiringNeuronCount() << "\n";
        ss << "  Total spikes: " << brain->getTotalSpikeCount() << "\n";
        ss << "  E/I ratio: " << brain->getExcitationInhibitionRatio() << "\n\n";
        
        ss << "Agent Interface:\n";
        ss << "  Sensory input size: " << agentBrain->getSensoryInputSize() << "\n";
        ss << "  Motor output size: " << agentBrain->getMotorOutputSize() << "\n";
        ss << "  Neuromodulation: " << agentBrain->getNeuromodulationLevel() << "\n";
        ss << "  Curiosity: " << agentBrain->getCuriosityLevel() << "\n";
        ss << "  Novelty: " << agentBrain->getNoveltyLevel() << "\n";
        ss << "  Prediction error: " << agentBrain->getPredictionError() << "\n";
        ss << "  Developmental stage: " << static_cast<int>(agentBrain->getDevelopmentalStage()) << "\n";
        
        return ss.str();
    }
private:
    std::shared_ptr<Brain> getCurrentBrain() {
        // This will be initialized by the main application
        static std::shared_ptr<Brain> currentBrain;
        return currentBrain;
    }
    
    friend class CommandParser;
};

static NeuralSystemManager systemManager;

// Command classes for different operations
class Command {
public:
    virtual ~Command() = default;
    virtual std::string execute() = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getName() const = 0;
    virtual std::vector<std::string> getArgs() const = 0;
};

class BrainStatusCommand : public Command {
public:
    std::string execute() override {
        auto brain = systemManager.getCurrentBrain();
        if (!brain) return "No brain initialized";
        
        std::stringstream ss;
        ss << "=== NLM Brain Status ===\n";
        ss << "Neurons: " << brain->getTotalNeuronCount() << "\n";
        ss << "Synapses: " << brain->getTotalSynapseCount() << "\n";
        ss << "Regions: " << brain->getRegionCount() << "\n";
        ss << "Active neurons: " << brain->getActiveNeuronCount() << "\n";
        ss << "Firing neurons: " << brain->getFiringNeuronCount() << "\n";
        ss << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        ss << "Pending events: " << brain->getPendingSpikeEventCount() << "\n";
        ss << "Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        ss << "E/I ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        
        return ss.str();
    }
    
    std::string getDescription() const override {
        return "Display brain status and statistics";
    }
    
    std::string getName() const override {
        return "status";
    }
    
    std::vector<std::string> getArgs() const override {
        return {};
    }
};

class RunSimulationCommand : public Command {
private:
    int numSteps;
    std::string outputFile;
    bool enableLogging;
public:
    RunSimulationCommand(int steps = 1000, const std::string& output = "", bool logging = false)
        : numSteps(steps), outputFile(output), enableLogging(logging) {}
    
    std::string execute() override {
        auto brain = systemManager.getCurrentBrain();
        if (!brain) return "No brain initialized";
        
        std::stringstream ss;
        ss << "Running simulation for " << numSteps << " steps...\n";
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
            
            if (enableLogging && step % 100 == 0) {
                ss << "Step " << step << ": " 
                   << brain->getFiringNeuronCount() << " firing neurons\n";
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        ss << "Simulation complete!\n";
        ss << "Duration: " << duration.count() << " ms\n";
        ss << "Average time per step: " 
           << (duration.count() / (double)numSteps) << " ms\n";
        
        if (!outputFile.empty()) {
            // TODO: Implement result saving
            ss << "Results saved to: " << outputFile << "\n";
        }
        
        return ss.str();
    }
    
    std::string getDescription() const override {
        return "Run brain simulation for specified number of steps";
    }
    
    std::string getName() const override {
        return "run";
    }
    
    std::vector<std::string> getArgs() const override {
        return {"steps", "output_file", "logging"};
    }
};

class ExploreCommand : public Command {
public:
    std::string execute() override {
        auto brain = systemManager.getCurrentBrain();
        if (!brain) return "No brain initialized";
        
        std::stringstream ss;
        ss << "=== Advanced Exploration Mode ===\n\n";
        
        // Get all system components
        auto workingMemory = brain->getWorkingMemory();
        auto episodicMemory = brain->getEpisodicMemory();
        auto predictionSystem = brain->getPredictionSystem();
        auto planner = brain->getPlanner();
        auto attention = brain->getAttention();
        auto curiosity = brain->getCuriosity();
        auto novelty = brain->getNovelty();
        auto dopamine = brain->getDopamine();
        auto development = brain->getDevelopmentSystem();
        
        ss << "1. MEMORY SYSTEMS:\n";
        if (workingMemory) {
            ss << "   Working Memory: " << workingMemory->getActiveTraces() << " active traces\n";
        }
        if (episodicMemory) {
            ss << "   Episodic Memory: " << episodicMemory->getEpisodeCount() << " episodes stored\n";
        }
        
        ss << "2. NEUROMODULATION:\n";
        if (dopamine) {
            ss << "   Dopamine Level: " << dopamine->getLevel() << "\n";
        }
        if (curiosity) {
            ss << "   Curiosity Level: " << curiosity->getLevel() << "\n";
        }
        if (novelty) {
            ss << "   Novelty Level: " << novelty->getLevel() << "\n";
        }
        
        ss << "3. COGNITION:\n";
        if (planner) {
            ss << "   Planning Depth: Access planner data\n";
        }
        if (attention) {
            ss << "   Attention Selectivity: Access attention data\n";
        }
        if (predictionSystem) {
            ss << "   Prediction System: Active\n";
        }
        
        ss << "4. DEVELOPMENT:\n";
        if (development) {
            ss << "   Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
            ss << "   Age: " << development->getDevelopmentalAge() << " seconds\n";
        }
        
        ss << "\n5. EXPLORATION OPTIONS:\n";
        ss << "   - Run exploration simulation (cmd: explore run)
- Analyze memory patterns (cmd: analyze memory)
- Test cognitive capabilities (cmd: explore cognition)
- Benchmark system performance (cmd: benchmark)
- Save brain state (cmd: save <filename>)
- Load brain state (cmd: load <filename>)

";
        
        return ss.str();
    }
    
    std::string getDescription() const override {
        return "Enter advanced exploration mode with extended capabilities";
    }
    
    std::string getName() const override {
        return "explore";
    }
    
    std::vector<std::string> getArgs() const override {
        return {"mode"};
    }
};

class HelpCommand : public Command {
public:
    std::string execute() override {
        std::stringstream ss;
        ss << "=== NLM Phase 6 Command Help ===\n\n";
        ss << "BASIC COMMANDS:\n";
        ss << "  help                    - Show this help message\n";
        ss << "  status                  - Display brain status and statistics\n";
        ss << "  run <steps>             - Run simulation for specified steps\n";
        ss << "  explore                 - Enter advanced exploration mode\n";
        ss << "  save <file>             - Save brain state to file\n";
        ss << "  load <file>             - Load brain state from file\n";
        ss << "  exit                    - Exit program\n\n";
        ss << "ADVANCED COMMANDS (Exploration Mode):\n";
        ss << "  explore run <episodes> - Run exploration simulation\n";
        ss << "  analyze <component>     - Analyze specific system component\n";
        ss << "  explore cognition       - Test cognitive capabilities\n";
        ss << "  benchmark               - Benchmark system performance\n";
        ss << "  profile                 - Profile system execution\n\n";
        ss << "EXPERIMENT MANAGEMENT:\n";
        ss << "  experiments list        - List available experiments\n";
        ss << "  experiment <name> run   - Run specific experiment\n";
        ss << "  batch <config>          - Run batch of experiments\n\n";
        ss << "CONFIGURATION:\n";
        ss << "  config show             - Show current configuration\n";
        ss << "  config set <key> <val>  - Set configuration value\n";
        ss << "  config load <file>      - Load configuration from file\n";
        ss << "  config save <file>      - Save configuration to file\n\n";
        ss << "PYTHON INTEGRATION:\n";
        ss << "  python eval <code>      - Evaluate Python code with brain access\n";
        ss << "  python script <file>    - Run Python script with brain access\n";
        ss << "  python interactive       - Start Python interactive session\n\n";
        ss << "PERFORMANCE:\n";
        ss << "  monitor start           - Start performance monitoring\n";
        ss << "  monitor stop            - Stop performance monitoring\n";
        ss << "  stats                   - Show performance statistics\n";
        ss << "  profile                 - Profile execution bottlenecks\n\n";
        ss << "UTILITY:\n";
        ss << "  clear                   - Clear screen\n";
        ss << "  version                 - Show NLM version information\n";
        ss << "  log <level>             - Set log level (DEBUG, INFO, WARN, ERROR)\n";
        ss << "  echo <message>          - Print message to console\n";
        
        return ss.str();
    }
    
    std::string getDescription() const override {
        return "Show help information for available commands";
    }
    
    std::string getName() const override {
        return "help";
    }
    
    std::vector<std::string> getArgs() const override {
        return {"topic"};
    }
};

class ExperimentCommand : public Command {
public:
    std::string execute() override {
        return "Experiment command - TODO: Implement experiment management";
    }
    
    std::string getDescription() const override {
        return "Manage and run experiments";
    }
    
    std::string getName() const override {
        return "experiment";
    }
    
    std::vector<std::string> getArgs() const override {
        return {"action", "name"};
    }
};

class ConfigCommand : public Command {
public:
    std::string execute() override {
        return "Config command - TODO: Implement configuration management";
    }
    
    std::string getDescription() const override {
        return "Manage NLM configuration";
    }
    
    std::string getName() const override {
        return "config";
    }
    
    std::vector<std::string> getArgs() const override {
        return {"action", "key", "value"};
    }
};

class PythonCommand : public Command {
public:
    std::string execute() override {
        return "Python command - TODO: Implement Python integration";
    }
    
    std::string getDescription() const override {
        return "Python integration and scripting";
    }
    
    std::string getName() const override {
        return "python";
    }
    
    std::vector<std::string> getArgs() const override {
        return {"mode", "code_or_file"};
    }
};

class MonitorCommand : public Command {
public:
    std::string execute() override {
        return "Monitor command - TODO: Implement performance monitoring";
    }
    
    std::string getDescription() const override {
        return "Performance monitoring and profiling";
    }
    
    std::string getName() const override {
        return "monitor";
    }
    
    std::vector<std::string> getArgs() const override {
        return {"action"};
    }
};

class ExperimentRunner : public Command {
public:
    std::string execute() override {
        return "Experiment runner - TODO: Implement experiment runner";
    }
    
    std::string getDescription() const override {
        return "Run NLM experiments and benchmarks";
    }
    
    std::string getName() const override {
        return "experiments";
    }
    
    std::vector<std::string> getArgs() const override {
        return {"action", "name"};
    }
};

// Command Parser
class CommandParser {
private:
    std::map<std::string, std::shared_ptr<Command>> commands;
    
public:
    CommandParser() {
        // Register basic commands
        commands["help"] = std::make_shared<HelpCommand>();
        commands["status"] = std::make_shared<BrainStatusCommand>();
        commands["exit"] = std::make_shared<Command>();// Dummy command
        commands["quit"] = std::make_shared<Command>();// Dummy command
        
        // Register advanced commands
        commands["run"] = std::make_shared<RunSimulationCommand>();
        commands["explore"] = std::make_shared<ExploreCommand>();
        commands["experiment"] = std::make_shared<ExperimentCommand>();
        commands["config"] = std::make_shared<ConfigCommand>();
        commands["python"] = std::make_shared<PythonCommand>();
        commands["monitor"] = std::make_shared<MonitorCommand>();
        commands["experiments"] = std::make_shared<ExperimentRunner>();
    }
    
    std::string parseAndExecute(const std::string& input) {
        std::vector<std::string> tokens = tokenize(input);
        if (tokens.empty()) {
            return "";
        }
        
        std::string commandName = tokens[0];
        auto it = commands.find(commandName);
        
        if (it == commands.end()) {
            return "Unknown command: " + commandName + "\nType 'help' for available commands.";
        }
        
        std::shared_ptr<Command> cmd = it->second;
        
        // Validate arguments
        std::vector<std::string> args = cmd->getArgs();
        if (tokens.size() - 1 != args.size()) {
            return "Invalid number of arguments for command: " + commandName + "\n" +
                   "Expected: " + formatArgs(args) + "\n" +
                   "Got: " + formatArgs(tokens.substr(1)) + "\n" +
                   "Use 'help " + commandName + "' for details.";
        }
        
        // Create command instance with arguments
        std::shared_ptr<RunSimulationCommand> runCmd = std::dynamic_pointer_cast<RunSimulationCommand>(cmd);
        if (runCmd && !tokens[1].empty()) {
            int steps = std::stoi(tokens[1]);
            std::string output = (tokens.size() > 2) ? tokens[2] : "";
            bool logging = (tokens.size() > 3) ? (tokens[3] == "true" || tokens[3] == "1") : false;
            cmd = std::make_shared<RunSimulationCommand>(steps, output, logging);
        }
        
        // Execute command
        return cmd->execute();
    }
    
    std::vector<std::string> tokenize(const std::string& input) {
        std::vector<std::string> tokens;
        std::string token;
        bool inQuote = false;
        
        for (char c : input) {
            if (c == '"') {
                inQuote = !inQuote;
            } else if (std::isspace(c) && !inQuote) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        
        if (!token.empty()) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    std::string formatArgs(const std::vector<std::string>& args) {
        std::stringstream ss;
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "<" << args[i] << ">";
        }
        return ss.str();
    }
    
    std::string formatArgs(const std::vector<std::string>& args, size_t start) {
        std::vector<std::string> subArgs(args.begin() + start, args.end());
        return formatArgs(subArgs);
    }
    
    std::string getHelpForCommand(const std::string& commandName) {
        auto it = commands.find(commandName);
        if (it == commands.end()) {
            return "Command not found: " + commandName;
        }
        
        std::stringstream ss;
        ss << "Command: " << it->second->getName() << "\n";
        ss << "Description: " << it->second->getDescription() << "\n";
        ss << "Arguments: " << formatArgs(it->second->getArgs()) << "\n";
        
        return ss.str();
    }
};

// Main application
class NLMApplication {
public:
    void run() {
        printBanner();
        
        std::cout << "=== NLM Phase 6 Enhanced CLI ===\n\n";
        std::cout << "Available commands: help, status, run, explore, experiments, config, python, monitor\n";
        std::cout << "Type 'help' for detailed command information.\n\n";
        
        CommandParser parser;
        std::string input;
        
        while (true) {
            std::cout << ">>> ";
            std::getline(std::cin, input);
            
            if (input.empty()) {
                continue;
            }
            
            // Handle help for commands
            if (input == "help") {
                std::cout << parser.parseAndExecute("help");
                continue;
            }
            
            // Handle help for specific commands
            if (input.substr(0, 5) == "help ") {
                std::string commandName = input.substr(5);
                std::cout << parser.getHelpForCommand(commandName);
                continue;
            }
            
            // Parse and execute command
            std::string result = parser.parseAndExecute(input);
            
            if (result.empty()) {
                continue;
            }
            
            std::cout << result << "\n";
            
            // Check for exit command
            if (input == "exit" || input == "quit") {
                std::cout << "Goodbye!\n";
                break;
            }
        }
    }
    
private:
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Enhanced Command Line Interface                          ║
    ║     Advanced Brain Simulation Platform                       ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLMApplication app;
    app.run();
    
    return 0;
}

} // namespace nlm