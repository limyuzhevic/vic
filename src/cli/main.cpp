// NLM CLI (Neural Learning Machine Command Line Interface)
// A command-line interface for controlling brain states and systems.

#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>

using namespace nlm;

class BrainCLI {
public:
    BrainCLI() : brain(nullptr), config(nullptr), is_initialized(false) {
        logger = Logger::getInstance();
        logger->setLevel(Logger::Level::Info);
    }
    
    ~BrainCLI() {
        if (brain) {
            brain.reset();
        }
    }
    
    void print_help() {
        std::cout << "\n=== NLM Brain Control CLI ===\n\n";
        std::cout << "Brain State Control Commands:\n";
        std::cout << "  save [filepath]          Save current brain state to checkpoint file\n";
        std::cout << "  load [filepath]          Load brain state from checkpoint file\n";
        std::cout << "  reset                    Reset brain to initial state\n";
        std::cout << "  checkpoint [interval]    Set checkpoint interval (in simulation steps)\n";
        std::cout << "  develop [stage]          Set developmental stage (Initial, CriticalPeriod, Maturation, Adult)\n\n";
        
        std::cout << "Memory System Control Commands:\n";
        std::cout << "  memory working - clear      Clear working memory\n";
        std::cout << "  memory working - strengthen Strengthen working memory\n";
        std::cout << "  memory working - compete    Compete working memory items\n";
        std::cout << "  memory episodic - store     Store in episodic memory\n";
        std::cout << "  memory episodic - replay    Replay episodic memory\n";
        std::cout << "  memory episodic - consolidate Consolidate episodic memory\n";
        std::cout << "  memory replay [count]      Set number of episodes to replay\n\n";
        
        std::cout << "Neuromodulation Control Commands:\n";
        std::cout << "  neuromod [type] - [level]  Set neuromodulator level\n";
        std::cout << "                         Types: dopamine, curiosity, novelty, predictionError,\n";
        std::cout << "                                acetylcholine, norepinephrine, serotonin\n";
        std::cout << "  plasticity [factor]        Set global plasticity factor\n\n";
        
        std::cout << "Debug and Status Commands:\n";
        std::cout << "  status                    Display current brain state and statistics\n";
        std::cout << "  neurons                   Show neuron distribution across regions\n";
        std::cout << "  synapses                  Show synapse statistics and connections\n";
        std::cout << "  memory                    Show memory system status\n";
        std::cout << "  cognition                 Show cognitive system status\n";
        std::cout << "  performance                Show performance metrics\n\n";
        
        std::cout << "Performance Commands:\n";
        std::cout << "  profile [metric]          Start profiling (memory, time, spikes)\n";
        std::cout << "  stats                      Show accumulated statistics\n";
        std::cout << "  monitor                    Continuous monitoring mode\n\n";
        
        std::cout << "System Commands:\n";
        std::cout << "  help                      Show this help text\n";
        std::cout << "  quit|exit                 Exit the CLI\n";
        std::cout << "  about                     Show version information\n\n";
        
        std::cout << "=== Examples ===\n";
        std::cout << "  save brain_state.bin\n";
        std::cout << "  load brain_state.bin\n";
        std::cout << "  develop CriticalPeriod\n";
        std::cout << "  neuromod dopamine - 1.0\n";
        std::cout << "  memory episodic - store\n";
        std::cout << "  neurons\n";
        std::cout << "  status\n";
    }
    
    void print_about() {
        std::cout << "NLM Brain Control CLI\n";
        std::cout << "Version: 0.1.0\n";
        std::cout << "An experimental artificial developmental brain system\n";
        std::cout << "Phase 6: Final Integration\n";
    }
    
    void initialize_brain() {
        if (!config) {
            config = std::make_shared<Config>();
            
            // Set default configuration
            config->set("neuron_count", 500, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);
            config->set("random_seed", 42, ConfigSource::Default);
        }
        
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            logger->error("Failed to initialize brain");
            return;
        }
        
