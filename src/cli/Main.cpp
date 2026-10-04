// NLM Command-Line Interface
// Phase 8: Advanced User Interface
//
// This file implements a comprehensive command-line interface for NLM
// providing multiple commands for different use cases and user skill levels

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Types/Types.hpp"
#include <filesystem>

namespace nlm {
namespace cli {

// Forward declarations
class Command;
class CommandLineInterface;

// Command base class
class Command {
public:
    virtual ~Command() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual bool execute(const std::vector<std::string>& args) = 0;
};

// Simple argument parser
class ArgumentParser {
public:
    struct Argument {
        std::string name;
        std::string description;
        bool required = false;
        std::string defaultValue;
        bool hasValue = false;
    };

    bool addArgument(const std::string& name, const std::string& description, 
                   bool required = false, const std::string& defaultValue = "") {
        Argument arg;
        arg.name = name;
        arg.description = description;
        arg.required = required;
        arg.defaultValue = defaultValue;
        arguments[name] = arg;
        return true;
    }

    bool parse(const std::vector<std::string>& args, std::map<std::string, std::string>& parsedArgs) {
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i].substr(0, 2) == "--") {
                std::string argName = args[i].substr(2);
                if (arguments.find(argName) != arguments.end()) {
                    if (i + 1 < args.size() && args[i + 1].substr(0, 2) != "--") {
                        parsedArgs[argName] = args[++i];
                    } else {
                        parsedArgs[argName] = "";
                    }
                }
            }
        }

        for (const auto& pair : arguments) {
            if (pair.second.required && parsedArgs.find(pair.first) == parsedArgs.end()) {
                return false;
            }
            if (parsedArgs.find(pair.first) == parsedArgs.end() && !pair.second.defaultValue.empty()) {
                parsedArgs[pair.first] = pair.second.defaultValue;
            }
        }
        return true;
    }

    std::vector<Argument> getArguments() const {
        std::vector<Argument> args;
        for (const auto& pair : arguments) {
            args.push_back(pair.second);
        }
        return args;
    }

private:
    std::map<std::string, Argument> arguments;
};

// Simulation Command
class RunCommand : public Command {
public:
    std::string getName() const override { return "run"; }
    std::string getDescription() const override { 
        return "Run a neural simulation with specified configuration"; 
    }
    std::string getUsage() const override { 
        return "nlm run --steps <N> [--config <file>] [--output <file>] [--log-level <LEVEL>] [--save] [--quiet]"; 
    }

    bool execute(const std::vector<std::string>& args) override {
        ArgumentParser parser;
        parser.addArgument("steps", "Number of simulation steps to run", true);
        parser.addArgument("config", "Configuration file path", false, "configs/default.cfg");
        parser.addArgument("output", "Output file for brain state", false, "brain_state.bin");
        parser.addArgument("log-level", "Log level (INFO, WARNING, ERROR)", false, "INFO");
        parser.addArgument("save", "Save brain state after simulation");
        parser.addArgument("quiet", "Suppress output messages");

        std::map<std::string, std::string> parsedArgs;
        if (!parser.parse(args, parsedArgs)) {
            std::cerr << "Error: Missing required arguments" << std::endl;
            std::cout << "Usage: " << getUsage() << std::endl;
            return false;
        }

        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(parseLogLevel(parsedArgs["log-level"]));
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);

        std::cout << "=== NLM Simulation ===" << std::endl;
        std::cout << "Steps: " << parsedArgs["steps"] << std::endl;
        std::cout << "Config: " << parsedArgs["config"] << std::endl;

        try {
            auto config = std::make_shared<Config>();
            config->loadFromFile(parsedArgs["config"]);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                std::cerr << "Error: Failed to initialize brain" << std::endl;
                return false;
            }

            int steps = std::stoi(parsedArgs["steps"]);
            bool save = parsedArgs.find("save") != parsedArgs.end();
            bool quiet = parsedArgs.find("quiet") != parsedArgs.end();

            for (int step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);

