// NLM CLI - Improved Command Line Interface for NLM Management
// Provides comprehensive tools for configuration management, simulation control, and analysis

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <filesystem>
#include <chrono>
#include <thread>
#include <cctype>

using namespace nlm;
using namespace std;

class NLMCLI {
public:
    NLMCLI() {
        initializeLogger();
        loadConfiguration();
    }
    
    ~NLMCLI() {
        saveConfiguration();
    }
    
    void run() {
        printBanner();
        showHelp();
        
        string command;
        while (true) {
            cout << "nlm> ";
            getline(cin, command);
            
            if (command.empty()) {
                continue;
            }
            
            handleCommand(command);
        }
    }
    
private:
    struct Impl;
    unique_ptr<Impl> pImpl;
    
    void initializeLogger() {
        auto logger = make_shared<Logger>();
        auto consoleLogger = make_shared<ConsoleLogger>(LogLevel::Info);
        consoleLogger->setUseColors(true);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    void loadConfiguration() {
        config = make_shared<Config>();
        
        // Load default config
        if (config->loadFromFile("configs/default.cfg")) {
            NLM_LOG_INFO("Loaded default configuration");
        } else {
            NLM_LOG_WARNING("Using default configuration (file not found)");
        }
        
        // Set default values
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
    }
    
    void saveConfiguration() {
        if (config) {
            config->saveToFile("configs/user.cfg");
            NLM_LOG_INFO("Configuration saved to user.cfg");
        }
    }
    
    void printBanner() {
        cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM CLI - Neural Learning Machine Command Line Interface    ║
    ║                                                               ║
    ║     Enhanced configuration and simulation management tools      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
        )" << endl;
    }
    
    void showHelp() {
        cout << "Available commands:" << endl;
        cout << "  help                    Show this help message" << endl;
        cout << "  config list             List all configuration parameters" << endl;
        cout << "  config get <key>        Get configuration value" << endl;
        cout << "  config set <key> <value> Set configuration value" << endl;
        cout << "  config load <file>      Load configuration from file" << endl;
        cout << "  config save <file>      Save configuration to file" << endl;
        cout << "  brain create            Create a new brain instance" << endl;
        cout << "  brain reset             Reset current brain" << endl;
        cout << "  brain status            Show brain status and statistics" << endl;
        cout << "  simulation start        Start simulation" << endl;
        cout << "  simulation stop         Stop simulation" << endl;
        cout << "  simulation step <n>     Run N simulation steps" << endl;
        cout << "  neurons list            List all neurons" << endl;
        cout << "  neurons info <id>       Show neuron information" << endl;
        cout << "  neurons inject <id> <val> Inject current into neuron" << endl;
        cout << "  synapses list           List all synapses" << endl;
        cout << "  synapses analyze        Analyze synaptic changes" << endl;
        cout << "  experiment run <type>  Run experiment (basic/plasticity/stdp)" << endl;
        cout << "  experiment list         List available experiments" << endl;
        cout << "  log level <level>      Set log level (debug/info/warning/error)" << endl;
        cout << "  log file <file>        Set log file output" << endl;
        cout << "  log show               Show recent log entries" << endl;
        cout << "  analyze stats           Show performance statistics" << endl;
        cout << "  analyze neurons         Analyze neuron population" << endl;
        cout << "  analyze synapses        Analyze synaptic connectivity" << endl;
        cout << "  export brain <file>    Export brain state to file" << endl;
        cout << "  import brain <file>    Import brain state from file" << endl;
        cout << "  quit                   Exit CLI" << endl;
        cout << endl;
    }
    
    void handleCommand(const string& command) {
        vector<string> tokens = tokenize(command);
        if (tokens.empty()) {
            return;
        }
        
        string cmd = toLower(tokens[0]);
        
        if (cmd == "help") {
            showHelp();
        } else if (cmd == "quit" || cmd == "exit") {
            saveConfiguration();
            cout << "Exiting NLM CLI..." << endl;
            exit(0);
        } else if (cmd == "config") {
            handleConfigCommand(tokens);
        } else if (cmd == "brain") {
            handleBrainCommand(tokens);
        } else if (cmd == "simulation") {
            handleSimulationCommand(tokens);
        } else if (cmd == "neurons") {
            handleNeuronCommand(tokens);
        } else if (cmd == "synapses") {
            handleSynapseCommand(tokens);
        } else if (cmd == "experiment") {
            handleExperimentCommand(tokens);
        } else if (cmd == "log") {
            handleLogCommand(tokens);
        } else if (cmd == "analyze") {
            handleAnalyzeCommand(tokens);
        } else if (cmd == "export") {
            handleExportCommand(tokens);
        } else if (cmd == "import") {
            handleImportCommand(tokens);
        } else {
            NLM_LOG_ERROR("Unknown command: " + cmd);
            cout << "Unknown command: " << cmd << endl;
            cout << "Type 'help' for available commands." << endl;
        }
    }
    