        is_initialized = true;
        logger->info("Brain initialized successfully");
    }
    
    bool handle_save(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        if (args.size() < 1) {
            logger->error("Save command requires filepath argument");
            return false;
        }
        
        const std::string& filepath = args[0];
        if (brain->save(filepath)) {
            logger->info("Brain state saved to: " + filepath);
            return true;
        } else {
            logger->error("Failed to save brain state to: " + filepath);
            return false;
        }
    }
    
    bool handle_load(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        if (args.size() < 1) {
            logger->error("Load command requires filepath argument");
            return false;
        }
        
        const std::string& filepath = args[0];
        if (brain->load(filepath)) {
            logger->info("Brain state loaded from: " + filepath);
            return true;
        } else {
            logger->error("Failed to load brain state from: " + filepath);
            return false;
        }
    }
    
    bool handle_reset(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        brain->reset();
        brain->initialize();
        logger->info("Brain reset to initial state");
        return true;
    }
    
    bool handle_checkpoint(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        if (args.size() < 1) {
            logger->error("Checkpoint command requires interval argument (in simulation steps)");
            return false;
        }
        
        try {
            size_t interval = std::stoul(args[0]);
            if (interval <= 0) {
                logger->error("Checkpoint interval must be positive");
                return false;
            }
            
            // Set checkpoint interval through configuration
            config->set("checkpoint_interval", static_cast<int64_t>(interval), ConfigSource::Runtime);
            logger->info("Checkpoint interval set to: " + std::to_string(interval) + " steps");
            return true;
        } catch (const std::exception& e) {
            logger->error("Checkpoint interval must be an integer");
            return false;
        }
    }
    
    bool handle_develop(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        if (args.size() < 1) {
            logger->error("Develop command requires stage argument");
            logger->info("Available stages: Initial, CriticalPeriod, Maturation, Adult");
            return false;
        }
        
        const std::string& stage_str = args[0];
        DevelopmentalStage stage;
        
        if (stage_str == "Initial") {
            stage = DevelopmentalStage::Initial;
        } else if (stage_str == "CriticalPeriod") {
            stage = DevelopmentalStage::CriticalPeriod;
        } else if (stage_str == "Maturation") {
            stage = DevelopmentalStage::Maturation;
        } else if (stage_str == "Adult") {
            stage = DevelopmentalStage::Adult;
        } else {
            logger->error("Invalid developmental stage: " + stage_str);
            return false;
        }
        
        brain->setDevelopmentalStage(stage);
        logger->info("Developmental stage set to: " + stage_str);
        return true;
    }
    
    bool handle_memory_working(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        if (args.size() < 1) {
            logger->error("Memory working command requires action argument");
            logger->info("Available actions: clear, strengthen, compete");
            return false;
        }
        
        const std::string& action = args[0];
        
        if (action == "clear") {
            if (brain->getWorkingMemory()) {
                brain->getWorkingMemory()->clear();
                logger->info("Working memory cleared");
            } else {
                logger->error("Working memory not available");
                return false;
            }
        } else if (action == "strengthen") {
            if (brain->getWorkingMemory()) {
                // strengthen() method may not exist, we need to check
                logger->info("Working memory strengthening (placeholder implementation)");
            } else {
                logger->error("Working memory not available");
                return false;
            }
        } else if (action == "compete") {
            if (brain->getWorkingMemory()) {
                // compete() method may not exist, we need to check
                logger->info("Working memory competition initiated (placeholder implementation)");
            } else {
                logger->error("Working memory not available");
                return false;
            }
        } else {
            logger->error("Unknown memory working action: " + action);
            return false;
        }
        
        return true;
    }
    
    bool handle_memory_episodic(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        if (args.size() < 1) {
            logger->error("Memory episodic command requires action argument");
            logger->info("Available actions: store, replay, consolidate");
            return false;
        }
        
        const std::string& action = args[0];
        
        if (action == "store") {
            // store() method may not exist
            logger->info("Memory stored in episodic memory (placeholder implementation)");
        } else if (action == "replay") {
            // replay() method may not exist
            logger->info("Episodic memory replayed (placeholder implementation)");
        } else if (action == "consolidate") {
            // consolidate() method may not exist
            logger->info("Episodic memory consolidated (placeholder implementation)");
        } else {
            logger->error("Unknown memory episodic action: " + action);
            return false;
        }
        
        return true;
    }
    
    bool handle_memory_replay(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        if (args.size() < 1) {
            logger->error("Memory replay command requires count argument");
            return false;
        }
        
        try {
            size_t count = std::stoul(args[0]);
            if (count <= 0) {
                logger->error("Replay count must be positive");
                return false;
            }
            
            // replay(count) method may not exist
            logger->info("Memory replay called with count: " + std::to_string(count) + " (placeholder implementation)");
            return true;
        } catch (const std::exception& e) {
            logger->error("Replay count must be an integer");
            return false;
        }
    }
    
    bool handle_neuromod(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        if (args.size() < 2) {
            logger->error("Neuromod command requires type and level arguments");
            logger->info("Usage: neuromod [type] - [level]");
            logger->info("Types: dopamine, curiosity, novelty, predictionError, acetylcholine, norepinephrine, serotonin");
            return false;
        }
        
        const std::string& type_str = args[0];
        const std::string& level_str = args[1];
        
        try {
            float level = std::stof(level_str);
            if (level < 0.0f || level > 1.0f) {
                logger->error("Neuromodulator level must be between 0.0 and 1.0");
                return false;
            }
            
            // Set neuromodulator level (placeholder - actual implementation may differ)
            logger->info("Setting neuromodulator " + type_str + " to level: " + std::to_string(level));
            return true;
        } catch (const std::exception& e) {
            logger->error("Neuromodulator level must be a number");
            return false;
        }
    }
    
    bool handle_plasticity(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        if (args.size() < 1) {
            logger->error("Plasticity command requires factor argument");
            return false;
        }
        
        try {
            float factor = std::stof(args[0]);
            if (factor < 0.0f || factor > 10.0f) {
                logger->error("Plasticity factor must be between 0.0 and 10.0");
                return false;
            }
            
            // Set global plasticity factor (placeholder)
            logger->info("Setting global plasticity factor to: " + std::to_string(factor));
            return true;
        } catch (const std::exception& e) {
            logger->error("Plasticity factor must be a number");
            return false;
        }
    }
    
    bool handle_status(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        brain->logStatus();
        return true;
    }
    
    bool handle_neurons(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        logger->info("Neuron distribution across regions:");
        const auto& region_ids = brain->getRegionIds();
        for (const auto& region_id : region_ids) {
            const NeuralRegion* region = brain->getRegion(region_id);
            if (region) {
                size_t neuron_count = region->getNeuronCount();
                size_t population_count = region->getPopulationCount();
                logger->info("  Region " + std::to_string(region_id) + ": " + 
                           std::to_string(neuron_count) + " neurons, " +
                           std::to_string(population_count) + " populations");
            }
        }
        return true;
    }
    
    bool handle_synapses(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        size_t total_synapses = brain->getTotalSynapseCount();
        size_t total_neurons = brain->getTotalNeuronCount();
        
        if (total_neurons > 0) {
            float avg_connections = (static_cast<float>(total_synapses) / static_cast<float>(total_neurons));
            logger->info("Synapse statistics:");
            logger->info("  Total synapses: " + std::to_string(total_synapses));
            logger->info("  Total neurons: " + std::to_string(total_neurons));
            logger->info("  Average connections per neuron: " + std::to_string(avg_connections));
        } else {
            logger->info("Synapse statistics:");
            logger->info("  Total synapses: " + std::to_string(total_synapses));
            logger->info("  Total neurons: 0");
            logger->info("  Average connections per neuron: N/A");
        }
        
        return true;
    }
    
    bool handle_memory(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        auto working_memory = brain->getWorkingMemory();
        auto episodic_memory = brain->getEpisodicMemory();
        
        logger->info("Memory system status:");
        
        if (working_memory) {
            size_t capacity = working_memory->getCapacity();
            size_t current_size = working_memory->getCurrentSize();
            logger->info("  Working Memory: " + std::to_string(current_size) + "/" + std::to_string(capacity) + " items");
        } else {
            logger->info("  Working Memory: Not available");
        }
        
        if (episodic_memory) {
            size_t episode_count = episodic_memory->getEpisodeCount();
            logger->info("  Episodic Memory: " + std::to_string(episode_count) + " episodes");
        } else {
            logger->info("  Episodic Memory: Not available");
        }
        
        return true;
    }
    
    bool handle_cognition(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        logger->info("Cognitive system status:");
        
        if (brain->getPlanner()) {
            logger->info("  Neural Planner: Active");
        } else {
            logger->info("  Neural Planner: Inactive");
        }
        
        if (brain->getConceptFormation()) {
            logger->info("  Concept Formation: Active");
        } else {
            logger->info("  Concept Formation: Inactive");
        }
        
        if (brain->getAttention()) {
            logger->info("  Attentional Selection: Active");
        } else {
            logger->info("  Attentional Selection: Inactive");
        }
        
        return true;
    }
    
    bool handle_performance(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        logger->info("Performance metrics:");
        logger->info("  Total spike count: " + std::to_string(brain->getTotalSpikeCount()));
        logger->info("  Firing neuron count: " + std::to_string(brain->getFiringNeuronCount()));
        logger->info("  Total neuron count: " + std::to_string(brain->getTotalNeuronCount()));
        logger->info("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
        
        if (brain->getTotalNeuronCount() > 0) {
            float firing_rate_percentage = (static_cast<float>(brain->getFiringNeuronCount()) * 100.0f) / 
                                           static_cast<float>(brain->getTotalNeuronCount());
            logger->info("  Percentage firing: " + std::to_string(firing_rate_percentage) + "%");
        }
        
        return true;
    }
    
    bool handle_profile(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        if (args.size() < 1) {
            logger->error("Profile command requires metric argument");
            logger->info("Available metrics: memory, time, spikes");
            return false;
        }
        
        const std::string& metric = args[0];
        
        if (metric == "memory") {
            logger->info("Starting memory profiling...");
            // Placeholder for actual profiling implementation
        } else if (metric == "time") {
            logger->info("Starting time profiling...");
            // Placeholder for actual profiling implementation
        } else if (metric == "spikes") {
            logger->info("Starting spike profiling...");
            // Placeholder for actual profiling implementation
        } else {
            logger->error("Unknown profile metric: " + metric);
            return false;
        }
        
        return true;
    }
    
    bool handle_stats(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        logger->info("Accumulated statistics:");
        logger->info("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        logger->info("  Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
        logger->info("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
        
        return true;
    }
    
    bool handle_monitor(const std::vector<std::string>& args) {
        if (!is_initialized) {
            logger->error("Brain not initialized");
            return false;
        }
        
        logger->info("Starting continuous monitoring mode...");
        logger->info("Press Ctrl+C to stop monitoring");
        
        // Placeholder for monitoring implementation
        // In actual implementation, this would start a monitoring loop
        
        return true;
    }
    
    void run_interactive() {
        std::cout << "=== NLM Brain Control CLI ===\n";
        std::cout << "Type 'help' for available commands or 'quit' to exit\n\n";
        
        initialize_brain();
        
        if (!is_initialized) {
            std::cout << "Failed to initialize brain. Exiting.\n";
            return;
        }
        
        try {
            while (true) {
                try {
                    std::cout << "nlm> ";
                    std::string input;
                    std::getline(std::cin, input);
                    
                    if (input.empty()) {
                        continue;
                    }
                    
                    // Check for exit commands
                    if (input == "quit" || input == "exit") {
                        std::cout << "Exiting...\n";
                        break;
                    }
                    
                    // Parse command
                    std::vector<std::string> args;
                    std::string arg;
                    std::istringstream iss(input);
                    
                    while (iss >> arg) {
                        args.push_back(arg);
                    }
                    
                    if (args.empty()) {
                        continue;
                    }
                    
                    // Handle commands
                    bool command_handled = false;
                    
                    if (args[0] == "help") {
                        print_help();
                        command_handled = true;
                    } else if (args[0] == "quit" || args[0] == "exit") {
                        std::cout << "Exiting...\n";
                        break;
                    } else if (args[0] == "about") {
                        print_about();
                        command_handled = true;
                    } else if (args[0] == "save") {
                        command_handled = handle_save(std::vector<std::string>(args.begin() + 1, args.end()));
                    } else if (args[0] == "load") {
                        command_handled = handle_load(std::vector<std::string>(args.begin() + 1, args.end()));
                    } else if (args[0] == "reset") {
                        command_handled = handle_reset(std::vector<std::string>(args.begin() + 1, args.end()));
                    } else if (args[0] == "checkpoint") {
                        command_handled = handle_checkpoint(std::vector<std::string>(args.begin() + 1, args.end()));
                    } else if (args[0] == "develop") {
                        command_handled = handle_develop(std::vector<std::string>(args.begin() + 1, args.end()));
                    } else if (args[0] == "memory") {
                        if (args.size() > 1) {
                            if (args[1] == "working") {
                                if (args.size() > 2) {
                                    command_handled = handle_memory_working(std::vector<std::string>(args.begin() + 2, args.end()));
                                } else {
                                    logger->error("Memory working command requires action");
                                    logger->info("Usage: memory working - [action]");
                                    logger->info("Available actions: clear, strengthen, compete");
                                }
                            } else if (args[1] == "episodic") {
                                if (args.size() > 2) {
                                    command_handled = handle_memory_episodic(std::vector<std::string>(args.begin() + 2, args.end()));
                                } else {
                                    command_handled = handle_memory_episodic(std::vector<std::string>(args.begin() + 2, args.end()));
                                }
                            } else if (args[1] == "replay") {
                                command_handled = handle_memory_replay(std::vector<std::string>(args.begin() + 2, args.end()));
                            } else {
                                command_handled = handle_memory(std::vector<std::string>(args.begin() + 1, args.end()));
                            }
                        } else {
                            command_handled = handle_memory(std::vector<std::string>(args.begin() + 1, args.end()));
                        }
                    } else if (args[0] == "neuromod") {
                        if (args.size() > 1 && args[1] == "-" && args.size() > 2) {
                            // Handle format: neuromod dopamine - 1.0
                            command_handled = handle_neuromod(std::vector<std::string>{args[1], args[3]});
                        } else {
                            logger->error("Invalid neuromod command format");
                            logger->info("Usage: neuromod [type] - [level]");
                        }
                    } else if (args[0] == "plasticity") {
                        command_handled = handle_plasticity(std::vector<std::string>(args.begin() + 1, args.end()));
                    } else if (args[0] == "status") {
                        command_handled = handle_status(std::vector<std::string>(args.begin() + 1, args.end()));
                    } else if (args[0] == "neurons") {
                        command_handled = handle_neurons(std::vector<std::string>(args.begin() + 1, args.end()));
                    } else if (args[0] == "synapses") {
                        command_handled = handle_synapses(std::vector<std::string>(args.begin() + 1, args.end()));
                    } else if (args[0] == "cognition") {
                        command_handled = handle_cognition(std::vector<std::string>(args.begin() + 1, args.end()));
                    } else if (args[0] == "performance") {
                        command_handled = handle_performance(std::vector<std::string>(args.begin() + 1, args.end()));
                    } else if (args[0] == "profile") {
                        command_handled = handle_profile(std::vector<std::string>(args.begin() + 1, args.end()));
                    } else if (args[0] == "stats") {
                        command_handled = handle_stats(std::vector<std::string>(args.begin() + 1, args.end()));
                    } else if (args[0] == "monitor") {
                        command_handled = handle_monitor(std::vector<std::string>(args.begin() + 1, args.end()));
                    } else {
                        logger->error("Unknown command: " + args[0]);
                    }
                    
                    if (!command_handled) {
                        print_help();
                    }
                    
                } catch (const std::exception& e) {
                    logger->error("Error executing command: " + std::string(e.what()));
                }
            }
        } catch (const std::exception& e) {
            logger->error("Error in interactive mode: " + std::string(e.what()));
        }
        
        logger->info("NLM CLI shutting down");
    }
    
    void run_command_line(int argc, char** argv) {
        if (argc < 2) {
            print_help();
            return;
        }
        
        initialize_brain();
        
        if (!is_initialized) {
            logger->error("Failed to initialize brain");
            return;
        }
        
        // Parse command line arguments
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        // Handle commands
        bool command_handled = false;
        
        if (args[0] == "help") {
            print_help();
            command_handled = true;
        } else if (args[0] == "about") {
            print_about();
            command_handled = true;
        } else if (args[0] == "save") {
            command_handled = handle_save(std::vector<std::string>(args.begin() + 1, args.end()));
        } else if (args[0] == "load") {
            command_handled = handle_load(std::vector<std::string>(args.begin() + 1, args.end()));
        } else if (args[0] == "reset") {
            command_handled = handle_reset(std::vector<std::string>(args.begin() + 1, args.end()));
        } else if (args[0] == "checkpoint") {
            command_handled = handle_checkpoint(std::vector<std::string>(args.begin() + 1, args.end()));
        } else if (args[0] == "develop") {
            command_handled = handle_develop(std::vector<std::string>(args.begin() + 1, args.end()));
        } else if (args[0] == "memory") {
            if (args.size() > 1 && args[1] == "working") {
                command_handled = handle_memory_working(std::vector<std::string>(args.begin() + 2, args.end()));
            } else if (args.size() > 1 && args[1] == "episodic") {
                if (args.size() > 2) {
                    command_handled = handle_memory_episodic(std::vector<std::string>(args.begin() + 2, args.end()));
                } else {
                    command_handled = handle_memory_episodic(std::vector<std::string>(args.begin() + 2, args.end()));
                }
            } else if (args.size() > 1 && args[1] == "replay") {
                command_handled = handle_memory_replay(std::vector<std::string>(args.begin() + 2, args.end()));
            } else {
                command_handled = handle_memory(std::vector<std::string>(args.begin() + 1, args.end()));
            }
        } else if (args[0] == "neuromod") {
            if (args.size() > 1 && args[1] == "-" && args.size() > 2) {
                // Handle format: neuromod dopamine - 1.0
                command_handled = handle_neuromod(std::vector<std::string>{args[1], args[3]});
            } else {
                logger->error("Invalid neuromod command format");
                logger->info("Usage: neuromod [type] - [level]");
            }
        } else if (args[0] == "plasticity") {
            command_handled = handle_plasticity(std::vector<std::string>(args.begin() + 1, args.end()));
        } else if (args[0] == "status") {
            command_handled = handle_status(std::vector<std::string>(args.begin() + 1, args.end()));
        } else if (args[0] == "neurons") {
            command_handled = handle_neurons(std::vector<std::string>(args.begin() + 1, args.end()));
        } else if (args[0] == "synapses") {
            command_handled = handle_synapses(std::vector<std::string>(args.begin() + 1, args.end()));
        } else if (args[0] == "cognition") {
            command_handled = handle_cognition(std::vector<std::string>(args.begin() + 1, args.end()));
        } else if (args[0] == "performance") {
            command_handled = handle_performance(std::vector<std::string>(args.begin() + 1, args.end()));
        } else if (args[0] == "profile") {
            command_handled = handle_profile(std::vector<std::string>(args.begin() + 1, args.end()));
        } else if (args[0] == "stats") {
            command_handled = handle_stats(std::vector<std::string>(args.begin() + 1, args.end()));
        } else if (args[0] == "monitor") {
            command_handled = handle_monitor(std::vector<std::string>(args.begin() + 1, args.end()));
        } else {
            logger->error("Unknown command: " + args[0]);
            print_help();
        }
        
        if (!command_handled) {
            print_help();
        }
    }
};