                if (!quiet && step % 100 == 0) {
                    std::cout << "Step " << step << "/" << steps 
                              << " | Neurons: " << brain->getFiringNeuronCount()
                              << " | Spikes: " << brain->getTotalSpikeCount()
                              << " | Rate: " << brain->getAverageFiringRate() << " Hz"
                              << std::endl;
                }
            }

            if (save) {
                std::cout << "Saving brain state to " << parsedArgs["output"] << std::endl;
                brain->save(parsedArgs["output"]);
            }

            brain->logStatus();
            std::cout << "=== Simulation Complete ===" << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }

        return true;
    }

private:
    LogLevel parseLogLevel(const std::string& level) {
        std::string upperLevel = level;
        std::transform(upperLevel.begin(), upperLevel.end(), upperLevel.begin(), ::toupper);
        if (upperLevel == "WARNING") return LogLevel::Warning;
        if (upperLevel == "ERROR") return LogLevel::Error;
        return LogLevel::Info;
    }
};

// Configuration Command
class ConfigCommand : public Command {
public:
    std::string getName() const override { return "config"; }
    std::string getDescription() const override { 
        return "Manage configuration files and settings"; 
    }
    std::string getUsage() const override { 
        return "nlm config [list|save <file>|load <file>|show <key>|set <key> <value>]"; 
    }

    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cout << "Usage: " << getUsage() << std::endl;
            return true;
        }

        std::string subcommand = args[0];
        
        if (subcommand == "list") {
            auto config = std::make_shared<Config>();
            auto keys = config->getKeys();
            std::cout << "Configuration keys:" << std::endl;
            for (const auto& key : keys) {
                std::cout << "  " << key << std::endl;
            }
        }
        else if (subcommand == "show") {
            if (args.size() < 2) {
                std::cerr << "Error: Missing key argument" << std::endl;
                return false;
            }
            auto config = std::make_shared<Config>();
            if (config->has(args[1])) {
                std::cout << args[1] << " = " << config->summary() << std::endl;
            } else {
                std::cout << "Key not found: " << args[1] << std::endl;
            }
        }
        else if (subcommand == "set") {
            if (args.size() < 3) {
                std::cerr << "Error: Missing key and value arguments" << std::endl;
                return false;
            }
            auto config = std::make_shared<Config>();
            config->set(args[1], args[2]);
            std::cout << "Set " << args[1] << " = " << args[2] << std::endl;
        }
        else if (subcommand == "save") {
            if (args.size() < 2) {
                std::cerr << "Error: Missing file argument" << std::endl;
                return false;
            }
            auto config = std::make_shared<Config>();
            if (config->saveToFile(args[1])) {
                std::cout << "Configuration saved to " << args[1] << std::endl;
            } else {
                std::cerr << "Error: Failed to save configuration" << std::endl;
                return false;
            }
        }
        else if (subcommand == "load") {
            if (args.size() < 2) {
                std::cerr << "Error: Missing file argument" << std::endl;
                return false;
            }
            auto config = std::make_shared<Config>();
            if (config->loadFromFile(args[1])) {
                std::cout << "Configuration loaded from " << args[1] << std::endl;
            } else {
                std::cerr << "Error: Failed to load configuration" << std::endl;
                return false;
            }
        }
        else {
            std::cerr << "Error: Unknown subcommand: " << subcommand << std::endl;
            std::cout << "Usage: " << getUsage() << std::endl;
            return false;
        }

        return true;
    }
};

// Stats Command
class StatsCommand : public Command {
public:
    std::string getName() const override { return "stats"; }
    std::string getDescription() const override { 
        return "Display brain statistics and metrics"; 
    }
    std::string getUsage() const override { 
        return "nlm stats [--brain <file>] [--format <json|text|table>] [--verbose]"; 
    }

