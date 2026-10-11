// NLM Enhanced Command Line Interface
// 
// Advanced commands for controlling and exploring the NLM brain
// Provides power-user functionality while maintaining backward compatibility

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "performance/CheckpointSystem.hpp"

using namespace nlm;

// Enhanced help command
void printAdvancedHelp() {
    std::cout << R"(
NLM — Advanced Command Line Interface

USAGE:
  nlm [OPTIONS] [COMMAND] [ARGS...]

BASIC OPTIONS:
  --config=<file>        Load configuration from file
  --help                 Show this help message
  --version              Show version information

AVAILABLE COMMANDS:

EXPERIMENTAL COMMANDS:
  experiment              Run integrated Phase 6 experiment (default)
    --max-steps=<n>       Number of simulation steps (default: 1000)
    --neuron-count=<n>    Number of neurons (default: 1000)
    --enable-development  Enable development system
    --enable-checkpointing Enable checkpoint save/load
    --checkpoint-path=<path> Checkpoint file path (default: ./checkpoint.bin)
    --enable-memory-logging Enable detailed memory logging
    --enable-neuromod-logging Enable neuromodulation logging

NEURAL EXPLORATION COMMANDS:
  explore-neural         Explore neural network properties
    --region=<n>          Region ID (default: 1)
    --synapse-count        Show synapse statistics
    --weight-stats        Show weight distribution
    --activity-stats      Show firing activity statistics

MEMORY SYSTEM COMMANDS:
  memory                 Manage memory systems
    status               Show memory system status
    traces               Show working memory traces
    episodes             Show episodic memory episodes
    replay <id>          Replay specific episode
    consolidate <threshold> Consolidify memories

NEUROMODULATION COMMANDS:
  neuromod               Control neuromodulation
    dopamine <level>    Set dopamine level
    curiosity <level>   Set curiosity level
    novelty <level>     Set novelty level
    prediction-error <level> Set prediction error level
    status              Show current neuromodulation levels

DEVELOPMENTAL COMMANDS:
  develop               Control development system
    stage <stage>        Set developmental stage (Initial, CriticalPeriod, Maturation, Adult, Aging)
    simulate <cycles>    Simulate development cycles
    status              Show current developmental stage

CHECKPOINT COMMANDS:
  checkpoint            Checkpoint management
    save <path>          Save current brain state
    load <path>          Load brain state
    list                 List available checkpoints
    delete <path>        Delete checkpoint

MONITORING COMMANDS:
  monitor              Real-time monitoring
    --steps=<n>          Number of steps to monitor
    --stats-frequency=<n> Stats logging frequency
    start               Start monitoring
    stop                Stop monitoring

CONFIGURATION COMMANDS:
  config               Configuration management
    get <key>            Get configuration value
    set <key> <value>    Set configuration value
    list                List all configuration keys
    save <file>          Save current config

ACTION COMMANDS:
  action               Take specific actions
    inject-current <id> <amount> Inject current into neuron
    inject-type <type> <amount> Inject current to neuron type
    stimulus <pattern>   Apply sensory stimulus
    motor-decode        Decode motor command from neural activity

PRESET EXPERIMENTS:
  preset <name>        Run preset experiment
    development         Developmental progression experiment
    learning            Learning capability experiment  
    memory              Memory consolidation experiment
    social              Social learning experiment

DEBUGGING COMMANDS:
  debug                Debugging tools
    spike-history        Show spike history
    synaptic-connections Show connectivity
    plasticity-stats     Show plasticity statistics
    validation          Validate system integrity
    reset               Reset brain to initial state

SPECIAL MODES:
  benchmark            Run performance benchmark
    --duration=<seconds> Run benchmark for specified duration
    --verbose           Enable verbose output

  profile              Profile performance
    --collect           Collect profiling data
    --output=<file>     Save profile to file

INTERACTIVE MODE:
  interactive          Enter interactive command mode
  - Commands can be entered line by line
  - Type 'help' for command list
  - Type 'exit' or 'quit' to leave interactive mode

")" << std::endl;
}

