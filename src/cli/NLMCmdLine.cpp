#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <nlohmann/json.hpp>

#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/core/Config/Config.hpp"

using json = nlohmann::json;

class NLMCommandLine {
public:
    NLMCommandLine() {
        initializeLogger();
    }
    
    void run() {
        parseArguments();
    }
    
private:
    struct Options {
        std::string command;
        std::string configFile;
        std::string outputFile;
        std::string action;
        std::string subcommand;
        int steps = 100;
        int neurons = 1000;
        int regions = 1;
        float connectionProbability = 0.1f;
        bool verbose = false;
        bool help = false;
        bool version = false;
        std::vector<std::string> extraArgs;
    };
    
    Options options;
    std::shared_ptr<nlm::Logger> logger;
    
    void initializeLogger() {
        logger = std::make_shared<nlm::Logger>();
        auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
        logger->addLogger(consoleLogger);
        nlm::Logger::setGlobal(logger);
    }
    
    void parseArguments() {
        if (argc < 2) {
            printHelp();
            return;
        }
        
        options.command = argv[1];
        
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                options.help = true;
            } else if (arg == "--version") {
                options.version = true;
            } else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
            } else if (arg == "--config" && i + 1 < argc) {
                options.configFile = argv[++i];
            } else if (arg == "--output" && i + 1 < argc) {
                options.outputFile = argv[++i];
            } else if (arg == "--steps" && i + 1 < argc) {
                options.steps = std::stoi(argv[++i]);
            } else if (arg == "--neurons" && i + 1 < argc) {
                options.neurons = std::stoi(argv[++i]);
            } else if (arg == "--regions" && i + 1 < argc) {
                options.regions = std::stoi(argv[++i]);
            } else if (arg == "--connectivity" && i + 1 < argc) {
                options.connectionProbability = std::stof(argv[++i]);
            } else {
                options.extraArgs.push_back(arg);
            }
        }
        
        if (options.help) {
            printHelp();
            return;
        }
        
        if (options.version) {
            printVersion();
            return;
        }
        
        executeCommand();
    }
    
    void printHelp() {
        std::cout << "=== NLM Command Line Interface ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm [OPTIONS] [COMMAND] [SUBCOMMAND]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  simulate     Run a brain simulation" << std::endl;
        std::cout << "  brain        Brain management operations" << std::endl;
        std::cout << "  world        World simulation operations" << std::endl;
        std::cout << "  agent        Agent operations" << std::endl;
        std::cout << "  config       Configuration management" << std::endl;
        std::cout << "  export       Export data to file formats" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h              Show this help message" << std::endl;
        std::cout << "  --version               Show version information" << std::endl;
        std::cout << "  --verbose, -v           Enable verbose output" << std::endl;
        std::cout << "  --config FILE           Load configuration from file" << std::endl;
        std::cout << "  --output FILE           Save output to file" << std::endl;
        std::cout << "  --steps N               Number of simulation steps" << std::endl;
        std::cout << "  --neurons N             Number of neurons" << std::endl;
        std::cout << "  --regions N             Number of brain regions" << std::endl;
        std::cout << "  --connectivity P        Connection probability" << std::endl;
        std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm simulate --steps 500" << std::endl;
        std::cout << "  nlm brain create --neurons 2000 --regions 2" << std::endl;
        std::cout << "  nlm export --output results.json" << std::endl;
    }
    
    void printVersion() {
        std::cout << "NLM (Neural Learning Machine) v0.1.0" << std::endl;
        std::cout << "Phase 2: Real Neural Computation" << std::endl;
        std::cout << "An experimental artificial developmental brain." << std::endl;
    }
    
    void executeCommand() {
        if (options.subcommand.empty()) {
            printHelp();
            return;
        }
        
        if (options.command == "simulate") {
            simulateCommand();
        } else if (options.command == "brain") {
            brainCommand();
        } else if (options.command == "world") {
            worldCommand();
        } else if (options.command == "agent") {
            agentCommand();
        } else if (options.command == "config") {
            configCommand();
        } else if (options.command == "export") {
            exportCommand();
        } else {
            std::cerr << "Unknown command: " << options.command << std::endl;
            printHelp();
        }
    }
    
    void simulateCommand() {
        std::cout << "=== Running NLM Simulation ===" << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        
        if (!options.configFile.empty()) {
            if (config->loadFromFile(options.configFile)) {
                std::cout << "Loaded configuration from: " << options.configFile << std::endl;
            } else {
                std::cout << "Using default configuration (file not found)" << std::endl;
            }
        }
        
        // Apply command line overrides
        config->set("neuron_count", static_cast<int64_t>(options.neurons), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(options.regions), nlm::ConfigSource::Default);
        config->set("connection_probability", options.connectionProbability, nlm::ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        std::cout << "Brain initialized: " << brain->getTotalNeuronCount() << " neurons, " 
                  << brain->getTotalSynapseCount() << " synapses" << std::endl;
        
        // Run simulation
        for (nlm::SimulationStep step = 0; step < options.steps; ++step) {
            brain->step(step);
            
            if (options.verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": " << brain->getFiringNeuronCount() 
                          << " firing neurons, total spikes: " << brain->getTotalSpikeCount() << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "=== Simulation Complete ===" << std::endl;
        std::cout << "Final statistics:" << std::endl;
        std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Active neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        
        if (!options.outputFile.empty()) {
            brain->save(options.outputFile);
            std::cout << "Brain state saved to: " << options.outputFile << std::endl;
        }
    }
    
    void brainCommand() {
        if (options.extraArgs.empty()) {
            std::cerr << "Brain subcommand required (create, load, save, status)" << std::endl;
            return;
        }
        
        options.subcommand = options.extraArgs[0];
        options.extraArgs.erase(options.extraArgs.begin());
        
        if (options.subcommand == "create") {
            brainCreateCommand();
        } else if (options.subcommand == "load") {
            brainLoadCommand();
        } else if (options.subcommand == "save") {
            brainSaveCommand();
        } else if (options.subcommand == "status") {
            brainStatusCommand();
        } else {
            std::cerr << "Unknown brain subcommand: " << options.subcommand << std::endl;
        }
    }
    
    void brainCreateCommand() {
        std::cout << "=== Creating New Brain ===" << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        
        config->set("neuron_count", static_cast<int64_t>(options.neurons), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(options.regions), nlm::ConfigSource::Default);
        config->set("connection_probability", options.connectionProbability, nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        std::cout << "Brain created successfully:" << std::endl;
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Regions: " << brain->getRegionCount() << std::endl;
        
        if (!options.outputFile.empty()) {
            brain->save(options.outputFile);
            std::cout << "Saved to: " << options.outputFile << std::endl;
        }
    }
    
    void brainLoadCommand() {
        if (options.extraArgs.empty()) {
            std::cerr << "Output file required for brain load command" << std::endl;
            return;
        }
        
        std::string inputFile = options.extraArgs[0];
        auto config = std::make_shared<nlm::Config>();
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        if (brain->load(inputFile)) {
            std::cout << "Brain loaded successfully from: " << inputFile << std::endl;
            brainStatusCommand();
        } else {
            std::cerr << "Failed to load brain from: " << inputFile << std::endl;
        }
    }
    
    void brainSaveCommand() {
        if (options.extraArgs.empty()) {
            std::cerr << "Input file required for brain save command" << std::endl;
            return;
        }
        
        std::string inputFile = options.extraArgs[0];
        
        auto config = std::make_shared<nlm::Config>();
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        if (brain->load(inputFile)) {
            std::string outputFile = options.outputFile.empty() ? (inputFile + ".backup") : options.outputFile;
            if (brain->save(outputFile)) {
                std::cout << "Brain saved to: " << outputFile << std::endl;
            } else {
                std::cerr << "Failed to save brain to: " << outputFile << std::endl;
            }
        } else {
            std::cerr << "Failed to load brain from: " << inputFile << std::endl;
        }
    }
    
    void brainStatusCommand() {
        auto config = std::make_shared<nlm::Config>();
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        brain->logStatus();
    }
    
    void worldCommand() {
        if (options.extraArgs.empty()) {
            std::cerr << "World subcommand required (simulate, create, reset)" << std::endl;
            return;
        }
        
        options.subcommand = options.extraArgs[0];
        options.extraArgs.erase(options.extraArgs.begin());
        
        // Implementation for world subcommands
        std::cout << "World " << options.subcommand << " command" << std::endl;
    }
    
    void agentCommand() {
        if (options.extraArgs.empty()) {
            std::cerr << "Agent subcommand required (create, simulate, reset)" << std::endl;
            return;
        }
        
        options.subcommand = options.extraArgs[0];
        options.extraArgs.erase(options.extraArgs.begin());
        
        // Implementation for agent subcommands
        std::cout << "Agent " << options.subcommand << " command" << std::endl;
    }
    
    void configCommand() {
        if (options.extraArgs.empty()) {
            std::cerr << "Config subcommand required (create, load, save, show)" << std::endl;
            return;
        }
        
        options.subcommand = options.extraArgs[0];
        options.extraArgs.erase(options.extraArgs.begin());
        
        // Implementation for config subcommands
        std::cout << "Config " << options.subcommand << " command" << std::endl;
    }
    
    void exportCommand() {
        if (options.extraArgs.empty()) {
            std::cerr << "Export subcommand required (brain, simulation, agent)" << std::endl;
            return;
        }
        
        options.subcommand = options.extraArgs[0];
        options.extraArgs.erase(options.extraArgs.begin());
        
        if (options.subcommand == "brain" && !options.outputFile.empty()) {
            // Export brain data
            auto config = std::make_shared<nlm::Config>();
            auto brain = std::make_shared<nlm::Brain>(config);
            brain->initialize();
            
            brain->save(options.outputFile);
            std::cout << "Brain data exported to: " << options.outputFile << std::endl;
        } else {
            std::cerr << "Export format or output file not specified" << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    NLMCommandLine cli;
    cli.run();
    return 0;
}