    bool execute(const std::vector<std::string>& args) override {
        ArgumentParser parser;
        parser.addArgument("brain", "Brain state file path", false, "brain_state.bin");
        parser.addArgument("format", "Output format (json, text, table)", false, "text");
        parser.addArgument("verbose", "Verbose output");

        std::map<std::string, std::string> parsedArgs;
        if (!parser.parse(args, parsedArgs)) {
            std::cerr << "Error: Missing required arguments" << std::endl;
            std::cout << "Usage: " << getUsage() << std::endl;
            return false;
        }

        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);

        std::cout << "=== Brain Statistics ===" << std::endl;
        
        // Try to load if file exists
        if (std::filesystem::exists(parsedArgs["brain"])) {
            if (brain->load(parsedArgs["brain"])) {
                std::cout << "Loaded brain state from " << parsedArgs["brain"] << std::endl;
            }
        }

        auto info = brain->getConfig();
        std::cout << "Brain Configuration:" << std::endl;
        std::cout << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        
        if (parsedArgs.find("verbose") != parsedArgs.end()) {
            std::cout << std::endl << "Detailed Information:" << std::endl;
            brain->logStatus();
        }

        return true;
    }
};

// Phase Command
class PhaseCommand : public Command {
public:
    std::string getName() const override { return "phase"; }
    std::string getDescription() const override { 
        return "Run specific phase experiments"; 
    }
    std::string getUsage() const override { 
        return "nlm phase <3|4|5|6> [--steps <N>] [--config <file>] [--verify]"; 
    }

    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cerr << "Error: Missing phase number" << std::endl;
            std::cout << "Usage: " << getUsage() << std::endl;
            return false;
        }

        char phaseChar = args[0][0];
        if (!std::isdigit(phaseChar)) {
            std::cerr << "Error: Invalid phase number: " << args[0] << std::endl;
            return false;
        }

        int phase = phaseChar - '0';
        if (phase < 3 || phase > 6) {
            std::cerr << "Error: Phase must be between 3 and 6" << std::endl;
            return false;
        }

        ArgumentParser parser;
        parser.addArgument("steps", "Number of steps", false, "1000");
        parser.addArgument("config", "Configuration file", false, "configs/default.cfg");
        parser.addArgument("verify", "Run integration verification");

        std::map<std::string, std::string> parsedArgs;
        if (!parser.parse(args, parsedArgs)) {
            std::cerr << "Error: Missing required arguments" << std::endl;
            std::cout << "Usage: " << getUsage() << std::endl;
            return false;
        }

        std::cout << "=== NLM Phase " << phase << " Experiment ===" << std::endl;
        std::cout << "Steps: " << parsedArgs["steps"] << std::endl;
        std::cout << "Config: " << parsedArgs["config"] << std::endl;

        // For now, just run a basic simulation
        // In a real implementation, this would call the actual phase experiment code
        auto config = std::make_shared<Config>();
        config->loadFromFile(parsedArgs["config"]);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain" << std::endl;
            return false;
        }

        int steps = std::stoi(parsedArgs["steps"]);
        bool verify = parsedArgs.find("verify") != parsedArgs.end();

        for (int step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
        }

        if (verify) {
            std::cout << "Running integration verification..." << std::endl;
            // Would call actual verification logic
            std::cout << "Verification complete (placeholder)" << std::endl;
        }

        std::cout << "=== Phase " << phase << " Complete ===" << std::endl;
        brain->logStatus();

        return true;
    }
};