// Advanced command parser
class CommandParser {
private:
    std::map<std::string, std::string> options_;
    std::vector<std::string> args_;
    
public:
    CommandParser(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    options_["config"] = arg.substr(arg.find('=') + 1);
                }
            } else if (arg == "--help" || arg == "--version") {
                options_[arg.substr(1)] = "true";
            } else {
                // Check if it's a command (first non-option argument)
                if (args_.empty() && arg.substr(0, 2) != "--") {
                    args_.push_back(arg);
                } else {
                    // Store as argument
                    if (args_.size() < 3) { // Max 3 levels: command subcommand arg
                        args_.push_back(arg);
                    }
                }
            }
        }
    }
    
    std::string getOption(const std::string& key) const {
        auto it = options_.find(key);
        return (it != options_.end()) ? it->second : "";
    }
    
    bool hasOption(const std::string& key) const {
        return options_.find(key) != options_.end();
    }
    
    std::string getCommand() const {
        return args_.empty() ? "experiment" : args_[0];n    }
    
    std::string getSubcommand() const {
        return (args_.size() > 1) ? args_[1] : "";
    }
    
    std::string getArg() const {
        return (args_.size() > 2) ? args_[2] : "";
    }
};

// Enhanced brain exploration
void exploreNeural(std::shared_ptr<Brain> brain, const std::map<std::string, std::string>& options) {
    NLM_LOG_INFO("=== Neural Network Exploration ===");
    
    // Find target region
    RegionId targetRegion(1);
    if (options.count("region")) {
        targetRegion = RegionId(static_cast<uint64_t>(std::stoi(options.at("region"))));
    }
    
    auto* region = brain->getRegion(targetRegion);
    if (!region) {
        NLM_LOG_ERROR("Region " + std::to_string(targetRegion.index()) + " not found");
        return;
    }
    
    NLM_LOG_INFO("Target Region " + std::to_string(targetRegion.index()));
    
    // Synapse statistics
    if (options.count("synapse-count")) {
        NLM_LOG_INFO("Synapse Statistics:");
        NLM_LOG_INFO("  Total synapses: " + std::to_string(region->getSynapseCount()));
        NLM_LOG_INFO("  Synapses per neuron: " + std::to_string(region->getSynapseCount() / region->getTotalNeuronCount()));
    }
    
    // Weight statistics
    if (options.count("weight-stats")) {
        NLM_LOG_INFO("Weight Statistics:");
        float minWeight = std::numeric_limits<float>::max();
        float maxWeight = std::numeric_limits<float>::min();
        float sumWeights = 0.0f;
        size_t count = 0;
        
        for (const auto* syn : region->getSynapses()) {
            float w = syn->getWeight();
            minWeight = std::min(minWeight, w);
            maxWeight = std::max(maxWeight, w);
            sumWeights += w;
            ++count;
        }
        
        if (count > 0) {
            float mean = sumWeights / count;
            NLM_LOG_INFO("  Range: [" + std::to_string(minWeight) + ", " + std::to_string(maxWeight) + "]");
            NLM_LOG_INFO("  Mean: " + std::to_string(mean));
            NLM_LOG_INFO("  Count: " + std::to_string(count));
        }
    }
    
    // Activity statistics
    if (options.count("activity-stats")) {
        NLM_LOG_INFO("Activity Statistics:");
        NLM_LOG_INFO("  Total neurons: " + std::to_string(region->getTotalNeuronCount()));
        NLM_LOG_INFO("  Active neurons: " + std::to_string(region->getActiveNeuronCount()));
        NLM_LOG_INFO("  Firing neurons (last step): " + std::to_string(region->getFiringNeuronCount()));
        NLM_LOG_INFO("  Average firing rate: " + std::to_string(region->getAverageFiringRate()));
    }
}