    vector<string> tokenize(const string& command) {
        vector<string> tokens;
        string token;
        istringstream tokenStream(command);
        
        while (tokenStream >> token) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    string toLower(const string& str) {
        string result = str;
        transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return tolower(c); });
        return result;
    }
    
    void handleConfigCommand(const vector<string>& tokens) {
        if (tokens.size() < 2) {
            cout << "Config command requires subcommand." << endl;
            cout << "Available subcommands: list, get, set, load, save" << endl;
            return;
        }
        
        string subcmd = toLower(tokens[1]);
        
        if (subcmd == "list") {
            cout << "Configuration parameters:" << endl;
            auto keys = config->getKeys();
            for (const auto& key : keys) {
                auto val = config->get<string>(key);
                if (val.has_value()) {
                    cout << "  " << key << " = " << val.value() << endl;
                } else {
                    cout << "  " << key << " = <unknown>" << endl;
                }
            }
        } else if (subcmd == "get") {
            if (tokens.size() < 3) {
                cout << "Usage: config get <key>" << endl;
                return;
            }
            
            string key = tokens[2];
            auto val = config->get<string>(key);
            if (val.has_value()) {
                cout << key << " = " << val.value() << endl;
            } else {
                cout << "Key not found: " << key << endl;
            }
        } else if (subcmd == "set") {
            if (tokens.size() < 4) {
                cout << "Usage: config set <key> <value>" << endl;
                return;
            }
            
            string key = tokens[2];
            string value = tokens[3];
            config->set(key, value, ConfigSource::User);
            NLM_LOG_INFO("Configuration updated: " + key + " = " + value);
            cout << "Configuration set: " << key << " = " << value << endl;
        } else if (subcmd == "load") {
            if (tokens.size() < 3) {
                cout << "Usage: config load <file>" << endl;
                return;
            }
            
            string filepath = tokens[2];
            if (config->loadFromFile(filepath)) {
                NLM_LOG_INFO("Configuration loaded from: " + filepath);
                cout << "Configuration loaded from: " << filepath << endl;
            } else {
                NLM_LOG_ERROR("Failed to load configuration from: " + filepath);
                cout << "Failed to load configuration from: " << filepath << endl;
            }
        } else if (subcmd == "save") {
            if (tokens.size() < 3) {
                cout << "Usage: config save <file>" << endl;
                return;
            }
            
            string filepath = tokens[2];
            if (config->saveToFile(filepath)) {
                NLM_LOG_INFO("Configuration saved to: " + filepath);
                cout << "Configuration saved to: " << filepath << endl;
            } else {
                NLM_LOG_ERROR("Failed to save configuration to: " + filepath);
                cout << "Failed to save configuration to: " << filepath << endl;
            }
        } else {
            cout << "Unknown config subcommand: " << subcmd << endl;
            cout << "Available subcommands: list, get, set, load, save" << endl;
        }
    }
    
    void handleBrainCommand(const vector<string>& tokens) {
        if (tokens.size() < 2) {
            cout << "Brain command requires subcommand." << endl;
            cout << "Available subcommands: create, reset, status" << endl;
            return;
        }
        
        string subcmd = toLower(tokens[1]);
        
        if (subcmd == "create") {
            createBrain();
        } else if (subcmd == "reset") {
            resetBrain();
        } else if (subcmd == "status") {
            showBrainStatus();
        } else {
            cout << "Unknown brain subcommand: " << subcmd << endl;
        }
    }
    
    void handleSimulationCommand(const vector<string>& tokens) {
        if (tokens.size() < 2) {
            cout << "Simulation command requires subcommand." << endl;
            cout << "Available subcommands: start, stop, step" << endl;
            return;
        }
        
        string subcmd = toLower(tokens[1]);
        
        if (subcmd == "start") {
            startSimulation();
        } else if (subcmd == "stop") {
            stopSimulation();
        } else if (subcmd == "step") {
            if (tokens.size() < 3) {
                cout << "Usage: simulation step <n>" << endl;
                return;
            }
            
            int steps = stoi(tokens[2]);
            stepSimulation(steps);
        } else {
            cout << "Unknown simulation subcommand: " << subcmd << endl;
        }
    }
    
    void handleNeuronCommand(const vector<string>& tokens) {
        if (tokens.size() < 2) {
            cout << "Neuron command requires subcommand." << endl;
            cout << "Available subcommands: list, info, inject" << endl;
            return;
        }
        
        string subcmd = toLower(tokens[1]);
        
        if (subcmd == "list") {
            listNeurons();
        } else if (subcmd == "info") {
            if (tokens.size() < 3) {
                cout << "Usage: neurons info <id>" << endl;
                return;
            }
            
            int id = stoi(tokens[2]);
            showNeuronInfo(id);
        } else if (subcmd == "inject") {
            if (tokens.size() < 4) {
                cout << "Usage: neurons inject <id> <value>" << endl;
                return;
            }
            
            int id = stoi(tokens[2]);
            float value = stof(tokens[3]);
            injectCurrent(id, value);
        } else {
            cout << "Unknown neuron subcommand: " << subcmd << endl;
        }
    }
    
    void handleSynapseCommand(const vector<string>& tokens) {
        if (tokens.size() < 2) {
            cout << "Synapse command requires subcommand." << endl;
            cout << "Available subcommands: list, analyze" << endl;
            return;
        }
        
        string subcmd = toLower(tokens[1]);
        
        if (subcmd == "list") {
            listSynapses();
        } else if (subcmd == "analyze") {
            analyzeSynapses();
        } else {
            cout << "Unknown synapse subcommand: " << subcmd << endl;
        }
    }
    
    void handleExperimentCommand(const vector<string>& tokens) {
        if (tokens.size() < 2) {
            cout << "Experiment command requires subcommand." << endl;
            cout << "Available subcommands: run, list" << endl;
            return;
        }
        
        string subcmd = toLower(tokens[1]);
        
        if (subcmd == "run") {
            if (tokens.size() < 3) {
                cout << "Usage: experiment run <type>" << endl;
                cout << "Available types: basic, plasticity, stdp" << endl;
                return;
            }
            
            string type = tokens[2];
            runExperiment(type);
        } else if (subcmd == "list") {
            listExperiments();
        } else {
            cout << "Unknown experiment subcommand: " << subcmd << endl;
        }
    }
    
    void handleLogCommand(const vector<string>& tokens) {
        if (tokens.size() < 2) {
            cout << "Log command requires subcommand." << endl;
            cout << "Available subcommands: level, file, show" << endl;
            return;
        }
        
        string subcmd = toLower(tokens[1]);
        
        if (subcmd == "level") {
            if (tokens.size() < 3) {
                cout << "Current log level: " << Logger::getGlobal().getLevel() << endl;
                return;
            }
            
            string levelStr = tokens[2];
            LogLevel level;
            
            if (levelStr == "debug") level = LogLevel::Debug;
            else if (levelStr == "info") level = LogLevel::Info;
            else if (levelStr == "warning") level = LogLevel::Warning;
            else if (levelStr == "error") level = LogLevel::Error;
            else if (levelStr == "critical") level = LogLevel::Critical;
            else {
                cout << "Invalid log level: " << levelStr << endl;
                cout << "Available levels: debug, info, warning, error, critical" << endl;
                return;
            }
            
            Logger::getGlobal().setLevel(level);
            NLM_LOG_INFO("Log level set to: " + levelStr);
            cout << "Log level set to: " << levelStr << endl;
        } else if (subcmd == "file") {
            if (tokens.size() < 3) {
                cout << "Usage: log file <filepath>" << endl;
                return;
            }
            
            string filepath = tokens[2];
            // Get console logger and set output file
            auto loggers = Logger::getGlobal().getLoggers();
            for (auto& logger : loggers) {
                if (dynamic_cast<ConsoleLogger*>(logger.get())) {
                    static_cast<ConsoleLogger*>(logger.get())->setOutputFile(filepath);
                    break;
                }
            }
            NLM_LOG_INFO("Log file set to: " + filepath);
            cout << "Log file set to: " << filepath << endl;
        } else if (subcmd == "show") {
            showRecentLogs();
        } else {
            cout << "Unknown log subcommand: " << subcmd << endl;
        }
    }
    
    void handleAnalyzeCommand(const vector<string>& tokens) {
        if (tokens.size() < 2) {
            cout << "Analyze command requires subcommand." << endl;
            cout << "Available subcommands: stats, neurons, synapses" << endl;
            return;
        }
        
        string subcmd = toLower(tokens[1]);
        
        if (subcmd == "stats") {
            showStatistics();
        } else if (subcmd == "neurons") {
            analyzeNeurons();
        } else if (subcmd == "synapses") {
            analyzeSynapses();
        } else {
            cout << "Unknown analyze subcommand: " << subcmd << endl;
        }
    }
    
    void handleExportCommand(const vector<string>& tokens) {
        if (tokens.size() < 3) {
            cout << "Usage: export brain <filepath>" << endl;
            return;
        }
        
        string filepath = tokens[2];
        exportBrain(filepath);
    }
    
    void handleImportCommand(const vector<string>& tokens) {
        if (tokens.size() < 3) {
            cout << "Usage: import brain <filepath>" << endl;
            return;
        }
        
        string filepath = tokens[2];
        importBrain(filepath);
    }
    
    // Implementation of brain management functions
    void createBrain() {
        if (brain) {
            NLM_LOG_WARNING("Brain already exists. Resetting.");
            resetBrain();
        }
        
        brain = make_shared<Brain>(config);
        if (brain->initialize()) {
            NLM_LOG_INFO("Brain created and initialized successfully");
            cout << "Brain created and initialized successfully" << endl;
        } else {
            NLM_LOG_ERROR("Failed to initialize brain");
            cout << "Failed to initialize brain" << endl;
            brain.reset();
        }
    }
    
    void resetBrain() {
        if (brain) {
            brain->reset();
            brain->initialize();
            NLM_LOG_INFO("Brain reset successfully");
            cout << "Brain reset successfully" << endl;
        } else {
            NLM_LOG_WARNING("No brain to reset");
            cout << "No brain to reset" << endl;
        }
    }
    
    void showBrainStatus() {
        if (!brain) {
            cout << "No brain created yet." << endl;
            return;
        }
        
        brain->logStatus();
    }
    
    // Implementation of simulation management functions
    void startSimulation() {
        if (!brain) {
            cout << "No brain created. Create a brain first." << endl;
            return;
        }
        
        simulating = true;
        NLM_LOG_INFO("Simulation started");
        cout << "Simulation started" << endl;
    }
    
    void stopSimulation() {
        simulating = false;
        NLM_LOG_INFO("Simulation stopped");
        cout << "Simulation stopped" << endl;
    }
    
    void stepSimulation(int steps) {
        if (!brain) {
            cout << "No brain created. Create a brain first." << endl;
            return;
        }
        
        if (!simulating) {
            NLM_LOG_WARNING("Simulation not running. Starting...");
            simulating = true;
        }
        
        NLM_LOG_INFO("Running " + to_string(steps) + " simulation steps");
        cout << "Running " << steps << " simulation steps..." << endl;
        
        for (int i = 0; i < steps && simulating; ++i) {
            brain->step(i, i * config->getOr<double>("simulation_timestep", 0.001));
            
            if (i % 100 == 0) {
                cout << "Step " << i << "/" << steps << " (" << (100.0 * i / steps) << "%)" << endl;
            }
        }
        
        NLM_LOG_INFO("Simulation steps completed");
        cout << "Simulation steps completed" << endl;
    }
    
    // Implementation of neuron management functions
    void listNeurons() {
        if (!brain) {
            cout << "No brain created yet." << endl;
            return;
        }
        
        auto regions = brain->getRegionIds();
        for (const auto& regionId : regions) {
            auto* region = brain->getRegion(regionId);
            if (region) {
                auto neurons = region->getAllNeurons();
                cout << "Region " << regionId << ": " << neurons.size() << " neurons" << endl;
            }
        }
    }
    
    void showNeuronInfo(int id) {
        if (!brain) {
            cout << "No brain created yet." << endl;
            return;
        }
        
        auto regions = brain->getRegionIds();
        for (const auto& regionId : regions) {
            auto* region = brain->getRegion(regionId);
            if (region) {
                auto neurons = region->getAllNeurons();
                for (const auto& neuron : neurons) {
                    if (neuron->getId() == id) {
                        auto state = neuron->getState();
                        cout << "Neuron ID: " << id << endl;
                        cout << "  Type: " << static_cast<int>(neuron->getType()) << endl;
                        cout << "  Membrane Potential: " << state.membranePotential << " mV" << endl;
                        cout << "  Threshold: " << state.threshold << " mV" << endl;
                        cout << "  Firing State: " << static_cast<int>(state.firingState) << endl;
                        cout << "  Firing Rate: " << state.firingRate << " Hz" << endl;
                        cout << "  Last Spike Time: " << state.lastSpikeTime << " ms" << endl;
                        return;
                    }
                }
            }
        }
        
        cout << "Neuron not found: " << id << endl;
    }
    
    void injectCurrent(int id, float value) {
        if (!brain) {
            cout << "No brain created yet." << endl;
            return;
        }
        
        brain->injectCurrent(static_cast<NeuronId>(id), value);
        NLM_LOG_INFO("Injected current: neuron " + to_string(id) + " = " + to_string(value) + " nA");
        cout << "Injected current: neuron " << id << " = " << value << " nA" << endl;
    }
    
    // Implementation of synapse management functions
    void listSynapses() {
        if (!brain) {
            cout << "No brain created yet." << endl;
            return;
        }
        
        size_t totalSynapses = brain->getTotalSynapseCount();
        cout << "Total synapses: " << totalSynapses << endl;
    }
    
    void analyzeSynapses() {
        if (!brain) {
            cout << "No brain created yet." << endl;
            return;
        }
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            cout << "No regions found." << endl;
            return;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) {
            cout << "No synapses found." << endl;
            return;
        }
        
        double totalWeight = 0.0;
        double positiveSynapses = 0;
        double negativeSynapses = 0;
        
        for (const auto& synapse : synapses) {
            double weight = synapse->getWeight();
            totalWeight += weight;
            if (weight > 0) positiveSynapses++;
            else if (weight < 0) negativeSynapses++;
        }
        
        cout << "Synaptic Analysis:" << endl;
        cout << "  Total synapses: " << synapses.size() << endl;
        cout << "  Average weight: " << (synapses.size() > 0 ? totalWeight / synapses.size() : 0.0) << endl;
        cout << "  Positive synapses: " << positiveSynapses << endl;
        cout << "  Negative synapses: " << negativeSynapses << endl;
    }
    
    // Implementation of experiment functions
    void runExperiment(const string& type) {
        if (!brain) {
            cout << "No brain created yet." << endl;
            return;
        }
        
        if (type == "basic") {
            runBasicExperiment();
        } else if (type == "plasticity") {
            runPlasticityExperiment();
        } else if (type == "stdp") {
            runSTDPExperiment();
        } else {
            cout << "Unknown experiment type: " << type << endl;
        }
    }
    
    void listExperiments() {
        cout << "Available experiments:" << endl;
        cout << "  basic     Basic connectivity and activity" << endl;
        cout << "  plasticity Plasticity learning with Hebbian and STDP" << endl;
        cout << "  stdp      Spike-timing dependent plasticity verification" << endl;
    }
    
    void runBasicExperiment() {
        NLM_LOG_INFO("Running basic connectivity experiment");
        cout << "Running basic connectivity experiment..." << endl;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            cout << "No regions found." << endl;
            return;
        }
        
        // Inject current and check for activity
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            cout << "No neurons found." << endl;
            return;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into first 10 neurons
        size_t count = min(size_t(10), neurons.size());
        for (size_t i = 0; i < count; ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run simulation
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        cout << "Experiment completed: " << spikes << " spikes generated" << endl;
    }
    
    void runPlasticityExperiment() {
        NLM_LOG_INFO("Running plasticity experiment");
        cout << "Running plasticity experiment..." << endl;
        
        // Enable plasticity
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            cout << "No regions found." << endl;
            return;
        }
        
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
        
        // Record initial weights
        vector<double> initialWeights;
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
        
        // Apply input patterns
        for (SimulationStep step = 0; step < 100; ++step) {
            for (size_t i = 0; i < min(size_t(20), brain->getTotalNeuronCount() / 4); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        // Analyze changes
        double totalChange = 0.0;
        size_t changedCount = 0;
        
        for (size_t i = 0; i < min(initialWeights.size(), region->getSynapses().size()); ++i) {
            double newWeight = region->getSynapses()[i]->getWeight();
            double change = newWeight - initialWeights[i];
            if (abs(change) > 0.01) {
                changedCount++;
                totalChange += abs(change);
            }
        }
        
        cout << "Plasticity experiment completed:" << endl;
        cout << "  Synapses changed: " << changedCount << " out of " << min(initialWeights.size(), region->getSynapses().size()) << endl;
        cout << "  Total weight change: " << totalChange << endl;
    }
    
    void runSTDPExperiment() {
        NLM_LOG_INFO("Running STDP verification experiment");
        cout << "Running STDP verification experiment..." << endl;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            cout << "No regions found." << endl;
            return;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            cout << "Not enough synapses for STDP test." << endl;
            return;
        }
        
        // Record initial weights
        vector<double> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
        }
        
        // Create correlated activity
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            for (int trial = 0; trial < 20; ++trial) {
                // Fire pre-synaptic neuron
                neurons[0]->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post-synaptic neuron
                neurons[1]->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Check for potentiation
        double totalDelta = 0.0;
        for (size_t i = 0; i < 5; ++i) {
            double delta = synapses[i]->getWeight() - beforeWeights[i];
            totalDelta += delta;
        }
        
        cout << "STDP experiment completed:" << endl;
        if (totalDelta > 0.001) {
            cout << "  LTP observed: pre-before-post produced potentiation (Δ = " << totalDelta << ")" << endl;
        } else if (totalDelta < -0.001) {
            cout << "  LTD observed: post-before-pre produced depression (Δ = " << totalDelta << ")" << endl;
        } else {
            cout << "  No significant change: STDP may not be triggering" << endl;
        }
    }
    
    // Implementation of logging functions
    void showRecentLogs() {
        // In a real implementation, we would show the last N log entries
        // For now, just show a message
        NLM_LOG_INFO("Recent logs would be displayed here");
        cout << "Recent logs would be displayed here (in a real implementation)" << endl;
    }
    
    // Implementation of analysis functions
    void showStatistics() {
        if (!brain) {
            cout << "No brain created yet." << endl;
            return;
        }
        
        cout << "NLM Performance Statistics:" << endl;
        cout << "  Total neurons: " << brain->getTotalNeuronCount() << endl;
        cout << "  Total synapses: " << brain->getTotalSynapseCount() << endl;
        cout << "  Total spikes: " << brain->getTotalSpikeCount() << endl;
        cout << "  Active neurons: " << brain->getActiveNeuronCount() << endl;
        cout << "  Firing neurons: " << brain->getFiringNeuronCount() << endl;
        cout << "  Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << endl;
        cout << "  Pending spike events: " << brain->getPendingSpikeEventCount() << endl;
    }
    
    void analyzeNeurons() {
        if (!brain) {
            cout << "No brain created yet." << endl;
            return;
        }
        
        cout << "Neuron Population Analysis:" << endl;
        auto regions = brain->getRegionIds();
        for (const auto& regionId : regions) {
            auto* region = brain->getRegion(regionId);
            if (region) {
                auto neurons = region->getAllNeurons();
                cout << "  Region " << regionId << ": " << neurons.size() << " neurons" << endl;
                
                // Count neuron types
                map<NeuronType, size_t> typeCounts;
                for (const auto& neuron : neurons) {
                    typeCounts[neuron->getType()]++;
                }
                
                for (const auto& pair : typeCounts) {
                    cout << "    " << static_cast<int>(pair.first) << ": " << pair.second << " neurons" << endl;
                }
            }
        }
    }
    
    // Implementation of export/import functions
    void exportBrain(const string& filepath) {
        if (!brain) {
            cout << "No brain to export." << endl;
            return;
        }
        
        if (brain->save(filepath)) {
            NLM_LOG_INFO("Brain exported to: " + filepath);
            cout << "Brain exported to: " << filepath << endl;
        } else {
            NLM_LOG_ERROR("Failed to export brain to: " + filepath);
            cout << "Failed to export brain to: " << filepath << endl;
        }
    }
    
    void importBrain(const string& filepath) {
        if (brain) {
            NLM_LOG_WARNING("Brain already exists. Overwriting.");
        }
        
        auto newBrain = make_shared<Brain>(config);
        if (newBrain->load(filepath)) {
            brain = newBrain;
            NLM_LOG_INFO("Brain imported from: " + filepath);
            cout << "Brain imported from: " << filepath << endl;
        } else {
            NLM_LOG_ERROR("Failed to import brain from: " + filepath);
            cout << "Failed to import brain from: " << filepath << endl;
        }
    }
    
    // Additional member variables
    shared_ptr<Config> config;
    shared_ptr<Brain> brain;
    bool simulating = false;
};

int main() {
    try {
        NLMCLI cli;
        cli.run();
        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown error occurred" << endl;
        return 1;
    }
}