// Demo Command
class DemoCommand : public Command {
public:
    std::string getName() const override { return "demo"; }
    std::string getDescription() const override { 
        return "Run pre-configured demonstrations"; 
    }
    std::string getUsage() const override { 
        return "nlm demo <basic|plasticity|stdp|memory|neuromodulation> [--steps <N>]"; 
    }

    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cerr << "Error: Missing demo type" << std::endl;
            std::cout << "Usage: " << getUsage() << std::endl;
            return false;
        }

        std::string demoType = args[0];
        ArgumentParser parser;
        parser.addArgument("steps", "Number of steps", false, "500");

        std::map<std::string, std::string> parsedArgs;
        if (!parser.parse(args, parsedArgs)) {
            std::cerr << "Error: Missing required arguments" << std::endl;
            std::cout << "Usage: " << getUsage() << std::endl;
            return false;
        }

        std::cout << "=== NLM Demo: " << demoType << " ===" << std::endl;
        
        // Initialize with demo-specific configuration
        auto config = std::make_shared<Config>();
        config->set("random_seed", 42);
        config->set("neuron_count", 500);
        config->set("simulation_timestep", 0.001);

        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain" << std::endl;
            return false;
        }

        int steps = std::stoi(parsedArgs["steps"]);

        if (demoType == "basic") {
            std::cout << "Running basic connectivity test..." << std::endl;
            // Run basic connectivity test
            for (int step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);
                if (step % 100 == 0) {
                    std::cout << "Step " << step << ". Spikes: " << brain->getTotalSpikeCount() << std::endl;
                }
            }
        }
        else if (demoType == "plasticity") {
            std::cout << "Running plasticity experiment..." << std::endl;
            // Enable plasticity and run learning experiment
            for (auto& region : brain->getRegions()) {
                for (auto& synapse : region->getSynapses()) {
                    synapse->enablePlasticity(true, true, false);
                }
            }
            
            for (int step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);
            }
        }
        else if (demoType == "stdp") {
            std::cout << "Running STDP verification..." << std::endl;
            // Enable only STDP for STDP-specific test
            for (auto& region : brain->getRegions()) {
                for (auto& synapse : region->getSynapses()) {
                    synapse->enablePlasticity(false, true, false);
                }
            }
            
            for (int step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);
            }
        }
        else {
            std::cerr << "Error: Unknown demo type: " << demoType << std::endl;
            return false;
        }

        std::cout << "=== Demo Complete ===" << std::endl;
        brain->logStatus();

        return true;
    }
};

// Benchmark Command
class BenchmarkCommand : public Command {
public:
    std::string getName() const override { return "benchmark"; }
    std::string getDescription() const override { 
        return "Performance benchmarking"; 
    }
    std::string getUsage() const override { 
        return "nlm benchmark [--neurons <N>] [--steps <N>] [--iterations <N>] [--warmup <N>]"; 
    }

    bool execute(const std::vector<std::string>& args) override {
        ArgumentParser parser;
        parser.addArgument("neurons", "Number of neurons", false, "1000");
        parser.addArgument("steps", "Steps per iteration", false, "1000");
        parser.addArgument("iterations", "Number of iterations", false, "5");
        parser.addArgument("warmup", "Warmup iterations", false, "2");

        std::map<std::string, std::string> parsedArgs;
        if (!parser.parse(args, parsedArgs)) {
            std::cerr << "Error: Missing required arguments" << std::endl;
            std::cout << "Usage: " << getUsage() << std::endl;
            return false;
        }

        int neurons = std::stoi(parsedArgs["neurons"]);
        int steps = std::stoi(parsedArgs["steps"]);
        int iterations = std::stoi(parsedArgs["iterations"]);
        int warmup = std::stoi(parsedArgs["warmup"]);

        std::cout << "=== NLM Performance Benchmark ===" << std::endl;
        std::cout << "Neurons: " << neurons << std::endl;
        std::cout << "Steps per iteration: " << steps << std::endl;
        std::cout << "Iterations: " << iterations << std::endl;
        std::cout << "Warmup: " << warmup << std::endl;

        double totalTime = 0.0;
        size_t totalSpikes = 0;

        for (int iter = 0; iter < iterations + warmup; ++iter) {
            auto config = std::make_shared<Config>();
            config->set("random_seed", 42 + iter);
            config->set("neuron_count", neurons);

            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                std::cerr << "Error: Failed to initialize brain" << std::endl;
                return false;
            }

            auto start = std::chrono::high_resolution_clock::now();
            
            for (int step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(end - start).count();

            if (iter >= warmup) {
                totalTime += duration;
                totalSpikes += brain->getTotalSpikeCount();
                std::cout << "Iteration " << iter - warmup + 1 << ": " 
                          << "Time: " << std::fixed << std::setprecision(3) << duration << "s "
                          << "Spikes: " << brain->getTotalSpikeCount() << " "
                          << "Rate: " << brain->getAverageFiringRate() << " Hz"
                          << std::endl;
            }
        }

        std::cout << std::endl << "=== Benchmark Results ===" << std::endl;
        std::cout << "Average time per iteration: " << std::fixed << std::setprecision(3) 
                  << (totalTime / iterations) << "s" << std::endl;
        std::cout << "Average spikes per iteration: " << (totalSpikes / iterations) << std::endl;
        std::cout << "Average firing rate: " << (totalSpikes / iterations / steps * 1000) << " Hz" << std::endl;

        return true;
    }
};