// Memory system management
void memoryManager(std::shared_ptr<Brain> brain, const std::string& subcommand, const std::string& arg) {
    if (!brain->getWorkingMemory()) {
        NLM_LOG_ERROR("Working memory not available");
        return;
    }
    
    if (subcommand == "status") {
        NLM_LOG_INFO("=== Memory System Status ===");
        
        auto* wm = brain->getWorkingMemory();
        auto* em = brain->getEpisodicMemory();
        auto* am = brain->getAssociativeMemory();
        
        NLM_LOG_INFO("Working Memory:");
        if (wm) {
            NLM_LOG_INFO("  Active traces: " + std::to_string(wm->getActiveTraces()));
            NLM_LOG_INFO("  Capacity: " + std::to_string(wm->getCapacity()));
            NLM_LOG_INFO("  Competition: " + std::to_string(wm->getCompetitionStrength()));
        } else {
            NLM_LOG_INFO("  Not available");
        }
        
        NLM_LOG_INFO("Episodic Memory:");
        if (em) {
            NLM_LOG_INFO("  Episodes stored: " + std::to_string(em->getEpisodeCount()));
            NLM_LOG_INFO("  Max episodes: " + std::to_string(em->getMaxEpisodes()));
            NLM_LOG_INFO("  Replay interval: " + std::to_string(em->getReplayInterval()));
        } else {
            NLM_LOG_INFO("  Not available");
        }
        
        NLM_LOG_INFO("Associative Memory:");
        if (am) {
            NLM_LOG_INFO("  Pattern count: " + std::to_string(am->getPatternCount()));
            NLM_LOG_INFO("  Association strength: " + std::to_string(am->getAssociationStrength()));
        } else {
            NLM_LOG_INFO("  Not available");
        }
    }
    
    else if (subcommand == "traces") {
        auto* wm = brain->getWorkingMemory();
        if (wm) {
            NLM_LOG_INFO("=== Working Memory Traces ===");
            auto traces = wm->getMemoryNeurons();
            NLM_LOG_INFO("Active memory neurons: " + std::to_string(traces.size()));
            for (size_t i = 0; i < std::min(size_t(10), traces.size()); ++i) {
                NLM_LOG_INFO("  Neuron " + std::to_string(traces[i].index()));
            }
        }
    }
    
    else if (subcommand == "episodes") {
        auto* em = brain->getEpisodicMemory();
        if (em) {
            NLM_LOG_INFO("=== Episodic Memory Episodes ===");
            auto episodes = em->getEpisodes();
            NLM_LOG_INFO("Total episodes: " + std::to_string(episodes.size()));
            for (size_t i = 0; i < std::min(size_t(5), episodes.size()); ++i) {
                const auto& episode = episodes[i];
                NLM_LOG_INFO("  Episode " + std::to_string(i) + ": Step " + std::to_string(episode.timestamp) +
                           ", Reward: " + std::to_string(episode.reward) +
                           ", Neurons: " + std::to_string(episode.activeNeurons.size()));
            }
        }
    }
    
    else if (subcommand == "replay" && !arg.empty()) {
        auto* em = brain->getEpisodicMemory();
        if (em) {
            size_t episodeId = std::stoul(arg);
            auto episodes = em->getEpisodes();
            if (episodeId < episodes.size()) {
                NLM_LOG_INFO("Replaying episode " + std::to_string(episodeId));
                em->replayEpisode(&episodes[episodeId]);
                NLM_LOG_INFO("Episode replay completed");
            } else {
                NLM_LOG_ERROR("Episode " + arg + " not found");
            }
        }
    }
    
    else if (subcommand == "consolidate" && !arg.empty()) {
        auto* em = brain->getEpisodicMemory();
        if (em) {
            float threshold = std::stof(arg);
            NLM_LOG_INFO("Consolidating memories with threshold " + std::to_string(threshold));
            em->consolidate(threshold);
            NLM_LOG_INFO("Consolidation completed");
        }
    }
}