// Help Command
class HelpCommand : public Command {
public:
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { 
        return "Show help information"; 
    }
    std::string getUsage() const override { 
        return "nlm help [command]"; 
    }

    bool execute(const std::vector<std::string>& args) override {
        if (args.empty()) {
            std::cout << "=== NLM Command-Line Interface ===" << std::endl;
            std::cout << "Available commands:" << std::endl;
            std::cout << "  run        Run a neural simulation" << std::endl;
            std::cout << "  config     Manage configuration files" << std::endl;
            std::cout << "  stats      Display brain statistics" << std::endl;
            std::cout << "  phase      Run specific phase experiments" << std::endl;
            std::cout << "  demo       Run pre-configured demonstrations" << std::endl;
            std::cout << "  benchmark  Performance benchmarking" << std::endl;
            std::cout << "  help       Show this help information" << std::endl;
            std::cout << std::endl;
            std::cout << "For detailed help on a specific command: nlm help <command>" << std::endl;
        } else {
            Command* cmd = getCommand(args[0]);
            if (cmd) {
                std::cout << "=== " << cmd->getName() << " ===" << std::endl;
                std::cout << cmd->getDescription() << std::endl;
                std::cout << "Usage: " << cmd->getUsage() << std::endl;
            } else {
                std::cerr << "Error: Unknown command: " << args[0] << std::endl;
            }
        }

        return true;
    }

private:
    Command* getCommand(const std::string& name) {
        if (name == "run") return new RunCommand();
        if (name == "config") return new ConfigCommand();
        if (name == "stats") return new StatsCommand();
        if (name == "phase") return new PhaseCommand();
        if (name == "demo") return new DemoCommand();
        if (name == "benchmark") return new BenchmarkCommand();
        if (name == "help") return new HelpCommand();
        return nullptr;
    }
};

// Command Line Interface
class CommandLineInterface {
public:
    CommandLineInterface() {
        commands["run"] = new RunCommand();
        commands["config"] = new ConfigCommand();
        commands["stats"] = new StatsCommand();
        commands["phase"] = new PhaseCommand();
        commands["demo"] = new DemoCommand();
        commands["benchmark"] = new BenchmarkCommand();
        commands["help"] = new HelpCommand();
    }

    ~CommandLineInterface() {
        for (auto& pair : commands) {
            delete pair.second;
        }
    }

    bool run(int argc, char* argv[]) {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }

        if (args.empty()) {
            std::cout << "Usage: nlm <command> [options]" << std::endl;
            std::cout << "For more information: nlm help" << std::endl;
            return false;
        }

        std::string commandName = args[0];
        auto it = commands.find(commandName);
        if (it == commands.end()) {
            std::cerr << "Error: Unknown command: " << commandName << std::endl;
            std::cout << "For more information: nlm help" << std::endl;
            return false;
        }

        Command* command = it->second;
        return command->execute(args);
    }

private:
    std::map<std::string, Command*> commands;
};

}} // namespace nlm::cli

// Main function
int main(int argc, char* argv[]) {
    nlm::cli::CommandLineInterface cli;
    return cli.run(argc, argv) ? 0 : 1;
}