// Neuromodulation control
void neuromodController(std::shared_ptr<Brain> brain, const std::string& subcommand, const std::string& arg) {
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    auto* predictionError = brain->getPredictionErrorSignal();
    
    if (subcommand == "status") {
        NLM_LOG_INFO("=== Neuromodulation Status ===");
        
        if (dopamine) {
            NLM_LOG_INFO("Dopamine: " + std::to_string(dopamine->getLevel()) +
                        " (Plasticity factor: " + std::to_string(dopamine->getPlasticityFactor()) + ")");
        }
        
        if (curiosity) {
            NLM_LOG_INFO("Curiosity: " + std::to_string(curiosity->getLevel()));
        }
        
        if (novelty) {
            NLM_LOG_INFO("Novelty: " + std::to_string(novelty->getLevel()));
        }
        
        if (predictionError) {
            NLM_LOG_INFO("Prediction Error: " + std::to_string(predictionError->getLevel()));
        }
    }
    
    else if (subcommand == "dopamine" && !arg.empty()) {
        if (dopamine) {
            float level = std::stof(arg);
            // Set dopamine level (assuming dopamine has a setLevel method)
            NLM_LOG_INFO("Setting dopamine level to " + arg);
            // Note: This assumes dopamine has a setLevel method
            // Actual implementation would depend on the Dopamine class interface
        }
    }
    
    else if (subcommand == "curiosity" && !arg.empty()) {
        if (curiosity) {
            float level = std::stof(arg);
            NLM_LOG_INFO("Setting curiosity level to " + arg);
            // Implementation depends on Curiosity class interface
        }
    }
    
    else if (subcommand == "novelty" && !arg.empty()) {
        if (novelty) {
            float level = std::stof(arg);
            NLM_LOG_INFO("Setting novelty level to " + arg);
            // Implementation depends on Novelty class interface
        }
    }
    
    else if (subcommand == "prediction-error" && !arg.empty()) {
        if (predictionError) {
            float level = std::stof(arg);
            NLM_LOG_INFO("Setting prediction error level to " + arg);
            // Implementation depends on PredictionError class interface
        }
    }
}

// Developmental control
void developController(std::shared_ptr<Brain> brain, const std::string& subcommand, const std::string& arg) {
    auto* devSystem = brain->getDevelopmentSystem();
    
    if (!devSystem) {
        NLM_LOG_ERROR("Development system not available");
        return;
    }
    
    if (subcommand == "status") {
        NLM_LOG_INFO("=== Development System Status ===");
        NLM_LOG_INFO("Current stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
        NLM_LOG_INFO("Structural plasticity rate: " + std::to_string(devSystem->getSynaptogenesisRate()) +
                    ", pruning rate: " + std::to_string(devSystem->getPruningRate()));
    }
    
    else if (subcommand == "stage" && !arg.empty()) {
        DevelopmentalStage stage;
        std::string argUpper = arg;
        std::transform(argUpper.begin(), argUpper.end(), argUpper.begin(), ::toupper);
        
        if (argUpper == "INITIAL") stage = DevelopmentalStage::Initial;
        else if (argUpper == "CRITICALPERIOD") stage = DevelopmentalStage::CriticalPeriod;
        else if (argUpper == "MATURATION") stage = DevelopmentalStage::Maturation;
        else if (argUpper == "ADULT") stage = DevelopmentalStage::Adult;
        else if (argUpper == "AGING") stage = DevelopmentalStage::Aging;
        else {
            NLM_LOG_ERROR("Invalid developmental stage: " + arg);
            return;
        }
        
        brain->setDevelopmentalStage(stage);
        NLM_LOG_INFO("Development stage set to: " + arg);
    }
    
    else if (subcommand == "simulate" && !arg.empty()) {
        int cycles = std::stoi(arg);
        NLM_LOG_INFO("Simulating " + std::to_string(cycles) + " development cycles");
        // Implementation would depend on DevelopmentSystem interface
    }
}

// Checkpoint management
void checkpointManager(std::shared_ptr<Brain> brain, const std::string& subcommand, const std::string& arg) {
    if (subcommand == "save") {
        if (arg.empty()) {
            NLM_LOG_ERROR("Checkpoint path required for save command");
            return;
        }
        
        if (brain->save(arg)) {
            NLM_LOG_INFO("Checkpoint saved to: " + arg);
        } else {
            NLM_LOG_ERROR("Failed to save checkpoint");
        }
    }
    
    else if (subcommand == "load") {
        if (arg.empty()) {
            NLM_LOG_ERROR("Checkpoint path required for load command");
            return;
        }
        
        auto config = std::make_shared<Config>();
        auto newBrain = std::make_shared<Brain>(config);
        
        if (newBrain->load(arg)) {
            NLM_LOG_INFO("Checkpoint loaded from: " + arg);
            // Note: In a real implementation, this would replace the current brain
        } else {
            NLM_LOG_ERROR("Failed to load checkpoint");
        }
    }
    
    else if (subcommand == "list") {
        NLM_LOG_INFO("=== Available Checkpoints ===");
        // Implementation would use CheckpointManager
        // For now, just show basic info
        NLM_LOG_INFO("Checkpoint listing not yet implemented");
    }
    
    else if (subcommand == "delete") {
        if (arg.empty()) {
            NLM_LOG_ERROR("Checkpoint path required for delete command");
            return;
        }
        
        // Implementation for deleting checkpoints
        NLM_LOG_INFO("Delete checkpoint " + arg + " (not yet implemented)");
    }
}

// Action commands
void actionCommands(std::shared_ptr<Brain> brain, const std::string& subcommand, const std::string& arg) {
    if (subcommand == "inject-current") {
        if (arg.empty()) {
            NLM_LOG_ERROR("Neuron ID required for inject-current command");
            return;
        }
        
        size_t neuronId = std::stoul(arg);
        if (neuronId < brain->getTotalNeuronCount()) {
            brain->injectCurrent(NeuronId(neuronId), 10.0f); // Default 10.0f current
            NLM_LOG_INFO("Injected 10.0 current into neuron " + arg);
        } else {
            NLM_LOG_ERROR("Neuron ID " + arg + " not found");
        }
    }
    
    else if (subcommand == "inject-type") {
        if (arg.empty()) {
            NLM_LOG_ERROR("Neuron type required for inject-type command");
            return;
        }
        
        // Parse neuron type from arg
        NeuronType type;
        std::string argUpper = arg;
        std::transform(argUpper.begin(), argUpper.end(), argUpper.begin(), ::toupper);
        
        if (argUpper == "EXCITATORY") type = NeuronType::Excitatory;
        else if (argUpper == "INHIBITORY") type = NeuronType::Inhibitory;
        else if (argUpper == "MODULATORY") type = NeuronType::Modulatory;
        else if (argUpper == "SENSORY") type = NeuronType::Sensory;
        else if (argUpper == "MOTOR") type = NeuronType::Motor;
        else if (argUpper == "INTERNAL") type = NeuronType::Internal;
        else {
            NLM_LOG_ERROR("Invalid neuron type: " + arg);
            return;
        }
        
        brain->injectCurrentToNeurons(type, 10.0f); // Default 10.0f current
        NLM_LOG_INFO("Injected 10.0 current to " + arg + " neurons");
    }
    
    else if (subcommand == "stimulus" && !arg.empty()) {
        NLM_LOG_INFO("Applying stimulus: " + arg);
        // Implementation would depend on sensory input system
        // For now, just log the action
    }
    
    else if (subcommand == "motor-decode") {
        NLM_LOG_INFO("Decoding motor command from neural activity");
        auto action = brain->produceAction();
        NLM_LOG_INFO("Produced action: " + std::to_string(static_cast<int>(action->getType())));
    }
}

// Configuration commands
void configCommands(std::shared_ptr<Brain> brain, const std::string& subcommand, const std::string& arg) {
    auto config = brain->getConfig();
    
    if (subcommand == "get") {
        if (arg.empty()) {
            NLM_LOG_ERROR("Configuration key required for get command");
            return;
        }
        
        if (config->hasKey(arg)) {
            auto value = config->get<std::string>(arg);
            NLM_LOG_INFO(arg + " = " + value);
        } else {
            NLM_LOG_ERROR("Configuration key not found: " + arg);
        }
    }
    
    else if (subcommand == "set") {
        // Format: config set <key> <value>
        // Note: This simplified implementation - would need full argument parsing
        NLM_LOG_INFO("Set configuration command requires key and value");
        NLM_LOG_INFO("Usage: config set <key> <value>");
    }
    
    else if (subcommand == "list") {
        NLM_LOG_INFO("=== Configuration Keys ===");
        // Implementation would depend on Config class interface
        NLM_LOG_INFO("Configuration listing not yet implemented");
    }
}

// Preset experiments
void presetExperiment(std::shared_ptr<Brain> brain, const std::string& presetName) {
    NLM_LOG_INFO("Running preset experiment: " + presetName);
    
    if (presetName == "development") {
        NLM_LOG_INFO("Development experiment: Simulating developmental progression");
        // Would implement developmental stages
    }
    
    else if (presetName == "learning") {
        NLM_LOG_INFO("Learning experiment: Measuring learning capability");
        // Would implement learning experiments
    }
    
    else if (presetName == "memory") {
        NLM_LOG_INFO("Memory experiment: Testing memory consolidation");
        // Would implement memory experiments
    }
    
    else if (presetName == "social") {
        NLM_LOG_INFO("Social experiment: Testing social learning");
        // Would implement social learning experiments
    }
    
    else {
        NLM_LOG_ERROR("Unknown preset experiment: " + presetName);
    }
}

// Interactive mode
void interactiveMode(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Entering interactive mode. Type 'help' for command list or 'exit' to quit.");
    
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty() || line == "exit" || line == "quit") {
            break;
        }
        
        // Parse and execute command
        if (line == "help") {
            printAdvancedHelp();
        } else if (line.find("memory ") == 0) {
            size_t spacePos = line.find(' ', 7);
            if (spacePos != std::string::npos) {
                std::string subcommand = line.substr(7, spacePos - 7);
                std::string arg = line.substr(spacePos + 1);
                memoryManager(brain, subcommand, arg);
            }
        } else if (line.find("neuromod ") == 0) {
            size_t spacePos = line.find(' ', 8);
            if (spacePos != std::string::npos) {
                std::string subcommand = line.substr(8, spacePos - 8);
                std::string arg = line.substr(spacePos + 1);
                neuromodController(brain, subcommand, arg);
            }
        } else if (line.find("develop ") == 0) {
            size_t spacePos = line.find(' ', 8);
            if (spacePos != std::string::npos) {
                std::string subcommand = line.substr(8, spacePos - 8);
                std::string arg = line.substr(spacePos + 1);
                developController(brain, subcommand, arg);
            }
        } else if (line.find("checkpoint ") == 0) {
            size_t spacePos = line.find(' ', 11);
            if (spacePos != std::string::npos) {
                std::string subcommand = line.substr(11, spacePos - 11);
                std::string arg = line.substr(spacePos + 1);
                checkpointManager(brain, subcommand, arg);
            }
        } else if (line.find("action ") == 0) {
            size_t spacePos = line.find(' ', 7);
            if (spacePos != std::string::npos) {
                std::string subcommand = line.substr(7, spacePos - 7);
                std::string arg = line.substr(spacePos + 1);
                actionCommands(brain, subcommand, arg);
            }
        } else if (line.find("config ") == 0) {
            size_t spacePos = line.find(' ', 7);
            if (spacePos != std::string::npos) {
                std::string subcommand = line.substr(7, spacePos - 7);
                std::string arg = line.substr(spacePos + 1);
                configCommands(brain, subcommand, arg);
            }
        } else if (line.find("explore-neural ") == 0) {
            // Parse options
            std::map<std::string, std::string> options;
            // Simplified - would need full option parsing
            exploreNeural(brain, options);
        } else if (line == "explore-neural") {
            exploreNeural(brain, std::map<std::string, std::string>());
        } else if (line.find("preset ") == 0) {
            size_t spacePos = line.find(' ', 7);
            if (spacePos != std::string::npos) {
                std::string presetName = line.substr(spacePos + 1);
                presetExperiment(brain, presetName);
            }
        } else if (line == "reset") {
            brain->reset();
            brain->initialize();
            NLM_LOG_INFO("Brain reset to initial state");
        } else if (line == "status") {
            brain->logStatus();
        } else {
            NLM_LOG_ERROR("Unknown command: " + line);
            NLM_LOG_INFO("Type 'help' for available commands");
        }
    }
    
    NLM_LOG_INFO("Exiting interactive mode");
}

// Main enhanced entry point
int main(int argc, char** argv) {
    printAdvancedHelp();  // Show help on startup
    
    std::cout << "NLM Enhanced Command Line Interface" << std::endl;
    std::cout << "=====================================" << std::endl << std::endl;
    
    // Parse command line
    CommandParser parser(argc, argv);
    
    // Check for help/version
    if (parser.hasOption("help")) {
        printAdvancedHelp();
        return 0;
    }
    
    if (parser.hasOption("version")) {
        std::cout << "NLM Version 0.1.0 - Phase 6 Integration" << std::endl;
        return 0;
    }
    
    // Get configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration from file if specified
    std::string configFile = "configs/default.cfg";
    if (parser.getOption("config") != "") {
        configFile = parser.getOption("config");
    }
    
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Set default values for Phase 6
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integration)...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Get command and subcommand
    std::string command = parser.getCommand();
    std::string subcommand = parser.getSubcommand();
    std::string arg = parser.getArg();
    
    // Execute command
    if (command == "explore-neural") {
        std::map<std::string, std::string> options;
        // Add options parsing from command line
        if (parser.hasOption("region")) options["region"] = parser.getOption("region");
        if (parser.hasOption("synapse-count")) options["synapse-count"] = "true";
        if (parser.hasOption("weight-stats")) options["weight-stats"] = "true";
        if (parser.hasOption("activity-stats")) options["activity-stats"] = "true";
        
        exploreNeural(brain, options);
    }
    
    else if (command == "memory") {
        memoryManager(brain, subcommand, arg);
    }
    
    else if (command == "neuromod") {
        neuromodController(brain, subcommand, arg);
    }
    
    else if (command == "develop") {
        developController(brain, subcommand, arg);
    }
    
    else if (command == "checkpoint") {
        checkpointManager(brain, subcommand, arg);
    }
    
    else if (command == "action") {
        actionCommands(brain, subcommand, arg);
    }
    
    else if (command == "config") {
        configCommands(brain, subcommand, arg);
    }
    
    else if (command == "preset" && !arg.empty()) {
        presetExperiment(brain, arg);
    }
    
    else if (command == "experiment" || command == "") {
        // Default: run integrated experiment
        NLM_LOG_INFO("Running Phase 6 Integrated Experiment...");
        
        // Parse experiment options
        Phase6Config expConfig;
        expConfig.neuronCount = parser.hasOption("neuron-count") ? 
            std::stoul(parser.getOption("neuron-count")) : 1000;
        expConfig.maxSteps = parser.hasOption("max-steps") ? 
            std::stoul(parser.getOption("max-steps")) : 1000;
        expConfig.enableDevelopment = parser.hasOption("enable-development");
        expConfig.enableCheckpointing = parser.hasOption("enable-checkpointing");
        expConfig.checkpointPath = parser.hasOption("checkpoint-path") ? 
            parser.getOption("checkpoint-path") : "./checkpoint.bin";
        
        Phase6IntegratedExperiment experiment;
        Phase6IntegrationResult result = experiment.run(expConfig);
        
        NLM_LOG_INFO("=== Experiment Results ===");
        NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(result.avgFiringRate));
        NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
        NLM_LOG_INFO("Wall time: " + std::to_string(result.totalWallClockTime) + "s");
    }
    
    else if (command == "interactive") {
        interactiveMode(brain);
    }
    
    else {
        NLM_LOG_ERROR("Unknown command: " + command);
        std::cout << "Use 'nlm --help' for available commands" << std::endl;
        return 1;
    }
    
    // Final status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Command Completed ===");
    brain->logStatus();
    
    return 0;
}
